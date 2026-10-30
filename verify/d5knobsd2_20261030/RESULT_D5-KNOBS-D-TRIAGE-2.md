# RESULT D5-KNOBS-D-TRIAGE-2 (2026-10-30)

Text-only triage of class (d) rows 64 to 126 of `verify/d5knobsinv_20261030/RESULT_D5-KNOBS-INVENTORY.md` (table order, lines 544-606 of that file). Second half of D5-KNOBS-D-TRIAGE-1 (`verify/d5knobsd1_20261030/RESULT_D5-KNOBS-D-TRIAGE-1.md`), same method and verdict set. No source edit, no tracker edit, no build, no game run.

**Verdict: GREEN. 63 of 63 rows have one verdict and a citation.**

| verdict | count |
|---|---|
| DEV-KEEP | 36 |
| DELETE-CANDIDATE | 22 |
| PENDING | 4 |
| DEAD | 1 |
| total | 63 |

## Method

- Read site and 3 to 20 surrounding lines for each knob (`S` below = `mashedmod/src/mashed_re/`), plus a `grep -rn` over `mashedmod/src` for every other source site.
- One `git grep -w` pass over `scripts/`, `ROADMAP.md`, `re/` and `verify/` for the 63 names (4,044 hits; the two D5-KNOBS result folders excluded). Rows cite executable users first (scripts, `run_*`, arm stacks), then PREREG/RESULT files.
- Seeded-ON check: every name against `S/Core/FaithDefaults.cpp` `kFaith` (lines 28-70).
- Owner search for PENDING: `re/orchestrator/DECISIONS.md`, `ROADMAP.md`, `re/NEXT_SESSION.md` and the live ledger `log/orch/roadmap.json`. None of the 63 names appears in any of the four.
- Verdicts as in TRIAGE-1: DEV-KEEP = debug, measurement or harness input. DELETE-CANDIDATE = the default path is the finished port and the knob only selects the superseded path, or the knob is seeded or forced ON. PENDING = a default-OFF behaviour change with no owner row. DEAD = no reader or an unreachable reader.

## Findings that change the inventory

1. **Six rows are default ON, not OFF** (seeded by `S/Core/FaithDefaults.cpp`, lines 29-35, before any read): `POS_MODEL`, `ROUND_GRID_ORDER`, `PATHPROG_LATE`, `SPAWN_BASIS_RW`, `RING_DBUF`, `STEER_ACOS_CRT`. The inventory's `OFF (unset -> off)` default column is wrong for them. Opt-out is `MASHED_NO_<name>=1`. Their source comments still say "default OFF" or "opt-in".
2. **Five more rows are ON by default through `POS_MODEL`**: `POS_WHEELS`, `POS_TRANS`, `POS_ORDER`, `POS_CONTACTS` are ORed into `PosModelAll()` (`S/Vehicle/PosModelKnobs.h:31-36`), which is `MASHED_POS_MODEL` (seeded). `MASHED_RW_WHEELPOS` is ORed with `PosModelWheels()` (`S/Collision/WheelContactSolver.cpp:174-175`). None of the five can add anything by default.
3. **Two rows force ON something already ON**: `PONTOON_CARRY` and `PONTOON_ANIM` (`S/Track/WarzonePontoon.cpp:715-717`, `Env(...) || !NoX()`). The real opt-out is `MASHED_NO_PONTOON_*`.
4. **Four PENDING rows**: `MUSIC_NO_RACE_CDAUDIO`, `RFLAME_SORT`, `SAVE_FIRSTRUN_WRITERS`, `TERRAIN_NOLIGHT`. Each is a default-OFF behaviour variant. The first three have a GREEN RESULT that says "no flip" or "that is a decision row"; no decision row exists. `TERRAIN_NOLIGHT` has an open question in its own comment (S/D3d9Render/TrackRenderer.cpp:797-800).
5. **One DEAD row**: `WORLDVEL`. Its reader `AiPhysWorldVel` (S/D3d9Render/TrackRenderer.cpp:687) has no caller anywhere in `mashedmod/`.
6. **Name collisions**: `MODE7_PC` and `TGT3_PC` are x87 precision selectors, not the seeded `MODE7` / `TGT3`. `MASHED_SPEED` is read only in the QoL .asi (`mashedmod/src/qol_asi/mashed_qol.cpp:317`); the standalone reads `MASHED_SPEED_REFRESH`, a different name.
7. **Open uncertainty**: `THROTTLE_K` is read inside `UpdateCar` (S/D3d9Render/TrackRenderer.cpp:5176), called at `S/exe_main.cpp:3862`. This step did not check whether `UpdateCar` still drives the default player path after D2. Marked `[UNCERTAIN]` in its row.

