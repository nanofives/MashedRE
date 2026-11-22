# RESULT HB-LOCK-ENFORCE (run 55, meta/tooling row)

## Why arm ws_00412190 started under the hold
`re/frida/stalker_write_surface.py:168` takes `MashedLock(f'stalker:{rva:#010x}')`, a plain (non-priority) lock.
`MashedLock._acquire` (old `mashed_lock.py`) waited only on the open solo window and on pending priority
markers. A PRIORITY-HOLD is a `held=true` linger record (`prio_hold`, `_priolinger_<pid>.lock`), and only NEW SOLO OPENS
looked at it (`solo_blockers`). With the game lock free, a plain acquirer under a hold took the lock at once.
Control (old code from orch/integration, hold written, free game lock): arm printed LAUNCHED after 0.2 s.
The build entry point, `ddkobj.py` and the binder selftest had no mashed_lock call at all.

## Change
- `re/orchestrator/mashed_lock.py`: `hold_blockers()` (held PRIORITY-HOLD records plus an open SOLO window, as `status` shows them),
  `wait_clear()` (prints `WAITING (<label>) - <blocker>` once, `clear after Ns` when done), CLI `wait-clear [--label L] [--wait N] [--poll S]`
  (exit 3 on the bounded timeout), and non-priority `MashedLock._acquire` now waits on a held PRIORITY-HOLD.
  Exemptions: priority acquirers, a hold whose owner is an ancestor of the process, a solo token the process carries,
  and `MASHED_LOCK_SKIP_WAIT=1` (for the hold owner's own children).
- `mashedmod/build.bat`: `mashed_lock.py wait-clear --label build.bat` before vcvars.
- `re/tools/ddkobj/ddkobj.py`: `main()` calls `wait_clear` for build, check and selftest.
- `re/tools/bind/porttop_bind.py`: `cmd_selftest` calls `wait_clear` first (wait line only, checks unchanged; 4 lines).
- Test: `re/orchestrator/mashed_lock_hold_enforce_test.py` (private MASHED_LOCK_DIR, real subprocesses).

## Checks run
1. `mashed_lock_hold_enforce_test.py`: 9/9 PASS (hold_enforce_test.log): clear returns at once, status shows PRIORITY-HOLD,
   plain-lock arm under a hold does NOT launch (still waiting after 2.5 s), wait-clear waits, `ddkobj.py check` waits, SKIP env bypasses,
   `--wait 1` exits 3 with a line, arm launches after release, waiters print WAITING and `clear after`.
2. Control on old mashed_lock.py: arm LAUNCHED after 0.2 s under a hold (the failure reproduced).
3. Existing: `mashed_lock_selftest.py` 14/14 PASS; `mashed_lock_concurrency_test.py` PASS (overlaps=0 errors=0);
   `mashed_lock_markerdir_test.py` 5/5 PASS; `mashed_lock_tempglob_test.py` 4/4 PASS.
4. `porttop_bind.py selftest` (after status free): FAIL 14, the head's own count (all audio, same as the stop); 0 new (binder_selftest.log).
5. VS2022 `mashedmod\build.bat` with a live SOLO window open (FIX-00420420-CRASH): printed `WAITING (build.bat) - SOLO ...`, `clear after 51s`, then `Build OK`, exit 0 (build_vs2022.log).
6. Object build `MASHED_DDKOBJ=1 mashedmod\build.bat` (status free): Build OK, exit 0.
7. `ddkobj.py check`: IDENTICAL 2336/2435, DIFFERENT 72, not-in-map 27. BYTECHECK.tsv vs `log\orch\run55_headcheck\BYTECHECK.tsv`: 0 of 2436 rows differ,
   so no newly DIFFERENT unit and no unit -> 100% neighbour fallback to classify.
8. `ddkobj.py selftest`: OK, 0 failures (ddkobj_selftest.log).

## Unfinished scope / caveats
- Callers outside the four wrappers that launch a game WITHOUT MashedLock (a bare `Popen` of MASHED.exe) are not covered; I did not survey them. [UNCERTAIN] test: grep arms for MASHED.exe launches with no `mashed_lock`.
- The train's own build and arms must run as a descendant of the hold owner, or set `MASHED_LOCK_SKIP_WAIT=1`, or use `mashed_lock --priority`, else they now wait on their own hold. The ancestor rule covers children of the pwsh that wrote the hold; a hold written with `prio-hold --owner-pid` for a pid that is not an ancestor of a worker's builds is the intended blocker. L0 should check the first train pass after merge.
- `build.bat` waits without a time limit (hold ends by release, owner death or the 24 h record ttl; solo by its 1 h expiry).
