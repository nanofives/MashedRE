# GATEFIRE mode-5 reset — gates for FUN_00418560 Branch A.
# Four runs: M5off / M5on / M5on_r2 on the gates dump, plus M5step for the
# default-build control (must still hash 86b7b2bb). Only MASHED_MODE5_RESET differs.
$ErrorActionPreference = 'Stop'
$root = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$out  = Join-Path $root 'verify\d3_gatefire_20261008'
$exe  = Join-Path $root 'mashedmod\build\mashed_re.exe'
Set-Location $root
$FRAMES = 14400

$runs = @(
  @{ name = 'M5off';   on = $false; step = $false },
  @{ name = 'M5on';    on = $true;  step = $false },
  @{ name = 'M5on_r2'; on = $true;  step = $false },
  @{ name = 'M5step';  on = $false; step = $true  }
)

foreach ($r in $runs) {
  $log = Join-Path $root 'mashed_re.log'
  if (Test-Path $log) { Remove-Item $log -Force }

  $env:MASHED_MUTE = '1'; $env:MASHED_TRACK_VIEW = 'Training'
  $env:MASHED_CAR = '1';  $env:MASHED_ROUND = '1'
  $env:MASHED_WIN_POS = 'primary-bl'; $env:MASHED_ROUND_RULE = '4'
  $env:MASHED_DETERMINISTIC = '1'; $env:MASHED_DET_FRAMES = "$FRAMES"
  foreach ($k in 'MASHED_REFDIST','MASHED_SLOTSTATE_SEED','MASHED_RACEPCT_BRIDGE',
                 'MASHED_U9186_GATES','MASHED_AI_STEPDUMP','MASHED_A364_RESET',
                 'MASHED_MODE5_RESET') {
    Remove-Item "Env:\$k" -ErrorAction SilentlyContinue
  }

  if ($r.step) {
    $art = Join-Path $out "$($r.name).csv"
    $env:MASHED_AI_STEPDUMP = $art
  } else {
    $art = Join-Path $out "$($r.name).gates.csv"
    $env:MASHED_SLOTSTATE_SEED = '1'; $env:MASHED_RACEPCT_BRIDGE = '1'
    $env:MASHED_U9186_GATES = $art
    if ($r.on) { $env:MASHED_MODE5_RESET = '1' }
  }
  if (Test-Path $art) { Remove-Item $art -Force }

  Write-Host "=== $($r.name)  step=$($r.step) mode5=$($env:MASHED_MODE5_RESET) ==="
  $p = Start-Process -FilePath $exe -WorkingDirectory $root -PassThru
  Write-Host "    pid=$($p.Id)"
  $ok = $p.WaitForExit(900000)
  if (-not $ok) { Write-Host "    TIMEOUT - stopping pid $($p.Id) ONLY"; Stop-Process -Id $p.Id -Force }
  Write-Host "    exit=$($p.ExitCode)  self_exited=$ok"
  if (Test-Path $art) { Write-Host ("    rows = " + (Get-Content $art | Measure-Object -Line).Lines) }
  else { Write-Host "    NO artifact" }
}
Write-Host 'M5 DONE'