## Table

Evidence paths are repo-relative. `S/` = `mashedmod/src/mashed_re/`. Rows are numbered 64 to 126, continuing TRIAGE-1 (1 to 63).

| # | knob | site | verdict | evidence | why |
|---|---|---|---|---|---|
| 64 | MASHED_LIBRW_STICKYVA | S/LibRw/RwRaceSubmit.cpp:992 | DELETE-CANDIDATE | S/LibRw/RwRaceSubmit.cpp:984-992 (`Knob(name, true)`); verify/rstates_20261016/RESULT_rstates.md:174 | rstates fix, default ON, `=0` opt-out only. |
| 65 | MASHED_LIBRW_SUPERG_VA | S/D3d9Render/TrackRenderer.cpp:3565 | DELETE-CANDIDATE | S/D3d9Render/TrackRenderer.cpp:3564-3567 (`=0` keeps the Arctic-only rule, "A/B knob"); verify/rres_20261016/RESULT_rres.md:50 | Opt-out of the finished SuperG vertex-alpha rule (FUN_00496d00 callers 0x00448951, 0x00450f35). |
| 66 | MASHED_MISSILE_FORCE | S/Powerup/PowerupMissile.cpp:39 | DEV-KEEP | S/Powerup/PowerupMissile.cpp:30-38 ("NON-DEGENERACY CONTROL (harness only; unset in every real run)") | Control that breaks the parity gate. Zero consumers in scripts/ re/ verify/. |
| 67 | MASHED_MODE7_PC | S/Ai/AiTargetClosest.cpp:420 | DEV-KEEP | verify/d3_gatefire_20261008/run_h4.ps1:21; verify/d3_mode7_m4_20261010/run_m4_replay.ps1:17 | x87 precision selector, default `_PC_24`. Not the seeded `MODE7` (a different name). |
| 68 | MASHED_MORTAR_FORCE | S/Powerup/PowerupMortar.cpp:28 | DEV-KEEP | S/Powerup/PowerupMortar.cpp:20-27 (same control form as row 66); re/tools/pu_replay/pu_replay.cpp:1008; re/analysis/D3_CONTACT_PORT_2026-09-28.md:517 | Non-degeneracy control for the mortar. |
| 69 | MASHED_MSG_IDS | S/exe_main.cpp:10066 | DEV-KEEP | S/exe_main.cpp:10060; re/analysis/race_hud_capture_20260902.md:2780; re/analysis/CHANGELOG.md:2379 | HUD message id injection for verification. No script sets it. |
| 70 | MASHED_MUSIC_NO_RACE_CDAUDIO | S/Audio/AudioEngine.cpp:437 | PENDING | S/Audio/AudioEngine.cpp:430-433 ("default OFF: no cdaudio.rws stream in Race / Results"); verify/wsjmusic_20261026/RESULT_D4-WSJ-MUSIC.md:3 (GREEN, re-scoped); verify/wsjmusic_20261026/run_port.py | Faithful behaviour (the original starts no race music voice, FUN_00466b50) held behind a default-OFF knob. No ledger, DECISIONS, ROADMAP or NEXT_SESSION row owns the flip. |
| 71 | MASHED_NEGCTL | S/Harness/NegCtl.cpp:340 | DEV-KEEP | re/frida/run_negctl.py:30; re/frida/run_diff.py:750; re/frida/early_window_leaf_diff.py:281 | Negative-control arm for the Frida diff harness. |
| 72 | MASHED_OBJBSP_ORDER | S/LibRw/RwRaceSubmit.cpp:452 | DELETE-CANDIDATE | S/LibRw/RwRaceSubmit.cpp:451 ("experiment knob for the sector order (default b2f)"); verify/rsky_20261016/RESULT_rsky.md:31 | `f` selects the experiment order, default is the finished one. |
| 73 | MASHED_OBJSHAD_PLANES | S/LibRw/RwRaceFx.cpp:1118 | DELETE-CANDIDATE | S/LibRw/RwRaceFx.cpp:1116-1121 (`=cur` tests against this frame's planes; default is the previous render's, FUN_00481a30) | Selects the superseded plane source. Zero consumers. |
| 74 | MASHED_P3_MEASURE | S/D3d9Render/TrackRenderer.cpp:5353 | DEV-KEEP | verify/d3_gatefire_20261008/run_h4.ps1:21; verify/d3_gatefire_20261008/PREREG_LEG2.md:114 | Holds phase 3 for N frames for a measurement arm. |
| 75 | MASHED_PATHPROG_LATE | S/Race/PathProgStep.cpp:257 | DELETE-CANDIDATE | S/Core/FaithDefaults.cpp:31 (seeded ON); verify/a5flip_20261026/run_arms.py:93 (and 83 more arm scripts) | Seeded ON, so the inventory's OFF default is wrong. |
| 76 | MASHED_PATHPROG_PC | S/Race/PathProgStep.cpp:173 | DEV-KEEP | S/Race/PathProgStep.cpp:171-173 ("24 \| 53 \| 64; default 24"); verify/d3_gatefire_20261008/run_h4.ps1:21 | x87 precision selector. |
| 77 | MASHED_PHYS_NOCONTACT | S/Vehicle/VehiclePhysicsRun.cpp:49 | DEV-KEEP | S/Vehicle/VehiclePhysicsRun.cpp:40-41 ("skip the wheel-contact solver ... to isolate the probe cost"); re/analysis/FLAG_INVENTORY_2026-08-15.md:142 | Profiler A/B arm. |
| 78 | MASHED_PHYS_PROF | S/Vehicle/VehiclePhysicsRun.cpp:48 | DEV-KEEP | S/Vehicle/VehiclePhysicsRun.cpp:36-38 ("Off unless MASHED_PHYS_PROF is set"); re/analysis/FLAG_INVENTORY_2026-08-15.md:143 | Physics step profiler. |
| 79 | MASHED_PICKUPS_PC | S/Gameplay/PickupTick.cpp:201 | DEV-KEEP | verify/d3_gatefire_20261008/run_h4.ps1:21; verify/d3_mode7_m2_20261010/run_m2a_replay.ps1:13 | x87 precision selector, default 24. |
| 80 | MASHED_PICKUPS_PREGO_NO_REFDIST | S/D3d9Render/TrackRenderer.cpp:286 | DEV-KEEP | S/D3d9Render/TrackRenderer.cpp:277 ("(diagnostic) drops the call"); verify/pregocubeflip_20261029/run_arms.py:432; verify/iconcube_20261028/run_arms.py:431 | Diagnostic arm input of the shipped pre-GO registry chain (default ON with MASHED_ICONCUBE, TrackRenderer.cpp:279-282). |
| 81 | MASHED_PLAYERS | S/exe_main.cpp:6221 | DEV-KEEP | S/exe_main.cpp:6213-6222 (player count, default 2); verify/fe_residuals_20261016/u9672/RESULT.md:43 | Dev override of the player count. |
| 82 | MASHED_PONTOON_ANIM | S/Track/WarzonePontoon.cpp:717 | DELETE-CANDIDATE | S/Track/WarzonePontoon.cpp:715-717 (`Env(...) \|\| !NoAnim()`: ON unless `MASHED_NO_PONTOON_ANIM`); verify/dflip_20261017/apply_code.py:35 | Forces ON a feature that is already ON. The real opt-out is the `MASHED_NO_` form. |
| 83 | MASHED_PONTOON_CARRY | S/Track/WarzonePontoon.cpp:715 | DELETE-CANDIDATE | S/Track/WarzonePontoon.cpp:712-715 (same form); verify/dflip_20261017/apply_code.py:33 | Same as row 82. |
| 84 | MASHED_POS_CONTACTS | S/Vehicle/PosModelKnobs.h:36 | DELETE-CANDIDATE | S/Vehicle/PosModelKnobs.h:36 (`PosModelAll() \|\| ...`) with POS_MODEL seeded ON (S/Core/FaithDefaults.cpp:29); verify/d2bsp_20261016/run_arms.py:29 | Sub-knob ORed into an umbrella that is ON by default, so it adds nothing. |
| 85 | MASHED_POS_MODEL | S/Vehicle/PosModelKnobs.h:31 | DELETE-CANDIDATE | S/Core/FaithDefaults.cpp:29 (seeded ON); verify/a5flip_20261026/run_arms.py:30 (and 96 more arm scripts) | Seeded ON, so the inventory's OFF default is wrong. Opt-out is `MASHED_NO_POS_MODEL`. |
| 86 | MASHED_POS_ORDER | S/Vehicle/PosModelKnobs.h:35 | DELETE-CANDIDATE | S/Vehicle/PosModelKnobs.h:35; S/D3d9Render/TrackRenderer.cpp:5300,5484 (readers); verify/d2engage_20261016/run_arms.py:25 | Redundant sub-knob, as row 84. |
| 87 | MASHED_POS_TRANS | S/Vehicle/PosModelKnobs.h:34 | DELETE-CANDIDATE | S/Vehicle/PosModelKnobs.h:34; S/D3d9Render/TrackRenderer.cpp:5313,5497,5805 (readers); verify/d2engage_20261016/run_arms.py:24 | Redundant sub-knob, as row 84. |
| 88 | MASHED_POS_WHEELS | S/Vehicle/PosModelKnobs.h:33 | DELETE-CANDIDATE | S/Vehicle/PosModelKnobs.h:33; verify/d2engage_20261016/run_arms.py:24 | Redundant sub-knob, as row 84. |
| 89 | MASHED_QHULL_OPT | S/Collision/b5b_qhull_selftest.cpp:27 | DEV-KEEP | S/Collision/b5b_qhull_selftest.cpp:1-11 (offline acceptance harness); mashedmod/deps/qhull-2002.1/build_b5b_selftest.bat:16-20 | Option string of the standalone qhull self-test exe. |
| 90 | MASHED_QHULL_PC | S/Collision/b5b_qhull_selftest.cpp:103 | DEV-KEEP | S/Collision/b5b_qhull_selftest.cpp:100-103 ("{24,53,64}; default 24"); mashedmod/deps/qhull-2002.1/build_b5b_selftest.bat:16 | Precision selector of the same self-test. |
| 91 | MASHED_R7_INPUT_PAD_PRESS | S/exe_main.cpp:2848 | DEV-KEEP | verify/padcoopflip_20261026/run_arms.py:441; verify/padcoop_20261025/run_arms.py:441 | Scripted pad press for the R7 exit arms. |
| 92 | MASHED_R7_INPUT_PRESS | S/exe_main.cpp:2847 | DEV-KEEP | verify/padcoopflip_20261026/run_arms.py:436; verify/r7flip_20261025/run_arms.py:436 | Scripted key press for the R7 exit arms. |
| 93 | MASHED_REGISTRY_PC | S/Race/WorldRegistry.cpp:131 | DEV-KEEP | S/Race/WorldRegistry.cpp:130 ("24 \| 53 \| 64; default 24"); verify/d3_gatefire_20261008/run_h4.ps1:21 | x87 precision selector. |
| 94 | MASHED_RENDER_PROF | S/D3d9Render/TrackRenderer.cpp:9442 | DEV-KEEP | S/D3d9Render/TrackRenderer.cpp:9439-9441 ("Off unless the env is set"); re/analysis/FLAG_INVENTORY_2026-08-15.md:151 | Render section profiler. |
| 95 | MASHED_RFLAME_SORT | S/Powerup/PowerupEffects.cpp:380 | PENDING | S/Powerup/PowerupEffects.cpp:373-378 ("default OFF ... as the original's tick does, CALL 0x0045ac40 @0x0045af41"); verify/d3rflame_20261024/RESULT_D3-CF-RFLAME.md:3,9 (GREEN, "No flip proposed here (that is a decision row)") | Faithful sort held behind a default-OFF knob. No row owns the flip. |
| 96 | MASHED_RING_DBUF | S/Vehicle/VehiclePhysicsRun.cpp:543 | DELETE-CANDIDATE | S/Core/FaithDefaults.cpp:33 (seeded ON); verify/a5flip_20261026/run_arms.py:114 (and 78 more arm scripts) | Seeded ON, so the inventory's OFF default is wrong. u9152 RESULT line 23 ("Not a default candidate") predates the FAITH seed. |
| 97 | MASHED_RING_DBUF_DISCARD | S/Vehicle/VehiclePhysicsRun.cpp:547 | DEV-KEEP | S/Vehicle/VehiclePhysicsRun.cpp:547-549 (`s && RingDbuf()`); verify/u9152_20261018/RESULT_u9152.md:25 ("never fires in these arms"); verify/a5flip_20261026/run_arms.py:115 | Probe input carried in 77 arm env stacks, default OFF, not seeded. |
| 98 | MASHED_ROUND_COLOURS | S/exe_main.cpp:9768 | DEV-KEEP | S/exe_main.cpp:9762-9768 ("deliberate poke the harness uses to prove the mapping"); verify/librwrest_checks_20261016/run_stand.py:28 | Per-slot colour injection for the livery check. |
| 99 | MASHED_ROUND_GRID_ORDER | S/Race/RoundGrid.cpp:239 | DELETE-CANDIDATE | S/Core/FaithDefaults.cpp:30 (seeded ON); verify/a4order_20261017/run_arms.py:28 (and 94 more arm scripts) | Seeded ON, so the inventory's OFF default is wrong. |
| 100 | MASHED_ROUND_SCORES | S/exe_main.cpp:4196 | DEV-KEEP | S/exe_main.cpp:4185-4196; re/analysis/D3_AUDIT_2026-09-14.md:200; re/analysis/CHANGELOG.md:2396 | HUD score injection for verification. No script sets it. |
| 101 | MASHED_RULE_ENGINE | S/D3d9Render/TrackRenderer.cpp:7749 | DELETE-CANDIDATE | S/D3d9Render/TrackRenderer.cpp:7743-7750 ("The original has no engine-off state"; `=0` turns it off, default ON); verify/d3close_20261021/RESULT_d3close.md:29 | Opt-out of a path the original always runs. Zero script users. |
| 102 | MASHED_RW_WHEELPOS | S/Collision/WheelContactSolver.cpp:174 | DELETE-CANDIDATE | S/Collision/WheelContactSolver.cpp:171-175 (`getenv \|\| PosModelWheels()`, and PosModelWheels() is ON through POS_MODEL); verify/d2posmodel_20261016/RESULT_d2posmodel.md:63 | Same arm as POS_WHEELS, already ON by default. |
| 103 | MASHED_SAVE_FIRSTRUN_WRITERS | S/Race/GameFlow.cpp:262 | PENDING | S/Race/GameFlow.cpp:246-248 ("default OFF, =1 on"); verify/spanwriters_20261027/RESULT_D4-FU-SPANINIT-WRITERS.md:6 ("GREEN (knobs OFF by default, no flip)") | Two original writers modelled behind a default-OFF knob. No row owns the flip. |
| 104 | MASHED_SAVE_PATH | S/Race/GameFlow.cpp:708 | DEV-KEEP | S/Race/GameFlow.cpp:691; S/Save/GameSave.cpp:109-118; verify/spanwriters_20261027/run_spw.py:60 | Scratch save path for the round-trip harness. |
| 105 | MASHED_SAVE_RT_IN | S/Race/GameFlow.cpp:725 | DEV-KEEP | verify/settrackpu_20261026/run_stp.py:51; verify/spanwriters_20261027/run_spw.py:59 | Round-trip harness input. |
| 106 | MASHED_SAVE_RT_OUT | S/Race/GameFlow.cpp:707 | DEV-KEEP | verify/settrackpu_20261026/run_stp.py:51; verify/wsf24efc_20261026/run_24efc.py:55 | Round-trip harness output. |
| 107 | MASHED_SHADOW_NEED_PAGES | S/Core/ShadowTrack.h:768 | DEV-KEEP | S/Core/ShadowTrack.h:765-767 ("[D4-WSB-SHADOW-PRIVATE]"); verify/wsbshadowpriv_20261027/apply_reclass.py:35 | Shadow-compare sampling filter, dev .asi only. |
| 108 | MASHED_SHADOW_PRIV_HI | S/Core/ShadowTrack.h:458 | DEV-KEEP | S/Core/ShadowTrack.h:453-458 ("Diagnostic bisection 2026-09-10") | Upper bound of the private-memory tracking window. Zero consumers. |
| 109 | MASHED_SHADOW_PRIV_LO | S/Core/ShadowTrack.h:457 | DEV-KEEP | S/Core/ShadowTrack.h:453-457; re/analysis/lane3_write_tracking_20260910.md:100 | Lower bound of the same window. |
| 110 | MASHED_SHADOW_STW | S/Core/ShadowTrack.h:629 | DEV-KEEP | S/Core/ShadowTrack.h:626-629; verify/d4wsbportbroad1_20261029/run_shadow8.py:2 (and 14 more) | Stop-the-world switch of the shadow comparer. |
| 111 | MASHED_SKY_D3D9 | S/D3d9Render/TrackRenderer.cpp:9904 | DELETE-CANDIDATE | S/D3d9Render/TrackRenderer.cpp:9902-9904 ("keeps the sky on the legacy D3D9 draw ... old route"); verify/rprops_20261016/RESULT_rprops.md:136 | Selects the superseded sky route. |
| 112 | MASHED_SPAWN_BASIS_RW | S/Vehicle/BodyOrientationIntegrate.cpp:136 | DELETE-CANDIDATE | S/Core/FaithDefaults.cpp:32 (seeded ON); verify/a5flip_20261026/run_arms.py:96 (and 83 more arm scripts) | Seeded ON, so the inventory's OFF default is wrong. |
| 113 | MASHED_SPEED | mashedmod/src/qol_asi/mashed_qol.cpp:317 | DEV-KEEP | mashedmod/src/qol_asi/mashed_qol.cpp:317-318 (`EnvQ16("MASHED_SPEED", 0.05, 20.0)`); re/frida/speed_probe.py:6 | Sim-speed multiplier of the QoL .asi. Not read by the standalone (VehiclePhysicsRun.cpp:1739 reads `MASHED_SPEED_REFRESH`, a different name). |
| 114 | MASHED_STEER_ACOS_CRT | S/Ai/AiStandalone.cpp:274 | DELETE-CANDIDATE | S/Core/FaithDefaults.cpp:35 (seeded ON); verify/a5flip_20261026/run_arms.py:182 (and 63 more arm scripts) | Seeded ON, so the inventory's OFF default is wrong. |
| 115 | MASHED_STEER_AXIS_TERRAIN | S/Vehicle/VehiclePhysicsRun.cpp:705 | DELETE-CANDIDATE | S/Vehicle/VehiclePhysicsRun.cpp:704 ("=1 reverts to the averaged terrain normal for A/B only"); re/analysis/CHANGELOG.md:2133 | Selects the superseded averaged-normal path. |
| 116 | MASHED_SUBSTEP_OUTER_ORDER | S/Vehicle/VehiclePhysicsRun.cpp:2648 | DEV-KEEP | S/Vehicle/VehiclePhysicsRun.cpp:2640-2641 ("diagnostic mechanism controls"); verify/a5flip_20261026/run_arms.py:406 (42 arm scripts) | Order controls of the substep loop. Default `o` is the order the soflip flip set (RESULT_soflip, 2026-10-21). |
| 117 | MASHED_SUPERG_REFLECT_FIRST | S/D3d9Render/TrackRenderer.cpp:2937 | DELETE-CANDIDATE | S/D3d9Render/TrackRenderer.cpp:2937-2941 (`=0` keeps the after-world draw, default ON); verify/rres_20261016/RESULT_rres.md:50 | Opt-out of the finished draw order. |
| 118 | MASHED_TEAMS | S/exe_main.cpp:9824 | DEV-KEEP | S/exe_main.cpp:9818-9824; re/analysis/race_hud_capture_20260902.md:2666 | Team assignment injection for HUD verification. No script sets it. |
| 119 | MASHED_TEAM_KEYS | S/exe_main.cpp:6322 | DEV-KEEP | S/exe_main.cpp:6286-6291; re/analysis/race_hud_capture_20260902.md:2137 | Team ability key taps (display-only). No script sets it. |
| 120 | MASHED_TEAM_PLAY | S/exe_main.cpp:9955 | DEV-KEEP | S/exe_main.cpp:9955-9956 (writes `0x0067ea64`); verify/d3_gatefire_20261008/RESULT_LEG0.md:96 | Team Play switch for the original-vs-port arms. |
| 121 | MASHED_TERRAIN_NOLIGHT | S/D3d9Render/TrackRenderer.cpp:801 | PENDING | S/D3d9Render/TrackRenderer.cpp:797-800 ("the Arctic sea ... does darken and is not yet confirmed against an original"); re/analysis/CAR_BRIGHTNESS_2026-09-30.md:532 | Value `4` skips the ambient fold. Default-OFF visual variant with an open question and no owner row. Zero script users. |
| 122 | MASHED_TEXANIM_MASKTEX | S/LibRw/RwRaceSubmit.cpp:558 | DEV-KEEP | S/LibRw/RwRaceSubmit.cpp:556-557 ("dev-only mask arm"); verify/hbtexanimrefs_20261027/run_render.py:5 | Mask arm for the texanim scoring. |
| 123 | MASHED_TGT3_PC | S/Ai/AiTargetClosest.cpp:245 | DEV-KEEP | S/Ai/AiTargetClosest.cpp:244 ("24 \| 53 \| 64; default 24"); verify/d3_gatefire_20261008/run_h4.ps1:21 | x87 precision selector. Not the seeded `TGT3`. |
| 124 | MASHED_THROTTLE_K | S/D3d9Render/TrackRenderer.cpp:5220 | DELETE-CANDIDATE | S/D3d9Render/TrackRenderer.cpp:5214-5218 ("=0.42 reverts", calibrated to 0.58 on 2026-06-30); re/analysis/feel_calibration.md:36; reader inside `UpdateCar` (TrackRenderer.cpp:5176), called at exe_main.cpp:3862 | Tuning aid left from a finished calibration. [UNCERTAIN] whether `UpdateCar` still drives the default player path after D2: this step did not check it. |
| 125 | MASHED_U9186_GATES | S/D3d9Render/TrackRenderer.cpp:6694 | DEV-KEEP | S/D3d9Render/TrackRenderer.cpp:6692-6697 (dump path); verify/a5flip_20261026/run_arms.py:448 (and 138 more arm scripts) | Gate dump path carried in arm env stacks. |
| 126 | MASHED_WORLDVEL | S/D3d9Render/TrackRenderer.cpp:689 | DEAD | S/D3d9Render/TrackRenderer.cpp:687 (`AiPhysWorldVel`); a grep of mashedmod/src for `AiPhysWorldVel` finds only this definition | The reader is a function with no caller. |

