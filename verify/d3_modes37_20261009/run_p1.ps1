# D-11072 P1 (PREREG_PATHPROG.md G-P1): standalone course-block dump on Training and Arctic.
# MEASUREMENT ONLY. Spawns and stops ONLY its own PIDs.
$root = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$out  = Join-Path $root 'verify\d3_modes37_20261009'
$exe  = Join-Path $root 'mashedmod\build\mashed_re.exe'
Set-Location $root
foreach ($t in @('Training','Arctic')) {
  $dump = Join-Path $out "p_course_$t.bin"
  if (Test-Path $dump) { Remove-Item $dump -Force }
  $env:MASHED_MUTE = '1'; $env:MASHED_TRACK_VIEW = $t; $env:MASHED_CAR = '1'
  $env:MASHED_ROUND = '1'; $env:MASHED_ROUND_RULE = '4'; $env:MASHED_WIN_POS = 'primary-bl'
  $env:MASHED_DETERMINISTIC = '1'; $env:MASHED_DET_FRAMES = '120'
  $env:MASHED_PATHPROG = '1'; $env:MASHED_PATHPROG_DUMP = $dump
  $p = Start-Process -FilePath $exe -WorkingDirectory $root -PassThru
  $ok = $p.WaitForExit(300000)
  if (-not $ok) { Stop-Process -Id $p.Id -Force; $p.WaitForExit(15000) | Out-Null }
  Write-Host ("{0}: pid={1} exit={2} self_exited={3} dump={4}" -f $t, $p.Id, $p.ExitCode, $ok, (Test-Path $dump))
}
Remove-Item Env:\MASHED_PATHPROG, Env:\MASHED_PATHPROG_DUMP -ErrorAction SilentlyContinue
