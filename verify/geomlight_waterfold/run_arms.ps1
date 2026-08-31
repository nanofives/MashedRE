# Water-scoped ambient-fold verification: two arms per track, sequential.
#   geomON    = current race/geomlight default (fold entirely off)
#   geomWATER = MASHED_LIBRW_AMBFOLD_SEA=1 (refined key: water ASSET NAME + water flag class)
# Protocol is verify/geomlight_broadcheck/RESULT.md: an ORIGINAL 12-float camera basis is
# transplanted via MASHED_CAM_POSE so both arms and the original reference share a vantage.
# Shot measured downstream: <arm>/race1/01_grid.bmp (validated: reproduces the published
# City numbers 1.9% / 74.0 / 115.0 / 148.3 to within 0.1 luma).
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$base = $PSScriptRoot

# track dir -> MASHED_TRACK_SEL (kAreas[] index)
$tracks = [ordered]@{
    arctic_s8  = 0
    arctic_s14 = 0
    city       = 2
    dump       = 11
    training   = 12
}
# TRAINING has no basis file of its own; it uses the handoff basis
# (re/analysis/NEXT_SESSION_race_parity_20260830.md), the same one the 15.45 came from.
$training_basis = "1.42528,2.89283,19.67259,-0.87664,0.47269,-0.08985,0.48115,0.86122,-0.16370,-0.00000,-0.18674,-0.98241"

foreach ($t in $tracks.Keys) {
    $bf = Join-Path $base "$t\orig_cambasis.txt"
    $basis = if (Test-Path $bf) { (Get-Content $bf -Raw).Trim() } else { $training_basis }
    foreach ($arm in @("geomON", "geomWATER")) {
        $out = "verify/geomlight_waterfold/$t/$arm"
        Write-Host "=== $t / $arm (SEL=$($tracks[$t])) ==="
        $env:MASHED_RACE_DEMO    = "1"
        $env:MASHED_GOTO         = "6"
        $env:MASHED_DETERMINISTIC= "1"
        $env:MASHED_WIN_POS      = "left-bl"
        $env:MASHED_TRACK_SEL    = "$($tracks[$t])"
        $env:MASHED_CAM_POSE     = $basis
        $env:MASHED_VERIFY_OUT   = $out
        if ($arm -eq "geomWATER") { $env:MASHED_LIBRW_AMBFOLD_SEA = "1" }
        else { Remove-Item Env:\MASHED_LIBRW_AMBFOLD_SEA -ErrorAction SilentlyContinue }
        # Wait for EXIT, never poll for the BMP: a shot that exists early is a MENU frame
        # and yields a confidently wrong number (memory race-capture-wait-for-exit).
        $p = Start-Process -FilePath (Join-Path $root "mashedmod\build\mashed_re.exe") `
                           -WorkingDirectory $root -PassThru
        try { $p | Wait-Process -Timeout 180 } catch { Write-Host "  TIMEOUT -> killing $($p.Id)" }
        if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force }   # only OUR pid, never by name
        $shot = Join-Path $root "$out\race1\01_grid.bmp"
        if (Test-Path $shot) { Write-Host "  shot OK $shot" } else { Write-Host "  SHOT MISSING $shot" }
    }
}
Write-Host "=== all arms done ==="
