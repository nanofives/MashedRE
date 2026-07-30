#requires -Version 7.0
<#
.SYNOPSIS
  claude2 read-fleet supervisor — loops a queue of READ-ONLY units, each run as a
  detached delegate.ps1 worker on the Accenture (account2) worker account, with
  bounded concurrency, per-unit timeout+retry, and a cost/status manifest.

  WHY delegate.ps1 (not spawn_child): the worker has a hard -TimeoutSec, so a
  wedged worker returns TIMEOUT instead of hanging silently — the ~50% silent
  wedge that makes the spawn_child transport fragile (memory
  feedback-claude2-fleet-spawn-lessons) cannot swallow a unit here. Workers are
  launched fully DETACHED (Start-Process), so they survive this supervisor's
  teardown; a 0-byte -Save file is treated as failure, never success.

  Read-only ONLY. Every queue unit must be pure Read/Grep/Glob — account2
  prompt-gates (and hangs on) writes/builds/MCP. Route those to claude3.

.EXAMPLE
  pwsh -File re/orchestrator/read_fleet.ps1 -Queue re/orchestrator/read_fleet/queue.json
.EXAMPLE
  pwsh -File re/orchestrator/read_fleet.ps1 -Queue q.json -MaxConcurrent 3 -MaxRetries 1
#>
[CmdletBinding()]
param(
  [Parameter(Mandatory)][string]$Queue,
  [int]$MaxConcurrent = 3,
  [int]$MaxRetries    = 1,
  [string]$OutDir,
  [int]$StaggerSec    = 2,
  [int]$PollSec       = 5,
  [switch]$NoPreflight   # disable the read-only preflight (not recommended)
)

$ErrorActionPreference = 'Stop'
$ROOT = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent   # ...\Mashed
$DELEGATE = 'C:\Users\maria\Desktop\Proyectos\.claude\skills\repo-fleet\scripts\delegate.ps1'
if (-not (Test-Path $DELEGATE)) { throw "delegate.ps1 not found at $DELEGATE" }

. "$PSScriptRoot\preflight.ps1"   # Test-ReadOnlyPrompt (unit-tested separately)

$q = Get-Content -LiteralPath $Queue -Raw | ConvertFrom-Json
$defaults = $q.defaults
if (-not $OutDir) {
  $stamp = (Get-Date).ToString('yyyyMMdd_HHmmss')
  $OutDir = Join-Path $ROOT "re/orchestrator/read_fleet/runs/$stamp"
}
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
$promptDir = Join-Path $OutDir 'prompts'; New-Item -ItemType Directory -Force -Path $promptDir | Out-Null

# ---- build the pending list (preflight + attempt counter) -------------------
$pending  = [System.Collections.Generic.Queue[object]]::new()
$results  = @{}     # id -> final record (rejections recorded here too)
$rejected = 0
foreach ($u in $q.units) {
  $reason = if ($NoPreflight -or $u.skip_preflight) { $null } else { Test-ReadOnlyPrompt $u.prompt }
  if ($reason) {
    $rejected++
    Write-Host ("  ✗ REJECTED [{0}] not read-only: {1}" -f $u.id, $reason) -ForegroundColor Red
    Write-Host  "      account2 hangs on this; route it to a claude3 lane (or add skip_preflight)." -ForegroundColor DarkGray
    $results[$u.id] = [pscustomobject]@{
      id = $u.id; status = 'REJECTED-NONREADONLY'; ok = $false; cost_usd = 0.0;
      secs = 0; attempts = 0; save = ''; model = ($u.model ?? $defaults.model); reason = $reason
    }
    continue
  }
  $pending.Enqueue([pscustomobject]@{
    id         = $u.id
    prompt     = $u.prompt
    model      = if ($u.model)      { $u.model }      else { $defaults.model }
    repo       = if ($u.repo)       { $u.repo }       else { $defaults.repo }
    timeoutSec = if ($u.timeoutSec) { $u.timeoutSec } else { $defaults.timeoutSec }
    attempt    = 1
  })
}

$running  = @{}     # id -> @{ unit; proc; save; status; started }
$total    = $q.units.Count
Write-Host "read-fleet: $($pending.Count) units queued ($rejected rejected by preflight), MaxConcurrent=$MaxConcurrent, MaxRetries=$MaxRetries" -ForegroundColor Cyan
Write-Host "  outdir: $OutDir" -ForegroundColor DarkGray

