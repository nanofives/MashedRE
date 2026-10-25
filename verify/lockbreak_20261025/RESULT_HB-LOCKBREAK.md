# RESULT HB-LOCKBREAK: the max-age rule broke a live lock holder

Verdict: **GREEN.** The break came from the hard max-age rule in `MashedLock._acquire`, not from the
dead-PID path. Fixed in both lock implementations: a live holder is never broken for age, and PID reuse
is now caught by the holder's recorded process start time. One K357 arm (`H_U7PX`, batch 3) overlapped the
D4 gate run for about 30 s at its tail. No K357 prediction arm ran inside the window. No re-run is needed
for the RESULT.

No game was launched for this step, and the live `mashed_machine` lock was never read or written. All
lock tests ran in a private `tempfile.mkdtemp()` dir.

## 1. Which path broke the lock

Incident record, `verify/d4dc82a0_20261025/RESULT_D4-DC-82a0.md:107-110` (orch/integration `70fac88db`):
`breaking STALE ... (pid=18904 alive=True age=1801s)`.

Pre-fix code (orch/integration `70fac88db`):

- `re/orchestrator/mashed_lock.py:76` `MAX_AGE_SEC = 30 * 60` ("a lock older than this whose PID looks
  alive is still broken").
- `re/orchestrator/mashed_lock.py:364` `stale = (not _pid_alive(opid)) or (age > MAX_AGE_SEC)`. With
  `alive=True`, only the second clause can be true: 1801 > 1800.
- `re/orchestrator/mashed_lock.py:366` is the only line that prints `breaking STALE ... alive=... age=...`.
  `MachineLock.ps1:165` prints `(pid=...)` with no `alive`/`age`, and the solo drain
  (`mashed_lock.py:274`) prints nothing. So the breaker was a Python `MashedLock._acquire` (the gate job's
  `mashed_lock.py run`), via the **max-age clause**, not the stale (dead-PID) clause.

The same max-age rule existed in three other places, all fixed: the solo drain `mashed_lock.py:274`, and
`MachineLock.ps1:64` (solo drain) and `:163` (`Acquire-LockBody`), both via `$script:MaxAgeSec` (`:21`).

Why 30 min is wrong: K357 holds the lock once for a whole batch (`run_arms.py`, one `mashed_lock.py run`
around 11 arms). Batch 3's arms sum to 1832 s (`secs` in `arms2/RUNLOG.txt`), 02:24:57 to 02:55:28, so a
batch with both H arms crosses 1800 s while its holder is alive.

## 2. Fix (orch/HB-LOCKBREAK)

`re/orchestrator/mashed_lock.py`:

- `MAX_AGE_SEC` removed. New `_holder_stale(path, o)` (`:156`): stale only when the record has no PID, the
  PID is dead, or the PID now belongs to a process whose start time differs from the recorded
  `owner_start` by more than 2.0 s (the existing `_owner_dead`, HB-SOLOSTALE). Age is not an input.
- The game-lock record now carries `owner_start` = `_pid_start(os.getpid())` (`:371`).
- Both readers use it: `MashedLock._acquire` (`:381`) and the solo drain (`:290`).
- Records written before this fix have no `owner_start`. A live PID keeps them held (no age fallback).
  `_pid_alive` already returns True when liveness cannot be read, so an unknown state waits, never steals.
- Second defect found by the control run: an empty or unparsable lock file (a writer between its
  `O_EXCL` create and its JSON write) read as `pid=None` = dead and was broken at once. It is now kept for
  `EMPTY_GRACE_SEC = 10` (`:78`) by file mtime, then broken.

`re/orchestrator/MachineLock.ps1`: same rule. `$script:MaxAgeSec` removed, `Get-PidStart` (`:105`) and
`Test-HolderStale` (`:113`) added, used by the solo drain (`:66`) and `Acquire-LockBody` (`:189`).
`Acquire-LockBody` writes `owner_start` (`:181`). `-Force` keeps breaking on request.

The `break` CLI is unchanged: a manual break of a wedged live holder is still possible.

## 3. Selftest (fake dir only)

`re/orchestrator/mashed_lock_selftest.py` now runs entirely in `tempfile.mkdtemp()` (`ml.LOCK_DIR` in the
parent and every child, `TEMP`/`TMP` for pwsh), and gains 8 cases (7-14). Holders are real sleeper
subprocesses, so liveness is checked against a live foreign PID.

| # | case | fixed | pre-fix control |
|---|---|---|---|
| 7 | live holder, `owner_start` set, age 1801 s: acquire times out, record kept | PASS | **FAIL** (`ACQUIRED`) |
| 8 | live holder, legacy record (no `owner_start`), age 7 days | PASS | **FAIL** |
| 9 | live PID, `owner_start` 1000 s off (PID reuse): broken | PASS | FAIL (old code had no reuse check) |
| 10 | solo drain waits on a live 1801 s holder, proceeds once it is killed | PASS | **FAIL** (`still=False`) |
| 11 | dead holder, fresh record: broken | PASS | PASS |
| 12 | empty lock file: kept inside 10 s, broken after | PASS | **FAIL** (broken at once) |
| 13 | `MachineLock.ps1` `Acquire-Lock`, live holder age 1801 s: times out | PASS | **FAIL** |
| 14 | pwsh holder's `owner_start` read from Python as the same process: not broken | PASS | FAIL (`owner_start=None`) |

Cases 1-6 (HB-LOCKPRIO) pass on both. Logs: `selftest_fixed.txt` (14/14, exit 0) and
`selftest_control_prefix.txt` (the same selftest against the orch/integration `mashed_lock.py` and
`MachineLock.ps1` copied into a scratch dir: 6/14, exit 1). Case 7 is the incident reproduction.

## 4. Which K357 arms ran inside the broken window

Sources: `verify/k357_20261025/data/arms2/RUNLOG.txt` and `data/run_arms3.txt` on orch/D3-CF-K357
(`a30d080b5`), and `verify/d4dc82a0_20261025/gates_RUNLOG.txt`. RUNLOG times are arm end times (local,
UTC-3). Start = end - `secs`.

- Holder pid 18904 = K357 batch 3 (`run_arms3.txt`: `HELD 'mashed_machine' pid=18904 (D3-CF-K357)`), the
  amended batch whose numbers the RESULT reports (exe `3182ad61`).
- Its first arm `B_U7PXR3_K0` ended 02:26:11 after 74 s, so the hold began at or before 02:24:57.
  02:24:57 + 1801 s = 02:54:58.
- The D4 gate job's first arm `H_DEF` ended 03:04:25 after 567 s, so it started at 02:54:58. The break
  happened at or just before that second. Both computations agree.
- **Window: 02:54:58 to 02:55:28** (the end of K357's last arm).

| K357 batch-3 arm | ran | in window |
|---|---|---|
| `B_U7PXR3_K0`, `B_U7P_K0`, `B_X_K0`, `B_DEF_K0/K1/K26`, `B_U7PX_K0/K1/K26` | 02:24:57 to 02:36:05 | no |
| `H_DEF` | 02:36:05 to 02:46:19 | no |
| `H_U7PX` | 02:46:19 to 02:55:28 (549 s) | **last ~30 s** overlapped D4 `H_DEF` |

Batches 1 and 2 (`arms1/`, 01:08-01:56) were taken under the merge-train waits, ended well before the
hold in question, and are outside the window.

Effect on K357's RESULT:

- Every prediction arm (P0, P0b, P0c, P1, P2 in `RESULT_D3-CF-K357.md` section 3) ran before 02:46:19, with
  no other game holder. Their hashes and the P2 `reck.py` result stand.
- `H_U7PX` is a **report** row, not a prediction. It exited by itself (`exit=0 self_exited=True`) with
  the same line counts as the batch-1 run under a clean hold (step 14659, gates 56873, ptrace 28436), the
  same ptrace hash `09551e6a`, and the same `R6 ROUND END winner=car3 scores=[4,5,7,8]` log line. Its
  step/gates hashes differ from batch 1 because the exe differs (amendment 1, `9085dbdd` vs `3182ad61`),
  not because of the overlap.
- The other side of the same overlap, D4's `H_DEF`, reproduced the default G-CTRL trio
  `bacfc747/8dd1c004/6f9932b6` exactly. That is one witness that a deterministic `H` arm
  (`MASHED_DETERMINISTIC=1`, fixed `DET_FRAMES`) keeps its hashes under this overlap.
  `[UNCERTAIN]` bit-identity of K357's `H_U7PX` step/gates (`5943c5f1/b493262b`) itself: proving it needs
  a re-run of that one arm, which this step was told not to do.

Recommendation: no K357 re-run for the RESULT or for L-10 (which keeps `MASHED_R7_EXIT` OFF). If the lead
wants the report row confirmed, re-run `H_U7PX` alone (one ~9 min arm) when K357 is rebased onto the
U9779 merge, where its gates get re-measured anyway.

## 5. Gates

Not run: this step changes only lock scripts (no source, no exe). The hard constraint for this step
forbade game runs while a D3 solo verification is in progress.
