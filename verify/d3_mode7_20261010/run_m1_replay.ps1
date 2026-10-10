# mode 7 M1 G-M1-REG + G-M1-PASS (verify/d3_mode7_20261010/PREREG_MODE7.md section 3). MEASUREMENT ONLY; spawns and stops ONLY its own PIDs.
param([string[]]$Pcs = @('24','53'))
$root = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$out  = Join-Path $root 'verify\d3_mode7_20261010'
$exe  = Join-Path $root 'mashedmod\build\mashed_re.exe'
Set-Location $root
foreach ($pc in $Pcs) {
  $rep = Join-Path $out "m1_replay_$(if ($env:M1_TAG) {$env:M1_TAG} else {"rg1"})_pc$pc.txt"
  if (Test-Path $rep) { Remove-Item $rep -Force }
  $env:MASHED_MUTE = '1'; $env:MASHED_TRACK_VIEW = 'Training'; $env:MASHED_CAR = '1'
  $env:MASHED_ROUND = '1'; $env:MASHED_ROUND_RULE = '4'; $env:MASHED_WIN_POS = 'primary-bl'
  $env:MASHED_DETERMINISTIC = '1'; $env:MASHED_DET_FRAMES = '60'
  $env:MASHED_PATHPROG = '1'; $env:MASHED_REGISTRY_PC = $pc
  $env:MASHED_REGISTRY_REPLAY = Join-Path $out $(if ($env:M1_IN) {$env:M1_IN} else {'o_rg1.msd.reg.tsv'})
  $env:MASHED_REGISTRY_REPLAY_OUT = $rep
  $p = Start-Process -FilePath $exe -WorkingDirectory $root -PassThru
  $ok = $p.WaitForExit(600000)
  if (-not $ok) { Stop-Process -Id $p.Id -Force; $p.WaitForExit(15000) | Out-Null }
  Write-Host ("PC{0}: pid={1} exit={2} self_exited={3}" -f $pc, $p.Id, $p.ExitCode, $ok)
  if (Test-Path $rep) { Get-Content $rep | Select-Object -Last 12 }
}
foreach ($k in 'MASHED_PATHPROG','MASHED_REGISTRY_PC','MASHED_REGISTRY_REPLAY','MASHED_REGISTRY_REPLAY_OUT') { Remove-Item "Env:\$k" -ErrorAction SilentlyContinue }
