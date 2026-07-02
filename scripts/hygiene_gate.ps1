# Session-start hygiene gate: runs scripts/hygiene_daily.ps1 at most once every
# $ThresholdDays, and only when a work session actually starts. Silent no-op
# (exit 0, no output) when the last run is fresh, so most session starts cost
# nothing. Same safety contract as hygiene_daily.ps1: never kills processes,
# never writes inside original\.

$ThresholdDays = 3   # the "X amount of work" knob: raise/lower to taste

$root = Split-Path -Parent $PSScriptRoot
$stamp = Join-Path $root 're\diag\hygiene_last_run.txt'

if (Test-Path $stamp) {
    $last = $null
    try {
        $last = [datetime]::ParseExact((Get-Content $stamp -TotalCount 1).Trim(), 'yyyy-MM-dd HH:mm:ss', $null)
    } catch {}
    if ($last -and ((Get-Date) - $last).TotalDays -lt $ThresholdDays) { exit 0 }
}

& (Join-Path $PSScriptRoot 'hygiene_daily.ps1')
Set-Content $stamp (Get-Date -Format 'yyyy-MM-dd HH:mm:ss')
exit 0
