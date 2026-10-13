param([Parameter(Mandatory)] [int[]]$Screens, [switch]$NoBmp)
# Sequential standalone captures, pre and post builds, one PID at a time (run_sa.ps1 kills only its own PID).
$here = Split-Path -Parent $MyInvocation.MyCommand.Path
foreach ($s in $Screens) {
    foreach ($b in 'pre', 'post') {
        $out = Join-Path $here "sa${s}_$b.json"
        $bmp = if ($NoBmp) { "" } else { Join-Path $here "sa_bb${s}_$b.bmp" }
        & (Join-Path $here 'run_sa.ps1') -Screen $s -Build $b -Out $out -Bmp $bmp 2>&1 | Out-File -Append -Encoding utf8 (Join-Path $here 'sa_batch.log')
        if (Test-Path $out) { "screen $s $b OK" } else { "screen $s $b NO OUTPUT" }
    }
}
