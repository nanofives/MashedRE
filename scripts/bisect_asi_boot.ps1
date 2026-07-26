#requires -Version 7.0
<#
.SYNOPSIS
  Boot-bisect the dev .asi against the pc=0x44 load-time AV.

.DESCRIPTION
  Builds mashed_re_dev.asi from a SUBSET of mashedmod\asi_sources.rsp, deploys it, and boot-tests
  original\MASHED.exe N times, classifying each run as BOOTS or AV with its exit code and timing.

  Encodes the three footguns that cost a full session on 2026-07-26 (see
  verify/menu_crash_pc44/FINDINGS_2026-07-26.md):

  1. LoadDevAsi() in dinput8_shim.cpp FALLS BACK to ..\mashedmod\build\mashed_re_dev.asi when
     original\mashed_re_dev.asi is absent. Renaming only the deployed copy does NOT unload the hooks.
     This script always writes BOTH locations so what you test is what you built.
  2. Start-Process -PassThru + HasExited polling reports ExitCode = -1 (0xFFFFFFFF) for this AV,
     which CLAUDE.md documents as the FORCE-KILL signature -> reads as a clean exit. This script
     always calls WaitForExit() before reading ExitCode. True AV code is 0xC0000005.
  3. build_bisect_asi.bat omits QhullBridge_asi.obj and QHULL_LIB, so any subset touching Collision\
     fails to link. This script mirrors build.bat's real asi link line instead.

  Also: the AV fires ~5 s after .asi load, and a single-hook run_diff finishes in ~4.2 s. So a GREEN
  diff can be a RACE WIN rather than a healthy harness. -Seconds defaults to 20 to clear that window
  by a wide margin.

.PARAMETER Count
  Take the first N .cpp entries of asi_sources.rsp. For a binary bisect over the ordered list.

.PARAMETER ListFile
  Explicit newline-separated list of rsp-relative .cpp paths (quotes optional) to build instead.

.PARAMETER Exclude
  Regex; drop matching TUs from the selected set. Use to test "everything except the suspect".

.PARAMETER Runs
  Boot attempts per build (default 2). The AV is timing-sensitive, so >1 guards against a lucky run.

.PARAMETER Seconds
  Survival threshold in seconds (default 20).

.PARAMETER Restore
  Skip building; just restore the pristine .asi backup and exit.

.EXAMPLE
  # baseline: is the current full .asi bad?
  pwsh scripts\bisect_asi_boot.ps1 -Count 0

.EXAMPLE
  # binary bisect step: first 170 of 339 TUs
  pwsh scripts\bisect_asi_boot.ps1 -Count 170

.EXAMPLE
  # test a named suspect out
  pwsh scripts\bisect_asi_boot.ps1 -Exclude 'Collision\\RwpSolverCore2\d'
#>
[CmdletBinding()]
param(
  [int]$Count = -1,
  [string]$ListFile,
  [string]$Exclude,
  [int]$Runs = 2,
  [int]$Seconds = 20,
  [switch]$Restore
)

$ErrorActionPreference = 'Stop'
$Repo   = Split-Path -Parent $PSScriptRoot
$Mod    = Join-Path $Repo 'mashedmod'
$Out    = Join-Path $Mod 'build'
$Src    = Join-Path $Mod 'src\mashed_re'
$Rsp    = Join-Path $Mod 'asi_sources.rsp'
$Deployed = Join-Path $Repo 'original\mashed_re_dev.asi'
$BuiltAsi = Join-Path $Out  'mashed_re_dev.asi'
$Pristine = Join-Path $Out  'mashed_re_dev.asi.pristine'
$VcVars = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat'
$QhullLib = Join-Path $Mod 'deps\qhull-2002.1\qhull_2002_1.lib'
$QhullObj = Join-Path $Out 'QhullBridge_asi.obj'

function Save-Pristine {
  if (-not (Test-Path $Pristine) -and (Test-Path $BuiltAsi)) {
    Copy-Item $BuiltAsi $Pristine -Force
    Write-Host "[pristine] saved a known-good .asi -> $Pristine" -ForegroundColor DarkGray
  }
}
function Restore-Pristine {
  if (Test-Path $Pristine) {
    Copy-Item $Pristine $BuiltAsi -Force
    Copy-Item $Pristine $Deployed -Force
    Write-Host "[pristine] restored to BOTH build\ and original\" -ForegroundColor Green
  } else { Write-Warning "no pristine backup to restore" }
}

if ($Restore) { Restore-Pristine; return }

# ---- select TUs -------------------------------------------------------------
$all = Get-Content $Rsp | ForEach-Object { $_.Trim().Trim('"') } | Where-Object { $_ -match '\.cpp$' }
Write-Host "asi_sources.rsp holds $($all.Count) TUs"

