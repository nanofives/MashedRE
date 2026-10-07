# Leg E2 driver (AMEND_E2.md A5). Six sequential runs under the F2 determinism knobs:
# three knob-OFF (E2off_1..3, the fresh reference A1 requires) and three knob-ON.
#
# Same shape as verify/d3_determinism_20261007/run_f2.ps1: waits for the DET_FRAMES
# self-exit rather than a wall-clock kill, and stops only the PID it spawned.
$ErrorActionPreference = 'Stop'
$root = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$out  = Join-Path $root 'verify\d3_consumer_20261007'
$exe  = Join-Path $root 'mashedmod\build\mashed_re.exe'
Set-Location $root

$FRAMES     = 14400
$WAIT_LIMIT = 900

$runs = @()
foreach ($i in 1..3) { $runs += @{ name = "E2off_$i"; arc = $false } }
foreach ($i in 1..3) { $runs += @{ name = "E2on_$i";  arc = $true  } }

foreach ($r in $runs) {
  $log = Join-Path $root 'mashed_re.log'
  if (Test-Path $log) { Remove-Item $log -Force }
  $csv = Join-Path $out "$($r.name).csv"
  if (Test-Path $csv) { Remove-Item $csv -Force }

  $env:MASHED_MUTE          = '1'
  $env:MASHED_TRACK_VIEW    = 'Training'
  $env:MASHED_CAR           = '1'
  $env:MASHED_ROUND         = '1'
  $env:MASHED_WIN_POS       = 'primary-bl'
  $env:MASHED_ROUND_RULE    = '4'
  $env:MASHED_DETERMINISTIC = '1'
  $env:MASHED_DET_FRAMES    = "$FRAMES"
  $env:MASHED_AI_STEPDUMP   = $csv
  Remove-Item Env:\MASHED_RACEMETRIC_ARC -ErrorAction SilentlyContinue
  if ($r.arc) { $env:MASHED_RACEMETRIC_ARC = '1' }

  Write-Host "=== $($r.name)  arc=$($r.arc) ==="
  $p = Start-Process -FilePath $exe -WorkingDirectory $root -PassThru
  Write-Host "    pid=$($p.Id)"
  $sw = [Diagnostics.Stopwatch]::StartNew()
  $exited = $p.WaitForExit($WAIT_LIMIT * 1000)
  if (-not $exited) {
    Write-Host "    TIMEOUT after $WAIT_LIMIT s - stopping pid $($p.Id) ONLY"
    Stop-Process -Id $p.Id -Force
    $p.WaitForExit(15000) | Out-Null
  }
  Write-Host "    exit=$($p.ExitCode)  wall=$([int]$sw.Elapsed.TotalSeconds)s  self_exited=$exited"

  if (Test-Path $log) { Copy-Item $log (Join-Path $out "$($r.name).log") -Force }
  else { Write-Host "    NO mashed_re.log for $($r.name)" }
  if (Test-Path $csv) {
    Write-Host ("    csv rows (incl header) = " + (Get-Content $csv | Measure-Object -Line).Lines)
  } else { Write-Host "    NO csv for $($r.name)" }
}
Write-Host 'E2 DONE'
