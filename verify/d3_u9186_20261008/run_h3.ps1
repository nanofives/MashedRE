# U-9186 leg H3 driver (PREREG_H3.md section 2). Six runs: OFF/ON x 3 repeats.
# Both arms carry MASHED_SLOTSTATE_SEED (H3's gate chain needs it) and the
# race_pct bridge (FUN_00408ad0's input). ON adds MASHED_REFDIST.
$ErrorActionPreference = 'Stop'
$root = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$out  = Join-Path $root 'verify\d3_u9186_20261008'
$exe  = Join-Path $root 'mashedmod\build\mashed_re.exe'
Set-Location $root
$FRAMES = 14400
$runs = @()
foreach ($i in 1..3) { $runs += @{ name = "H3off_$i"; on = $false } }
foreach ($i in 1..3) { $runs += @{ name = "H3on_$i";  on = $true  } }
foreach ($r in $runs) {
  $log = Join-Path $root 'mashed_re.log'
  if (Test-Path $log) { Remove-Item $log -Force }
  $art = Join-Path $out "$($r.name).gates.csv"
  if (Test-Path $art) { Remove-Item $art -Force }
  $env:MASHED_MUTE = '1'; $env:MASHED_TRACK_VIEW = 'Training'
  $env:MASHED_CAR = '1';  $env:MASHED_ROUND = '1'
  $env:MASHED_WIN_POS = 'primary-bl'; $env:MASHED_ROUND_RULE = '4'
  $env:MASHED_DETERMINISTIC = '1'; $env:MASHED_DET_FRAMES = "$FRAMES"
  $env:MASHED_SLOTSTATE_SEED = '1'; $env:MASHED_RACEPCT_BRIDGE = '1'
  $env:MASHED_U9186_GATES = $art
  Remove-Item Env:\MASHED_REFDIST -ErrorAction SilentlyContinue
  if ($r.on) { $env:MASHED_REFDIST = '1' }
  Write-Host "=== $($r.name)  refdist=$($env:MASHED_REFDIST) ==="
  $p = Start-Process -FilePath $exe -WorkingDirectory $root -PassThru
  Write-Host "    pid=$($p.Id)"
  $ok = $p.WaitForExit(900000)
  if (-not $ok) { Write-Host "    TIMEOUT - stopping pid $($p.Id) ONLY"; Stop-Process -Id $p.Id -Force; $p.WaitForExit(15000) | Out-Null }
  Write-Host "    exit=$($p.ExitCode)  self_exited=$ok"
  if (Test-Path $log) { Copy-Item $log (Join-Path $out "$($r.name).log") -Force }
  if (Test-Path $art) { Write-Host ("    rows = " + (Get-Content $art | Measure-Object -Line).Lines) } else { Write-Host "    NO artifact" }
}
Write-Host 'H3 DONE'