if ($ListFile) {
  $sel = Get-Content $ListFile | ForEach-Object { $_.Trim().Trim('"') } | Where-Object { $_ }
} elseif ($Count -ge 0 -and $Count -lt $all.Count) {
  $sel = $all[0..([Math]::Max($Count-1,0))]
  if ($Count -eq 0) { $sel = $all }   # 0 = full set (baseline)
} else {
  $sel = $all
}
if ($Exclude) {
  $before = $sel.Count
  $sel = $sel | Where-Object { $_ -notmatch $Exclude }
  Write-Host "excluded $($before - $sel.Count) TU(s) matching /$Exclude/"
}
# dll_main.cpp + Core\HookSystem.cpp are structural - a .asi without them exports nothing
foreach ($must in @('dll_main.cpp','Core\HookSystem.cpp')) {
  if ($sel -notcontains $must -and $all -contains $must) { $sel = @($must) + $sel; Write-Host "[force] re-added required $must" }
}
Write-Host "building with $($sel.Count) TU(s)" -ForegroundColor Cyan

Save-Pristine

# ---- build ------------------------------------------------------------------
$subRsp = Join-Path $env:TEMP 'asi_bisect_subset.rsp'
($sel | ForEach-Object { '"' + $_ + '"' }) -join "`r`n" | Set-Content $subRsp -Encoding ASCII

$extraObj = if (($sel -match 'Collision\\') -and (Test-Path $QhullObj)) { "`"$QhullObj`"" } else { '' }
$linkLib  = if (($sel -match 'Collision\\') -and (Test-Path $QhullLib)) { "`"$QhullLib`"" } else { '' }

$bat = Join-Path $env:TEMP 'asi_bisect_build.bat'
@"
@echo off
call "$VcVars" >nul || exit /b 1
pushd "$Src"
cl /nologo /EHsc /W3 /O2 /LD /Fo"$Out\\" /Fe"$BuiltAsi" @"$subRsp" $extraObj /link /DLL /MAP:"$Out\mashed_re_dev.map" /MAPINFO:EXPORTS $linkLib
set RC=%errorlevel%
popd
exit /b %RC%
"@ | Set-Content $bat -Encoding ASCII

Write-Host "[build] ..." -NoNewline
$log = & cmd /c "`"$bat`" 2>&1"
if ($LASTEXITCODE -ne 0) {
  Write-Host " FAILED" -ForegroundColor Red
  $log | Select-Object -Last 15
  Write-Host "`n[!] BUILD FAILURE IS NOT A BISECT RESULT - the subset is not self-contained." -ForegroundColor Yellow
  Write-Host "    Widen the subset (missing dependency) rather than recording this as BOOTS/AV." -ForegroundColor Yellow
  Restore-Pristine
  exit 2
}
Write-Host " OK"
Copy-Item $BuiltAsi $Deployed -Force   # footgun #1: both locations must match

# ---- boot test --------------------------------------------------------------
$exe = Join-Path $Repo 'original\MASHED.exe'
$wd  = Join-Path $Repo 'original'
$av = 0; $ok = 0
for ($i=1; $i -le $Runs; $i++) {
  $p = Start-Process -FilePath $exe -WorkingDirectory $wd -PassThru
  $sw = [Diagnostics.Stopwatch]::StartNew()
  $exited = $p.WaitForExit($Seconds * 1000)      # footgun #2: ALWAYS WaitForExit first
  $t = [math]::Round($sw.Elapsed.TotalSeconds,2)
  if ($exited) {
    $code = $p.ExitCode
    $tag = if ($code -eq 0xC0000005 -or $code -eq -1073741819) { 'AV 0xC0000005' } else { "exit 0x{0:X8}" -f $code }
    Write-Host ("  run {0}: DIED {1,6}s  {2}" -f $i,$t,$tag) -ForegroundColor Red
    $av++
  } else {
    Write-Host ("  run {0}: BOOTS {1,6}s (alive at threshold)" -f $i,$t) -ForegroundColor Green
    Stop-Process -Id $p.Id -Force              # only ever our own pid
    $ok++
  }
  Start-Sleep -Seconds 2
}

$verdict = if ($av -gt 0) { 'BAD  (contains the crasher)' } else { 'GOOD (no crash in this subset)' }
Write-Host "`nVERDICT: $verdict   [$ok booted / $av died over $Runs runs, $($sel.Count) TUs]" -ForegroundColor Cyan
Write-Host "restore the known-good .asi with:  pwsh scripts\bisect_asi_boot.ps1 -Restore"
