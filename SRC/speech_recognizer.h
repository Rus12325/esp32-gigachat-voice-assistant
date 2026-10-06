#pragma once
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include "config.h"

enum class STTStatus { OK, AUTH_ERROR, NETWORK_ERROR, API_ERROR, PARSE_ERROR, TIMEOUT };

struct STTResult {
    STTStatus status = STTStatus::NETWORK_ERROR;
    String text;
    String error;
    String userText;
};

class SpeechRecognizer {
public:
    SpeechRecognizer();
    void begin();
    STTResult recognize(int16_t* samples, uint32_t numSamples);

private:
    WiFiClientSecure _client;
    bool _connect();
    bool _writeAll(const uint8_t* data, size_t len);
    String _readResponse(int& httpCode);
};