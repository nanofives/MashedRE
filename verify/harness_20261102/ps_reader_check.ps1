param([string]$Dir, [string]$Wt)
$env:TEMP = $Dir; $env:MASHED_LOCK_DIR = $Dir
. (Join-Path $Wt 're\orchestrator\MachineLock.ps1')
$o = & py.exe -3.12 (Join-Path $Wt 're\orchestrator\mashed_lock.py') prio-hold --owner-pid $PID --label ps-reader
$l = @(Get-PrioLinger 'mashed_machine')
"[1/2] ps-Get-PrioLinger-sees-train-hold $(if ($l.Count -eq 1 -and [int]$l[0].pid -eq $PID -and $l[0].held) { 'PASS' } else { 'FAIL' }) ($o; count=$($l.Count))"
& py.exe -3.12 (Join-Path $Wt 're\orchestrator\mashed_lock.py') prio-release --owner-pid $PID --linger 0 | Out-Null
$l = @(Get-PrioLinger 'mashed_machine')
"[2/2] ps-Get-PrioLinger-empty-after-release $(if ($l.Count -eq 0) { 'PASS' } else { 'FAIL' }) (count=$($l.Count))"
