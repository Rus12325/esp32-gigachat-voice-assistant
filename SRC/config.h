#pragma once
#include "secrets.h"

// ===== GigaChat =====
#define SYSTEM_PROMPT "You are a friendly AI assistant. Answer briefly and clearly in 2-3 sentences. Reply in the same language as the user."

// ===== Пины =====
#define BUTTON_PIN    0   // GPIO0 = кнопка BOOT на ESP32 (INPUT_PULLUP)
#define I2S_SCK_PIN   26
#define I2S_WS_PIN    25
#define I2S_SD_PIN    33
#define TFT_BL_PIN    32

// ===== Аудио =====
#define SAMPLE_RATE       16000
#define BITS_PER_SAMPLE   16
#define CHANNELS          1
#define MAX_RECORD_SEC    3
#define AUDIO_BUFFER_SIZE (SAMPLE_RATE * MAX_RECORD_SEC * 2)