## Notes for the delete step

- The 22 DELETE-CANDIDATE rows split four ways. Seeded (6: `POS_MODEL`, `ROUND_GRID_ORDER`, `PATHPROG_LATE`, `SPAWN_BASIS_RW`, `RING_DBUF`, `STEER_ACOS_CRT`): delete the `kFaith` entry and the read site in one change, with the default gates unchanged (as TRIAGE-1 note 2). Redundant under `POS_MODEL` (5: `POS_WHEELS`, `POS_TRANS`, `POS_ORDER`, `POS_CONTACTS`, `RW_WHEELPOS`): delete with `POS_MODEL`. Forced-ON of a default-ON feature (2: `PONTOON_CARRY`, `PONTOON_ANIM`). Opt-outs or superseded paths (9: `STICKYVA`, `SUPERG_VA`, `OBJBSP_ORDER`, `OBJSHAD_PLANES`, `RULE_ENGINE`, `SKY_D3D9`, `STEER_AXIS_TERRAIN`, `SUPERG_REFLECT_FIRST`, `THROTTLE_K`).
- Arm scripts under `verify/` that name these knobs (for example `verify/a5flip_20261026/run_arms.py:93-182`, 84 copies) are historical, read-only evidence. The delete step leaves them alone.
- The 4 PENDING rows need a decision row each (flip, keep as dev, or delete). This step files none.
- Raw data: none committed. Working files (`names.txt`, `gg.txt`, `ctx.txt`, `rows2.txt`) lived in the session scratchpad and were not retained.
