# RESULT HB-MASHEDLOCK-SOLO-PRIORITY

Verdict: GREEN. TEXT-ONLY (lock tooling). No game boot, no real solo window, no change to the live
lock files. Every test ran against a private `MASHED_LOCK_DIR` / `TEMP` temp dir.

## Problem (run 29)

Merge-train pass `20261101_191148` (`D:/MashedWT/mtrain_data/20261101_191148/merge_train.log`) waited
from 19:39 with a priority marker pending. `MATCH-TOPDOWN-T01` opened a solo window at 19:32 and
`HB-ASI-EAX-57C420-gates` opened another at 19:54 with a 12 h expiry (the old `run --solo` default).
`solo_begin` never looked at priority markers, so a new window could always open ahead of the train.

## Change

`re/orchestrator/mashed_lock.py` (and the PowerShell twin `MachineLock.ps1`, `Enter-SoloLock`):

1. A NEW non-priority solo open now waits while any of these is true:
   - a live priority marker of another process is pending (`<name>_prio_<pid>.lock`, HB-LOCKPRIO)
   - a priority acquirer holds the game lock
   - it is less than `PRIO_SOLO_LINGER_SEC` = 120 s since that holder released

   The last two use one new file per priority process, `<name>_priolinger_<pid>.lock`. It holds
   `held=true` from acquire to release, and is stale only when that PID is dead (`owner_start`
   guards against PID reuse). On release it becomes `held=false` with a 120 s expiry. The held state
   is needed because the marker clears when the train takes the lock. Without it, a window opened
   during the train's hold and then drained it (case 4 below, first draft). The 120 s linger covers
   the gap of a few seconds between the train's `Run-Arms` calls in one pass. The wait does not
   count against the caller's `wait`, the same rule `_acquire` already uses for markers.
2. A window that is already open is never touched. It stays valid until `solo-end` or its expiry,
   and its token holders still pass priority markers (case 1).
3. Priority solo opens (the train's `run --priority --solo` and `solo-begin --priority`) do not wait
   on markers or lingers. They queue on the solo file itself, as before (case 6).
4. Ordinary game-lock acquires ignore the hold/linger file. HB-LOCKPRIO marker semantics are
   unchanged (selftest 14/14, including `marker-cleared-after-acquire`).
5. Default solo expiry `SOLO_TTL_SEC` = 3600 s for both `run --solo` (was 43200) and `solo-begin`
   (was 14400). PowerShell `Enter-SoloLock -TtlSec` default is 3600 (was 14400). In the
   2026-11-01 train logs one default gate run (G-CTRL + B) took 10 to 23 min, and the D2 solo block
   took 9 to 13 min. A longer step passes `--ttl` explicitly. The merge train already passes
   `--ttl 14400` for its 13-track window, and its D2 `run --solo` window is PID-bound and ends with
   the run.
6. `status` prints `PRIORITY-HOLD` / `PRIORITY-LINGER` lines.
7. Side fix found by the extended hammer: on Windows, the `O_EXCL` create of the game-lock or solo
   file raised `PermissionError` while another process's delete of that file was still pending.
   The baseline crashed with it (`hammer_baseline_permissionerror.txt`). It is now retried like a
   held lock, still bounded by `wait`.

`WORKER_RULES.md` line 42 now names the 1 h default and the priority wait.

## Evidence (all in this folder)

| File | What |
|---|---|
| `concurrency_test_new_x3.txt` | `mashed_lock_concurrency_test.py`, 3 full runs on the new impl: scenarios 9/9 PASS x3, hammer PASS x3 (0 overlaps, 0 errors, 0 hung workers) |
| `scenarios_baseline_orch_integration.txt` | the same scenarios on `orch/integration`'s `mashed_lock.py` (control): 3/9, FAIL on cases 2, 3, 4, 5, 8, 9 |
| `selftest_new.txt` | `mashed_lock_selftest.py` 14/14 PASS on the new impl |
| `ps_smoke.ps1`, `ps_smoke_new.txt` | `MachineLock.ps1` smoke test, 5/5 PASS (priority hold then linger, solo waits through a linger, default TTL 3600, dead-PID hold is stale, priority solo ignores a marker) |
| `hammer_baseline_permissionerror.txt` | baseline hammer with the new `prio` op: PermissionError crash (item 7) |

New scenarios in `mashed_lock_concurrency_test.py` (`--scenarios-only`, `--skip-scenarios`):

1. `open-window-survives-pending-priority`
2. `new-solo-waits-while-priority-pending`
3. `ordinary-acquire-ignores-linger`
4. `new-solo-not-opened-during-priority-hold`
5. `new-solo-waits-through-linger`
6. `priority-solo-ignores-other-marker`
7. `solo-open-unblocked-without-claims`
8. `solo-begin-default-ttl-le-1h`
9. `run-solo-default-ttl-le-1h`

The hammer adds a `prio` op (priority acquires mixed into the load) with the linger set to 0 for
the hammer only.

`mashed_lock_selftest.py` case 10 (solo drain) now drops the lingers left by cases 2-5 before it
runs. Those lingers hold new solo opens back by design, and that case tests the drain.

Test-slot note: all 6 machine test slots were held by long game runs (`test-slot --status`). These
file-lock tests take about 30 s and boot no game, so they ran with `PROYECTOS_TEST_SLOT=off`.

## Rollout notes

- The live lock dir keeps working with mixed versions: old callers ignore the `_priolinger_` file,
  and its name does not match the `_prio_*` marker glob.
- Until a merge-train process runs the new `mashed_lock.py` (the train runs it from the
  orch-integration worktree), no hold/linger files are written. A new solo open by a new-version
  caller still waits on the train's existing priority markers.
- Gap that remains: the train's build/lint phases between gate calls are longer than 120 s, so a
  solo window can still open there. The train then waits for it, bounded by the new 1 h default.
