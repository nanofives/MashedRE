#requires -Version 7.0
<#
.SYNOPSIS
  Binary-search the installed-hook INDEX RANGE to find the hook that AVs MASHED at boot.

.DESCRIPTION
  Uses the hook system's built-in MASHED_HOOK_LO / MASHED_HOOK_HI knobs (Core/HookSystem.cpp:132)
  to install only registry indices [LO, HI). No rebuilds, no relinking — which is why this beats
  TU-level bisection: TU subsets of asi_sources.rsp are NOT self-contained and fail to LINK below
  ~170 files, and a link failure is not a bisect result.

  Established 2026-07-26 on the full .asi:
    MASHED_HOOK_HI=0 (install nothing) -> BOOTS 18 s
    all hooks installed               -> AV 0xC0000005 @ ~4.4 s
  So the trigger is hook INSTALLATION, not .asi static-init. (This corrects the 2026-07-25
  conclusion that subset installs "do not dodge it".)

  Invariant assumed by the search: installing [0, HI) is monotonic — GOOD for small HI, BAD once the
  culprit index is included. If the fault needs a COMBINATION of hooks this is violated; the script
  reports a monotonicity warning if it sees an inconsistency rather than silently returning nonsense.

  Traps encoded (both cost a full session — see verify/menu_crash_pc44/FINDINGS_2026-07-26.md):
   - ALWAYS WaitForExit() before reading ExitCode; otherwise this AV reads as -1 (0xFFFFFFFF), which
     CLAUDE.md documents as the force-kill signature, and you will call a crash a clean exit.
   - Only ever Stop-Process our own PID (never blanket-kill MASHED by name; other sessions may run).

.PARAMETER High
  Upper bound (exclusive) known to be BAD. Default 4096 = "all".

.PARAMETER Runs
  Boot attempts per probe (default 2). The AV is timing-sensitive; 1 run risks a false GOOD.

.PARAMETER Seconds
  Survival threshold (default 16). The AV lands ~4-7 s, so this clears it comfortably.
#>
[CmdletBinding()]
param([int]$High = 4096, [int]$Runs = 2, [int]$Seconds = 16)

$ErrorActionPreference = 'Stop'
$Repo = Split-Path -Parent $PSScriptRoot
$Exe  = Join-Path $Repo 'original\MASHED.exe'
$Wd   = Join-Path $Repo 'original'
$Manifest = Join-Path $Repo 'log\hook_manifest_bisect.txt'

function Test-Range([int]$lo, [int]$hi, [switch]$WithManifest) {
  $env:MASHED_HOOK_LO = $lo
  $env:MASHED_HOOK_HI = $hi
  if ($WithManifest) { $env:MASHED_HOOK_MANIFEST = $Manifest } else { Remove-Item Env:\MASHED_HOOK_MANIFEST -EA SilentlyContinue }
  $died = 0
  for ($i=1; $i -le $Runs; $i++) {
    $p = Start-Process -FilePath $Exe -WorkingDirectory $Wd -PassThru
    $sw = [Diagnostics.Stopwatch]::StartNew()
    $exited = $p.WaitForExit($Seconds * 1000)     # trap: never poll HasExited
    $t = [math]::Round($sw.Elapsed.TotalSeconds,2)
    if ($exited) { $died++; $code = $p.ExitCode; $last = "DIED ${t}s 0x$('{0:X8}' -f $code)" }
    else { Stop-Process -Id $p.Id -Force; $last = "BOOTS ${t}s" }
    Start-Sleep -Milliseconds 1500
  }
  Remove-Item Env:\MASHED_HOOK_LO,Env:\MASHED_HOOK_HI -EA SilentlyContinue
  Remove-Item Env:\MASHED_HOOK_MANIFEST -EA SilentlyContinue
  $bad = $died -gt 0
  Write-Host ("  [0,{0,4}) -> {1,-22} {2}" -f $hi, $last, $(if($bad){'BAD'}else{'GOOD'})) -ForegroundColor $(if($bad){'Red'}else{'Green'})
  return $bad
}

Write-Host "=== dumping hook manifest (index -> rva -> name) ===" -ForegroundColor Cyan
New-Item -ItemType Directory -Path (Split-Path $Manifest) -Force | Out-Null
Remove-Item $Manifest -EA SilentlyContinue
Test-Range 0 $High -WithManifest | Out-Null
if (Test-Path $Manifest) {
  $lines = Get-Content $Manifest
  Write-Host "manifest: $($lines.Count) entries -> $Manifest"
} else { Write-Warning "no manifest written; index->name mapping will be unavailable" }

Write-Host "`n=== sanity: bounds ===" -ForegroundColor Cyan
if (Test-Range 0 0)      { Write-Warning "HI=0 (install nothing) is BAD -> the fault is NOT hook installation. Stop; this script cannot help."; exit 3 }
if (-not (Test-Range 0 $High)) { Write-Warning "HI=$High is GOOD -> cannot reproduce the crash. Nothing to bisect."; exit 4 }

Write-Host "`n=== binary search for the first BAD index ===" -ForegroundColor Cyan
$lo = 0; $hi = $High      # invariant: [0,lo) GOOD, [0,hi) BAD
while ($hi - $lo -gt 1) {
  $mid = [int](($lo + $hi) / 2)
  if (Test-Range 0 $mid) { $hi = $mid } else { $lo = $mid }
}
$culprit = $hi - 1
Write-Host "`nFIRST BAD INDEX = $culprit  (installing [0,$hi) crashes; [0,$lo) boots)" -ForegroundColor Yellow

if (Test-Path $Manifest) {
  Write-Host "`n=== manifest rows around the culprit ===" -ForegroundColor Cyan
  Get-Content $Manifest | Select-Object -Skip ([Math]::Max($culprit-3,0)) -First 7
}

Write-Host "`nConfirm with a single-hook install once you have its name/RVA:" -ForegroundColor DarkGray
Write-Host "  `$env:MASHED_HOOK_ONLY='<name-or-0xRVA>'; then launch MASHED" -ForegroundColor DarkGray
Write-Host "NOTE: if the fault needs a COMBINATION of hooks, monotonicity fails and this index is" -ForegroundColor DarkGray
Write-Host "      only the point where the range first became bad - verify with MASHED_HOOK_ONLY." -ForegroundColor DarkGray
