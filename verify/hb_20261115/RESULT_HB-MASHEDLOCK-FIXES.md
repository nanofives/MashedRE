# RESULT HB-MASHEDLOCK-FIXES (rows HB-MASHEDLOCK-TEMPGLOB, HB-MASHEDLOCK-STATUS-PIDREUSE)

Branch orch/HB-MASHEDLOCK-FIXES. Code commit d23559f28 (`re/orchestrator/mashed_lock.py` only, Python). This RESULT is committed on top of it; the final sha is in the REPORT.
Machine: LIVING-PC, %TEMP% holds 645,127 entries. Row kind: harness, no game launched, no build.

## What the code does (d23559f28)
- `_scan_locks` uses `os.scandir` with a prefix/suffix filter instead of `glob.glob`.
- `_marker_dir_cfg()` is resolved per call: `MASHED_LOCK_MARKER_DIR` if set, else `LOCK_DIR`. Legacy `%TEMP%` markers are read only with `MASHED_LOCK_LEGACY_SCAN=1`.
- `status` applies the owner_start check through `_status_alive`, as acquire does (STATUS-PIDREUSE).

## Commands and outputs
1. `py.exe -3.12 re/orchestrator/mashed_lock_selftest.py`: 14/14 PASS, exit 0 (`selftest.log`). Case 9 `pid-reuse-start-mismatch-is-broken` PASS.
2. `py.exe -3.12 re/orchestrator/mashed_lock_concurrency_test.py`: scenarios 16/16 PASS. Hammer: procs=8 secs=12.0 acquires=44 solo_windows=15 status_reads=35 timeouts=0 overlaps=0 errors=0 hung_workers=0, PASS, exit 0 (`concurrency.log`).
3. `py.exe -3.12 re/orchestrator/mashed_lock.py status` with default markers (%TEMP%): wall 42.75 s (`status.log`). cProfile (separate run, 28.1 s under profile): 2 `_scan_locks` calls, 27.6 s tottime inside them (`prio_pending` 24.5 s, `prio_lingering` 3.6 s), 1.29M entries visited. The cost is the scandir walk of the 645k-entry directory itself. Timing varies between runs (a .NET enumeration of the same directory took 2.4 s once warm).
4. Same `status` with `MASHED_LOCK_MARKER_DIR=<empty dir>`: wall 0.84 s (`status_markerdir.log`).

## Verdict on each row
- STATUS-PIDREUSE: selftest case 9 PASS, status uses `_status_alive`. Covered by the selftest only. I did not stage a live reused-PID lock file and run `status` against it.
- TEMPGLOB: the scandir change alone does NOT cure the 38 s. It removes glob's overhead but the default path still walks all of %TEMP% (42.75 s here, no improvement). The speedup exists only with the opt-in `MASHED_LOCK_MARKER_DIR` (0.84 s).

## Open
default marker move stays an open row (needs claude3, MachineLock.ps1 + merge_train.ps1). Until it lands, the 38 s status stays on the default path. No .ps1/.sh edited here.

Raw logs (selftest.log, concurrency.log, status.log, status_markerdir.log) sit beside this file in the worktree but are gitignored, so they are not committed. The outputs quoted above are the record.
