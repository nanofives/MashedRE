# (b) ring-phase sweep: PREREG_BSCORE.md. Default build (no MASHED_SUBSTATE), the run_h4 recipe,
# MASHED_RING_SKIP = K for each K given (default 0..11). Only the AI step dump is kept.
# MEASUREMENT ONLY. Spawns and stops ONLY its own PIDs.
$ErrorActionPreference = 'Stop'
$root = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$out  = Join-Path $root 'verify\d3_bscore_20261009'
$exe  = Join-Path $root 'mashedmod\build\mashed_re.exe'
Set-Location $root
$FRAMES = 1200
$ks = @($args); if ($ks.Count -eq 0) { $ks = 0..11 }
foreach ($k in $ks) {
  foreach ($v in @('MASHED_SUBSTATE','MASHED_MODE5_RESET','MASHED_U9186_GATES','MASHED_PLAYERTRACE','MASHED_RING_SKIP')) { Remove-Item "Env:\$v" -ErrorAction SilentlyContinue }
  $env:MASHED_MUTE = '1'; $env:MASHED_TRACK_VIEW = 'Training'; $env:MASHED_CAR = '1'; $env:MASHED_ROUND = '1'
  $env:MASHED_ROUND_RULE = '4'; $env:MASHED_WIN_POS = 'primary-bl'; $env:MASHED_DETERMINISTIC = '1'
  $env:MASHED_DET_FRAMES = "$FRAMES"; $env:MASHED_RING_SKIP = "$k"
  $step = Join-Path $out ("K{0:D2}.step.csv" -f [int]$k)
  if (Test-Path $step) { Remove-Item $step -Force }
  $env:MASHED_AI_STEPDUMP = $step
  $p = Start-Process -FilePath $exe -WorkingDirectory $root -PassThru
  $ok = $p.WaitForExit(600000)
  if (-not $ok) { Write-Host "K=$k TIMEOUT - stopping pid $($p.Id) ONLY"; Stop-Process -Id $p.Id -Force }
  Write-Host ("K={0} pid={1} exit={2} lines={3} sha8={4}" -f $k, $p.Id, $p.ExitCode,
    (Get-Content $step | Measure-Object -Line).Lines, (Get-FileHash $step -Algorithm SHA256).Hash.Substring(0,8).ToLower())
}
