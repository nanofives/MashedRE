# RESULT HB-MASHEDLOCK-RACE (2026-11-01)

Verdict: BLOCKED. The fix and the self-test are written and compile, but the multi-process
self-test has not been run. The auto-mode classifier denied both attempts ("Interfere With
Workloads"), the second one wrapped in `test-slot`. Nothing below claims a pass.

## Traceback (D:/MashedWT/D4-WSB-PORT-BROAD-2_data/pass3.log, lines 30-49)
`mashed_lock.py run --solo` (`_cli` 473) -> `solo_begin` 251 -> `_solo_begin` 277 `solo_owner(name)`
-> `solo_owner` line 232 `os.remove(path)` ->
`PermissionError: [WinError 32] ... 'mashed_machine_solo.lock'` (another process had the file open).
Only `FileNotFoundError` was caught. The driver skipped arm 0055b080 without launching it.

## Change (re/orchestrator/mashed_lock.py)
- `_remove(path)`: retries up to 8 times on any OSError, treats FileNotFoundError as gone, never raises.
- `_remove_if_same(path, o)`: re-reads the record and removes only if it is still the one judged stale.
- `solo_owner`: an empty/unparsable/unreadable solo file younger than EMPTY_GRACE_SEC (10 s) is a
  busy window, not stale. A removal that fails is also busy (retry), not None.
- `_solo_admits_me` no longer matches when both the env token and the owner token are None.
- `prio_pending`: a young empty marker is skipped, not removed. Stale breaks use `_remove_if_same`.
- `_acquire` and `_solo_begin` stale breaks: remove only if same, sleep on failure instead of spinning.
- `release` retries a transient unreadable owner read so the lock is not leaked.
- `solo_end`, `prio_clear`, CLI `break` go through `_remove`.
- `MASHED_LOCK_DIR` env var redirects every lock file (default unchanged: user temp dir).
- Residual: a remove still has a small window between the same-record check and the unlink.

## Self-test (re/orchestrator/mashed_lock_selftest.py), NOT RUN
`py -3.12 re/orchestrator/mashed_lock_selftest.py [--procs 8] [--secs 12] [--impl <file>]`.
Private temp lock dir via MASHED_LOCK_DIR. Workers mix acquire, status reads, solo windows with
1-30 s ttl, a solo file with a dead owner PID, and a half-written solo file. Fails on any worker
exception, any game-lock overlap, or a hung worker. `--impl` runs the same hammer on another copy,
for example `git show orch/integration:re/orchestrator/mashed_lock.py`, as a baseline.
Checks done: `py_compile` of both files only.

## Correction (lead check, same day)
The self-test section above named the wrong file. `re/orchestrator/mashed_lock_selftest.py` is the
pre-existing 14-scenario test and is unchanged (it passes on old and new code, so it shows no
regression but does not discriminate the race). The multi-process hammer is the new file
`re/orchestrator/mashed_lock_concurrency_test.py`:
`py -3.12 re/orchestrator/mashed_lock_concurrency_test.py [--procs 8] [--secs 12] [--impl <mashed_lock.py>]`.
Baseline: `git show orch/integration:re/orchestrator/mashed_lock.py > old.py`, then `--impl old.py`
(the hammer redirects TEMP too, so old code also stays in the private dir). Status: written and
`py_compile`d, not run by this worker. The lead runs it 5 times plus the old-code baseline.
Verdict: GREEN-pending-run.
