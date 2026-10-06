#include "display_manager.h"
#include "config.h"
#include <math.h>

void DisplayManager::begin() {
    _tft.init();
    _tft.setRotation(0);
    _tft.fillScreen(BG_COLOR);

    ledcSetup(0, 5000, 8);
    ledcAttachPin(TFT_BL_PIN, 0);
    setBrightness(200);

    _sprite.setColorDepth(16);
}

void DisplayManager::setBrightness(uint8_t level) {
    ledcWrite(0, level);
}

void DisplayManager::_drawHeader(const String& title, uint16_t color) {
    _tft.fillRect(0, 0, _tft.width(), 36, color);
    _tft.setTextColor(TEXT_PRIMARY);
    _tft.setTextDatum(MC_DATUM);
    _tft.drawString(title, _tft.width() / 2, 18, 4);
}

void DisplayManager::_drawWrappedText(const String& text, int x, int y, int maxWidth,
                                      uint16_t color, uint8_t font) {
    _tft.setTextColor(color, BG_COLOR);
    _tft.setTextDatum(TL_DATUM);
    _tft.setTextWrap(false);

    // Используем FreeFont для поддержки Unicode/кириллицы
    _tft.setFreeFont(&FreeSans9pt7b);
    
    int curY = y;
    int lineHeight = 20;
    String remaining = text;

    while (remaining.length() > 0 && curY < _tft.height() - 20) {
        int charsFit = 0;
        int lastSpace = -1;

        for (int i = 0; i < (int)remaining.length(); i++) {
            String sub = remaining.substring(0, i + 1);
            int w = _tft.textWidth(sub, font);
            if (w > maxWidth) break;
            charsFit = i + 1;
            if (remaining[i] == ' ') lastSpace = i;
        }

        if (charsFit == 0) charsFit = 1;

        int splitAt = charsFit;
        if (charsFit < (int)remaining.length() && lastSpace > 0) {
            splitAt = lastSpace + 1;
        }

        String line = remaining.substring(0, splitAt);
        line.trim();
        _tft.drawString(line, x, curY, font);

        remaining = remaining.substring(splitAt);
        remaining.trim();
        curY += lineHeight;
    }
}

void DisplayManager::_drawMicIcon(int x, int y, uint16_t color, float level) {
    _tft.fillRoundRect(x - 8, y - 20, 16, 28, 8, color);
    _tft.drawCircle(x, y + 8, 14, color);
    _tft.drawLine(x, y + 22, x, y + 32, color);
    _tft.drawLine(x - 6, y + 32, x + 6, y + 32, color);

    if (level > 0.01f) {
        uint8_t rings = min(3, (int)(level * 30));
        for (int i = 0; i < rings; i++) {
            uint16_t ringColor = (i < 2) ? ACCENT : WARNING;
            _tft.drawCircle(x, y + 4, 20 + i * 10, ringColor);
        }
    }
}

void DisplayManager::_drawProgressBar(int x, int y, int w, int h, float progress,
                                      uint16_t color) {
    _tft.drawRoundRect(x, y, w, h, h / 2, TEXT_SECONDARY);
    int fillW = (int)(progress * (w - 4));
    if (fillW > 0) {
        _tft.fillRoundRect(x + 2, y + 2, fillW, h - 4, (h - 4) / 2, color);
    }
}

void DisplayManager::showSplash(float progress) {
    if (_currentScreen == UIScreen::SPLASH && progress > 0) {
        _drawProgressBar(20, 260, _tft.width() - 40, 10, progress, PRIMARY);
        return;
    }

    _currentScreen = UIScreen::SPLASH;
    _tft.fillScreen(BG_COLOR);

    _tft.setTextDatum(MC_DATUM);

    _tft.setTextColor(PRIMARY, BG_COLOR);
    _tft.drawString("AI", _tft.width() / 2, 95, 4);

    _tft.setTextColor(TEXT_PRIMARY, BG_COLOR);
    _tft.drawString("Assistant", _tft.width() / 2, 145, 4);

    _tft.setTextColor(TEXT_SECONDARY, BG_COLOR);
    _tft.drawString("Powered by GigaChat", _tft.width() / 2, 190, 2);

    _drawProgressBar(20, 260, _tft.width() - 40, 10, progress, PRIMARY);
}


void DisplayManager::showIdle() {
    _currentScreen = UIScreen::IDLE;
    _tft.fillScreen(BG_COLOR);
    _drawHeader("AI Assistant", PRIMARY);
    _drawMicIcon(_tft.width() / 2, 140, TEXT_SECONDARY);
    _tft.setTextColor(TEXT_PRIMARY, BG_COLOR);
    _tft.setTextDatum(MC_DATUM);
    _tft.drawString("Hold button", _tft.width() / 2, 210, 2);
    _tft.drawString("and speak", _tft.width() / 2, 230, 2);
    _tft.setTextColor(ACCENT, BG_COLOR);
    _tft.drawString("GigaChat", _tft.width() / 2, 300, 2);
}

