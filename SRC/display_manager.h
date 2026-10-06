#pragma once
#include <TFT_eSPI.h>

enum class UIScreen {
    SPLASH,
    IDLE,
    RECORDING,
    PROCESSING,
    RESPONSE,
    ERROR
};

class DisplayManager {
public:
    DisplayManager() : _sprite(&_tft) {}

    void begin();

    void showSplash(float progress = 0.5f);
    void showIdle();
    void showRecording(float level = 0, float seconds = 0);
    void showProcessing(const String& stage = "Processing...");
    void showResponse(const String& userText, const String& aiResponse);
    void showError(const String& error);

    void setBrightness(uint8_t level);

private:
    TFT_eSPI _tft;
    TFT_eSprite _sprite;
    UIScreen _currentScreen = UIScreen::SPLASH;

    void _drawHeader(const String& title, uint16_t color);
    void _drawWrappedText(const String& text, int x, int y, int maxWidth,
                          uint16_t color, uint8_t font = 2);
    void _drawMicIcon(int x, int y, uint16_t color, float level = 0);
    void _drawProgressBar(int x, int y, int w, int h, float progress, uint16_t color);

    static const uint16_t BG_COLOR       = 0x0000;
    static const uint16_t PRIMARY        = 0x1E9F;
    static const uint16_t ACCENT         = 0x07E0;
    static const uint16_t WARNING        = 0xFDA0;
    static const uint16_t ERROR_COLOR    = 0xF800;
    static const uint16_t TEXT_PRIMARY   = 0xFFFF;
    static const uint16_t TEXT_SECONDARY = 0x7BEF;
    static const uint16_t USER_BUBBLE    = 0x1153;
    static const uint16_t AI_BUBBLE      = 0x18C3;
};