#requires -Version 7.0
<#
.SYNOPSIS
  Roadmap-loop merge train (ROADMAP_LOOP_PLAN.md section 6). Takes 1-3 queued branches
  from the live merge_queue.json, merges them in a TEMP worktree, builds both targets,
  runs lint_rva_bodies.py (0 NEW) and the default gates (G-CTRL + B unchanged), adds the
  full gate set (solo) when a branch touches Vehicle/ Ai/ Collision/, and fast-forwards
  orch/integration only on GREEN. A RED batch is re-merged one branch at a time to find
  the culprit, which is bounced to stage red-merge. Never pushes, never deploys.

  Config: re/orchestrator/merge_train.json. Ledger: roadmap.ps1 (live dir log/orch).

.EXAMPLE
  pwsh -NoProfile -File re/orchestrator/merge_train.ps1            # one batch
  pwsh -NoProfile -File re/orchestrator/merge_train.ps1 -Full      # force build+gates on text-only
  pwsh -NoProfile -File re/orchestrator/merge_train.ps1 -Plan      # print what it would merge, change nothing
#>
[CmdletBinding()]
param([switch]$Full, [switch]$Plan, [int]$Max = 0, [switch]$KeepTemp)
$ErrorActionPreference = 'Stop'

$HERE = $PSScriptRoot
$MAIN = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$CFG = Get-Content (Join-Path $HERE 'merge_train.json') -Raw | ConvertFrom-Json
$RM = Join-Path $HERE 'roadmap.ps1'
$LOCKPY = Join-Path $HERE 'mashed_lock.py'
$TS = Get-Date -Format 'yyyyMMdd_HHmmss'
$DATA = Join-Path $CFG.data $TS
New-Item -ItemType Directory -Force -Path $DATA | Out-Null
$LOG = Join-Path $DATA 'merge_train.log'

function Say([string]$m) { $l = "[mtrain $(Get-Date -Format HH:mm:ss)] $m"; Write-Host $l; Add-Content -LiteralPath $LOG -Value $l }
function Git {
  param([Parameter(ValueFromRemainingArguments)]$a)
  if ($a[0] -ne '-C') { $a = @('-C', $MAIN) + $a }
  $o = & git.exe @a 2>&1; $script:GITRC = $LASTEXITCODE; return $o
}
function Ledger { param([Parameter(ValueFromRemainingArguments)]$a) & pwsh -NoProfile -File $RM @a | ForEach-Object { Say "ledger: $_" } }
function Sha8([string]$p) { if (Test-Path $p) { (Get-FileHash -Algorithm SHA256 $p).Hash.Substring(0, 8).ToLower() } else { 'MISSING' } }
function Like-Any([string]$f, $globs) { foreach ($g in $globs) { if ($f -like $g) { return $true } } return $false }

# ---- 1. pick the batch -------------------------------------------------------------
$queue = @((& pwsh -NoProfile -File $RM queue) | Out-String | ConvertFrom-Json -DateKind String)
if (-not $queue -or $queue.Count -eq 0) { Say 'queue empty'; Write-Output "REPORT merge-train`nverdict: GREEN`nbranch: (nothing queued)`nnext: none"; exit 0 }
$cap = if ($Max -gt 0) { $Max } else { [int]$CFG.batch_max }
$batch = @()
foreach ($q in $queue) {
  if ($q.solo) { if ($batch.Count -eq 0) { $batch = @($q) }; break }   # a solo branch merges alone
  $batch += $q
  if ($batch.Count -ge $cap) { break }
}
$INT = if ($env:MASHED_MTRAIN_INT) { $env:MASHED_MTRAIN_INT } else { $CFG.integration_branch }
$base = (Git rev-parse $INT) | Select-Object -First 1
Say "batch: $(($batch | ForEach-Object { "$($_.id)=$($_.branch)" }) -join ' ')  base $INT@$($base.Substring(0,9))"
if ($Plan) { exit 0 }

