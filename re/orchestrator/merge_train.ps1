#requires -Version 7.0
<#
.SYNOPSIS
  Roadmap-loop merge train (ROADMAP_LOOP_PLAN.md section 6). Takes 1-3 queued branches
  from the live merge_queue.json, merges them in a TEMP worktree, builds both targets,
  runs lint_rva_bodies.py (0 NEW) and the default gates (G-CTRL + B unchanged), adds the
  full gate set (solo) when a branch touches Vehicle/ Ai/ Collision/, and fast-forwards
  orch/integration only on GREEN. A RED batch is re-merged one branch at a time to find
  the culprit, which is bounced to stage red-merge. Never pushes, never deploys.
  A pass on a machine that cannot present (display asleep / locked, D3D9 hr=0x8876086A) gives verdict INFRA instead:
  nothing bounced, rows stay queued (HB-TRAIN-INFRA-PRECHECK, see Get-ArmInfra / Run-Gates).

  Config: re/orchestrator/merge_train.json. Ledger: roadmap.ps1 (live dir log/orch).

.EXAMPLE
  pwsh -NoProfile -File re/orchestrator/merge_train.ps1            # one batch
  pwsh -NoProfile -File re/orchestrator/merge_train.ps1 -Full      # force build+gates+full set (D2 x3, 13 tracks)
  pwsh -NoProfile -File re/orchestrator/merge_train.ps1 -Plan      # print what it would merge, change nothing

  -Full (HB-TRAIN-MATCH-TEXTONLY, 2026-11-10) counts only when the batch holds a decided-flip row (see Test-FlipRow);
  otherwise it is dropped and logged, and a full_set_paths file still brings the full set in. A text-only batch skips
  build and gates even under -Full (no build input changed, so the exe cannot change). -ForceFull keeps the old
  meaning of -Full (build + default gates + full set whatever the batch). re/match/** is text-only unless the build
  references the file (an .rsp entry, or an #include from mashedmod sources, followed through re/match includes).
  pwsh -NoProfile -File re/orchestrator/merge_train.ps1 -Drain     # passes back to back until the queue is empty (HB-TRAIN-DRAIN)

  -Drain (HB-TRAIN-DRAIN, 2026-11-10): runs one pass after another (each a child `merge_train.ps1 -NoFlip -ResultJson`,
  so every pass runs the newest script on orch/integration and keeps its own per-pass priority hold) while the queue
  is non-empty. After each GREEN pass whose gate hashes equal the merge_train.json refs (or that was text-only), it
  fast-forwards race/first-frame-parity in the main checkout and main (update-ref with the old value), checks both equal
  orch/integration, runs release_due.py and on DUE waits for publish.sh, then starts the next pass. It stops and
  reports on RED, INFRA, a hash change, a decided-flip row in the batch (left queued for its own -Full pass), or any
  error. Never pushes. Wrap the driver in test-slot if a slot is wanted: the slot stays outside, mashed_lock inside.
  A decided-flip row (Test-FlipRow): id -FLIP / FLIP-, "decided flip" in the ledger title, or (HB-TRAIN-FLIP-DECISION,
  2026-11-11) an L-n in the ledger `decision` field together with a title naming a default turned ON.
#>
[CmdletBinding()]
param([switch]$Full, [switch]$Plan, [int]$Max = 0, [switch]$KeepTemp, [switch]$SelfTest,
      [switch]$Drain, [switch]$NoFlip, [string]$ResultJson = '', [switch]$ForceFull)
$ErrorActionPreference = 'Stop'

$HERE = $PSScriptRoot
$MAIN = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$CFG = Get-Content (Join-Path $HERE 'merge_train.json') -Raw | ConvertFrom-Json
$RM = Join-Path $HERE 'roadmap.ps1'
$LOCKPY = Join-Path $HERE 'mashed_lock.py'
$DIAG = Join-Path $MAIN 'scripts\diag.py'
$TS = Get-Date -Format 'yyyyMMdd_HHmmss'
$DATA = Join-Path $(if ($SelfTest) { Join-Path ([IO.Path]::GetTempPath()) 'mtrain_selftest' } else { $CFG.data }) $(if ($Drain -and -not $SelfTest) { "drain_$TS" } else { $TS })
New-Item -ItemType Directory -Force -Path $DATA | Out-Null
$LOG = Join-Path $DATA 'merge_train.log'
$FullEff = [bool]$ForceFull   # HB-TRAIN-MATCH-TEXTONLY: the pass body sets it from -Full and the batch (Resolve-FullFlag)

function Say([string]$m) { $l = "[mtrain $(Get-Date -Format HH:mm:ss)] $m"; Write-Host $l; Add-Content -LiteralPath $LOG -Value $l }
# HB-EOL: tracked text files are written with LF, never the CRLF that Set-Content produces on Windows.
function Write-Lf([string]$path, [string[]]$lines) {
  [IO.File]::WriteAllText($path, ((($lines -join "`n") -replace "`r`n", "`n").TrimEnd("`n") + "`n"), (New-Object Text.UTF8Encoding($false)))
}
# HB-EOL: dump a git object byte-for-byte (a PowerShell pipe would decode it and Set-Content would re-encode with CRLF).
function Git-ShowBytes([string]$wt, [string]$spec, [string]$dest) {
  $psi = New-Object Diagnostics.ProcessStartInfo 'git.exe'
  foreach ($a in @('-C', $wt, 'show', $spec)) { $psi.ArgumentList.Add($a) }
  $psi.RedirectStandardOutput = $true; $psi.RedirectStandardError = $true; $psi.UseShellExecute = $false
  $p = [Diagnostics.Process]::Start($psi)
  $fs = [IO.File]::Create($dest)
  try { $p.StandardOutput.BaseStream.CopyTo($fs) } finally { $fs.Dispose() }
  $null = $p.StandardError.ReadToEnd(); $p.WaitForExit()
  $script:GITRC = $p.ExitCode
}
function Git {
  param([Parameter(ValueFromRemainingArguments)]$a)
  if ($a[0] -ne '-C') { $a = @('-C', $MAIN) + $a }
  $o = & git.exe @a 2>&1; $script:GITRC = $LASTEXITCODE; return $o
}
function Ledger { param([Parameter(ValueFromRemainingArguments)]$a) & pwsh -NoProfile -File $RM @a | ForEach-Object { Say "ledger: $_" } }

# HB-V5-SKILL: INTEGRATION_LOG.md records RED passes too. Before, a line was written only when a row merged, so a pass
# that bounced every row left no trace in the tracked log (only in D:\MashedWT\mtrain_data). Lines are newest first.
function Format-Bounced($bounced) { (@($bounced) | ForEach-Object { "$($_.it.id) ($($_.it.branch)): $(if ($_.conflict) { 'set aside, ' })$($_.why)" }) -join '; ' }
function New-LogLines([string]$il, [string]$line) {
  $ilLines = @(if (Test-Path $il) { Get-Content $il } else { '# orch/integration log (merge train, newest first)' })
  return @($ilLines[0], '', $line) + @($ilLines | Select-Object -Skip 1 | Where-Object { $_ -ne '' })
}
# A pass that merged nothing has no temp worktree commit to carry the line: commit it, alone, in the worktree that holds
# the integration branch (the same place `roadmap.ps1 snapshot` commits). The next pass folds such a ledger-only tip.
function Add-RedLogLine([string]$line) {
  $wl = (Git -C $MAIN worktree list --porcelain) -join "`n"
  $held = [regex]::Matches($wl, "worktree (.+)\nHEAD \w+\nbranch refs/heads/$([regex]::Escape($INT))(\n|$)")
  if (-not $held.Count) { Say "RED pass not logged: $INT is not checked out in any worktree"; return $false }
  $w = $held[0].Groups[1].Value
  $il = Join-Path $w 're\orchestrator\INTEGRATION_LOG.md'
  Write-Lf $il (New-LogLines $il $line)
  Git -C $w add -- 're/orchestrator/INTEGRATION_LOG.md' | Out-Null
  $o = Git -C $w commit -q -m 'orch: integration log, RED pass' -m 'Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>' -- 're/orchestrator/INTEGRATION_LOG.md'
  if ($script:GITRC -ne 0) { Say "RED pass log commit failed: $o"; return $false }
  Say "  RED pass logged in $INT ($((Git -C $w rev-parse --short HEAD) | Select-Object -First 1))"
  return $true
}
function Sha8([string]$p) { if (Test-Path $p) { (Get-FileHash -Algorithm SHA256 $p).Hash.Substring(0, 8).ToLower() } else { 'MISSING' } }
function Like-Any([string]$f, $globs) { foreach ($g in $globs) { if ($f -like $g) { return $true } } return $false }

# ---- helpers -------------------------------------------------------------------------
# HB-TRAIN-WTADD-RETRY: one `git worktree add`; the selftest swaps this for a fake that fails N times.
function Invoke-WtAdd([string]$p, [string]$at) { return (Git -C $MAIN worktree add --detach $p $at) }
$script:WtAddTries = 3; $script:WtAddWaitSec = 5
# HB-TRAIN-WTADD-RETRY: train 20261106_122811 lost a GREEN row to one failed add (stderr went to Out-Null, a manual
# add a minute later worked). git's output now goes to the train log, the add is retried (fresh path, prune between
# tries), and only the last failure throws, with git's message.
function New-Temp([string]$at) {
  $msg = ''
  for ($i = 1; $i -le $script:WtAddTries; $i++) {
    $p = Join-Path $MAIN ".worktrees\mtrain-$TS-$([guid]::NewGuid().ToString('N').Substring(0,4))"
    $o = @(Invoke-WtAdd $p $at)
    if ($script:GITRC -eq 0) {
      if ($i -gt 1) { Say "worktree add OK on attempt $i/$($script:WtAddTries): $p" }
      $script:Temps += $p   # HB-TRAINCLEAN: every temp worktree stays registered until Remove-Temp has removed it
      return $p
    }
    $msg = (@($o | ForEach-Object { "$_".Trim() } | Where-Object { $_ }) -join ' | ')
    Say "worktree add attempt $i/$($script:WtAddTries) failed rc=$($script:GITRC) at $at -> $p`: $msg"
    if ($i -lt $script:WtAddTries) {
      if ($script:WtAddWaitSec -gt 0) { Start-Sleep -Seconds $script:WtAddWaitSec }
      $null = Git -C $MAIN worktree prune
    }
  }
  throw "worktree add failed after $($script:WtAddTries) attempts at $at`: $msg"
}
function Remove-Wt([string]$p) {
  # SAFE removal only (never `git worktree remove --force`; WORKTREE-SYMLINK-WIPE). Returns the diag.py exit code.
  Push-Location $MAIN
  try { $o = & py.exe -3.12 $DIAG wt-remove $p 2>&1; $rc = $LASTEXITCODE; Say "wt-remove $p rc=$rc $($o | Select-Object -First 1)" } finally { Pop-Location }
  return $rc
}
$script:Temps = @()
function Remove-Temp([string]$p) {
  if ($KeepTemp -or -not $p) { return }
  if ((Remove-Wt $p) -eq 0) { $script:Temps = @($script:Temps | Where-Object { $_ -ne $p }) }
}
# HB-TRAINCLEAN / HB-TRAINFIX: the run body sits in a try/finally AT SCRIPT SCOPE whose finally calls Clear-Temps, so
# every temp worktree still registered is removed however the body ends (return, throw, foreign-tip skip, ledger
# failure). Never wrap the body in a function that dot-sources a [scriptblock] parameter: the body's variables share
# that function's scope, so `$body` (case-insensitive) assigned over the [scriptblock]$Body parameter and $merged /
# $bounced / $res never reached the report: the "System.Object[] to ScriptBlock" crash reverted by b94c480a0.
# The selftest checks this shape on the script's own AST.
function Clear-Temps {
  foreach ($t in @($script:Temps)) { Say "guard: removing leftover temp worktree $t"; Remove-Temp $t }
}
# HB-SOLOSTALE: each run records its PID and start time; a run killed mid-way leaves its mtrain worktree behind,
# and the next run reclaims it (diag.py wt-remove only). The solo token needs no sweep: solo-begin --owner-pid
# makes mashed_lock.py treat it as dead once this PID is gone.
function Write-TrainPid {
  $me = Get-Process -Id $PID
  Write-Lf (Join-Path $DATA 'merge_train.pid') @(([ordered]@{ pid = $PID; start = [DateTimeOffset]::new($me.StartTime).ToUnixTimeSeconds() } | ConvertTo-Json -Compress))
}
function Test-TrainDead([string]$ts) {
  $f = Join-Path (Split-Path $DATA -Parent) "$ts\merge_train.pid"
  if (-not (Test-Path -LiteralPath $f)) { return $true }
  try { $o = Get-Content -LiteralPath $f -Raw | ConvertFrom-Json } catch { return $true }
  $pr = Get-Process -Id ([int]$o.pid) -ErrorAction SilentlyContinue
  if (-not $pr) { return $true }
  return ([math]::Abs([DateTimeOffset]::new($pr.StartTime).ToUnixTimeSeconds() - [long]$o.start) -gt 2)
}
function Reclaim-DeadTrains {
  foreach ($d in @(Get-ChildItem -LiteralPath (Join-Path $MAIN '.worktrees') -Directory -Filter 'mtrain-*' -ErrorAction SilentlyContinue)) {
    if ($d.Name -notmatch '^mtrain-(\d{8}_\d{6})-') { continue }
    $ts = $Matches[1]
    if ($ts -eq $TS -or -not (Test-TrainDead $ts)) { continue }
    Say "reclaim: worktree $($d.Name) belongs to a dead train run ($ts)"
    Remove-Wt $d.FullName | Out-Null
  }
}
# HB-MASHEDLOCK-EARLY-PRIORITY: the pass holds its mashed_lock priority from before the first merge to the end of
# the gates, for THIS pwsh PID (<name>_priolinger_<pid>.lock, held=true). New non-priority solo opens wait on it;
# ordinary acquires, windows already open and the train's own priority calls do not. The 2026-11-01 starvations
# were all a solo window opened during the 4-8 min of merge+build before the first `run --priority` claim.
# Best effort: a failed write only reopens the old gap, so it warns and the pass goes on.
function Enter-PassPriority {
  try { $o = & py.exe -3.12 $LOCKPY prio-hold --owner-pid $PID --label "merge-train:$TS" 2>&1; Say "priority: $(($o | Select-Object -Last 1)) rc=$LASTEXITCODE" }
  catch { Say "WARN priority hold not registered: $($_.Exception.Message)" }
}
function Exit-PassPriority {
  try { $o = & py.exe -3.12 $LOCKPY prio-release --owner-pid $PID 2>&1; Say "priority: $(($o | Select-Object -Last 1)) rc=$LASTEXITCODE" }
  catch { Say "WARN priority release failed (stale once this PID exits): $($_.Exception.Message)" }
}
function Get-BranchWorktrees([string]$branch) {
  $wl = (Git -C $MAIN worktree list --porcelain) -join "`n"
  @([regex]::Matches($wl, "worktree (.+)\nHEAD \w+\nbranch refs/heads/$([regex]::Escape($branch))(?=\n|$)") | ForEach-Object { $_.Groups[1].Value })
}
# HB-WTCLEAN: after a GREEN fast-forward, drop the step's worktree (diag.py wt-remove) and its branch (git branch -d).
# Either refusing means skip and log. Never --force, never -D.
function Cleanup-Step([string]$branch, [string]$intBranch) {
  if (-not $branch -or $branch -eq $intBranch) { return }
  $intWt = @(Get-BranchWorktrees $intBranch) | Select-Object -First 1
  $mainNorm = [IO.Path]::GetFullPath($MAIN).TrimEnd('\','/')
  foreach ($w in @(Get-BranchWorktrees $branch)) {
    if ([IO.Path]::GetFullPath($w).TrimEnd('\','/') -ieq $mainNorm) { Say "  cleanup $branch`: checked out in the main checkout, skipped"; return }
    # HB-TRAINCLEAN: only worktrees under .worktrees\ are the loop's to remove
    $wtRoot = [IO.Path]::GetFullPath((Join-Path $MAIN '.worktrees')).TrimEnd('\','/') + '\'
    if (-not [IO.Path]::GetFullPath($w).StartsWith($wtRoot, [StringComparison]::OrdinalIgnoreCase)) { Say "  cleanup $branch`: worktree $w is not under .worktrees\, worktree and branch kept"; return }
    if ((Remove-Wt $w) -ne 0) { Say "  cleanup $branch`: worktree removal refused ($w), worktree and branch kept"; return }
  }
  if (-not $intWt) { Say "  cleanup $branch`: no worktree holds $intBranch to judge merged-ness, branch kept"; return }
  # -d judges merged-ness against HEAD of the repo it runs in, so run it where the integration branch is checked out
  $o = Git -C $intWt branch --delete $branch
  if ($script:GITRC -ne 0) { Say "  cleanup $branch`: git branch --delete refused, branch kept: $(($o | Select-Object -First 1))" }
  else { Say "  cleanup $branch`: worktree and branch removed" }
}
# ---- HB-TRAIN-MATCH-TEXTONLY: re/match/** is text-only unless the build references it ---------------------------
# Match pilots write re/match/<slice>/<rva>.c; only some are compiled into the exe/.asi, through an #include from a
# mashedmod TU (e.g. mashedmod/src/mashed_re/Match/MatchS01.cpp -> re/match/s01/00401340.c) or an .rsp entry. A
# match-only batch used to build and run every gate (about 17 of 20 min of such a pass).
function Get-RepoRel([string]$wt, [string]$full) {
  $root = [IO.Path]::GetFullPath($wt).TrimEnd('\', '/') + '\'
  $f = [IO.Path]::GetFullPath($full)
  if (-not $f.StartsWith($root, [StringComparison]::OrdinalIgnoreCase)) { return '' }
  return ($f.Substring($root.Length) -replace '\\', '/')
}
# The set of repo-relative re/match paths the build reads ('*' = all of them: a build script names re/match itself).
function Get-BuildMatchRefs([string]$wt) {
  $roots = @($CFG.text_only_unless_built); if (-not $roots.Count -or -not $roots[0]) { $roots = @('re/match/') }
  $set = New-Object 'System.Collections.Generic.HashSet[string]' ([StringComparer]::OrdinalIgnoreCase)
  $todo = New-Object 'System.Collections.Generic.Queue[string]'
  function Under([string]$rel) { foreach ($r in $roots) { if ($rel -and $rel.StartsWith($r, [StringComparison]::OrdinalIgnoreCase)) { return $true } }; return $false }
  $mm = Join-Path $wt 'mashedmod'
  foreach ($bs in @('build.bat', 'build_objs.ps1')) {
    $bp = Join-Path $mm $bs
    if ((Test-Path -LiteralPath $bp) -and (Select-String -LiteralPath $bp -Pattern 're[\\/]match' -Quiet)) { return @('*') }
  }
  $srcRoot = Join-Path $mm 'src\mashed_re'
  foreach ($rsp in @('exe_sources.rsp', 'asi_sources.rsp')) {
    $rp = Join-Path $mm $rsp
    if (-not (Test-Path -LiteralPath $rp)) { continue }
    foreach ($ln in @(Get-Content -LiteralPath $rp)) {
      $e = $ln.Trim().Trim('"'); if (-not $e -or $e.StartsWith('#')) { continue }
      $rel = Get-RepoRel $wt (Join-Path $srcRoot $e)
      if ((Under $rel) -and $set.Add($rel)) { $todo.Enqueue($rel) }
    }
  }
  $files = @(Get-ChildItem -LiteralPath $mm -Recurse -File -Include *.cpp, *.c, *.h, *.hpp, *.inl -ErrorAction SilentlyContinue | Where-Object { $_.FullName -notmatch '\\(build|deps)\\' })
  if ($files.Count) {
    # a mashedmod file sits outside re/, so a path into re/match/ names "match" (seed scan only; the walk below reads all)
    foreach ($m in @(Select-String -LiteralPath ($files | ForEach-Object FullName) -Pattern '^\s*#\s*include\s*"([^"]*match[^"]*)"')) {
      $rel = Get-RepoRel $wt (Join-Path (Split-Path $m.Path -Parent) $m.Matches[0].Groups[1].Value)
      if ((Under $rel) -and $set.Add($rel)) { $todo.Enqueue($rel) }
    }
  }
  while ($todo.Count) {   # a referenced re/match file can include its own siblings
    $rel = $todo.Dequeue(); $fp = Join-Path $wt $rel
    if (-not (Test-Path -LiteralPath $fp -PathType Leaf)) { continue }
    foreach ($m in @(Select-String -LiteralPath $fp -Pattern '^\s*#\s*include\s*"([^"]+)"')) {
      $r2 = Get-RepoRel $wt (Join-Path (Split-Path $fp -Parent) $m.Matches[0].Groups[1].Value)
      if ((Under $r2) -and $set.Add($r2)) { $todo.Enqueue($r2) }
    }
  }
  return @($set)
}
# One changed path: text-only by the globs, or under a text_only_unless_built root and not read by the build.
function Test-TextOnlyPath([string]$f, $matchRefs) {
  if (Like-Any $f $CFG.text_only_globs) { return $true }
  $roots = @($CFG.text_only_unless_built); if (-not $roots.Count -or -not $roots[0]) { $roots = @('re/match/') }
  foreach ($r in $roots) {
    if ($f.StartsWith($r, [StringComparison]::OrdinalIgnoreCase)) { return (@($matchRefs) -notcontains '*') -and (@($matchRefs) -notcontains $f) }
  }
  return $false
}
# -Full counts only with a decided flip in the batch; -ForceFull always.
function Resolve-FullFlag([bool]$full, [bool]$force, $flips) { return [bool]($force -or ($full -and @($flips).Count -gt 0)) }
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
    foreach ($st in 1, 2, 3) { Git-ShowBytes $wt ":${st}:$f" "$tmp.$st" }
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
  $lockArgs = @('run', '--priority', '--label', "merge-train:$TS", '--wait', '7200')
  if ($Solo) { $lockArgs += '--solo' }
  & py.exe -3.12 $LOCKPY @lockArgs -- py.exe -3.12 $runner @arms 2>&1 | ForEach-Object { Add-Content -LiteralPath $LOG -Value $_ }
  return $LASTEXITCODE
}
# HB-TRAIN-INFRA-PRECHECK: a run on a machine that cannot present (display asleep, screen locked) is not a finding.
# Every exe boot then fails CreateDevice with D3DERR hr=0x8876086A (diag.py D3D9-NOTAVAIL, same regex) and every gate
# hash comes out MISSING; run 31 pass 2 (20261102_091940) bounced MATCH-S08FU-NOTES that way as "default gates
# changed". Such a pass gives verdict INFRA: no bounce, no one-at-a-time re-merge, every unmerged row stays queued.
# INFRA needs evidence of the fault (the D3D9 signature in an arm log, a BLOCKED render probe, or no arm log at all);
# all-MISSING arms whose logs carry no signature get a render probe, and only a BLOCKED probe makes them INFRA, so a
# merged exe that dies at boot on a live display is still bounced instead of holding the queue forever.
$INFRA_RX = 'CreateDevice\(.*hr=0x8876086[AC]'
# InitD3D9 walks several CreateDevice attempts and logs each one (exe_main.cpp:3189-3203), so a failed attempt can
# be followed by a working one: the signature counts only in a log with no "CreateDevice(...) -> OK" line.
$DEVICE_OK_RX = 'CreateDevice\(.*\) -> OK '
# Callers consult this only for a gate that did not match its reference.
# '' = no infra fault; 'ALL-MISSING' = every hash MISSING, arm logs present, no signature (the caller probes);
# anything else = the INFRA reason.
function Get-ArmInfra([string]$out, [string[]]$arms, [string[]]$hashes) {
  $sig = @($arms | Where-Object { $lg = Join-Path $out "$_.log"; (Test-Path -LiteralPath $lg) -and (Select-String -LiteralPath $lg -Pattern $INFRA_RX -Quiet) -and -not (Select-String -LiteralPath $lg -Pattern $DEVICE_OK_RX -Quiet) })
  if ($sig.Count) { return "D3D9-NOTAVAIL (CreateDevice hr=0x8876086A/C) in $($sig -join ', ')" }
  if ($hashes.Count -and @($hashes | Where-Object { $_ -ne 'MISSING' }).Count -eq 0) {
    if (-not @($arms | Where-Object { Test-Path -LiteralPath (Join-Path $out "$_.log") }).Count) { return 'all gate hashes MISSING and no arm wrote a log (the runner or the lock never ran the exe)' }
    return 'ALL-MISSING'
  }
  return ''
}
# diag.py probe-render on the merged exe in the private root, under the machine lock. status OK | BLOCKED | UNKNOWN |
# UNAVAILABLE (no RENDER line: an older diag.py without --exe/--cwd, or the probe itself failed).
function Test-RenderProbe([string]$wt) {
  $diagPy = Join-Path $wt 'scripts\diag.py'; $exe = Join-Path $wt 'mashedmod\build\mashed_re.exe'
  $o = @(& py.exe -3.12 $LOCKPY run --priority --label "merge-train-probe:$TS" --wait 7200 -- py.exe -3.12 $diagPy probe-render --exe $exe --cwd $CFG.root 2>&1 | ForEach-Object { "$_" })
  Add-Content -LiteralPath (Join-Path $DATA 'probe_render.txt') -Value $o
  $line = @($o | Where-Object { $_ -match '^RENDER: ' } | Select-Object -Last 1)
  if ($line.Count -and $line[0] -match '^RENDER: (\w+)') { return @{ status = $Matches[1]; text = $line[0] } }
  return @{ status = 'UNAVAILABLE'; text = "probe-render gave no RENDER line: $(($o | Select-Object -Last 1))" }
}
function Gate-Default([string]$wt) {
  $out = Join-Path $DATA 'arms'
  $g = $CFG.gctrl; $b = $CFG.b
  $res = @{ ok = $true; text = ''; infra = $false; why = ''; hashes = $null }
  $tries = 0; $gArms = @(); $inf = ''
  do {
    $arm = if ($tries -eq 0) { $g.arm } else { "$($g.arm)-$($tries + 1)" }
    $gArms += $arm
    Run-Arms $wt @($arm) $out | Out-Null
    $h = @($g.files | ForEach-Object { Sha8 (Join-Path $out ($_ -replace "^$([regex]::Escape($g.arm))", $arm)) })
    $gok = (($h -join '/') -eq ($g.ref -join '/'))
    $tries++
    $inf = if ($gok) { '' } else { Get-ArmInfra $out @($arm) $h }
    if ($inf -and $inf -ne 'ALL-MISSING') { break }   # a dead display: no G-CTRL rerun, no B arms
  } while (-not $gok -and $tries -le [int]$g.retries)   # a G-CTRL run can drop one frame's rows: rerun first
  if ($inf -and $inf -ne 'ALL-MISSING') {
    $res.ok = $false; $res.infra = $true; $res.why = $inf
    $res.text = "G-CTRL $($h -join '/')  B not run (INFRA: $inf)"
    $res.hashes = @{ gctrl = @($h); b = @() }
    return $res
  }
  Run-Arms $wt $b.arms $out | Out-Null
  $hb = @($b.arms | ForEach-Object { Sha8 (Join-Path $out "$_.step.csv") })
  $bok = (($hb -join '/') -eq ($b.ref -join '/'))
  $res.ok = $gok -and $bok
  $res.hashes = @{ gctrl = @($h); b = @($hb) }   # HB-TRAIN-DRAIN: the drain compares these to the refs after the pass
  $res.text = "G-CTRL $($h -join '/')$(if($gok){''}else{' (ref ' + ($g.ref -join '/') + ')'})  B $($hb -join '/')$(if($bok){''}else{' (ref ' + ($b.ref -join '/') + ')'})"
  if (-not $res.ok) {
    $inf = Get-ArmInfra $out ($gArms + $b.arms) ($h + $hb)
    if ($inf -eq 'ALL-MISSING') {
      $pr = Test-RenderProbe $wt
      Say "  all gate hashes MISSING, render probe: $($pr.text)"
      if ($pr.status -eq 'BLOCKED') { $inf = "all gate hashes MISSING and the render probe is BLOCKED ($($pr.text))" }
      else { $res.text += " (all MISSING, render probe $($pr.status): a boot failure of the merged exe, not a display fault)"; $inf = '' }
    }
    if ($inf) { $res.infra = $true; $res.why = $inf; $res.text += " (INFRA: $inf)" }
  }
  return $res
}
function Get-ShortD2Arms([string]$out, [string[]]$arms, [int]$min) {
  @($arms | Where-Object { $lg = Join-Path $out "$_.motion_diag.log"; -not (Test-Path $lg) -or @(Get-Content $lg).Count -lt $min })
}
function Run-D2Arms([string]$wt, $f, [string]$out) {
  Run-Arms $wt $f.d2_arms $out -Solo | Out-Null
  $logs = @($f.d2_arms | ForEach-Object { Join-Path $out "$_.motion_diag.log" })
  # HB-D2RERUN: a D2 arm is wall-clock bound; a motion_diag under d2_min_lines is a short run, not a RED. Re-run only
  # the short arms (solo), at most d2_max_reruns times per arm, and log every rerun.
  $maxRe = if ($null -ne $f.d2_max_reruns) { [int]$f.d2_max_reruns } else { 2 }
  $tries = @{}; foreach ($a in $f.d2_arms) { $tries[$a] = 0 }
  while ($true) {
    $shortArms = @(Get-ShortD2Arms $out $f.d2_arms ([int]$f.d2_min_lines) | Where-Object { $tries[$_] -lt $maxRe })
    if (-not $shortArms.Count) { break }
    foreach ($a in $shortArms) { $tries[$a]++; Say "  D2 rerun: $a motion_diag under $($f.d2_min_lines) lines, rerun $($tries[$a])/$maxRe" }
    Run-Arms $wt $shortArms $out -Solo | Out-Null
  }
  $short = @(Get-ShortD2Arms $out $f.d2_arms ([int]$f.d2_min_lines))
  foreach ($a in $short) { Say "  D2 short after $($tries[$a]) rerun(s): $a" }
  return @{ logs = $logs; short = $short; maxRe = $maxRe; tries = $tries }
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
  $r2 = Run-D2Arms $wt $f $out; $logs = $r2.logs; $short = $r2.short; $maxRe = $r2.maxRe
  $d = & py.exe -3.12 (Join-Path $wt $f.d2_score) port @logs 2>&1
  $d | Set-Content (Join-Path $DATA 'score_d2.txt')
  $nd = @($d | Where-Object { $_ -match 'drvmed=\S+ in' -and $_ -notmatch 'OUT|=NA' }).Count
  if ($nd -ne 3 -or $short.Count) {   # HB-TRAIN-INFRA-PRECHECK: the display went to sleep after the default gates
    $d2inf = Get-ArmInfra $out $f.d2_arms @()
    if ($d2inf) { return @{ ok = $false; infra = $true; why = $d2inf; text = (($notes + "D2 $nd/3 (INFRA: $d2inf)") -join '; ') } }
  }
  if ($nd -ne 3 -or $short.Count) { $ok = $false }; $notes += "D2 $nd/3$(if($short.Count){" ($($short.Count) arm(s) still under $($f.d2_min_lines) lines after $maxRe rerun(s))"})"
  # 13 tracks: table identical to the reference
  Ensure-Root $CFG.root13
  $o13 = Join-Path $DATA 'tracks'
  $tok = & py.exe -3.12 $LOCKPY solo-begin --priority --label "merge-train13:$TS" --ttl 14400 --owner-pid $PID | Where-Object { $_ -match '^TOKEN ' }
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
# Gates on a built tree. HB-TRAIN-INFRA-PRECHECK: a render precheck first; BLOCKED = INFRA before any arm runs.
# UNKNOWN / UNAVAILABLE go on to the arms (Gate-Default classifies their result), only BLOCKED is display evidence.
function Run-Gates([string]$wt, $r, [bool]$needFull) {
  Ensure-Root $CFG.root
  $pr = Test-RenderProbe $wt
  Say "  render precheck: $($pr.text)"
  if ($pr.status -eq 'BLOCKED') { $r.ok = $false; $r.infra = $true; $r.gates = 'not run: render precheck BLOCKED'; $r.why = "INFRA: render precheck $($pr.text)"; return $r }
  $gd = Gate-Default $wt
  $r.gates = $gd.text; $r.hashes = $gd.hashes; Say "  gates: $($gd.text)"
  if ($gd.infra) { $r.ok = $false; $r.infra = $true; $r.why = "INFRA: $($gd.why)"; return $r }
  if (-not $gd.ok) { $r.ok = $false; $r.why = "default gates changed"; return $r }
  if ($needFull) {
    $gf = Gate-Full $wt
    $r.gates += "; $($gf.text)"; Say "  full set: $($gf.text)"
    if ($gf.infra) { $r.ok = $false; $r.infra = $true; $r.why = "INFRA: $($gf.why)"; return $r }
    if (-not $gf.ok) { $r.ok = $false; $r.why = "full gate set"; return $r }
  }
  return $r
}
# Step-2 outcome of a batch. On RED re-merges one branch at a time (first GREEN lands this run). HB-TRAIN-INFRA-PRECHECK:
# an INFRA result bounces nothing and stops the pass, so every row not merged stays queued for the next pass.
# Returns a hashtable (never dot-sourced): res, merged, bounced, infra (reason or '').
# HB-TRAIN-CONFLICT-SKIP (2026-11-10): a merge conflict found while merging the batch (before build or gates) sets that
# branch aside (bounced, reason conflict, no gate run charged to it) and the rest is merged and gated again as ONE
# batch, as often as conflicts come up. Only a failure after the merge (build, lint, gates) keeps the one-at-a-time
# path. Pass 20261110_042108 took 39 min (two full gate sets) because PORT-OT-FRONTEND conflicted on hooks.csv at 04:22.
function Resolve-Batch($items, $first) {
  $o = @{ res = $first; merged = @(); bounced = @(); infra = ''; error = '' }
  while (-not $first.ok -and $first.conflict -and $first.bad) {
    Remove-Temp $first.wt
    $o.bounced += @{ it = $first.bad; why = $first.why; gates = 'not run: merge conflict, set aside before build and gates'; conflict = $true }
    $items = @($items | Where-Object { $_.id -ne $first.bad.id })
    Say "  conflict: $($first.bad.id) set aside ($($first.why)); $(if ($items.Count) { "gating the other $($items.Count) row(s) as one batch" } else { 'no row left in the batch' })"
    if (-not $items.Count) { $o.res = $null; return $o }
    try { $first = Try-Batch $items }
    catch {
      $o.error = "$(($items | ForEach-Object id) -join '+'): $($_.Exception.Message)"
      Say "  batch after the conflict threw ($($_.Exception.Message)); rows not merged stay queued"
      $o.res = $null; return $o
    }
    $o.res = $first
  }
  if ($first.ok) { $o.merged = @($items); return $o }
  Remove-Temp $first.wt
  $o.res = $null
  if ($first.infra) { $o.infra = $first.why; return $o }
  if ($items.Count -eq 1) { $o.bounced = @(@{ it = $items[0]; why = $first.why; gates = $first.gates }); return $o }
  Say "batch RED ($($first.why)); re-merging one at a time"
  foreach ($it in $items) {
    # HB-TRAIN-WTADD-RETRY: a throw while trying a row (worktree add, git, build setup) stops the one-at-a-time pass
    # but never loses what it already proved: the GREEN row and the bounces so far are returned and landed by the
    # caller; the throwing row and the rows after it stay queued. Train 20261106_122811 lost MATCH-S09-FU that way.
    # HB-TRAIN-STACK (2026-11-08): after the first GREEN, each later row is tried STACKED on the rows already GREEN
    # (merged + it), so a GREEN row lands in this pass instead of being gated for nothing and left queued (train
    # 20261108_124527 gated PORT-RENDER-01 GREEN after FE-01 and dropped it). A RED stacked try bounces only `it`:
    # the stack without it was GREEN.
    try { $one = Try-Batch (@($o.merged) + @($it)) }
    catch {
      $o.error = "$($it.id): $($_.Exception.Message)"
      Say "  $($it.id): threw ($($_.Exception.Message)); stopping one-at-a-time$(if ($o.res) { ", landing GREEN $($o.merged[0].id)" }), rows not merged stay queued"
      break
    }
    if ($one.ok) {   # GREEN (alone, or stacked on the earlier GREEN rows): it becomes the result that lands this run
      if ($o.res) { Remove-Temp $o.res.wt }
      $o.res = $one; $o.merged = @($o.merged) + @($it); continue
    }
    Remove-Temp $one.wt
    if ($one.infra) { $o.infra = $one.why; Say "  $($it.id): $($one.why); stopping, the rows not merged stay queued"; break }
    if (-not $one.ok) { $o.bounced += @{ it = $it; why = $one.why; gates = $one.gates } }
  }
  return $o
}
function Tracker-Lint([string]$wt, [string]$baseRef) {
  $tlog = Join-Path $DATA 'tracker_lint.txt'
  $o = & py.exe -3.12 (Join-Path $HERE 'tracker_lint.py') --repo $wt --base $baseRef 2>&1; $rc = $LASTEXITCODE
  $o | Set-Content $tlog
  return @{ ok = ($rc -eq 0); rc = $rc; last = (($o | Select-Object -Last 1) -as [string]); log = $tlog; out = @($o) }
}
function Try-Batch($items) {
  $wt = New-Temp $base
  $r = @{ ok = $true; wt = $wt; why = ''; gates = 'not run'; sha = '' ; bad = $null; infra = $false; hashes = $null; textOnly = $false; conflict = $false }
  try {
    foreach ($it in $items) {
      $m = Merge-One $wt $it
      if (-not $m.ok) { $r.ok = $false; $r.why = $m.why; $r.bad = $it; $r.conflict = $true; return $r }   # HB-TRAIN-CONFLICT-SKIP: before build and gates
      Say "  merged $($it.id) ($($it.branch))"
    }
    $changed = @((Git -C $wt diff --name-only "$base..HEAD") | Where-Object { $_ })
    # HB-TRAIN-MATCH-TEXTONLY: re/match/** is text-only unless the build reads it; a flip pass (effective -Full) is
    # always built and gated.
    $mRefs = @(); if (@($changed | Where-Object { -not (Like-Any $_ $CFG.text_only_globs) -and $_ -like 're/match/*' }).Count) { $mRefs = @(Get-BuildMatchRefs $wt) }
    $textOnly = -not $FullEff -and (@($changed | Where-Object { -not (Test-TextOnlyPath $_ $mRefs) }).Count -eq 0)
    # The effective -Full forces the full set too: a decided flip can change a default outside full_set_paths
    # (D3-CF-SNAP-FLIP's default lives in D3d9Render/TrackRenderer.cpp and merged without D2 / 13 tracks).
    $needFull = $FullEff -or (@($changed | Where-Object { $f = $_; @($CFG.full_set_paths | Where-Object { $f.StartsWith($_) }).Count -gt 0 }).Count -gt 0)
    $r.textOnly = $textOnly
    Say "  changed $($changed.Count) file(s); text-only=$textOnly full-set=$needFull"
    # HB-LINT-408A50: lint the tree the LAN release builds from, a clean git archive of HEAD with the paths in
    # scripts/lan_release/archive_paths.txt. Every batch, text-only included (merge_train.json counts *.txt as
    # text-only, and re/tools/dual_copy_allowlist.txt is a build input). The worktree lint below sees files the
    # archive does not: 0e3d71a75 passed it and failed publish.sh on DUP-IN-TARGET 0x00408a50.
    $alPy = Join-Path $wt 'scripts/lan_release/archive_lint.py'
    if (Test-Path $alPy) {
      $alog = Join-Path $DATA 'archive_lint.txt'
      Push-Location $wt
      try { $al = & py.exe -3.12 $alPy --ref HEAD 2>&1; $arc = $LASTEXITCODE } finally { Pop-Location }
      $al | Set-Content $alog
      if ($arc -ne 0) { $r.ok = $false; $r.why = "archive_lint: $(($al | Select-Object -Last 1)) ($alog)"; return $r }
      Say "  archive lint OK: $(($al | Select-Object -Last 1))"
    } else { Say "  WARN archive lint SKIPPED: no scripts/lan_release/archive_lint.py in this tree" }
    # HB-TRAIN-HOOKSCSV-TEXTONLY: hooks.csv is text-only (nothing in the build or the gate scorers reads it), so a
    # re-classify row skips build + gates. The tracker lint (the train's own copy, next to this script) runs on every
    # batch instead and fails on a malformed hooks.csv / UNCERTAINTIES.md row the base does not already have.
    $tl = Tracker-Lint $wt $base
    if (-not $tl.ok) { $r.ok = $false; $r.why = "tracker_lint: $($tl.last) ($($tl.log))"; return $r }
    Say "  tracker lint OK: $($tl.last)"
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
    return (Run-Gates $wt $r $needFull)
  } finally {
    $r.sha = (Git -C $wt rev-parse HEAD) | Select-Object -First 1
  }
}

# ---- HB-TRAIN-DRAIN: batch pick, decided-flip check, pass result file, drain driver ---------------------------
# 2026-11-10: the train sat idle 00:10-03:01 with about 15 branches queued (it ran one pass, then waited for the lead),
# plus about 10 min between later passes. -Drain runs the passes back to back and does the lead's autopilot step
# (memory roadmap-loop-autopilot: fast-forward race/first-frame-parity and main on GREEN with unchanged hashes, then
# the LAN release check) itself. Every stop condition hands control back to the lead with a REPORT.
$DCFG = [ordered]@{
  parity_branch = 'race/first-frame-parity'; main_branch = 'main'; max_passes = 20
  flip_id_rx = '(^|-)FLIP(-|$)'; flip_title_rx = '(?i)\bdecided[ -]flips?\b'
  flip_decision_rx = '^L-\d+$'; flip_decision_title_rx = '(?i)\bdefault[ -]ON\b'
  lock_tries = 8; lock_wait_sec = 15
}
if ($CFG.PSObject.Properties['drain']) { foreach ($p in $CFG.drain.PSObject.Properties) { $DCFG[$p.Name] = $p.Value } }
function Select-Batch($queue, [int]$cap) {
  $b = @()
  foreach ($q in @($queue)) {
    if ($q.solo) { if ($b.Count -eq 0) { $b = @($q) }; break }   # a solo branch merges alone
    $b += $q
    if ($b.Count -ge $cap) { break }
  }
  return $b
}
function Get-Queue { @((& pwsh -NoProfile -File $RM queue) | Out-String | ConvertFrom-Json -DateKind String) }
function Get-LedgerRow([string]$id) { try { return ((& pwsh -NoProfile -File $RM get $id) | Out-String | ConvertFrom-Json -DateKind String) } catch { return $null } }
# A decided flip (MERGE_TRAIN.md "Decided default flips") changes the refs and must run alone with -Full after L0 has
# committed the new refs. Recognised by the row id (D4-FLIP-L29-L30, D3-SUBSTATE-FLIP, ...) or a title naming a
# decided flip. A false positive only stops the drain (the lead runs the row), it never merges anything.
# HB-TRAIN-FLIP-DECISION (2026-11-11): also a row whose ledger `decision` field holds an L-n AND whose title names a
# default turned ON. The decision field alone is not a flip marker: it stays set after the answer, and on 2026-11-11
# 100 of 646 rows carried one, 81 of them merged through the train as ordinary rows (knob default-OFF ports, policy
# carriers such as L115-ADMIT-1/2). Only the row's structured `decision` field and its title are read, never the note,
# so a near-match row that cites L-115/L-117/L-118 without its own decision never flags.
function Test-FlipRow($row) {
  if (-not $row) { return $false }
  if (([string]$row.id -match $DCFG.flip_id_rx) -or ([string]$row.title -match $DCFG.flip_title_rx)) { return $true }
  $dec = if ($row -is [System.Collections.IDictionary]) { $row['decision'] } elseif ($row.PSObject.Properties['decision']) { $row.decision } else { '' }
  return (([string]$dec -match $DCFG.flip_decision_rx) -and ([string]$row.title -match $DCFG.flip_decision_title_rx))
}
function Get-BatchFlips($batch) {
  @(foreach ($q in @($batch)) { $row = Get-LedgerRow $q.id; if (-not $row) { $row = $q }; if (Test-FlipRow $row) { [string]$q.id } })
}
function Write-PassResult([string]$path, [hashtable]$o) {
  if (-not $path) { return }
  Write-Lf $path @(($o | ConvertTo-Json -Depth 8))
}
function Read-Refs { $c = Get-Content (Join-Path $HERE 'merge_train.json') -Raw | ConvertFrom-Json; return @{ gctrl = @($c.gctrl.ref); b = @($c.b.ref) } }
function Wait-Drain([int]$s) { if ($s -gt 0) { Start-Sleep -Seconds $s } }
# '' = the drain may go on after this pass; anything else is the stop reason.
function Get-DrainStop($p, $refsNow) {
  switch ([string]$p.verdict) {
    'FLIP' { return "FLIP: decided-flip row(s) in the batch ($(@($p.flips) -join ', ')), left queued: run them alone with -Full after the refs are committed (MERGE_TRAIN.md)" }
    'INFRA' { return "INFRA: $($p.infra)" }
    'RED' { if ($p.conflict_only) { break }; return "RED: bounced [$((@($p.bounced) | ForEach-Object { "$($_.id): $($_.why)" }) -join '; ')]$(if ($p.error) { " error [$($p.error)]" })" }
    'GREEN' { }
    default { return "ERROR: pass verdict '$($p.verdict)' $($p.why)" }
  }
  if (-not @($p.merged).Count) { if ($p.conflict_only) { return '' }; return 'ERROR: GREEN pass with nothing merged' }
  $used = $p.refs
  if ($used -and ((@($used.gctrl) -join '/') -ne ($refsNow.gctrl -join '/') -or (@($used.b) -join '/') -ne ($refsNow.b -join '/'))) {
    return "HASH: merge_train.json refs changed during the pass (G-CTRL $(@($used.gctrl) -join '/') -> $($refsNow.gctrl -join '/'), B $(@($used.b) -join '/') -> $($refsNow.b -join '/'))"
  }
  if ($p.hashes) {
    $g = @($p.hashes.gctrl) -join '/'; $b = @($p.hashes.b) -join '/'
    if ($g -ne ($refsNow.gctrl -join '/') -or $b -ne ($refsNow.b -join '/')) { return "HASH: gate hashes differ from the refs (G-CTRL $g vs $($refsNow.gctrl -join '/'), B $b vs $($refsNow.b -join '/'))" }
  } elseif (-not $p.text_only) { return 'ERROR: GREEN pass with gates run but no hashes recorded' }
  return ''
}
# Fast-forward race/first-frame-parity (checked out in the main checkout) and main to the integration tip. Both must be
# ancestors of it (checked before anything moves). index.lock = another git in the main checkout: retry, never delete it.
function Advance-Main([string]$int) {
  $pb = $DCFG.parity_branch; $mb = $DCFG.main_branch
  $new = (Git rev-parse $int) | Select-Object -First 1
  $cur = (Git -C $MAIN symbolic-ref --short -q HEAD) | Select-Object -First 1
  if ($cur -ne $pb) { return @{ ok = $false; why = "the main checkout has '$cur' checked out, not $pb" } }
  $oldP = (Git rev-parse $pb) | Select-Object -First 1; $oldM = (Git rev-parse $mb) | Select-Object -First 1
  foreach ($pr in @(@($pb, $oldP), @($mb, $oldM))) {
    $null = Git merge-base --is-ancestor $pr[1] $new
    if ($script:GITRC -ne 0) { return @{ ok = $false; why = "$($pr[0]) @ $($pr[1].Substring(0,9)) is not an ancestor of $int @ $($new.Substring(0,9)): not a fast-forward, nothing moved" } }
  }
  if ($oldP -ne $new) {
    $tries = [int]$DCFG.lock_tries
    for ($i = 1; $i -le $tries; $i++) {
      $o = @(Git -C $MAIN -c merge.autoStash=false merge --ff-only -q $int)
      if ($script:GITRC -eq 0) { Say "drain: $pb fast-forwarded $($oldP.Substring(0,9)) -> $($new.Substring(0,9))$(if ($i -gt 1) { " on attempt $i" })"; break }
      $txt = (@($o | ForEach-Object { "$_".Trim() } | Where-Object { $_ }) -join ' | ')
      if ($txt -match 'index\.lock' -and $i -lt $tries) { Say "drain: ff of $pb blocked by index.lock (attempt $i/$tries), retry in $($DCFG.lock_wait_sec) s: $txt"; Wait-Drain ([int]$DCFG.lock_wait_sec); continue }
      return @{ ok = $false; why = "git merge --ff-only $int in the main checkout failed rc=$($script:GITRC) (attempt $i/$tries): $txt" }
    }
  }
  if ($oldM -ne $new) {
    $o = Git update-ref "refs/heads/$mb" $new $oldM
    if ($script:GITRC -ne 0) { return @{ ok = $false; why = "update-ref $mb failed: $o" } }
    Say "drain: $mb $($oldM.Substring(0,9)) -> $($new.Substring(0,9)) (update-ref, old value checked)"
  }
  $p2 = (Git rev-parse $pb) | Select-Object -First 1; $m2 = (Git rev-parse $mb) | Select-Object -First 1; $i2 = (Git rev-parse $int) | Select-Object -First 1
  if ($p2 -ne $i2 -or $m2 -ne $i2) { return @{ ok = $false; why = "after the fast-forward $pb=$p2 $mb=$m2 $int=$i2 (not all equal)" } }
  return @{ ok = $true; old_parity = $oldP; old_main = $oldM; new = $new }
}
function Find-GitBash {
  $g = (Get-Command git.exe -ErrorAction SilentlyContinue).Source
  if ($g) { $b = Join-Path (Split-Path (Split-Path $g -Parent) -Parent) 'bin\bash.exe'; if (Test-Path $b) { return $b } }
  return 'C:\Program Files\Git\bin\bash.exe'
}
function Invoke-ReleaseDue {
  Push-Location $MAIN
  try { $o = @(& py.exe -3.12 (Join-Path $MAIN 'scripts\lan_release\release_due.py') 2>&1 | ForEach-Object { "$_" }); $rc = $LASTEXITCODE } finally { Pop-Location }
  return @{ rc = $rc; first = (@($o) | Select-Object -First 1); text = $o }
}
function Invoke-Publish([string]$plog) {
  Push-Location $MAIN
  try { & (Find-GitBash) scripts/lan_release/publish.sh 2>&1 | ForEach-Object { Add-Content -LiteralPath $plog -Value "$_" }; $rc = $LASTEXITCODE } finally { Pop-Location }
  return $rc
}
# release_due.py exit 0 = DUE, 1 = NOT_DUE, 2 = ERROR. '' = go on, else the stop reason.
function Invoke-ReleaseStep([int]$n) {
  $rd = Invoke-ReleaseDue
  Say "drain: release_due rc=$($rd.rc): $($rd.first)"
  if ($rd.rc -eq 1) { return @{ stop = ''; text = "NOT_DUE" } }
  if ($rd.rc -ne 0) { return @{ stop = "ERROR: release_due.py rc=$($rd.rc): $($rd.first)"; text = "ERROR" } }
  $plog = Join-Path $DATA "publish_pass$n.log"
  Say "drain: DUE, publish.sh running (log $plog); the next pass waits for it"
  $prc = Invoke-Publish $plog
  $tail = @(Get-Content -LiteralPath $plog -ErrorAction SilentlyContinue | Select-Object -Last 1)
  Say "drain: publish.sh rc=$prc $tail"
  if ($prc -ne 0) { return @{ stop = "ERROR: publish.sh rc=$prc ($plog)"; text = "publish FAILED rc=$prc" } }
  return @{ stop = ''; text = "published ($tail)" }
}
function Invoke-PassChild([string]$rj) {
  $a = @('-NoProfile', '-File', $PSCommandPath, '-NoFlip', '-ResultJson', $rj)
  if ($Full) { $a += '-Full' }; if ($ForceFull) { $a += '-ForceFull' }; if ($Max -gt 0) { $a += @('-Max', "$Max") }; if ($KeepTemp) { $a += '-KeepTemp' }
  & pwsh @a 2>&1 | ForEach-Object { Write-Host "  | $_" }
  $rc = $LASTEXITCODE
  if (-not (Test-Path -LiteralPath $rj)) { return @{ verdict = 'ERROR'; why = "pass exited rc=$rc without writing $rj" } }
  return (Get-Content -LiteralPath $rj -Raw | ConvertFrom-Json -AsHashtable)
}
function Invoke-Drain([string]$int) {
  $passes = @(); $stop = ''; $lines = @()
  for ($n = 1; $n -le [int]$DCFG.max_passes; $n++) {
    $q = @(Get-Queue)
    if (-not $q.Count) { $stop = ''; Say "drain: queue empty after $($n - 1) pass(es)"; break }
    Say "drain: pass $n starts, $($q.Count) row(s) queued"
    $p = Invoke-PassChild (Join-Path $DATA "pass$n.result.json")
    $passes += $p
    Say "drain: pass $n verdict $($p.verdict) merged [$(@($p.merged) -join ' ')] bounced [$((@($p.bounced) | ForEach-Object { $_.id }) -join ' ')] data $($p.data)"
    $stop = Get-DrainStop $p (Read-Refs)
    $line = "pass $n $($p.verdict): merged [$(@($p.merged) -join ' ')] gates $($p.gates)"
    if ($stop) { $lines += $line; break }
    if ($p.conflict_only) { $line += " (set aside on a merge conflict: $((@($p.bounced) | ForEach-Object { $_.id }) -join ' '))" }
    if (-not @($p.merged).Count) { $lines += "$line; nothing merged, no fast-forward"; continue }   # conflict-only pass: every row bounced
    $adv = Advance-Main $int
    if (-not $adv.ok) { $stop = "ERROR: fast-forward of $($DCFG.parity_branch)/$($DCFG.main_branch) refused: $($adv.why)"; $lines += $line; break }
    $line += "; $($DCFG.parity_branch) + $($DCFG.main_branch) -> $($adv.new.Substring(0,9))"
    $rel = Invoke-ReleaseStep $n
    $line += "; release $($rel.text)"; $lines += $line
    if ($rel.stop) { $stop = $rel.stop; break }
  }
  if (-not $stop -and $n -gt [int]$DCFG.max_passes -and @(Get-Queue).Count) { $stop = "STOP: max_passes $($DCFG.max_passes) reached with rows still queued" }
  $verdict = if (-not $stop) { 'GREEN' } else { ($stop -split ':')[0] }
  $final = (Git rev-parse --short $int) | Select-Object -First 1
  $report = @(
    'REPORT merge-train-drain',
    "verdict: $verdict",
    "branch: $int @ $final",
    "evidence: $LOG",
    "gates: $(if ($passes.Count) { $passes[-1].gates } else { 'not run: queue empty' })",
    "decision: none",
    "next: $($passes.Count) pass(es): $($lines -join ' | ')$(if ($stop) { " | stopped: $stop" } else { ' | queue drained' })"
  )
  $report | Set-Content (Join-Path $DATA 'REPORT.txt') -Encoding utf8
  return @{ verdict = $verdict; stop = $stop; passes = $passes; report = $report }
}

# ---- self test (HB-TRAIN3): fake repo + fake arm runner, never reads the live queue ------------------
if ($SelfTest) {
  $fails = 0
  function Check([bool]$ok, [string]$what) { if (-not $ok) { $script:fails++ }; Say "  SELFTEST $(if ($ok) { 'PASS' } else { 'FAIL' }): $what" }
  $fake = Join-Path ([IO.Path]::GetTempPath()) "mtrain_selftest_repo_$TS"
  $MAIN = $fake; $DIAG = Join-Path $fake 'scripts\diag.py'; $INT = 'integration'
  New-Item -ItemType Directory -Force -Path (Join-Path $fake 'scripts'), (Join-Path $fake 're\analysis'), (Join-Path $fake 're\orchestrator') | Out-Null
  Copy-Item (Join-Path $HERE '..\..\scripts\diag.py') $DIAG
  $null = Git -C $fake init -q -b integration
  Git -C $fake config user.email t@t | Out-Null; Git -C $fake config user.name t | Out-Null
  Git -C $fake config core.autocrlf true | Out-Null   # same as the real checkout
  [IO.File]::WriteAllBytes((Join-Path $fake 're\analysis\CHANGELOG.md'), [Text.Encoding]::UTF8.GetBytes("# log`nbase`n"))
  Git -C $fake add . | Out-Null; Git -C $fake commit -q -m base | Out-Null
  Say "SELFTEST fake repo $fake"

  # --- HB-EOL: union-resolve a CHANGELOG conflict, then check the committed blob and the log writer for CR bytes
  foreach ($n in 'a', 'b') {
    Git -C $fake checkout -q -b "step-$n" integration | Out-Null
    [IO.File]::WriteAllBytes((Join-Path $fake 're\analysis\CHANGELOG.md'), [Text.Encoding]::UTF8.GetBytes("# log`nbase`nentry $n`n"))
    Git -C $fake commit -q -am "step $n" | Out-Null
  }
  Git -C $fake checkout -q integration | Out-Null
  $CFG.conflict_union = @('re/analysis/CHANGELOG.md')
  $wt = New-Temp 'integration'
  $m1 = Merge-One $wt @{ id = 'T-A'; branch = 'step-a' }
  $m2 = Merge-One $wt @{ id = 'T-B'; branch = 'step-b' }
  Check ($m1.ok -and $m2.ok) 'both branches merged, second one by union-resolve'
  $blob = Join-Path $DATA 'changelog.blob'
  Git-ShowBytes $wt 'HEAD:re/analysis/CHANGELOG.md' $blob
  $bytes = [IO.File]::ReadAllBytes($blob)
  Check (@($bytes | Where-Object { $_ -eq 13 }).Count -eq 0) "committed CHANGELOG.md has 0 CR bytes ($($bytes.Count) bytes, entries a+b: $([Text.Encoding]::UTF8.GetString($bytes) -match 'entry a' -and [Text.Encoding]::UTF8.GetString($bytes) -match 'entry b'))"
  $il = Join-Path $DATA 'INTEGRATION_LOG.md'
  Write-Lf $il @('# orch/integration log (merge train, newest first)', '', '- line')
  Check (@([IO.File]::ReadAllBytes($il) | Where-Object { $_ -eq 13 }).Count -eq 0) 'Write-Lf output (INTEGRATION_LOG.md path) has 0 CR bytes'
  $old = Join-Path $DATA 'old_style.txt'
  'x', 'y' | Set-Content $old -Encoding utf8
  Say "  SELFTEST contrast: Set-Content writes $(@([IO.File]::ReadAllBytes($old) | Where-Object { $_ -eq 13 }).Count) CR byte(s) for 2 lines on this host"
  Git -C $fake merge --ff-only -q (Git -C $wt rev-parse HEAD | Select-Object -First 1) 2>&1 | Out-Null
  Remove-Temp $wt

  # --- HB-V5-SKILL: a RED pass that merged nothing still writes INTEGRATION_LOG.md (one commit, that file only, newest first)
  $h0 = (Git -C $fake rev-parse HEAD) | Select-Object -First 1
  $fb = @(@{ it = @{ id = 'T-R'; branch = 'step-r' }; why = 'default gates changed'; gates = 'G-CTRL x/y/z' }, @{ it = @{ id = 'T-S'; branch = 'step-s' }; why = 'conflict'; conflict = $true; gates = 'not run' })
  $ok1 = Add-RedLogLine "- 2026-01-01 00:00 RED, nothing merged. Bounced: $(Format-Bounced $fb). Run data X"
  $ok2 = Add-RedLogLine '- 2026-01-01 00:01 RED, second'
  $rl = @(Get-Content (Join-Path $fake 're\orchestrator\INTEGRATION_LOG.md'))
  $rf = @((Git -C $fake diff --name-only "$h0..HEAD") | Where-Object { $_ })
  $rn = [int]((Git -C $fake rev-list --count "$h0..HEAD") | Select-Object -First 1)
  $rcr = @([IO.File]::ReadAllBytes((Join-Path $fake 're\orchestrator\INTEGRATION_LOG.md')) | Where-Object { $_ -eq 13 }).Count
  Check ($ok1 -and $ok2 -and $rn -eq 2 -and ($rf -join ',') -eq 're/orchestrator/INTEGRATION_LOG.md' -and $rl[0] -like '# orch/integration log*' -and $rl[2] -eq '- 2026-01-01 00:01 RED, second' -and $rl[3] -match 'T-R \(step-r\): default gates changed; T-S \(step-s\): set aside, conflict' -and $rcr -eq 0) "RED pass log: 2 commits touching only INTEGRATION_LOG.md, newest first, bounced rows named, 0 CR (commits $rn, files $($rf -join ','), CR $rcr)"
  $tbAll = (Get-Content $PSCommandPath -Raw)
  Check ($tbAll -match 'if \(-not \$merged\.Count -and \$bounced\.Count\) \{' -and $tbAll -match '\$\(if \(\$bounced\.Count\) \{ " Bounced: \$\(Format-Bounced \$bounced\)\." \}\)') 'RED pass log: the run body calls Add-RedLogLine when nothing merged, and a mixed pass names its bounced rows on the merged line'

  # --- HB-WTCLEAN: three step branches, all merged into integration, three different cleanup outcomes
  foreach ($n in 'clean', 'dirty', 'ahead') { Git -C $fake branch "step-$n" integration | Out-Null }
  Git -C $fake worktree add -q (Join-Path $fake ".worktrees\step-clean") step-clean | Out-Null
  Git -C $fake worktree add -q (Join-Path $fake ".worktrees\step-dirty") step-dirty | Out-Null
  Set-Content (Join-Path $fake '.worktrees\step-dirty\scratch.txt') 'uncommitted' ; Git -C (Join-Path $fake '.worktrees\step-dirty') add scratch.txt | Out-Null
  Git -C $fake checkout -q step-ahead | Out-Null; Set-Content (Join-Path $fake 'ahead.txt') 'x'; Git -C $fake add ahead.txt | Out-Null; Git -C $fake commit -q -m ahead | Out-Null; Git -C $fake checkout -q integration | Out-Null
  Cleanup-Step 'step-clean' $INT
  Cleanup-Step 'step-dirty' $INT
  Cleanup-Step 'step-ahead' $INT
  $brs = @((Git -C $fake branch --format '%(refname:short)'))
  Check (($brs -notcontains 'step-clean') -and -not (Test-Path (Join-Path $fake '.worktrees\step-clean'))) 'clean merged step: worktree and branch removed'
  Check (($brs -contains 'step-dirty') -and (Test-Path (Join-Path $fake '.worktrees\step-dirty'))) 'dirty step: wt-remove refused, worktree and branch kept (no force)'
  Check ($brs -contains 'step-ahead') 'unmerged step: git branch --delete refused, branch kept (no -D)'

  # --- HB-TRAINCLEAN guard paths, in the HB-TRAINFIX shape: script-scope try { } finally { Clear-Temps }
  function Temp-Dirs { @(Get-ChildItem -LiteralPath (Join-Path $fake '.worktrees') -Directory -Filter 'mtrain-*' -ErrorAction SilentlyContinue) }
  $script:Temps = @()
  try { $null = New-Temp 'integration'; $leakIn = (Temp-Dirs).Count } finally { Clear-Temps }
  Check ($leakIn -eq 1 -and (Temp-Dirs).Count -eq 0 -and $script:Temps.Count -eq 0) 'guard: body ends without removing its temp (foreign-tip skip shape), finally removed it'
  $caught = $false
  try { try { $null = New-Temp 'integration'; throw 'forced failure' } finally { Clear-Temps } } catch { $caught = $_.Exception.Message -eq 'forced failure' }
  Check ($caught -and (Temp-Dirs).Count -eq 0 -and $script:Temps.Count -eq 0) 'guard: forced throw in the body, exception propagates AND finally removed the temp'
  try { $t = New-Temp 'integration'; Remove-Temp $t } finally { Clear-Temps }
  Check ((Temp-Dirs).Count -eq 0 -and $script:Temps.Count -eq 0) 'guard: body that cleans up after itself, nothing left, registry empty'
  try { $ta = New-Temp 'integration'; $tb = New-Temp 'integration'; Remove-Temp $ta; $two = (Temp-Dirs).Count } finally { Clear-Temps }
  Check ($two -eq 1 -and (Temp-Dirs).Count -eq 0 -and $script:Temps.Count -eq 0) 'guard: two temps, one removed by the body, finally removed only the other'
  # a row worktree outside .worktrees\ is never removed by Cleanup-Step
  Git -C $fake branch step-out integration | Out-Null
  $outside = "$fake-outside-wt"
  Git -C $fake worktree add -q $outside step-out | Out-Null
  Cleanup-Step 'step-out' $INT
  $brs = @((Git -C $fake branch --format '%(refname:short)'))
  Check ((Test-Path $outside) -and ($brs -contains 'step-out')) 'row worktree outside .worktrees\: skipped, worktree and branch kept'
  Remove-Wt $outside | Out-Null

  # --- HB-TRAINFIX regression: the body assigns $body (the INTEGRATION_LOG update did) and sets $merged/$bounced
  function Old-Guarded([scriptblock]$Body) { try { . $Body } finally { Clear-Temps } }   # the reverted HB-TRAINCLEAN shape
  $oldErr = ''
  try { Old-Guarded { $body = @('# log', '', '- line') } } catch { $oldErr = $_.Exception.Message }
  Check ($oldErr -match 'ScriptBlock') "control: the old dot-sourcing wrapper reproduces the crash ($(($oldErr -split "`n")[0]))"
  $oldMerged = $null
  Old-Guarded { $oldMerged = @('X') }
  Check ($null -eq $oldMerged) 'control: the old wrapper loses the body''s $merged (function-local)'
  $merged = @(); $bounced = @(); $body = $null
  try { $body = @('# log', '', '- line'); $merged = @('T-A'); $bounced = @(@{ id = 'T-B' }) } finally { Clear-Temps }
  Check ($body.Count -eq 3 -and $merged.Count -eq 1 -and $bounced.Count -eq 1 -and $bounced[0].id -eq 'T-B') 'new shape: $body assignment works and $merged/$bounced reach the code after the guard'
  $merged = @(); $bounced = @(); $body = $null

  # --- HB-TRAINFIX AST check on this script: the run body is a script-scope try whose finally calls Clear-Temps
  $pe = $null; $ast = [System.Management.Automation.Language.Parser]::ParseFile($PSCommandPath, [ref]$null, [ref]$pe)
  $A = [System.Management.Automation.Language.Ast]
  $tries = @($ast.FindAll({ param($n) $n -is [System.Management.Automation.Language.TryStatementAst] -and $n.Body.Extent.Text -match '\$res = Try-Batch \$batch' }, $true))
  $top = @($tries | Where-Object { $_.Parent -eq $ast.EndBlock })
  $fin = $top.Count -eq 1 -and $null -ne $top[0].Finally -and $top[0].Finally.Extent.Text -match '\bClear-Temps\b'
  $covers = $top.Count -eq 1 -and $top[0].Body.Extent.Text -match 'Cleanup-Step \$br \$INT' -and $top[0].Body.Extent.Text -match 'Ledger set \$it\.id merged'
  Check ($pe.Count -eq 0 -and $fin -and $covers) "AST: one script-scope try holds Try-Batch through Cleanup-Step, finally calls Clear-Temps (parse errors $($pe.Count), top-level tries $($top.Count))"
  $dots = @($ast.FindAll({ param($n) $n -is [System.Management.Automation.Language.CommandAst] -and $n.InvocationOperator -eq 'Dot' -and $n.CommandElements[0] -is [System.Management.Automation.Language.VariableExpressionAst] -and $n.Extent.Text -notmatch '^\. \$Body$' }, $true))
  $sbParams = @($ast.FindAll({ param($n) $n -is [System.Management.Automation.Language.ParameterAst] -and $n.StaticType -eq [scriptblock] }, $true) | ForEach-Object { $_.Name.VariablePath.UserPath })
  $wrappedBody = @($ast.FindAll({ param($n) $n -is [System.Management.Automation.Language.FunctionDefinitionAst] -and $n.Body.Extent.Text -match 'Try-Batch \$batch' }, $true))
  Check ($dots.Count -eq 0 -and $wrappedBody.Count -eq 0 -and $sbParams.Count -eq 1 -and $sbParams[0] -eq 'Body') "AST: no function wraps the run body, no dot-sourced variable outside the selftest control (scriptblock params: $($sbParams -join ','))"

  # --- HB-MASHEDLOCK-EARLY-PRIORITY: the pass-wide priority hold, against a private MASHED_LOCK_DIR (never the live lock)
  $hp = $top.Count -eq 1 -and $top[0].Body.Statements.Count -gt 0 -and $top[0].Body.Statements[0].Extent.Text -eq 'Enter-PassPriority'
  $inner = @($top | ForEach-Object { $_.Finally.FindAll({ param($n) $n -is [System.Management.Automation.Language.TryStatementAst] -and $n.Body.Extent.Text -match '\bClear-Temps\b' -and $null -ne $n.Finally -and $n.Finally.Extent.Text -match '\bExit-PassPriority\b' }, $true) })
  Check ($hp -and $inner.Count -eq 1) "AST: Enter-PassPriority is the first statement of the run-body try (before Try-Batch), Exit-PassPriority runs in a finally after Clear-Temps (first=$hp inner=$($inner.Count))"
  $oldLd = $env:MASHED_LOCK_DIR; $env:MASHED_LOCK_DIR = Join-Path $DATA 'lockdir'
  New-Item -ItemType Directory -Force -Path $env:MASHED_LOCK_DIR | Out-Null
  $hf = Join-Path $env:MASHED_LOCK_DIR "mashed_machine_priolinger_$PID.lock"
  $sf = Join-Path $env:MASHED_LOCK_DIR 'mashed_machine_solo.lock'
  $kid = $null
  try {
    $during = $null; $threw = $false
    try { try { Enter-PassPriority; $during = Get-Content -LiteralPath $hf -Raw | ConvertFrom-Json; throw 'forced failure' } finally { try { Clear-Temps } finally { Exit-PassPriority } } } catch { $threw = $_.Exception.Message -eq 'forced failure' }
    $after = Get-Content -LiteralPath $hf -Raw | ConvertFrom-Json
    Check ($during.held -and [int]$during.pid -eq $PID -and $threw -and -not $after.held -and [int]$after.pid -eq $PID) "priority hold: held=true for this PID inside the body, a throw propagates AND the finally releases it (held=$($after.held))"
    Remove-Item -LiteralPath $hf -Force
    Enter-PassPriority
    $st = & py.exe -3.12 $LOCKPY status
    Check ([bool]($st -match "PRIORITY-HOLD 'mashed_machine' pid=$PID ")) "priority hold: status shows PRIORITY-HOLD pid=$PID"
    $kid = Start-Process py.exe -ArgumentList @('-3.12', $LOCKPY, 'solo-begin', '--label', 'selftest-worker', '--ttl', '60', '--wait', '60') -PassThru -NoNewWindow -RedirectStandardOutput (Join-Path $DATA 'solo_kid.txt')
    Start-Sleep -Seconds 3
    $blocked = -not (Test-Path -LiteralPath $sf)
    $orc = & py.exe -3.12 $LOCKPY run --label selftest-ordinary --wait 5 -- py.exe -3.12 -c "print('ordinary ok')"
    $ordinary = [bool]($orc -match 'ordinary ok')
    & py.exe -3.12 $LOCKPY prio-release --owner-pid $PID --linger 0 | Out-Null
    $opened = $false; foreach ($i in 1..40) { if (Test-Path -LiteralPath $sf) { $opened = $true; break }; Start-Sleep -Milliseconds 250 }
    Check ($blocked -and $ordinary -and $opened) "priority hold: a new non-priority solo open waits while held (blocked=$blocked), an ordinary acquire does not (ok=$ordinary), the solo opens after release (opened=$opened)"
  } finally {
    if ($kid -and -not $kid.HasExited) { Stop-Process -Id $kid.Id -Force -ErrorAction SilentlyContinue }   # only the PID this test spawned
    Remove-Item -LiteralPath $hf, $sf -Force -ErrorAction SilentlyContinue
    $env:MASHED_LOCK_DIR = $oldLd
  }

  # --- HB-TRAIN-HOOKSCSV-TEXTONLY: a hooks.csv-only re-classify batch is text-only, and the tracker lint still
  # bounces a malformed row. Runs the real Try-Batch on the fake repo (no archive_lint.py there, so no build is reached
  # on a text-only batch). The base carries one 9-column row, like the 4 pre-existing NCOL rows of the live hooks.csv.
  function Put([string]$rel, [string]$text) { [IO.File]::WriteAllBytes((Join-Path $fake $rel), [Text.Encoding]::UTF8.GetBytes($text)) }
  $hdr = "rva,name,subsystem,confidence,status,file,scenario,frida_diff,notes,exe_file`n# RVA: comment row,,,,,,,`n"
  $hbase = $hdr + "00401000,FunA,boot,C3,impl,a.cpp,,,n,a.cpp`n00401010,FunB,boot,C2,mapped,,,,n,`n00401020,FunC,boot,C4,verified,c.cpp,,,short row`n"
  $ubase = "# U`n| U-0001 | semantic | x | y | z | n/a | nothing |`n| U-0002 | semantic | x | y | z | n/a | nothing |`n"
  Put 'hooks.csv' $hbase; Put 'UNCERTAINTIES.md' $ubase
  Git -C $fake add hooks.csv UNCERTAINTIES.md | Out-Null; Git -C $fake commit -q -m 'trackers base' | Out-Null
  $cases = [ordered]@{
    'rc-ok'     = @{ h = ($hbase -replace '00401000,FunA,boot,C3', '00401000,FunA,boot,C4'); u = $ubase; log = $true; want = $true }
    'bad-ncol'  = @{ h = ($hbase + "00401030,FunD,boot,C2,mapped,,,,n,,extra`n"); u = $ubase; want = $false; key = 'NCOL rva=00401030' }
    'bad-conf'  = @{ h = ($hbase -replace '00401010,FunB,boot,C2', '00401010,FunB,boot,C5'); u = $ubase; want = $false; key = 'CONF rva=00401010' }
    'bad-dup'   = @{ h = ($hbase + "00401010,FunB2,boot,C2,mapped,,,,n,`n"); u = $ubase; want = $false; key = 'DUP rva=00401010' }
    'bad-quote' = @{ h = ($hbase + "00401040,FunE,boot,C2,mapped,,,,`"open quote,`n"); u = $ubase; want = $false; key = 'PARSE' }
    'bad-udup'  = @{ h = $hbase; u = ($ubase + "| U-0002 | semantic | dup | y | z | n/a | nothing |`n"); want = $false; key = 'UDUP U-0002' }
  }
  foreach ($k in $cases.Keys) {
    $c = $cases[$k]
    Git -C $fake checkout -q -b "step-$k" integration | Out-Null
    Put 'hooks.csv' $c.h; Put 'UNCERTAINTIES.md' $c.u
    if ($c.log) { Put 're\analysis\CHANGELOG.md' "# log`nbase`nentry a`nentry b`nre-classify $k`n" }
    Git -C $fake commit -q -am "step $k" | Out-Null
    Git -C $fake checkout -q integration | Out-Null
  }
  $base = (Git -C $fake rev-parse integration) | Select-Object -First 1
  foreach ($k in $cases.Keys) {
    $c = $cases[$k]
    $res = Try-Batch @(@{ id = "T-$k"; branch = "step-$k" })
    $tlo = if (Test-Path (Join-Path $DATA 'tracker_lint.txt')) { Get-Content (Join-Path $DATA 'tracker_lint.txt') -Raw } else { '' }
    if ($c.want) {
      Check ($res.ok -and $res.gates -match '^not run: text-only' -and $tlo -match 'pre-existing=1 NEW=0') "tracker: $k (hooks.csv + CHANGELOG only) is text-only, no build, lint OK with the base's 9-column row pre-existing (ok=$($res.ok) gates='$($res.gates)')"
    } else {
      Check ((-not $res.ok) -and $res.why -match '^tracker_lint: ' -and $tlo -match [regex]::Escape("NEW $($c.key)")) "tracker: $k bounced by the tracker lint with NEW $($c.key) (why='$($res.why)')"
    }
    Remove-Temp $res.wt
  }
  $oldGlobs = @('*.md', '*.txt', 'verify/*', 're/analysis/*')
  Check ((-not (Like-Any 'hooks.csv' $oldGlobs)) -and (Like-Any 'hooks.csv' $CFG.text_only_globs)) 'control: hooks.csv was not text-only under the old globs, is under merge_train.json'
  $src = @('mashedmod/src/mashed_re/Ai/AiStandalone.cpp', 'mashedmod/build.bat', 're/tools/dual_copy_allowlist.txt.bak', 'scripts/lint_rva_bodies.py', 're/match/s01.c', 're/orchestrator/tracker_lint.py', 're/frida/hooks_registry.py', 're/tools/hooks.csv')
  $leak = @($src | Where-Object { Like-Any $_ $CFG.text_only_globs })
  Check ($leak.Count -eq 0) "control: build inputs and tools stay non-text-only, the hooks.csv glob is root-only (leaks: $($leak -join ','))"

  # --- HB-TRAIN-CONFLICT-SKIP (real merges on the fake repo): A merges, B conflicts with A on a file outside
  # conflict_union, C is clean. B is set aside with no gate run, A+C are merged again and gated as ONE batch.
  New-Item -ItemType Directory -Force -Path (Join-Path $fake 'notes') | Out-Null
  Put 'notes\conf.txt' "base`n"; Git -C $fake add notes | Out-Null; Git -C $fake commit -q -m 'conf base' | Out-Null
  foreach ($cv in @(@('cA', 'notes\conf.txt', "A`n"), @('cB', 'notes\conf.txt', "B`n"), @('cC', 'notes\c.txt', "c`n"))) {
    Git -C $fake checkout -q -b "step-$($cv[0])" integration | Out-Null
    Put $cv[1] $cv[2]; Git -C $fake add notes | Out-Null; Git -C $fake commit -q -m "step $($cv[0])" | Out-Null
    Git -C $fake checkout -q integration | Out-Null
  }
  $base = (Git -C $fake rev-parse integration) | Select-Object -First 1
  $cItems = @(@{ id = 'T-cA'; branch = 'step-cA' }, @{ id = 'T-cB'; branch = 'step-cB' }, @{ id = 'T-cC'; branch = 'step-cC' })
  $mark = @(Get-Content -LiteralPath $LOG).Count
  $cFirst = Try-Batch $cItems
  $cO = Resolve-Batch $cItems $cFirst
  $cLog = @(Get-Content -LiteralPath $LOG | Select-Object -Skip $mark)
  $mergedA = @($cLog | Where-Object { $_ -match 'merged T-cA ' }).Count
  Check ($cFirst.conflict -and $cFirst.bad.id -eq 'T-cB' -and $cO.res.ok -and (($cO.merged | ForEach-Object id) -join ',') -eq 'T-cA,T-cC' -and $cO.bounced.Count -eq 1 -and $cO.bounced[0].it.id -eq 'T-cB' -and $cO.bounced[0].conflict -and $cO.bounced[0].gates -match '^not run: merge conflict' -and $mergedA -eq 2 -and -not ($cLog -match 're-merging one at a time') -and ($cLog -match 'conflict: T-cB set aside')) "conflict-skip (real merge): B conflicts on notes/conf.txt = set aside with no gate run, A+C merged again and gated as one batch (A merged $mergedA times, one-at-a-time not used, merged $(($cO.merged | ForEach-Object id) -join ','))"
  $cTree = @((Git -C $cO.res.wt diff --name-only "$base..HEAD") | Where-Object { $_ })
  Check ((($cTree | Sort-Object) -join ',') -eq 'notes/c.txt,notes/conf.txt' -and ((Get-Content (Join-Path $cO.res.wt 'notes\conf.txt') -Raw) -match '^A')) "conflict-skip (real merge): the landed tree holds A's conf.txt and C's c.txt only ($($cTree -join ','))"
  Remove-Temp $cO.res.wt

  # --- HB-TRAIN-MATCH-TEXTONLY (real Try-Batch on the fake repo): re/match files the build does not read are text-only
  New-Item -ItemType Directory -Force -Path (Join-Path $fake 'mashedmod\src\mashed_re\Match'), (Join-Path $fake 're\match\t01g'), (Join-Path $fake 're\match\s99'), (Join-Path $fake 're\match\rsp1') | Out-Null
  Put 'mashedmod\src\mashed_re\Match\MatchT01g.cpp' "// fake TU`n#include `"../../../../re/match/t01g/004111c0.c`"`n"
  Put 'mashedmod\exe_sources.rsp' "`"exe_main.cpp`"`n`"..\..\..\re\match\rsp1\a.c`"`n"
  Put 'mashedmod\asi_sources.rsp' "`"dllmain.cpp`"`n"
  Put 're\match\t01g\004111c0.c' "#include `"common.h`"`nint f(void){return 1;}`n"
  Put 're\match\t01g\common.h' "#define X 1`n"
  Put 're\match\rsp1\a.c' "int a;`n"
  Put 're\match\s99\00499999.c' "int g(void){return 2;}`n"
  Put 're\match\s99\RESULT.json' "{}`n"
  Git -C $fake add mashedmod re | Out-Null; Git -C $fake commit -q -m 'match base' | Out-Null
  $mrefs = @(Get-BuildMatchRefs $fake)
  Check ((($mrefs | Sort-Object) -join ',') -eq 're/match/rsp1/a.c,re/match/t01g/004111c0.c,re/match/t01g/common.h') "match-textonly: build refs = rsp entry + #include from a mashedmod TU + the include inside it ($(($mrefs | Sort-Object) -join ','))"
  $mcases = [ordered]@{
    'm-free'   = @{ files = @('re\match\s99\00499999.c', 're\match\s99\RESULT.json'); text = $true }
    'm-incl'   = @{ files = @('re\match\t01g\004111c0.c'); text = $false }
    'm-trans'  = @{ files = @('re\match\t01g\common.h'); text = $false }
    'm-rsp'    = @{ files = @('re\match\rsp1\a.c'); text = $false }
    'm-mix'    = @{ files = @('re\match\s99\00499999.c', 'mashedmod\src\mashed_re\Match\MatchT01g.cpp'); text = $false }
  }
  foreach ($k in $mcases.Keys) {
    Git -C $fake checkout -q -b "step-$k" integration | Out-Null
    foreach ($fl in $mcases[$k].files) { Put $fl "// changed by $k`n$((Get-Content (Join-Path $fake $fl) -Raw))" }
    Git -C $fake add mashedmod re | Out-Null; Git -C $fake commit -q -m "step $k" | Out-Null
    Git -C $fake checkout -q integration | Out-Null
  }
  $base = (Git -C $fake rev-parse integration) | Select-Object -First 1
  foreach ($k in $mcases.Keys) {
    $res = Try-Batch @(@{ id = "T-$k"; branch = "step-$k" })
    if ($mcases[$k].text) { Check ($res.ok -and $res.textOnly -and $res.gates -match '^not run: text-only') "match-textonly: $k (re/match files no build reads) = text-only, no build, no gates (gates '$($res.gates)')" }
    else { Check (-not $res.textOnly -and $res.why -match '^build failed') "match-textonly: $k = not text-only, the train goes on to the build (fake repo has no build.bat: '$($res.why)')" }
    Remove-Temp $res.wt
  }
  $FullEff = $true
  $res = Try-Batch @(@{ id = 'T-m-free'; branch = 'step-m-free' })
  Check (-not $res.textOnly -and $res.why -match '^build failed') "match-textonly: a flip pass (effective -Full) still builds and gates a match-only batch ('$($res.why)')"
  Remove-Temp $res.wt; $FullEff = $false
  Put 'mashedmod\build_objs.ps1' "# compiles re\match\*.c directly`n"
  Check ((@(Get-BuildMatchRefs $fake) -join ',') -eq '*') 'match-textonly: a build script naming re/match makes every re/match file a build input'
  Remove-Item -LiteralPath (Join-Path $fake 'mashedmod\build_objs.ps1') -Force
  Check (-not (Test-TextOnlyPath 're/match/s99/x.c' @('*')) -and (Test-TextOnlyPath 're/match/s99/x.c' @('re/match/t01g/a.c')) -and -not (Test-TextOnlyPath 're/matchx/a.c' @()) -and -not (Test-TextOnlyPath 're/tools/match/match.py' @())) 'match-textonly: Test-TextOnlyPath only frees re/match/ (not re/matchx/, not re/tools/match/)'
  # -Full only with a decided flip
  Check (-not (Resolve-FullFlag $true $false @()) -and (Resolve-FullFlag $true $false @('X-FLIP')) -and (Resolve-FullFlag $false $true @()) -and -not (Resolve-FullFlag $false $false @('X-FLIP'))) 'match-textonly: -Full counts only with a decided flip in the batch, -ForceFull always, no flag never'
  # live tree: the spec's example is a build input, and most of re/match is not
  $liveRefs = @(Get-BuildMatchRefs (Resolve-Path (Join-Path $HERE '..\..')).Path)
  $liveAll = @(Get-ChildItem -LiteralPath (Join-Path $HERE '..\..\re\match') -Recurse -File -ErrorAction SilentlyContinue).Count
  Check (($liveRefs -contains 're/match/t01g/004111c0.c') -and ($liveRefs -contains 're/match/s01/00401340.c') -and ($liveRefs -notcontains '*') -and $liveRefs.Count -lt $liveAll) "match-textonly (live tree): t01g/004111c0.c and s01/00401340.c are build inputs; $($liveRefs.Count) of $liveAll re/match files are"

  # --- HB-D2RERUN: fake arm runner, the second d2_arms arm comes back short (1485 lines) on the first run
  # The arm names come from merge_train.json (D2_DEF_n until run 23, D2_DEFH_n since); the second arm is the short one.
  $f = $CFG.full; $out = Join-Path $DATA 'd2arms'; New-Item -ItemType Directory -Force -Path $out | Out-Null
  $script:d2Short = $f.d2_arms[1]; $d2First = $f.d2_arms[0]
  $script:runs = @{}
  function Run-Arms([string]$wt, [string[]]$arms, [string]$out, [switch]$Solo) {
    foreach ($a in $arms) {
      $script:runs[$a] = 1 + [int]$script:runs[$a]
      $lines = switch ($script:Scenario) { 'short-once' { if ($a -eq $script:d2Short -and $script:runs[$a] -eq 1) { 1485 } else { 1700 } } 'short-always' { if ($a -eq $script:d2Short) { 1485 } else { 1700 } } default { 1700 } }
      Write-Lf (Join-Path $out "$a.motion_diag.log") @(1..$lines | ForEach-Object { "row $_" })
    }
    return 0
  }
  $script:Scenario = 'short-once'
  $r = Run-D2Arms 'unused' $f $out
  Check ($r.short.Count -eq 0 -and $script:runs[$script:d2Short] -eq 2 -and $script:runs[$d2First] -eq 1) "short-once: $($script:d2Short) rerun once (2 runs), others not rerun, 0 arms short"
  $script:runs = @{}; $script:Scenario = 'short-always'
  $r = Run-D2Arms 'unused' $f $out
  Check ($r.short.Count -eq 1 -and $script:runs[$script:d2Short] -eq (1 + $r.maxRe)) "short-always: reruns bounded at $($r.maxRe), arm still reported short (runs=$($script:runs[$script:d2Short]))"
  $script:runs = @{}; $script:Scenario = 'ok'
  $r = Run-D2Arms 'unused' $f $out
  Check ($r.short.Count -eq 0 -and $script:runs[$script:d2Short] -eq 1) 'full-length logs: no rerun'

  # --- HB-TRAIN-INFRA-PRECHECK: INFRA path (no bounce, rows stay queued) vs the normal path, all offline:
  # fake arm runner, fake render probe, fake Try-Batch. Never boots the exe. merge_train.json is never written:
  # the refs below live only in an in-memory copy of $CFG, and its SHA-256 is checked before and after.
  $cfgPath = Join-Path $HERE 'merge_train.json'; $cfgSha0 = (Get-FileHash -Algorithm SHA256 $cfgPath).Hash
  $CFG = $CFG | ConvertTo-Json -Depth 20 | ConvertFrom-Json
  $sleepLine = 'InitD3D9: CreateDevice(HAL,depthA,vsimm) -> fail hr=0x8876086A'
  $okLine = 'InitD3D9: CreateDevice(HAL,depthA,vsimm) -> OK hr=0x00000000 OK'
  # unit: Get-ArmInfra
  $ia = Join-Path $DATA 'infra_unit'; New-Item -ItemType Directory -Force -Path $ia | Out-Null
  Write-Lf (Join-Path $ia 'U1.log') @('boot', $sleepLine)
  Write-Lf (Join-Path $ia 'U2.log') @('boot', 'InitD3D9: CreateDevice(HAL,depthB,vsdef) -> fail hr=0x8876086C')
  Write-Lf (Join-Path $ia 'U3.log') @('boot', $okLine)
  Write-Lf (Join-Path $ia 'U5.log') @('boot', $sleepLine, 'InitD3D9: CreateDevice(REF,depthA,vsimm) -> OK hr=0x00000000 OK')
  Check ((Get-ArmInfra $ia @('U1') @('MISSING')) -match '^D3D9-NOTAVAIL .* in U1$') 'unit: arm log with CreateDevice hr=0x8876086A and no device = INFRA reason'
  Check ((Get-ArmInfra $ia @('U3', 'U2') @('1234abcd', 'MISSING')) -match 'in U2$') 'unit: hr=0x8876086C in one of two arm logs = INFRA, names that arm'
  Check ((Get-ArmInfra $ia @('U5') @('MISSING')) -eq 'ALL-MISSING') 'unit: 0x8876086A followed by a working CreateDevice is not the signature (falls to ALL-MISSING)'
  Check ((Get-ArmInfra $ia @('U3') @('MISSING', 'MISSING')) -eq 'ALL-MISSING') 'unit: all MISSING, log present, no signature = ALL-MISSING (caller probes)'
  Check ((Get-ArmInfra $ia @('U4') @('MISSING', 'MISSING')) -match 'no arm wrote a log') 'unit: all MISSING and no arm log = INFRA (runner never ran the exe)'
  Check ((Get-ArmInfra $ia @('U3') @('MISSING', '1234abcd')) -eq '') 'unit: one hash present, clean log = not INFRA'
  Check ((Get-ArmInfra $ia @('U3') @()) -eq '') 'unit: D2 form (no hashes), clean log = not INFRA'

  # Run-Gates end to end: render precheck -> Gate-Default -> verdict routing
  $g = $CFG.gctrl; $bA = $CFG.b
  $refDir = Join-Path $DATA 'infra_ref'; New-Item -ItemType Directory -Force -Path $refDir | Out-Null
  $gRef = @($g.files | ForEach-Object { $p = Join-Path $refDir $_; Write-Lf $p @("fake $_"); Sha8 $p })
  $bRef = @($bA.arms | ForEach-Object { $p = Join-Path $refDir "$_.step.csv"; Write-Lf $p @("fake B step"); Sha8 $p })
  $CFG.gctrl.ref = $gRef; $CFG.b.ref = $bRef
  function Ensure-Root([string]$root) { }
  function Test-RenderProbe([string]$wt) {
    $s = if ($script:ProbeSeq.Count) { $script:ProbeSeq[0] } else { 'OK' }
    if ($script:ProbeSeq.Count) { $script:ProbeSeq = @($script:ProbeSeq | Select-Object -Skip 1) }
    $script:probes++
    return @{ status = $s; text = "RENDER: $s - fake probe" }
  }
  function Run-Arms([string]$wt, [string[]]$arms, [string]$out, [switch]$Solo) {
    New-Item -ItemType Directory -Force -Path $out | Out-Null
    foreach ($a in $arms) {
      $script:runs[$a] = 1 + [int]$script:runs[$a]
      $base = $a -replace '-\d+$', ''
      $sc = $script:Scenario
      $logLines = switch ($sc) { 'asleep' { @('boot', $script:sleepLine) } 'fallback-red' { @('boot', $script:sleepLine, $script:okLine) } 'nolog' { $null } default { @('boot', $script:okLine) } }
      if ($logLines) { Write-Lf (Join-Path $out "$a.log") $logLines }
      if ($sc -in 'asleep', 'nolog', 'missing') { continue }
      if ($base -eq $script:gArm) { foreach ($fn in $script:gFiles) { Write-Lf (Join-Path $out ($fn -replace "^$([regex]::Escape($script:gArm))", $a)) @("fake $fn") } }
      else { Write-Lf (Join-Path $out "$a.step.csv") @($(if (($sc -in @('b-red', 'fallback-red')) -and ($a -eq 'B_DEF_K1')) { 'B changed' } else { 'fake B step' })) }
    }
    return 0
  }
  $script:sleepLine = $sleepLine; $script:okLine = $okLine; $script:gArm = $g.arm; $script:gFiles = @($g.files)
  function Gates-Case([string]$scenario, [string[]]$probeSeq) {
    Remove-Item -LiteralPath (Join-Path $DATA 'arms') -Recurse -Force -ErrorAction SilentlyContinue
    $script:Scenario = $scenario; $script:ProbeSeq = @($probeSeq); $script:probes = 0; $script:runs = @{}
    $rr = @{ ok = $true; wt = 'fake'; why = ''; gates = 'not run'; sha = ''; bad = $null; infra = $false }
    return (Run-Gates 'fake-wt' $rr $false)
  }
  $c = Gates-Case 'green' @('BLOCKED')
  Check (-not $c.ok -and $c.infra -and $c.why -match '^INFRA: render precheck RENDER: BLOCKED' -and $script:runs.Count -eq 0) "INFRA path: precheck BLOCKED = INFRA before any arm (arms run $($script:runs.Count), why '$($c.why)')"
  $c = Gates-Case 'green' @('OK')
  Check ($c.ok -and -not $c.infra -and $c.gates -notmatch 'ref|INFRA' -and $script:probes -eq 1) "normal path: precheck OK, refs match = GREEN, one probe, no rerun (gates '$($c.gates)')"
  $c = Gates-Case 'b-red' @('OK')
  Check (-not $c.ok -and -not $c.infra -and $c.why -eq 'default gates changed' -and $c.gates -match 'B \S+ \(ref ' -and $script:probes -eq 1) "normal path: a changed B hash is still RED 'default gates changed' (gates '$($c.gates)')"
  $c = Gates-Case 'asleep' @('OK')
  Check (-not $c.ok -and $c.infra -and $c.why -match '^INFRA: D3D9-NOTAVAIL' -and $script:runs[$g.arm] -eq 1 -and -not $script:runs.ContainsKey('B_DEF_K0') -and $c.gates -match 'MISSING') "INFRA path: 0x8876086A arm (display slept after the precheck) = INFRA, no G-CTRL rerun, B not run (runs $(($script:runs.Keys | Sort-Object) -join ','))"
  $c = Gates-Case 'asleep' @('UNKNOWN')
  Check (-not $c.ok -and $c.infra -and $c.why -match 'D3D9-NOTAVAIL') 'INFRA path: precheck UNKNOWN goes on to the arms, the 0x8876086A arm makes it INFRA'
  $c = Gates-Case 'nolog' @('OK')
  Check (-not $c.ok -and $c.infra -and $c.why -match 'no arm wrote a log') "INFRA path: arms that wrote nothing = INFRA (why '$($c.why)')"
  $c = Gates-Case 'missing' @('OK', 'BLOCKED')
  Check (-not $c.ok -and $c.infra -and $c.why -match 'MISSING and the render probe is BLOCKED' -and $script:probes -eq 2) "INFRA path: all-MISSING with clean logs, post-arm probe BLOCKED = INFRA (probes $($script:probes))"
  $c = Gates-Case 'missing' @('OK', 'OK')
  Check (-not $c.ok -and -not $c.infra -and $c.why -eq 'default gates changed' -and $c.gates -match 'boot failure of the merged exe') "normal path: all-MISSING with a live display stays RED (a broken exe is bounced, not held) (gates '$($c.gates)')"
  $c = Gates-Case 'fallback-red' @('OK')
  Check (-not $c.ok -and -not $c.infra -and $c.why -eq 'default gates changed') 'normal path: a 0x8876086A attempt followed by a working device is not INFRA, the changed hash stays RED'

  # Resolve-Batch: bounce / queue decisions with a fake Try-Batch (no merge, no build)
  function Remove-Temp([string]$p) { $script:removed++ }
  function Try-Batch($items) {
    $k = ($items | ForEach-Object { $_.id }) -join '+'
    $script:tbCalls += $k
    return $script:TB[$k]
  }
  function TB([string]$kind) {
    switch ($kind) {
      'green' { @{ ok = $true; wt = 'fake'; why = ''; gates = 'G-CTRL ok'; infra = $false } }
      'red' { @{ ok = $false; wt = 'fake'; why = 'default gates changed'; gates = 'G-CTRL x (ref y)'; infra = $false } }
      'infra' { @{ ok = $false; wt = 'fake'; why = 'INFRA: D3D9-NOTAVAIL (CreateDevice hr=0x8876086A/C) in H_DEF'; gates = 'G-CTRL MISSING/MISSING/MISSING  B not run'; infra = $true } }
    }
  }
  $iA = @{ id = 'T-A'; branch = 'orch/T-A' }; $iB = @{ id = 'T-B'; branch = 'orch/T-B' }; $iC = @{ id = 'T-C'; branch = 'orch/T-C' }
  function Batch-Case([object[]]$items, [hashtable]$tb) {
    $script:TB = $tb; $script:tbCalls = @(); $script:removed = 0
    $first = Try-Batch $items
    return (Resolve-Batch $items $first)
  }
  $o = Batch-Case @($iA) @{ 'T-A' = (TB 'infra') }
  Check ($o.infra -and $o.bounced.Count -eq 0 -and $o.merged.Count -eq 0 -and $null -eq $o.res) "INFRA path: single row INFRA = 0 bounced, 0 merged, row left queued ($($o.infra))"
  $o = Batch-Case @($iA, $iB, $iC) @{ 'T-A+T-B+T-C' = (TB 'infra') }
  Check ($o.infra -and $o.bounced.Count -eq 0 -and $script:tbCalls.Count -eq 1) "INFRA path: 3-row batch INFRA = 0 bounced, no one-at-a-time re-merge (Try-Batch calls $($script:tbCalls.Count))"
  $o = Batch-Case @($iA, $iB, $iC) @{ 'T-A+T-B+T-C' = (TB 'red'); 'T-A' = (TB 'red'); 'T-B' = (TB 'infra'); 'T-C' = (TB 'green') }
  Check ($o.infra -and $o.bounced.Count -eq 1 -and $o.bounced[0].it.id -eq 'T-A' -and $o.merged.Count -eq 0 -and ($script:tbCalls -join ',') -eq 'T-A+T-B+T-C,T-A,T-B') "INFRA path: INFRA mid one-at-a-time stops there; the RED before it bounces, the row after it is not tried (calls $($script:tbCalls -join ','))"
  $o = Batch-Case @($iA) @{ 'T-A' = (TB 'red') }
  Check (-not $o.infra -and $o.bounced.Count -eq 1 -and $o.bounced[0].why -eq 'default gates changed') 'normal path: single row RED = bounced (unchanged)'
  $o = Batch-Case @($iA, $iB, $iC) @{ 'T-A+T-B+T-C' = (TB 'red'); 'T-A' = (TB 'green'); 'T-A+T-B' = (TB 'red'); 'T-A+T-C' = (TB 'green') }
  Check (-not $o.infra -and $o.merged.Count -eq 2 -and $o.merged[0].id -eq 'T-A' -and $o.merged[1].id -eq 'T-C' -and $o.bounced.Count -eq 1 -and $o.bounced[0].it.id -eq 'T-B' -and $null -ne $o.res -and $script:removed -eq 3 -and ($script:tbCalls -join ',') -eq 'T-A+T-B+T-C,T-A,T-A+T-B,T-A+T-C') "normal path (HB-TRAIN-STACK): batch RED -> one at a time, later rows stacked on the GREEN ones: A and C land, B bounced (calls $($script:tbCalls -join ','), temps removed $($script:removed))"
  $o = Batch-Case @($iA, $iB, $iC) @{ 'T-A+T-B+T-C' = (TB 'red'); 'T-A' = (TB 'red'); 'T-B' = (TB 'green'); 'T-B+T-C' = (TB 'green') }
  Check (-not $o.infra -and $o.merged.Count -eq 2 -and $o.merged[0].id -eq 'T-B' -and $o.merged[1].id -eq 'T-C' -and $o.bounced.Count -eq 1 -and $o.bounced[0].it.id -eq 'T-A' -and ($script:tbCalls -join ',') -eq 'T-A+T-B+T-C,T-A,T-B,T-B+T-C') "normal path (HB-TRAIN-STACK): RED first, then two GREEN stacked: B and C land, A bounced (calls $($script:tbCalls -join ','))"
  $o = Batch-Case @($iA, $iB) @{ 'T-A+T-B' = (TB 'green') }
  Check (-not $o.infra -and $o.merged.Count -eq 2 -and $o.bounced.Count -eq 0 -and $script:tbCalls.Count -eq 1) 'normal path: batch GREEN = both merged (unchanged)'
  # HB-TRAIN-CONFLICT-SKIP routing (fake Try-Batch): a conflict sets the row aside and re-gates the rest as one batch
  function TBC($bad) { @{ ok = $false; wt = 'fake'; why = 'conflict needs a hand merge (frida-sweep recipe): hooks.csv'; gates = 'not run'; infra = $false; conflict = $true; bad = $bad } }
  $o = Batch-Case @($iA, $iB, $iC) @{ 'T-A+T-B+T-C' = (TBC $iB); 'T-A+T-C' = (TB 'green') }
  Check ((($script:tbCalls) -join ',') -eq 'T-A+T-B+T-C,T-A+T-C' -and (($o.merged | ForEach-Object id) -join ',') -eq 'T-A,T-C' -and $o.bounced.Count -eq 1 -and $o.bounced[0].it.id -eq 'T-B' -and $o.bounced[0].conflict -and $null -ne $o.res -and $o.res.ok) "conflict-skip: B conflicts = B set aside, A+C gated once as one batch and land (calls $($script:tbCalls -join ','))"
  $o = Batch-Case @($iA, $iB, $iC) @{ 'T-A+T-B+T-C' = (TBC $iA); 'T-B+T-C' = (TBC $iC); 'T-B' = (TB 'green') }
  Check ((($script:tbCalls) -join ',') -eq 'T-A+T-B+T-C,T-B+T-C,T-B' -and (($o.merged | ForEach-Object id) -join ',') -eq 'T-B' -and (($o.bounced | ForEach-Object { $_.it.id }) -join ',') -eq 'T-A,T-C') "conflict-skip: two conflicts in a row = both set aside, B gated alone once (calls $($script:tbCalls -join ','))"
  $o = Batch-Case @($iA, $iB, $iC) @{ 'T-A+T-B+T-C' = (TBC $iB); 'T-A+T-C' = (TB 'red'); 'T-A' = (TB 'green') }
  Check ((($script:tbCalls) -join ',') -eq 'T-A+T-B+T-C,T-A+T-C,T-A,T-A+T-C' -and (($o.merged | ForEach-Object id) -join ',') -eq 'T-A' -and (($o.bounced | ForEach-Object { "$($_.it.id)$(if ($_.conflict) { ':c' })" }) -join ',') -eq 'T-B:c,T-C') "conflict-skip: the rest gate-RED = the one-at-a-time path runs on the rest only (calls $($script:tbCalls -join ','))"
  $o = Batch-Case @($iA) @{ 'T-A' = (TBC $iA) }
  Check ($script:tbCalls.Count -eq 1 -and $o.merged.Count -eq 0 -and $o.bounced.Count -eq 1 -and $o.bounced[0].conflict -and $null -eq $o.res -and -not $o.infra) 'conflict-skip: a single conflicting row = bounced (conflict), no second Try-Batch'
  $o = Batch-Case @($iA, $iB) @{ 'T-A+T-B' = (TBC $iB); 'T-A' = (TB 'infra') }
  Check ($o.infra -and $o.bounced.Count -eq 1 -and $o.bounced[0].it.id -eq 'T-B' -and $o.merged.Count -eq 0) 'conflict-skip: the rest INFRA = conflict row bounced, the rest stays queued (INFRA)'

  # --- HB-TRAIN-WTADD-RETRY (a): New-Temp retries a failing `git worktree add` and logs git's message
  $realAdd = ${function:Invoke-WtAdd}
  $script:WtAddWaitSec = 0
  function Invoke-WtAdd([string]$p, [string]$at) {
    $script:addCalls++
    if ($script:addCalls -le $script:addFails) { $script:GITRC = 128; return @("fatal: fake add failure $($script:addCalls) for $at") }
    return (& $realAdd $p $at)
  }
  $script:addCalls = 0; $script:addFails = 2; $script:Temps = @()
  $tp = $null; $terr = ''
  try { $tp = New-Temp 'integration' } catch { $terr = $_.Exception.Message }
  $logTxt = Get-Content -LiteralPath $LOG -Raw
  Check ($tp -and (Test-Path -LiteralPath (Join-Path $tp '.git')) -and $script:addCalls -eq 3 -and $script:Temps.Count -eq 1 -and $script:Temps[0] -eq $tp -and $logTxt -match 'attempt 1/3 failed rc=128 .*fatal: fake add failure 1' -and $logTxt -match 'attempt 2/3 failed rc=128 .*fatal: fake add failure 2' -and $logTxt -match 'worktree add OK on attempt 3/3') "wtadd: add fails twice then succeeds = temp created on attempt 3, registered once, both git messages in the train log (calls $($script:addCalls), err '$terr')"
  if ($tp) { Remove-Wt $tp | Out-Null }; $script:Temps = @()
  $script:addCalls = 0; $script:addFails = 3; $terr = ''; $tp = $null
  try { $tp = New-Temp 'integration' } catch { $terr = $_.Exception.Message }
  Check (-not $tp -and $script:addCalls -eq 3 -and $terr -match '^worktree add failed after 3 attempts at integration: fatal: fake add failure 3' -and $script:Temps.Count -eq 0) "wtadd: three failures = throw after the last try with git's message ('$terr')"
  ${function:Invoke-WtAdd} = $realAdd; $script:WtAddWaitSec = 5

  # --- HB-TRAIN-WTADD-RETRY (b): a throw after a GREEN row in one-at-a-time mode still lands the GREEN row
  function Try-Batch($items) {
    $k = ($items | ForEach-Object { $_.id }) -join '+'
    $script:tbCalls += $k
    if ($script:TB[$k] -eq 'throw') { throw "worktree add failed after 3 attempts at abc123: fatal: fake for $k" }
    return $script:TB[$k]
  }
  $o = $null; $esc = ''
  try { $o = Batch-Case @($iA, $iB, $iC) @{ 'T-A+T-B+T-C' = (TB 'red'); 'T-A' = (TB 'green'); 'T-A+T-B' = 'throw'; 'T-C' = (TB 'green') } } catch { $esc = $_.Exception.Message }
  Check (-not $esc -and $o.merged.Count -eq 1 -and $o.merged[0].id -eq 'T-A' -and $null -ne $o.res -and $o.res.ok -and $o.bounced.Count -eq 0 -and $o.error -match '^T-B: worktree add failed' -and ($script:tbCalls -join ',') -eq 'T-A+T-B+T-C,T-A,T-A+T-B') "one-at-a-time: GREEN T-A then T-B throws = no exception escapes, T-A returned for landing, T-B/T-C stay queued (calls $($script:tbCalls -join ','), escaped '$esc', error '$($o.error)')"
  $o = $null; $esc = ''
  try { $o = Batch-Case @($iA, $iB, $iC) @{ 'T-A+T-B+T-C' = (TB 'red'); 'T-A' = (TB 'red'); 'T-B' = 'throw'; 'T-C' = (TB 'green') } } catch { $esc = $_.Exception.Message }
  Check (-not $esc -and $o.merged.Count -eq 0 -and $o.bounced.Count -eq 1 -and $o.bounced[0].it.id -eq 'T-A' -and $o.error -match '^T-B: ') "one-at-a-time: RED T-A then T-B throws = T-A still bounced, nothing merged, error reported (escaped '$esc')"
  # the run body lands whatever Resolve-Batch returns: $merged comes from $rb.merged, the error only reaches the report
  $tb = $top[0].Body.Extent.Text
  $rbFn = @($ast.FindAll({ param($n) $n -is [System.Management.Automation.Language.FunctionDefinitionAst] -and $n.Name -eq 'Resolve-Batch' }, $true))
  $rbTry = $rbFn.Count -eq 1 -and @($rbFn[0].Body.FindAll({ param($n) $n -is [System.Management.Automation.Language.TryStatementAst] -and $n.Body.Extent.Text -match 'Try-Batch \(@\(\$o\.merged\) \+ @\(\$it\)\)' -and $n.CatchClauses.Count -eq 1 }, $true)).Count -eq 1
  Check ($rbTry -and ($tb -match '\$trainErr = \$rb\.error') -and ($tb -notmatch 'throw \$trainErr')) "AST: Resolve-Batch catches the one-at-a-time Try-Batch, the run body carries the error to the report without rethrowing before landing (try/catch=$rbTry)"

  # AST: the run body routes through Resolve-Batch, bounces only $bounced, and verdict INFRA wins
  $tb = $top[0].Body.Extent.Text
  Check (($tb -match '\$rb = Resolve-Batch \$batch \$res') -and ($tb -match '\$bounced = @\(\$rb\.bounced\)') -and ($tb -match "foreach \(\`$b in \`$bounced\) \{\s*Ledger set \`$b\.it\.id red-merge") -and ($tb -match "\`$verdict = if \(\`$infra\) \{ 'INFRA' \}")) 'AST: run body = Try-Batch -> Resolve-Batch, red-merge only for $bounced, verdict INFRA when $infra'
  Check ((Get-FileHash -Algorithm SHA256 $cfgPath).Hash -eq $cfgSha0) "merge_train.json unchanged by the selftest (sha256 $($cfgSha0.Substring(0,12)))"

  # --- HB-TRAIN-DRAIN: drain driver on a fake main checkout (race/first-frame-parity checked out, main + integration
  # branches), fake queue, fake pass child, fake release_due / publish. Never reads the live queue, never runs a game.
  $sel = @(Select-Batch @(@{ id = 'A' }, @{ id = 'B' }, @{ id = 'S'; solo = $true }, @{ id = 'C' }) 3)
  $sel2 = @(Select-Batch @(@{ id = 'S'; solo = $true }, @{ id = 'A' }) 3)
  Check ((($sel | ForEach-Object id) -join ',') -eq 'A,B' -and (($sel2 | ForEach-Object id) -join ',') -eq 'S') "drain: Select-Batch keeps the old pick (stops before a solo row, a solo head merges alone): $(($sel | ForEach-Object id) -join ',') / $(($sel2 | ForEach-Object id) -join ',')"
  $flipRows = [ordered]@{ 'D4-FLIP-L29-L30' = 'flip two knobs'; 'D3-SUBSTATE-FLIP' = 'x'; 'MATCH-PORTSTATE-X' = 'A gate move when ON = its own decided-flip decision'; 'PORT-OT-HUD' = 'Port the HUD'; 'HB-FLIPPER' = 'not a flip id'; 'PORT-X' = 'flip the default after review' }
  $want = @{ 'D4-FLIP-L29-L30' = $true; 'D3-SUBSTATE-FLIP' = $true; 'MATCH-PORTSTATE-X' = $true; 'PORT-OT-HUD' = $false; 'HB-FLIPPER' = $false; 'PORT-X' = $false }
  $bad = @($flipRows.Keys | Where-Object { (Test-FlipRow @{ id = $_; title = $flipRows[$_] }) -ne $want[$_] })
  Check ($bad.Count -eq 0) "drain: Test-FlipRow by id (-FLIP / FLIP-) or 'decided flip' in the title (wrong: $($bad -join ','))"
  function Get-LedgerRow([string]$id) { return @{ id = $id; title = $(if ($id -eq 'Q-DEC') { 'L-99 decided flip of X' } else { 'ordinary port row' }) } }
  $bf = @(Get-BatchFlips @(@{ id = 'Q-1' }, @{ id = 'Q-DEC' }, @{ id = 'Q-2-FLIP' }))
  Check (($bf -join ',') -eq 'Q-DEC,Q-2-FLIP') "drain: Get-BatchFlips reads each row's ledger title ($($bf -join ','))"
  # HB-TRAIN-FLIP-DECISION: the ledger decision field (an L-n) plus a title naming a default turned ON. Rows modelled on
  # the live ledger of 2026-11-11 (D3-CF-COLLLUT, L115-ADMIT-2, PORT-STATE-03, a near-match row citing L-115 in its note).
  $decRows = @(
    @{ case = 'decided flip (decision L-26, default ON title, no FLIP in id)'; want = $true; row = @{ id = 'D3-CF-COLLLUT'; title = 'COLL_LUT faithful path default ON? (U-9775 resolved with the A5 timer knob)'; decision = 'L-26'; note = '' } }
    @{ case = 'decided flip as a ledger object (PSCustomObject, decision L-92)'; want = $true; row = [pscustomobject]@{ id = 'D4-ST5-ON'; title = 'L-91 (a): MASHED_ST5_LEG2 default ON (opt-out MASHED_NO_ST5_LEG2=1)'; decision = 'L-92'; note = '' } }
    @{ case = 'near-match row citing L-n in note and title, no decision'; want = $false; row = @{ id = 'MATCH-NEAR-0048e820'; title = 'admit 0x0048e820 (85.5%) under L-115 / L-118, default ON once admitted'; decision = ''; note = 'L-115 L-117 L-118 near-match' } }
    @{ case = 'decision carrier with its own L-n, no default ON'; want = $false; row = @{ id = 'L115-ADMIT-2'; title = 'Decision carrier: admit 0x0048e820 (85.5%) per function under L-115/L-117'; decision = 'L-118'; note = 'L-115' } }
    @{ case = 'decided port behind a default-OFF knob'; want = $false; row = @{ id = 'PORT-STATE-03'; title = 'Frontend menu records at their original address 0x00898ac0 behind a default-OFF knob'; decision = 'L-100'; note = '' } }
    @{ case = 'decision field not an L-n (P0-1)'; want = $false; row = @{ id = 'D1-R1'; title = 'capture default ON'; decision = 'P0-1'; note = '' } }
    @{ case = 'plain row'; want = $false; row = @{ id = 'PORT-OT-HUD'; title = 'Port the HUD'; decision = ''; note = '' } }
  )
  $badDec = @($decRows | Where-Object { (Test-FlipRow $_.row) -ne $_.want } | ForEach-Object case)
  Check ($badDec.Count -eq 0) "flip-decision: decision L-n + 'default ON' title flags, near-match L-n citations / default-OFF / plain rows do not (wrong: $($badDec -join '; '))"
  function Get-LedgerRow([string]$id) { switch ($id) { 'Q-COLL' { return @{ id = $id; title = 'COLL_LUT default ON?'; decision = 'L-26' } } 'Q-NEAR' { return @{ id = $id; title = 'admit 0x0048e820 under L-115'; decision = ''; note = 'L-117' } } default { return @{ id = $id; title = 'ordinary port row'; decision = '' } } } }
  $bf2 = @(Get-BatchFlips @(@{ id = 'Q-1' }, @{ id = 'Q-NEAR' }, @{ id = 'Q-COLL' }))
  Check (($bf2 -join ',') -eq 'Q-COLL') "flip-decision: Get-BatchFlips reads the ledger decision field (flags $($bf2 -join ','), not the near-match row)"
  $rjT = Join-Path $DATA 'pr_unit.json'
  Write-PassResult $rjT @{ verdict = 'GREEN'; merged = @('A'); hashes = @{ gctrl = @('1', '2', '3'); b = @('4', '5', '6') }; text_only = $false; refs = @{ gctrl = @('1', '2', '3'); b = @('4', '5', '6') } }
  $rb2 = Get-Content -LiteralPath $rjT -Raw | ConvertFrom-Json -AsHashtable
  Check ($rb2.verdict -eq 'GREEN' -and (@($rb2.merged) -join ',') -eq 'A' -and (@($rb2.hashes.b) -join '/') -eq '4/5/6' -and @([IO.File]::ReadAllBytes($rjT) | Where-Object { $_ -eq 13 }).Count -eq 0) 'drain: pass result file round-trips (verdict, merged, hashes), LF only'

  $fd = Join-Path ([IO.Path]::GetTempPath()) "mtrain_selftest_drain_$TS"
  New-Item -ItemType Directory -Force -Path $fd | Out-Null
  $MAIN = $fd
  $null = Git -C $fd init -q -b race/first-frame-parity
  Git -C $fd config user.email t@t | Out-Null; Git -C $fd config user.name t | Out-Null
  [IO.File]::WriteAllText((Join-Path $fd 'a.txt'), "base`n"); Git -C $fd add a.txt | Out-Null; Git -C $fd commit -q -m base | Out-Null
  Git -C $fd branch main | Out-Null; Git -C $fd branch integration | Out-Null
  $DCFG.lock_wait_sec = 0; $DCFG.lock_tries = 3
  $refsT = @{ gctrl = @('aaaaaaaa', 'bbbbbbbb', 'cccccccc'); b = @('dddddddd', 'dddddddd', 'dddddddd') }
  function Read-Refs { return $script:refsNow }
  function Get-Queue { if ($script:qLeft -le 0) { return @() }; return @(1..$script:qLeft | ForEach-Object { @{ id = "Q$_"; branch = "orch/Q$_" } }) }
  function Advance-Int {   # what a GREEN (or partly GREEN) pass does to the integration branch
    $tree = (Git -C $fd rev-parse 'integration^{tree}') | Select-Object -First 1
    $c = (Git -C $fd commit-tree $tree '-p' integration '-m' "pass $($script:passN)") | Select-Object -First 1
    Git -C $fd update-ref refs/heads/integration $c | Out-Null
  }
  function Invoke-PassChild([string]$rj) {
    $k = $script:dplan[$script:passN]; $script:passN++
    $script:ev += "pass$($script:passN)"
    if ($k.advance) { Advance-Int }
    if ($k.take) { $script:qLeft -= $k.take }
    if ($k.lockfile) { [IO.File]::WriteAllText((Join-Path $fd '.git\index.lock'), '') }
    return $k.res
  }
  function Wait-Drain([int]$s) { $script:ev += 'wait'; Remove-Item -LiteralPath (Join-Path $fd '.git\index.lock') -Force -ErrorAction SilentlyContinue }
  function Invoke-ReleaseDue { $script:ev += "release$($script:passN)"; $r = $script:rel[$script:passN]; if ($null -eq $r) { $r = 1 }; return @{ rc = $r; first = @('DUE x', 'NOT_DUE x', 'ERROR x')[$r]; text = @() } }
  function Invoke-Publish([string]$plog) { $script:ev += "publish$($script:passN)"; Add-Content -LiteralPath $plog -Value 'deployed fake'; return $script:pubRc }
  function PR([string]$v, [hashtable]$x) {
    $o = @{ verdict = $v; data = 'fake'; merged = @('Q1'); bounced = @(); gates = 'G-CTRL aaaaaaaa/bbbbbbbb/cccccccc  B dddddddd/dddddddd/dddddddd'; hashes = @{ gctrl = $refsT.gctrl; b = $refsT.b }; text_only = $false; refs = @{ gctrl = $refsT.gctrl; b = $refsT.b }; infra = ''; error = '' }
    foreach ($kk in $x.Keys) { $o[$kk] = $x[$kk] }; return $o
  }
  function Heads { @('race/first-frame-parity', 'main', 'integration' | ForEach-Object { (Git -C $fd rev-parse $_) | Select-Object -First 1 }) }
  function Drain-Case([object[]]$steps, [int]$q, [hashtable]$rel, [int]$pubRc = 0) {
    $script:dplan = $steps; $script:passN = 0; $script:ev = @(); $script:qLeft = $q; $script:rel = $rel; $script:pubRc = $pubRc; $script:refsNow = $refsT
    $d = Invoke-Drain 'integration'
    Say "  drain case events: $($script:ev -join ',') -> $($d.verdict) $($d.stop)"
    return $d
  }
  $G = @{ advance = $true; take = 1 }
  # 1. three GREEN passes (one text-only), release DUE after pass 2: publish runs before pass 3; stops on an empty queue
  $d = Drain-Case @(
    ($G + @{ res = (PR 'GREEN' @{}) }),
    ($G + @{ res = (PR 'GREEN' @{ merged = @('Q2', 'Q3'); hashes = $null; text_only = $true; gates = 'not run: text-only change (no build input touched)' }) }),
    ($G + @{ res = (PR 'GREEN' @{}) })) 3 @{ 2 = 0 }
  $h = Heads
  Check ($d.verdict -eq 'GREEN' -and $script:passN -eq 3 -and $h[0] -eq $h[2] -and $h[1] -eq $h[2] -and ($script:ev -join ',') -eq 'pass1,release1,pass2,release2,publish2,pass3,release3' -and $d.report[-1] -match 'queue drained') "drain 1: GREEN x3 (one text-only) = parity and main follow integration after every pass, publish (DUE after pass 2) finishes before pass 3, stops on the empty queue (events $($script:ev -join ','))"
  # 2. GREEN then RED: stop after pass 2, parity/main stay at pass 1's tip although the RED pass landed a GREEN row
  $h0 = Heads
  $d = Drain-Case @(($G + @{ res = (PR 'GREEN' @{}) }), ($G + @{ res = (PR 'RED' @{ merged = @('Q2'); bounced = @(@{ id = 'Q3'; why = 'default gates changed' }) }) })) 5 @{}
  $h = Heads
  Check ($d.verdict -eq 'RED' -and $script:passN -eq 2 -and $h[0] -eq $h[1] -and $h[0] -ne $h[2] -and $d.stop -match 'Q3: default gates changed' -and ($script:ev -join ',') -eq 'pass1,release1,pass2') "drain 2: RED pass 2 = stop, no fast-forward and no release after it (verdict $($d.verdict), stop '$($d.stop)')"
  Git -C $fd update-ref refs/heads/race/first-frame-parity $h[2] | Out-Null; Git -C $fd reset -q --hard $h[2] | Out-Null; Git -C $fd update-ref refs/heads/main $h[2] | Out-Null
  # 3. INFRA: stop, nothing moves
  $h0 = Heads
  $d = Drain-Case @(@{ take = 0; res = (PR 'INFRA' @{ merged = @(); infra = 'D3D9-NOTAVAIL in H_DEF' }) }) 4 @{}
  $h = Heads
  Check ($d.verdict -eq 'INFRA' -and $script:passN -eq 1 -and ($h -join ',') -eq ($h0 -join ',') -and ($script:ev -join ',') -eq 'pass1') "drain 3: INFRA = stop after the pass, nothing fast-forwarded, no release ($($d.stop))"
  # 4. GREEN whose hashes differ from the refs re-read after the pass, and refs that moved during the pass
  $d = Drain-Case @(($G + @{ res = (PR 'GREEN' @{ hashes = @{ gctrl = @('aaaaaaaa', 'eeeeeeee', 'cccccccc'); b = $refsT.b } }) })) 4 @{}
  $h = Heads
  Check ($d.verdict -eq 'HASH' -and $h[0] -ne $h[2] -and $d.stop -match 'aaaaaaaa/eeeeeeee/cccccccc' -and ($script:ev -join ',') -eq 'pass1') "drain 4a: GREEN with a hash unlike the refs = stop HASH, no fast-forward ($($d.stop))"
  Git -C $fd reset -q --hard $h[2] | Out-Null; Git -C $fd update-ref refs/heads/main $h[2] | Out-Null
  $d = Drain-Case @(($G + @{ res = (PR 'GREEN' @{ refs = @{ gctrl = @('11111111', 'bbbbbbbb', 'cccccccc'); b = $refsT.b } }) })) 4 @{}
  Check ($d.verdict -eq 'HASH' -and $d.stop -match 'refs changed during the pass') "drain 4b: refs in merge_train.json moved during the pass = stop HASH ($($d.stop))"
  $h = Heads; Git -C $fd reset -q --hard $h[2] | Out-Null; Git -C $fd update-ref refs/heads/main $h[2] | Out-Null
  # 5. FLIP: the pass refused the batch, nothing merged
  $h0 = Heads
  $d = Drain-Case @(@{ take = 0; res = (PR 'FLIP' @{ merged = @(); flips = @('D4-FLIP-X') }) }) 2 @{}
  $h = Heads
  Check ($d.verdict -eq 'FLIP' -and ($h -join ',') -eq ($h0 -join ',') -and $d.stop -match 'D4-FLIP-X') "drain 5: decided-flip batch = stop FLIP, nothing moved ($($d.stop))"
  # 6. index.lock in the main checkout at the fast-forward: retried after a wait, then GREEN
  $d = Drain-Case @(($G + @{ lockfile = $true; res = (PR 'GREEN' @{}) })) 1 @{}
  $h = Heads; $logTxt = Get-Content -LiteralPath $LOG -Raw
  Check ($d.verdict -eq 'GREEN' -and $h[0] -eq $h[2] -and $h[1] -eq $h[2] -and ($script:ev -join ',') -eq 'pass1,wait,release1' -and $logTxt -match 'blocked by index\.lock \(attempt 1/3\)' -and $logTxt -match 'fast-forwarded .* on attempt 2') "drain 6: index.lock = one wait and a retry, then both heads equal integration (events $($script:ev -join ','))"
  # 7. main not an ancestor of integration: refused before anything moves
  $side = (Git -C $fd commit-tree ((Git -C $fd rev-parse 'main^{tree}') | Select-Object -First 1) '-p' main '-m' side) | Select-Object -First 1
  Git -C $fd update-ref refs/heads/main $side | Out-Null
  $pBefore = (Git -C $fd rev-parse race/first-frame-parity) | Select-Object -First 1
  $d = Drain-Case @(($G + @{ res = (PR 'GREEN' @{}) })) 2 @{}
  $h = Heads
  Check ($d.verdict -eq 'ERROR' -and $d.stop -match 'main @ .* is not an ancestor' -and $h[0] -eq $pBefore -and $h[1] -eq $side) "drain 7: main diverged = refused, race/first-frame-parity not moved either ($($d.stop))"
  Git -C $fd update-ref refs/heads/main $h[0] | Out-Null
  # 8. main checkout on another branch: refused
  Git -C $fd checkout -q -b other | Out-Null
  $d = Drain-Case @(($G + @{ res = (PR 'GREEN' @{}) })) 2 @{}
  Check ($d.verdict -eq 'ERROR' -and $d.stop -match "has 'other' checked out") "drain 8: main checkout not on race/first-frame-parity = refused ($($d.stop))"
  Git -C $fd checkout -q race/first-frame-parity | Out-Null
  $h = Heads; Git -C $fd reset -q --hard $h[2] | Out-Null; Git -C $fd update-ref refs/heads/main $h[2] | Out-Null
  # 9. release ERROR and a failed publish both stop the drain after the fast-forward
  $d = Drain-Case @(($G + @{ res = (PR 'GREEN' @{}) }), ($G + @{ res = (PR 'GREEN' @{}) })) 2 @{ 1 = 2 }
  Check ($d.verdict -eq 'ERROR' -and $d.stop -match 'release_due\.py rc=2' -and $script:passN -eq 1) "drain 9a: release_due ERROR = stop before the next pass ($($d.stop))"
  $d = Drain-Case @(($G + @{ res = (PR 'GREEN' @{}) }), ($G + @{ res = (PR 'GREEN' @{}) })) 2 @{ 1 = 0 } 1
  Check ($d.verdict -eq 'ERROR' -and $d.stop -match 'publish\.sh rc=1' -and $script:passN -eq 1 -and ($script:ev -join ',') -eq 'pass1,release1,publish1') "drain 9b: publish.sh failed = stop, no retry, no next pass ($($d.stop))"
  # 10. empty queue at the start: no pass at all
  $d = Drain-Case @() 0 @{}
  Check ($d.verdict -eq 'GREEN' -and $script:passN -eq 0 -and $d.report[4] -match 'queue empty') 'drain 10: empty queue = no pass, GREEN'
  # HB-TRAIN-CONFLICT-SKIP: a RED pass whose only bounces were merge conflicts does not stop the drain
  $cb = @(@{ id = 'Q2'; why = 'conflict needs a hand merge (frida-sweep recipe): hooks.csv'; conflict = $true })
  $d = Drain-Case @(($G + @{ res = (PR 'RED' @{ conflict_only = $true; bounced = $cb }) }), ($G + @{ res = (PR 'GREEN' @{}) })) 2 @{}
  $h = Heads
  Check ($d.verdict -eq 'GREEN' -and $script:passN -eq 2 -and $h[0] -eq $h[2] -and $h[1] -eq $h[2] -and ($script:ev -join ',') -eq 'pass1,release1,pass2,release2' -and $d.report[-1] -match 'set aside on a merge conflict: Q2') "drain 12: conflict-only RED pass = the rest is fast-forwarded and released, drain goes on (events $($script:ev -join ','))"
  $d = Drain-Case @((@{ take = 1; res = (PR 'RED' @{ conflict_only = $true; merged = @(); bounced = $cb; hashes = $null; gates = '' }) }), ($G + @{ res = (PR 'GREEN' @{}) })) 2 @{}
  Check ($d.verdict -eq 'GREEN' -and ($script:ev -join ',') -eq 'pass1,pass2,release2' -and $d.report[-1] -match 'nothing merged, no fast-forward') "drain 12: conflict-only pass with nothing merged = no fast-forward, no release, next pass (events $($script:ev -join ','))"
  $d = Drain-Case @(($G + @{ res = (PR 'RED' @{ conflict_only = $false; bounced = @(@{ id = 'Q2'; why = 'default gates changed'; conflict = $false }) }) })) 2 @{}
  Check ($d.verdict -eq 'RED') 'drain 12: a gate-RED bounce still stops the drain'
  # 11. AST: the drain dispatch sits before Write-TrainPid and the batch pick, and the pass refuses flips before Enter-PassPriority
  $txt = Get-Content -LiteralPath $PSCommandPath -Raw
  $iDrain = $txt.LastIndexOf('$d = Invoke-Drain $INT'); $iPid = $txt.LastIndexOf("`nWrite-TrainPid"); $iFlip = $txt.LastIndexOf('$flips = @(Get-BatchFlips $batch)'); $iPrio = $txt.LastIndexOf("`nEnter-PassPriority")
  Check ($iDrain -gt 0 -and $iDrain -lt $iPid -and $iFlip -gt $iPid -and $iFlip -lt $iPrio) "drain 11: dispatch before the pass body, flip refusal before the priority hold and any merge (offsets $iDrain < $iPid, $iFlip < $iPrio)"
  $locks = @([regex]::Matches(($ast.FindAll({ param($n) $n -is [System.Management.Automation.Language.FunctionDefinitionAst] -and $n.Name -in 'Invoke-Drain', 'Advance-Main', 'Invoke-ReleaseStep', 'Invoke-PassChild' }, $true) | ForEach-Object { $_.Extent.Text }) -join "`n", 'LOCKPY|prio-hold|solo-begin'))
  Check ($locks.Count -eq 0) "drain 11: the drain driver takes no mashed_lock / priority hold of its own (only the passes do) ($($locks.Count) refs)"
  Remove-Item -LiteralPath $fd -Recurse -Force -ErrorAction SilentlyContinue

  Say "SELFTEST done: $fails failure(s)"
  Write-Output "SELFTEST $(if ($fails) { 'FAIL' } else { 'PASS' }) ($fails failure(s)) log $LOG"
  exit $(if ($fails) { 1 } else { 0 })
}

$INT = if ($env:MASHED_MTRAIN_INT) { $env:MASHED_MTRAIN_INT } else { $CFG.integration_branch }
# HB-TRAIN-DRAIN: the driver takes no lock and no worktree itself; each pass is a child process with its own PID file,
# reclaim, priority hold and REPORT.
if ($Drain -and -not $Plan) {
  Say "drain: start (max_passes $($DCFG.max_passes), passes$(if ($Full) { ' -Full' })$(if ($Max -gt 0) { " -Max $Max" }), log $LOG)"
  $d = Invoke-Drain $INT
  $d.report | Write-Output
  exit $(if ($d.verdict -eq 'GREEN') { 0 } else { 1 })
}

Write-TrainPid
if (-not $Plan) { Reclaim-DeadTrains }

# ---- 1. pick the batch -------------------------------------------------------------
$queue = @(Get-Queue)
if (-not $queue -or $queue.Count -eq 0) {
  Say 'queue empty'
  Write-PassResult $ResultJson @{ verdict = 'EMPTY'; data = $DATA; merged = @(); bounced = @() }
  Write-Output "REPORT merge-train`nverdict: GREEN`nbranch: (nothing queued)`nnext: none"; exit 0
}
$cap = if ($Max -gt 0) { $Max } else { [int]$CFG.batch_max }
$batch = @(Select-Batch $queue $cap)
$base = (Git rev-parse $INT) | Select-Object -First 1
Say "batch: $(($batch | ForEach-Object { "$($_.id)=$($_.branch)" }) -join ' ')  base $INT@$($base.Substring(0,9))"
if ($Plan) { exit 0 }
# HB-TRAIN-DRAIN: a drain pass never merges a decided flip; it stops before any merge and the lead runs the flip alone
# with -Full after committing the refs (MERGE_TRAIN.md "Decided default flips").
$flips = @(); if ($NoFlip -or $Full) { $flips = @(Get-BatchFlips $batch) }
$FullEff = Resolve-FullFlag ([bool]$Full) ([bool]$ForceFull) $flips
if ($Full -and -not $FullEff) { Say "-Full dropped: no decided-flip row in the batch (HB-TRAIN-MATCH-TEXTONLY); full_set_paths still bring the full set in, -ForceFull keeps the old -Full" }
elseif ($FullEff) { Say "full set forced: $(if ($ForceFull) { '-ForceFull' } else { "-Full with decided flip(s) $($flips -join ', ')" })" }
if ($NoFlip) {
  if ($flips.Count) {
    Say "decided-flip row(s) in the batch: $($flips -join ', '); nothing merged, every row stays queued"
    Write-PassResult $ResultJson @{ verdict = 'FLIP'; data = $DATA; flips = $flips; batch = @($batch | ForEach-Object id); merged = @(); bounced = @() }
    Write-Output "REPORT merge-train`nverdict: FLIP`nbranch: $INT @ $($base.Substring(0,9))`nevidence: $LOG`ngates: not run: decided flip in the batch`ndecision: none`nnext: run [$($flips -join ' ')] alone with -Full after the refs are committed"
    exit 0
  }
}

# ---- 2. run the batch; on RED, one at a time ----------------------------------------
# HB-TRAINFIX: steps 2 and 3 run inside a script-scope try/finally (see Clear-Temps), never a dot-sourcing wrapper
# HB-MASHEDLOCK-EARLY-PRIORITY: the priority hold opens first thing inside it and the finally releases it, so every
# exit (GREEN, RED, conflict, bounce, throw) ends it. Queue empty and -Plan exit above, before any hold exists.
try {
Enter-PassPriority
$res = Try-Batch $batch
$infraGates = $res.gates
$rb = Resolve-Batch $batch $res
$res = $rb.res; $merged = @($rb.merged); $bounced = @($rb.bounced); $infra = $rb.infra; $trainErr = $rb.error
if ($trainErr) { Say "ERROR mid one-at-a-time: $trainErr; landing what already went GREEN, the rest stays queued" }
if ($infra) { Say "INFRA: $infra; nothing bounced for it, every row not merged stays queued" }
foreach ($b in $bounced) {
  Ledger set $b.it.id red-merge "merge train $TS`: $($b.why)"
  Ledger dequeue $b.it.id
}

# ---- 3. fast-forward orch/integration on GREEN --------------------------------------
$verdict = if ($infra) { 'INFRA' } elseif ($bounced.Count -or -not $merged.Count) { 'RED' } else { 'GREEN' }
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
  $line = "- $(Get-Date -Format 'yyyy-MM-dd HH:mm') $(($merged | ForEach-Object { "$($_.id) ($($_.branch))" }) -join ', '): $($res.gates).$(if ($bounced.Count) { " Bounced: $(Format-Bounced $bounced)." }) Run data $DATA"
  $il = Join-Path $wt 're\orchestrator\INTEGRATION_LOG.md'
  Write-Lf $il (New-LogLines $il $line)
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
  foreach ($br in @($merged | ForEach-Object { $_.branch } | Select-Object -Unique)) { Cleanup-Step $br $INT }
}
if (-not $merged.Count -and $bounced.Count) {
  $gl = (@($bounced) | ForEach-Object { $_.gates } | Select-Object -Unique) -join ' | '
  $null = Add-RedLogLine "- $(Get-Date -Format 'yyyy-MM-dd HH:mm') RED, nothing merged. Bounced: $(Format-Bounced $bounced). Gates: $gl.$(if ($trainErr) { " Error: $trainErr." }) Run data $DATA"
}
} finally { try { Clear-Temps } finally { Exit-PassPriority } }

$final = (Git rev-parse --short $INT) | Select-Object -First 1
$report = @(
  "REPORT merge-train",
  "verdict: $verdict",
  "branch: $INT @ $final",
  "evidence: $LOG",
  "gates: $(if ($res) { $res.gates } elseif ($infra -and -not $bounced.Count) { "not scored: $infraGates" } else { ($bounced | ForEach-Object { $_.gates }) -join ' | ' })",
  "decision: none",
  "next: merged [$(($merged | ForEach-Object id) -join ' ')] bounced [$(($bounced | ForEach-Object { "$($_.it.id): $(if ($_.conflict) { 'set aside, ' })$($_.why)" }) -join '; ')]$(if ($infra) { " infra [$infra; rows not merged stay queued, re-run the train on a live display]" })$(if ($trainErr) { " error [$trainErr; rows not merged stay queued, re-run the train]" })"
)
$report | Set-Content (Join-Path $DATA 'REPORT.txt') -Encoding utf8
# HB-TRAIN-DRAIN: the structured outcome the drain driver reads (gates hashes, refs used, text-only, merged / bounced)
Write-PassResult $ResultJson @{
  verdict = $verdict; data = $DATA; batch = @($batch | ForEach-Object id); merged = @($merged | ForEach-Object id)
  bounced = @($bounced | ForEach-Object { @{ id = $_.it.id; why = $_.why; conflict = [bool]$_.conflict } }); infra = $infra; error = $trainErr
  conflict_only = [bool]($verdict -eq 'RED' -and $bounced.Count -and -not $trainErr -and @($bounced | Where-Object { -not $_.conflict }).Count -eq 0 -and ($merged.Count -or $bounced.Count -eq $batch.Count))
  gates = $(if ($res) { $res.gates } else { '' }); hashes = $(if ($res) { $res.hashes } else { $null }); text_only = [bool]($res -and $res.textOnly)
  refs = @{ gctrl = @($CFG.gctrl.ref); b = @($CFG.b.ref) }; base = $base; int_after = (Git rev-parse $INT | Select-Object -First 1)
}
$report | Write-Output
