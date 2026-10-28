# Merge-train recipes (account2 survey 2026-10-24, read-only, cited)

Used to build merge_train.ps1 / merge_train.json. Candidate-grade where it says so.


Two items you asked for don't exist under the expected names, so they're flagged rather than filled in:
- **Missing conflict rules:** the frida-sweep skill has no rules for `hooks.csv`, `STUBS.md`, `UNCERTAINTIES.md` or the rsp files.
- **No private-root script:** there is no setup script for private roots.

## 1. Conflict rules in frida-sweep
`.claude/skills/frida-sweep/SKILL.md` auto-resolves only three files. Any other conflicting file halts the sweep (SKILL.md:8, 61-64, 107).
- **`mashedmod/build.bat`:** keep both sides' `"%SRC%\...cpp" ^` lines, all before `/link /DLL`. Order doesn't matter (SKILL.md:109-131).
- **`re/frida/hooks_registry.py`:** keep both dict entries and a single closing `}`. If both sides use the same key, halt (SKILL.md:133-164).
- **`re/analysis/CHANGELOG.md`:** keep both sides' lines, newest first, ties broken by session id ascending (SKILL.md:166-186). Entries go below `<!-- ENTRIES -->` and the file is never rewritten (SKILL.md:54).
- **`hooks.csv`, `STUBS.md`, `UNCERTAINTIES.md`, `*_sources.rsp`:** no rule, so the skill halts on them. `re/tools/merge_trackers.py` exists (Glob), but I did not read it.
- Use `-m` on merges, never `-F -`, and use `git -c merge.autoStash=false` (NEXT_SESSION.md:265, 328; memory index).

## 2. Default gates
The runner is a per-lane copy of `verify/savelink_20261023/run_arms.py`, set up through env vars:
- `A4_OUT` (default `D:/MashedWT/kfdot_data/arms`), `A4_ROOT` (default `D:/MashedWT/kfdot_root`), `A4_EXE` (default `<wt>/mashedmod/build/mashed_re.exe`) (run_arms.py:15-18).
- It drops every inherited `MASHED_*` variable and sets `MASHED_ROOT=ROOT` (236-239).
- It runs from the root as cwd and kills only its own PID (267-283).
- Every run appends one line to `OUT/RUNLOG.txt` with `file:lines:sha8` (295-297). It does not create `OUT` (NEXT_SESSION.md:327).
- Usage line: `py -3.12 run_arms.py <arm>...` (run_arms.py:3).
- **It takes no mashed_lock.** It has no lock import at all.
- Base env for H/B arms: `MASHED_MUTE=1 TRACK_VIEW=Training CAR=1 ROUND=1 ROUND_RULE=0 GAME_MODE=10 WIN_POS=primary-bl DETERMINISTIC=1 DET_FRAMES=N` (242-247).

| Gate | Command | GREEN reads as | Runtime |
|---|---|---|---|
| **G-CTRL** | `py -3.12 run_arms.py H_DEF` (14400 frames, `PLAYERTRACE`, `AI_STEPDUMP`, `U9186_GATES`; run_arms.py:427-433) | sha8 of `H_DEF.step.csv / .gates.csv / .ptrace.log` = `bacfc747 / 8dd1c004 / 6f9932b6` (order per RUNLOG d3close:5) | ~291 s (d3close RUNLOG:5) |
| **B** | `py -3.12 run_arms.py B_DEF_K0 B_DEF_K1 B_DEF_K26` (1200 frames, `MASHED_RING_SKIP=k`; 434-439) | step.csv sha8 = `f9deb9b9 / d3601fcc / 9da9a26a` | 36-65 s each (RUNLOG:1-4) |
| **(b) 9/9** | `py -3.12 vsum.py <OUT>/B_DEF_K0.step.csv <OUT>/B_DEF_K1.step.csv <OUT>/B_DEF_K26.step.csv` (d3close vsum.py:3; uses `re/tools/b_envelopes.json`) | `3/3` on each of the three lines (`%-22s %d/3`, vsum.py:21) | offline |
| **(e) 3/3** | `py -3.12 re/tools/ai_speed_env.py --check <OUT>/B_DEF_K0.step.csv` (ai_speed_env.py:31; d3close RESULT:22) | `v1/v2/v3 (e) PASS` and launch `1425.7 / 2052.5 / 2055.0` (rpct e_check_flip.txt:2-7) | offline |
| **D2 3/3** | arms `D2_DEF_1 D2_DEF_2 D2_DEF_3`, run alone (D2BASE env, 90 s wall-kill; 226-229, 440-447), then `py -3.12 verify/d2rebase_20261019/score_d2_v2.py port <OUT>/D2_DEF_{1,2,3}.motion_diag.log` (score_d2_v2.py:8) | PASS 3/3 inside the band in `band.json`. Rerun any arm under 1600 motion_diag lines (memory) | ~90 s each |
| **13 tracks** | `py -3.12 verify/rsky_20261016/run13.py run --exe <exe> --root <root13> --out <dir>`, then `... score --out <dir> --cite <cites.json>` (run13.py:3-4, 187-195) | every row CITED or GREEN, 0 RED, plus `INSTRUMENT GATES: PASS` (rpct tracks_flip_score.txt:42; verdict logic run13.py:169-170) | up to 900 s timeout per track (run13.py:95) |

