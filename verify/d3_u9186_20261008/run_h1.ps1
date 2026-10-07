# U-9186 leg H1a driver (PREREG_H1.md section 2). Four runs:
#   H1base/H1seed/H1both - the gates probe, same three arms as G1, to score
#                          H1-CONVERGE / H1-GATE2 / H1-ALL.
#   H1step               - a DEFAULT build AI stepdump (no seed, no bridge, no
#                          gates probe) for H1-KNOBOFF against E2off_1.csv and
#                          for H1-NOREG-E / -B.
# Waits for the DET_FRAMES self-exit; stops only the PID it spawned.
$ErrorActionPreference = 'Stop'
$root = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$out  = Join-Path $root 'verify\d3_u9186_20261008'
$exe  = Join-Path $root 'mashedmod\build\mashed_re.exe'
Set-Location $root

$FRAMES     = 14400
$WAIT_LIMIT = 900

$runs = @(
  @{ name = 'H1base'; gates = $true;  extra = @{} },
  @{ name = 'H1seed'; gates = $true;  extra = @{ MASHED_SLOTSTATE_SEED = '1' } },
  @{ name = 'H1both'; gates = $true;  extra = @{ MASHED_SLOTSTATE_SEED = '1'; MASHED_RACEPCT_BRIDGE = '1' } },
  @{ name = 'H1step'; gates = $false; extra = @{} }
)

foreach ($r in $runs) {
  $log = Join-Path $root 'mashed_re.log'
  if (Test-Path $log) { Remove-Item $log -Force }

  $env:MASHED_MUTE          = '1'
  $env:MASHED_TRACK_VIEW    = 'Training'
  $env:MASHED_CAR           = '1'
  $env:MASHED_ROUND         = '1'
  $env:MASHED_WIN_POS       = 'primary-bl'
  $env:MASHED_ROUND_RULE    = '4'
  $env:MASHED_DETERMINISTIC = '1'
  $env:MASHED_DET_FRAMES    = "$FRAMES"
  Remove-Item Env:\MASHED_SLOTSTATE_SEED -ErrorAction SilentlyContinue
  Remove-Item Env:\MASHED_RACEPCT_BRIDGE -ErrorAction SilentlyContinue
  Remove-Item Env:\MASHED_U9186_GATES    -ErrorAction SilentlyContinue
  Remove-Item Env:\MASHED_AI_STEPDUMP    -ErrorAction SilentlyContinue

  if ($r.gates) {
    $art = Join-Path $out "$($r.name).gates.csv"
    $env:MASHED_U9186_GATES = $art
  } else {
    # H1-KNOBOFF compares against E2off_1.csv, which was taken with the stepdump
    # and NOT the gates probe. Same instrumentation on both sides or the
    # comparison is not like-for-like.
    $art = Join-Path $out "$($r.name).csv"
    $env:MASHED_AI_STEPDUMP = $art
  }
  if (Test-Path $art) { Remove-Item $art -Force }
  foreach ($k in $r.extra.Keys) { Set-Item "Env:\$k" $r.extra[$k] }

  Write-Host "=== $($r.name)  gates=$($r.gates) seed=$($env:MASHED_SLOTSTATE_SEED) bridge=$($env:MASHED_RACEPCT_BRIDGE) ==="
  $p = Start-Process -FilePath $exe -WorkingDirectory $root -PassThru
  Write-Host "    pid=$($p.Id)"
  $exited = $p.WaitForExit($WAIT_LIMIT * 1000)
  if (-not $exited) {
    Write-Host "    TIMEOUT - stopping pid $($p.Id) ONLY"
    Stop-Process -Id $p.Id -Force
    $p.WaitForExit(15000) | Out-Null
  }
  Write-Host "    exit=$($p.ExitCode)  self_exited=$exited"
  if (Test-Path $log) { Copy-Item $log (Join-Path $out "$($r.name).log") -Force }
  if (Test-Path $art) {
    Write-Host ("    rows (incl header) = " + (Get-Content $art | Measure-Object -Line).Lines)
  } else { Write-Host "    NO artifact for $($r.name)" }
}
Write-Host 'H1 DONE'
