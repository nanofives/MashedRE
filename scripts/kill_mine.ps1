#requires -Version 5
<#
kill_mine.ps1 — the ONLY sanctioned way for a fleet session to kill a MASHED instance.

Why this exists: multiple Claude sessions run their own MASHED concurrently. A blanket
kill (`taskkill /im MASHED.exe`, `Stop-Process -Name MASHED`, `pkill MASHED`) terminates
OTHER sessions' games — it has destroyed another session's TTD capture before (2026-06-17).

This script physically cannot blanket-kill:
  - It accepts a SINGLE numeric PID only. `[ValidatePattern('^\d+$')]` rejects any name,
    wildcard, `-Name`, or `/im` at parameter-binding time — there is no name path.
  - It verifies the PID is actually a MASHED.exe process before killing, so it cannot kill
    a sibling's non-MASHED process or a mistyped PID.
  - It only ever touches that one PID.

Usage:  pwsh scripts/kill_mine.ps1 <PID>
        (use the PID your tool printed, e.g. run_diff.py / scenario_launch.py "pid=NNNN")

NOTE (account2): custom PreToolUse hooks are disabled (allowManagedHooksOnly), so a raw
`taskkill /im` cannot be blocked at the harness level here — the fleet prompt FORBIDS it and
mandates this wrapper. On account3 a PreToolUse hook can enforce it physically; see
re/FLEET_KICKOFF.md.
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory, Position = 0)]
    [ValidatePattern('^\d+$')]          # digits only: a name/wildcard/-Name/-im fails binding
    [string]$ProcId
)
$ErrorActionPreference = 'Stop'
$id = [int]$ProcId
if ($id -le 4) { Write-Error "refusing PID $id (system/idle range)"; exit 2 }

$p = Get-Process -Id $id -ErrorAction SilentlyContinue
if (-not $p) { Write-Error "no live process with PID $id (already exited?)"; exit 3 }
if ($p.ProcessName -ne 'MASHED') {
    Write-Error "REFUSING: PID $id is '$($p.ProcessName)', not MASHED. kill_mine only ever kills a verified MASHED PID."
    exit 4
}
Stop-Process -Id $id -Force -Confirm:$false
Write-Output "killed MASHED PID $id"
