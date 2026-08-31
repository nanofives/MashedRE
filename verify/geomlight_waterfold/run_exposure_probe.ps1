# Disambiguate the 0.00% exposure masks, and retry the sands/foldON timeout.
#
# A 0.00% fold mask has TWO possible causes and they support different claims:
#   (a) a water asset IS in view, the fold fired, and it changed nothing measurable
#   (b) no water asset is in view at this vantage, so the fold never ran
# Only (a) licenses "the default flip is inert here". (b) means untested, not safe.
# BuildClump's per-clump `dff= water_asset=` log settles it directly, so this run keeps
# a PER-TRACK copy of log/librw_scene.txt instead of letting the next run overwrite it.
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$tracks = [ordered]@{ forest = 3; sands = 10; storm = 6; superg = 7; warzone = 8; rouabout = 9 }

foreach ($name in $tracks.Keys) {
    $sel = $tracks[$name]
    $out = "verify/geomlight_waterfold/exposure/$name/foldON"
    Write-Host "=== $name (SEL=$sel) foldON, keeping scene log ==="
    $env:MASHED_RACE_DEMO     = "1"
    $env:MASHED_GOTO          = "6"
    $env:MASHED_DETERMINISTIC = "1"
    $env:MASHED_WIN_POS       = "left-bl"
    $env:MASHED_TRACK_SEL     = "$sel"
    $env:MASHED_VERIFY_OUT    = $out
    Remove-Item Env:\MASHED_CAM_POSE -ErrorAction SilentlyContinue
    Remove-Item Env:\MASHED_LIBRW_AMBFOLD_SEA -ErrorAction SilentlyContinue
    $log = Join-Path $root "log\librw_scene.txt"
    Remove-Item $log -ErrorAction SilentlyContinue
    $p = Start-Process -FilePath (Join-Path $root "mashedmod\build\mashed_re.exe") `
                       -WorkingDirectory $root -PassThru
    try { $p | Wait-Process -Timeout 180 } catch { Write-Host "  TIMEOUT -> killing $($p.Id)" }
    if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force }
    if (Test-Path $log) {
        $dest = Join-Path $root "verify\geomlight_waterfold\exposure\$name\scene_dffs.txt"
        Select-String -Path $log -Pattern "^clump\[\d+\]: dff=" | ForEach-Object { $_.Line } |
            Sort-Object -Unique | Set-Content $dest
        $hits = @(Select-String -Path $dest -Pattern "water_asset=1")
        Write-Host "  water_asset=1 clumps: $($hits.Count)"
        $hits | ForEach-Object { Write-Host "    $($_.Line)" }
    } else { Write-Host "  NO SCENE LOG" }
}
Write-Host "=== probe done ==="
