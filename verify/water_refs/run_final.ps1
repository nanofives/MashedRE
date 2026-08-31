# Final state of the branch: shipping default, NO env vars set at all, every track that
# has a pose-matched original reference. Fold OFF, excluded clumps loaded.
# Measured rather than inherited -- the earlier arms reached these code paths via env vars.
$ErrorActionPreference = "Stop"
$root = $PSScriptRoot | Split-Path -Parent | Split-Path -Parent
$cases = @(
  @{ n="arctic_s8"; sel=0;  b="-24.72314,4.48565,16.75757,-0.87708,0.09408,0.47105,-0.38027,0.46315,-0.80056,-0.29348,-0.88127,-0.37044" },
  @{ n="city";      sel=2;  b="48.09298,3.72305,-25.70275,-0.09364,0.00000,-0.99561,0.51444,0.85616,-0.04838,0.85240,-0.51671,-0.08017" },
  @{ n="dump";      sel=11; b="-0.26396,4.32455,25.72489,0.99808,0.00000,-0.06201,0.02766,0.89500,0.44521,0.05550,-0.44607,0.89328" },
  @{ n="training";  sel=12; b="1.42528,2.89283,19.67259,-0.87664,0.47269,-0.08985,0.48115,0.86122,-0.16370,-0.00000,-0.18674,-0.98241" },
  @{ n="forest_s8"; sel=3;  b="" },
  @{ n="superg_s14";sel=7;  b="" }
)
foreach ($c in $cases) {
    $basis = $c.b
    if (-not $basis) {
        $bf = Join-Path $root "verify\water_refs\$($c.n)\orig_cambasis.txt"
        $basis = (Get-Content $bf -Raw).Trim()
    }
    $out = "verify/water_refs/final/$($c.n)"
    $env:MASHED_RACE_DEMO="1"; $env:MASHED_GOTO="6"; $env:MASHED_DETERMINISTIC="1"
    $env:MASHED_WIN_POS="left-bl"; $env:MASHED_TRACK_SEL="$($c.sel)"
    $env:MASHED_CAM_POSE=$basis; $env:MASHED_VERIFY_OUT=$out
    Remove-Item Env:\MASHED_LIBRW_AMBFOLD_SEA,Env:\MASHED_LIBRW_AMBFOLD,`
                Env:\MASHED_TRACK_SKIP_EXCLUDED -ErrorAction SilentlyContinue
    $shot = Join-Path $root "$out\race1\01_grid.bmp"
    $done=$false
    foreach ($try in 1..3) {
        $p = Start-Process -FilePath (Join-Path $root "mashedmod\build\mashed_re.exe") `
                           -WorkingDirectory $root -PassThru
        try { $p | Wait-Process -Timeout 220 } catch {}
        if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force }
        if (Test-Path $shot) { $done=$true; break }
    }
    Write-Host ("{0,-12} {1}" -f $c.n, $(if($done){"OK"}else{"FAILED x3"}))
}
Write-Host "=== final done ==="
