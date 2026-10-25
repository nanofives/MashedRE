# RESULT HB-SOLOSTALE: stale solo token and leftover mtrain worktree

Verdict: GREEN. Branch orch/HB-SOLOSTALE, based on orch/HB-TRAIN3 (f672161c4).

## Problem
A merge train killed mid-run left its solo token (`pid=None`, ttl 14400 s) in `<TEMP>\mashed_machine_solo.lock`
and its `.worktrees\mtrain-<TS>-*` worktree. `solo_owner()` only broke a token whose bound `pid` was dead
or whose expiry had passed, and the train's 13-track `solo-begin` bound no PID, so the re-run waited out the token.

## Change
- `re/orchestrator/mashed_lock.py`: `solo-begin --owner-pid <pid>` (and `owner_pid=` in `solo_begin`) records the owner PID plus its
  process start time. `_owner_dead()` treats the token as dead when the PID is gone, or when the live PID started
  more than 2 s away from the recorded start (PID reuse). `solo_owner()` uses it, so the next `solo_begin`/lock
  acquire reclaims it. The `--ttl` expiry (default 4 h) stays as the fallback for tokens with no owner. `status` prints `owner_pid`.
- `re/orchestrator/merge_train.ps1`: the 13-track `solo-begin` passes `--owner-pid $PID`. Each run writes
  `<data>\<TS>\merge_train.pid` (pid + start unix seconds). At start (not in `-Plan`) `Reclaim-DeadTrains` runs
  `Remove-Wt` (`py -3.12 scripts/diag.py wt-remove`, never `git worktree remove --force`) on every
  `.worktrees\mtrain-<TS>-*` whose run is dead (no pid file, PID gone, or PID reused). The current run and live runs are skipped.

## Proof (private TEMP dir and lock name, no real lock or token touched, live queue not run)
`solostale_test.py` (log `solostale_test.log`), 8/8:
1. dead `owner_pid`, `pid=None`, 4 h ttl running: reclaimed in 0.06 s (the run 2 shape).
2. live `owner_pid`, matching start: still blocks (TimeoutError after 2 s).
3. live PID with a start time 1 h off (PID reuse): reclaimed.
4. ownerless token inside its ttl: still blocks (fallback intact).
5. ownerless token past ttl: reclaimed.
6. CLI `solo-begin --owner-pid` stores `owner_pid` and `owner_start`; blocks while the owner lives; reclaimed within 0.02 s of the owner being killed.

`traindead_test.ps1` (log `traindead_test.log`) on `Test-TrainDead` from merge_train.ps1: live train dead=False,
killed train dead=True, reused PID dead=True, no pid file dead=True. PASS.

Regressions: `mashed_lock_selftest.py` 6/6 PASS, `merge_train.ps1 -SelfTest` 0 failures.
The test caught one bug in the first draft (ISO start string round-tripped through ConvertFrom-Json); the pid file now stores unix seconds.

## Not exercised
`Reclaim-DeadTrains` itself (the `wt-remove` call on a real leftover worktree) was not run against the real repo, per the brief.
It reuses the existing `Remove-Wt` that HB-TRAIN3 already exercises in its self-test.
Gates: not run (no build or game code touched).
