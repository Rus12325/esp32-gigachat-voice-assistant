lines = open(r'D:\ai_assistant\SRC\main.cpp', encoding='utf-8').readlines()  
lines[25] = 'bool buttonReleased = false;\n'  
lines.insert(26, 'bool lastButtonState = HIGH;\n')  
lines.insert(27, 'unsigned long buttonDebounce = 0;\n')  
lines.insert(28, '\n')  
open(r'D:\ai_assistant\SRC\main.cpp', 'w', encoding='utf-8').writelines(lines)  
