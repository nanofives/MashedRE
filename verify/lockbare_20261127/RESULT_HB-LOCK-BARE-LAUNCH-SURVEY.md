# RESULT HB-LOCK-BARE-LAUNCH-SURVEY (run 57, G4, meta/tooling row)

Verdict: survey GREEN, routing partial. 472 bare launch sites found, 145 routed through MashedLock, 15 are not game launches, 312 filed with a reason. No game was launched, no build, no gate, binds no RVA.

## Method
`re/tools/bare_launch_survey.py` (new, read-only) walks re/ scripts/ verify/ mashedmod/ for *.py *.ps1 *.sh *.bat. A site is a launcher token (Popen, subprocess.run/call, Start-Process, os.startfile, os.system, frida spawn, .spawn()) with a game token (MASHED.exe, mashed_re.exe, an EXE variable) within 6 lines. A file is LOCKED when it names mashed_lock / MashedLock, else BARE. `match=hard` means the literal MASHED.exe / mashed_re.exe is in the call, `soft` means a variable (EXE, MASHED_EXE) named it.
Raw output: `survey.tsv` (before), `survey_after.tsv` (after), per-site `disposition.tsv` (BARE rows only, each with file:line).

## Counts
- Sites found: 533 (before). BARE 472 (hard 25, soft 447) in 426 files. LOCKED 61.
- Fixed (guard): 145 sites in 132 files, all re/ and scripts/ *.py (119 in `re/frida`, 10 in `re/tools`, 1 each in `re/parity`, `re/analysis/negctl`, `scripts/diag.py`).
- Not a game launch: 15 (git / lib.exe / bash pool scripts / the survey tool's own text: `re/frida/run_diff_parallel.py:44`, `re/tools/gen_portstate_seed.py:49`, `re/tools/veccap/veccap_registry.py:185`, `re/tools/xref.py:151`, `re/tools/spawner_audit.py:5,6,25`, `re/tools/bind/test_porttop_bind_lib.py:177,182,226,230` (runs a freshly built test exe, not the game), `re/tools/ddkobj/mathprobe/ddkcrt_rename.py:43,52`, `re/tools/veccap/replay_offline.py:85,106`).
- Filed: 312 (below). After the change the survey shows BARE 327 = 312 filed + 15 not-a-launch. 472 = 145 + 15 + 312.

## Fix (the one mechanism)
`re/orchestrator/mashed_launch_guard.py`: one import line wraps `subprocess.Popen` and frida `Device.spawn`. The first launch whose argv names MASHED.exe / mashed_re.exe takes `MashedLock` (so it waits on SOLO and PRIORITY-HOLD, HB-LOCK-ENFORCE) and holds it until exit. It skips when this process already holds the lock, and when the lock owner is an ancestor (script run under `mashed_lock.py run`, which would otherwise deadlock the non-reentrant lock). `re/tools/inject_launch_guard.py` inserted the line before the first non-`__future__` import of each file (132 one-line insertions, `git diff --shortstat`: 132 files, 132 insertions, 0 deletions). The list of edited files is `git diff --name-only`; each call site is in `disposition.tsv` as FIXED-guard with its file:line.
Per-site file:line for all 472 BARE rows: `disposition.tsv`. The hard ones (literal exe in the call), by disposition:
- FIXED-guard (7): re/frida/c4_hud_cup_probe.py:56, canon_observe_refused.py:69, diag_smoke.py:67, observe_c4_render_math_hooks.py:130, observe_c4_save_hooks.py:426, observe_lap_hooks_at_menu.py:137, coverage_census.py:180.
- FILED-shell-wrapper (1): scripts/b19_navcheck.ps1:23. Soft: scripts/mashed_shot.ps1:20, scripts/observe_runtime.ps1:48 (PowerShell cannot import the Python guard; callers wrap the whole script in `mashed_lock.py run`; filed, a later row can add a `.ps1` helper). re/tools/pu_replay/sweep.ps1:43 runs an offline replay tool, not the game.
- FILED-verify-archive (11 hard): verify/audiolink_20261023/runtime_table.py:14, verify/d3_bulk_20261011/tools/peek.py:23, verify/menuanim_20261027/orig_anim_poll.py:11, verify/racelua2_20261122/vfs_watch.py:173, verify/d4dc82a0_20261025/run_all_sa.ps1:8, verify/playtest_loading_disc_20261122/run_orig.ps1:6 and run_re_req.ps1:9, verify/scr19_parity_20261012/{after_fix,arrow_fix}/run_sa.ps1:16, scr32_fix/run_sa.ps1:17, scr32_texty/run_sa.ps1:19.
- FILED-verify-archive soft: 259 py + 38 ps1 sites under verify/ (all rows in `disposition.tsv`).

## Why verify/ is filed, not edited (312 sites)
verify/ holds per-row evidence scripts of finished rows (dated folders 20260929 to 20261122). Editing them rewrites the provenance of committed evidence, and WORKER_RULES ("Builds and game runs") already requires a row to wrap each run in `mashed_lock.py run`. Two risks stay: a worker copies an old script as a template, or runs it unwrapped. Mitigation that exists now: any new script copied from re/ gets the guard line with it. A follow-up row can add the guard line to a verify/ folder when a row reuses it (rule: add on reuse). Not filed as open work because nothing runs them unwrapped by policy.

## Checks run
1. `py -3.12 re/orchestrator/mashed_launch_guard_test.py` (under the test slot): 8/8 PASS (`guard_test.log`): game launch takes the lock (owner == self), lock free after exit, non-game Popen takes nothing, under `mashed_lock.py run` two launches finish with no deadlock (1.6 s), waits while another process holds the lock, proceeds after release, waits under PRIORITY-HOLD, proceeds when the hold owner dies.
2. Compile check of all 132 edited files: 132 compiled, 0 bad.
3. Guard line of `re/tools/sa_capture.py` executed in isolation: imports, installs, `mashed_launch_guard` in sys.modules.
4. Re-survey after the edit (`survey_after.tsv`): BARE 327 = 312 filed + 15 not-a-launch, LOCKED 209.
Not run: no build, ddkobj check, selftest or gate (no code path of the exe changed), no game arm. `mashed_lock.py` itself is unedited, so its own tests were not re-run. Frida folders: no harness run, 0 frida-<hash> folders created.

## Caveats / [UNCERTAIN] (two lines in `re/orchestrator/UNCERTAIN_REGISTER.md`)
- [UNCERTAIN] each guarded script was compiled and its guard line import-run, not booted; whether every site launches through Popen / `Device.spawn` rests on reading code. A launch by os.startfile or ctypes CreateProcess would not be wrapped (the survey found none by text).
- [UNCERTAIN] the 61 LOCKED sites are classified by file, not call path: a launch could sit outside its `with MashedLock`.
- The guard holds the lock for the script's whole life, not per boot, so two guarded scripts queue as before, and a guarded script that sleeps long keeps others waiting. Same behaviour as `mashed_lock.py run`.
- Unfinished: PowerShell launchers (4 files above) and the verify/ archive stay bare by filing; no ledger row opened for them.

## Files
New: re/orchestrator/mashed_launch_guard.py, mashed_launch_guard_test.py, re/tools/bare_launch_survey.py, inject_launch_guard.py, this folder. Edited: 132 scripts (one line each), re/orchestrator/UNCERTAIN_REGISTER.md.
