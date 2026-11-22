# RESULT HB-LOCK-PUBLISH-FIX (meta / tooling row, no G-CTRL, no B, no captures)

Change (commit on orch/HB-LOCK-PUBLISH-FIX, base orch/integration a2843847a):
- mashedmod/build.bat: the HB-LOCK-ENFORCE wait is skipped only when MASHED_LOCK_WAIT=off, with one printed line
  (`[mashed_lock] wait skipped: MASHED_LOCK_WAIT=off (archive build)`). Without the variable, a missing
  re/orchestrator/mashed_lock.py is an error naming the path, exit 1. No fail-open.
- scripts/lan_release/publish.sh: `export MASHED_LOCK_WAIT=off` next to `export MASHED_NO_DEPLOY=1`.
  publish_mashed.bat only calls publish.sh, so it has no build call of its own.
- ddkobj.py / porttop_bind.py: ddkobj.py is called only under MASHED_DDKOBJ=1 (build.bat:322-324), and
  re/tools is not in archive_paths.txt, so an archive object build would fail there too (not a regression:
  publish.sh never sets MASHED_DDKOBJ). gen_portstate_seed.py and extract_image.py already carry `if exist` guards.
  porttop_bind.py is not called by build.bat. Untouched, nothing to report to BIND-LIB-ROUTE.

Checks:
1. PASS. Clean `git archive HEAD` of archive_paths.txt (mashedmod, scripts, dual_copy_allowlist, re/match; no
   re/orchestrator) built with MASHED_NO_DEPLOY=1 MASHED_LOCK_WAIT=off: "=== Build OK ===", mashed_re.exe 4,362,752 B,
   skip line printed. Log: check1_archive_build.log. publish.sh itself not run.
2. PASS. Temp tree holding only build.bat, variable unset: prints
   `[ERROR] mashed_lock.py not found at "<tree>\mashedmod\..\re\orchestrator\mashed_lock.py" ...`, errorlevel 1
   (checked through a calling .bat; a bare `cmd /c` from git-bash showed 0, a harness artifact). Log: check2_missing_lock.log.
3. PASS. mashed_lock_hold_enforce_test.py: 9/9. Log: check3_hold_enforce_test.log.
4. PASS. Worktree build.bat under a test PRIORITY-HOLD (isolated MASHED_LOCK_DIR, owner a sleeper PID): printed
   `[mashed-lock] WAITING (build.bat) - PRIORITY-HOLD ...`, still waiting after 12 s, `clear after 12s` after release.
   Log: check4_build_under_hold.log (the later vswhere line is the build proceeding in a shell without VS env, then stopped by me).

Unfinished scope: none. Object build not run (ddkobj path unchanged).
