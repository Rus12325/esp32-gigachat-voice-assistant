@echo off
powershell -NoProfile -Command "Get-CimInstance Win32_SerialPort | ForEach-Object { $_.Name; $_.DeviceID; $_.Status }"
echo.
echo Searching for process using COM8...
powershell -NoProfile -Command "Get-CimInstance Win32_SerialPort | Where-Object { $_.DeviceID -eq 'COM8' } | Select-Object Name,DeviceID"
echo.
echo If still blocked, unplug and replug ESP32 USB, then run:
echo pio run --target upload
pause