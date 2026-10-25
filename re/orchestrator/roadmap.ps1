#requires -Version 7.0
<#
.SYNOPSIS
  Roadmap-loop ledger (ROADMAP_LOOP_PLAN.md section 4). One live ledger at one absolute
  path that every tier and every worktree uses. Separate from state.json (the per-hook
  /orchestrate ledger). Children write their own transitions through this script.

  Live dir: $env:MASHED_ORCH_STATE, default C:\Users\maria\Desktop\Proyectos\Mashed\log\orch
  (gitignored). Holds roadmap.json, merge_queue.json, DECISIONS.md, events.jsonl.
  `snapshot` copies them into the orch/integration worktree and commits them there.

.EXAMPLE
  pwsh -NoProfile -File re/orchestrator/roadmap.ps1 status
  pwsh -NoProfile -File re/orchestrator/roadmap.ps1 next [-Wave 1] [-NoSolo]
  pwsh -NoProfile -File re/orchestrator/roadmap.ps1 claim D5-PDOD <session-id> [-Parent <lead-id>]
  pwsh -NoProfile -File re/orchestrator/roadmap.ps1 set D5-PDOD working "editing ROADMAP P-DoD" -Branch orch/D5-PDOD
  pwsh -NoProfile -File re/orchestrator/roadmap.ps1 enqueue D5-PDOD -Branch orch/D5-PDOD
  pwsh -NoProfile -File re/orchestrator/roadmap.ps1 decision D1-R1 "accept RESULT_d1r1 as closing R1?"
  pwsh -NoProfile -File re/orchestrator/roadmap.ps1 fleet
  pwsh -NoProfile -File re/orchestrator/roadmap.ps1 get D5-PDOD
  pwsh -NoProfile -File re/orchestrator/roadmap.ps1 snapshot "<commit message>"
#>
[CmdletBinding()]
param(
  [Parameter(Position=0)][ValidateSet('status','next','claim','set','fleet','get','enqueue','dequeue','queue','decision','add','snapshot')][string]$Cmd = 'status',
  [Parameter(Position=1)][string]$Id,
  [Parameter(Position=2)][string]$Arg2,
  [Parameter(Position=3)][string]$Arg3,
  [string]$Branch, [string]$Worktree, [string]$Evidence, [string]$Parent, [string]$Owner,
  [string]$Sha, [int]$Wave = -1, [switch]$NoSolo, [switch]$Force
)
$ErrorActionPreference = 'Stop'

$LIVE = if ($env:MASHED_ORCH_STATE) { $env:MASHED_ORCH_STATE } else { 'C:\Users\maria\Desktop\Proyectos\Mashed\log\orch' }
$ORCH_WT = if ($env:MASHED_ORCH_WT) { $env:MASHED_ORCH_WT } else { 'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration' }
$LEDGER = Join-Path $LIVE 'roadmap.json'
$QUEUE  = Join-Path $LIVE 'merge_queue.json'
$DECIDE = Join-Path $LIVE 'DECISIONS.md'
$EVENTS = Join-Path $LIVE 'events.jsonl'
$LOCK   = Join-Path $LIVE 'roadmap.lock'

$STAGES = @('open','claimed','working','review','merge-queued','merged','done','blocked','decision','red-merge','handoff')
$LIVE_STAGES = @('claimed','working','review','merge-queued','red-merge','handoff')
$FINISHED = @('merged','done')

function Now { (Get-Date).ToUniversalTime().ToString('yyyy-MM-ddTHH:mm:ssZ') }

# HB-EOL: ledger files are copied into tracked paths by `snapshot`, so write them LF (Set-Content/Add-Content give CRLF on Windows).
function Write-Lf([string]$path, [string]$text) { [IO.File]::WriteAllText($path, (($text -replace "`r`n", "`n").TrimEnd("`n") + "`n"), (New-Object Text.UTF8Encoding($false))) }
# HB-TRAINFIX: snapshot copies through this, not Copy-Item. Live DECISIONS.md / events.jsonl still hold CRLF lines from
# before HB-EOL, and a verbatim copy into the eol=lf paths made every `git add` warn. The live files are left as they are
# (rewriting them in place could drop a concurrent append); the tracked copy is LF.
function Copy-Lf([string]$src, [string]$dst) { Write-Lf $dst ([IO.File]::ReadAllText($src)) }
function Append-Lf([string]$path, [string]$text) { [IO.File]::AppendAllText($path, (($text -replace "`r`n", "`n").TrimEnd("`n") + "`n"), (New-Object Text.UTF8Encoding($false))) }

