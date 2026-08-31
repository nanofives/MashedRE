# Exposure sweep for the tracks that gained a fold when it was defaulted ON.
#
# Forest / sands / Storm / SuperG / Warzone carry water DFFs (WATER, WATERFALL, SEA,
# RIVER) but have NO pose-matched original reference, so this cannot deliver a verdict.
# It answers the narrower question the default flip actually raises: HOW MUCH does the
# fold change these tracks? mask 0.00% => the flip is inert there and no reference is
# needed. A large mask => a reference capture is owed before trusting the default.
#
# rouabout is included because verify/geomlight_broadcheck/RESULT.md left it "suspect,
# not judged" (fired 2.0% on a natural cam, 0% on the ref vantage).
#
# Stage-1 method of that broadcheck: standalone-only, natural camera (no basis to
# transplant since there is no original), both arms otherwise identical.
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)

# Label -> kAreas[] index, per verify/geomlight_broadcheck/RESULT.md.
# Keys are STRINGS on purpose: with integer keys, $tracks[$k] on an OrderedDictionary
# resolves as a POSITIONAL index rather than a key lookup, which silently returned
# empty labels and collapsed all 12 runs into 3 colliding output dirs on the first try.
$tracks = [ordered]@{
    forest   = 3
    sands    = 10
    storm    = 6
    superg   = 7
    warzone  = 8
    rouabout = 9
}

foreach ($name in $tracks.Keys) {
    $sel = $tracks[$name]
    foreach ($arm in @("foldON", "foldOFF")) {
        $out = "verify/geomlight_waterfold/exposure/$name/$arm"
        Write-Host "=== $name / $arm (SEL=$sel) ==="
        $env:MASHED_RACE_DEMO     = "1"
        $env:MASHED_GOTO          = "6"
        $env:MASHED_DETERMINISTIC = "1"
        $env:MASHED_WIN_POS       = "left-bl"
        $env:MASHED_TRACK_SEL     = "$sel"
        $env:MASHED_VERIFY_OUT    = $out
        Remove-Item Env:\MASHED_CAM_POSE -ErrorAction SilentlyContinue
        if ($arm -eq "foldOFF") { $env:MASHED_LIBRW_AMBFOLD_SEA = "0" }
        else { Remove-Item Env:\MASHED_LIBRW_AMBFOLD_SEA -ErrorAction SilentlyContinue }
        $p = Start-Process -FilePath (Join-Path $root "mashedmod\build\mashed_re.exe") `
                           -WorkingDirectory $root -PassThru
        try { $p | Wait-Process -Timeout 180 } catch { Write-Host "  TIMEOUT -> killing $($p.Id)" }
        if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force }   # our pid only
        $shot = Join-Path $root "$out\race1\01_grid.bmp"
        if (Test-Path $shot) { Write-Host "  shot OK" } else { Write-Host "  SHOT MISSING $shot" }
    }
}
Write-Host "=== exposure sweep done ==="
