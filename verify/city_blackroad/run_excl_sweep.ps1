# Does loading Clump_Exclude_From_World clumps help or hurt on every track that has a
# pose-matched original reference? Every one of the 13 tracks uses that command, so this
# cannot be judged on City alone -- and TRAINING's 15.45 is the project's headline number.
# Arms: skip = current behaviour; load = MASHED_TRACK_LOAD_EXCLUDED=1.
# Retries built in: this demo hangs at challenge-select maybe 1 run in 5 (seen on 3
# unrelated arms today), and a hang must not be read as a result.
$ErrorActionPreference = "Stop"
$root = $PSScriptRoot | Split-Path -Parent | Split-Path -Parent
$cases = @(
  @{ n="arctic_s8"; sel=0;  basis="-24.72314,4.48565,16.75757,-0.87708,0.09408,0.47105,-0.38027,0.46315,-0.80056,-0.29348,-0.88127,-0.37044" },
  @{ n="city";      sel=2;  basis="48.09298,3.72305,-25.70275,-0.09364,0.00000,-0.99561,0.51444,0.85616,-0.04838,0.85240,-0.51671,-0.08017" },
  @{ n="dump";      sel=11; basis="-0.26396,4.32455,25.72489,0.99808,0.00000,-0.06201,0.02766,0.89500,0.44521,0.05550,-0.44607,0.89328" },
  @{ n="training";  sel=12; basis="1.42528,2.89283,19.67259,-0.87664,0.47269,-0.08985,0.48115,0.86122,-0.16370,-0.00000,-0.18674,-0.98241" }
)
foreach ($c in $cases) {
  foreach ($arm in @("skip","load")) {
    $out = "verify/city_blackroad/sweep/$($c.n)/$arm"
    $env:MASHED_RACE_DEMO="1"; $env:MASHED_GOTO="6"; $env:MASHED_DETERMINISTIC="1"
    $env:MASHED_WIN_POS="left-bl"
    $env:MASHED_TRACK_SEL="$($c.sel)"; $env:MASHED_CAM_POSE=$c.basis
    $env:MASHED_VERIFY_OUT=$out
    if ($arm -eq "load") { $env:MASHED_TRACK_LOAD_EXCLUDED="1" }
    else { Remove-Item Env:\MASHED_TRACK_LOAD_EXCLUDED -ErrorAction SilentlyContinue }
    $shot = Join-Path $root "$out\race1\01_grid.bmp"
    $done = $false
    foreach ($try in 1..3) {
      $p = Start-Process -FilePath (Join-Path $root "mashedmod\build\mashed_re.exe") `
                         -WorkingDirectory $root -PassThru
      try { $p | Wait-Process -Timeout 220 } catch {}
      if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force }
      if (Test-Path $shot) { $done = $true; break }
    }
    Write-Host ("{0,-10} {1,-5} {2}" -f $c.n, $arm, $(if($done){"OK"}else{"FAILED after 3 tries"}))
  }
}
Write-Host "=== sweep done ==="
