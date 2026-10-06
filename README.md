# AI Assistant — ESP32 + GigaChat + Whisper STT

Умный голосовой ассистент на базе ESP32 с микрофоном (I2S INMP441), 
дисплеем (ST7789 172x320), GigaChat (NLP) и OpenAI Whisper (Speech-to-Text).

## Возможности

- **Голосовой ввод**: нажмите и удерживайте кнопку BOOT, говорите, отпустите
- **Текстовый ввод**: печатайте в Serial Monitor (115200 бод)
- **Speech-to-Text**: распознавание речи через OpenAI Whisper API
- **AI-ответ**: GigaChat от Сбера (русскоязычная модель)
- **Дисплей**: анимации, уровень звука, ответы (User/AI)

## Аппаратное обеспечение

| Компонент | Пин ESP32 |
|-----------|-----------|
| Микрофон INMP441 I2S | |
|   BCK (SCK)          | GPIO 26   |
|   WS (L/R)           | GPIO 25   |
|   SD (DOUT)          | GPIO 33   |
| Дисплей ST7789 SPI  | |
|   MOSI               | GPIO 23   |
|   SCLK               | GPIO 18   |
|   CS                 | GPIO 15   |
|   DC                 | GPIO 2    |
|   RST                | GPIO 4    |
|   BL                 | GPIO 32   |
| Кнопка BOOT          | GPIO 0 (встроенная) |

## Настройка

### 1. Файл `SRC/secrets.h`

Создайте на основе шаблона (уже создан). Заполните:

```cpp
#define WIFI_SSID     "YourWiFiName"
#define WIFI_PASSWORD "YourWiFiPassword"
#define GIGACHAT_AUTH_BASE64 "Base64(client_id:client_secret)"
#define OPENAI_API_KEY "sk-..."
```

Где взять:
- **GigaChat API**: https://developers.sber.ru/portal/products/gigachat
  - Создайте приложение → получите Client ID и Client Secret
  - Закодируйте в Base64: `echo -n "client_id:client_secret" | base64`
- **OpenAI API Key**: https://platform.openai.com/api-keys
  - Создайте ключ с доступом к модели `whisper-1`

### 2. Сборка и прошивка

```bash
# Установите PlatformIO (VS Code расширение или CLI)
pio run --target upload
# Монитор порта
pio device monitor -b 115200
```

## Использование

1. **Голос**: удерживайте BOOT → говорите → отпустите → ждите ответ
2. **Текст**: откройте Serial Monitor → введите текст → Enter
3. **Перезагрузка**: нажмите кнопку на экране ошибки

## Архитектура

```
main.cpp — главный цикл (FSM)
├── audio_recorder — запись с I2S микрофона
├── wav_encoder — упаковка в WAV
├── speech_recognizer — Whisper STT (multipart/form-data)
├── gigachat_client — GigaChat chat/completions
└── display_manager — ST7789 + TFT_eSPI
```

## Зависимости (platformio.ini)

- `bodmer/TFT_eSPI` — дисплей
- `bblanchon/ArduinoJson` — JSON парсинг

## Примечания

- Для INMP441 используйте I2S с пинами SCK=26, WS=25, SD=33
- Дисплей ST7789 172x320 с инициализацией из `build_flags` в platformio.ini
- Аудио буфер выделяется в PSRAM (если доступна), иначе в DRAM
- Максимальная запись: 3 секунды (настраивается в config.h)
- Для работоспособности нужен доступ к api.openai.com и ngw.devices.sberbank.ru:9443