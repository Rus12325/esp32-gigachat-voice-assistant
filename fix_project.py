import re

def fix_main():
    path = r'd:\ai_assistant\SRC\main.cpp'
    with open(path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Исправляем логику записи
    content = re.sub(r'(\s*)if\s*\(!buttonPressed\s*\|\|\s*elapsed\s*>=\s*MAX_RECORD_SEC\)\s*\{', 
                     r'\1if (buttonReleased || elapsed >= MAX_RECORD_SEC) {', content)
    
    with open(path, 'w', encoding='utf-8') as f:
        f.write(content)
    print("Fixed main.cpp")

def fix_display():
    path = r'd:\ai_assistant\SRC\display_manager.cpp'
    with open(path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Оптимизируем showRecording, чтобы убрать мерцание
    # Заменяем полную очистку области на более точечную или убираем её
    # В данном случае, просто закомментируем или заменим fillRect на что-то менее агрессивное
    content = content.replace('_tft.fillRect(0, 40, _tft.width(), 160, BG_COLOR);', 
                             '// _tft.fillRect(0, 40, _tft.width(), 160, BG_COLOR); // Reduced flicker')
    
    with open(path, 'w', encoding='utf-8') as f:
        f.write(content)
    print("Fixed display_manager.cpp")

if __name__ == "__main__":
    fix_main()
    fix_display()
