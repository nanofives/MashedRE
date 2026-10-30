# RESULT D5-KNOBS-D-TRIAGE-1 (2026-10-30)

Text-only triage of the first 63 class (d) rows of `verify/d5knobsinv_20261030/RESULT_D5-KNOBS-INVENTORY.md` (table order, lines 481-543 of that file). No source edit, no tracker edit, no build, no game run.

**Verdict: GREEN. 63 of 63 rows have one verdict and a citation.**

| verdict | count |
|---|---|
| DEV-KEEP | 39 |
| DELETE-CANDIDATE | 24 |
| PENDING | 0 |
| DEAD | 0 |
| total | 63 |

## Method

- Read site and 6 to 20 surrounding lines for each knob (`S` below = `mashedmod/src/mashed_re/`).
- One `grep -rIoE` pass over `scripts/`, `ROADMAP.md`, `re/` and `verify/` (`*.md *.py *.ps1 *.sh *.bat *.json *.tsv *.txt *.js *.csv`) for the 63 names, plus a `grep -n` over `mashedmod/src` for every source site.
- Owner search for PENDING: `re/orchestrator/DECISIONS.md`, live ledger `log/orch/roadmap.json`, `ROADMAP.md`, `re/NEXT_SESSION.md`. None of the 63 names appears in the ledger or in DECISIONS.md, so no row is PENDING.
- Verdict definitions: DEV-KEEP = debug, measurement or harness input. DELETE-CANDIDATE = the default path is the finished port and the knob only selects the superseded path (or the knob was seeded ON). DEAD = no reader or unreachable reader (none found: every one of the 63 has a reachable read).

## Findings that change the inventory

1. **Six rows are default ON, not OFF.** `Core/FaithDefaults.cpp` seeds the process environment before any read (file header lines 3-12: "MASHED_NO_<name>=1 skips that one knob"). The inventory scanner reads the site line only, so its `OFF (unset -> off)` default column is wrong for `A5_DRAFT_LUT` (FaithDefaults.cpp:38), `A6B_ACOS_CRT` and `A6B_NORM_LUT` (:61), `AI_CONTACT_EVENT` and `AI_TICK_FIRST` (:30-31), `CC_PC24` (:36). They are class (b) in effect and are DELETE-CANDIDATE below. Their source comments still say "default OFF" (for example `S/Vehicle/AeroStabilize.cpp:25`, `S/Ai/AiStandalone.cpp:2259`, `S/Collision/CarCarContacts.cpp:99`); those comments are stale.
2. **The other 18 DELETE-CANDIDATE rows are not seeded.** 12 are default-ON opt-outs (`!(e && e[0]=='0')` or `Knob(name, true)`: rows 16-19, 33, 52-55, 60-62), 3 select a superseded path (rows 30, 35, 59), and 3 are a vsync opt-out (27), a finished tuning aid (31) and a finished experiment (56).
3. **Zero PENDING.** The 63 are either harness or diagnostic inputs, or opt-outs of a finished path. No default-OFF behaviour change without an owner exists in this half.
4. Rows with zero consumers outside the source file (only the inventory names them): `CAR_COLOUR`, `FONT_FLATRGB`, `FONT_GAMMA`, `LIBRW_LIFT`, `ABIL_KEYS`. They still have a reachable reader, so none is DEAD.

## Table

Evidence paths are repo-relative. `S/` = `mashedmod/src/mashed_re/`. `FI` = `re/analysis/FLAG_INVENTORY_2026-08-15.md`. `LS` = `re/analysis/LIBRW_SIZING_2026-08.md`.

