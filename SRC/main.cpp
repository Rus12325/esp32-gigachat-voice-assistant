#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "gigachat_client.h"
#include "display_manager.h"

GigaChatClient gigachat;
DisplayManager display;

enum AppState { INIT, IDLE, PROCESSING, SHOWING_RESPONSE, ERROR };
AppState state = INIT;

String serialBuf = "";

// === Кнопка BOOT ===
int btnLast = HIGH;
unsigned long btnDeb = 0;
bool btnPressed = false;

void readButton() {
    int val = digitalRead(BUTTON_PIN);
    if (val != btnLast) btnDeb = millis();
    btnLast = val;
    if (millis() - btnDeb > 50 && val == LOW) {
        btnPressed = true;
    }
}

void sendToGiga(const String& txt) {
    if (txt.isEmpty()) return;
    display.showProcessing("OpenRouter...");
    GigaResponse r = gigachat.chat(txt);
    if (r.status == GigaStatus::OK) {
        Serial.println("User: " + r.userText);
        Serial.println("AI:   " + r.text);
        display.showResponse(r.userText, r.text);
        state = SHOWING_RESPONSE;
    } else {
        Serial.println("AI err: " + r.error);
        display.showError(r.error);
        state = ERROR;
    }
}

void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println();
    Serial.println("=== AI Assistant (OpenRouter) ===");
    Serial.printf("Free heap: %u\n", ESP.getFreeHeap());

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    delay(200);

    Serial.println("Display...");
    display.begin();
    display.showSplash(0.0f);
    delay(500);

    Serial.println("WiFi...");
    display.showSplash(0.3f);
    if (!gigachat.connectWiFi()) {
        display.showError("WiFi FAIL");
        state = ERROR; return;
    }

    Serial.println("OpenRouter AI...");
    display.showSplash(0.6f);
    gigachat.begin();

    state = IDLE;
    display.showIdle();
    Serial.println("=== READY ===");
    Serial.println("Type text in Serial Monitor and press Enter.");
    Serial.println("Press BOOT button to say 'Привет!' to AI.");
    Serial.println();
}

void loop() {
    readButton();

    // Serial text input
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
            if (serialBuf.length() > 0) { sendToGiga(serialBuf); serialBuf = ""; }
        } else { serialBuf += c; }
    }

    // Кнопка BOOT → отправляем "Привет!" в AI
    if (btnPressed && state != PROCESSING) {
        btnPressed = false;
        if (state == ERROR) {
            delay(300);
            ESP.restart();
        } else {
            sendToGiga("Привет!");
        }
    }

    delay(10);
}