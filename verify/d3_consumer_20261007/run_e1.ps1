# Leg E1 liveness driver. Sequential runs; each gets a clean mashed_re.log.
$ErrorActionPreference = 'Stop'
$root = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$out  = Join-Path $root 'verify\d3_consumer_20261007'
Set-Location $root

$runs = @(
  @{ name = 'L4';  rule = '4'  },
  @{ name = 'L10'; rule = '10' },
  @{ name = 'L0';  rule = ''   },
  @{ name = 'L4b'; rule = '4'  }
)

foreach ($r in $runs) {
  $log = Join-Path $root 'mashed_re.log'
  if (Test-Path $log) { Remove-Item $log -Force }
  $args = @(
    '-3.12', 're/tools/sa_capture.py', "verify/d3_consumer_20261007/$($r.name)", '8,120,240',
    'MASHED_MUTE=1', 'MASHED_TRACK_VIEW=Training', 'MASHED_CAR=1', 'MASHED_ROUND=1',
    'MASHED_WIN_POS=primary-bl',
    "MASHED_AI_STEPDUMP=$out\$($r.name).csv"
  )
  if ($r.rule -ne '') { $args += "MASHED_ROUND_RULE=$($r.rule)" }
  Write-Host "=== $($r.name) rule='$($r.rule)' ==="
  & py @args
  Write-Host "rc=$LASTEXITCODE"
  if (Test-Path $log) { Copy-Item $log (Join-Path $out "$($r.name).log") -Force }
  else { Write-Host "NO mashed_re.log for $($r.name)" }
}
Write-Host 'E1 DONE'
