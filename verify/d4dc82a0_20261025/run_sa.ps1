param(
    [Parameter(Mandatory)] [string]$Exe,        # mashed_re.exe to run
    [Parameter(Mandatory)] [string]$Screen,
    [Parameter(Mandatory)] [string]$Out,        # absolute drawstream json path
    [string]$Frames = "200:203",
    [string]$Log,                               # copy the run's mashed_re.log here
    [int]$TimeoutSec = 90
)
# verify/scr19_parity_20261012/arrow_fix/run_sa.ps1 with the exe as a parameter, the private root
# D:\MashedWT\D4-DC-82a0_root as MASHED_ROOT (the exe chdirs there, so mashed_re.log lands there),
# MASHED_SETTINGS_DEFAULTS=1 always, and MASHED_DBG_MEASURE=1 (the measure-table log lines).
$root = 'D:\MashedWT\D4-DC-82a0_root'
if (Test-Path $Out) { Remove-Item $Out }
$rlog = Join-Path $root 'mashed_re.log'
if (Test-Path $rlog) { Remove-Item $rlog }
$env:MASHED_ROOT = $root
$env:MASHED_GOTO = $Screen
$env:MASHED_DBG_DRAWSTREAM = $Frames
$env:MASHED_DBG_DRAWSTREAM_OUT = $Out
$env:MASHED_DBG_MEASURE = '1'
$env:MASHED_WIN_POS = 'primary-bl'
$env:MASHED_SETTINGS_DEFAULTS = '1'
$p = Start-Process -FilePath $Exe -WorkingDirectory $root -PassThru
"spawned PID $($p.Id)"
$t0 = Get-Date
while (-not (Test-Path $Out) -and ((Get-Date) - $t0).TotalSeconds -lt $TimeoutSec -and -not $p.HasExited) { Start-Sleep -Milliseconds 500 }
$last = -1
for ($i = 0; $i -lt 20 -and (Test-Path $Out); $i++) {
    $len = (Get-Item $Out).Length
    if ($len -eq $last -and $len -gt 0) { break }
    $last = $len; Start-Sleep -Milliseconds 500
}
if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force; "killed PID $($p.Id)" } else { "PID $($p.Id) exited $($p.ExitCode)" }
if (Test-Path $Out) { "out: $Out ($((Get-Item $Out).Length) bytes)" } else { "NO OUTPUT" }
Select-String -Path $rlog -Pattern '^settings:' | Select-Object -Last 1 | % { $_.Line }
Select-String -Path $rlog -Pattern '^measure:' | % { $_.Line }
if ($Log) { Copy-Item $rlog $Log }
