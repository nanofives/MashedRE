# Final confirmation on the SHIPPING default (no env vars set at all): excluded clumps
# loaded + water fold on. Pose-matched basis transplanted, so these numbers are directly
# comparable to the project's reference figures.
# Measured rather than assumed: the earlier sweep reached this same code path via
# MASHED_TRACK_LOAD_EXCLUDED=1 on a build where it was opt-in, and "logically identical"
# is not a measurement.
$ErrorActionPreference = "Stop"
$root = $PSScriptRoot | Split-Path -Parent | Split-Path -Parent
$cases = @(
  @{ n="arctic_s8"; sel=0;  basis="-24.72314,4.48565,16.75757,-0.87708,0.09408,0.47105,-0.38027,0.46315,-0.80056,-0.29348,-0.88127,-0.37044" },
  @{ n="city";      sel=2;  basis="48.09298,3.72305,-25.70275,-0.09364,0.00000,-0.99561,0.51444,0.85616,-0.04838,0.85240,-0.51671,-0.08017" },
  @{ n="dump";      sel=11; basis="-0.26396,4.32455,25.72489,0.99808,0.00000,-0.06201,0.02766,0.89500,0.44521,0.05550,-0.44607,0.89328" },
  @{ n="training";  sel=12; basis="1.42528,2.89283,19.67259,-0.87664,0.47269,-0.08985,0.48115,0.86122,-0.16370,-0.00000,-0.18674,-0.98241" }
)
foreach ($c in $cases) {
    $out = "verify/city_blackroad/confirm/$($c.n)"
    $env:MASHED_RACE_DEMO="1"; $env:MASHED_GOTO="6"; $env:MASHED_DETERMINISTIC="1"
    $env:MASHED_WIN_POS="left-bl"; $env:MASHED_TRACK_SEL="$($c.sel)"
    $env:MASHED_CAM_POSE=$c.basis; $env:MASHED_VERIFY_OUT=$out
    Remove-Item Env:\MASHED_TRACK_LOAD_EXCLUDED,Env:\MASHED_TRACK_SKIP_EXCLUDED,`
                Env:\MASHED_LIBRW_AMBFOLD_SEA,Env:\MASHED_LIBRW_AMBFOLD -ErrorAction SilentlyContinue
    $shot = Join-Path $root "$out\race1\01_grid.bmp"
    $done=$false
    foreach ($try in 1..3) {
        $p = Start-Process -FilePath (Join-Path $root "mashedmod\build\mashed_re.exe") `
                           -WorkingDirectory $root -PassThru
        try { $p | Wait-Process -Timeout 220 } catch {}
        if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force }
        if (Test-Path $shot) { $done=$true; break }
    }
    Write-Host ("{0,-10} {1}" -f $c.n, $(if($done){"OK"}else{"FAILED after 3 tries"}))
}
Write-Host "=== confirm done ==="