| # | knob | site | verdict | evidence | why |
|---|---|---|---|---|---|
| 1 | MASHED_A5_DRAFT_LUT | S/Vehicle/ForceIntegrator.cpp:33 | DELETE-CANDIDATE | S/Core/FaithDefaults.cpp:38; re/analysis/CHANGELOG.md (2026-10-20 d2off row) | Seeded ON in the FAITH default; opt-out is `MASHED_NO_A5_DRAFT_LUT`. |
| 2 | MASHED_A6B_ACOS_CRT | S/Vehicle/AeroStabilize.cpp:95 | DELETE-CANDIDATE | S/Core/FaithDefaults.cpp:61; re/NEXT_SESSION.md:425 ("Default now" list) | Seeded ON; the stale "default OFF" comment is AeroStabilize.cpp:25. |
| 3 | MASHED_A6B_NORM_LUT | S/Vehicle/AeroStabilize.cpp:96 | DELETE-CANDIDATE | S/Core/FaithDefaults.cpp:61; re/NEXT_SESSION.md:425 | Seeded ON, same as row 2. |
| 4 | MASHED_ABIL_KEYS | S/exe_main.cpp:6323 | DEV-KEEP | S/exe_main.cpp:6286-6291 ("verification poke, display-only, not set in normal play"); re/analysis/race_hud_capture_20260902.md:2513 | Alias of the `MASHED_TEAM_KEYS` tap parser. No script sets it (grep of scripts/ re/ verify/). |
| 5 | MASHED_AILEAF_PC | S/Ai/AiLeafTargets.cpp:281 | DEV-KEEP | verify/d3_gatefire_20261008/run_h4.ps1:39 (arm `AL_ON53` passes `MASHED_AILEAF_PC=53`); S/Ai/AiLeafTargets.cpp:279 | x87 precision selector, default 24 (the race loop's). Harness arm input. |
| 6 | MASHED_AI_CONTACT_EVENT | S/Ai/AiStandalone.cpp:2313 | DELETE-CANDIDATE | S/Core/FaithDefaults.cpp:30; verify/a5flip_20261026/run_arms.py:43-52 (stacks `E`, `EL`, `SmE`) | Seeded ON; run_arms.py uses it as an arm toggle, which goes with it. |
| 7 | MASHED_AI_DIFFFLAG | S/Ai/AiStandalone.cpp:2389 | DEV-KEEP | S/Ai/AiStandalone.cpp:2380-2386 ("a measurement aid, not a port of anything"); verify/d3_gatefire_20261008/PREREG_414180.md:76; re/analysis/D3_AI_RESIDUE_2026-09-27.md | Seeds `I32(0x0089a368)` so two captures compare in the same regime. No script sets it. |
| 8 | MASHED_AI_TICK_FIRST | S/D3d9Render/TrackRenderer.cpp:5508 | DELETE-CANDIDATE | S/Core/FaithDefaults.cpp:31; verify/a5flip_20261026/run_arms.py:43-58 | Seeded ON; stale "runs on countdown frames" comment at TrackRenderer.cpp:5455. |
| 9 | MASHED_C4N | S/Collision/NarrowC4Probe.cpp:54 | DEV-KEEP | S/Collision/NarrowC4Probe.cpp:24 ("Arm with MASHED_HOOK_ONLY=L2_C4N_<rva> and MASHED_C4N=1"); verify/wsbnarrow_20261026/run_c4n.py:56 | In-process A/B probe for the narrow-phase port. |
| 10 | MASHED_C4N_STRIDE | S/Collision/NarrowC4Probe.cpp:62 | DEV-KEEP | S/Collision/NarrowC4Probe.cpp:24; verify/wsbnarrow_20261026/run_c4n.py:57 | Sampling stride of the same probe. |
| 11 | MASHED_CAM_FOV | S/D3d9Render/TrackRenderer.cpp:9759 | DEV-KEEP | S/D3d9Render/TrackRenderer.cpp:9744 ("still overrides, for A/B against a capture"); verify/d1_lens/RESULT.md:44,53 | FOV override used for the lens A/B table. No script sets it. |
| 12 | MASHED_CARPARTS_LAW | S/D3d9Render/TrackRenderer.cpp:4637 | DEV-KEEP | S/D3d9Render/TrackRenderer.cpp:10149 ("default OFF, one shot"); verify/librwrest_checks_20261016/run_stand.py:10,29 | Replay input for the car-parts pose check. |
| 13 | MASHED_CARPARTS_LAW_OUT | S/D3d9Render/TrackRenderer.cpp:4638 | DEV-KEEP | S/D3d9Render/TrackRenderer.h:383 ("DIAGNOSTIC"); verify/librwrest_checks_20261016/run_stand.py:65 | Output path of row 12. |
| 14 | MASHED_CAR_BRAKE_ROWS | S/D3d9Render/TrackRenderer.cpp:10229 | DEV-KEEP | S/D3d9Render/TrackRenderer.cpp:10223 ("MEASUREMENT ONLY, default unset"); verify/carlod2_20261016/run_arm.py:84 | Instrument input for flag 0x800. |
| 15 | MASHED_CAR_COLOUR | S/exe_main.cpp:2114 | DEV-KEEP | S/exe_main.cpp:2112-2115 (overrides `cfg.cars[0].colour` after the frontend cursor) | Dev override of car 0 colour. Zero consumers in scripts/ re/ verify/ ROADMAP.md. |
| 16 | MASHED_CAR_ENV | S/LibRw/RwCarFx.cpp:153 | DELETE-CANDIDATE | S/LibRw/RwCarFx.cpp:146-160 ("DEFAULT ON together with MASHED_CAR_VIS, opt-out MASHED_CAR_ENV=0"); verify/carlod_20261016/RESULT_carlod.md | Tri-state read; the unset path returns `CarVisEnabled()` (ON). |
| 17 | MASHED_CAR_ENV_PREVCAM | S/D3d9Render/TrackRenderer.cpp:10218 | DELETE-CANDIDATE | S/D3d9Render/TrackRenderer.cpp:10215-10222 (default ON, `=0` is "the rtex behaviour; no-op control"); verify/carlod_20261016/run_all.py:43 (arm `nolag4`) | Opt-out of the finished previous-camera coefficient. Delete together with the `nolag4` arm. |
| 18 | MASHED_CAR_LIVERY | S/D3d9Render/TrackRenderer.cpp:4287 | DELETE-CANDIDATE | S/D3d9Render/TrackRenderer.cpp:4285-4290 (`=0` restores "the pre-librwrest behaviour"); verify/librwrest_checks_20261016/PREREG_librwrest_checks.md | Opt-out of per-slot liveries. |
| 19 | MASHED_CAR_PARTS | S/D3d9Render/TrackRenderer.cpp:4311 | DELETE-CANDIDATE | S/D3d9Render/TrackRenderer.cpp:4306-4314 (`=0` draws "in the streamed pose, as before"); verify/librwrest_20261016/RESULT_librwrest.md | Opt-out of the FUN_00420870 part poses. |
| 20 | MASHED_CAR_SEL | S/exe_main.cpp:2120 | DEV-KEEP | verify/a4order_20261017/run_arms.py:76 (`'MASHED_CAR_SEL': '0'` in the arm env); S/exe_main.cpp:2117; FI:78 | Selects car 0's type; every run_arms.py arm env sets it. |
| 21 | MASHED_CC_PC24 | S/Collision/CarCarContacts.cpp:97 | DELETE-CANDIDATE | S/Core/FaithDefaults.cpp:36; re/analysis/CHANGELOG.md (u9769 row, 2026-10-19) | Seeded ON; stale "default OFF" comment at CarCarContacts.cpp:99. |
| 22 | MASHED_CUP | S/exe_main.cpp:1657 | DEV-KEEP | S/exe_main.cpp:1653-1655 ("G4: championship CUP ... Default 1"); FI:84; re/analysis/PLAYTHROUGH_G4_2026-06-18.md | G4 playthrough harness round count. No script sets it. |
| 23 | MASHED_D2BSP_KEY0 | S/Collision/CollisionWorld.cpp:239 | DEV-KEEP | S/Collision/CollisionWorld.h:47-49 ("forces the batch key to 0 (diagnostic)"); verify/d2bsp_20261016/run_arms.py:22 (stack `WK0`) | Diagnostic only, no opt-out of a path. |
| 24 | MASHED_D2CHUNK_FRAMEMS | S/Vehicle/VehiclePhysicsRun.cpp:1666 | DEV-KEEP | S/Vehicle/VehiclePhysicsRun.cpp:1129 ("(diagnostic)"); verify/d2bsp_20261016/run_arms.py:25 (stack `L0`) | Keeps the pre-fix `frameMs` for A4 in an A/B arm. |
| 25 | MASHED_D2LAT_SPAWN_YAW | S/D3d9Render/TrackRenderer.cpp:1716 | DEV-KEEP | S/D3d9Render/TrackRenderer.cpp:1709 ("DIAGNOSTIC ONLY, default OFF, never a fix"); verify/d2lateral_20261016/run_arms.py:25,34 | Spawn-yaw probe used by the `Y`/`YL` arms. |
| 26 | MASHED_D2SINK_SM | S/Vehicle/D2SinkProbe.cpp:40 | DEV-KEEP | S/Vehicle/D2SinkProbe.cpp:39-41 (armed only when the probe path is also set); re/tools/statediff/a20_wheelstate.py:12; verify/d2_wheelstate_20261002/PREREG_STEP2.md | Wheel-state snapshot probe. Other sites (S/Collision/CarWorldContacts.cpp:82, S/Collision/WheelContactSolver.cpp:105,284) only emit probe lines. |
| 27 | MASHED_D3D_NOVSYNC | S/exe_main.cpp:3121 | DELETE-CANDIDATE | verify/d5shimstatic_20261030/RESULT_D5-SHIM-STATIC.md:19 (row R3, "env opt-out" of `PresentationInterval = D3DPRESENT_INTERVAL_ONE`, exe_main.cpp:3081) | Opt-out of the shipped vsync pacing. [UNCERTAIN] whether a no-vsync escape hatch is wanted: no doc names a user of it. |
| 28 | MASHED_D3D_REF | S/exe_main.cpp:3120 | DEV-KEEP | S/exe_main.cpp:3109 ("try the REF software device FIRST"); FI:14,88 | Device-selection diagnostic for hang triage (re/analysis/INITD3D9_HANG_AND_REMEASURE_2026-07-14.md). |
| 29 | MASHED_DECOUPLE | mashedmod/src/qol_asi/mashed_qol.cpp:1494 | DEV-KEEP | scripts/mashed_launch.ps1:104; mashedmod/src/qol_asi/mashed_qol.cpp:47 | Launcher sets it for the QoL .asi. |
| 30 | MASHED_FONT_FLATRGB | S/D3d9Render/MashedFont.cpp:227 | DELETE-CANDIDATE | S/D3d9Render/MashedFont.cpp:213-227 (default RGB = coverage; `=1` restores flat 255, the heavier-edge path measured at MashedFont.cpp:222-225) | Opt-out to the superseded glyph path. Zero consumers. |
| 31 | MASHED_FONT_GAMMA | S/D3d9Render/MashedFont.cpp:210 | DELETE-CANDIDATE | S/D3d9Render/MashedFont.cpp:203-208 (original draws raw coverage; env "lets the boldness be tuned ... while we dial it in"); FI:110 | Tuning aid left from a finished glyph fix, default 1.0. Zero consumers. |
| 32 | MASHED_FRAME_PROF | S/exe_main.cpp:3644 | DEV-KEEP | S/exe_main.cpp:3640 ("WS-A s3 frame profiler"); FI:112 | Wall-clock profiler switch. No script sets it. |
| 33 | MASHED_FX_BODYPOSE | S/D3d9Render/TrackRenderer.cpp:10307 | DELETE-CANDIDATE | S/D3d9Render/TrackRenderer.cpp:10303-10309 (`=0` keeps the old instance matrix; default ON); verify/rfx2_20261016/RESULT_rfx2.md | Opt-out of the finished body-pose replay. |
| 34 | MASHED_FX_DEBUG | S/D3d9Render/TrackRenderer.cpp:9178 | DEV-KEEP | S/D3d9Render/TrackRenderer.cpp:9173 ("[diag]"); verify/fxrebase_20261027/run_arms.py:438 (stack `X0`) | Per-slot FX counters. |
| 35 | MASHED_FX_RAWSPEED | S/D3d9Render/TrackRenderer.cpp:9106 | DELETE-CANDIDATE | S/D3d9Render/TrackRenderer.cpp:9103-9106 ("=1 keeps the old inputs for A/B"); verify/fe_residuals_20261016/fx/RESULT_FX.md:18 (the "before" row) | Selects the superseded raw-speed inputs. |
| 36 | MASHED_GAME_LENGTH | S/exe_main.cpp:2152 | DEV-KEEP | S/exe_main.cpp:2142 ("MASHED_GAME_LENGTH overrides"); FI:114; re/analysis/D3_MODES_2026-09-14.md | Dev override of nav ea88. No script sets it. |
| 37 | MASHED_GOTO | S/exe_main.cpp:9985 | DEV-KEEP | re/parity/sweep.py:13,190; S/D3d9Render/DrawStreamDump.h:24 | Parity sweep drives the standalone to a screen with it. |
| 38 | MASHED_HIDE_SHADOW | mashedmod/src/qol_asi/mashed_qol.cpp:1262 | DEV-KEEP | scripts/mashed_launch.ps1:112; mashedmod/src/qol_asi/mashed_qol.cpp:1256 ("Diagnostic") | Launcher-set QoL diagnostic. |
| 39 | MASHED_HOOK_ALLOW_BOOTPATCH | S/Core/HookSystem.cpp:110 | DEV-KEEP | S/Core/HookSystem.cpp:73 ("deliberate experiments only"); re/diag/KNOWN_ISSUES.md:168 | Disables the boot-patch guard on the .asi. |
| 40 | MASHED_HOOK_HI | S/Core/HookSystem.cpp:229 | DEV-KEEP | scripts/bisect_hook_index.ps1:7,13; scripts/bisect_hooks.py | Registry-index bisect upper bound. |
| 41 | MASHED_HOOK_LANE2 | S/Core/HookSystem.cpp:252 | DEV-KEEP | S/Core/HookSystem.cpp:244-253 ("A default .asi run must never carry a port nobody has [verified]"); re/tools/decomp2port.py:494; verify/d4wsbportbroad1_20261029/PREREG_D4-WSB-PORT-BROAD-1.md | Gate for unverified lane-2 ports on the dev .asi. |
| 42 | MASHED_HOOK_LO | S/Core/HookSystem.cpp:228 | DEV-KEEP | scripts/bisect_hook_index.ps1:46; scripts/bisect_hooks_set.py:4 | Registry-index bisect lower bound. |
| 43 | MASHED_HOOK_MANIFEST | S/Core/HookSystem.cpp:235 | DEV-KEEP | S/Core/HookSystem.cpp:187; verify/dupinstall2_20261016/install_observe.py:94 | Writes the index to RVA to name manifest (opt-in). |
| 44 | MASHED_HOOK_SKIP | S/Core/HookSystem.cpp:234 | DEV-KEEP | S/Core/HookSystem.cpp:186; verify/dupinstall2_20261016/tracker2.py:58 | Exact-token hook denylist. |
| 45 | MASHED_INTERP | mashedmod/src/qol_asi/mashed_qol.cpp:1495 | DEV-KEEP | scripts/mashed_launch.ps1:105 | Launcher-set .asi camera interpolation. |
| 46 | MASHED_INTERP_SHADOW | mashedmod/src/qol_asi/mashed_qol.cpp:699 | DEV-KEEP | scripts/mashed_launch.ps1:110 | Launcher-set .asi feature toggle. |
| 47 | MASHED_INTERP_WHEELS | mashedmod/src/qol_asi/mashed_qol.cpp:708 | DEV-KEEP | scripts/mashed_launch.ps1:111 | Launcher-set .asi feature toggle. |
| 48 | MASHED_INTRO_PRESS | S/exe_main.cpp:2923 | DEV-KEEP | S/exe_main.cpp:2906 ("Arm-test knobs"); verify/intromovies_20261028/run_intro.py:18 | Intro-movie arm test input. |
| 49 | MASHED_JUMPFIX | mashedmod/src/qol_asi/mashed_qol.cpp:1496 | DEV-KEEP | scripts/mashed_launch.ps1:108; mashedmod/src/qol_asi/mashed_qol.cpp:1362 | Launcher-set .asi ramp-takeoff fix. |
| 50 | MASHED_L1B_INPUTS | S/Race/SubStateCallees.cpp:471 | DEV-KEEP | S/D3d9Render/TrackRenderer.cpp:5249 ("measurement-only"); verify/d3_gatefire_20261008/run_h4.ps1:57 (arm `L1B_SUB`) | Per-frame input dump path. |
| 51 | MASHED_LAPS | S/exe_main.cpp:2168 | DEV-KEEP | S/exe_main.cpp:2162-2168; verify/d4dc77b0lap_20261030/RESULT_D4-DC-77b0-LAP.md:48 ("the exe_main.cpp:2165 dev override"); FI:122 | Dev lap-count override. |
| 52 | MASHED_LIBRW_AFTERWORLDORDER | S/D3d9Render/TrackRenderer.cpp:9378 | DELETE-CANDIDATE | S/D3d9Render/TrackRenderer.cpp:9375-9380 (`=0` keeps plain clump order; default ON); verify/rstates_20261016/RESULT_rstates.md | rstates fix, opt-out only. |
| 53 | MASHED_LIBRW_BINMESHORDER | S/Track/DffModel.cpp:436 | DELETE-CANDIDATE | S/Track/DffModel.cpp:431-439 (`=0` keeps material order; default ON); verify/rstates_20261016/RESULT_rstates.md | rstates F7, opt-out only. |
| 54 | MASHED_LIBRW_CLUMPORDER | S/LibRw/RwRaceSubmit.cpp:994 | DELETE-CANDIDATE | S/LibRw/RwRaceSubmit.cpp:984-994 (`Knob(name, true)`, "ON since rstates (2026-10-16)") | rstates fix, opt-out only. |
| 55 | MASHED_LIBRW_DUAL | S/LibRw/RwSceneBuild.cpp:52 | DELETE-CANDIDATE | S/LibRw/RwSceneBuild.cpp:48-56 (`=0` builds without the effect; default ON); verify/rtex_20261016/RESULT_rtex.md | rtex MatFX DUAL, opt-out only. |
| 56 | MASHED_LIBRW_LIFT | S/LibRw/RwRaceSubmit.cpp:1716 | DELETE-CANDIDATE | S/LibRw/RwRaceSubmit.cpp:1710-1719 ("D-S3-7 experiment"), applied at :1853; LS:875 ("a null experiment") | Finished experiment: cars rasterise in the default path. |
| 57 | MASHED_LIBRW_LINEAR | S/LibRw/RwRasterBridge.cpp:289 | DEV-KEEP | S/LibRw/RwRasterBridge.cpp:284 (`=1` LINEAR, `=2` NEAREST); LS:1807-1810 (non-degeneracy control) | Sampler A/B control. |
| 58 | MASHED_LIBRW_NODRAW | S/LibRw/RwRaceSubmit.cpp:1479 | DEV-KEEP | S/LibRw/RwRaceSubmit.cpp:1474; LS:682,1837 ("decisive test") | Runs the submit path without drawing, for the sizing and non-degeneracy checks. |
| 59 | MASHED_LIBRW_NOMIP | S/LibRw/RwRasterBridge.cpp:103 | DELETE-CANDIDATE | S/LibRw/RwRasterBridge.cpp:101 ("=1 reverts to a single level for A/B"); re/analysis/NEXT_SESSION_race_parity_20260830.md:41 | Selects the superseded single-level path. |
| 60 | MASHED_LIBRW_PAIRORDER | S/D3d9Render/TrackRenderer.cpp:10059 | DELETE-CANDIDATE | S/D3d9Render/TrackRenderer.cpp:10055-10061 (`=0` keeps record order; default ON); verify/rstates_20261016/RESULT_rstates.md | rstates fix, opt-out only. |
| 61 | MASHED_LIBRW_RAINCULL | S/LibRw/RwRaceSubmit.cpp:993 | DELETE-CANDIDATE | S/LibRw/RwRaceSubmit.cpp:984-993 (`Knob(name, true)`); verify/rstates_20261016/RESULT_rstates.md | rstates fix, opt-out only. |
| 62 | MASHED_LIBRW_SECTORWALK | S/LibRw/RwRaceSubmit.cpp:991 | DELETE-CANDIDATE | S/LibRw/RwRaceSubmit.cpp:984-991 (`Knob(name, true)`); verify/rstates_20261016/RESULT_rstates.md | rstates fix, opt-out only. |
| 63 | MASHED_LIBRW_SMOKE | S/LibRw/RwBridge.cpp:89 | DEV-KEEP | S/LibRw/RwBridge.cpp:84-88 ("the only end-to-end check of engine bring-up in isolation"); S/exe_main.cpp:9452-9462 (reachable, "default OFF"); LS:631 | Engine bring-up probe. No script sets it. |

## Notes for the second half and the delete step

- Deleting a DELETE-CANDIDATE row also removes arms that name it: rows 6, 8 (`verify/a5flip_20261026/run_arms.py:43-58` and its copies), 17 (`verify/carlod_20261016/run_all.py:43`). Those arm scripts are historical and read-only evidence, so the delete step must leave their copies alone.
- The six FAITH-seeded rows (1, 2, 3, 6, 8, 21) are deleted by removing the `kFaith` entry and the read site together, in one change, with the default gates unchanged.
- Raw data: none committed. The working files (`rows.txt`, `hits_*.txt`, `agg.txt`) lived in the session scratchpad and were not retained.
