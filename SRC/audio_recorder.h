#pragma once
#include <Arduino.h>
#include <driver/i2s.h>
#include "config.h"

class AudioRecorder {
public:
    bool begin();
    bool startRecording();
    bool stopRecording();
    bool isRecording() const { return _recording; }
    
    int16_t* getBuffer() { return _buffer; }
    uint32_t getSamplesRecorded() const { return _samplesRecorded; }
    float getRecordingLevel() const { return _level; }
    
    void updateRecording(); // вызывать в loop

private:
    int16_t* _buffer = nullptr;
    uint32_t _samplesRecorded = 0;
    bool _recording = false;
    float _level = 0;
    
    bool _i2sInit();
    void _i2sDeinit();
};