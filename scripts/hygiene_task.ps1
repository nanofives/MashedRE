# Daily Mashed RE hygiene — registered as Windows scheduled task "MashedRE-Hygiene" (daily 09:00).
# Replaces the 2026-07-01 CronCreate job, which was session-scoped and did not persist.
# Runs: diag.py doctor --no-heal, diag.py wt-remove --all-stale, backup-freshness check (>14 days warns).
# Output appends to log\hygiene_task.log and echoes to stdout.
$ErrorActionPreference = 'Continue'
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8  # diag.py emits UTF-8; default OEM codepage mojibakes it
$repo = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$log  = Join-Path $repo 'log\hygiene_task.log'
Set-Location $repo

$lines = New-Object System.Collections.Generic.List[string]
$lines.Add(('=== MashedRE hygiene {0} ===' -f (Get-Date -Format 'yyyy-MM-dd HH:mm:ss')))

$lines.Add('--- py -3.12 scripts\diag.py doctor --no-heal ---')
& py -3.12 scripts\diag.py doctor --no-heal 2>&1 | ForEach-Object { $lines.Add([string]$_) }
$lines.Add(('doctor exit code: {0}' -f $LASTEXITCODE))

$lines.Add('--- py -3.12 scripts\diag.py wt-remove --all-stale ---')
& py -3.12 scripts\diag.py wt-remove --all-stale 2>&1 | ForEach-Object { $lines.Add([string]$_) }
$lines.Add(('wt-remove exit code: {0}' -f $LASTEXITCODE))

$lines.Add('--- backup freshness (C:\Users\maria\MashedRE_Backups) ---')
$backupRoot = 'C:\Users\maria\MashedRE_Backups'
if (Test-Path $backupRoot) {
    $newest = Get-ChildItem $backupRoot -Directory |
        Sort-Object CreationTime -Descending | Select-Object -First 1
    if ($null -eq $newest) {
        $lines.Add("WARNING: no backup folders found in $backupRoot")
    } else {
        $days = [math]::Round(((Get-Date) - $newest.CreationTime).TotalDays, 1)
        if ($days -gt 14) {
            $lines.Add("WARNING: newest backup '$($newest.Name)' is $days days old (>14) - refresh MashedRE_Backups")
        } else {
            $lines.Add("OK: newest backup '$($newest.Name)' is $days days old")
        }
    }
} else {
    $lines.Add("WARNING: backup root $backupRoot does not exist")
}

$lines.Add('')
$lines | Add-Content -Path $log -Encoding UTF8
$lines | Write-Output
