# RESULT HB-MASHEDLOCK-EARLY-PRIORITY

Verdict: **GREEN**, text-only tooling, no game boots, no exe build.
Branch `orch/HB-MASHEDLOCK-EARLY-PRIORITY`, built on `orch/HB-MASHEDLOCK-SOLO-PRIORITY` @ `b52d95787` (that
branch is unchanged). The `.ps1` edit is applied, so no diff is needed here.

## Problem

Every 2026-11-01 starvation had one shape. The train merged and built for 4-8 min, and only its first
`mashed_lock.py run --priority` (inside `Gate-Default`) wrote a priority marker. A worker opened a solo window in
that gap: EAX at 20:49 before the 20:57 claim, PATHPROG-REMERGE at 21:29 before the 21:33 claim. b52d95787 makes new
solo opens yield to a *pending or held* priority claim, but the train had no claim yet during merge+build.

## Change

1. `re/orchestrator/mashed_lock.py`: two CLI commands for a long-lived owner process that is not Python.
   - `prio-hold --owner-pid <pid> --label X [--ttl S]` writes the b52d95787 held record
     `<name>_priolinger_<pid>.lock` (`held=true`, `pid` and `owner_start` of the owner) for the given PID.
     Refuses a dead PID.
   - `prio-release --owner-pid <pid> [--linger S]` turns it into a `held=false` record with a
     `PRIO_SOLO_LINGER_SEC` (120 s) expiry. A back-to-back pass re-registers inside that window. `--linger 0`
     removes it. Only that owner's file is touched.
   - `prio_linger_mark` takes an optional `pid`, and the new functions are `prio_hold` / `prio_release`.
     Existing call sites are unchanged.
2. `re/orchestrator/merge_train.ps1`:
   - `Enter-PassPriority` (`prio-hold --owner-pid $PID --label merge-train:$TS`) is the **first statement of the
     script-scope run-body try**, before `Try-Batch` merges or builds anything.
   - The finally is now `try { Clear-Temps } finally { Exit-PassPriority }`, so the hold ends on GREEN, RED,
     conflict, bounce, foreign-tip skip and throw, even if `Clear-Temps` throws.
   - "Queue empty" and `-Plan` exit before the try, so they never hold anything.
   - Both helpers are best effort (try/catch + `Say`). A failed write only reopens the old gap, so it never aborts
     a pass.
   - If the train pwsh is killed, the record goes stale as soon as that PID is gone (owner_start guards reuse).
3. Tests: 7 new scenarios in `mashed_lock_concurrency_test.py` (cases 10-16) and 4 new checks in
   `merge_train.ps1 -SelfTest`.

### Why the held record and not a pass-wide PRIORITY-PENDING marker

The starvation was solo windows only. The held record blocks exactly that: new non-priority solo opens.
- Ordinary game-lock acquires ignore it, so workers' single boots still run during the 4-8 min build.
- At gate time the existing per-call `run --priority` marker still queues the train ahead of a worker's next boot
  (HB-LOCKPRIO, unchanged).
- A pass-wide `_prio_` marker would also stall every ordinary acquire for the whole merge+build, including
  text-only passes that never boot the game.
- Status shows the hold as `PRIORITY-HOLD 'mashed_machine' pid=<train pid> label=merge-train:<TS>`.

If the lead wants the stronger form anyway, add a `--pending` flag to `prio-hold` that also writes
`_prio_<owner>.lock` with the owner's PID (about 3 lines). It is not in this change.

### Windows other sessions hold

- `prio-hold` never reads or writes the solo file or the game lock.
- A window already open stays valid until `solo-end` or its expiry. Its token children still get the game lock
  (case 14: window record byte-identical before and after).
- The train's own priority solo opens (D2 `run --priority --solo`, the 13-track `solo-begin --priority`) do not
  wait on any hold (case 13).

## Evidence (all files in `verify/harness_20261102/`)

| Test | Result | File |
|---|---|---|
| `mashed_lock_concurrency_test.py` (16 scenarios + 8-proc hammer), new impl, x3 | 3/3 PASS: 16/16 each, overlaps 0, errors 0, hung 0 | `concurrency_test_x3_and_base.txt` |
| same scenarios against base `b52d95787` mashed_lock.py (control) | 10/16. Case 10 PASS: the no-hold control **reproduces the starvation** (solo open +0.02 s, gate got +2.56 s, after the window closed). Case 11 FAIL: the solo opens during the build (+0.01 s). 12-16 abort: no `prio_hold` | same file |
| `mashed_lock_selftest.py` | 14/14 PASS | `selftests_final.txt` |
| PowerShell reader: `MachineLock.ps1` `Get-PrioLinger` sees the train-written hold, empty after release | 2/2 PASS | `selftests_final.txt`, script `ps_reader_check.ps1` |
| `merge_train.ps1 -SelfTest` | 20 PASS / 3 FAIL. The 4 new checks PASS: AST order, release after a throw, status line, live solo-blocked / ordinary-not-blocked / opens-after-release | `selftests_final.txt` |
| `merge_train.ps1 -SelfTest` on base `b52d95787` (control) | 16 PASS / the same 3 FAIL | `merge_train_selftest_base_b52d95787.txt` |

New scenarios, with timings from run 1:
- 10 `control-no-hold-reproduces-starvation`: a fake train merges and builds without the lock, then takes it with
  priority. A worker opens a solo during the build, and the gate waits for that window.
- 11 `pass-hold-keeps-new-solo-out-of-merge-build-and-gate`: same timeline, but `prio-hold` runs first. Gate got
  +1.09 s, gate done +1.40 s, solo opened +1.42 s (only after release).
- 12 `pass-hold-ordinary-acquire-not-blocked`: 0.0 s.
- 13 `pass-hold-priority-solo-not-blocked`: CLI `solo-begin --priority`, 0.15 s.
- 14 `pass-hold-leaves-open-window`.
- 15 `pass-hold-release-own-only-and-lingers`: release by another PID keeps mine, default release lingers 120 s,
  a dead PID is refused.
- 16 `pass-hold-stale-when-owner-dies`: the owner exits without releasing, and a new solo opens 0.01 s later.

### Already failing, not caused by this change

- **merge_train selftest HB-D2RERUN, 3 FAIL on base too.** The fake runner hardcodes `D2_DEF_2`.
  `merge_train.json` `full.d2_arms` has been `D2_DEFH_1..3` since the SUBSTATE flip (run 23), so `runs['D2_DEF_2']`
  is never set. The real train is unaffected. The fix is to read the arm name from `$f.d2_arms[1]`. That is a
  separate one-line row, left out of scope here.
- **Concurrency scenarios are load-sensitive.** On LIVING-PC at 90% CPU (6 test slots busy) the b52d95787 cases
  1-6 and an earlier version of cases 10-11 timed out on child-process startup: one run 11/16 and one 0/16, both
  aborting at the existing case 1 `t1.wait(10)`.
  - Cases 10-11 were then made event-driven: the train waits for the worker's `TRY` event, so the outcome does not
    depend on startup latency.
  - The bounds in cases 12-16 were widened, and each still separates "blocked" (a forever wait or a TimeoutError)
    from "slow".
  - The three committed runs are 16/16.

## Not done

- No game boots and no exe build (text-only).
- The live train, the orch-integration worktree and the live lock dir were not touched. Every test uses a private
  `MASHED_LOCK_DIR` (and `TEMP` for the PS reader).
- No push, merge or enqueue.
