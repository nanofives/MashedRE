# Standalone Arctic capture at the ORIGINAL s8 sea-dominant pose (69% sea).
# Uses the FROZEN exe copy (verify/sea_level_20260929/bin), not mashedmod/build,
# because another session owns the build tree this session.
# Protocol copied verbatim from verify/geomlight_waterfold/run_arms.ps1.
param([string]$Arm = "base", [string]$Basis = "", [hashtable]$Extra = @{})
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
if (-not $Basis) { $Basis = (Get-Content (Join-Path $root "verify\arctic_ref\sea_search\s8\orig_cambasis.txt") -Raw).Trim() }
$out = "verify/sea_level_20260929/sa_$Arm"
$env:MASHED_RACE_DEMO     = "1"
$env:MASHED_GOTO          = "6"
$env:MASHED_DETERMINISTIC = "1"
$env:MASHED_WIN_POS       = "left-bl"
$env:MASHED_TRACK_SEL     = "0"          # kAreas[0] = Arctic = "Timgidski"
$env:MASHED_CAM_POSE      = $Basis
$env:MASHED_VERIFY_OUT    = $out
$env:MASHED_MUTE          = "1"
$env:MASHED_TITLE         = "sea-level investigation"
foreach ($k in $Extra.Keys) { Set-Item -Path "Env:\$k" -Value $Extra[$k] }
Write-Host "=== arm=$Arm basis=$Basis extra=$($Extra.Keys -join ',') ==="
$p = Start-Process -FilePath (Join-Path $root "verify\sea_level_20260929\bin\mashed_re.exe") `
                   -WorkingDirectory $root -PassThru
Write-Host "  pid $($p.Id)"
try { $p | Wait-Process -Timeout 180 } catch { Write-Host "  TIMEOUT -> killing $($p.Id)" }
if (-not $p.HasExited) { Stop-Process -Id $p.Id -Force }   # ONLY our pid
foreach ($k in $Extra.Keys) { Remove-Item "Env:\$k" -ErrorAction SilentlyContinue }
$shot = Join-Path $root "$out\race1\01_grid.bmp"
if (Test-Path $shot) { Write-Host "  shot OK $shot" } else { Write-Host "  SHOT MISSING $shot" }
