$comPort = [System.IO.Ports.SerialPort]::GetPortNames() | Where-Object { $_ -eq "COM8" }
if ($comPort) {
    Write-Host "COM8 detected - attempting to find holding process..."
    $proc = Get-CimInstance Win32_SerialPort | Where-Object { $_.DeviceID -eq "COM8" } | Select-Object -First 1
    if ($proc) {
        Write-Host ("Device: " + $proc.Name)
    }
} else {
    Write-Host "COM8 not found in port list - device may be disconnected"
}
Write-Host "`nTry unplugging and replugging ESP32 USB, then run:"
Write-Host "  pio run --target upload"