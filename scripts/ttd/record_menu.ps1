# record_menu.ps1 — TTD feasibility spike, capture phase.
#
# Records a Time Travel Debugging trace of an ALREADY-BOOTED MASHED.exe sitting
# at the main menu. Attach-at-menu (not record-the-boot) is deliberate: it
# sidesteps the fragile compat-shim/EMULATEHEAP boot path (see CLAUDE.md "Runtime
# state") and directly tests the two claims that motivate the TTD lane:
#   1. TTD can capture the hot menu loop (FastSqrt ~2,700 calls/s) where a Frida
#      Interceptor on the same path destabilizes MASHED in ~6 s.
#   2. The resulting .run is replayable offline to extract per-call state.
#
# FULL-PROCESS recording (no -module). Selective -module MASHED.exe recording on
# attach produced a trace with NO recorded threads ("empty trace", 2026-06-17) ->
# TTD.Calls/TTD.Memory all 0. -ring -maxFile bounds the file to the most-recent
# window; the menu hot loop (FastSqrt ~2,700/s) lands in that window regardless.
#
# REQUIRES ELEVATION (TTD recording needs admin). This script self-elevates via
# UAC. Recorder binaries were copied out of the WinDbg AppX to tools\ttd_x86\
# (WindowsApps ACLs block execution in place).
#
# Usage (from a normal terminal — it will prompt for elevation):
#   pwsh scripts\ttd\record_menu.ps1                 # 10 s capture, 512 MB ring
#   pwsh scripts\ttd\record_menu.ps1 -Seconds 15 -MaxFileMB 1024
#
# PRECONDITION: boot MASHED.exe the normal way and leave it at the main menu,
# visible (do NOT minimize — see memory project_intro_minimize_freeze), THEN run.

param(
    [int]$Seconds   = 10,
    [int]$MaxFileMB = 2048,
    [int]$ProcId    = 0,     # target an explicit MASHED PID (multi-session safe)
    [switch]$Elevated
)
$ErrorActionPreference = 'Stop'

$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$Ttd      = Join-Path $RepoRoot 'tools\ttd_x86\TTD.exe'
$OutDir   = Join-Path $RepoRoot 'log\ttd'

function Test-Admin {
    $id = [Security.Principal.WindowsIdentity]::GetCurrent()
    (New-Object Security.Principal.WindowsPrincipal($id)).IsInRole(
        [Security.Principal.WindowsBuiltInRole]::Administrator)
}

# --- self-elevate ---------------------------------------------------------
if (-not (Test-Admin)) {
    Write-Host "[record_menu] not elevated; relaunching via UAC..." -ForegroundColor Yellow
    $argl = @('-NoProfile','-ExecutionPolicy','Bypass','-File',$PSCommandPath,
              '-Seconds',$Seconds,'-MaxFileMB',$MaxFileMB,'-Elevated')
    Start-Process pwsh -Verb RunAs -ArgumentList $argl
    return
}

# --- preflight ------------------------------------------------------------
if (-not (Test-Path $Ttd)) {
    throw "TTD.exe not found at $Ttd. Run scripts\ttd\setup_recorder.ps1 (or copy x86\ttd\* out of the WinDbg package) first."
}
# Multi-session safe: target an explicit PID if given; otherwise auto-pick ONLY when a
# single MASHED is running. NEVER blanket-kill, NEVER grab "first of many" (could be
# another session's instance). See memory feedback_multisession_mashed_kill_by_pid.
if ($ProcId -gt 0) {
    $proc = Get-Process -Id $ProcId -ErrorAction SilentlyContinue
    if (-not $proc) {
        Write-Host "[record_menu] no process with PID $ProcId." -ForegroundColor Red
        if ($Elevated) { Read-Host "press Enter to close" }; exit 1
    }
} else {
    $all = @(Get-Process MASHED -ErrorAction SilentlyContinue)
    if ($all.Count -eq 0) {
        Write-Host "[record_menu] MASHED.exe is not running. Boot it, leave it visible, then re-run." -ForegroundColor Red
        if ($Elevated) { Read-Host "press Enter to close" }; exit 1
    }
    if ($all.Count -gt 1) {
        Write-Host "[record_menu] $($all.Count) MASHED instances running (PIDs: $($all.Id -join ', '))." -ForegroundColor Red
        Write-Host "Refusing to guess — re-run with -ProcId <pid> to target one (avoids capturing another session's game)." -ForegroundColor Red
        if ($Elevated) { Read-Host "press Enter to close" }; exit 1
    }
    $proc = $all[0]
}
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

Write-Host "[record_menu] attaching TTD to MASHED.exe pid=$($proc.Id)" -ForegroundColor Cyan
Write-Host "[record_menu] full-process, ring=$MaxFileMB MB, ~$Seconds s capture" -ForegroundColor Cyan
Write-Host "[record_menu] expect MASHED to slow/stutter while recording — that is TTD instrumentation, not a hang." -ForegroundColor DarkGray

# --- record ---------------------------------------------------------------
# -attach starts the recording and the controller stays resident; we stop it by PID.
$recArgs = @('-acceptEula','-out',$OutDir,'-ring','-maxFile',$MaxFileMB,
             '-timestampFileName','-attach',$proc.Id)
$rec = Start-Process -FilePath $Ttd -ArgumentList $recArgs -PassThru -WindowStyle Hidden

Start-Sleep -Seconds $Seconds

Write-Host "[record_menu] stopping recording (pid=$($proc.Id))..." -ForegroundColor Cyan
& $Ttd -stop $proc.Id 2>&1 | Write-Host
& $Ttd -wait 120     2>&1 | Write-Host   # let the recorder finalize the .run

# --- report ---------------------------------------------------------------
$run = Get-ChildItem $OutDir -Filter *.run -ErrorAction SilentlyContinue |
       Sort-Object LastWriteTime -Descending | Select-Object -First 1
if ($run) {
    Write-Host ""
    Write-Host "[record_menu] DONE -> $($run.FullName)" -ForegroundColor Green
    Write-Host ("[record_menu] size: {0:N1} MB" -f ($run.Length/1MB)) -ForegroundColor Green
    Write-Host "Next: extract FastSqrt calls with scripts\ttd\extract_fastsqrt.txt (see README)." -ForegroundColor Green
} else {
    Write-Host "[record_menu] NO .run produced — recording failed. Check $OutDir for a .out diagnostic." -ForegroundColor Red
}
if ($Elevated) { Read-Host "press Enter to close" }
