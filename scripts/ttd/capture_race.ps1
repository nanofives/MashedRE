# capture_race.ps1 — autonomous race-trace capture: drive ORIGINAL MASHED into a
# canonical Quick-Battle race (Frida nav, no admin needed), then TTD-attach the live
# race and record. One UAC (TTD needs admin); nav + capture run back-to-back so the
# race can't end in a human-timing gap. The race self-runs (AI/camera/physics), so no
# input is needed during the capture window.
#
# Sequential by design: Frida navigates, then DETACHES (nav_to_race.py leaves MASHED
# running un-instrumented), THEN TTD attaches — never both instrumenting at once.
#
# Usage (normal terminal; self-elevates):
#   pwsh scripts\ttd\capture_race.ps1
#   pwsh scripts\ttd\capture_race.ps1 -Seconds 12 -MaxFileMB 2048

param(
    [int]$Seconds   = 10,
    [int]$MaxFileMB = 2048,
    [switch]$Elevated
)
$ErrorActionPreference = 'Stop'
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$Ttd      = Join-Path $RepoRoot 'tools\ttd_x86\TTD.exe'
$OutDir   = Join-Path $RepoRoot 'log\ttd'
$Nav      = Join-Path $RepoRoot 're\frida\nav_to_race.py'

function Test-Admin {
    $id = [Security.Principal.WindowsIdentity]::GetCurrent()
    (New-Object Security.Principal.WindowsPrincipal($id)).IsInRole(
        [Security.Principal.WindowsBuiltInRole]::Administrator)
}
if (-not (Test-Admin)) {
    Write-Host "[capture_race] not elevated; relaunching via UAC..." -ForegroundColor Yellow
    Start-Process pwsh -Verb RunAs -ArgumentList @('-NoProfile','-ExecutionPolicy','Bypass',
        '-File',$PSCommandPath,'-Seconds',$Seconds,'-MaxFileMB',$MaxFileMB,'-Elevated')
    return
}
# The elevated relaunch opens its OWN console, so its output is invisible to the caller
# (an un-elevated shell just sees "relaunching via UAC" and exits 0). Transcript every
# elevated run to a log so a failed capture can be diagnosed after the fact instead of
# guessing -- this cost a full debug cycle on 2026-09-09.
$LogFile = Join-Path $OutDir ("capture_race_{0:yyyy-MM-dd_HHmmss}.log" -f (Get-Date))
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
try { Start-Transcript -Path $LogFile -Force | Out-Null } catch { }
Write-Host "[capture_race] transcript: $LogFile"

if (-not (Test-Path $Ttd)) { throw "TTD.exe not found at $Ttd (run setup_recorder.ps1)." }
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

# --- 1. navigate the ORIGINAL into a live race (Frida; leaves MASHED running) -------
Write-Host "[capture_race] driving MASHED into a Quick-Battle race..." -ForegroundColor Cyan
$navOut = & py -3.12 $Nav 2>&1
$navOut | ForEach-Object { Write-Host "  $_" }
$m = $navOut | Select-String 'RACE_PID=(\d+)' | Select-Object -Last 1
$racePid = if ($m) { [int]$m.Matches[0].Groups[1].Value } else { 0 }
if ($racePid -le 0) {
    Write-Host "[capture_race] navigation did not reach a race — aborting." -ForegroundColor Red
    if ($Elevated) { Read-Host "press Enter to close" }
    exit 3
}
# Snapshot the pre-existing traces. Reporting "the newest .run in the folder" is a
# FALSE-SUCCESS generator: on 2026-09-09 TTD was blocked by Defender ASR and wrote
# nothing, yet this script announced DONE and pointed at a trace from three months
# earlier. A capture must only ever claim a file that did not exist before it ran.
$preExisting = @(Get-ChildItem $OutDir -Filter *.run -ErrorAction SilentlyContinue |
                 Select-Object -ExpandProperty FullName)

Write-Host "[capture_race] race live in pid=$racePid; attaching TTD (full-process, ring=$MaxFileMB MB, ~$Seconds s)" -ForegroundColor Cyan

# --- 2. TTD-attach the running race --------------------------------------------------
$recArgs = @('-acceptEula','-out',$OutDir,'-ring','-maxFile',$MaxFileMB,
             '-timestampFileName','-attach',$racePid)
Start-Process -FilePath $Ttd -ArgumentList $recArgs -WindowStyle Hidden | Out-Null
Start-Sleep -Seconds $Seconds
Write-Host "[capture_race] stopping recording (pid=$racePid)..." -ForegroundColor Cyan
& $Ttd -stop $racePid 2>&1 | Write-Host
& $Ttd -wait 120      2>&1 | Write-Host

# --- 3. report -----------------------------------------------------------------------
$run = Get-ChildItem $OutDir -Filter *.run -ErrorAction SilentlyContinue |
       Where-Object { $preExisting -notcontains $_.FullName } |
       Sort-Object LastWriteTime -Descending | Select-Object -First 1
if ($run) {
    Write-Host ""
    Write-Host "[capture_race] DONE -> $($run.FullName)" -ForegroundColor Green
    Write-Host ("[capture_race] size: {0:N1} MB" -f ($run.Length/1MB)) -ForegroundColor Green
    Write-Host "Next: py -3.12 scripts\ttd\extract_calls.py $($run.FullName) --rva 0x4c3b30 --count 128" -ForegroundColor Green
} else {
    Write-Host '[capture_race] NO NEW .run produced -- the capture FAILED.' -ForegroundColor Red
    Write-Host '[capture_race] TTD reporting an empty trace name and writing no file means the' -ForegroundColor Yellow
    Write-Host '[capture_race] recorder was killed before it wrote anything. On this machine the' -ForegroundColor Yellow
    Write-Host '[capture_race] known cause is the Defender ASR rule "Block use of copied or' -ForegroundColor Yellow
    Write-Host '[capture_race] impersonated system tools" killing \ttd_x86\TTDInject.exe: the recorder' -ForegroundColor Yellow
    Write-Host '[capture_race] is a COPY out of the WinDbg AppX, which is what that rule targets.' -ForegroundColor Yellow
    Write-Host '[capture_race] Check Defender > Protection history, then see the TTD README.' -ForegroundColor Yellow
    if ($Elevated) { Read-Host 'press Enter to close' }
    exit 4
}
try { Stop-Transcript | Out-Null } catch { }
Write-Host "[capture_race] transcript written: $LogFile"

# leave MASHED running for inspection; user can close it.
if ($Elevated) { Read-Host "press Enter to close" }
