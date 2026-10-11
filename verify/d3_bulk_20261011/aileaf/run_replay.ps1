# D3 bulk G-AILEAF: G-AILEAF-REPLAY. MEASUREMENT ONLY; spawns and stops ONLY its own PIDs.
# Rows: re/tools/aileaf_unicorn.py (original bytes in Unicorn). The exe replays them through the naked bodies.
param([string]$In, [string]$Tag)
$root = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$out  = Join-Path $root 'verify\d3_bulk_20261011\aileaf'
$exe  = Join-Path $root 'mashedmod\build\mashed_re.exe'
Set-Location $root
$rep = Join-Path $out "replay_$Tag.txt"
if (Test-Path $rep) { Remove-Item $rep -Force }
$env:MASHED_MUTE = '1'; $env:MASHED_TRACK_VIEW = 'Training'; $env:MASHED_CAR = '1'
$env:MASHED_ROUND = '1'; $env:MASHED_ROUND_RULE = '4'; $env:MASHED_WIN_POS = 'primary-bl'
$env:MASHED_DETERMINISTIC = '1'; $env:MASHED_DET_FRAMES = '60'
$env:MASHED_PATHPROG = '1'
$env:MASHED_AILEAF_REPLAY = $In
$env:MASHED_AILEAF_REPLAY_OUT = $rep
$env:MASHED_AILEAF_REPLAY_ROWS = (Join-Path $out "replay_${Tag}_rows.tsv")
$p = Start-Process -FilePath $exe -WorkingDirectory $root -PassThru
$ok = $p.WaitForExit(600000)
if (-not $ok) { Stop-Process -Id $p.Id -Force; $p.WaitForExit(15000) | Out-Null }
Write-Host ("{0}: pid={1} exit={2} self_exited={3}" -f $Tag, $p.Id, $p.ExitCode, $ok)
if (Test-Path $rep) { Get-Content $rep | Select-Object -Last 12 }
foreach ($k in 'MASHED_PATHPROG','MASHED_AILEAF_REPLAY','MASHED_AILEAF_REPLAY_OUT','MASHED_AILEAF_REPLAY_ROWS','MASHED_DETERMINISTIC','MASHED_DET_FRAMES') { Remove-Item "Env:\$k" -ErrorAction SilentlyContinue }
