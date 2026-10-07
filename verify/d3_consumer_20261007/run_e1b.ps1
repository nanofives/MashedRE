# Leg E1 resume: the two runs the first driver did not finish (L0 control, L4b repeat).
$ErrorActionPreference = 'Stop'
$root = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$out  = Join-Path $root 'verify\d3_consumer_20261007'
Set-Location $root

$runs = @(
  @{ name = 'L0';  rule = ''  },
  @{ name = 'L4b'; rule = '4' }
)

foreach ($r in $runs) {
  $log = Join-Path $root 'mashed_re.log'
  if (Test-Path $log) { Remove-Item $log -Force }
  $a = @(
    '-3.12', 're/tools/sa_capture.py', "verify/d3_consumer_20261007/$($r.name)", '8,120,240',
    'MASHED_MUTE=1', 'MASHED_TRACK_VIEW=Training', 'MASHED_CAR=1', 'MASHED_ROUND=1',
    'MASHED_WIN_POS=primary-bl',
    "MASHED_AI_STEPDUMP=$out\$($r.name).csv"
  )
  if ($r.rule -ne '') { $a += "MASHED_ROUND_RULE=$($r.rule)" }
  Write-Host "=== $($r.name) rule='$($r.rule)' ==="
  & py @a
  Write-Host "rc=$LASTEXITCODE"
  if (Test-Path $log) { Copy-Item $log (Join-Path $out "$($r.name).log") -Force }
  else { Write-Host "NO mashed_re.log for $($r.name)" }
}
Write-Host 'E1b DONE'
