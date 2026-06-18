# setup_recorder.ps1 — copy the TTD recorder + replay engine out of the WinDbg
# AppX package into tools\ttd_x86\ (gitignored; WindowsApps ACLs block running the
# binaries in place). Idempotent. Run once per machine / after a WinDbg update.
#
# Requires the Microsoft.WinDbg Store package (provides TTD.exe + TTDReplay*).
#   winget install Microsoft.WinDbg     # if absent
#
# Usage:  pwsh scripts\ttd\setup_recorder.ps1
$ErrorActionPreference = 'Stop'
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$Dst      = Join-Path $RepoRoot 'tools\ttd_x86'

$pkg = Get-AppxPackage Microsoft.WinDbg -ErrorAction SilentlyContinue
if (-not $pkg) {
    throw "Microsoft.WinDbg package not found. Install it:  winget install Microsoft.WinDbg"
}
$src = Join-Path $pkg.InstallLocation 'x86\ttd'
if (-not (Test-Path (Join-Path $src 'TTD.exe'))) {
    throw "x86 TTD.exe not found under $src — package layout changed; inspect $($pkg.InstallLocation)."
}
New-Item -ItemType Directory -Force -Path $Dst | Out-Null
Copy-Item -Path (Join-Path $src '*') -Destination $Dst -Recurse -Force

$ttd = Join-Path $Dst 'TTD.exe'
$ver = (& $ttd -? 2>&1 | Select-Object -First 1)
Write-Host "[setup_recorder] recorder ready at $Dst" -ForegroundColor Green
Write-Host "[setup_recorder] $ver" -ForegroundColor Green
Get-ChildItem $Dst | Select-Object Name,Length | Format-Table -AutoSize
