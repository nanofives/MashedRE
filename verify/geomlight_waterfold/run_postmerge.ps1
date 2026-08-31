# Post-merge acceptance: re-run the two gates on the MERGED tree.
# race/first-frame-parity carries 13 commits race/geomlight never saw (camera basis, mip
# chain, spawn grid), any of which could interact with the fold, so the pre-merge numbers
# do not transfer for free. Gates: Arctic sea matches the original, TRAINING stays 15.45.
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$cases = @(
    @{ name = "arctic_s8"; sel = 0;  basis = "-24.72314,4.48565,16.75757,-0.87708,0.09408,0.47105,-0.38027,0.46315,-0.80056,-0.29348,-0.88127,-0.37044" },
    @{ name = "training";  sel = 12; basis = "1.42528,2.89283,19.67259,-0.87664,0.47269,-0.08985,0.48115,0.86122,-0.16370,-0.00000,-0.18674,-0.98241" }
)
foreach ($c in $cases) {
    foreach ($arm in @("foldON", "foldOFF")) {
        $out = "verify/geomlight_waterfold/postmerge/$($c.name)/$arm"
        Write-Host "=== $($c.name) / $arm ==="
        $env:MASHED_RACE_DEMO     = "1"
        $env:MASHED_GOTO          = "6"
        $env:MASHED_DETERMINISTIC = "1"
        $env:MASHED_WIN_POS       = "left-bl"
        $env:MASHED_TRACK_SEL     = "$($c.sel)"
        $env:MASHED_CAM_POSE      = $c.basis
        $env:MASHED_VERIFY_OUT    = $out
        if ($arm -eq "foldOFF") { $env:MASHED_LIBRW_AMBFOLD_SEA = "0" }
        else { Remove-Item Env:\MASHED_LIBRW_AMBFOLD_SEA -ErrorAction SilentlyContinue }
        $p = Start-Process -FilePath (Join-Path $root "mashedmod\build\mashed_re.exe") `
                           -WorkingDirectory $root -PassThru
        try { $p | Wait-Process -Timeout 180 } catch { Write-Host "  TIMEOUT -> killing $($p.Id)" }
        if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force }
        $shot = Join-Path $root "$out\race1\01_grid.bmp"
        if (Test-Path $shot) { Write-Host "  shot OK" } else { Write-Host "  SHOT MISSING" }
    }
}
Write-Host "=== postmerge done ==="