function With-Lock([scriptblock]$Body) {
  $t0 = Get-Date
  while ($true) {
    try { $fs = [System.IO.File]::Open($LOCK, 'CreateNew', 'Write'); $fs.Close(); break }
    catch [System.IO.IOException] {
      $age = ((Get-Date) - (Get-Item $LOCK -ErrorAction SilentlyContinue).LastWriteTime).TotalSeconds
      if ($age -gt 60) { Remove-Item $LOCK -Force -ErrorAction SilentlyContinue; continue }
      if (((Get-Date) - $t0).TotalSeconds -gt 30) { throw "roadmap.lock busy for 30 s ($LOCK)" }
      Start-Sleep -Milliseconds 150
    }
  }
  try { & $Body } finally { Remove-Item $LOCK -Force -ErrorAction SilentlyContinue }
}

function Load { Get-Content -LiteralPath $LEDGER -Raw | ConvertFrom-Json -DateKind String }
function Save($s) {
  $s.updated = Now
  $tmp = "$LEDGER.tmp"
  Write-Lf $tmp (($s | ConvertTo-Json -Depth 8) -join "`n")
  Move-Item -LiteralPath $tmp -Destination $LEDGER -Force
}
function Log-Event($row, $from, $to, $note) {
  $e = [ordered]@{ at = Now; id = $row.id; from = $from; to = $to; owner = $row.owner; note = $note }
  Append-Lf $EVENTS ($e | ConvertTo-Json -Compress)
}
function Row($s, $id) {
  $r = $s.rows | Where-Object { $_.id -eq $id } | Select-Object -First 1
  if (-not $r) { throw "no row '$id'" }
  return $r
}
function Ensure-Field($r, $name, $val) {
  if ($r.PSObject.Properties.Name -notcontains $name) { $r | Add-Member -NotePropertyName $name -NotePropertyValue $val }
}
function Apply-Opts($r) {
  foreach ($p in @('Branch','Worktree','Evidence','Parent','Owner','Sha')) {
    $v = Get-Variable -Name $p -ValueOnly
    if ($v) { $f = $p.ToLower(); Ensure-Field $r $f ''; $r.$f = $v }
  }
}
function Deps-Met($s, $r) {
  foreach ($d in @($r.deps)) {
    if (-not $d) { continue }
    $dr = $s.rows | Where-Object { $_.id -eq $d } | Select-Object -First 1
    if (-not $dr -or $dr.stage -notin $FINISHED) { return $false }
  }
  return $true
}
function Ready($s) {
  @($s.rows | Where-Object { $_.stage -eq 'open' -and (Deps-Met $s $_) } |
    Where-Object { $Wave -lt 0 -or $_.wave -le $Wave } |
    Where-Object { -not ($NoSolo -and $_.solo) } |
    Sort-Object wave, id)
}
function Short([string]$t, [int]$n) { if (-not $t) { '' } elseif ($t.Length -le $n) { $t } else { $t.Substring(0, $n - 1) + '~' } }
function Age([string]$iso) {
  if (-not $iso) { return '' }
  $m = [int]((Get-Date).ToUniversalTime() - [datetime]::Parse($iso).ToUniversalTime()).TotalMinutes
  if ($m -lt 120) { "${m}m" } else { "$([int]($m/60))h" }
}

if (-not (Test-Path $LEDGER) -and $Cmd -ne 'add') { throw "no ledger at $LEDGER" }

