# U-9186 leg G1 driver (PREREG_G1.md section 2). Three measurement runs under the F2
# determinism knobs. Waits for the DET_FRAMES self-exit; stops only the PID it spawned.
$ErrorActionPreference = 'Stop'
$root = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$out  = Join-Path $root 'verify\d3_u9186_20261008'
$exe  = Join-Path $root 'mashedmod\build\mashed_re.exe'
Set-Location $root

$FRAMES     = 14400
$WAIT_LIMIT = 900

$runs = @(
  @{ name = 'G1base'; extra = @{} },
  @{ name = 'G1seed'; extra = @{ MASHED_SLOTSTATE_SEED = '1' } },
  @{ name = 'G1both'; extra = @{ MASHED_SLOTSTATE_SEED = '1'; MASHED_RACEPCT_BRIDGE = '1' } }
)

foreach ($r in $runs) {
  $log = Join-Path $root 'mashed_re.log'
  if (Test-Path $log) { Remove-Item $log -Force }
  $gates = Join-Path $out "$($r.name).gates.csv"
  if (Test-Path $gates) { Remove-Item $gates -Force }

  $env:MASHED_MUTE          = '1'
  $env:MASHED_TRACK_VIEW    = 'Training'
  $env:MASHED_CAR           = '1'
  $env:MASHED_ROUND         = '1'
  $env:MASHED_WIN_POS       = 'primary-bl'
  $env:MASHED_ROUND_RULE    = '4'
  $env:MASHED_DETERMINISTIC = '1'
  $env:MASHED_DET_FRAMES    = "$FRAMES"
  $env:MASHED_U9186_GATES   = $gates
  Remove-Item Env:\MASHED_SLOTSTATE_SEED  -ErrorAction SilentlyContinue
  Remove-Item Env:\MASHED_RACEPCT_BRIDGE  -ErrorAction SilentlyContinue
  Remove-Item Env:\MASHED_AI_STEPDUMP     -ErrorAction SilentlyContinue
  foreach ($k in $r.extra.Keys) { Set-Item "Env:\$k" $r.extra[$k] }

  Write-Host "=== $($r.name)  seed=$($env:MASHED_SLOTSTATE_SEED) bridge=$($env:MASHED_RACEPCT_BRIDGE) ==="
  $p = Start-Process -FilePath $exe -WorkingDirectory $root -PassThru
  Write-Host "    pid=$($p.Id)"
  $exited = $p.WaitForExit($WAIT_LIMIT * 1000)
  if (-not $exited) {
    Write-Host "    TIMEOUT - stopping pid $($p.Id) ONLY"
    Stop-Process -Id $p.Id -Force
    $p.WaitForExit(15000) | Out-Null
  }
  Write-Host "    exit=$($p.ExitCode)  self_exited=$exited"
  if (Test-Path $log)   { Copy-Item $log (Join-Path $out "$($r.name).log") -Force }
  if (Test-Path $gates) {
    Write-Host ("    gates rows (incl header) = " + (Get-Content $gates | Measure-Object -Line).Lines)
  } else { Write-Host "    NO gates csv for $($r.name)" }
}
Write-Host 'G1 DONE'
