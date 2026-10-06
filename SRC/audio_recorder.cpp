#include "audio_recorder.h"

bool AudioRecorder::begin() {
    Serial.printf("Free heap before audio alloc: %u\n", ESP.getFreeHeap());
    Serial.printf("PSRAM found: %s\n", psramFound() ? "YES" : "NO");
    Serial.printf("Need audio buffer: %u bytes\n", AUDIO_BUFFER_SIZE);

    // Используем PSRAM, если доступна
    if (psramFound()) {
        _buffer = (int16_t*)ps_malloc(AUDIO_BUFFER_SIZE);
        Serial.println("Allocating audio buffer in PSRAM");
    } else {
        _buffer = (int16_t*)malloc(AUDIO_BUFFER_SIZE);
        Serial.println("Allocating audio buffer in DRAM (no PSRAM)");
    }

    if (!_buffer) {
        Serial.println("Audio buffer allocation FAILED");
        return false;
    }
    Serial.println("Audio buffer allocation OK");

    // Устанавливаем I2S один раз при инициализации
    if (!_i2sInit()) {
        Serial.println("I2S init FAILED");
        return false;
    }
    Serial.println("I2S init OK");

    return true;
}

bool AudioRecorder::_i2sInit() {
    i2s_config_t config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pins = {
        .bck_io_num   = I2S_SCK_PIN,
        .ws_io_num    = I2S_WS_PIN,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num  = I2S_SD_PIN
    };

    esp_err_t err = i2s_driver_install(I2S_NUM_0, &config, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("i2s_driver_install error: %d\n", err);
        return false;
    }

    err = i2s_set_pin(I2S_NUM_0, &pins);
    if (err != ESP_OK) {
        Serial.printf("i2s_set_pin error: %d\n", err);
        return false;
    }

    return true;
}

void AudioRecorder::_i2sDeinit() {
    i2s_driver_uninstall(I2S_NUM_0);
}

bool AudioRecorder::startRecording() {
    if (_recording) return false;

    _samplesRecorded = 0;
    memset(_buffer, 0, AUDIO_BUFFER_SIZE);

    // Очищаем DMA буферы перед стартом
    i2s_zero_dma_buffer(I2S_NUM_0);

    _recording = true;
    Serial.println("Recording started");
    return true;
}

bool AudioRecorder::stopRecording() {
    if (!_recording) return false;

    _recording = false;
    // I2S не деинициализируем — установлен один раз в begin()

    Serial.printf("Recording stopped. Samples: %u (%.2f sec)\n",
                  _samplesRecorded,
                  (float)_samplesRecorded / SAMPLE_RATE);
    return true;
}

void AudioRecorder::updateRecording() {
    if (!_recording) return;

    uint32_t maxSamples = AUDIO_BUFFER_SIZE / sizeof(int16_t);
    if (_samplesRecorded >= maxSamples) {
        stopRecording();
        return;
    }

    size_t bytesRead = 0;
    uint32_t remaining = (maxSamples - _samplesRecorded) * sizeof(int16_t);
    uint32_t toRead = min((uint32_t)2048, remaining);

    esp_err_t err = i2s_read(I2S_NUM_0, _buffer + _samplesRecorded, toRead, &bytesRead, 10);
    if (err != ESP_OK) {
        Serial.printf("I2S read error: %d\n", err);
        return;
    }

    if (bytesRead > 0) {
        uint32_t newSamples = bytesRead / sizeof(int16_t);

        int64_t sum = 0;
        for (uint32_t i = 0; i < newSamples; i++) {
            sum += abs(_buffer[_samplesRecorded + i]);
        }

        if (newSamples > 0) {
            _level = (float)sum / newSamples / 32768.0f;
        } else {
            _level = 0;
        }

        _samplesRecorded += newSamples;
    }
}