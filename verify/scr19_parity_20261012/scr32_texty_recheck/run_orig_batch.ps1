param([Parameter(Mandatory)] [int[]]$Screens, [int]$Retries = 2)
# Sequential original-side captures (recheck_frame_probe.py), one PID at a time, each killed by the probe itself.
# A run that fails (no json / probe exit != 0) is retried: original-side piz collisions (U-9677) are re-runs.
$here = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $here
foreach ($s in $Screens) {
    for ($a = 0; $a -le $Retries; $a++) {
        $out = "orig_frame$s.json"; $bmp = "orig_bb$s.bmp"
        if (Test-Path $out) { Remove-Item $out }
        $log = py -3.12 recheck_frame_probe.py $s $out $bmp 2>&1
        $log | Out-File -Append -Encoding utf8 orig_batch.log
        if ($LASTEXITCODE -eq 0 -and (Test-Path $out) -and ((Get-Content $out -Raw) -notmatch '"frame": null')) { "screen $s OK (attempt $a)"; break }
        "screen $s FAILED attempt $a"
    }
}
