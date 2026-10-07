# GATEFIRE LEG 0 driver — PREREG_GATEFIRE.md section 3 (runtime substrate census).
# Seven runs: GF0off_{1,2,3} / GF0on_{1,2,3} on the gates dump (the census columns),
# plus GF0step on the AI stepdump for GF0-CTL (the default build must still hash to
# 86b7b2bb after this session's TrackRenderer edit).
#
# Deliberately writes GF0* names, NOT H3*: H3off_1/H3on_1.gates.csv are committed
# artifacts of a different schema and must not be overwritten.
#
# MEASUREMENT ONLY. Nothing is seeded. Spawns and stops ONLY its own PIDs.
$ErrorActionPreference = 'Stop'
$root = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$out  = Join-Path $root 'verify\d3_gatefire_20261008'
$exe  = Join-Path $root 'mashedmod\build\mashed_re.exe'
Set-Location $root

$FRAMES     = 14400
$WAIT_LIMIT = 900

$runs = @()
foreach ($i in 1..3) { $runs += @{ name = "GF0off_$i"; on = $false; step = $false } }
foreach ($i in 1..3) { $runs += @{ name = "GF0on_$i";  on = $true;  step = $false } }
$runs += @{ name = 'GF0step'; on = $false; step = $true }

foreach ($r in $runs) {
  $log = Join-Path $root 'mashed_re.log'
  if (Test-Path $log) { Remove-Item $log -Force }

  $env:MASHED_MUTE          = '1'
  $env:MASHED_TRACK_VIEW    = 'Training'
  $env:MASHED_CAR           = '1'
  $env:MASHED_ROUND         = '1'
  $env:MASHED_WIN_POS       = 'primary-bl'
  $env:MASHED_ROUND_RULE    = '4'
  $env:MASHED_DETERMINISTIC = '1'
  $env:MASHED_DET_FRAMES    = "$FRAMES"
  # Clear every lane knob first, then set only what this arm needs.
  Remove-Item Env:\MASHED_REFDIST        -ErrorAction SilentlyContinue
  Remove-Item Env:\MASHED_SLOTSTATE_SEED -ErrorAction SilentlyContinue
  Remove-Item Env:\MASHED_RACEPCT_BRIDGE -ErrorAction SilentlyContinue
  Remove-Item Env:\MASHED_U9186_GATES    -ErrorAction SilentlyContinue
  Remove-Item Env:\MASHED_AI_STEPDUMP    -ErrorAction SilentlyContinue

  if ($r.step) {
    # GF0-CTL: a DEFAULT build stepdump, every knob off, to be hashed against
    # 86b7b2bb (E2off_1 / H1step / H3step / H3step_r2).
    $art = Join-Path $out "$($r.name).csv"
    $env:MASHED_AI_STEPDUMP = $art
  } else {
    $art = Join-Path $out "$($r.name).gates.csv"
    # Both census arms carry the seed + race_pct bridge, exactly as run_h3.ps1's
    # gates arms did, so the census is read under the same conditions H3 measured.
    $env:MASHED_SLOTSTATE_SEED = '1'
    $env:MASHED_RACEPCT_BRIDGE = '1'
    $env:MASHED_U9186_GATES = $art
    if ($r.on) { $env:MASHED_REFDIST = '1' }
  }
  if (Test-Path $art) { Remove-Item $art -Force }

  Write-Host "=== $($r.name)  step=$($r.step) refdist=$($env:MASHED_REFDIST) ==="
  $p = Start-Process -FilePath $exe -WorkingDirectory $root -PassThru
  Write-Host "    pid=$($p.Id)"
  $exited = $p.WaitForExit($WAIT_LIMIT * 1000)
  if (-not $exited) {
    Write-Host "    TIMEOUT - stopping pid $($p.Id) ONLY"
    Stop-Process -Id $p.Id -Force
    $p.WaitForExit(15000) | Out-Null
  }
  Write-Host "    exit=$($p.ExitCode)  self_exited=$exited"
  if (Test-Path $log) { Copy-Item $log (Join-Path $out "$($r.name).log") -Force }
  if (Test-Path $art) {
    Write-Host ("    rows (incl header) = " + (Get-Content $art | Measure-Object -Line).Lines)
  } else { Write-Host "    NO artifact for $($r.name)" }
}
Write-Host 'GF0 DONE'
