# register_capture_task.ps1 — ONE-TIME setup so race captures run unattended.
#
# TTD recording requires admin (proven: "Administrative privileges are required in
# order to record program execution", 0x80070005). I can't click UAC. This registers
# a Scheduled Task that runs capture_race.ps1 with HIGHEST privileges; thereafter it
# can be TRIGGERED from a normal (non-elevated) shell with NO UAC prompt:
#
#     Start-ScheduledTask -TaskName MashedTtdRaceCapture
#     # or: schtasks /run /tn MashedTtdRaceCapture
#
# => fully autonomous capture: the task drives MASHED into a race (Frida nav) and
#    TTD-records it, producing log\ttd\MASHED_<ts>.run.
#
# Registering needs admin ONCE (this script self-elevates -> one UAC). The task runs
# as the current user, interactively (so MASHED's window + nav work), elevated.
#
# SECURITY NOTE: this leaves a persistent task that runs an in-repo script elevated
# on demand. Remove it any time with:
#     pwsh scripts\ttd\register_capture_task.ps1 -Remove
#     # or: Unregister-ScheduledTask -TaskName MashedTtdRaceCapture -Confirm:$false

param([switch]$Elevated, [switch]$Remove, [int]$Seconds = 10)
$ErrorActionPreference = 'Stop'
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$Cap      = Join-Path $RepoRoot 'scripts\ttd\capture_race.ps1'
$TaskName = 'MashedTtdRaceCapture'

function Test-Admin {
    $id = [Security.Principal.WindowsIdentity]::GetCurrent()
    (New-Object Security.Principal.WindowsPrincipal($id)).IsInRole(
        [Security.Principal.WindowsBuiltInRole]::Administrator)
}
if (-not (Test-Admin)) {
    Write-Host "[register] not elevated; relaunching via UAC (one-time)..." -ForegroundColor Yellow
    $a = @('-NoProfile','-ExecutionPolicy','Bypass','-File',$PSCommandPath,'-Elevated','-Seconds',$Seconds)
    if ($Remove) { $a += '-Remove' }
    Start-Process pwsh -Verb RunAs -ArgumentList $a
    return
}

if ($Remove) {
    Unregister-ScheduledTask -TaskName $TaskName -Confirm:$false -ErrorAction SilentlyContinue
    Write-Host "[register] removed task '$TaskName'." -ForegroundColor Green
    if ($Elevated) { Read-Host "press Enter to close" }
    return
}

$pwshExe = (Get-Command pwsh).Source
$arg = '-NoProfile -ExecutionPolicy Bypass -File "{0}" -Seconds {1}' -f $Cap, $Seconds
$action    = New-ScheduledTaskAction    -Execute $pwshExe -Argument $arg -WorkingDirectory $RepoRoot
$principal = New-ScheduledTaskPrincipal  -UserId ([Security.Principal.WindowsIdentity]::GetCurrent().Name) `
                                         -LogonType Interactive -RunLevel Highest
$settings  = New-ScheduledTaskSettingsSet -AllowStartIfOnBatteries -DontStopIfGoingOnBatteries `
                                          -ExecutionTimeLimit (New-TimeSpan -Minutes 5) `
                                          -MultipleInstances IgnoreNew
Register-ScheduledTask -TaskName $TaskName -Action $action -Principal $principal `
                       -Settings $settings -Force | Out-Null

Write-Host "[register] task '$TaskName' registered (RunLevel Highest, interactive)." -ForegroundColor Green
Write-Host "Trigger unattended (no UAC) with:" -ForegroundColor Green
Write-Host "    Start-ScheduledTask -TaskName $TaskName" -ForegroundColor Green
if ($Elevated) { Read-Host "press Enter to close" }
