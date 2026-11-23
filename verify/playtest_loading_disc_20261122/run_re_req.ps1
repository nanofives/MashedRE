param([string]$Dir, [string]$OutDir, [int]$RunS = 40, [double]$Every = 0.2, [string]$Tag = "re")
$req = "$OutDir\rereq.txt"
Remove-Item $req -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force $OutDir | Out-Null
$env:MASHED_DBG_BBDUMP_REQ = $req
$env:MASHED_SETTINGS_DEFAULTS = "1"
$env:MASHED_INTRO_PRESS = "3"
Remove-Item "$Dir\mashed_re.log" -ErrorAction SilentlyContinue
$p = Start-Process -FilePath "$Dir\mashed_re.exe" -WorkingDirectory $Dir -PassThru
Write-Output "spawned pid $($p.Id)"
$t0 = Get-Date
$n = 0
$started = $false
while (-not $p.HasExited -and ((Get-Date) - $t0).TotalSeconds -lt $RunS) {
    if (-not $started) {
        # start requesting once the frontend assets are loaded (loadicon line in the log)
        if ((Test-Path "$Dir\mashed_re.log") -and (Select-String -Path "$Dir\mashed_re.log" -Pattern "loadicon:" -Quiet)) { $started = $true; $ts = Get-Date }
    }
    if ($started -and -not (Test-Path $req) -and ((Get-Date) - $ts).TotalSeconds -lt 12) {
        $tgt = "$OutDir\${Tag}_{0:D3}.bmp" -f $n
        Set-Content -Path $req -Value $tgt -NoNewline
        $n++
    }
    Start-Sleep -Milliseconds ([int]($Every * 1000))
}
if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force; Write-Output "stopped pid $($p.Id)" } else { Write-Output "exited $($p.ExitCode)" }
Write-Output "requests $n"
Select-String -Path "$Dir\mashed_re.log" -Pattern "loadicon:" | ForEach-Object { $_.Line }