# ---- helpers -------------------------------------------------------------------------
function New-Temp([string]$at) {
  $p = Join-Path $MAIN ".worktrees\mtrain-$TS-$([guid]::NewGuid().ToString('N').Substring(0,4))"
  Git -C $MAIN worktree add --detach $p $at | Out-Null
  if ($script:GITRC -ne 0) { throw "worktree add failed: $p" }
  return $p
}
function Remove-Temp([string]$p) {
  if ($KeepTemp -or -not $p) { return }
  # SAFE removal only (never `git worktree remove --force`; WORKTREE-SYMLINK-WIPE)
  Push-Location $MAIN
  try { $o = & py.exe -3.12 scripts\diag.py wt-remove $p 2>&1; Say "wt-remove $p rc=$LASTEXITCODE $($o | Select-Object -Last 1)" } finally { Pop-Location }
}
function Merge-One([string]$wt, $item) {
  $msg = "Merge $($item.branch) ($($item.id)) via merge train"
  $o = Git -C $wt -c merge.autoStash=false merge --no-ff -m $msg $item.branch
  if ($script:GITRC -eq 0) { return @{ ok = $true } }
  $conf = @((Git -C $wt diff --name-only --diff-filter=U) | Where-Object { $_ })
  $halt = @($conf | Where-Object { $_ -notin $CFG.conflict_union })
  if ($halt.Count -gt 0) {
    Git -C $wt merge --abort | Out-Null
    return @{ ok = $false; why = "conflict needs a hand merge (frida-sweep recipe): $($halt -join ', ')" }
  }
  foreach ($f in $conf) {   # frida-sweep rule for these files: keep both sides
    $tmp = Join-Path $DATA ("u_" + ($f -replace '[\\/]', '_'))
    foreach ($st in 1, 2, 3) { (Git -C $wt show ":${st}:$f") | Set-Content -LiteralPath "$tmp.$st" -Encoding utf8 }
    & git.exe merge-file --union "$tmp.2" "$tmp.1" "$tmp.3"
    Copy-Item "$tmp.2" (Join-Path $wt $f) -Force
    Git -C $wt add -- $f | Out-Null
  }
  Git -C $wt -c core.editor=true commit --no-edit -q | Out-Null
  if ($script:GITRC -ne 0) { Git -C $wt merge --abort | Out-Null; return @{ ok = $false; why = "union-resolve commit failed" } }
  Say "  union-resolved: $($conf -join ', ')"
  return @{ ok = $true }
}
function Ensure-Root([string]$root) {
  if (Test-Path (Join-Path $root 'original\MASHED.exe')) { return }
  Say "creating private root $root (robocopy of original\)"
  & robocopy (Join-Path $MAIN 'original') (Join-Path $root 'original') /E /NFL /NDL /NJH /NJS /NP | Out-Null
  if ($LASTEXITCODE -ge 8) { throw "robocopy into $root failed rc=$LASTEXITCODE" }
}
function Run-Arms([string]$wt, [string[]]$arms, [string]$out, [switch]$Solo) {
  New-Item -ItemType Directory -Force -Path $out | Out-Null
  $env:A4_EXE = Join-Path $wt 'mashedmod\build\mashed_re.exe'
  $env:A4_ROOT = $CFG.root; $env:A4_OUT = $out
  $runner = Join-Path $wt $CFG.runner
  $lockArgs = @('run', '--label', "merge-train:$TS", '--wait', '7200')
  if ($Solo) { $lockArgs += '--solo' }
  & py.exe -3.12 $LOCKPY @lockArgs -- py.exe -3.12 $runner @arms 2>&1 | ForEach-Object { Add-Content -LiteralPath $LOG -Value $_ }
  return $LASTEXITCODE
}
function Gate-Default([string]$wt) {
  $out = Join-Path $DATA 'arms'
  $g = $CFG.gctrl; $b = $CFG.b
  $res = @{ ok = $true; text = '' }
  $tries = 0
  do {
    $arm = if ($tries -eq 0) { $g.arm } else { "$($g.arm)-$($tries + 1)" }
    Run-Arms $wt @($arm) $out | Out-Null
    $h = @($g.files | ForEach-Object { Sha8 (Join-Path $out ($_ -replace "^$([regex]::Escape($g.arm))", $arm)) })
    $gok = (($h -join '/') -eq ($g.ref -join '/'))
    $tries++
  } while (-not $gok -and $tries -le [int]$g.retries)   # a G-CTRL run can drop one frame's rows: rerun first
  Run-Arms $wt $b.arms $out | Out-Null
  $hb = @($b.arms | ForEach-Object { Sha8 (Join-Path $out "$_.step.csv") })
  $bok = (($hb -join '/') -eq ($b.ref -join '/'))
  $res.ok = $gok -and $bok
  $res.text = "G-CTRL $($h -join '/')$(if($gok){''}else{' (ref ' + ($g.ref -join '/') + ')'})  B $($hb -join '/')$(if($bok){''}else{' (ref ' + ($b.ref -join '/') + ')'})"
  return $res
}
function Gate-Full([string]$wt) {
  $f = $CFG.full; $out = Join-Path $DATA 'arms'; $notes = @(); $ok = $true
  # (b) 9/9 and (e) 3/3 read the B arms already run by Gate-Default
  $steps = @($CFG.b.arms | ForEach-Object { Join-Path $out "$_.step.csv" })
  $v = & py.exe -3.12 (Join-Path $wt $f.vsum) @steps 2>&1
  $v | Set-Content (Join-Path $DATA 'vsum_b.txt')
  $nb = @($v | Where-Object { $_ -match '\s3/3\s' }).Count
  if ($nb -ne 3) { $ok = $false }; $notes += "(b) $nb/3 arms 3/3"
  $e = & py.exe -3.12 (Join-Path $wt $f.ai_speed_env) --check $steps[0] 2>&1
  $e | Set-Content (Join-Path $DATA 'e_check.txt')
  $ne = @($e | Where-Object { $_ -match '\(e\) PASS' }).Count
  $launch = ($f.e_launch | ForEach-Object { [bool]($e -match "launch=$([regex]::Escape($_))") }) -notcontains $false
  if ($ne -ne 3 -or -not $launch) { $ok = $false }; $notes += "(e) $ne/3 launch $(if($launch){'same'}else{'CHANGED'})"
  # D2 3/3 inside the solo window
  Run-Arms $wt $f.d2_arms $out -Solo | Out-Null
  $logs = @($f.d2_arms | ForEach-Object { Join-Path $out "$_.motion_diag.log" })
  $short = @($logs | Where-Object { -not (Test-Path $_) -or @(Get-Content $_).Count -lt [int]$f.d2_min_lines })
  $d = & py.exe -3.12 (Join-Path $wt $f.d2_score) port @logs 2>&1
  $d | Set-Content (Join-Path $DATA 'score_d2.txt')
  $nd = @($d | Where-Object { $_ -match 'drvmed=\S+ in' -and $_ -notmatch 'OUT|=NA' }).Count
  if ($nd -ne 3 -or $short.Count) { $ok = $false }; $notes += "D2 $nd/3$(if($short.Count){" ($($short.Count) arm(s) under $($f.d2_min_lines) lines: rerun, not RED)"})"
  # 13 tracks: table identical to the reference
  Ensure-Root $CFG.root13
  $o13 = Join-Path $DATA 'tracks'
  $tok = & py.exe -3.12 $LOCKPY solo-begin --label "merge-train13:$TS" --ttl 14400 | Where-Object { $_ -match '^TOKEN ' }
  $env:MASHED_LOCK_SOLO_TOKEN = ($tok -replace '^TOKEN ', '')
  try {
    & py.exe -3.12 (Join-Path $wt $f.run13) run --exe (Join-Path $wt 'mashedmod\build\mashed_re.exe') --root $CFG.root13 --out $o13 2>&1 | ForEach-Object { Add-Content -LiteralPath $LOG -Value $_ }
  } finally { & py.exe -3.12 $LOCKPY solo-end --token $env:MASHED_LOCK_SOLO_TOKEN | Out-Null; $env:MASHED_LOCK_SOLO_TOKEN = $null }
  $s13 = & py.exe -3.12 (Join-Path $wt $f.run13) score --out $o13 --cite (Join-Path $wt $f.run13_cites) 2>&1
  $s13 | Set-Content (Join-Path $DATA 'tracks_score.txt')
  $ref = Get-Content (Join-Path $wt $f.run13_reference)
  $same = (($s13 | Where-Object { $_ -match '^\d\d_' }) -join "`n") -eq (($ref | Where-Object { $_ -match '^\d\d_' }) -join "`n")
  $ig = [bool]($s13 -match 'INSTRUMENT GATES: PASS')
  if (-not ($same -and $ig)) { $ok = $false }; $notes += "13 tracks $(if($same){'identical'}else{'DIFFER'}) gates $(if($ig){'PASS'}else{'FAIL'})"
  return @{ ok = $ok; text = ($notes -join '; ') }
}
function Try-Batch($items) {
  $wt = New-Temp $base
  $r = @{ ok = $true; wt = $wt; why = ''; gates = 'not run'; sha = '' ; bad = $null }
  try {
    foreach ($it in $items) {
      $m = Merge-One $wt $it
      if (-not $m.ok) { $r.ok = $false; $r.why = $m.why; $r.bad = $it; return $r }
      Say "  merged $($it.id) ($($it.branch))"
    }
    $changed = @((Git -C $wt diff --name-only "$base..HEAD") | Where-Object { $_ })
    $textOnly = -not $Full -and (@($changed | Where-Object { -not (Like-Any $_ $CFG.text_only_globs) }).Count -eq 0)
    $needFull = @($changed | Where-Object { $f = $_; @($CFG.full_set_paths | Where-Object { $f.StartsWith($_) }).Count -gt 0 }).Count -gt 0
    Say "  changed $($changed.Count) file(s); text-only=$textOnly full-set=$needFull"
    if ($textOnly) { $r.gates = 'not run: text-only change (no build input touched)'; return $r }
    # build both targets, never deploy
    $blog = Join-Path $DATA 'build.log'
    $env:MASHED_NO_DEPLOY = '1'
    Push-Location $wt
    try { & cmd /c "call mashedmod\build.bat > `"$blog`" 2>&1"; $brc = $LASTEXITCODE } finally { Pop-Location; Remove-Item env:MASHED_NO_DEPLOY -ErrorAction SilentlyContinue }
    $exe = Join-Path $wt 'mashedmod\build\mashed_re.exe'; $asi = Join-Path $wt 'mashedmod\build\mashed_re_dev.asi'
    if ($brc -ne 0 -or -not (Test-Path $exe) -or -not (Test-Path $asi)) { $r.ok = $false; $r.why = "build failed rc=$brc ($blog)"; return $r }
    Say "  build OK exe=$(Sha8 $exe) asi=$(Sha8 $asi)"
    Push-Location $wt
    try { $lint = & py.exe -3.12 scripts\lint_rva_bodies.py 2>&1; $lrc = $LASTEXITCODE } finally { Pop-Location }
    $lint | Set-Content (Join-Path $DATA 'lint.txt')
    if ($lrc -ne 0) { $r.ok = $false; $r.why = "lint_rva_bodies: $(($lint | Select-Object -Last 1))"; return $r }
    Say "  lint OK: $(($lint | Select-Object -Last 1))"
    Ensure-Root $CFG.root
    $gd = Gate-Default $wt
    $r.gates = $gd.text; Say "  gates: $($gd.text)"
    if (-not $gd.ok) { $r.ok = $false; $r.why = "default gates changed"; return $r }
    if ($needFull) {
      $gf = Gate-Full $wt
      $r.gates += "; $($gf.text)"; Say "  full set: $($gf.text)"
      if (-not $gf.ok) { $r.ok = $false; $r.why = "full gate set"; return $r }
    }
    return $r
  } finally {
    $r.sha = (Git -C $wt rev-parse HEAD) | Select-Object -First 1
  }
}

# ---- 2. run the batch; on RED, one at a time ----------------------------------------
$res = Try-Batch $batch
$merged = @(); $bounced = @()
if ($res.ok) { $merged = $batch }
else {
  Remove-Temp $res.wt
  if ($batch.Count -eq 1) { $bounced = @(@{ it = $batch[0]; why = $res.why; gates = $res.gates }) ; $res = $null }
  else {
    Say "batch RED ($($res.why)); re-merging one at a time"
    $res = $null
    foreach ($it in $batch) {
      $one = Try-Batch @($it)
      if ($one.ok -and -not $res) { $res = $one; $merged = @($it); continue }   # first GREEN one lands this run
      if (-not $one.ok) { $bounced += @{ it = $it; why = $one.why; gates = $one.gates } }
      Remove-Temp $one.wt
    }
  }
}
foreach ($b in $bounced) {
  Ledger set $b.it.id red-merge "merge train $TS`: $($b.why)"
  Ledger dequeue $b.it.id
}

# ---- 3. fast-forward orch/integration on GREEN --------------------------------------
$verdict = if ($bounced.Count -or -not $merged.Count) { 'RED' } else { 'GREEN' }
$tip = (Git rev-parse $INT) | Select-Object -First 1
if ($merged.Count) {
  $wt = $res.wt
  if ($tip -ne $base) {
    $moved = @((Git diff --name-only "$base..$tip") | Where-Object { $_ })
    $foreign = @($moved | Where-Object { $_ -notlike 're/orchestrator/ledger/*' -and $_ -ne 're/orchestrator/DECISIONS.md' -and $_ -ne 're/orchestrator/INTEGRATION_LOG.md' })
    if ($foreign.Count) { Say "$INT moved with code changes ($($foreign -join ', ')); not fast-forwarding"; $verdict = 'RED'; $merged = @() }
    else { Git -C $wt -c merge.autoStash=false merge --no-edit -q $tip | Out-Null; Say "  folded ledger-only tip $($tip.Substring(0,9))" }
  }
}
if ($merged.Count) {
  $line = "- $(Get-Date -Format 'yyyy-MM-dd HH:mm') $(($merged | ForEach-Object { "$($_.id) ($($_.branch))" }) -join ', '): $($res.gates). Run data $DATA"
  $il = Join-Path $wt 're\orchestrator\INTEGRATION_LOG.md'
  if (-not (Test-Path $il)) { Set-Content $il "# orch/integration log (merge train, newest first)`n" -Encoding utf8 }
  $body = Get-Content $il; $new = @($body[0], '', $line) + @($body | Select-Object -Skip 1 | Where-Object { $_ -ne '' })
  $new | Set-Content $il -Encoding utf8
  Git -C $wt add -- 're/orchestrator/INTEGRATION_LOG.md' | Out-Null
  Git -C $wt commit -q -m "orch: integration log, $(($merged | ForEach-Object id) -join ', ')" -m "Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>" | Out-Null
  $sha = (Git -C $wt rev-parse HEAD) | Select-Object -First 1
  # ff where the branch is checked out; atomic compare-and-swap of the ref if it is not
  $wl = (Git -C $MAIN worktree list --porcelain) -join "`n"
  $held = [regex]::Matches($wl, "worktree (.+)\nHEAD \w+\nbranch refs/heads/$([regex]::Escape($INT))(\n|$)")
  if ($held.Count) { $ff = Git -C $held[0].Groups[1].Value -c merge.autoStash=false merge --ff-only -q $sha }
  else { $ff = Git -C $MAIN update-ref "refs/heads/$INT" $sha $tip }
  if ($script:GITRC -ne 0) { Say "ff of $INT failed: $ff"; $verdict = 'RED'; $merged = @() }
  else {
    Say "fast-forwarded $INT -> $($sha.Substring(0,9))"
    foreach ($it in $merged) { Ledger set $it.id merged "merge train $TS`: $($res.gates)" -Sha $sha; Ledger dequeue $it.id }
  }
  Remove-Temp $wt
}

$final = (Git rev-parse --short $INT) | Select-Object -First 1
$report = @(
  "REPORT merge-train",
  "verdict: $verdict",
  "branch: $INT @ $final",
  "evidence: $LOG",
  "gates: $(if ($res) { $res.gates } else { ($bounced | ForEach-Object { $_.gates }) -join ' | ' })",
  "decision: none",
  "next: merged [$(($merged | ForEach-Object id) -join ' ')] bounced [$(($bounced | ForEach-Object { "$($_.it.id): $($_.why)" }) -join '; ')]"
)
$report | Set-Content (Join-Path $DATA 'REPORT.txt') -Encoding utf8
$report | Write-Output
