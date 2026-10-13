param(
    [Parameter(Mandatory)] [string]$Screen,
    [Parameter(Mandatory)] [ValidateSet('pre','post')] [string]$Build,  # mashedmod\build\mashed_re_<Build>.exe
    [Parameter(Mandatory)] [string]$Out,        # absolute drawstream json path
    [string]$Frames = "200:203",
    [int]$TimeoutSec = 90,
    [string]$Bmp = ""
)
# Recheck runner. Always MASHED_SETTINGS_DEFAULTS=1. Spawns ONE standalone, kills only that PID.
$wt = 'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\scr32texty'
Set-Location $wt
if (Test-Path $Out) { Remove-Item $Out }
$env:MASHED_GOTO = $Screen
$env:MASHED_DBG_DRAWSTREAM = $Frames
$env:MASHED_DBG_DRAWSTREAM_OUT = $Out
$env:MASHED_WIN_POS = 'primary-bl'
$env:MASHED_DBG_MEASURE = '1'
$env:MASHED_SETTINGS_DEFAULTS = '1'
if ($Bmp) { $env:MASHED_DBG_BBDUMP = "1"; $env:MASHED_DBG_BBDUMP_OUT = $Bmp } else { $env:MASHED_DBG_BBDUMP = $null; $env:MASHED_DBG_BBDUMP_OUT = $null }
$p = Start-Process -FilePath "$wt\mashedmod\build\mashed_re_$Build.exe" -WorkingDirectory $wt -PassThru
"spawned PID $($p.Id) build $Build screen $Screen"
$t0 = Get-Date
while (-not (Test-Path $Out) -and ((Get-Date) - $t0).TotalSeconds -lt $TimeoutSec -and -not $p.HasExited) { Start-Sleep -Milliseconds 500 }
$last = -1
for ($i = 0; $i -lt 20 -and (Test-Path $Out); $i++) {
    $len = (Get-Item $Out).Length
    if ($len -eq $last -and $len -gt 0) { break }
    $last = $len; Start-Sleep -Milliseconds 500
}
if ($Bmp) { for ($i = 0; $i -lt 20 -and -not (Test-Path $Bmp); $i++) { Start-Sleep -Milliseconds 500 } }
if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force; "killed PID $($p.Id)" } else { "PID $($p.Id) exited $($p.ExitCode)" }
if (Test-Path $Out) { "out: $Out ($((Get-Item $Out).Length) bytes)" } else { "NO OUTPUT" }
Select-String -Path "$wt\mashed_re.log" -Pattern '^settings:' | % { $_.Line }
