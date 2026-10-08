# H4 driver: PREREG_H4.md section 1. Five arms on the 18:44 build:
#   A / A2 = default (determinism floor), B = SLOT_PLAYER,
#   C = full branch-2 arm, D = C minus SLOT_PLAYER.
# Every arm: PLAYERTRACE (car 0) + U9186_GATES (reader; GF1 unset) + AI_STEPDUMP (cars 1..3).
#
# MEASUREMENT ONLY. Spawns and stops ONLY its own PIDs.
$ErrorActionPreference = 'Stop'
$root = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$out  = Join-Path $root 'verify\d3_gatefire_20261008'
$exe  = Join-Path $root 'mashedmod\build\mashed_re.exe'
Set-Location $root

$FRAMES     = 14400
$WAIT_LIMIT = 900

$LANE = @('MASHED_REFDIST','MASHED_SLOTSTATE_SEED','MASHED_SLOT_PLAYER',
          'MASHED_A364_RESET','MASHED_RACEPCT_BRIDGE','MASHED_NO_ELIM',
          'MASHED_WIRE_B2','MASHED_U9186_GATES','MASHED_AI_STEPDUMP',
          'MASHED_NO_SLOTSTATE_SEED','MASHED_NO_SLOT_PLAYER','MASHED_GF1',
          'MASHED_PLAYERTRACE')

$B2 = @('MASHED_A364_RESET','MASHED_REFDIST','MASHED_RACEPCT_BRIDGE','MASHED_NO_ELIM','MASHED_WIRE_B2')
$runs = @(
  @{ name = 'H4_A';  knobs = @() },
  @{ name = 'H4_A2'; knobs = @() },
  @{ name = 'H4_B';  knobs = @('MASHED_SLOT_PLAYER') },
  @{ name = 'H4_C';  knobs = @('MASHED_SLOT_PLAYER') + $B2 },
  @{ name = 'H4_D';  knobs = $B2 },
  # post-flip (18:56 build, SLOT_PLAYER default-ON): must equal B and A respectively
  @{ name = 'H4_post';   knobs = @() },
  @{ name = 'H4_optout'; knobs = @('MASHED_NO_SLOT_PLAYER') }
)
$sel = @($args)   # capture: inside a Where-Object block $args is the block's own
if ($sel.Count -gt 0) { $runs = $runs | Where-Object { $sel -contains $_.name } }
else { $runs = $runs | Where-Object { $_.name -notlike 'H4_post*' -and $_.name -ne 'H4_optout' } }

foreach ($r in $runs) {
  $log = Join-Path $root 'mashed_re.log'
  $pt  = Join-Path $root 'player_trace.log'
  foreach ($f in @($log, $pt)) { if (Test-Path $f) { Remove-Item $f -Force } }
  foreach ($k in $LANE) { Remove-Item "Env:\$k" -ErrorAction SilentlyContinue }

  $env:MASHED_MUTE          = '1'
  $env:MASHED_TRACK_VIEW    = 'Training'
  $env:MASHED_CAR           = '1'
  $env:MASHED_ROUND         = '1'
  $env:MASHED_ROUND_RULE    = '4'
  $env:MASHED_WIN_POS       = 'primary-bl'
  $env:MASHED_DETERMINISTIC = '1'
  $env:MASHED_DET_FRAMES    = "$FRAMES"
  $env:MASHED_PLAYERTRACE   = '1'

  $step  = Join-Path $out "$($r.name).step.csv"
  $gates = Join-Path $out "$($r.name).gates.csv"
  foreach ($f in @($step, $gates)) { if (Test-Path $f) { Remove-Item $f -Force } }
  $env:MASHED_AI_STEPDUMP = $step
  $env:MASHED_U9186_GATES = $gates
  foreach ($k in $r.knobs) { Set-Item "Env:\$k" '1' }

  Write-Host "=== $($r.name)  knobs=[$($r.knobs -join ',')] ==="
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
  if (Test-Path $pt)  { Copy-Item $pt  (Join-Path $out "$($r.name).ptrace.log") -Force }
  foreach ($f in @($step, $gates, (Join-Path $out "$($r.name).ptrace.log"))) {
    if (Test-Path $f) {
      Write-Host ("    {0}  lines={1}  sha8={2}" -f (Split-Path $f -Leaf),
        (Get-Content $f | Measure-Object -Line).Lines,
        (Get-FileHash $f -Algorithm SHA256).Hash.Substring(0,8).ToLower())
    } else { Write-Host "    MISSING $(Split-Path $f -Leaf)" }
  }
}
Write-Host 'H4 DONE'
