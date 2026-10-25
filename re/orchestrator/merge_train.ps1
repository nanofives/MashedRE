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
param([switch]$Full, [switch]$Plan, [int]$Max = 0, [switch]$KeepTemp, [switch]$SelfTest)
$ErrorActionPreference = 'Stop'

$HERE = $PSScriptRoot
$MAIN = 'C:\Users\maria\Desktop\Proyectos\Mashed'
$CFG = Get-Content (Join-Path $HERE 'merge_train.json') -Raw | ConvertFrom-Json
$RM = Join-Path $HERE 'roadmap.ps1'
$LOCKPY = Join-Path $HERE 'mashed_lock.py'
$DIAG = Join-Path $MAIN 'scripts\diag.py'
$TS = Get-Date -Format 'yyyyMMdd_HHmmss'
$DATA = Join-Path $(if ($SelfTest) { Join-Path ([IO.Path]::GetTempPath()) 'mtrain_selftest' } else { $CFG.data }) $TS
New-Item -ItemType Directory -Force -Path $DATA | Out-Null
$LOG = Join-Path $DATA 'merge_train.log'

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
function Sha8([string]$p) { if (Test-Path $p) { (Get-FileHash -Algorithm SHA256 $p).Hash.Substring(0, 8).ToLower() } else { 'MISSING' } }
function Like-Any([string]$f, $globs) { foreach ($g in $globs) { if ($f -like $g) { return $true } } return $false }

# ---- helpers -------------------------------------------------------------------------
function New-Temp([string]$at) {
  $p = Join-Path $MAIN ".worktrees\mtrain-$TS-$([guid]::NewGuid().ToString('N').Substring(0,4))"
  Git -C $MAIN worktree add --detach $p $at | Out-Null
  if ($script:GITRC -ne 0) { throw "worktree add failed: $p" }
  return $p
}
function Remove-Wt([string]$p) {
  # SAFE removal only (never `git worktree remove --force`; WORKTREE-SYMLINK-WIPE). Returns the diag.py exit code.
  Push-Location $MAIN
  try { $o = & py.exe -3.12 $DIAG wt-remove $p 2>&1; $rc = $LASTEXITCODE; Say "wt-remove $p rc=$rc $($o | Select-Object -First 1)" } finally { Pop-Location }
  return $rc
}
function Remove-Temp([string]$p) {
  if ($KeepTemp -or -not $p) { return }
  Remove-Wt $p | Out-Null
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
    if ((Remove-Wt $w) -ne 0) { Say "  cleanup $branch`: worktree removal refused ($w), worktree and branch kept"; return }
  }
  if (-not $intWt) { Say "  cleanup $branch`: no worktree holds $intBranch to judge merged-ness, branch kept"; return }
  # -d judges merged-ness against HEAD of the repo it runs in, so run it where the integration branch is checked out
  $o = Git -C $intWt branch --delete $branch
  if ($script:GITRC -ne 0) { Say "  cleanup $branch`: git branch --delete refused, branch kept: $(($o | Select-Object -First 1))" }
  else { Say "  cleanup $branch`: worktree and branch removed" }
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
  if ($nd -ne 3 -or $short.Count) { $ok = $false }; $notes += "D2 $nd/3$(if($short.Count){" ($($short.Count) arm(s) still under $($f.d2_min_lines) lines after $maxRe rerun(s))"})"
  # 13 tracks: table identical to the reference
  Ensure-Root $CFG.root13
  $o13 = Join-Path $DATA 'tracks'
  $tok = & py.exe -3.12 $LOCKPY solo-begin --priority --label "merge-train13:$TS" --ttl 14400 | Where-Object { $_ -match '^TOKEN ' }
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

  # --- HB-D2RERUN: fake arm runner, D2_DEF_2 comes back short (1485 lines) on the first run
  $f = $CFG.full; $out = Join-Path $DATA 'd2arms'; New-Item -ItemType Directory -Force -Path $out | Out-Null
  $script:runs = @{}
  function Run-Arms([string]$wt, [string[]]$arms, [string]$out, [switch]$Solo) {
    foreach ($a in $arms) {
      $script:runs[$a] = 1 + [int]$script:runs[$a]
      $lines = switch ($script:Scenario) { 'short-once' { if ($a -eq 'D2_DEF_2' -and $script:runs[$a] -eq 1) { 1485 } else { 1700 } } 'short-always' { if ($a -eq 'D2_DEF_2') { 1485 } else { 1700 } } default { 1700 } }
      Write-Lf (Join-Path $out "$a.motion_diag.log") @(1..$lines | ForEach-Object { "row $_" })
    }
    return 0
  }
  $script:Scenario = 'short-once'
  $r = Run-D2Arms 'unused' $f $out
  Check ($r.short.Count -eq 0 -and $script:runs['D2_DEF_2'] -eq 2 -and $script:runs['D2_DEF_1'] -eq 1) "short-once: D2_DEF_2 rerun once (2 runs), others not rerun, 0 arms short"
  $script:runs = @{}; $script:Scenario = 'short-always'
  $r = Run-D2Arms 'unused' $f $out
  Check ($r.short.Count -eq 1 -and $script:runs['D2_DEF_2'] -eq (1 + $r.maxRe)) "short-always: reruns bounded at $($r.maxRe), arm still reported short (runs=$($script:runs['D2_DEF_2']))"
  $script:runs = @{}; $script:Scenario = 'ok'
  $r = Run-D2Arms 'unused' $f $out
  Check ($r.short.Count -eq 0 -and $script:runs['D2_DEF_2'] -eq 1) 'full-length logs: no rerun'

  Say "SELFTEST done: $fails failure(s)"
  Write-Output "SELFTEST $(if ($fails) { 'FAIL' } else { 'PASS' }) ($fails failure(s)) log $LOG"
  exit $(if ($fails) { 1 } else { 0 })
}

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
  if (-not (Test-Path $il)) { Write-Lf $il @('# orch/integration log (merge train, newest first)', '') }
  $body = @(Get-Content $il); $new = @($body[0], '', $line) + @($body | Select-Object -Skip 1 | Where-Object { $_ -ne '' })
  Write-Lf $il $new
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
