# GATEFIRE idx364 — gates for the FUN_00414060 step-6 partial port.
# Four runs: A364off / A364on / A364on_r2 on the gates dump, plus A364step on the
# AI stepdump for A364-KNOBOFF (the default build must still hash to 86b7b2bb).
# MEASUREMENT arms only differ by MASHED_A364_RESET. Spawns and stops ONLY its own PIDs.
$ErrorActionPreference = 'Stop'
$root = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$out  = Join-Path $root 'verify\d3_gatefire_20261008'
$exe  = Join-Path $root 'mashedmod\build\mashed_re.exe'
Set-Location $root
$FRAMES = 14400

$runs = @(
  @{ name = 'A364off';    on = $false; step = $false },
  @{ name = 'A364on';     on = $true;  step = $false },
  @{ name = 'A364on_r2';  on = $true;  step = $false },
  @{ name = 'A364step';   on = $false; step = $true  }
)

foreach ($r in $runs) {
  $log = Join-Path $root 'mashed_re.log'
  if (Test-Path $log) { Remove-Item $log -Force }

  $env:MASHED_MUTE = '1'; $env:MASHED_TRACK_VIEW = 'Training'
  $env:MASHED_CAR = '1';  $env:MASHED_ROUND = '1'
  $env:MASHED_WIN_POS = 'primary-bl'; $env:MASHED_ROUND_RULE = '4'
  $env:MASHED_DETERMINISTIC = '1'; $env:MASHED_DET_FRAMES = "$FRAMES"
  foreach ($k in 'MASHED_REFDIST','MASHED_SLOTSTATE_SEED','MASHED_RACEPCT_BRIDGE',
                 'MASHED_U9186_GATES','MASHED_AI_STEPDUMP','MASHED_A364_RESET') {
    Remove-Item "Env:\$k" -ErrorAction SilentlyContinue
  }

  if ($r.step) {
    $art = Join-Path $out "$($r.name).csv"
    $env:MASHED_AI_STEPDUMP = $art
  } else {
    $art = Join-Path $out "$($r.name).gates.csv"
    $env:MASHED_SLOTSTATE_SEED = '1'; $env:MASHED_RACEPCT_BRIDGE = '1'
    $env:MASHED_U9186_GATES = $art
    if ($r.on) { $env:MASHED_A364_RESET = '1' }
  }
  if (Test-Path $art) { Remove-Item $art -Force }

  Write-Host "=== $($r.name)  step=$($r.step) a364reset=$($env:MASHED_A364_RESET) ==="
  $p = Start-Process -FilePath $exe -WorkingDirectory $root -PassThru
  Write-Host "    pid=$($p.Id)"
  $ok = $p.WaitForExit(900000)
  if (-not $ok) { Write-Host "    TIMEOUT - stopping pid $($p.Id) ONLY"; Stop-Process -Id $p.Id -Force }
  Write-Host "    exit=$($p.ExitCode)  self_exited=$ok"
  if (Test-Path $art) {
    Write-Host ("    rows = " + (Get-Content $art | Measure-Object -Line).Lines)
  } else { Write-Host "    NO artifact" }
}
Write-Host 'A364 DONE'
