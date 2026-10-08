# H2 driver — PREREG_H2.md section 2. Two runs on the post-flip build:
#   H2_def    = the NEW default (seed on, no env set at all)
#   H2_optout = MASHED_NO_SLOTSTATE_SEED, which must restore the OLD default exactly
# Both use the WS_ctl recipe so they are directly comparable to WS_ctl.step.csv.
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
          'MASHED_NO_SLOTSTATE_SEED')

$runs = @(
  @{ name = 'H2_def';    optout = $false },
  @{ name = 'H2_optout'; optout = $true  }
)

foreach ($r in $runs) {
  $log = Join-Path $root 'mashed_re.log'
  if (Test-Path $log) { Remove-Item $log -Force }
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
  if ($r.optout) { $env:MASHED_NO_SLOTSTATE_SEED = '1' }
  if (Test-Path $art) { Remove-Item $art -Force }

  Write-Host "=== $($r.name)  optout=$($r.optout) ==="
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
    Write-Host ("    rows = " + (Get-Content $art | Measure-Object -Line).Lines)
    Write-Host ("    sha8 = " + (Get-FileHash $art -Algorithm SHA256).Hash.Substring(0,8).ToLower())
  } else { Write-Host "    NO artifact for $($r.name)" }
}
Write-Host 'H2 DONE'
