# RESULT D5-SHIM-CONSUMERS (static, text-only, 2026-10-30)

Method: grep and read on `orch/D5-SHIM-CONSUMERS` (from `orch/integration` 9d3e2421c). No build, no run, no Ghidra, no deletion, no source or script edit. Input: R17-R19 of `verify/d5shimstatic_20261030/RESULT_D5-SHIM-STATIC.md`. File counts come from `rg -l` over the tree minus `mashedmod/src`, `verify`, `log`, `re/analysis`, `*.md`, `*.bak`.

SHIM VERDICT: d3d9 shim STAYS (dev harness, out of the shipping matrix, not deleted): the original-side parity and diff lane has no replacement for its windowed 640x480 backbuffer, `MASHED_ORIG_BBDUMP`, draw-count capture, `MASHED_PARITY_BG` and frame limiter. dinput8 shim STAYS for now (dev harness only, not shipped): the Frida lane does not need it, but the player launcher `scripts/mashed_launch.ps1` (`Play Mashed.bat`) requires `dinput8.dll` plus `mashed_qol.asi`, and an autoload-reliance sweep is still owed. Both are already excluded from the LAN release.

## 1. What each shim does for the original (what a removal takes away)

| Shim | Feature | Evidence |
|---|---|---|
| d3d9 | Forces windowed 640x480 `CreateDevice`; `MASHED_RES`, `MASHED_HIRES`, `MASHED_WIN_POS`, borderless, screen pin | `d3d9_shim.cpp:69-161` |
| d3d9 | Backbuffer dump `MASHED_ORIG_BBDUMP` and on-demand `MASHED_ORIG_BBDUMP_REQ` (the original-side half of the parity harness) | `d3d9_shim.cpp:241, 335` |
| d3d9 | Frame limiter `MASHED_FPS_CAP`, `_CAP_RACE`, `_LOG`, OSD `MASHED_FPS_OSD` | `d3d9_shim.cpp:362-374, 464` |
| d3d9 | `MASHED_PARITY_BG`, aggregate draw counters (`draw3d.json`) | `d3d9_shim.cpp:562`, ROADMAP lines 440-442 |
| dinput8 | Autoloads `mashed_qol.asi` then `mashed_re_dev.asi` (next to the DLL, else `..\mashedmod\build\`) unless `MASHED_RE_NO_AUTO_HOOK=1` | `dinput8_shim.cpp:34-96` |
| qol_asi | Player QoL on the original: `MASHED_NO_SAVE`, `MASHED_UNLOCK`, `MASHED_SPEED`, decouple, borderless | `mashed_qol.cpp:14-20`, `mashed_launch.ps1:47-114` |
| dll_main / asi | Entry point of `mashed_re_dev.asi`, the hook harness diff-original needs | `dll_main.cpp:1-5`, `run_diff.py:43` |

## 2. Does the original-side lane still need the d3d9 shim after D5? YES

- **640x480 coupling.** `scripts/patch_mashed_fix_camera_res.py:29-31` states 640x480 must equal the shim's forced backbuffer. The patch is one of the ten boot patches (CLAUDE.md "Runtime state"). Remove the shim and the original renders 640x480 internals into an unforced device, or fullscreen (`frida-sweep` SKILL.md:36 says that without the shim "any spawned MASHED.exe runs fullscreen"). The patch and the shim retire together or not at all.
- **Original-side capture.** `re/parity/sweep.py:277-280` refuses to run without `original\d3d9.dll`. The same pre-check sits in `re/frida/`: `auto_count_at_menu.py:123`, `canonical_observe_viewport_init.py:120`, `hot_path_behavioral.py:182`, `mass_canonical_observe{,_r2,_r3,_wave0}.py`, `camera_probe.py:859`, `cam_frame_writer_watch.py:188`, `pickup_pos_probe2.py:267`. Other BBDUMP users: `race_draw_burst.py`, `race_hud_burst.py`, `speed_probe.py`, `capture_orig_screens.py`, `frontend_parity.py`, `chal_icon_probe.py`, `re/tools/{sa_capture,sa_boostwatch,sa_headwatch,sa_leaderwatch,band_slide_check,ab_report,focus_pause_check}.py`.
- **Frame limiter.** `MASHED_FPS_CAP` is set by `capture_player_dynamics.py:112`, `capture_qhull_hull.py:155`, `capture_relight_parity.py:163`, `coverage_census.py:176`. Without the shim the original runs uncapped under capture, which changes frame-count based windows.
- **Unaffected by D5.** The exe has its own pacing and never reads these (D5-SHIM-STATIC R3-R5). The shim was never in the shipping matrix.
- **Per lane.** diff-original (`run_diff.py`, `scenario_launch.py`): needs the asi, which it loads by `Module.load` of `mashedmod\build\mashed_re_dev.asi` with autoload suppressed, so it needs the d3d9 shim but not the dinput8 proxy. Parity harness (`sweep.py`, `menu_draw_burst.py`, `race_draw_burst.py`): needs d3d9. `MASHED_ORIG_BBDUMP`: d3d9 only.

## 3. dinput8 proxy: who really needs it

- Frida lane: `MASHED_RE_NO_AUTO_HOOK` appears in 149 files (`rg -l` count), and the proxy comment (`dinput8_shim.cpp:62-67`) says the Frida harness sets it so `Module.load` is not blocked. So the lane does not use the proxy to load the asi.
- Player launcher: `scripts/mashed_launch.ps1:57` lists `d3d9.dll`, `dinput8.dll`, `mashed_qol.asi` as required in `original\`. `Play Mashed.bat` calls it.
- Probe scripts that name the real module (`boot_nullbadfopen.py:47`, `boot_skip_badlock.py:50`, `boot_probe_heaphook.py:43`, `input_feed.py:72`, `input_recon.py:71`, `input_diag.py:34`, `find_keystate.py:14`) look up `dinput8_real.DLL`, `DINPUT8.dll` and `dinput8.dll` in turn, so they work with or without the proxy.
- [UNCERTAIN] Whether any script spawns the original and expects autoload without setting `MASHED_RE_NO_AUTO_HOOK`. Missing evidence: my attempt to list spawners lacking the variable failed (`rg` not on the bash PATH inside `xargs`). Method for the next row: Grep tool over `re scripts verify` for files matching `frida.spawn|subprocess.Popen` and `MASHED.exe` that lack `MASHED_RE_NO_AUTO_HOOK`.

## 4. Consumer map (outside the exe)

| Consumer | Needs | Breaks if removed |
|---|---|---|
| `mashedmod/build.bat:199-207, 242-290` (R1, R10) | Deletes stray proxies in `%OUT%`, builds and deploys the asi | R1 loop is a no-op once no builder writes proxies there. R10 deploys the asi, still wanted by the dev lane |
| `mashedmod/build_d3d9_shim.bat` | Builds `original\d3d9.dll`, copies `d3d9_real.dll` | Only path that creates the shim. Breaks every original-side capture on a fresh PC |
| `mashedmod/build_dinput8_shim.bat` | Builds `dinput8.dll`, `dinput8_real.dll` | Breaks asi autoload and `mashed_launch.ps1` |
| `mashedmod/build_qol_asi.bat` | Builds `mashed_qol.asi` | Breaks `mashed_launch.ps1` (QoL, decouple, no-save) |
| `mashedmod/build_bisect_asi.bat`, `build_minimal_asi.bat`, `scripts/bisect_asi_boot.ps1:13` | Asi variants, text cites the dinput8 fallback path | Bisect lane only |
| `scripts/mashed_launch.ps1`, `Play Mashed.bat`, `Play Mashed (TV).bat` | d3d9 + dinput8 + qol asi + `MASHED_FPS_*` | Player launch of the ORIGINAL stops. Not the exe launcher (`lan_release` ships its own `Play Mashed.bat`) |
| `scripts/setup_mashed_compat.ps1:15-19` | Comment: never set `DISABLEDXMAXIMIZEDWINDOWEDMODE` while the shim is deployed | Text only, but the rule holds while the shim stays |
| `scripts/repatch_original.py:122` | Prints "rebuild the d3d9 shim" as step 1 | Message only |
| `scripts/patch_mashed_fix_camera_res.py:29-31` | 640x480 equals shim backbuffer | The coupling above |
| `re/parity/sweep.py`, `recipes.toml` | `original\d3d9.dll`, BBDUMP | Parity harness original arm |
| `re/frida/*` (about 85 files name `mashed_re_dev`, 27 name the d3d9 shim, 12 name `MASHED_FPS_`) | See section 2 | diff-original, canonical observe, probes |
| `re/tools/` (`sa_*`, `focus_pause_check.py`, `ab_report.py`, `band_slide_check.py`, `asi_only_triage.py`) | BBDUMP, FPS log, asi names | Original-side measurement |
| `re/orchestrator/merge_train.ps1:266-268`, `mashed_lock.py:4` | Builds `mashed_re_dev.asi`; treats "the d3d9 shim" as the shared GPU resource | Merge train build leg fails if the asi target goes |
| `.claude/skills/frida-sweep/SKILL.md:36`, `promote-c3-batch/SKILL.md:381` | Precondition `original/d3d9.dll`; rebuild canonical asi | Skill text goes stale if shims go |
| `scripts/lan_release/stage_lan_release.py:21`, `publish.sh:13` (R8, R9) | Exclusion list and `MASHED_NO_DEPLOY=1` | Nothing breaks. The exclusion is harmless when files vanish |
| `hooks.csv` | Mentions of the asi in file columns | Tracker, re-classify only |
| `verify/**` | 5 old probe scripts under `scr19_parity_20261012` name `d3d9.dll` or BBDUMP; many `PROVENANCE.txt` | History, not run again |
| `milestone_a_*.txt`, `re/frida/diff_template.js(.bak)` | Notes, template | Text |

## 5. Removal order (the shims stay, so this is the order IF Mariano later retires them)

1. `Play Mashed.bat`, `Play Mashed (TV).bat`, `scripts/mashed_launch.ps1`: retire or repoint at the exe. Breaks: original-with-QoL play. Prerequisite for step 2.
2. `mashedmod/src/qol_asi`, `build_qol_asi.bat`, `original\mashed_qol.asi`: breaks only step 1's launcher. `MASHED_SPEED` for `force_screen.py:224-239` goes with it.
3. `mashedmod/src/dinput8_shim`, `build_dinput8_shim.bat`, `original\dinput8.dll`, `original\dinput8_real.dll`: breaks any spawner without `MASHED_RE_NO_AUTO_HOOK` (section 3 [UNCERTAIN]). Run the sweep first.
4. `mashedmod/build.bat` R1 loop (`:199-207, 279-290`): dead code after step 3.
5. `mashedmod/src/d3d9_shim`, `build_d3d9_shim.bat`, `original\d3d9.dll`, `original\d3d9_real.dll`: LAST. Needs the whole original-side lane retired first: `sweep.py`, about 12 Frida pre-checks, all BBDUMP users, `fix_camera_res`, and the ten-patch recipe. Not planned.
6. `mashedmod/src/mashed_re/dll_main.cpp`, `asi_sources.rsp`, `original\mashed_re_dev.asi`: only when no diff-original is wanted. ROADMAP D5 says "Dev .asi out of the shipping matrix", not removed. `merge_train.ps1` must drop its asi build leg first.

## 6. Edits a child may do (text and source only, no .ps1/.bat)

1. `mashedmod/src/mashed_re/LibRw/RwBridge.cpp:237-241` comment reword (R2). Source edit in an exe build input, so give it its own row and run the default-gates check.
2. `.claude/skills/frida-sweep/SKILL.md:36` and `promote-c3-batch/SKILL.md`: add "dev-only, stays after D5". Text.
3. `scripts/patch_mashed_fix_camera_res.py` docstring: note the coupling survives D5. A `.py`, docstring only.
4. `scripts/lan_release/stage_lan_release.py:21` docstring wording. A `.py`, text only.
5. CLAUDE.md "Runtime state" and ROADMAP D5 lines 3006-3007: one line "dev harness stays, not shipped". Do not edit ROADMAP without a text row (WORKER_RULES).
6. Optional audit script: Grep-based list of spawners lacking `MASHED_RE_NO_AUTO_HOOK` (section 3), output into `verify/`. No source touched.

## 7. Route to Mariano (no child edits)

- `mashedmod/build.bat` R1 loop and R10 (split the asi leg or make deploy opt-in). R10 is wanted either way, because a plain `build.bat` still redeploys the shared asi.
- `mashedmod/build_d3d9_shim.bat`, `build_dinput8_shim.bat`, `build_qol_asi.bat`, `build_bisect_asi.bat`, `build_minimal_asi.bat`.
- `scripts/mashed_launch.ps1`, `scripts/setup_mashed_compat.ps1`, `scripts/bisect_asi_boot.ps1`, `re/orchestrator/merge_train.ps1`, `Play Mashed.bat`, `Play Mashed (TV).bat`.
- Decision: keep the original-with-QoL launcher after D5 or not (gates step 1 above).

## Limits

- No build or run. The `original\` file list was read by `ls` only (d3d9.dll Sep 2, dinput8.dll Aug 1, mashed_qol.asi Aug 30, mashed_re_dev.asi Oct 24).
- File counts come from `rg -l` with the exclusions above, so `verify/` and `re/analysis/` references are not counted. The 5 `verify` scripts are the only run scripts there that match.
- The autoload-reliance sweep was not done (section 3).