Notes on the table:
- All game-booting gates run the standalone with `MASHED_ROOT` set to a private root and take no lock. vsum, ai_speed_env and score_d2 are offline scorers.
- For the 13-track check: I could not find which `cites*.json` recent lanes pass. Candidates are listed under `verify/*/cites*.json`, for example `verify/rskyverify_20261016/cites_merged.json`.
- The 13-track check uses a separate `<lane>_root13` (NEXT_SESSION.md:264).
- A G-CTRL run can drop one frame's rows, so rerun before calling it RED (NEXT_SESSION.md:263).
- The live refs are `merge_train.json` `gctrl.ref` / `b.ref`, not the table above. Since the D3-CF-U9779 flip (2026-10-24) they are G-CTRL `ef5f2e10 / 57f7ff83 / b4f4b55a`, B `c37d4c0c / 0a8f9806 / 7013671d`; the table's hashes are the pre-flip defaults (`MASHED_NU7` opt-out).

### Decided default flips
A row that changes the default gates on purpose (a user-decided knob flip, e.g. L-4) would be bounced RED against the old refs. Protocol:
1. The flip row records its expected new G-CTRL and B hashes (ledger note + its RESULT).
2. It merges **alone**: nothing else in the queue while it runs (enqueue the other rows after it lands).
3. The refs change and the flip merge are **one step, nothing merges in between**. With no train live and only the flip row queued, commit the expected new hashes to `merge_train.json` on `orch/integration` (cite the decision and RESULT in `_doc`, keep the old refs there), then run the train for the flip at once. Pull every other row out of the queue first (`roadmap.ps1 dequeue`) and re-enqueue it only after the flip is on `orch/integration`: a row merged against the new refs on a tree without the flip reproduces the old hashes and bounces as a false RED. GREEN means the merged tree reproduces exactly the new hashes; any other value is RED.
4. Run the flip's pass with `merge_train.ps1 -Full`. `-Full` forces the full set (D2 x3 solo, 13 tracks) as well as the build and default gates, because a default can live outside `full_set_paths` (D3-CF-SNAP-FLIP's lives in `D3d9Render/TrackRenderer.cpp`).
5. If the refs were not updated first and the train bounced the flip with exactly the expected hashes, that RED is the flip confirmed: update the refs as in 3, re-enqueue the row alone, run the train again.

## 3. Lint
- Command: `py -3.12 scripts/lint_rva_bodies.py` (lint_rva_bodies.py:27). It also accepts `--verbose` and `--seed`.
- It prints `[rva-lint] N finding(s): ... | allowlisted=K NEW=M` (120-122).
- On a pass it prints `[rva-lint] OK -- no NEW duplicate bodies` and exits 0 (146-148).
- On a new finding it prints `FAILED: N NEW` and exits 1 (136-144).
- The allowlist is `re/tools/dual_copy_allowlist.txt`.

