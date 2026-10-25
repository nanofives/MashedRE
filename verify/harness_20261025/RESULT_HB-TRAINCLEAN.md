# RESULT HB-TRAINCLEAN

Verdict: GREEN. No MASHED.exe / mashed_re.exe launched, no gate or D2 arm run, no .asi deployed, live ledger touched only for this row.

## (a) LF-to-CRLF warnings on ledger snapshot files

Cause: `roadmap.ps1` already writes LF (HB-EOL), but this checkout has `core.autocrlf=true` and no attribute for the
ledger dir, so `git add` of an LF file still prints "LF will be replaced by CRLF".

Fix (smallest blast radius, 3 lines in `.gitattributes`, no script change):
`re/orchestrator/ledger/**`, `re/orchestrator/DECISIONS.md`, `re/orchestrator/INTEGRATION_LOG.md` get `text eol=lf`.

Proof, throwaway repo with `core.autocrlf=true`, 3 ledger-shaped files (roadmap.json, events.jsonl, merge_queue.json):
- without the attributes: `git add` printed 3 warnings (one per file).
- with the attributes: `git add` printed nothing (rc=0), commit clean, `git ls-files --eol` shows `i/lf w/lf attr/text eol=lf` for all 3.

## (b) Merge-train leftovers

Findings in `merge_train.ps1` (at 70fac88db):
1. HB-TRAIN3's row worktree: run 3 executed the pre-HB-TRAIN3 script (the cleanup it added is part of the branch being merged), so `Cleanup-Step` did not exist yet for that run.
2. `mtrain-20261025_000339-a922`: temp removal was a plain call, not guarded. Any exit that skipped the explicit `Remove-Temp` leaked it:
   a throw anywhere in steps 2-3, a ledger failure, and the foreign-tip path (`$merged` emptied, so the `if ($merged.Count)` block holding `Remove-Temp $wt` never ran).
   HB-SOLOSTALE only reclaims it on the NEXT run.
3. `Cleanup-Step` did not check that a row worktree lives under `.worktrees\`.

Changes:
- `New-Temp` registers each temp in `$script:Temps`, `Remove-Temp` unregisters it after a successful `diag.py wt-remove` (rc 0).
- New `Invoke-Guarded` runs steps 2-3 (dot-sourced, same scope) in try/finally and removes every still-registered temp. Removal goes only through `Remove-Wt` = `py.exe -3.12 scripts/diag.py wt-remove`. No `git worktree remove`.
- `Cleanup-Step` skips (worktree and branch kept, logged) any worktree not under `<MAIN>\.worktrees\`. Dirty worktrees were already refused by wt-remove and stay refused.

Proof: `merge_train.ps1 -SelfTest` (fake repo under %TEMP%, real diag.py copy, no live queue), 0 failures. New checks:
- body ends without removing its temp (foreign-tip shape): guard removed it, registry empty
- forced `throw` inside the body: exception propagates AND the finally block removed the temp worktree
- body that cleans up after itself: nothing left
- two temps, body removes one: guard removes only the other
- row worktree outside `.worktrees\`: skipped, worktree and branch kept
Existing HB-EOL / HB-WTCLEAN / HB-D2RERUN checks still PASS. Full log: `mtrain_selftest_HB-TRAINCLEAN.txt`. Script parses with 0 errors. No live train run.

Not touched (per instruction): `.worktrees/HB-TRAIN3` and `mtrain-20261025_000339-a922`. At the time of checking, neither appeared in `git worktree list` or `.worktrees\`, so the lead may already have removed them.
