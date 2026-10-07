# -Step runs ONLY the AMEND_H3.md section 2 stepdump arm (H3step / H3step_r2):
# a DEFAULT build with every knob unset, dumping the 78-column MASHED_AI_STEPDUMP
# schema that ai_speed_env.py / ai_ctrl_window.py / det_prefix.py take. The gates
# arms emit the 24-column MASHED_U9186_GATES schema instead, which is why
# H3-KNOBOFF / -NOREG-E / -NOREG-B could not be scored off them (RESULT_H3.md s4).
# NOTE: param() must be the first statement in a PowerShell script.
param([switch]$Step)

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
if ($Step) {
  foreach ($i in 1..2) { $runs += @{ name = ($(if ($i -eq 1) { 'H3step' } else { "H3step_r$i" })); step = $true } }
} else {
  foreach ($i in 1..3) { $runs += @{ name = "H3off_$i"; on = $false } }
  foreach ($i in 1..3) { $runs += @{ name = "H3on_$i";  on = $true  } }
}
foreach ($r in $runs) {
  $log = Join-Path $root 'mashed_re.log'
  if (Test-Path $log) { Remove-Item $log -Force }
  $env:MASHED_MUTE = '1'; $env:MASHED_TRACK_VIEW = 'Training'
  $env:MASHED_CAR = '1';  $env:MASHED_ROUND = '1'
  $env:MASHED_WIN_POS = 'primary-bl'; $env:MASHED_ROUND_RULE = '4'
  $env:MASHED_DETERMINISTIC = '1'; $env:MASHED_DET_FRAMES = "$FRAMES"
  # Clear every H3-lane knob first, then set only what this arm needs.
  Remove-Item Env:\MASHED_REFDIST        -ErrorAction SilentlyContinue
  Remove-Item Env:\MASHED_SLOTSTATE_SEED -ErrorAction SilentlyContinue
  Remove-Item Env:\MASHED_RACEPCT_BRIDGE -ErrorAction SilentlyContinue
  Remove-Item Env:\MASHED_U9186_GATES    -ErrorAction SilentlyContinue
  Remove-Item Env:\MASHED_AI_STEPDUMP    -ErrorAction SilentlyContinue
  if ($r.step) {
    $art = Join-Path $out "$($r.name).csv"
    $env:MASHED_AI_STEPDUMP = $art
  } else {
    $art = Join-Path $out "$($r.name).gates.csv"
    $env:MASHED_SLOTSTATE_SEED = '1'; $env:MASHED_RACEPCT_BRIDGE = '1'
    $env:MASHED_U9186_GATES = $art
    if ($r.on) { $env:MASHED_REFDIST = '1' }
  }
  if (Test-Path $art) { Remove-Item $art -Force }
  Write-Host "=== $($r.name)  step=$($r.step) refdist=$($env:MASHED_REFDIST) ==="
  $p = Start-Process -FilePath $exe -WorkingDirectory $root -PassThru
  Write-Host "    pid=$($p.Id)"
  $ok = $p.WaitForExit(900000)
  if (-not $ok) { Write-Host "    TIMEOUT - stopping pid $($p.Id) ONLY"; Stop-Process -Id $p.Id -Force; $p.WaitForExit(15000) | Out-Null }
  Write-Host "    exit=$($p.ExitCode)  self_exited=$ok"
  if (Test-Path $log) { Copy-Item $log (Join-Path $out "$($r.name).log") -Force }
  if (Test-Path $art) { Write-Host ("    rows = " + (Get-Content $art | Measure-Object -Line).Lines) } else { Write-Host "    NO artifact" }
}
Write-Host 'H3 DONE'
