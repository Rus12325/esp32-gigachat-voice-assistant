lines = open(r'D:\ai_assistant\SRC\main.cpp', encoding='utf-8').readlines()
lines = open(r'D:\ai_assistant\SRC\main.cpp', encoding='utf-8').readlines()


for i, line in enumerate(lines):
    if 'bool buttonPressed = false;' in line:
        lines.insert(i+1, 'bool buttonReleased = false;\n')
        lines.insert(i+2, 'bool lastButtonState = HIGH;\n')
        lines.insert(i+3, 'unsigned long buttonDebounce = 0;\n')
        lines.insert(i+4, '\n')
        for j in range(i+5, len(lines)):
                del lines[j]
                break
        break


for i, line in enumerate(lines):
    if 'void readButton() {' in line:
        j = i+1
        while j < len(lines) and lines[j].strip() != '}':
            j += 1
        new_body = [
            '    }\n',
        ]
        lines[i+1:j] = new_body
        break


for i, line in enumerate(lines):
    if 'if (buttonPressed) {' in line and i > 100:
        j = i+1
        while j < len(lines) and lines[j].strip() != '}':
            j += 1
        for k in range(i, j):
            if 'buttonPressed = false;' in lines[k]:
                lines.insert(k+1, '                buttonReleased = false;\n')
                break
        break


for i, line in enumerate(lines):
    if 'if (!buttonPressed || elapsed >= MAX_RECORD_SEC) {' in line:
        j = i+1
        while j < len(lines) and lines[j].strip() != '}':
            j += 1
        for k in range(i, j):
            if 'recorder.stopRecording();' in lines[k]:
                lines.insert(k+1, '                buttonReleased = false;\n')
                break
        break

open(r'D:\ai_assistant\SRC\main.cpp', 'w', encoding='utf-8').writelines(lines)
