# RESULT HB-MASHEDLOCK-FIXES (HB-MASHEDLOCK-TEMPGLOB + HB-MASHEDLOCK-STATUS-PIDREUSE)

Branch orch/HB-MASHEDLOCK-FIXES, file `re/orchestrator/mashed_lock.py`, new test `re/orchestrator/mashed_lock_tempglob_test.py`. One commit covers both rows (same file).

## Changes
- STATUS-PIDREUSE: `status` now calls `_status_alive(o)` = pid alive AND not `_owner_dead(o)` (same owner_start check acquire uses).
- TEMPGLOB: `prio_pending` / `prio_lingering` use `_scan_locks` (os.scandir + startswith/endswith) instead of `glob.glob`. Marker files stay in LOCK_DIR (%TEMP%) by default, because MachineLock.ps1 and merge_train.ps1 read and write them there and .ps1 edits were out of scope. Opt-in `MASHED_LOCK_MARKER_DIR` moves Python markers to a small dir (legacy %TEMP% markers read only with `MASHED_LOCK_LEGACY_SCAN=1`).

## Passed (run, final code)
`mashed_lock_tempglob_test.py` 4/4 PASS: reused pid with mismatched owner_start -> alive=False; matching owner_start -> alive=True; dead pid -> not alive; 20000-entry fake temp with MASHED_LOCK_MARKER_DIR, prio_pending+prio_lingering 0.143 s.

## Measured, not fixed by default
On the real %TEMP% (634,296 entries) a plain scandir prefix listing took 4.8-5.6 s vs glob 6.5-7.2 s (about 25% faster only), and prio_pending 4.4-5.7 s. The remaining cost is listing the 600k-entry directory itself. The 38 s status was mostly the first `_pid_alive` call (31.7 s once, psutil path, run before the scan) and cold directory cache [UNCERTAIN: cause of the 31.7 s not isolated]. So TEMPGLOB is only fully fixed with MASHED_LOCK_MARKER_DIR set (and a matching dir in the .ps1 files). Not set as default: needs .ps1 edits (claude2 cannot).

## FAILED
`mashed_lock_selftest.py` 2/14 `priority-marker-visible-while-waiting` FAIL (run on an earlier revision, where the marker dir was computed at import, so the self-test's `ml.LOCK_DIR = FAKE_DIR` reassignment did not move markers). Since changed: marker dir resolved per call (`_marker_dir_cfg()`), default = LOCK_DIR.

## Did not finish
Re-run of selftest + concurrency test + tempglob test on the fixed code (log /tmp/self3.log, queued behind test-slot memory gate ~10 min). Whether the 14 selftest checks and the concurrency test pass now: [UNCERTAIN], unverified.
