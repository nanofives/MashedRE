# Clear the Forest/SuperG water debt: for each captured original vantage, run the
# standalone with the fold ON (shipping default) and OFF, basis transplanted.
#
# The fold mask |foldON - foldOFF| > 6 IS the water surface, so the judgement is the same
# one the Arctic sea got: does the fold move the water TOWARD the original (correct) or
# AWAY (over-bright)? Several settle times per track because the verdict must be read on a
# WATER-DOMINANT pose -- on a pose where water is a few percent of the frame the number is
# dominated by everything else, which produced a confidently wrong call on Arctic once.
$ErrorActionPreference = "Stop"
$root = $PSScriptRoot | Split-Path -Parent | Split-Path -Parent
# dir -> MASHED_TRACK_SEL (kAreas[]): Forest 3, SuperG 7
$sel = @{ forest_s8=3; forest_s14=3; forest_s20=3; superg_s8=7; superg_s14=7; superg_s20=7 }

foreach ($name in $sel.Keys | Sort-Object) {
    $bf = Join-Path $root "verify\water_refs\$name\orig_cambasis.txt"
    if (-not (Test-Path $bf)) { Write-Host "$name : NO BASIS, skipped"; continue }
    $basis = (Get-Content $bf -Raw).Trim()
    foreach ($arm in @("foldON","foldOFF")) {
        $out = "verify/water_refs/$name/$arm"
        $env:MASHED_RACE_DEMO="1"; $env:MASHED_GOTO="6"; $env:MASHED_DETERMINISTIC="1"
        $env:MASHED_WIN_POS="left-bl"; $env:MASHED_TRACK_SEL="$($sel[$name])"
        $env:MASHED_CAM_POSE=$basis; $env:MASHED_VERIFY_OUT=$out
        if ($arm -eq "foldOFF") { $env:MASHED_LIBRW_AMBFOLD_SEA="0" }
        else { Remove-Item Env:\MASHED_LIBRW_AMBFOLD_SEA -ErrorAction SilentlyContinue }
        $shot = Join-Path $root "$out\race1\01_grid.bmp"
        $done=$false
        foreach ($try in 1..3) {   # challenge-select hang is ~1 in 5; never read it as a result
            $p = Start-Process -FilePath (Join-Path $root "mashedmod\build\mashed_re.exe") `
                               -WorkingDirectory $root -PassThru
            try { $p | Wait-Process -Timeout 220 } catch {}
            if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force }
            if (Test-Path $shot) { $done=$true; break }
        }
        Write-Host ("{0,-12} {1,-8} {2}" -f $name, $arm, $(if($done){"OK"}else{"FAILED x3"}))
    }
}
Write-Host "=== water_refs arms done ==="
