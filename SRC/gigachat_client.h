#pragma once
#include <Arduino.h>
#include <WiFiClientSecure.h>

enum class GigaStatus { OK, AUTH_ERROR, NETWORK_ERROR, API_ERROR, PARSE_ERROR, TIMEOUT };

struct GigaResponse {
    GigaStatus status = GigaStatus::NETWORK_ERROR;
    String text;
    String userText;
    String error;
};

class GigaChatClient {
public:
    void begin();
    bool connectWiFi();
    GigaResponse chat(const String& userMessage);
    GigaResponse processAudio(int16_t* samples, uint32_t numSamples); // fallback stub

private:
    WiFiClientSecure _client;

    bool _connect();
    bool _writeAll(const uint8_t* data, size_t len);
    bool _writeAll(const String& s);
    String _readResponse(int& httpCode);
};