function Start-Unit($u) {
  $pf   = Join-Path $promptDir "$($u.id).a$($u.attempt).txt"
  $save = Join-Path $OutDir    "$($u.id).md"
  $con  = Join-Path $OutDir    "$($u.id).console.txt"
  Set-Content -LiteralPath $pf -Value $u.prompt -Encoding utf8
  # delegate.ps1 prints its status line (with cost) via Write-Host, which is the
  # Information stream — NOT stdout — so -RedirectStandardOutput would drop it.
  # Run through -Command and tee ALL streams (*>&1) into the console file so the
  # cost/status parse in Complete-Unit sees it. Detached + hidden so the worker
  # survives THIS supervisor's teardown.
  $cmd = "& '$DELEGATE' -PromptFile '$pf' -Repo '$($u.repo)' -Model '$($u.model)' " +
         "-TimeoutSec $($u.timeoutSec) -Save '$save' *>&1 | " +
         "Out-File -LiteralPath '$con' -Encoding utf8"
  $p = Start-Process pwsh -ArgumentList @('-NoProfile','-Command',$cmd) `
        -WindowStyle Hidden -PassThru
  $running[$u.id] = @{ unit=$u; proc=$p; save=$save; console=$con; started=(Get-Date) }
  Write-Host ("  → start [{0}] attempt {1} (model={2}, pid={3})" -f $u.id,$u.attempt,$u.model,$p.Id) -ForegroundColor Yellow
}

function Complete-Unit($id) {
  $r    = $running[$id]
  $u    = $r.unit
  $secs = [int]((Get-Date) - $r.started).TotalSeconds
  $ok   = (Test-Path $r.save) -and ((Get-Item $r.save).Length -gt 0)
  # pull the delegate status line (── delegate → … [OK]  … cost=$X)
  $status = 'UNKNOWN'; $cost = 0.0
  if (Test-Path $r.console) {
    $line = Select-String -Path $r.console -Pattern 'delegate →' | Select-Object -Last 1
    if ($line -and $line.Line -match '\[(\w+)\].*cost=\$?([0-9.]+)') {
      $status = $Matches[1]; $cost = [double]$Matches[2]
    }
  }
  if (-not $ok -and $status -eq 'UNKNOWN') { $status = 'NO_OUTPUT' }
  elseif ($ok -and $status -eq 'UNKNOWN') { $status = 'OK' }

  $retryable = ($status -in @('TIMEOUT','ERROR','NO_OUTPUT')) -and ($u.attempt -le $MaxRetries)
  if ($retryable) {
    Write-Host ("  ✗ [{0}] {1} in {2}s — re-queue (attempt {3})" -f $id,$status,$secs,($u.attempt+1)) -ForegroundColor Red
    $u.attempt++
    $pending.Enqueue($u)
  } else {
    $tag = if ($ok -and $status -notin @('TIMEOUT','ERROR')) { 'Green' } else { 'Red' }
    Write-Host ("  ✓ [{0}] {1}  {2}s  `${3}  -> {4}" -f $id,$status,$secs,[math]::Round($cost,4),$r.save) -ForegroundColor $tag
    $results[$id] = [pscustomobject]@{
      id=$id; status=$status; ok=$ok; cost_usd=$cost; secs=$secs;
      attempts=$u.attempt; save=$r.save; model=$u.model
    }
  }
  $running.Remove($id)
}

# ---- supervisor loop ---------------------------------------------------------
while ($pending.Count -gt 0 -or $running.Count -gt 0) {
  while ($running.Count -lt $MaxConcurrent -and $pending.Count -gt 0) {
    Start-Unit ($pending.Dequeue())
    if ($StaggerSec -gt 0) { Start-Sleep -Seconds $StaggerSec }
  }
  Start-Sleep -Seconds $PollSec
  foreach ($id in @($running.Keys)) {
    if ($running[$id].proc.HasExited) { Complete-Unit $id }
  }
}

# ---- manifest ----------------------------------------------------------------
$manifest = Join-Path $OutDir 'manifest.json'
$summary  = [pscustomobject]@{
  queue        = $Queue
  units_total  = $total
  outdir       = $OutDir
  total_cost   = [math]::Round(($results.Values | Measure-Object cost_usd -Sum).Sum, 4)
  green        = ($results.Values | Where-Object { $_.ok -and $_.status -notin @('TIMEOUT','ERROR') }).Count
  results      = @($results.Values | Sort-Object id)
}
$summary | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $manifest -Encoding utf8
Write-Host ""
Write-Host ("read-fleet DONE: {0}/{1} green, total cost `${2}" -f $summary.green,$total,$summary.total_cost) -ForegroundColor Cyan
Write-Host "  manifest: $manifest" -ForegroundColor DarkGray
Write-Host "  briefs:   $OutDir\<id>.md" -ForegroundColor DarkGray
