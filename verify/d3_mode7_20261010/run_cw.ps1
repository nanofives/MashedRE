# PcScope restore check: MASHED_LOG_CW under three arms. MEASUREMENT ONLY; spawns and stops ONLY its own PIDs.
$root = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$out  = Join-Path $root 'verify\d3_mode7_20261010'
$exe  = Join-Path $root 'mashedmod\build\mashed_re.exe'
Set-Location $root
foreach ($arm in @(@{n='cw_ctrl'; k=@()}, @{n='cw_reg'; k=@('MASHED_REGISTRY')}, @{n='cw_pp'; k=@('MASHED_PATHPROG')})) {
  foreach ($k in 'MASHED_REGISTRY','MASHED_PATHPROG') { Remove-Item "Env:\$k" -ErrorAction SilentlyContinue }
  $log = Join-Path $root 'mashed_re.log'; if (Test-Path $log) { Remove-Item $log -Force }
  $env:MASHED_MUTE = '1'; $env:MASHED_TRACK_VIEW = 'Training'; $env:MASHED_CAR = '1'
  $env:MASHED_ROUND = '1'; $env:MASHED_ROUND_RULE = '4'; $env:MASHED_WIN_POS = 'primary-bl'
  $env:MASHED_DETERMINISTIC = '1'; $env:MASHED_DET_FRAMES = '1900'; $env:MASHED_LOG_CW = '1'
  foreach ($k in $arm.k) { Set-Item "Env:\$k" '1' }
  $p = Start-Process -FilePath $exe -WorkingDirectory $root -PassThru
  $ok = $p.WaitForExit(600000)
  if (-not $ok) { Stop-Process -Id $p.Id -Force; $p.WaitForExit(15000) | Out-Null }
  Write-Host ("{0}: pid={1} exit={2}" -f $arm.n, $p.Id, $p.ExitCode)
  if (Test-Path $log) { Select-String -Path $log -Pattern '\[cw\]' | ForEach-Object { '   ' + $_.Line } }
}
foreach ($k in 'MASHED_REGISTRY','MASHED_PATHPROG','MASHED_LOG_CW') { Remove-Item "Env:\$k" -ErrorAction SilentlyContinue }
