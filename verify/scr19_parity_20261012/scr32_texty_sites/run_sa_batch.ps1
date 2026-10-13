param([Parameter(Mandatory)] [int[]]$Screens, [string]$Build = 'sites', [switch]$NoBmp)
# Sequential standalone captures of ONE build, one PID at a time (run_sa.ps1 kills only its own PID).
$here = Split-Path -Parent $MyInvocation.MyCommand.Path
foreach ($s in $Screens) {
    $out = Join-Path $here "sa${s}_$Build.json"
    $bmp = if ($NoBmp) { "" } else { Join-Path $here "sa_bb${s}_$Build.bmp" }
    & (Join-Path $here 'run_sa.ps1') -Screen $s -Build $Build -Out $out -Bmp $bmp 2>&1 | Out-File -Append -Encoding utf8 (Join-Path $here 'sa_batch.log')
    if (Test-Path $out) { "screen $s $Build OK" } else { "screen $s $Build NO OUTPUT" }
}
