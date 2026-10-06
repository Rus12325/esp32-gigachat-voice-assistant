#include "speech_recognizer.h"
#include <ArduinoJson.h>
#include "secrets.h"

// Заглушка — STT временно отключён
// Для голосового ввода замените на SaluteSpeech/Yandex STT

SpeechRecognizer::SpeechRecognizer() {}

void SpeechRecognizer::begin() {
    Serial.println("=== SpeechRecognizer: DISABLED ===");
    _client.setInsecure();
    _client.setTimeout(10);
}

bool SpeechRecognizer::_connect() {
    return false;
}

bool SpeechRecognizer::_writeAll(const uint8_t* data, size_t len) {
    return false;
}

String SpeechRecognizer::_readResponse(int& httpCode) {
    httpCode = -1;
    return "";
}

STTResult SpeechRecognizer::recognize(int16_t* samples, uint32_t numSamples) {
    STTResult r;
    r.status = STTStatus::API_ERROR;
    r.error = "STT disabled";
    r.userText = "[audio placeholder]";
    return r;
}