# Kill any process holding COM8
$port = "COM8"
Write-Host "Looking for processes holding $port..."

# Try to find what's using the port
$result = netstat -ano | findstr "$port"
if ($result) {
    Write-Host "Netstat found:"
    Write-Host $result
}

# Force kill common serial monitor processes
$processesToKill = @("putty", "screen", "minicom", "arduino", "platformio", "terminal", "code", "mobaxterm", "tera term", "serial", "cutecom")
foreach ($proc in $processesToKill) {
    $p = Get-Process -Name $proc -ErrorAction SilentlyContinue
    if ($p) {
        Write-Host "Killing $proc (PID: $($p.Id))..."
        $p | Stop-Process -Force
    }
}

# Try to release COM port using mode command
Write-Host "Attempting to release $port..."
mode $port dtr=off 2>$null
mode $port rts=off 2>$null

Write-Host "Done. Try uploading now."