#pragma once
#include <cstdint>
#include <cstring>

struct WAVHeader {
    char     riff[4]        = {'R','I','F','F'};
    uint32_t fileSize;
    char     wave[4]        = {'W','A','V','E'};
    char     fmt[4]         = {'f','m','t',' '};
    uint32_t fmtSize        = 16;
    uint16_t audioFormat    = 1; // PCM
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char     data[4]        = {'d','a','t','a'};
    uint32_t dataSize;
};

class WAVEncoder {
public:
    static void createHeader(WAVHeader& header, uint32_t dataSize, 
                             uint32_t sampleRate = 16000, 
                             uint16_t bitsPerSample = 16,
                             uint16_t channels = 1) {
        header.numChannels  = channels;
        header.sampleRate   = sampleRate;
        header.bitsPerSample = bitsPerSample;
        header.blockAlign   = channels * bitsPerSample / 8;
        header.byteRate     = sampleRate * header.blockAlign;
        header.dataSize     = dataSize;
        header.fileSize     = 36 + dataSize;
    }

    static uint8_t* encode(int16_t* samples, uint32_t numSamples, uint32_t& outSize,
                           uint32_t sampleRate = 16000) {
        uint32_t dataSize = numSamples * sizeof(int16_t);
        outSize = sizeof(WAVHeader) + dataSize;
        
        uint8_t* buffer = (uint8_t*)malloc(outSize);
        if (!buffer) return nullptr;

        WAVHeader header;
        createHeader(header, dataSize, sampleRate);
        
        memcpy(buffer, &header, sizeof(WAVHeader));
        memcpy(buffer + sizeof(WAVHeader), samples, dataSize);
        
        return buffer;
    }
};