# Backup Mashed RE critical binaries + handcrafted state to an out-of-project
# location, so a future incident wiping `original/` (like the 2026-05-20
# c3_batch_k incident) can be recovered without re-decompressing the game.
#
# Backups go to:
#   $env:USERPROFILE\MashedRE_Backups\<timestamp>\
# (timestamped subdirs — never overwrite history)
#
# Restore is `Copy-Item -Recurse <backup>\original\* <project>\original\`.
#
# Run after every state-changing event:
#   - Initial setup (now)
#   - After re-patching MASHED.exe
#   - After rebuilding d3d9 shim
#   - Before any risky cleanup pass
#
# Idempotent: re-running creates a new timestamped backup, doesn't touch
# old ones.

$ErrorActionPreference = 'Stop'

$Project = Resolve-Path "$PSScriptRoot\.."
$BackupRoot = Join-Path $env:USERPROFILE "MashedRE_Backups"
$Stamp = Get-Date -Format "yyyyMMdd-HHmmss"
$Dest = Join-Path $BackupRoot $Stamp

Write-Host "=== Mashed RE backup ==="
Write-Host "  source: $Project"
Write-Host "  dest:   $Dest"

New-Item -ItemType Directory -Force -Path $Dest | Out-Null

# 1. original/ — the game binaries + handcrafted state
$Src = Join-Path $Project "original"
$Dst = Join-Path $Dest "original"
Write-Host ""
Write-Host "Backing up original/ -> $Dst"
Copy-Item -Path $Src -Destination $Dst -Recurse -Force

# 2. canonical configs we may need to restore
$Src = Join-Path $Project "scripts\canonical"
$Dst = Join-Path $Dest "scripts_canonical"
if (Test-Path $Src) {
    Write-Host "Backing up scripts/canonical/ -> $Dst"
    Copy-Item -Path $Src -Destination $Dst -Recurse -Force
}

# 3. built d3d9 shim (the proxy we deploy to original\d3d9.dll)
$Src = Join-Path $Project "mashedmod\build\d3d9.dll"
$Dst = Join-Path $Dest "mashedmod_build_d3d9.dll"
if (Test-Path $Src) {
    Write-Host "Backing up mashedmod/build/d3d9.dll -> $Dst"
    Copy-Item -Path $Src -Destination $Dst -Force
}

# 4. record SHA-256 of every backed-up file (so we can verify integrity later)
$Manifest = Join-Path $Dest "MANIFEST.sha256"
Write-Host ""
Write-Host "Writing SHA-256 manifest -> $Manifest"
Get-ChildItem -Path $Dest -Recurse -File `
    | Where-Object { $_.Name -ne "MANIFEST.sha256" } `
    | ForEach-Object {
        $hash = (Get-FileHash -Algorithm SHA256 -Path $_.FullName).Hash.ToLower()
        $rel = $_.FullName.Substring($Dest.Length + 1)
        "$hash  $rel"
      } `
    | Out-File -FilePath $Manifest -Encoding ascii

# 5. info file
$Info = Join-Path $Dest "INFO.txt"
@"
Mashed RE backup created $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
Project root: $Project
Git HEAD:     $(git -C "$Project" rev-parse HEAD 2>$null)

Contents:
  original/                  Full original/ dir (game binaries + state)
  scripts_canonical/         Canonical configs (videocfg etc.)
  mashedmod_build_d3d9.dll   Built d3d9 shim proxy
  MANIFEST.sha256            SHA-256 of every backed-up file
  INFO.txt                   This file

Restore (PowerShell, run from project root):
  `$src = "$Dest"
  Copy-Item -Recurse -Force "`$src\original\*" original\
  Copy-Item -Recurse -Force "`$src\scripts_canonical\*" scripts\canonical\
  Copy-Item -Force "`$src\mashedmod_build_d3d9.dll" mashedmod\build\d3d9.dll
  Copy-Item -Force original\d3d9.dll original\d3d9.dll  # redeploy proxy

To verify integrity later:
  pwsh scripts\verify_backup.ps1 $Dest
"@ | Out-File -FilePath $Info -Encoding utf8

# 6. list newest 5 backups so the user knows what they have
Write-Host ""
Write-Host "Existing backups under ${BackupRoot}:"
Get-ChildItem -Path $BackupRoot -Directory `
    | Sort-Object Name -Descending `
    | Select-Object -First 5 `
    | ForEach-Object {
        $size = (Get-ChildItem $_.FullName -Recurse -File `
                 | Measure-Object -Property Length -Sum).Sum / 1MB
        $sizeStr = [Math]::Round($size, 1)
        Write-Host "  $($_.Name)  ($sizeStr MB)"
      }

Write-Host ""
Write-Host "=== Backup done ==="
Write-Host "Restore command:"
Write-Host "  Get-Content '$Info' | Select-String -Pattern 'Restore' -Context 0,5"