switch ($Cmd) {
  'status' {
    $s = Load
    $by = $s.rows | Group-Object stage
    $counts = ($STAGES | ForEach-Object { $g = $by | Where-Object Name -eq $_; if ($g) { "$_ $($g.Count)" } }) -join ' | '
    Write-Output "roadmap ledger $($s.updated)  rows $($s.rows.Count): $counts"
    $live = @($s.rows | Where-Object { $_.stage -in $LIVE_STAGES })
    if ($live) {
      Write-Output "live:"
      foreach ($r in $live) { Write-Output ("  {0,-20} {1,-12} {2,-10} {3,4} {4}" -f $r.id, $r.stage, (Short $r.owner 10), (Age $r.updated), (Short $r.note 60)) }
    }
    $dec = @($s.rows | Where-Object { $_.stage -eq 'decision' })
    if ($dec) { Write-Output ("decision: " + (($dec | ForEach-Object { "$($_.id)($($_.decision))" }) -join ', ')) }
    $ready = Ready $s
    $solo = @($ready | Where-Object solo).Count
    Write-Output ("ready {0} ({1} solo): {2}" -f $ready.Count, $solo, ((($ready | Select-Object -First 12) | ForEach-Object { "$($_.id)/w$($_.wave)$(if($_.solo){'*'})" }) -join ' '))
    $q = if (Test-Path $QUEUE) { @((Get-Content $QUEUE -Raw | ConvertFrom-Json -DateKind String)) } else { @() }
    Write-Output ("merge queue {0}{1}" -f $q.Count, $(if ($q.Count) { ': ' + (($q | ForEach-Object id) -join ' ') } else { '' }))
  }
  'next' {
    $s = Load
    foreach ($r in (Ready $s)) {
      Write-Output ("{0,-20} w{1} {2,-4} [{3}] {4}" -f $r.id, $r.wave, $(if($r.solo){'SOLO'}else{''}), (@($r.needs) -join ','), (Short $r.next 90))
    }
  }
  'get' { (Row (Load) $Id) | ConvertTo-Json -Depth 6 }
  'claim' {
    if (-not $Id -or -not $Arg2) { throw "usage: claim <id> <session> [-Parent <id>]" }
    With-Lock {
      $s = Load; $r = Row $s $Id
      $ok = ($r.stage -eq 'open') -or ($Force -and $r.stage -in @('handoff','red-merge','blocked'))
      if (-not $ok) { Write-Output "REFUSED $Id is '$($r.stage)' owner=$($r.owner)"; exit 1 }
      if (-not (Deps-Met $s $r) -and -not $Force) { Write-Output "REFUSED $Id deps not met: $(@($r.deps) -join ',')"; exit 1 }
      $from = $r.stage; $r.stage = 'claimed'; $r.owner = $Arg2; $r.updated = Now
      Apply-Opts $r
      Save $s; Log-Event $r $from 'claimed' ''
      Write-Output "CLAIMED $Id by $Arg2"
    }
  }
  'set' {
    if (-not $Id -or -not $Arg2) { throw "usage: set <id> <stage> [note] [-Branch -Worktree -Evidence -Parent -Owner -Sha]" }
    if ($Arg2 -notin $STAGES) { throw "stage must be one of: $($STAGES -join ', ')" }
    With-Lock {
      $s = Load; $r = Row $s $Id
      $from = $r.stage; $r.stage = $Arg2; $r.updated = Now
      if ($Arg3) { $r.note = $Arg3 }
      if ($Arg2 -eq 'open') { $r.owner = '' }
      Apply-Opts $r
      Save $s; Log-Event $r $from $Arg2 $Arg3
      Write-Output "SET $Id $from -> $Arg2"
    }
  }
  'enqueue' {
    if (-not $Id) { throw "usage: enqueue <id> -Branch <branch> [-Sha <sha>]" }
    With-Lock {
      $s = Load; $r = Row $s $Id
      $br = if ($Branch) { $Branch } elseif ($r.branch) { $r.branch } else { throw "enqueue needs -Branch" }
      $q = if (Test-Path $QUEUE) { @((Get-Content $QUEUE -Raw | ConvertFrom-Json -DateKind String)) } else { @() }
      $q = @($q | Where-Object { $_.id -ne $Id })
      $q += [pscustomobject]@{ id = $Id; branch = $br; sha = $Sha; solo = [bool]$r.solo; at = Now }
      Write-Lf $QUEUE ((ConvertTo-Json -InputObject @($q) -Depth 4) -join "`n")
      $from = $r.stage; $r.stage = 'merge-queued'; $r.branch = $br; $r.updated = Now
      Save $s; Log-Event $r $from 'merge-queued' $br
      Write-Output "ENQUEUED $Id ($br) position $($q.Count)"
    }
  }
  'dequeue' {
    if (-not $Id) { throw "usage: dequeue <id>" }
    With-Lock {
      $q = if (Test-Path $QUEUE) { @((Get-Content $QUEUE -Raw | ConvertFrom-Json -DateKind String)) } else { @() }
      $q = @($q | Where-Object { $_.id -ne $Id })
      Write-Lf $QUEUE ((ConvertTo-Json -InputObject @($q) -Depth 4) -join "`n")
      Write-Output "DEQUEUED $Id ($($q.Count) left)"
    }
  }
  'queue' {
    if (Test-Path $QUEUE) { Get-Content $QUEUE -Raw } else { '[]' }
  }
  'decision' {
    if (-not $Id -or -not $Arg2) { throw "usage: decision <id> ""<question, options, evidence path>""" }
    With-Lock {
      $s = Load; $r = Row $s $Id
      $n = 1 + @(Select-String -LiteralPath $DECIDE -Pattern '^\| L-\d+ ' -ErrorAction SilentlyContinue).Count
      $did = 'L-{0}' -f $n
      if (-not (Select-String -LiteralPath $DECIDE -Pattern '^## Loop decisions' -Quiet)) {
        Append-Lf $DECIDE "`n## Loop decisions (filed by the roadmap loop)`n`n| id | row | question / options / evidence | filed | answer |`n|---|---|---|---|---|"
      }
      Append-Lf $DECIDE ("| {0} | {1} | {2} | {3} | **open** |" -f $did, $Id, ($Arg2 -replace '\|','/'), (Now))
      $from = $r.stage; $r.stage = 'decision'; $r.decision = $did; $r.updated = Now
      Save $s; Log-Event $r $from 'decision' $did
      Write-Output "DECISION $did filed for $Id"
    }
  }
  'add' {
    # add <id> <section> "<title>"  (new step rows, e.g. from a scoping worker)
    if (-not $Id -or -not $Arg2 -or -not $Arg3) { throw "usage: add <id> <section> ""<title>"" [-Wave n]" }
    With-Lock {
      $s = Load
      if ($s.rows | Where-Object { $_.id -eq $Id }) { throw "row '$Id' exists" }
      $s.rows += [pscustomobject][ordered]@{ id = $Id; section = $Arg2; title = $Arg3; stage = 'open'; owner = ''; parent = $Parent;
        branch = ''; worktree = ''; evidence = $Evidence; deps = @(); needs = @(); solo = $false; decision = '';
        wave = $(if ($Wave -ge 0) { $Wave } else { 9 }); next = ''; note = ''; updated = Now }
      Save $s; Write-Output "ADDED $Id"
    }
  }
  'fleet' {
    $s = Load
    $own = @($s.rows | Where-Object { $_.owner -and $_.stage -notin $FINISHED + @('open') })
    if (-not $own) { Write-Output "fleet: no owned rows" }
    foreach ($r in $own) {
      Write-Output ("{0,-20} {1,-12} owner={2} parent={3} branch={4} {5}" -f $r.id, $r.stage, $r.owner, $r.parent, $r.branch, (Age $r.updated))
    }
    $lock = & py.exe -3.12 (Join-Path $PSScriptRoot 'mashed_lock.py') status 2>&1
    Write-Output "game lock: $($lock -join ' / ')"
  }
  'snapshot' {
    $dst = Join-Path $ORCH_WT 're\orchestrator\ledger'
    New-Item -ItemType Directory -Force -Path $dst | Out-Null
    foreach ($f in @($LEDGER, $QUEUE, $EVENTS)) { if (Test-Path $f) { Copy-Lf $f (Join-Path $dst (Split-Path $f -Leaf)) } }
    if (Test-Path $DECIDE) { Copy-Lf $DECIDE (Join-Path $ORCH_WT 're\orchestrator\DECISIONS.md') }
    $msg = if ($Id) { $Id } else { "orch: ledger snapshot $(Now)" }
    git -C $ORCH_WT add -- 're/orchestrator/ledger' 're/orchestrator/DECISIONS.md'
    git -C $ORCH_WT diff --cached --quiet
    if ($LASTEXITCODE -eq 0) { Write-Output "snapshot: nothing changed"; break }
    git -C $ORCH_WT -c merge.autoStash=false commit -q -m $msg -m "Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>" -- 're/orchestrator/ledger' 're/orchestrator/DECISIONS.md'
    Write-Output "snapshot committed $(git -C $ORCH_WT rev-parse --short HEAD)"
  }
}
