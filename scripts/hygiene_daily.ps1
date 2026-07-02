# Daily Mashed RE hygiene sweep. Prints exactly a 5-line summary.
#
# Safety contract (multi-session rules):
#   - NEVER kills processes: doctor runs with --no-heal (DIAGNOSE-ONLY), so
#     check_zombie_games only reports; nothing here calls Stop-Process/taskkill.
#   - NEVER writes inside original\: worktree removal goes through
#     diag.py wt-remove --all-stale, the junction-safe path (no --force).
#   - Only mutation performed: removing CLEAN, stale worktrees via diag.py.

$ErrorActionPreference = 'Continue'
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root

# 1) Environment health, report-only.
$doctorOut = & py -3.12 scripts/diag.py doctor --no-heal 2>&1 | Out-String
$doctorRc = $LASTEXITCODE
$doctorLine = ($doctorOut -split "`r?`n" | Where-Object { $_ -match '^summary:' } | Select-Object -First 1)
if (-not $doctorLine) { $doctorLine = "no summary line (rc=$doctorRc)" }
$needAttention = if ($doctorLine -match '(\d+) need attention') { [int]$Matches[1] } else { -1 }

# 2) Stale-worktree purge (the only sanctioned bulk-removal path).
$wtOut = & py -3.12 scripts/diag.py wt-remove --all-stale 2>&1 | Out-String
$wtRc = $LASTEXITCODE
$wtLine = ($wtOut -split "`r?`n" | Where-Object { $_ -match '^wt-remove --all-stale:' } | Select-Object -First 1)
if ($wtLine) { $wtLine = $wtLine -replace '^wt-remove --all-stale:\s*', '' }
else { $wtLine = "no summary line (rc=$wtRc)" }

# 3) Backup freshness: newest folder under MashedRE_Backups must be <= 14 days old.
$bkRoot = 'C:\Users\maria\MashedRE_Backups'
$bkWarn = $true
$newest = Get-ChildItem -Directory $bkRoot -ErrorAction SilentlyContinue |
    Sort-Object Name -Descending | Select-Object -First 1
if (-not $newest) {
    $bkLine = "WARN - no backup folders under $bkRoot"
} else {
    $ts = $null
    # Folder names are yyyyMMdd-HHmmss; fall back to mtime if a name doesn't parse.
    try { $ts = [datetime]::ParseExact($newest.Name, 'yyyyMMdd-HHmmss', $null) } catch {}
    if (-not $ts) { $ts = $newest.LastWriteTime }
    $ageDays = [math]::Round(((Get-Date) - $ts).TotalDays, 1)
    if ($ageDays -gt 14) {
        $bkLine = "WARN - newest backup $($newest.Name) is $ageDays days old (>14): refresh MashedRE_Backups"
    } else {
        $bkWarn = $false
        $bkLine = "OK - newest backup $($newest.Name) is $ageDays days old"
    }
}

$status = if ($needAttention -eq 0 -and -not $bkWarn -and $doctorRc -eq 0 -and $wtRc -eq 0) { 'OK' } else { 'ATTENTION' }

Write-Output "=== Mashed RE hygiene $(Get-Date -Format 'yyyy-MM-dd HH:mm') ==="
Write-Output "DOCTOR:    $doctorLine"
Write-Output "WORKTREES: $wtLine"
Write-Output "BACKUP:    $bkLine"
Write-Output "STATUS:    $status"
