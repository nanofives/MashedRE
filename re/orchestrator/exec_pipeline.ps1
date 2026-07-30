#requires -Version 7.0
<#
.SYNOPSIS
  claude3 execution-tier pipeline — the NARROW, machine-bound half of the
  two-tier orchestration. Runs a queue of build/verify jobs SERIALIZED under a
  machine lock (so no two game-spawning jobs overlap), parses verdicts, and
  emits a promotion-ready report for the human/re-classify skill to finalize.

  WHY SERIALIZED (not fanned out like the read-fleet): every job here touches a
  SHARED machine resource — the one GPU + game process (concurrent boots contend;
  measured 1.12x for 2), the single build tree / .asi, or the git working tree.
  Fan-out does not help and collides (simultaneous frida.spawn races). So the
  narrow tier processes one job at a time and guards with a machine lock.

  WHAT IT AUTOMATES: build.bat, run_diff_scenario_batch.py, and the batch Stalker
  sweep — all shell-scriptable, no claude session needed. WHAT IT DOES NOT DO:
  authoring (brief -> .cpp, a judgment step) and re-classify/commit (evidence-
  gated skill). Those stay with the human. This produces the evidence + a
  candidate list; promotion is a separate, deliberate step.

.EXAMPLE
  pwsh -File re/orchestrator/exec_pipeline.ps1 -Queue re/orchestrator/exec_pipeline/exec_queue.json
#>
[CmdletBinding()]
param(
  [Parameter(Mandatory)][string]$Queue,
  [string]$OutDir,
  [switch]$Force   # break a stale machine lock whose owner PID is dead
)

$ErrorActionPreference = 'Stop'
$ROOT = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent   # ...\Mashed
$LOCK = Join-Path $PSScriptRoot 'exec_pipeline\.machine.lock'
$BUILD = Join-Path $ROOT 'mashedmod\build.bat'
$PY = 'py'; $PYV = '-3.12'
$BATCH = Join-Path $ROOT 're\frida\run_diff_scenario_batch.py'
$STALKER = Join-Path $ROOT 're\frida\stalker_write_surface_batch.py'

# ---- machine lock: serialize all game-spawning / build work -----------------
function Acquire-MachineLock {
  New-Item -ItemType Directory -Force -Path (Split-Path $LOCK) | Out-Null
  if (Test-Path $LOCK) {
    $owner = Get-Content $LOCK -Raw | ConvertFrom-Json
    $alive = $null -ne (Get-Process -Id $owner.pid -ErrorAction SilentlyContinue)
    if ($alive -and -not $Force) {
      throw "machine lock held by pid=$($owner.pid) since $($owner.at). Another machine-bound run is active. Wait, or -Force if that PID is dead."
    }
    Write-Host "  breaking stale machine lock (pid=$($owner.pid), alive=$alive)" -ForegroundColor Yellow
  }
  @{ pid = $PID; at = (Get-Date).ToString('o') } | ConvertTo-Json | Set-Content -LiteralPath $LOCK
}
function Release-MachineLock { Remove-Item -LiteralPath $LOCK -ErrorAction SilentlyContinue }