void DisplayManager::showRecording(float level, float seconds) {
    if (_currentScreen != UIScreen::RECORDING) {
        _currentScreen = UIScreen::RECORDING;
        _tft.fillScreen(BG_COLOR);
        _drawHeader("Recording", ERROR_COLOR);
    }

    _drawMicIcon(_tft.width() / 2, 120, ERROR_COLOR, level);

    _tft.setTextColor(TEXT_PRIMARY, BG_COLOR);
    _tft.setTextDatum(MC_DATUM);

    char timeStr[20];
    snprintf(timeStr, sizeof(timeStr), "%.1f sec", seconds);
    _tft.drawString(timeStr, _tft.width() / 2, 210, 4);

    float progress = seconds / MAX_RECORD_SEC;
    _drawProgressBar(20, 250, _tft.width() - 40, 12, progress, ERROR_COLOR);

    _tft.setTextColor(TEXT_SECONDARY, BG_COLOR);
    _tft.drawString("Release to send", _tft.width() / 2, 290, 2);
}

void DisplayManager::showProcessing(const String& stage) {
    if (_currentScreen != UIScreen::PROCESSING) {
        _currentScreen = UIScreen::PROCESSING;
        _tft.fillScreen(BG_COLOR);
        _drawHeader("Processing", WARNING);
    }

    static uint8_t frame = 0;
    frame = (frame + 1) % 8;

    int cx = _tft.width() / 2;
    int cy = 130;

    _tft.fillRect(cx - 40, cy - 40, 80, 80, BG_COLOR);

    for (int i = 0; i < 8; i++) {
        float angle = (i * 45) * DEG_TO_RAD;
        int x = cx + cos(angle) * 25;
        int y = cy + sin(angle) * 25;
        uint16_t dotColor = (i == frame) ? WARNING : TEXT_SECONDARY;
        _tft.fillCircle(x, y, 4, dotColor);
    }

    _tft.fillRect(0, 200, _tft.width(), 30, BG_COLOR);
    _tft.setTextColor(TEXT_PRIMARY, BG_COLOR);
    _tft.setTextDatum(MC_DATUM);
    _tft.drawString(stage, _tft.width() / 2, 210, 2);
}

void DisplayManager::showResponse(const String& userText, const String& aiResponse) {
    _currentScreen = UIScreen::RESPONSE;
    _tft.fillScreen(BG_COLOR);

    _drawHeader("Response", ACCENT);

    int y = 44;
    int padding = 8;
    int maxW = _tft.width() - 20;

    _tft.setTextColor(TEXT_SECONDARY, BG_COLOR);
    _tft.setTextDatum(TL_DATUM);
    _tft.drawString("You:", 10, y, 2);
    y += 18;

    int userH = 60;
    _tft.fillRoundRect(6, y - 2, _tft.width() - 12, userH, 6, USER_BUBBLE);
    // Показываем текст латиницей или транслитом (кириллица не поддерживается шрифтами TFT_eSPI)
    _drawWrappedText(userText, 10 + padding, y + padding, maxW - padding * 2, TEXT_PRIMARY, 2);
    y += userH + 10;

    _tft.setTextColor(TEXT_SECONDARY, BG_COLOR);
    _tft.drawString("AI:", 10, y, 2);
    y += 18;

    int aiH = _tft.height() - y - 24;
    if (aiH < 40) aiH = 40;
    _tft.fillRoundRect(6, y - 2, _tft.width() - 12, aiH, 6, AI_BUBBLE);
    _drawWrappedText(aiResponse, 10 + padding, y + padding, maxW - padding * 2, TEXT_PRIMARY, 2);
}

void DisplayManager::showError(const String& error) {
    _currentScreen = UIScreen::ERROR;
    _tft.fillScreen(BG_COLOR);

    _drawHeader("Error", ERROR_COLOR);

    _tft.setTextColor(ERROR_COLOR, BG_COLOR);
    _tft.setTextDatum(MC_DATUM);
    _tft.drawString("X", _tft.width() / 2, 100, 4);

    _drawWrappedText(error, 15, 150, _tft.width() - 30, TEXT_PRIMARY, 2);

    _tft.setTextColor(TEXT_SECONDARY, BG_COLOR);
    _tft.setTextDatum(MC_DATUM);
    _tft.drawString("Press button", _tft.width() / 2, 290, 2);
}