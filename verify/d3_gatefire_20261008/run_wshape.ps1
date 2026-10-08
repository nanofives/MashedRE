# W-SHAPE port side driver — PREREG_WSHAPE.md section 4.
# Two runs: WS_ctl (default build, every lane knob off, the GF0step recipe) and
# WS_fire (the FULL prerequisite stack + NO_ELIM + WIRE_B2, so branch 2 can fire).
#
# MEASUREMENT ONLY. Nothing is seeded. Spawns and stops ONLY its own PIDs
# (never blanket-kill MASHED/mashed_re by name — other sessions may be capturing).
$ErrorActionPreference = 'Stop'
$root = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$out  = Join-Path $root 'verify\d3_gatefire_20261008'
$exe  = Join-Path $root 'mashedmod\build\mashed_re.exe'
Set-Location $root

$FRAMES     = 14400
$WAIT_LIMIT = 900

$LANE = @('MASHED_REFDIST','MASHED_SLOTSTATE_SEED','MASHED_SLOT_PLAYER',
          'MASHED_A364_RESET','MASHED_RACEPCT_BRIDGE','MASHED_NO_ELIM',
          'MASHED_WIRE_B2','MASHED_U9186_GATES','MASHED_AI_STEPDUMP')

$runs = @(
  @{ name = 'WS_ctl';  fire = $false },
  @{ name = 'WS_fire'; fire = $true  }
)

foreach ($r in $runs) {
  $log = Join-Path $root 'mashed_re.log'
  if (Test-Path $log) { Remove-Item $log -Force }

  # Clear every lane knob first, then set only what this arm needs.
  foreach ($k in $LANE) { Remove-Item "Env:\$k" -ErrorAction SilentlyContinue }

  $env:MASHED_MUTE          = '1'
  $env:MASHED_TRACK_VIEW    = 'Training'
  $env:MASHED_CAR           = '1'
  $env:MASHED_ROUND         = '1'
  $env:MASHED_ROUND_RULE    = '4'
  $env:MASHED_WIN_POS       = 'primary-bl'
  $env:MASHED_DETERMINISTIC = '1'
  $env:MASHED_DET_FRAMES    = "$FRAMES"

  $art = Join-Path $out "$($r.name).step.csv"
  $env:MASHED_AI_STEPDUMP = $art

  if ($r.fire) {
    # FULL prerequisite stack. An under-specified arm produced a false "the wiring
    # is inert" earlier this session — WS-ARMED checks the arm actually took.
    $env:MASHED_SLOTSTATE_SEED = '1'
    $env:MASHED_SLOT_PLAYER    = '1'
    $env:MASHED_A364_RESET     = '1'
    $env:MASHED_REFDIST        = '1'
    $env:MASHED_RACEPCT_BRIDGE = '1'
    $env:MASHED_NO_ELIM        = '1'
    $env:MASHED_WIRE_B2        = '1'
  }
  if (Test-Path $art) { Remove-Item $art -Force }

  Write-Host "=== $($r.name)  fire=$($r.fire) ==="
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
    Write-Host ("    sha8 = " + (Get-FileHash $art -Algorithm SHA256).Hash.Substring(0,8).ToLower())
  } else { Write-Host "    NO artifact for $($r.name)" }
}
Write-Host 'WSHAPE DONE'