# ---- job runners ------------------------------------------------------------
function Run-Build($job, $log) {
  Write-Host "  [build] mashedmod\build.bat ..." -ForegroundColor Yellow
  $out = & cmd /c "`"$BUILD`"" 2>&1 | Out-String
  Set-Content -LiteralPath $log -Value $out -Encoding utf8
  $ok = $out -match '=== Build OK ==='
  Write-Host ("  [build] {0}" -f $(if($ok){'OK'}else{'FAILED'})) -ForegroundColor $(if($ok){'Green'}else{'Red'})
  return [pscustomobject]@{ type='build'; ok=$ok; log=$log }
}

function Run-StateBatch($job, $log) {
  $hooks = @($job.hooks)
  $args = @($PYV, $BATCH) + $hooks + @('--scenario', $job.scenario, '--dwell', [string]$job.dwell)
  if ($job.round)    { $args += @('--round', [string]$job.round) }
  if ($job.sentinel) { $args += @('--sentinel', $job.sentinel) }
  if ($job.repeat_first) { $args += '--repeat-first' }
  Write-Host ("  [state_batch {0}] {1} hooks, one boot ..." -f $job.id, $hooks.Count) -ForegroundColor Yellow
  $out = & $PY @args 2>&1 | Out-String
  Set-Content -LiteralPath $log -Value $out -Encoding utf8
  # parse the STATE-REUSE verdict + per-hook GREEN lines
  $green = @(); $inconc = @(); $red = @()
  foreach ($line in ($out -split "`r?`n")) {
    if ($line -match '^\s*\d+\s+(\S+)\s+GREEN')                    { $green  += $Matches[1] }
    elseif ($line -match '^\s*\d+\s+(\S+)\s+INCONCLUSIVE')         { $inconc += $Matches[1] }
    elseif ($line -match '^\s*\d+\s+(\S+)\s+RED')                  { $red    += $Matches[1] }
  }
  $tally = if ($out -match 'GREEN:\s*(\d+)/(\d+)') { "$($Matches[1])/$($Matches[2])" } else { '?' }
  Write-Host ("  [state_batch {0}] GREEN {1}  candidates: {2}" -f $job.id, $tally, ($green -join ', ')) -ForegroundColor Green
  return [pscustomobject]@{
    type='state_batch'; id=$job.id; green_tally=$tally;
    promotion_candidates=$green; inconclusive=$inconc; red=$red; log=$log
  }
}

function Run-StalkerBatch($job, $log) {
  $args = @($PYV, $STALKER)
  if ($job.targets_file) { $args += @('--file', $job.targets_file) }
  else { $args += @($job.targets) }
  $args += @('--dwell', [string]($job.dwell | ForEach-Object { if($_){$_}else{18} }))
  Write-Host ("  [stalker_batch {0}] sweep ..." -f $job.id) -ForegroundColor Yellow
  $out = & $PY @args 2>&1 | Out-String
  Set-Content -LiteralPath $log -Value $out -Encoding utf8
  $cap = if ($out -match 'captured:\s*(\d+)') { [int]$Matches[1] } else { 0 }
  $reach = if ($out -match 'REACHABLE-now[^:]*:\s*(\d+)') { [int]$Matches[1] } else { 0 }
  Write-Host ("  [stalker_batch {0}] captured={1} reachable-now={2}" -f $job.id, $cap, $reach) -ForegroundColor Green
  return [pscustomobject]@{ type='stalker_batch'; id=$job.id; captured=$cap; reachable_now=$reach; log=$log }
}

# ---- pipeline ---------------------------------------------------------------
$q = Get-Content -LiteralPath $Queue -Raw | ConvertFrom-Json
if (-not $OutDir) {
  $stamp = (Get-Date).ToString('yyyyMMdd_HHmmss')
  $OutDir = Join-Path $ROOT "re/orchestrator/exec_pipeline/runs/$stamp"
}
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

Write-Host "exec-pipeline: $($q.jobs.Count) jobs (SERIALIZED under machine lock)" -ForegroundColor Cyan
Write-Host "  outdir: $OutDir" -ForegroundColor DarkGray
Acquire-MachineLock
$results = @()
try {
  $i = 0
  foreach ($job in $q.jobs) {
    $i++
    $log = Join-Path $OutDir ("{0:d2}_{1}.log" -f $i, ($job.id ?? $job.type))
    switch ($job.type) {
      'build'         { $results += Run-Build $job $log }
      'state_batch'   { $results += Run-StateBatch $job $log }
      'stalker_batch' { $results += Run-StalkerBatch $job $log }
      default         { Write-Host "  [skip] unknown job type '$($job.type)'" -ForegroundColor Red }
    }
    # a failed build stops the pipeline — later verify jobs need the .asi
    if ($job.type -eq 'build' -and -not $results[-1].ok) {
      Write-Host "  build failed — halting pipeline" -ForegroundColor Red; break
    }
  }
} finally {
  Release-MachineLock
}

# ---- report -----------------------------------------------------------------
$candidates = @($results | Where-Object { $_.type -eq 'state_batch' } | ForEach-Object { $_.promotion_candidates } | Where-Object { $_ })
$manifest = Join-Path $OutDir 'manifest.json'
[pscustomobject]@{
  queue = $Queue; outdir = $OutDir; jobs = $results.Count
  promotion_candidates = $candidates
  results = $results
} | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $manifest -Encoding utf8

Write-Host ""
Write-Host ("exec-pipeline DONE. promotion candidates (GREEN, ready for re-classify): {0}" -f `
            $(if($candidates){$candidates -join ', '}else{'(none)'})) -ForegroundColor Cyan
Write-Host "  manifest: $manifest" -ForegroundColor DarkGray
Write-Host "  NEXT (human/skill): verify evidence, then re-classify + commit the GREEN hooks." -ForegroundColor DarkGray
