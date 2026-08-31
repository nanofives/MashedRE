# Interaction check between the two changes landed today.
#
# Loading Clump_Exclude_From_World clumps makes the water clump appear on six tracks that
# never had one before (Forest, sands, Storm, SuperG, Warzone; rouabout has none but was
# left "suspect" by the earlier broadcheck). The water ambient fold is ON by default. So
# those tracks now get FOLDED water for the first time, and none of them has a
# pose-matched original reference to judge it against.
#
# This cannot give a verdict. It answers: does the fold now fire there, and how much does
# it move the frame? Arms differ only in MASHED_LIBRW_AMBFOLD_SEA.
# Also re-confirms the default actually flipped, on the two tracks with references.
$ErrorActionPreference = "Stop"
$root = $PSScriptRoot | Split-Path -Parent | Split-Path -Parent
$tracks = [ordered]@{ forest=3; sands=10; storm=6; superg=7; warzone=8; rouabout=9; city=2; training=12 }

foreach ($name in $tracks.Keys) {
    $sel = $tracks[$name]
    foreach ($arm in @("foldON","foldOFF")) {
        $out = "verify/city_blackroad/interaction/$name/$arm"
        $env:MASHED_RACE_DEMO="1"; $env:MASHED_GOTO="6"; $env:MASHED_DETERMINISTIC="1"
        $env:MASHED_WIN_POS="left-bl"; $env:MASHED_TRACK_SEL="$sel"
        $env:MASHED_VERIFY_OUT=$out
        Remove-Item Env:\MASHED_CAM_POSE -ErrorAction SilentlyContinue
        if ($arm -eq "foldOFF") { $env:MASHED_LIBRW_AMBFOLD_SEA="0" }
        else { Remove-Item Env:\MASHED_LIBRW_AMBFOLD_SEA -ErrorAction SilentlyContinue }
        $log = Join-Path $root "log\librw_scene.txt"
        Remove-Item $log -ErrorAction SilentlyContinue
        $shot = Join-Path $root "$out\race1\01_grid.bmp"
        $done=$false
        foreach ($try in 1..3) {
            $p = Start-Process -FilePath (Join-Path $root "mashedmod\build\mashed_re.exe") `
                               -WorkingDirectory $root -PassThru
            try { $p | Wait-Process -Timeout 220 } catch {}
            if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force }
            if (Test-Path $shot) { $done=$true; break }
        }
        if ($arm -eq "foldON" -and (Test-Path $log)) {
            $dest = Join-Path $root "verify\city_blackroad\interaction\$name\scene_dffs.txt"
            Select-String -Path $log -Pattern "^clump\[\d+\]: dff=" | ForEach-Object { $_.Line } |
                Sort-Object -Unique | Set-Content $dest
            $w = @(Select-String -Path $dest -Pattern "water_asset=1")
            Write-Host ("{0,-9} water_asset=1 clumps: {1}" -f $name, $w.Count)
            $w | ForEach-Object { Write-Host "    $($_.Line)" }
        }
        if (-not $done) { Write-Host "  $name/$arm FAILED after 3 tries" }
    }
}
Write-Host "=== interaction sweep done ==="
