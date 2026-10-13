param(
    [Parameter(Mandatory)] [string]$Screen,
    [Parameter(Mandatory)] [string]$Out,        # absolute drawstream json path
    [string]$Frames = "200:203",
    [switch]$Defaults,                           # MASHED_SETTINGS_DEFAULTS=1
    [int]$TimeoutSec = 90
)
$wt = 'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\scr32arrow'
Set-Location $wt
if (Test-Path $Out) { Remove-Item $Out }
$env:MASHED_GOTO = $Screen
$env:MASHED_DBG_DRAWSTREAM = $Frames
$env:MASHED_DBG_DRAWSTREAM_OUT = $Out
$env:MASHED_WIN_POS = 'primary-bl'
$env:MASHED_DBG_MEASURE = '1'
if ($Defaults) { $env:MASHED_SETTINGS_DEFAULTS = '1' } else { $env:MASHED_SETTINGS_DEFAULTS = $null }
$p = Start-Process -FilePath "$wt\mashedmod\build\mashed_re.exe" -WorkingDirectory $wt -PassThru
"spawned PID $($p.Id)"
$t0 = Get-Date
while (-not (Test-Path $Out) -and ((Get-Date) - $t0).TotalSeconds -lt $TimeoutSec -and -not $p.HasExited) { Start-Sleep -Milliseconds 500 }
# let the writer finish
$last = -1
for ($i = 0; $i -lt 20 -and (Test-Path $Out); $i++) {
    $len = (Get-Item $Out).Length
    if ($len -eq $last -and $len -gt 0) { break }
    $last = $len; Start-Sleep -Milliseconds 500
}
if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force; "killed PID $($p.Id)" } else { "PID $($p.Id) exited $($p.ExitCode)" }
if (Test-Path $Out) { "out: $Out ($((Get-Item $Out).Length) bytes)" } else { "NO OUTPUT" }
Select-String -Path "$wt\mashed_re.log" -Pattern '^(settings|measure):' | % { $_.Line }

