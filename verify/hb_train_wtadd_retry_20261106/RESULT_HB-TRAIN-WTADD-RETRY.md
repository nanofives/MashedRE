# RESULT HB-TRAIN-WTADD-RETRY (2026-11-06)

Verdict: GREEN. `merge_train.ps1 -SelfTest` passes end to end: 60 PASS, 0 FAIL (56 existing checks plus 4 new ones in 2 groups).
No game runs and no build (the change is to the harness only).

## Trigger

Train 20261106_122811, 13:05: after a batch RED, MATCH-S09-FU passed its full set on its own. Then the
temp `git worktree add` for the next row failed. `New-Temp` sent git's output to `Out-Null` and threw on the first
failure, so the run ended before the GREEN row landed. A manual add a minute later worked.

## What changed (`re/orchestrator/merge_train.ps1`)

1. **`New-Temp` retries the add.** The `git worktree add --detach` call now sits in `Invoke-WtAdd`, so the self-test can
   swap in a fake. New-Temp makes up to `$script:WtAddTries` = 3 attempts. Each attempt gets a fresh temp path. Each
   failure goes to the train log with git's rc and output
   (`worktree add attempt i/3 failed rc=<rc> at <ref> -> <path>: <git message>`). Between attempts it waits
   `$script:WtAddWaitSec` = 5 s and runs `git worktree prune`. A success after a failure is logged. Only the third
   failure throws, as `worktree add failed after 3 attempts at <ref>: <git message>`. Only a successful path is added to
   `$script:Temps`.
2. **One-at-a-time mode keeps what it already proved.** In `Resolve-Batch`, each per-row `Try-Batch` runs inside a
   `try/catch`. On a throw, the error is stored in `$o.error` and the one-at-a-time loop stops. The function then
   returns normally:
   - the first GREEN row (`$o.res`/`$o.merged`) and the bounces so far go back to the run body, which lands the GREEN
     row as usual (fast-forward `orch/integration`, ledger `merged`, dequeue, cleanup);
   - the row that threw and every row after it stay queued.

   The run body carries the error as `$trainErr` into the train log and into the REPORT's `next:` line
   (`error [<row>: <message>; rows not merged stay queued, re-run the train]`). It never rethrows it.
   An exception from the first (whole-batch) `Try-Batch` still propagates as before, because nothing is proven GREEN
   at that point. The existing script-scope `finally` still removes the temp worktrees and releases the priority hold.

## New self-test checks

| Check | Result |
|---|---|
| wtadd: fake add fails twice, then the real add succeeds. Temp created on attempt 3 and registered once. Both git messages (`fatal: fake add failure 1/2`) and the attempt-3 OK line are in the train log | PASS |
| wtadd: 3 failures. Throws after the last attempt with git's message, nothing registered | PASS |
| one-at-a-time: batch RED, T-A GREEN, then T-B throws. No exception escapes, T-A comes back as merged with `res.ok`, T-C is never tried, error = `T-B: worktree add failed ...` | PASS |
| one-at-a-time: T-A RED, then T-B throws. T-A is still bounced, nothing merged, error reported | PASS |
| AST: `Resolve-Batch` wraps `Try-Batch @($it)` in a try with one catch. The run body sets `$trainErr = $rb.error` and does not rethrow it | PASS |

Full self-test output (PASS/FAIL lines): `verify/hb_train_wtadd_retry_20261106/selftest_output.txt`.
Final line: `SELFTEST PASS (0 failure(s))`. The self-test also confirmed `merge_train.json` was unchanged (sha256 06DDBBFFDF7F...).

## Not covered

- The landing itself (ff of `orch/integration`, ledger writes) is checked through the AST plus the `Resolve-Batch`
  return value, not by running the live run body. The run body already lands whatever `$rb.merged` holds, and that
  code is unchanged.
- The retry addresses a transient add failure. The cause of the 13:05 failure was never logged (stderr was discarded),
  so it is still unknown. The next occurrence will now leave git's message in `merge_train.log`.
