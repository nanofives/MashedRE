#requires -Version 7.0
<#
.SYNOPSIS
  Orchestrator ledger helper — cheap situational awareness + durable state
  transitions for the Fable-5 orchestrator, so the smart model spends tokens on
  JUDGMENT, not on reading raw files or hand-editing JSON. state.json is the
  single source of truth; this is its only writer during a run.

.EXAMPLE
  pwsh -File re/orchestrator/orch.ps1 status
  pwsh -File re/orchestrator/orch.ps1 next
  pwsh -File re/orchestrator/orch.ps1 set slot_object_field8 verified "GREEN, gate live"
  pwsh -File re/orchestrator/orch.ps1 add my_hook 0x00401234 state_getter candidate
#>
[CmdletBinding()]
param(
  [Parameter(Position=0)][ValidateSet('status','next','set','add','get')][string]$Cmd = 'status',
  [Parameter(Position=1)][string]$Id,
  [Parameter(Position=2)][string]$Arg2,
  [Parameter(Position=3)][string]$Arg3,
  [Parameter(Position=4)][string]$Arg4
)

$ErrorActionPreference = 'Stop'
$STATE = Join-Path $PSScriptRoot 'state.json'
$STAGES = @('candidate','briefed','authored','verified','promoted','blocked')

function Load { Get-Content -LiteralPath $STATE -Raw | ConvertFrom-Json }
function Save($s) {
  $s.updated = (Get-Date).ToUniversalTime().ToString('o')
  $s | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $STATE -Encoding utf8
}

# next-action per stage (what the orchestrator should DO with an item here)
$ACTION = @{
  candidate = 'brief  → read-fleet (account2, off-quota)'
  briefed   = 'author → write .cpp + registry (Fable-5 judgment)'
  authored  = 'verify → exec-pipeline (build + state_batch)'
  verified  = 'promote→ re-classify + commit (deliberate)'
  blocked   = 'review → read the note; unblock or defer'
  promoted  = '(done)'
}

$s = Load

switch ($Cmd) {
  'status' {
    Write-Host "orchestrator ledger  (updated $($s.updated))" -ForegroundColor Cyan
    foreach ($stage in $STAGES) {
      $items = @($s.items | Where-Object { $_.stage -eq $stage })
      if (-not $items) { continue }
      Write-Host ("`n[{0}]  {1} item(s)   → {2}" -f $stage, $items.Count, $ACTION[$stage]) -ForegroundColor Yellow
      foreach ($it in $items) {
        Write-Host ("    {0,-28} {1,-12} {2}" -f $it.id, $it.rva, $it.note)
      }
    }
    $active = @($s.items | Where-Object { $_.stage -notin @('promoted') })
    Write-Host ("`nsummary: {0} active, {1} promoted, {2} total" -f `
      $active.Count, @($s.items | Where-Object { $_.stage -eq 'promoted' }).Count, $s.items.Count) -ForegroundColor Cyan
  }
  'next' {
    # actionable items in pipeline order — what to work THIS iteration
    Write-Host "actionable this iteration (pipeline order):" -ForegroundColor Cyan
    foreach ($stage in @('verified','briefed','authored','candidate','blocked')) {
      $items = @($s.items | Where-Object { $_.stage -eq $stage })
      if ($items) {
        Write-Host ("  {0}: {1}" -f $ACTION[$stage], ($items.id -join ', ')) -ForegroundColor Yellow
      }
    }
  }
  'get' {
    $s.items | Where-Object { $_.id -eq $Id } | ConvertTo-Json -Depth 6
  }
  'set' {
    if (-not $Id -or -not $Arg2) { throw "usage: set <id> <stage> [note]" }
    if ($Arg2 -notin $STAGES) { throw "stage must be one of: $($STAGES -join ', ')" }
    $it = $s.items | Where-Object { $_.id -eq $Id } | Select-Object -First 1
    if (-not $it) { throw "no item with id '$Id'" }
    $old = $it.stage; $it.stage = $Arg2
    if ($Arg3) { $it.note = $Arg3 }
    Save $s
    Write-Host ("set [{0}]  {1} → {2}{3}" -f $Id, $old, $Arg2, $(if($Arg3){"  ($Arg3)"}else{''})) -ForegroundColor Green
  }
  'add' {
    if (-not $Id -or -not $Arg2) { throw "usage: add <id> <rva> [kind] [stage]" }
    if ($s.items | Where-Object { $_.id -eq $Id }) { throw "item '$Id' already exists" }
    $stage = if ($Arg4) { $Arg4 } else { 'candidate' }
    if ($stage -notin $STAGES) { throw "stage must be one of: $($STAGES -join ', ')" }
    $s.items += [pscustomobject]@{
      id = $Id; rva = $Arg2; kind = $(if($Arg3){$Arg3}else{'unknown'}); stage = $stage; note = ''
    }
    Save $s
    Write-Host ("add [{0}] {1} ({2}) @ {3}" -f $Id, $Arg2, $(if($Arg3){$Arg3}else{'unknown'}), $stage) -ForegroundColor Green
  }
}
