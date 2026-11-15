# RESULT HB-MASHEDLOCK-DEFAULT-MARKERDIR + HB-MASHEDLOCK-STATUS-PIDREUSE-RUN

Branch orch/HB-MASHEDLOCK-DEFAULT-MARKERDIR (base orch/integration 38a6f3ae5). Harness rows, no game builds, no G-CTRL/B.
Evidence of the problem: verify/mashedlock_20261114/RESULT_HB-MASHEDLOCK-FIXES.md (status 42.75 s with 645k TEMP entries, 0.84 s with a private marker dir).

## Row 1: HB-MASHEDLOCK-DEFAULT-MARKERDIR (GREEN)

Default marker dir for `<name>_prio_<pid>.lock` and `<name>_priolinger_<pid>.lock` is now `%LOCALAPPDATA%\mashed_lock\markers` on all three tools. The lock file and the solo file stay at their fixed %TEMP% paths (direct opens, never a directory listing).

- `re/orchestrator/mashed_lock.py`: `_default_marker_dir()`, `_marker_dir_is_default()`, `_marker_dir_cfg()`, `_legacy_scan_wanted()`. Order: `MASHED_LOCK_MARKER_DIR`, then an isolated lock dir (MASHED_LOCK_DIR set, or `ml.LOCK_DIR` reassigned as selftest does) keeps markers beside its locks, else the default dir.
- `re/orchestrator/MachineLock.ps1`: `Get-MarkerDir` and `Get-MarkerScanDirs` with the same order and the same stamp file; `Get-PrioPending`, `Get-PrioLinger`, `Acquire-Lock` and `Write-PrioLinger` use them.
- `re/orchestrator/merge_train.ps1`: it writes markers only through mashed_lock.py (prio-hold, run --priority, solo-begin), so it follows the python default. Added a selftest check `marker dir: default priority/linger marker dir is outside %TEMP%`, which resolved to `C:\Users\maria\AppData\Local\mashed_lock\markers`.
- New test `re/orchestrator/mashed_lock_markerdir_test.py` 5/5 (default dir outside TEMP, legacy marker visible in window, dropped after window, forced with MASHED_LOCK_LEGACY_SCAN=1, isolated lock dir keeps markers).

### Changeover (no mixed state)
A tool older than this change writes markers into %TEMP%. For 3600 s after the stamp file `markers\changeover.epoch` is first written (by either new tool), the new tools scan the old %TEMP% location as well as the new dir, so a priority or linger marker held by an older tool stays visible. After the window the legacy scan stops, which is what removes the slow listing. `MASHED_LOCK_LEGACY_SCAN=1` forces it, `=0` skips it. Not covered: an older tool does not see a marker a new tool writes into the new dir during that hour. Both tools are short-lived (priority marker expires wait+300 s, linger 120 s), and the train and the lock holders all run the merged copy once the branch lands. The lock file itself (`mashed_machine.lock`) and the solo file are unchanged, so a lock held by an older tool is seen by new tools at all times.
Test-hygiene note: any new-code run on the default path creates the stamp and starts the hour. My runs created `%LOCALAPPDATA%\mashed_lock\markers\changeover.epoch` at 16:01 (the merge_train SelfTest). I deleted that file and the empty dirs so the window starts at the first real use after the merge.

### Checks (all under test-slot, one slot for the four suites, full log kept local (gitignored) at /tmp/hb_markerdir_suites.log)
- `mashed_lock_selftest.py` 14/14 PASS, rc=0
- `mashed_lock_concurrency_test.py` scenarios 16/16 PASS, then procs=8 secs=12.0: acquires=46 solo_windows=22 status_reads=44 timeouts=0 overlaps=0 errors=0 hung_workers=0, PASS, rc=0
- `mashed_lock_tempglob_test.py` 4/4 PASS, rc=0 (20000-entry fake temp, prio_pending+prio_lingering 0.016 s)
- `merge_train.ps1 -SelfTest`: `SELFTEST done: 0 failure(s)`, 138 PASS lines, rc=0
- `mashed_lock_markerdir_test.py` 5/5 PASS (first slot, 600 s memory-gate wait)
- Both .ps1 files parse with 0 errors (System.Management.Automation.Language.Parser).
- test-slot waited ~600 s for the memory gate twice (25-26 GB committed of 29.5 GB), then started under the cap. No hub dispatch involved.

### `mashed_lock.py status` wall time on the DEFAULT path (no MASHED_LOCK_DIR, no MASHED_LOCK_MARKER_DIR), real %TEMP% with 668,372 entries
- Window active (legacy %TEMP% scan on, private LOCALAPPDATA so the live stamp is untouched): 8.631 s real
- Window over (`MASHED_LOCK_LEGACY_SCAN=0`): 0.721 s real
Compare 42.75 s before (RESULT_HB-MASHEDLOCK-FIXES). The 8.6 s in the window is the one-hour cost of the legacy scan, 0.72 s afterwards. The earlier 42.75 s figure is not reproduced here: this run's `_pid_alive` path was not the cost, the %TEMP% listing was [UNCERTAIN: what produced the extra seconds in the earlier measurement].

## Row 2: HB-MASHEDLOCK-STATUS-PIDREUSE-RUN (GREEN)
Script `verify/hb_20261115/pidreuse_run.py`. It spawns a sleeping child, stages a lock whose pid is that live child with `owner_start` = real start minus 5000, in a private MASHED_LOCK_DIR and MASHED_LOCK_MARKER_DIR (temp dirs, removed at the end), and runs `py -3.12 re/orchestrator/mashed_lock.py status --name pidreuse_run` as a fresh process. Literal output:

```
staged pid=20232 (live=True) real_start=1794768707.2 owner_start=1794763707.2
status stdout: HELD 'pidreuse_run' pid=20232 alive=False label=reused-pid-stage since=2026-11-15T00:00:00  (C:\Users\maria\AppData\Local\Temp\mlk_pidreuse_3qqyp4kx\locks\pidreuse_run.lock)
status rc: 0
control (matching owner_start) stdout: HELD 'pidreuse_run' pid=20232 alive=True label=reused-pid-stage since=2026-11-15T00:00:00  (C:\Users\maria\AppData\Local\Temp\mlk_pidreuse_3qqyp4kx\locks\pidreuse_run.lock)
RESULT PASS
```
Reused PID with mismatching owner_start reads alive=False, the matching control reads alive=True. The claim no longer rests on selftest case 9 alone.
