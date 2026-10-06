#include "gigachat_client.h"
#include <ArduinoJson.h>
#include "config.h"
#include "secrets.h"

#define API_HOST "openrouter.ai"
#define API_PORT 443
#define API_CHAT_PATH "/api/v1/chat/completions"

void GigaChatClient::begin() {
    Serial.println("=== OpenRouter AI Client INIT ===");
    _client.setInsecure();
    _client.setTimeout(120);
    Serial.println("OpenRouter Client ready");
}

bool GigaChatClient::connectWiFi() {
    Serial.print("Connecting to WiFi");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int a = 0;
    while (WiFi.status() != WL_CONNECTED && a < 40) { delay(500); Serial.print("."); a++; }
    Serial.println();
    if (WiFi.status() == WL_CONNECTED) { Serial.print("WiFi OK, IP: "); Serial.println(WiFi.localIP()); return true; }
    Serial.println("WiFi FAILED");
    return false;
}

bool GigaChatClient::_connect() {
    if (_client.connected()) { _client.stop(); delay(50); }
    for (int a = 1; a <= 3; a++) {
        Serial.printf("Connect %s:%u (%d/3)...\n", API_HOST, API_PORT, a);
        if (_client.connect(API_HOST, API_PORT)) { Serial.println("Connected"); delay(200); return true; }
        if (a < 3) delay(1000);
    }
    return false;
}

bool GigaChatClient::_writeAll(const uint8_t* data, size_t len) {
    size_t sent = 0;
    unsigned long tmo = millis() + 60000;
    while (sent < len) {
        size_t ch = (len - sent < 512) ? len - sent : 512;
        int w = _client.write(data + sent, ch);
        if (w < 0) return false;
        if (w == 0) { if (millis() > tmo) return false; delay(5); continue; }
        sent += w; tmo = millis() + 60000; delay(2);
    }
    _client.flush();
    return true;
}

bool GigaChatClient::_writeAll(const String& s) { return _writeAll((const uint8_t*)s.c_str(), s.length()); }

String GigaChatClient::_readResponse(int& httpCode) {
    httpCode = -1;
    unsigned long tmo = millis() + 30000;
    while (!_client.available()) { if (millis() > tmo) return ""; delay(10); }

    String sl = _client.readStringUntil('\n'); sl.trim();
    int fs = sl.indexOf(' ');
    if (fs > 0) {
        int ss = sl.indexOf(' ', fs + 1);
        String cs = (ss > 0) ? sl.substring(fs + 1, ss) : sl.substring(fs + 1);
        httpCode = cs.toInt();
    }

    bool chunked = false; int cl = -1;
    while (true) {
        String ln = _client.readStringUntil('\n'); ln.trim();
        if (ln.length() == 0) break;
        String ll = ln; ll.toLowerCase();
        if (ll.startsWith("content-length:")) cl = ln.substring(ln.indexOf(':') + 1).toInt();
        if (ll.startsWith("transfer-encoding:") && ll.indexOf("chunked") >= 0) chunked = true;
    }

    String body;
    if (cl > 0) body.reserve(cl + 16); else body.reserve(4096);

    if (chunked) {
        while (true) {
            String sz = _client.readStringUntil('\n'); sz.trim();
            if (sz.length() == 0) continue;
            int csz = (int)strtol(sz.c_str(), NULL, 16);
            if (csz <= 0) break;
            for (int i = 0; i < csz; i++) {
                while (!_client.available()) delay(5);
                int c = _client.read(); if (c < 0) break;
                body += (char)c;
            }
            _client.readStringUntil('\n');
        }
    } else if (cl > 0) {
        for (int i = 0; i < cl; i++) {
            while (!_client.available()) delay(5);
            int c = _client.read(); if (c < 0) break;
            body += (char)c;
        }
    } else {
        unsigned long t = millis() + 5000;
        while (millis() < t) {
            while (_client.available()) { body += (char)_client.read(); t = millis() + 5000; }
            delay(5);
        }
    }
    return body;
}

GigaResponse GigaChatClient::chat(const String& userMessage) {
    GigaResponse resp;
    if (WiFi.status() != WL_CONNECTED) { resp.status = GigaStatus::NETWORK_ERROR; resp.error = "No WiFi"; return resp; }
    if (!_connect()) { resp.status = GigaStatus::NETWORK_ERROR; resp.error = "Conn failed"; return resp; }

    String body;
    {
        JsonDocument doc;
        doc["model"] = "openai/gpt-4o-mini";
        JsonArray msgs = doc["messages"].to<JsonArray>();
        JsonObject sys = msgs.add<JsonObject>();
        sys["role"] = "system";
        sys["content"] = SYSTEM_PROMPT;
        JsonObject usr = msgs.add<JsonObject>();
        usr["role"] = "user";
        usr["content"] = userMessage;
        doc["temperature"] = 0.7;
        doc["max_tokens"] = 512;
        serializeJson(doc, body);
    }

    String req = "POST " + String(API_CHAT_PATH) + " HTTP/1.1\r\n";
    req += "Host: " + String(API_HOST) + "\r\n";
    req += "Authorization: Bearer " + String(OPENROUTER_API_KEY) + "\r\n";
    req += "Content-Type: application/json\r\n";
    req += "Content-Length: " + String(body.length()) + "\r\n";
    req += "Connection: close\r\n\r\n";

    if (!_writeAll(req)) { _client.stop(); resp.status = GigaStatus::NETWORK_ERROR; resp.error = "Send fail"; return resp; }
    if (!_writeAll(body)) { _client.stop(); resp.status = GigaStatus::NETWORK_ERROR; resp.error = "Send body fail"; return resp; }
    _client.flush();

    int httpCode = -1;
    String response = _readResponse(httpCode);
    _client.stop();
    if (response.isEmpty()) { resp.status = GigaStatus::TIMEOUT; resp.error = "No response"; return resp; }

    JsonDocument rd;
    if (deserializeJson(rd, response)) { resp.status = GigaStatus::PARSE_ERROR; resp.error = "JSON err"; return resp; }
    if (httpCode < 200 || httpCode >= 300) {
        String errMsg = rd["error"]["message"].as<String>();
        if (errMsg.isEmpty()) errMsg = "HTTP " + String(httpCode);
        resp.status = GigaStatus::API_ERROR; resp.error = errMsg; return resp;
    }

    JsonArray choices = rd["choices"].as<JsonArray>();
    if (!choices.isNull() && choices.size() > 0) {
        JsonObject msg = choices[0]["message"];
        if (!msg.isNull()) { resp.text = msg["content"].as<String>(); resp.userText = userMessage; resp.status = GigaStatus::OK; return resp; }
    }

    resp.status = GigaStatus::PARSE_ERROR;
    resp.error = "No text in response";
    Serial.println("Chat raw: " + response);
    return resp;
}

GigaResponse GigaChatClient::processAudio(int16_t* samples, uint32_t numSamples) {
    GigaResponse resp;
    resp.status = GigaStatus::OK;
    resp.userText = "[audio captured]";
    resp.text = "Audio processed via OpenRouter. Use voice button.";
    return resp;
}