## 4. `mashedmod/build.bat`
- It builds both targets into `mashedmod/build/` (`%~dp0build`): `mashed_re.exe` and `mashed_re_dev.asi` (build.bat:9-11, 229, 258).
- Source lists are `exe_sources.rsp` and `asi_sources.rsp`, compiled through `build_objs.ps1` with a per-TU object cache (225-227, 254-256).
- Env vars it honors:
  - `MASHED_SKIP_RVA_LINT=1` (34)
  - `MASHED_SKIP_ASM_LINT=1` (47)
  - `MASHED_BUILD_FULL=1` (16)
  - `clean` argument (17)
  - `MASHED_NO_DEPLOY` (271)
- **It deploys by default:** it copies the `.asi` into `original\mashed_re_dev.asi`. Any defined `MASHED_NO_DEPLOY` skips that copy (264-277).
- In a worktree, `original\` is a junction to the shared install (267-268). So from a worktree, run `set MASHED_NO_DEPLOY=1` and then `cmd /c mashedmod\build.bat`. Outputs land in `<wt>\mashedmod\build\`.

## 5. Private root (no dedicated script)
1. Copy the install: `robocopy original\ D:/MashedWT/<lane>_root\original /E`. Robocopy exit codes 0-7 mean success (NEXT_SESSION.md:264; `verify/librwrest_checks_20261016/run_stand.py:39-40`). Some roots keep `original\` inside the root (NEXT_SESSION.md:469).
2. Set an AppCompat layer on that copy's path: `~ RUNASINVOKER WIN98RTM HIGHDPIAWARE EMULATEHEAP` (NEXT_SESSION.md:69-70). `scripts/setup_mashed_compat.ps1` only targets `original\MASHED.exe` (line 35), so a lane has to set the HKCU Layers value for its own root path.
3. Copy the freshly built `mashed_re_dev.asi` into the root by hand. A `MASHED_NO_DEPLOY` build does not refresh it (NEXT_SESSION.md:103).
4. Launches must run with the tool sandbox disabled (NEXT_SESSION.md:68).

## 6. Lock callers (main checkout; `.worktrees` excluded)
**`re/orchestrator/mashed_lock.py`** ÔÇö used as an import plus context manager, or from the CLI (mashed_lock.py:23-30):
- `re/frida/run_diff_scenario_batch.py:110` imports it. Line 621: `MashedLock(f"state_batch:{names[0]...}")`.
- `re/frida/stalker_write_surface.py:52` imports it. Line 168: `MashedLock(f'stalker:{rva:#010x}')`.
- `re/frida/stalker_write_surface_batch.py:40` imports it. Line 186: `MashedLock('stalker_batch')`.
- `re/frida/probe_save_globals.py:34` imports it. Line 216: `MashedLock('probe_save_globals')`.
- Inside the module: line 183, `with MashedLock(label=label, name=name, wait=wait)` backs the `run` subcommand.
- Docs only: `.claude/skills/orchestrate/SKILL.md:129` (`py -3.12 re/orchestrator/mashed_lock.py status | break`), `ORCHESTRATOR.md:150-154`, `exec_pipeline/README.md:23-34`.

**`re/orchestrator/MachineLock.ps1`** ÔÇö dot-sourced, exposing `Acquire-Lock -Name -Label` and `Release-Lock $h` (lines 6-8, 23, 64):
- `re/orchestrator/exec_pipeline.ps1:33` dot-sources it. Line 48: `Acquire-Lock -Name 'mashed_build' -Label 'exec-pipeline build'`. Line 56: `Release-Lock`.

No caller exists in `scripts/` or `mashedmod/`, and none of the gate runners take the lock.

## 7. `scripts/diag.py wt-remove`
- Single worktree: `py -3.12 scripts/diag.py wt-remove <path>`. Bulk: `py -3.12 scripts/diag.py wt-remove --all-stale` (diag.py:520-521).
- It strips top-level junctions first, then runs `git worktree remove` without `--force`, then `git worktree prune` (430-458).
- Exit codes: 2 if the path is missing or not a directory, 1 if the worktree is dirty or refuses removal, 0 on success.
- `--all-stale` keeps any worktree that is dirty, has commits ahead of `main`, or whose index changed less than `--min-age-days` ago (default 3). It prints `removed N, kept M` (441-488).
- frida-sweep's teardown runs it per merged branch, followed by `git branch -d` (never `-D`) (SKILL.md:97-98).
