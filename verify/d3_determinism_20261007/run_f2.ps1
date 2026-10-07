# Leg F2 driver. Four sequential runs of the standalone under MASHED_DETERMINISTIC.
#
# Unlike run_e1.ps1 this does NOT use sa_capture.py's wall-clock kill: under the
# synthetic clock a wall-clock kill lands at a different synthetic instant each run
# (exe_main.cpp:395-400). MASHED_DET_FRAMES ends the run at a fixed frame index and
# this driver waits for that exit (memory race-capture-wait-for-exit).
#
# MASHED process hygiene: the PID spawned here is tracked and only that PID is ever
# stopped, and only on the bounded-wait timeout. No kill-by-name.
$ErrorActionPreference = 'Stop'
$root = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$out  = Join-Path $root 'verify\d3_determinism_20261007'
$exe  = Join-Path $root 'mashedmod\build\mashed_re.exe'
Set-Location $root

$FRAMES     = 14400
$WAIT_LIMIT = 900   # seconds; generous - 14400 frames at ~57 fps is ~250 s

$runs = @(
  @{ name = 'F2a'; extra = @{} },
  @{ name = 'F2b'; extra = @{} },
  @{ name = 'F2c'; extra = @{} },
  @{ name = 'F2x'; extra = @{ MASHED_SIM_HZ = '59' } }   # the control
)

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
  Remove-Item Env:\MASHED_SIM_HZ -ErrorAction SilentlyContinue
  foreach ($k in $r.extra.Keys) { Set-Item "Env:\$k" $r.extra[$k] }

  $sim = if ($env:MASHED_SIM_HZ) { $env:MASHED_SIM_HZ } else { '(default 60)' }
  Write-Host "=== $($r.name)  DET_FRAMES=$FRAMES  SIM_HZ=$sim ==="

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
    $n = (Get-Content $csv | Measure-Object -Line).Lines
    Write-Host "    csv rows (incl header) = $n"
  } else { Write-Host "    NO csv for $($r.name)" }
}
Write-Host 'F2 DONE'
