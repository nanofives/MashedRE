# mode 7 M4 G-M4-REPLAY (PREREG_M4.md section 2). MEASUREMENT ONLY; spawns and stops ONLY its own PIDs.
param([string[]]$Pcs = @('24','53'))
$root = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$out  = Join-Path $root 'verify\d3_mode7_m4_20261010'
$exe  = Join-Path $root 'mashedmod\build\mashed_re.exe'
Set-Location $root
$sets = @(
  @{ tag = 'orig'; ins = @('o_c30a.msd.c30.tsv', 'o_c30b.msd.c30.tsv') }
)
foreach ($s in $sets) {
  foreach ($pc in $Pcs) {
    $rep = Join-Path $out "m4_replay_$($s.tag)_pc$pc.txt"
    if (Test-Path $rep) { Remove-Item $rep -Force }
    $env:MASHED_MUTE = '1'; $env:MASHED_TRACK_VIEW = 'Training'; $env:MASHED_CAR = '1'
    $env:MASHED_ROUND = '1'; $env:MASHED_ROUND_RULE = '4'; $env:MASHED_WIN_POS = 'primary-bl'
    $env:MASHED_DETERMINISTIC = '1'; $env:MASHED_DET_FRAMES = '60'
    $env:MASHED_PATHPROG = '1'; $env:MASHED_MODE7_PC = $pc
    $env:MASHED_C30_REPLAY = ($s.ins | ForEach-Object { Join-Path $out $_ }) -join ';'
    $env:MASHED_C30_REPLAY_OUT = $rep
    $p = Start-Process -FilePath $exe -WorkingDirectory $root -PassThru
    $ok = $p.WaitForExit(600000)
    if (-not $ok) { Stop-Process -Id $p.Id -Force; $p.WaitForExit(15000) | Out-Null }
    Write-Host ("{0} PC{1}: pid={2} exit={3} self_exited={4}" -f $s.tag, $pc, $p.Id, $p.ExitCode, $ok)
    if (Test-Path $rep) { Get-Content $rep | Select-Object -Last 8 }
  }
}
foreach ($k in 'MASHED_PATHPROG','MASHED_MODE7_PC','MASHED_C30_REPLAY','MASHED_C30_REPLAY_OUT') { Remove-Item "Env:\$k" -ErrorAction SilentlyContinue }
