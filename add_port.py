with open(r'd:\ai_assistant\platformio.ini', 'r', encoding='utf-8') as f:
    lines = f.readlines()

with open(r'd:\ai_assistant\platformio.ini', 'w', encoding='utf-8') as f:
    for line in lines:
        f.write(line)
        if 'monitor_speed = 115200' in line:
            f.write('upload_port = COM8\n')
