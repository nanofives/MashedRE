# RESULT HB-TRAIN-SELFTEST-FAILMATCH (run 57, text row, no build, no gates, no game arm)

Base: orch/integration c6e9a71dd. Branch orch/HB-TRAIN-SELFTEST-FAILMATCH, worktree D:\MashedWT\HB-TRAIN-SELFTEST-FAILMATCH.

## Change (re/orchestrator/merge_train.ps1, Get-BinderFailLines)
The FAIL / FAILS, DIFFERENT, COMPILES (unexpected) and "N fail / N failures / N wrapper-shape failures" tests are now `-cmatch`
(were `-match`, case-insensitive: `\bFAILS?\b` matched "0 fail" in the passing line). The non-zero count rule
(`(?<![\d.])[1-9]\d* (fail|failures|wrapper-shape failures)\b`) is kept.

## Every failure line porttop_bind.py selftest can print (grep of `[selftest]` prints, lines 1671-1932) and how it is still caught
| porttop_bind.py line | Text | Caught by |
|---|---|---|
| 1671 | `N wrapper-shape failures` | non-zero count rule (N>=1) |
| 1685 | `as C FAILS` / `as C++ COMPILES (unexpected)` | `FAILS` upper-case / `COMPILES (unexpected)` |
| 1718 | `N compile, N fail` | non-zero count rule (`0 fail` passes) |
| 1756 | `FAIL N unnamed, N stale` | `FAIL` upper-case |
| 1829 | `N fail (unit test: ...)` | non-zero count rule |
| 1875 | `plan DIFFERENT`, `wrappers+stops DIFFERENT N` | `DIFFERENT` |
| 1910 | `refs_file DIFFERENT` | `DIFFERENT` |
| 1930 | `BINDINGS.tsv ...: DIFFERENT` | `DIFFERENT` |
| 1932 | closing `PASS` / `FAIL N` | excluded by the existing closing-line rule |
Detail lines (`FAIL: 0x... stays listed`, in the refs `drop` text of line 1910) carry an upper-case FAIL and ride on a line already caught.
Cases not changed: lower-case `fails (expected)` (line 1685, C++ expected to fail) is no longer flagged, which is correct.

## -SelfTest cases added (3)
1. the exact passing line `... 3 compile, 0 fail` present only after the merge -> ok, new 0 (pass stands)
2. `2 compile, 1 fail` line (with `FAIL 1` closing) -> NEW failure, pass fails
3. upper-case `as C FAILS` line -> NEW failure, pass fails
All 3 print SELFTEST PASS (train_selftest.log).

## Checks
- `pwsh -NoProfile -File re/orchestrator/merge_train.ps1 -SelfTest`: all 3 new cases and all 5 existing binder cases PASS.
  **Total: 5 failure(s), not 0, and the same 5 fail on the unchanged orch/integration c6e9a71dd** (run from
  .worktrees\orch-integration, log kept in %TEMP%): `clean merged step: worktree and branch removed` and the four `guard:` cases
  (temp-worktree removal). Cause: `diag.py wt-remove` returns rc=1 with a Python Traceback inside the fake temp repo
  ("cleanup ...: worktree removal refused"), unrelated to the binder code. Not touched here, needs its own row.
- Real selftest of the head: mashed_lock.py status = free (no PRIORITY-HOLD/SOLO); `py -3.12 re/tools/bind/porttop_bind.py selftest`
  (under mashed_lock run) rc=0, closing `[selftest] PASS`, output binder_selftest.out (25 lines).
  Get-BinderFailLines (new) over it: **0 lines returned**. The old function over the same output returned 3 false positives
  (`0 fail` x2: lib kind and C emit of the 4 held rvas, and `fails (expected)`). See faillines_new.txt, run_faillines.ps1.

Files: verify/trainfailmatch_20261127/{RESULT..., train_selftest.log, binder_selftest.out, faillines_new.txt, run_faillines.ps1}.
