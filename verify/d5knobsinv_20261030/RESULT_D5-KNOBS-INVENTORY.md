# RESULT D5-KNOBS-INVENTORY (2026-10-30)

Static text-only inventory of every `MASHED_*` token under `mashedmod/src`. No source change, no tracker edit, no build, no run.

## Method and totals

- Grep: `grep -rn "MASHED_" mashedmod/src` = 2315 matching lines in 236 files; `grep -rhoE "MASHED_[A-Z0-9_]+" mashedmod/src | sort -u` = **659 distinct tokens**.
- Scanner `D:/MashedWT/D5-KNOBS-INVENTORY_data/scan.py` (724 source files): a token inside a string literal on a non-comment part of a line is a READ site (574 tokens, any reader: `getenv`, `GetEnvironmentVariableA`, `EnvOn/EnvOff/EnvSet/Env` wrappers). The other **85 tokens** appear only in comments, `#define`/include-guard names or fragments and are listed as class x (not env reads).
- Table coverage: 574 read rows + 85 x rows = **659 = every token**.
- Default column is derived from the read line: `EnvOff`/`MASHED_NO_*`/`*_DISABLE` = ON, `== nullptr` = ON, parsed value = value, else OFF.
- Class is **heuristic**: (b) = opt-out name/helper, or a comment within 6 lines of the read says default ON / opt-out; (a) = name matches a dev/debug/harness pattern; (c) = default OFF with an owner found in `re/orchestrator/DECISIONS.md` (L-n), live ledger `log/orch/roadmap.json` row id, `ROADMAP.md` or `re/NEXT_SESSION.md` (line cited), or a source comment saying default OFF; (d) = none of these. Treat (b) as candidates to verify before deleting: the D5-KNOBS batch must confirm the positive path is the default on the exe and gates stay unchanged.
- Last column "evidence hint" = first `verify/**/PREREG*|RESULT*.md` naming the knob (a pointer, not an owner).

## Counts

| class | count |
|---|---|
| b opt-out of finished port (delete candidates) | 114 |
| c default-OFF / pending flip | 97 |
| a dev/debug keep | 237 |
| d unknown | 126 |
| x no env read (comment/define/dead reference) | 85 |
| total | 659 |

**Class (d) count: 126.**

Caveats: (1) a knob read from several files is cited at its first read site, with the number of sites in the `n` column. (2) class (a) was assigned by name pattern and was not read site by site. (3) a read through a name built at run time (prefix + suffix, e.g. `MASHED_PONTOON_`) is not visible to a literal scan.

## Table (class b first)

| knob | read site | n | default | class | basis | owner (L-n / ledger / doc line) | evidence hint |
|---|---|---|---|---|---|---|---|
| MASHED_A5_RUBBER_LIVE | mashedmod/src/mashed_re/Vehicle/ForceIntegratorLeaves.cpp:39 | 1 | OFF (unset -> off) | b | comment near read says default ON / opt-out |  | verify/rbflip_20261021/PREREG_rbflip.md |
| MASHED_A6A_LC0 | mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:227 | 1 | OFF (unset -> off) | b | comment near read says default ON / opt-out |  | verify/u9764_20261019/PREREG_u9764.md |
| MASHED_AI_LUT | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:153 | 1 | value (parsed) | b | comment near read says default ON / opt-out | NEXT_SESSION.md:521 | verify/c2k93_20261017/PREREG_c2k93.md |
| MASHED_AI_VEHICLES | mashedmod/src/mashed_re/Race/RaceSession.cpp:70 | 1 | OFF (unset -> off) | b | comment near read says default ON / opt-out | L-30(D3-AI-VEHICLES) ledger:D4-FLIP-L29-L30 | verify/aiveh_20261026/PREREG_D3-AI-VEHICLES.md |
| MASHED_AUDIO_WIRE | mashedmod/src/mashed_re/Audio/AudioWire.cpp:244 | 1 | value (parsed) | b | comment near read says default ON / opt-out | L-27(D4-WSJ-WIRE) ledger:D3-AI-VEHICLES | verify/wsjwire_20261026/PREREG_D4-WSJ-WIRE.md |
| MASHED_CAR_BEAM | mashedmod/src/mashed_re/LibRw/RwRaceFx.cpp:191 | 1 | ON (EnvOff helper) | b | opt-out of default-ON path |  | verify/rfx_20261016/RESULT_rfx.md |
| MASHED_CAR_DIRT | mashedmod/src/mashed_re/LibRw/RwCarFx.cpp:61 | 1 | ON (EnvOff helper) | b | opt-out of default-ON path |  | verify/rtex_20261016/PREREG_rtex.md |
| MASHED_CAR_SHADOW | mashedmod/src/mashed_re/LibRw/RwRaceFx.cpp:190 | 1 | ON (EnvOff helper) | b | opt-out of default-ON path |  | verify/rfx_20261016/RESULT_rfx.md |
| MASHED_CAR_SHADOW_RT | mashedmod/src/mashed_re/LibRw/RwRaceFx.cpp:2103 | 1 | ON (EnvOff helper) | b | opt-out of default-ON path |  | verify/rres_20261016/PREREG_rres.md |
| MASHED_CAR_VIS | mashedmod/src/mashed_re/LibRw/RwCarFx.cpp:66 | 1 | ON (EnvOff helper) | b | opt-out of default-ON path |  | verify/carlod2_20261016/PREREG_carlod2.md |
| MASHED_COLL_LUT | mashedmod/src/mashed_re/Collision/ContactDeps.h:45 | 1 | OFF (unset -> off) | b | comment near read says default ON / opt-out | L-18(D3-CF-COLLLUT) L-26(D3-CF-COLLLUT) | verify/a5flip_20261026/RESULT_D3-CF-A5TIMER-FLIP.md |
| MASHED_CONFIG_EDIT | mashedmod/src/mashed_re/exe_main.cpp:2684 | 1 | OFF (unset -> off) | b | comment near read says default ON / opt-out |  |  |
| MASHED_CUP_STANDINGS | mashedmod/src/mashed_re/exe_main.cpp:2247 | 1 | OFF (unset -> off) | b | comment near read says default ON / opt-out |  |  |
| MASHED_CUP_TIERS | mashedmod/src/mashed_re/exe_main.cpp:2386 | 1 | OFF (unset -> off) | b | comment near read says default ON / opt-out |  |  |
| MASHED_GAMEMODE_STUB | mashedmod/src/mashed_re/Vehicle/ForceIntegratorStubs.cpp:130 | 1 | OFF (unset -> off) | b | comment near read says default ON / opt-out | ROADMAP.md:1925 |  |
| MASHED_HEADING_ASSIST | mashedmod/src/mashed_re/Race/HeadingAssist.cpp:224 | 1 | value (parsed) | b | comment near read says default ON / opt-out |  | verify/d3b_20261016/PREREG_d3b.md |
| MASHED_HEADLIGHT | mashedmod/src/mashed_re/LibRw/RwRaceFx.cpp:1066 | 1 | ON (EnvOff helper) | b | opt-out of default-ON path |  | verify/rfx2_20261016/RESULT_rfx2.md |
| MASHED_ICONCUBE | mashedmod/src/mashed_re/D3d9Render/PickupIconCube.cpp:65 | 1 | OFF (unset -> off) | b | comment near read says default ON / opt-out | L-52(D5-ICONCUBE-SPIN) ledger:D5-PREGO-CUBE-FLIP | verify/d4wsbsolvkv_20261029/RESULT_D4-WSB-PORT-SOLV-KV.md |
| MASHED_LENSFLARE | mashedmod/src/mashed_re/LibRw/RwRaceFx.cpp:992 | 1 | ON (EnvOff helper) | b | opt-out of default-ON path |  | verify/rfx2_20261016/RESULT_rfx2.md |
| MASHED_LIBRW_FOGFIX | mashedmod/src/mashed_re/LibRw/RwRaceSubmit.cpp:1503 | 1 | OFF (unset -> off) | b | comment near read says default ON / opt-out |  |  |
| MASHED_MP_RULES | mashedmod/src/mashed_re/exe_main.cpp:2146 | 1 | OFF (unset -> off) | b | comment near read says default ON / opt-out |  |  |
| MASHED_NO_ | mashedmod/src/mashed_re/Core/FaithDefaults.cpp:87 | 2 | ON (opt-out name) | b | opt-out of default-ON path | L-14(D4-DC-MATH-4680) L-23(D3-CF-A5TIMER-FLIP) L-26(D3-CF-COLLLUT) | verify/a6flip_20261019/RESULT_a6flip.md |
| MASHED_NO_A364_RESET | mashedmod/src/mashed_re/Race/AiDifficultyReset.cpp:89 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/d3_b2flip_20261009/PREREG_B2FLIP.md |
| MASHED_NO_A3_LUT | mashedmod/src/mashed_re/Vehicle/VehicleInit.cpp:41 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/camribbon_20261016/RESULT_camribbon.md |
| MASHED_NO_A4_CONTACT_GATE | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1681 | 1 | ON (opt-out name) | b | opt-out of default-ON path | NEXT_SESSION.md:579 | verify/d3_car2pickup_20261013/RESULT_CAR2PICKUP.md |
| MASHED_NO_A5_RACE_TIMER | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1181 | 1 | ON (opt-out name) | b | opt-out of default-ON path | L-23(D3-CF-A5TIMER-FLIP) | verify/d2ref_20261017/RESULT_d2ref.md |
| MASHED_NO_A5_RACE_TIMER_RUN | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:2237 | 1 | ON (opt-out name) | b | opt-out of default-ON path | L-23(D3-CF-A5TIMER-FLIP) | verify/a5flip_20261026/RESULT_D3-CF-A5TIMER-FLIP.md |
| MASHED_NO_A5_SUSP_LUT | mashedmod/src/mashed_re/Vehicle/ForceIntegrator.cpp:118 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/a6flip_20261019/PREREG_a6flip.md |
| MASHED_NO_A6A_F32 | mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:204 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/a6flip_20261019/PREREG_a6flip.md |
| MASHED_NO_A6A_LUT | mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:195 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/a6flip_20261019/PREREG_a6flip.md |
| MASHED_NO_AI_CLOCK_LATE | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:2164 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/c2k93_20261017/RESULT_c2k93.md |
| MASHED_NO_AI_COUNTDOWN_CLOCK | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:2208 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/a4order_20261017/PREREG_a4order.md |
| MASHED_NO_AI_LUT | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:155 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/c2k93_20261017/RESULT_c2k93.md |
| MASHED_NO_ANIM_TRUNC | mashedmod/src/mashed_re/Frontend/MenuAnimTickTwin.cpp:42 | 1 | ON (opt-out name) | b | opt-out of default-ON path | L-32(D4-WSH-EXEFILE) | verify/menuspeed_20261027/PREREG_D4-FU-MENUSPEED.md |
| MASHED_NO_ARCTIC_SKY_RTT | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:9890 | 2 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/hbarcticskyflip_20261027/RESULT_HB-ARCTIC-SKY-FLIP.md |
| MASHED_NO_AUDIO_WIRE | mashedmod/src/mashed_re/Audio/AudioWire.cpp:242 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/wsjwire_20261026/RESULT_D4-WSJ-WIRE.md |
| MASHED_NO_BOX_PRODUCERS | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:211 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/d3cfboxflip_20261025/RESULT_D3-CF-BOX-FLIP.md |
| MASHED_NO_CAM_RIBBON_FIX | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:3264 | 2 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/camribbon_20261016/PREREG_camribbon.md |
| MASHED_NO_CC_MATRIX_INVERT | mashedmod/src/mashed_re/Collision/ContactStubs.cpp:173 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/ccinv_20261019/PREREG_ccinv.md |
| MASHED_NO_CLUMPANIM | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:3210 | 2 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/rhooks_20261016/RESULT_rhooks.md |
| MASHED_NO_COLL_LUT | mashedmod/src/mashed_re/Collision/ContactDeps.h:43 | 1 | ON (opt-out name) | b | opt-out of default-ON path | L-26(D3-CF-COLLLUT) | verify/colllut_20261026/RESULT_D3-CF-COLLLUT.md |
| MASHED_NO_COPTERS | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:10555 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/d3_gatefire_20261008/PREREG_H2.md |
| MASHED_NO_COUNTDOWN_SPAWN | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:5464 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/u9747_20261018/RESULT_u9747.md |
| MASHED_NO_D2CHUNK | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1135 | 1 | ON (opt-out name) | b | opt-out of default-ON path | ROADMAP.md:621 NEXT_SESSION.md:529 | verify/d2bsp_20261016/PREREG_d2bsp.md |
| MASHED_NO_D2CLS | mashedmod/src/mashed_re/Collision/CarWorldContacts.cpp:132 | 1 | ON (opt-out name) | b | opt-out of default-ON path | ROADMAP.md:621 NEXT_SESSION.md:529 | verify/d2close_20261016/RESULT_d2close.md |
| MASHED_NO_D2LAT_REFRESH | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1692 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/d2ref_20261017/RESULT_d2ref.md |
| MASHED_NO_DEAD_COAST | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:946 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/fxflip_20261027/RESULT_D4-FX-FLIP-DEADCOAST.md |
| MASHED_NO_ELIM | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:137 | 1 | ON (opt-out name) | b | opt-out of default-ON path | ROADMAP.md:2505 NEXT_SESSION.md:954 | verify/d3_b2flip_20261009/PREREG_B2FLIP.md |
| MASHED_NO_ELIM_RESOLVE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:8098 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/u9779_20261024/PREREG_D3-CF-U9779.md |
| MASHED_NO_FAITH | mashedmod/src/mashed_re/Core/FaithDefaults.cpp:83 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/cpcflip_20261020/RESULT_cpcflip.md |
| MASHED_NO_FIX_5CEA60 | mashedmod/src/mashed_re/Collision/WheelContactSolver.cpp:61 | 2 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/faithflip_20261020/PREREG_faithflip.md |
| MASHED_NO_FOG | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:9975 | 4 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/d1_fxcut/RESULT.md |
| MASHED_NO_FWD_AT | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:533 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/faithflip_20261020/PREREG_faithflip.md |
| MASHED_NO_FX_ORIG_GATES | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:9140 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/fxgates_flip_20261026/RESULT_D4-FX-GATES-FLIP.md |
| MASHED_NO_GRID_DIR | mashedmod/src/mashed_re/Race/RoundGrid.cpp:142 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/d2ref_20261017/RESULT_d2ref.md |
| MASHED_NO_HEADING_ASSIST | mashedmod/src/mashed_re/Race/HeadingAssist.cpp:222 | 1 | ON (opt-out name) | b | opt-out of default-ON path | NEXT_SESSION.md:517 | verify/dflip_20261017/PREREG_dflip.md |
| MASHED_NO_HEADING_ASSIST_PLAYER | mashedmod/src/mashed_re/Race/HeadingAssist.cpp:228 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/u9749_20261017/RESULT_u9749.md |
| MASHED_NO_HOOKSLOTS | mashedmod/src/mashed_re/Render/TrackHookSlots.cpp:138 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/rhooks_20261016/PREREG_rhooks.md |
| MASHED_NO_ICONCUBE | mashedmod/src/mashed_re/D3d9Render/PickupIconCube.cpp:64 | 1 | ON (opt-out name) | b | opt-out of default-ON path | L-56(D5-PREGO-CUBE-FLIP) | verify/d3subflip_20261029/RESULT_D3-SUBSTATE-FLIP.md |
| MASHED_NO_KILL_DAMAGE | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:952 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/fxflip_20261027/RESULT_D4-FX-FLIP-DEADCOAST.md |
| MASHED_NO_LAUNCH_REV | mashedmod/src/mashed_re/Vehicle/LaunchRevCharge.cpp:260 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/d2_writer_20261001/PREREG_STEP3.md |
| MASHED_NO_LEAVES1_X87 | mashedmod/src/mashed_re/Collision/RwpSolverLeaves1.cpp:126 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/wsbnarrowflip_20261027/RESULT_D4-WSB-NARROW-FLIP.md |
| MASHED_NO_MENU_CLOCK_FAITHFUL | mashedmod/src/mashed_re/exe_main.cpp:5321 | 1 | ON (opt-out name) | b | opt-out of default-ON path | L-36(D4-FU-MENUSPEED) ledger:D4-FU-MENUCLOCK-FLIP | verify/menuanimspeed_20261027/RESULT_D5-MENUANIMSPEED.md |
| MASHED_NO_NARROW_DOT_ORDER | mashedmod/src/mashed_re/Collision/NarrowFixKnobs.h:17 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/wsbnarrowflip_20261027/RESULT_D4-WSB-NARROW-FLIP.md |
| MASHED_NO_NARROW_PAIR_INT | mashedmod/src/mashed_re/Collision/NarrowFixKnobs.h:21 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/wsbnarrowflip_20261027/RESULT_D4-WSB-NARROW-FLIP.md |
| MASHED_NO_OBJBSP | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:2839 | 2 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/rsky_20261016/PREREG_rsky.md |
| MASHED_NO_OWN_RAND_564310 | mashedmod/src/mashed_re/Collision/RwpSolverLeaves1.cpp:78 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/d4wsb_randflip_20261028/RESULT_D4-WSB-RAND-FLIP.md |
| MASHED_NO_PARTICLES | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:10614 | 1 | ON (opt-out name) | b | opt-out of default-ON path | ROADMAP.md:3130 | verify/allmode/RESULT.md |
| MASHED_NO_PICKUPS | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:10642 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/d1_fxbloom/RESULT.md |
| MASHED_NO_PICKUPS_PREGO | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:281 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/d3subflip_20261029/RESULT_D3-SUBSTATE-FLIP.md |
| MASHED_NO_PICKUP_GLOW | mashedmod/src/mashed_re/Gameplay/PickupGlow.cpp:50 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/iconcube_20261028/PREREG_D5-ICONCUBE-GLOW.md |
| MASHED_NO_PONTOON_ANIM | mashedmod/src/mashed_re/Track/WarzonePontoon.cpp:711 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/dflip_20261017/PREREG_dflip.md |
| MASHED_NO_PONTOON_CARRY | mashedmod/src/mashed_re/Track/WarzonePontoon.cpp:713 | 1 | ON (opt-out name) | b | opt-out of default-ON path | NEXT_SESSION.md:518 | verify/dflip_20261017/PREREG_dflip.md |
| MASHED_NO_PONTOON_HIDE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:2640 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/pontoon_20261016/RESULT_pontoon.md |
| MASHED_NO_PONTOON_STATE | mashedmod/src/mashed_re/Track/WarzonePontoon.cpp:712 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/dflip_20261017/PREREG_dflip.md |
| MASHED_NO_PROPPLACE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:2304 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/rprops_20261016/PREREG_rprops.md |
| MASHED_NO_R7_EXIT | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:8203 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/r7flip_20261025/PREREG_D3-CF-R7FLIP.md |
| MASHED_NO_R7_INPUT | mashedmod/src/mashed_re/exe_main.cpp:1053 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/r7flip_20261025/PREREG_D3-CF-R7FLIP.md |
| MASHED_NO_R7_PHYS | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:1543 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/u9779_20261024/PREREG_D3-CF-U9779.md |
| MASHED_NO_RACEPCT_BRIDGE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:8770 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/d3_b2flip_20261009/PREREG_B2FLIP.md |
| MASHED_NO_RACEPCT_FUN408610 | mashedmod/src/mashed_re/Race/PathNodes.cpp:347 | 2 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/d3_pathprog_a_20261016/PREREG_pathprog_a.md |
| MASHED_NO_RACE_CAM_BASIS | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:5521 | 3 | ON (opt-out name) | b | opt-out of default-ON path | ledger:D1-R2 ROADMAP.md:309 | verify/d1r2_20261016/PREREG_d1r2.md |
| MASHED_NO_RACE_TAIL_LATE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:188 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/u9779_20261024/PREREG_D3-CF-U9779.md |
| MASHED_NO_REFDIST | mashedmod/src/mashed_re/Race/SpectatorDistances.cpp:370 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/d3_b2flip_20261009/PREREG_B2FLIP.md |
| MASHED_NO_REFDIST_AFTER_AI | mashedmod/src/mashed_re/Race/SpectatorDistances.cpp:333 | 2 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/d3_u9683_20261016/PREREG_u9683.md |
| MASHED_NO_ROUND_END7 | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:168 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/r7flip_20261025/RESULT_D3-CF-R7FLIP.md |
| MASHED_NO_RULE7_KEEP | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:176 | 1 | ON (opt-out name) | b | opt-out of default-ON path | NEXT_SESSION.md:284 | verify/r7keep_20261022/PREREG_r7keep.md |
| MASHED_NO_RW_SSE_POINTS | mashedmod/src/mashed_re/Math/RwV3dTransformPointsCPU.cpp:68 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/faithflip_20261020/PREREG_faithflip.md |
| MASHED_NO_SAVE | mashedmod/src/qol_asi/mashed_qol.cpp:1492 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  |  |
| MASHED_NO_SCENARIOVIS | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:2619 | 2 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/rhooks_20261016/RESULT_rhooks.md |
| MASHED_NO_SEA_TILE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:2909 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/sea_fix_20260930/PREREG.md |
| MASHED_NO_SECTOR_BINMESH | mashedmod/src/mashed_re/LibRw/RwSceneBuild.cpp:291 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/rsky_20261016/PREREG_rsky.md |
| MASHED_NO_SEEK414F00 | mashedmod/src/mashed_re/Ai/AiTrackObjSeek.cpp:156 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/seek414f00_20261016/PREREG_seek414f00.md |
| MASHED_NO_SKYFIX | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:2595 | 2 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/rprops_20261016/PREREG_rprops.md |
| MASHED_NO_SKYSLOTS | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:1230 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/rskyverify_20261016/RESULT_rskyverify.md |
| MASHED_NO_SLOTSTATE_SEED | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:596 | 1 | ON (opt-out name) | b | opt-out of default-ON path | NEXT_SESSION.md:1168 | verify/d3_gatefire_20261008/PREREG_H2.md |
| MASHED_NO_SLOT_PLAYER | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:638 | 1 | ON (opt-out name) | b | opt-out of default-ON path | NEXT_SESSION.md:1107 | verify/d3_b2flip_20261009/PREREG_B2FLIP.md |
| MASHED_NO_SPAWN_SETTLE | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1244 | 1 | ON (opt-out name) | b | opt-out of default-ON path | ROADMAP.md:1929 |  |
| MASHED_NO_SPAWN_Y_LINE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:7654 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/u9755_20261018/RESULT_u9755.md |
| MASHED_NO_START_BOOST | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1502 | 1 | ON (opt-out name) | b | opt-out of default-ON path | ROADMAP.md:1925 NEXT_SESSION.md:2591 | verify/d3_carcar_20261005/PREREG_CARCAR.md |
| MASHED_NO_SUBSTATE_SETTLE_FIRST | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:5296 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/d3subsettle_20261029/PREREG_D3-SUBSTATE-SETTLE-ORDER.md |
| MASHED_NO_TEXANIM | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:1602 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/hbtexanimrefs_20261027/RESULT_HB-TEXANIM-REFS.md |
| MASHED_NO_TXDSCOPE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:1827 | 4 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/txdscope_20261016/PREREG_txdscope.md |
| MASHED_NO_UVSCROLL | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:9454 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/d3_gatefire_20261008/PREREG_H2.md |
| MASHED_NO_WEATHER | mashedmod/src/mashed_re/exe_main.cpp:9725 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  |  |
| MASHED_NO_WIRE_B2 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:1140 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  | verify/d3_b2flip_20261009/PREREG_B2FLIP.md |
| MASHED_OBJ_SHADOW | mashedmod/src/mashed_re/LibRw/RwRaceFx.cpp:1072 | 1 | ON (EnvOff helper) | b | opt-out of default-ON path |  | verify/rfx2_20261016/RESULT_rfx2.md |
| MASHED_PARTMORPH | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:4421 | 1 | OFF (unset -> off) | b | comment near read says default ON / opt-out | L-62(D5-PARTMORPH-FLIPPREP) ledger:D5-PARTMORPH-PRODUCER | verify/d5deferredsweep_20261030/RESULT_D5-DEFERRED-SWEEP.md |
| MASHED_PICKUPS_PREGO | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:281 | 1 | OFF (unset -> off) | b | comment near read says default ON / opt-out | L-52(D5-ICONCUBE-SPIN) ledger:D5-PREGO-CUBE-FLIP | verify/d4wsbsolvkv_20261029/RESULT_D4-WSB-PORT-SOLV-KV.md |
| MASHED_QOL_DISABLE | mashedmod/src/dinput8_shim/dinput8_shim.cpp:42 | 1 | ON (opt-out name) | b | opt-out of default-ON path |  |  |
| MASHED_RING_SLOT0 | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:504 | 1 | OFF (unset -> off) | b | comment near read says default ON / opt-out | NEXT_SESSION.md:1868 | verify/d3_carcar_20261005/PREREG_CARCAR.md |
| MASHED_RPLIGHT | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:822 | 1 | OFF (unset -> off) | b | comment near read says default ON / opt-out | ROADMAP.md:347 | verify/d1r3_20261016/RESULT_d1r3.md |
| MASHED_WCS_COMBINE_PC24 | mashedmod/src/mashed_re/Collision/WheelContactSolver.cpp:443 | 1 | OFF (unset -> off) | b | comment near read says default ON / opt-out |  | verify/cpcflip_20261020/PREREG_cpcflip.md |
| MASHED_WCS_TRINORMAL | mashedmod/src/mashed_re/Collision/WheelContactSolver.cpp:89 | 2 | OFF (unset -> off) | b | comment near read says default ON / opt-out |  | verify/trinflip_20261020/PREREG_trinflip.md |
| MASHED_A4_DOT_ORDER | mashedmod/src/mashed_re/Vehicle/VehicleControl.cpp:171 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/dcphys2_20261022/PREREG_dcphys2.md |
| MASHED_A4_ORDER | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:939 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/a4order_20261017/PREREG_a4order.md |
| MASHED_A5_AIR_F32 | mashedmod/src/mashed_re/Vehicle/ForceIntegrator.cpp:132 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/dcphys2_20261022/PREREG_dcphys2.md |
| MASHED_A5_CONTACT_INT | mashedmod/src/mashed_re/Vehicle/ForceIntegrator.cpp:137 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/dcphys2_20261022/PREREG_dcphys2.md |
| MASHED_A5_DOT_ORDER | mashedmod/src/mashed_re/Vehicle/ForceIntegrator.cpp:143 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/dcphys2_20261022/PREREG_dcphys2.md |
| MASHED_A5_FD0_LIVE | mashedmod/src/mashed_re/Vehicle/ForceIntegrator.cpp:44 | 2 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/d2off_20261020/PREREG_d2off.md |
| MASHED_A5_RING_ORDER | mashedmod/src/mashed_re/Vehicle/ForceIntegrator.cpp:380 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/d2off_20261020/PREREG_d2off.md |
| MASHED_A5_SPIN_FILD | mashedmod/src/mashed_re/Vehicle/ForceIntegrator.cpp:142 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/dcphys2_20261022/PREREG_dcphys2.md |
| MASHED_A6A_DOT_ORDER | mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:240 | 1 | OFF (unset -> off) | c | owner found | ROADMAP.md:2978 NEXT_SESSION.md:502 | verify/dcphys_20261021/PREREG_dcphys.md |
| MASHED_A6A_DRIVE_NORM_LUT | mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:212 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/dcphys_20261021/PREREG_dcphys.md |
| MASHED_A6A_GEAR_ORDER | mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:217 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/u9771_20261019/PREREG_u9771.md |
| MASHED_A6A_SPIN_ORDER | mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:223 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/f5flip_20261020/PREREG_f5flip.md |
| MASHED_A8_A4_FIRST | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1114 | 1 | OFF (unset -> off) | c | owner found | ROADMAP.md:2163 | verify/a4order_20261017/PREREG_a4order.md |
| MASHED_AILEAF | mashedmod/src/mashed_re/Ai/AiLeafTargets.cpp:336 | 1 | OFF (unset -> off) | c | owner found | NEXT_SESSION.md:602 | verify/d3_bulk_20261011/aileaf/PREREG_AILEAF.md |
| MASHED_AILEAF_LIVEROWS | mashedmod/src/mashed_re/Ai/AiLeafTargets.cpp:357 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/d3_bulk_20261011/aileaf/RESULT_AILEAF.md |
| MASHED_AIM_FORCE | mashedmod/src/mashed_re/Powerup/PowerupAim.cpp:27 | 1 | OFF (unset -> off) | c | owner found | ROADMAP.md:2382 |  |
| MASHED_AI_DRIVES_PLAYER | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:5570 | 1 | OFF (unset -> off) | c | owner found | ROADMAP.md:2301 |  |
| MASHED_AI_FWD_ATROW | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:258 | 1 | OFF (set, not "0") | c | source comment says default OFF, no ledger owner |  | verify/d3bc_20261019/PREREG_d3bc.md |
| MASHED_AI_YAWW | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:348 | 1 | OFF (unset -> off) | c | owner found | NEXT_SESSION.md:2332 | verify/d3_leg3_20261005/PREREG_LEG3.md |
| MASHED_CAM_POSE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:9485 | 1 | value (parsed) | c | owner found | ROADMAP.md:3107 | verify/arctic_ref/geomlight_cmp/RESULT.md |
| MASHED_CAR | mashedmod/src/mashed_re/exe_main.cpp:9714 | 1 | value (parsed) | c | owner found | L-62(D5-PARTMORPH-FLIPPREP) ledger:D3-CF-U9782 ROADMAP.md:2551 | verify/aiveh_20261026/RESULT_D3-AI-VEHICLES.md |
| MASHED_CARCAR_CONTACT | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1856 | 1 | OFF (unset -> off) | c | owner found | ROADMAP.md:2551 NEXT_SESSION.md:1725 | verify/d3_carcar_20261005/PREREG_CARCAR.md |
| MASHED_CAR_BRAKE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:10251 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/carlod2_20261016/PREREG_carlod2.md |
| MASHED_CAR_LOD_EYE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:10283 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/rres_20261016/PREREG_rres.md |
| MASHED_CAR_RELIGHT | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:10152 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/librwrest_20261016/RESULT_librwrest.md |
| MASHED_D2SINK | mashedmod/src/mashed_re/Vehicle/D2SinkProbe.cpp:13 | 1 | OFF (unset -> off) | c | owner found | ROADMAP.md:778 | verify/d2bsp_20261016/RESULT_d2bsp.md |
| MASHED_D2S_RINGY | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:729 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/d2settle_20261016/PREREG_d2settle.md |
| MASHED_DETERMINISTIC | mashedmod/src/mashed_re/exe_main.cpp:9338 | 2 | OFF (unset -> off) | c | owner found | ROADMAP.md:538 NEXT_SESSION.md:1531 | verify/arctic_ref/geomlight_cmp/RESULT.md |
| MASHED_DRIVE_HOLD | mashedmod/src/mashed_re/exe_main.cpp:1650 | 1 | OFF (unset -> off) | c | owner found | L-57(D3-SUBSTATE-SETTLE-ORDER) L-58(D3-SUBSTATE-SETTLE-ORDER) ledger:D2-BAND-42S | verify/d1_nopart/RESULT.md |
| MASHED_DRIVE_HOLD_MS | mashedmod/src/mashed_re/exe_main.cpp:1784 | 1 | value (parsed) | c | owner found | L-57(D3-SUBSTATE-SETTLE-ORDER) L-58(D3-SUBSTATE-SETTLE-ORDER) ledger:D2-BAND-42S | verify/d2band42_20261029/PREREG_D2-BAND-42S.md |
| MASHED_FIXUP_F32 | mashedmod/src/mashed_re/Collision/ContactFixup.cpp:186 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/f5flip_20261020/PREREG_f5flip.md |
| MASHED_FIXUP_SLIDE_ORDER | mashedmod/src/mashed_re/Collision/ContactFixup.cpp:354 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/f5flip_20261020/PREREG_f5flip.md |
| MASHED_FIXUP_SUM_ORDER | mashedmod/src/mashed_re/Collision/ContactFixup.cpp:163 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/f5flip_20261020/PREREG_f5flip.md |
| MASHED_FIX_5CEA44 | mashedmod/src/mashed_re/Collision/CarWorldContacts.cpp:260 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/d2off_20261020/PREREG_d2off.md |
| MASHED_FI_RAND | mashedmod/src/mashed_re/Vehicle/ForceIntegratorStubs.cpp:79 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/u9776_20261020/PREREG_u9776.md |
| MASHED_GAME_MODE | mashedmod/src/mashed_re/exe_main.cpp:2481 | 2 | value (parsed) | c | owner found | ROADMAP.md:2222 NEXT_SESSION.md:426 | verify/brecipe_20261021/PREREG_brecipe.md |
| MASHED_GATE_RIBBON_AI | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:3452 | 1 | OFF (unset -> off) | c | owner found | ROADMAP.md:2300 | verify/d3_racepos_20261006/PREREG_LEGC_LEGA.md |
| MASHED_GF1 | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:6805 | 1 | OFF (unset -> off) | c | owner found | NEXT_SESSION.md:1242 | verify/d3_gatefire_20261008/PREREG_H4.md |
| MASHED_HANDLING_TYPE | mashedmod/src/mashed_re/Vehicle/VehicleInit.cpp:107 | 1 | value (parsed) | c | owner found | ROADMAP.md:1774 | verify/dcphys_20261021/PREREG_dcphys.md |
| MASHED_HEADING_FWD_LIVE | mashedmod/src/mashed_re/Race/HeadingAssist.cpp:157 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/u9767_20261019/PREREG_u9767.md |
| MASHED_INTERP_OVERLAYS | mashedmod/src/qol_asi/mashed_qol.cpp:721 | 2 | value (parsed) | c | source comment says default OFF, no ledger owner |  |  |
| MASHED_INTERP_SUBMIT | mashedmod/src/qol_asi/mashed_qol.cpp:1314 | 1 | value (parsed) | c | source comment says default OFF, no ledger owner |  |  |
| MASHED_INTRO_MOVIES | mashedmod/src/mashed_re/exe_main.cpp:2914 | 1 | value (parsed) | c | owner found | L-63(D5-INTROMOVIES-I1B) ledger:D5-INTROMOVIES-I1B ledger:D5-INTROMOVIES-FLIP [open: D5-INTROMOVIES-FLIP] | verify/intromovies_20261028/PREREG_D5-INTROMOVIES.md |
| MASHED_MATMUL_PC24 | mashedmod/src/mashed_re/Collision/RwpSolverMath2.cpp:80 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/u9776_20261020/PREREG_u9776.md |
| MASHED_MEASURE_NOOPP | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:5706 | 2 | OFF (unset -> off) | c | owner found | ROADMAP.md:1897 NEXT_SESSION.md:1823 | verify/d3_car2pickup_20261013/RESULT_CAR2PICKUP.md |
| MASHED_MEASURE_SOLO | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:5051 | 2 | OFF (unset -> off) | c | owner found | ROADMAP.md:891 | verify/a4order_20261017/PREREG_a4order.md |
| MASHED_MODE5_RESET | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:1873 | 1 | OFF (unset -> off) | c | owner found | ROADMAP.md:2481 NEXT_SESSION.md:928 | verify/camribbon_20261016/PREREG_camribbon_addendum2.md |
| MASHED_MODE7 | mashedmod/src/mashed_re/Ai/AiTargetClosest.cpp:460 | 1 | OFF (unset -> off) | c | owner found | NEXT_SESSION.md:846 | verify/d3bc_20261019/PREREG_d3bc.md |
| MASHED_MODE_LIVE | mashedmod/src/mashed_re/exe_main.cpp:2093 | 3 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/brecipe_20261021/PREREG_brecipe.md |
| MASHED_MUTE | mashedmod/src/mashed_re/exe_main.cpp:1853 | 3 | OFF (unset -> off) | c | owner found | NEXT_SESSION.md:71 | verify/audiovoltods_20261024/PREREG_voltods.md |
| MASHED_P3_RESET_PAIR | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:5376 | 2 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/d3subflip_20261028/RESULT_D3-SUBSTATE-FLIP.md |
| MASHED_PARTS_KINDS | mashedmod/src/mashed_re/D3d9Render/ParticleSystem.cpp:272 | 1 | value (parsed) | c | owner found | ROADMAP.md:499 | verify/d1_fxbloom/RESULT.md |
| MASHED_PATHPROG | mashedmod/src/mashed_re/Race/PathNodes.cpp:342 | 1 | OFF (unset -> off) | c | owner found | ledger:D4-DC-77b0 ledger:D4-DC-77b0-LAP ledger:D4-DC-77b0-NOTE [open: D4-DC-77b0,D4-DC-77b0-NOTE] | verify/c3k44_20261017/PREREG_c3k44.md |
| MASHED_PICKUPS_ORIG | mashedmod/src/mashed_re/Gameplay/PickupTick.cpp:233 | 1 | OFF (unset -> off) | c | owner found | NEXT_SESSION.md:858 | verify/d3bc_20261019/PREREG_d3bc.md |
| MASHED_PP_PLAYERDZ | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:7689 | 1 | value (parsed) | c | source comment says default OFF, no ledger owner |  | verify/d3_pathprog_a_20261016/PREREG_pathprog_a.md |
| MASHED_PP_PUBLISH | mashedmod/src/mashed_re/Race/PathProgStep.cpp:212 | 1 | value (parsed) | c | owner found | ledger:D4-DC-77b0-NOTE [open: D4-DC-77b0-NOTE] | verify/d3_pathprog_a_20261016/PREREG_pathprog_a.md |
| MASHED_PP_SPHERE_FIRST | mashedmod/src/mashed_re/Race/PathProgStep.cpp:99 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/d3_pathprog_a_20261016/PREREG_pathprog_a.md |
| MASHED_PU_FORCE | mashedmod/src/mashed_re/Powerup/PowerupSystem.cpp:56 | 1 | OFF (unset -> off) | c | owner found | ROADMAP.md:2382 | verify/d3close_20261021/PREREG_d3close.md |
| MASHED_RACEMETRIC_ARC | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:8865 | 1 | OFF (unset -> off) | c | owner found | NEXT_SESSION.md:1257 | verify/d3_consumer_20261007/PREREG_CONSUMER.md |
| MASHED_RACE_MODE | mashedmod/src/mashed_re/exe_main.cpp:2165 | 1 | value (parsed) | c | owner found | ledger:D4-DC-77b0-LAPLONG [open: D4-DC-77b0-LAPLONG] | verify/d4dc77b0lap_20261030/RESULT_D4-DC-77b0-LAP.md |
| MASHED_REAL_PHYSICS | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:303 | 1 | OFF (unset -> off) | c | owner found | ROADMAP.md:44 | verify/d2_b0c_20261002/PREREG_STEP2.md |
| MASHED_REASSERT_KEEP_NORMAL | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:429 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/u9754_20261018/PREREG_u9754.md |
| MASHED_REGISTRY | mashedmod/src/mashed_re/Race/WorldRegistry.cpp:164 | 1 | OFF (unset -> off) | c | owner found | NEXT_SESSION.md:852 | verify/d3bc_20261019/PREREG_d3bc.md |
| MASHED_REG_CALLBACKS | mashedmod/src/mashed_re/Gameplay/PickupTick.cpp:314 | 1 | OFF (unset -> off) | c | owner found | NEXT_SESSION.md:858 | verify/d3bc_20261019/PREREG_d3bc.md |
| MASHED_REG_CARS | mashedmod/src/mashed_re/Race/CarRegistrant.cpp:48 | 1 | OFF (unset -> off) | c | owner found | NEXT_SESSION.md:858 | verify/d3bc_20261019/PREREG_d3bc.md |
| MASHED_REG_LATE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:267 | 1 | OFF (set, not "0") | c | source comment says default OFF, no ledger owner |  | verify/iconcube_20261028/PREREG2_D5-ICONCUBE-SPIN.md |
| MASHED_RFX_EMITTERS | mashedmod/src/mashed_re/Gameplay/PickupGlow.cpp:49 | 2 | OFF (unset -> off) | c | owner found | L-52(D5-ICONCUBE-SPIN) | verify/d5defercallerwalk_20261030/RESULT_D5-DEFER-CALLERWALK.md |
| MASHED_RING_SKIP | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:813 | 1 | value (parsed) | c | owner found | ROADMAP.md:2317 NEXT_SESSION.md:597 | verify/d2settle_20261016/PREREG_d2settle.md |
| MASHED_ROTATE_LUT | mashedmod/src/mashed_re/Math/RwMatrixRotate.cpp:53 | 1 | OFF (unset -> off) | c | owner found | ledger:D4-DC-MATH-4d20 | verify/dcmath4d20_20261025/RESULT_dcmath4d20.md |
| MASHED_ROTATE_PC24 | mashedmod/src/mashed_re/Math/RwMatrixRotate.cpp:59 | 1 | OFF (unset -> off) | c | owner found | ledger:D4-DC-MATH-4d20 | verify/dcmath4d20_20261025/RESULT_dcmath4d20.md |
| MASHED_ROUND | mashedmod/src/mashed_re/exe_main.cpp:9735 | 1 | value (parsed) | c | owner found | L-4(D3-CF-U9779) ledger:D3-CF-U9779 ledger:D4-DC-77b0-LAPLONG [open: D4-DC-77b0-LAPLONG] | verify/brecipe_20261021/PREREG_brecipe.md |
| MASHED_ROUND_GRID | mashedmod/src/mashed_re/Race/RoundGrid.cpp:44 | 1 | OFF (unset -> off) | c | owner found | NEXT_SESSION.md:552 | verify/c3k44_20261017/PREREG_c3k44.md |
| MASHED_ROUND_RULE | mashedmod/src/mashed_re/exe_main.cpp:9783 | 1 | value (parsed) | c | owner found | ROADMAP.md:2222 NEXT_SESSION.md:426 | verify/brecipe_20261021/PREREG_brecipe.md |
| MASHED_RW_LUT_ALL | mashedmod/src/mashed_re/Math/RwLutGuard.h:99 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/u9764_20261019/PREREG_u9764.md |
| MASHED_SAVE | mashedmod/src/mashed_re/exe_main.cpp:9893 | 1 | value (parsed) | c | owner found | L-28(D4-WSF-SAVE-RT) ledger:D4-SAVE-KNOB ledger:D4-WSF-SAVE-FLIP | verify/savelink2_20261023/PREREG_knob_progress.md |
| MASHED_SAVE_KEEP_IMAGE | mashedmod/src/mashed_re/Race/GameFlow.cpp:240 | 1 | OFF (unset -> off) | c | owner found | L-28(D4-WSF-SAVE-RT) ledger:D4-WSF-SAVE-FLIP | verify/settrackpu_20261026/PREREG_D4-FU-SETTRACKPU.md |
| MASHED_SAVE_ORIGINAL | mashedmod/src/mashed_re/Save/GameSave.cpp:122 | 1 | OFF (unset -> off) | c | owner found | ledger:D4-SAVE-KNOB NEXT_SESSION.md:83 | verify/savelink2_20261023/PREREG_knob_progress.md |
| MASHED_SETTINGS_DEFAULTS | mashedmod/src/mashed_re/exe_main.cpp:1966 | 2 | OFF (unset -> off) | c | owner found | NEXT_SESSION.md:611 | verify/d4dc82a0_20261025/RESULT_D4-DC-82a0.md |
| MASHED_SHADOW_PRIVATE | mashedmod/src/mashed_re/Core/ShadowTrack.h:425 | 2 | OFF (unset -> off) | c | owner found | L-38(D4-WSB-C4-BROAD-WS) ledger:D4-WSB-SHADOW-PRIVATE | verify/d4wsbportbroad1_20261029/PREREG_D4-WSB-PORT-BROAD-1.md |
| MASHED_SIM_HZ | mashedmod/src/mashed_re/exe_main.cpp:3689 | 1 | value (parsed) | c | owner found | NEXT_SESSION.md:1545 | verify/d3_determinism_20261007/PREREG_F2.md |
| MASHED_SLOT_TYPE_LIVE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:146 | 1 | OFF (set, not "0") | c | source comment says default OFF, no ledger owner |  | verify/brecipe_20261021/PREREG_brecipe.md |
| MASHED_SPEED_REFRESH | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1744 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/d2engage_20261016/PREREG_d2engage.md |
| MASHED_STEER_HOLD | mashedmod/src/mashed_re/exe_main.cpp:3824 | 1 | value (parsed) | c | owner found | ROADMAP.md:1558 | verify/d2_b0c_20261002/PREREG_STEP2.md |
| MASHED_STEER_HOLD_AFTER | mashedmod/src/mashed_re/exe_main.cpp:3831 | 1 | value (parsed) | c | owner found | ROADMAP.md:1558 | verify/d2rebase_20261019/PREREG_d2rebase.md |
| MASHED_STEER_SCALE_RDATA | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:689 | 1 | OFF (set, not "0") | c | source comment says default OFF, no ledger owner |  | verify/d3bc_20261019/PREREG_d3bc.md |
| MASHED_SUBSTEP_OUTER | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:2607 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/soflip_20261021/PREREG_soflip.md |
| MASHED_TER_SPIN_F32 | mashedmod/src/mashed_re/Collision/CarWorldContacts.cpp:265 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/f5flip_20261020/PREREG_f5flip.md |
| MASHED_TGT3 | mashedmod/src/mashed_re/Ai/AiTargetClosest.cpp:276 | 1 | OFF (unset -> off) | c | owner found | NEXT_SESSION.md:545 | verify/c3k44_20261017/PREREG_c3k44.md |
| MASHED_TITLE | mashedmod/src/mashed_re/exe_main.cpp:1846 | 2 | value (parsed) | c | owner found | ROADMAP.md:1558 NEXT_SESSION.md:2855 | verify/d2_b0c_20261002/PREREG_STEP2.md |
| MASHED_TRACK_SEL | mashedmod/src/mashed_re/exe_main.cpp:2455 | 1 | value (parsed) | c | owner found | ROADMAP.md:1558 | verify/arctic_ref/geomlight_cmp/RESULT.md |
| MASHED_TRACK_VIEW | mashedmod/src/mashed_re/exe_main.cpp:9696 | 1 | value (parsed) | c | owner found | NEXT_SESSION.md:2854 | verify/d3_arm_20261005/PREREG_ARM.md |
| MASHED_UP_TRINORMAL | mashedmod/src/mashed_re/Collision/WheelContactSolver.cpp:188 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/lk21_20261018/PREREG_lk21.md |
| MASHED_VERIFY_OUT | mashedmod/src/mashed_re/exe_main.cpp:1204 | 1 | value (parsed) | c | owner found | ROADMAP.md:286 | verify/d1_nopart/RESULT.md |
| MASHED_VIDEOCFG_WRITE | mashedmod/src/mashed_re/Boot/VideoCfgFirstRun.cpp:140 | 1 | OFF (unset -> off) | c | owner found | L-29(D4-WSF-VIDEOCFG) ledger:D4-FLIP-L29-L30 | verify/firstrun_20261028/RESULT_D5-FIRSTRUN.md |
| MASHED_WEDGE_TRAP | mashedmod/src/mashed_re/exe_main.cpp:8885 | 1 | OFF (unset -> off) | c | owner found | ROADMAP.md:2203 |  |
| MASHED_WORLD_CONTACT | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1843 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  |  |
| MASHED_X87_PC24 | mashedmod/src/mashed_re/exe_main.cpp:998 | 1 | OFF (unset -> off) | c | source comment says default OFF, no ledger owner |  | verify/d2_pc24_20261010/PREREG.md |
| MASHED_A3_LOG | mashedmod/src/mashed_re/Vehicle/VehicleInit.cpp:326 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2settle_20261016/PREREG_d2settle.md |
| MASHED_A4_ORDER_TRACE | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1001 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/a4order_20261017/PREREG_a4order.md |
| MASHED_A5GDIAG | mashedmod/src/mashed_re/Vehicle/ForceIntegrator.cpp:156 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2_budget_20261002/PREREG_STEP1.md |
| MASHED_A5_RACE_TIMER_LOG | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1187 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/a5timer_20261026/PREREG_D3-CF-A5TIMER.md |
| MASHED_A6ADUMP | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:2449 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2engage_20261016/PREREG_d2engage.md |
| MASHED_A6A_DRIVELOG | mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:104 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/u9771_20261019/PREREG_u9771.md |
| MASHED_A6A_SELFTEST_CAP | mashedmod/src/mashed_re/Vehicle/PhysicsChainHooks.cpp:224 | 3 | value (parsed) | a | dev/debug/harness name |  | verify/dcphys_20261021/PREREG_dcphys.md |
| MASHED_A6A_TAILLOG | mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:90 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/u9771_20261019/PREREG_u9771.md |
| MASHED_A6A_VZLOG | mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:77 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/u9771_20261019/RESULT_u9771.md |
| MASHED_A6BTEST | mashedmod/src/mashed_re/Vehicle/AeroStabilize.cpp:157 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_A6_DIAG | mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:753 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_AILEAF_REPLAY | mashedmod/src/mashed_re/Ai/AiLeafTargets.cpp:461 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_AILEAF_REPLAY_OUT | mashedmod/src/mashed_re/Ai/AiLeafTargets.cpp:463 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_AILEAF_REPLAY_ROWS | mashedmod/src/mashed_re/Ai/AiLeafTargets.cpp:469 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_AILEAF_SEED4C0 | mashedmod/src/mashed_re/Ai/AiLeafTargets.cpp:352 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/d3_bulk_20261011/aileaf/PREREG_AILEAF.md |
| MASHED_AILEAF_TRACE | mashedmod/src/mashed_re/Ai/AiLeafTargets.cpp:296 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_AI_AB | mashedmod/src/mashed_re/Ai/AiControllerAB.cpp:782 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_AI_CONTACT_EVENT_TRACE | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:2326 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3bB2_20261017/PREREG_d3bB2.md |
| MASHED_AI_DIAG | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:6215 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_AI_LEADERGATE_SELFTEST | mashedmod/src/mashed_re/Ai/AiTargeting.cpp:627 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_AI_LOOK_REPLAY | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:1941 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3b_20261016/PREREG_d3b.md |
| MASHED_AI_LOOK_REPLAY_MASK | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:1946 | 1 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_AI_LOOK_REPLAY_OUT | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:1942 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3b_20261016/PREREG_d3b.md |
| MASHED_AI_LOS_SELFTEST | mashedmod/src/mashed_re/Ai/AiLineOfSight.cpp:108 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/dupinstall_20261016/RESULT_dupinstall.md |
| MASHED_AI_SPLINE_SELFTEST | mashedmod/src/mashed_re/Ai/AiNavHooks.cpp:346 | 3 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_AI_STEER_SELFTEST | mashedmod/src/mashed_re/Ai/AiTargeting.cpp:364 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_AI_STEPDUMP | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:6978 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2settle_20261016/PREREG_d2settle.md |
| MASHED_AI_TICKTRACE | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:2079 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_AI_V0GUARD_SELFTEST | mashedmod/src/mashed_re/Ai/AiTargeting.cpp:495 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_AI_WALLAHEAD_SELFTEST | mashedmod/src/mashed_re/Ai/AiWallAhead.cpp:107 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/dupinstall_20261016/RESULT_dupinstall.md |
| MASHED_AI_WALLLAT_SELFTEST | mashedmod/src/mashed_re/Ai/AiWallLateral.cpp:94 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_AUDIO_EVENT_LOG | mashedmod/src/mashed_re/Audio/AudioEngine.cpp:46 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/wsjmusic_20261026/PREREG_D4-WSJ-MUSIC.md |
| MASHED_AUDIO_WIRE_SELFTEST | mashedmod/src/mashed_re/Audio/AudioWire.cpp:368 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/wsjwire_20261026/PREREG_D4-WSJ-WIRE.md |
| MASHED_BATCH_DUMP | mashedmod/src/mashed_re/Collision/CollisionWorld.cpp:411 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2bsp_20261016/PREREG_d2bsp.md |
| MASHED_BATCH_DUMP_SLOT | mashedmod/src/mashed_re/Collision/CollisionWorld.cpp:416 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/u9759_20261018/PREREG_u9759.md |
| MASHED_BOX_TRACE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:217 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3cfboxflip_20261025/RESULT_D3-CF-BOX-FLIP.md |
| MASHED_BQ_AB | mashedmod/src/mashed_re/Render/GlobalByteQuadAB.cpp:124 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_BSP_REPLAY | mashedmod/src/mashed_re/Collision/CollisionWorld.cpp:450 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2bsp_20261016/PREREG_d2bsp.md |
| MASHED_BSP_REPLAY_OUT | mashedmod/src/mashed_re/Collision/CollisionWorld.cpp:451 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2bsp_20261016/PREREG_d2bsp.md |
| MASHED_C30_REPLAY | mashedmod/src/mashed_re/Ai/AiTargetClosest.cpp:502 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3_mode7_m4_20261010/PREREG_M4.md |
| MASHED_C30_REPLAY_OUT | mashedmod/src/mashed_re/Ai/AiTargetClosest.cpp:504 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_CAM_DUMP | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:3411 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/camribbon_20261016/PREREG_camribbon.md |
| MASHED_CAM_DUMP_LIBRW | mashedmod/src/mashed_re/LibRw/RwRaceSubmit.cpp:1338 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_CAM_REPLAY | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:8441 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/camribbon_20261016/RESULT_camribbon.md |
| MASHED_CAM_REPLAY_BASIS | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:9640 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d1r1_20261016/PREREG_d1r1.md |
| MASHED_CAM_REPLAY_GO | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:7715 | 3 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3subflip_20261029/RESULT_D3-SUBSTATE-FLIP.md |
| MASHED_CAM_REPLAY_P3 | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:8522 | 1 | OFF (set, not "0") | a | dev/debug/harness name |  | verify/d3subflip_20261029/RESULT_D3-SUBSTATE-FLIP.md |
| MASHED_CAM_REPLAY_SHOTS | mashedmod/src/mashed_re/exe_main.cpp:4697 | 2 | value (parsed) | a | dev/debug/harness name |  | verify/d1r2_20261016/PREREG_d1r2.md |
| MASHED_CAM_TICKS_REPLAY | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:8276 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3_gatefire_20261008/PREREG_U9201.md |
| MASHED_CARCAR_LOG | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1859 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3_car2pickup_20261013/PREREG_CAR2PICKUP.md |
| MASHED_CARCAR_RECDUMP | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:2116 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/u9778_20261021/PREREG_u9778.md |
| MASHED_CAR_BRAKELOG | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:10255 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/carlod2_20261016/PREREG_carlod2.md |
| MASHED_CAR_REPLAY | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:10175 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d1r1_20261016/PREREG_d1r1.md |
| MASHED_CAR_STATE0 | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:4567 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d5deferredsweep_20261030/RESULT_D5-DEFERRED-SWEEP.md |
| MASHED_CAR_VISLOG | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:10492 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/carlod_20261016/RESULT_carlod.md |
| MASHED_CC_REPLAY | mashedmod/src/mashed_re/Collision/CarCarContacts.cpp:534 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/u9769_20261019/PREREG_u9769.md |
| MASHED_CC_REPLAY_OUT | mashedmod/src/mashed_re/Collision/CarCarContacts.cpp:535 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/u9769_20261019/RESULT_u9769.md |
| MASHED_CC_TRACE | mashedmod/src/mashed_re/Collision/CarCarContacts.cpp:76 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/u9769_20261019/PREREG_u9769.md |
| MASHED_CFGEDIT_DEMO | mashedmod/src/mashed_re/exe_main.cpp:9347 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_CHAL_UNLOCK | mashedmod/src/mashed_re/exe_main.cpp:9959 | 1 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_CONTACT_DIAG | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:2237 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_COUPLING_DIAG | mashedmod/src/mashed_re/Math/RwMatrixRotate.cpp:99 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2_gain_20261001/PREREG.md |
| MASHED_CROWN_TEST | mashedmod/src/mashed_re/exe_main.cpp:4472 | 1 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_D2S_SEED1 | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:740 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2settle_20261016/PREREG_d2settle.md |
| MASHED_D3D_TIMEOUT_MS | mashedmod/src/mashed_re/exe_main.cpp:3117 | 1 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_DBG_BBDUMP | mashedmod/src/mashed_re/exe_main.cpp:4834 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/freecam_title_20261001/PREREGISTRATION.md |
| MASHED_DBG_BBDUMP_OUT | mashedmod/src/mashed_re/exe_main.cpp:4848 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/freecam_title_20261001/PREREGISTRATION.md |
| MASHED_DBG_BBDUMP_REQ | mashedmod/src/mashed_re/exe_main.cpp:4861 | 1 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_DBG_CAM | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:9667 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_DBG_CAM_OUT | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:9671 | 1 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_DBG_CARPOS | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:10006 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/gridyaw_20261016/PREREG_gridyaw.md |
| MASHED_DBG_CARPOS_OUT | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:10011 | 1 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_DBG_CARSHADE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:10447 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_DBG_CAR_STATE0 | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:4578 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d5partmorphflip_20261030/PREREG_D5-PARTMORPH-FLIPPREP.md |
| MASHED_DBG_CONFIGEDIT | mashedmod/src/mashed_re/exe_main.cpp:2697 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_DBG_D3DREC | mashedmod/src/mashed_re/D3d9Render/DrawRec.cpp:407 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d1r1_20261016/PREREG_d1r1.md |
| MASHED_DBG_DRAWLOG | mashedmod/src/mashed_re/D3d9Render/RwIm2DBridge.cpp:142 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_DBG_DRAWSTREAM | mashedmod/src/mashed_re/D3d9Render/DrawStreamDump.cpp:56 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/menuspeed_20261027/PREREG_D4-FU-MENUSPEED.md |
| MASHED_DBG_DRAWSTREAM3D | mashedmod/src/mashed_re/D3d9Render/DrawStreamDump.cpp:154 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/d1r1_20261016/PREREG_d1r1.md |
| MASHED_DBG_DRAWSTREAM3D_OUT | mashedmod/src/mashed_re/D3d9Render/DrawStreamDump.cpp:185 | 1 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_DBG_DRAWSTREAM_OUT | mashedmod/src/mashed_re/D3d9Render/DrawStreamDump.cpp:91 | 1 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_DBG_DUMPBADGE | mashedmod/src/mashed_re/exe_main.cpp:8245 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_DBG_FXBODY | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:10346 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/rfx2_20261016/RESULT_rfx2.md |
| MASHED_DBG_HULL | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:4128 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_DBG_LENS | mashedmod/src/mashed_re/LibRw/RwRaceFx.cpp:1007 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_DBG_MEASURE | mashedmod/src/mashed_re/exe_main.cpp:5830 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d4dc82a0_20261025/RESULT_D4-DC-82a0.md |
| MASHED_DBG_MENU | mashedmod/src/mashed_re/exe_main.cpp:5268 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_DBG_MODAL | mashedmod/src/mashed_re/exe_main.cpp:10007 | 1 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_DBG_NO_ARC | mashedmod/src/mashed_re/exe_main.cpp:5022 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_DBG_NO_PREVIEWS | mashedmod/src/mashed_re/exe_main.cpp:4941 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_DBG_OBJSHAD | mashedmod/src/mashed_re/LibRw/RwRaceFx.cpp:1090 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_DBG_PARTMORPH | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:4537 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d5partmorphflip_20261030/PREREG_D5-PARTMORPH-FLIPPREP.md |
| MASHED_DBG_PARTSTATE0 | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:4431 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/partmorph_20261028/PREREG_D5-PARTMORPH-PRODUCER.md |
| MASHED_DBG_PICKUPDUMP | mashedmod/src/mashed_re/D3d9Render/PickupField.cpp:30 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/pickups_fix_20261001/RESULT_STAGE1.md |
| MASHED_DBG_PUGLOW | mashedmod/src/mashed_re/Gameplay/PickupGlow.cpp:79 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/iconcube_20261028/PREREG_D5-ICONCUBE-GLOW.md |
| MASHED_DBG_SHADOWRT_DUMP | mashedmod/src/mashed_re/LibRw/RwRaceFx.cpp:2233 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_DBG_SHADOWRT_NAMED | mashedmod/src/mashed_re/LibRw/RwRaceFx.cpp:1936 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_DBG_SHADOWSEL | mashedmod/src/mashed_re/LibRw/RwRaceFx.cpp:2042 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_DBG_TEXMATCH | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:962 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_DEMO_DRIVE | mashedmod/src/mashed_re/exe_main.cpp:1747 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_DET_FRAMES | mashedmod/src/mashed_re/exe_main.cpp:9341 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/d1_nopart/RESULT.md |
| MASHED_DEV_HUD | mashedmod/src/mashed_re/exe_main.cpp:4035 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_DRIVE_DEMO | mashedmod/src/mashed_re/exe_main.cpp:3732 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d1_nopart/RESULT.md |
| MASHED_ELIM_TRACE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:8363 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/rpct_20261022/PREREG_rpct.md |
| MASHED_FIXUP_LOG | mashedmod/src/mashed_re/Collision/ContactFixup.cpp:71 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2lateral_20261016/PREREG_d2lateral.md |
| MASHED_FORCE_SPLIT | mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:115 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/u9760_20261019/PREREG_u9760.md |
| MASHED_FORCE_SPLIT_SLOT | mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:78 | 4 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_FORCE_SPLIT_WHEEL | mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:117 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/u9762_20261019/PREREG_u9762.md |
| MASHED_FORCE_SWVP | mashedmod/src/mashed_re/exe_main.cpp:3119 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_FPS_CAP | mashedmod/src/d3d9_shim/d3d9_shim.cpp:362 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/d3-cf-r7detframe_20261025/RESULT_D3-CF-R7DETFRAME.md |
| MASHED_FPS_CAP_RACE | mashedmod/src/d3d9_shim/d3d9_shim.cpp:372 | 1 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_FPS_LOG | mashedmod/src/d3d9_shim/d3d9_shim.cpp:374 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/no_focus_pause_20260819/RESULT.md |
| MASHED_FPS_OSD | mashedmod/src/d3d9_shim/d3d9_shim.cpp:464 | 1 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_FX_ORIG_SURFACE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:9280 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/fxgates_20261026/PREREG_fxgates.md |
| MASHED_FX_PAIRLOG | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:1675 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/fxbrake_20261027/RESULT_D4-FX-BRAKEBYTE-WRITER.md |
| MASHED_GRID_DIR_TRACE | mashedmod/src/mashed_re/Race/RoundGrid.cpp:210 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/gridyaw_20261016/RESULT_gridyaw.md |
| MASHED_GRID_ROTATE_TRACE | mashedmod/src/mashed_re/Race/RoundGrid.cpp:177 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/dc4d20flip_20261025/RESULT_D4-DC-4d20-FLIP.md |
| MASHED_HEADING_ASSIST_TRACE | mashedmod/src/mashed_re/Race/HeadingAssist.cpp:141 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3b_20261016/PREREG_d3b.md |
| MASHED_HIRES | mashedmod/src/d3d9_shim/d3d9_shim.cpp:78 | 2 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_HOOK_ONLY | mashedmod/src/mashed_re/Ai/AiControllerAB.cpp:787 | 7 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3_gatefire_20261008/RESULT_LEG1A.md |
| MASHED_HUD3D | mashedmod/src/mashed_re/LibRw/RwRaceFx.cpp:1743 | 1 | ON (EnvOff helper) | a | opt-out of default-ON path |  | verify/rfx_20261016/RESULT_rfx.md |
| MASHED_HUD_NONTEAM | mashedmod/src/mashed_re/LibRw/RwRaceFx.cpp:1744 | 1 | ON (EnvOff helper) | a | opt-out of default-ON path |  | verify/rfx2_20261016/PREREG_rfx2.md |
| MASHED_HUD_REPLAY | mashedmod/src/mashed_re/LibRw/RwRaceFx.cpp:1270 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/rfx2_20261016/PREREG_rfx2.md |
| MASHED_INTRO_SHOT | mashedmod/src/mashed_re/exe_main.cpp:2924 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/intromovies_20261028/PREREG_D5-INTROMOVIES.md |
| MASHED_LAP_DIAG | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:8785 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_LEAF_SELFTEST | mashedmod/src/mashed_re/Race/CameraClusterHooks.cpp:78 | 7 | OFF (set, not "0") | a | dev/debug/harness name |  | verify/dcleaf_20261022/RESULT_dcleaf.md |
| MASHED_LEAF_SELFTEST_CAP | mashedmod/src/mashed_re/Race/CameraClusterHooks.cpp:83 | 2 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_LEAF_SELFTEST_SWEEP | mashedmod/src/mashed_re/Render/RwStreamCore.cpp:447 | 4 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/dcleaf_20261022/RESULT_dcleaf.md |
| MASHED_LEG2_TRACE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:8303 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3subph3_20261028/PREREG_d3subph3.md |
| MASHED_LIBRW_RWSTATES | mashedmod/src/mashed_re/LibRw/RwRaceSubmit.cpp:1403 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d1r1_20261016/RESULT_d1r1.md |
| MASHED_LIBRW_TEXLOG | mashedmod/src/mashed_re/LibRw/RwRasterBridge.cpp:226 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_LIBRW_WALKLOG | mashedmod/src/mashed_re/LibRw/RwRaceSubmit.cpp:1157 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/rstates_20261016/RESULT_rstates.md |
| MASHED_LINK_AB | mashedmod/src/mashed_re/Render/RwPluginLinkSetAB.cpp:209 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_LOG_CW | mashedmod/src/mashed_re/exe_main.cpp:999 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2_pc24_20261010/PREREG.md |
| MASHED_LT_LOG | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:1148 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/u9776_20261020/PREREG_u9776.md |
| MASHED_MENU_ANIM_LOG | mashedmod/src/mashed_re/exe_main.cpp:5331 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/menuclockflip_20261027/RESULT_D4-FU-MENUCLOCK-FLIP.md |
| MASHED_MODE7_TRACE | mashedmod/src/mashed_re/Ai/AiTargetClosest.cpp:434 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3_u9683_20261016/PREREG_u9683.md |
| MASHED_MOTION_DIAG | mashedmod/src/mashed_re/Vehicle/LaunchRevCharge.cpp:289 | 3 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2_b0c_20261002/PREREG_STEP2.md |
| MASHED_MOTION_DIAG_AI | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:2518 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_NAV_DEMO | mashedmod/src/mashed_re/exe_main.cpp:9344 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2_sched_20261001/PREREG_STEP1.md |
| MASHED_NAV_KEYS | mashedmod/src/mashed_re/exe_main.cpp:9348 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/fe_residuals_20261016/u9672/RESULT.md |
| MASHED_NAV_KEYS_GAP | mashedmod/src/mashed_re/exe_main.cpp:9352 | 1 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_NO_RECORD_VEL_KEEP | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1387 | 1 | ON (opt-out name) | a | opt-out of default-ON path |  | verify/d3_car2pickup_20261013/RESULT_CAR2PICKUP.md |
| MASHED_OFFMESH_LOG | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:5850 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_ORIG_BBDUMP | mashedmod/src/d3d9_shim/d3d9_shim.cpp:241 | 1 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_ORIG_BBDUMP_REQ | mashedmod/src/d3d9_shim/d3d9_shim.cpp:335 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/r10_20261016/RESULT_r10.md |
| MASHED_OWN_RAND_LOG | mashedmod/src/mashed_re/Collision/RwpSolverLeaves1.cpp:71 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d4wsb_randflip_20261028/RESULT_D4-WSB-RAND-FLIP.md |
| MASHED_PARITY | mashedmod/src/mashed_re/exe_main.cpp:9345 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/parity_20260819/RESULT.md |
| MASHED_PARITY_BG | mashedmod/src/d3d9_shim/d3d9_shim.cpp:562 | 2 | value (parsed) | a | dev/debug/harness name |  | verify/parity_20260819/RESULT.md |
| MASHED_PARTMORPH_SELFTEST | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:4483 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/partmorph_20261028/PREREG_D5-PARTMORPH.md |
| MASHED_PATHPROG_DBG | mashedmod/src/mashed_re/Race/PathNodes.cpp:244 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_PATHPROG_DUMP | mashedmod/src/mashed_re/Race/PathNodes.cpp:391 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/camribbon_20261016/PREREG_camribbon.md |
| MASHED_PATHPROG_GATE_LOG | mashedmod/src/mashed_re/Race/PathProgStep.cpp:269 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d4dc77b0lap_20261030/RESULT_D4-DC-77b0-LAP.md |
| MASHED_PATHPROG_REPLAY | mashedmod/src/mashed_re/Race/PathProgStep.cpp:350 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_PATHPROG_REPLAY_OUT | mashedmod/src/mashed_re/Race/PathProgStep.cpp:352 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_PATHPROG_TRACE | mashedmod/src/mashed_re/Race/PathProgStep.cpp:295 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/c3k44_20261017/PREREG_c3k44.md |
| MASHED_PHYS_C4_SELFTEST | mashedmod/src/mashed_re/Collision/RwpIntegrator.cpp:51 | 8 | value (parsed) | a | dev/debug/harness name |  | verify/wsbbvh_20261027/PREREG_D4-WSB-BVH-WRITESET.md |
| MASHED_PHYS_SELFTEST_CAP | mashedmod/src/mashed_re/Vehicle/PhysicsChainHooks.cpp:223 | 3 | value (parsed) | a | dev/debug/harness name |  | verify/dcphys2_20261022/PREREG_dcphys2.md |
| MASHED_PICKUPS_REPLAY | mashedmod/src/mashed_re/Gameplay/PickupTick.cpp:488 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_PICKUPS_REPLAY_OUT | mashedmod/src/mashed_re/Gameplay/PickupTick.cpp:490 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_PKCB_REPLAY | mashedmod/src/mashed_re/Gameplay/PickupTick.cpp:367 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3_mode7_m3_20261010/PREREG_M3.md |
| MASHED_PKCB_REPLAY_OUT | mashedmod/src/mashed_re/Gameplay/PickupTick.cpp:369 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_PKCB_TRACE | mashedmod/src/mashed_re/Gameplay/PickupTick.cpp:253 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3_mode7_m3_20261010/PREREG_M3.md |
| MASHED_PLAYERTRACE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:5950 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d1r3_20261016/RESULT_d1r3.md |
| MASHED_PLAYERTRACE_OUT | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:5951 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d1r2_20261016/RESULT_d1r2.md |
| MASHED_PLAYTHROUGH | mashedmod/src/mashed_re/exe_main.cpp:1652 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_PLAY_DEMO | mashedmod/src/mashed_re/exe_main.cpp:3734 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2_b0c_20261002/PREREG_STEP2.md |
| MASHED_PONTOON_REPLAY | mashedmod/src/mashed_re/Track/WarzonePontoon.cpp:779 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/dcmath4680_20261025/PREREG_dcmath4680.md |
| MASHED_PONTOON_REPLAY_OUT | mashedmod/src/mashed_re/Track/WarzonePontoon.cpp:876 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_PONTOON_STATE | mashedmod/src/mashed_re/Track/WarzonePontoon.cpp:716 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/pontoon_20261016/PREREG_pontoon.md |
| MASHED_PONTOON_TRACE | mashedmod/src/mashed_re/Track/WarzonePontoon.cpp:694 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/pontoon_20261016/PREREG_pontoon.md |
| MASHED_POS_SEED | mashedmod/src/mashed_re/Vehicle/PosModelKnobs.h:32 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2bsp_20261016/PREREG_d2bsp.md |
| MASHED_POS_SEED_KEEP | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1275 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2posmodel_20261016/PREREG_d2posmodel.md |
| MASHED_POS_SEED_MIN | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1274 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2posmodel_20261016/PREREG_d2posmodel.md |
| MASHED_PP_BOOSTSEED | mashedmod/src/mashed_re/Vehicle/ForceIntegratorStubs.cpp:197 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3_pathprog_a_20261016/PREREG_pathprog_a.md |
| MASHED_PP_SPHERE_DUMP | mashedmod/src/mashed_re/Race/PathProgStep.cpp:98 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3_pathprog_a_20261016/PREREG_pathprog_a.md |
| MASHED_PU_CONTACTDUMP | mashedmod/src/mashed_re/Powerup/PowerupContact.cpp:44 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_PU_STEPDUMP | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:7389 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_QOL_BORDERLESS | mashedmod/src/d3d9_shim/d3d9_shim.cpp:109 | 1 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_QOL_LOG | mashedmod/src/qol_asi/mashed_qol.cpp:1479 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_RACE_DEMO | mashedmod/src/mashed_re/exe_main.cpp:9346 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/arctic_ref/geomlight_cmp/RESULT.md |
| MASHED_RECDUMP | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1045 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2off_20261020/PREREG_d2off.md |
| MASHED_RECDUMP_A4 | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1764 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/u9749_20261017/PREREG_u9749.md |
| MASHED_RECDUMP_SLOT | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1020 | 1 | value (parsed) | a | dev/debug/harness name |  | verify/lk21_20261018/PREREG_lk21.md |
| MASHED_REFDIST_REPLAY | mashedmod/src/mashed_re/Race/SpectatorDistances.cpp:271 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3_u9219_20261013/PREREG_U9219.md |
| MASHED_REFDIST_REPLAY_OUT | mashedmod/src/mashed_re/Race/SpectatorDistances.cpp:273 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3_u9219_20261013/PREREG_U9219.md |
| MASHED_REFDIST_TRACE | mashedmod/src/mashed_re/Race/SpectatorDistances.cpp:344 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3_u9683_20261016/PREREG_u9683.md |
| MASHED_REGISTRY_REPLAY | mashedmod/src/mashed_re/Race/WorldRegistry.cpp:220 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_REGISTRY_REPLAY_OUT | mashedmod/src/mashed_re/Race/WorldRegistry.cpp:222 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_REGISTRY_TRACE | mashedmod/src/mashed_re/Race/WorldRegistry.cpp:183 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3_car2pickup_20261013/PREREG_CAR2PICKUP.md |
| MASHED_RENDER_CANLK_SELFTEST | mashedmod/src/mashed_re/Render/RasterDeviceWrappers.cpp:317 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_RENDER_COPY_SELFTEST | mashedmod/src/mashed_re/Render/RasterDeviceWrappers.cpp:634 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_RENDER_LKREAD_SELFTEST | mashedmod/src/mashed_re/Render/RasterDeviceWrappers.cpp:878 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_RENDER_MIPLK_SELFTEST | mashedmod/src/mashed_re/Render/RasterDeviceWrappers.cpp:474 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_RENDER_UNLOCK_SELFTEST | mashedmod/src/mashed_re/Render/RasterDeviceWrappers.cpp:156 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_RES | mashedmod/src/d3d9_shim/d3d9_shim.cpp:69 | 7 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_RESULT_DEMO | mashedmod/src/mashed_re/exe_main.cpp:1648 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/freecam_title_20261001/PREREGISTRATION.md |
| MASHED_RE_BORDERLESS | mashedmod/src/d3d9_shim/d3d9_shim.cpp:120 | 1 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_RE_NO_AUTO_HOOK | mashedmod/src/dinput8_shim/dinput8_shim.cpp:69 | 2 | value (parsed) | a | dev/debug/harness name |  | verify/capfix_20261016/RESULT_capfix.md |
| MASHED_RE_NO_SCREEN1_PIN | mashedmod/src/d3d9_shim/d3d9_shim.cpp:142 | 1 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_ROOT | mashedmod/src/mashed_re/exe_main.cpp:9304 | 3 | value (parsed) | a | dev/debug/harness name |  | verify/arctic_ref/geomlight_cmp/RESULT.md |
| MASHED_ROUND_GRID_CHECK | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:7597 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_ROUND_GRID_ORDER_REPLAY | mashedmod/src/mashed_re/Race/RoundGrid.cpp:289 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_ROUND_GRID_ORDER_REPLAY_OUT | mashedmod/src/mashed_re/Race/RoundGrid.cpp:290 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_ROUND_GRID_REPLAY | mashedmod/src/mashed_re/Race/RoundGrid.cpp:314 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/roundgrid_c3_20261016/RESULT_roundgrid_c3.md |
| MASHED_ROUND_GRID_REPLAY_OUT | mashedmod/src/mashed_re/Race/RoundGrid.cpp:316 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/roundgrid_c3_20261016/RESULT_roundgrid_c3.md |
| MASHED_ROUND_GRID_TRACE | mashedmod/src/mashed_re/Race/RoundGrid.cpp:36 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_RULE7_TRACE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:180 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/r7keep_20261022/PREREG_r7keep.md |
| MASHED_SAVE_SELFTEST | mashedmod/src/mashed_re/Race/GameFlow.cpp:728 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d4_save_20261005/PREREG_GAMESAVE_EXE.md |
| MASHED_SAVE_SELFTEST_OUT | mashedmod/src/mashed_re/Save/GameSave.cpp:398 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d4_save_20261005/RESULT_GAMESAVE_EXE.md |
| MASHED_SAVE_SELFTEST_OUT2 | mashedmod/src/mashed_re/Race/GameFlow.cpp:732 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_SAVE_SELFTEST_SHIPPED | mashedmod/src/mashed_re/Race/GameFlow.cpp:646 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_SEEK_TRACE | mashedmod/src/mashed_re/Ai/AiTrackObjSeek.cpp:147 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/pontoon_20261016/PREREG_pontoon.md |
| MASHED_SETTLE_DIAG | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1140 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2posmodel_20261016/PREREG_d2posmodel.md |
| MASHED_SHADOW_AB | mashedmod/src/mashed_re/Core/ShadowAB.h:101 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d4wsbportbroad1_20261029/PREREG_D4-WSB-PORT-BROAD-1.md |
| MASHED_SHADOW_TRACE | mashedmod/src/mashed_re/Core/ShadowTrack.h:176 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/wsbshadowpriv_20261027/PREREG_D4-WSB-SHADOW-PRIVATE.md |
| MASHED_SNAP_LOG | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:1697 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/hbfullcheck_20261028/RESULT_HB-FULLCHECK-R17.md |
| MASHED_SPAN_BOOT_SEED | mashedmod/src/mashed_re/exe_main.cpp:9918 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/spanwriters_20261027/PREREG_D4-FU-SPANINIT-WRITERS.md |
| MASHED_SPAN_BOOT_SEED_DUMP | mashedmod/src/mashed_re/exe_main.cpp:9927 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/spanwriters_20261027/PREREG_D4-FU-SPANINIT-WRITERS.md |
| MASHED_SPAWN_SOLVER_DIAG | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:878 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/u9756_20261018/PREREG_u9756.md |
| MASHED_SPAWN_Y_LOG | mashedmod/src/mashed_re/Collision/CollisionWorld.cpp:337 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2off_20261020/PREREG_d2off.md |
| MASHED_SUBFLIP_DIAG | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:5181 | 1 | OFF (set, not "0") | a | dev/debug/harness name |  | verify/d3subflip_20261028/RESULT_D3-SUBSTATE-FLIP.md |
| MASHED_SUBSTATE | mashedmod/src/mashed_re/Race/SubStateCallees.cpp:72 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/a5timer_20261026/PREREG_D3-CF-A5TIMER.md |
| MASHED_SUBSTATE_CD | mashedmod/src/mashed_re/Race/SubStateCallees.cpp:527 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3_gatefire_20261008/PREREG_414180.md |
| MASHED_SUBSTEP_VELPROBE | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1142 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_TGT3_REPLAY | mashedmod/src/mashed_re/Ai/AiTargetClosest.cpp:316 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d3_u9684_20261013/PREREG_U9684.md |
| MASHED_TGT3_REPLAY_OUT | mashedmod/src/mashed_re/Ai/AiTargetClosest.cpp:318 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_TGT3_TRACE | mashedmod/src/mashed_re/Ai/AiTargetClosest.cpp:264 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/c3k44_20261017/PREREG_c3k44.md |
| MASHED_TRACE_F350 | mashedmod/src/mashed_re/Collision/RwpSolverCore10.cpp:55 | 1 | value (parsed) | a | dev/debug/harness name |  |  |
| MASHED_TRACK_SKIP_EXCLUDED | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:2836 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/city_blackroad/RESULT.md |
| MASHED_UNLOCK | mashedmod/src/qol_asi/mashed_qol.cpp:1493 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_UTIL_SLIDER2_SELFTEST | mashedmod/src/mashed_re/Util/FloatSlider2SelfTest.cpp:143 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_VEHICLE_PBT_SELFTEST | mashedmod/src/mashed_re/Vehicle/GhostPlaybackTick.cpp:214 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  |  |
| MASHED_WCS_POSLOG | mashedmod/src/mashed_re/Collision/WheelContactSolver.cpp:115 | 2 | value (parsed) | a | dev/debug/harness name |  | verify/u9773_20261020/PREREG_u9773.md |
| MASHED_WCS_TRILOG | mashedmod/src/mashed_re/Collision/WheelContactSolver.cpp:409 | 2 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/u9773_20261020/PREREG_u9773.md |
| MASHED_WIN_POS | mashedmod/src/d3d9_shim/d3d9_shim.cpp:161 | 2 | value (parsed) | a | dev/debug/harness name |  | verify/d1_nopart/RESULT.md |
| MASHED_WORLD_CONTACT_LOG | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1848 | 1 | OFF (unset -> off) | a | dev/debug/harness name |  | verify/d2_gain_20261001/PREREG.md |
| MASHED_A5_DRAFT_LUT | mashedmod/src/mashed_re/Vehicle/ForceIntegrator.cpp:33 | 1 | ON (seeded "1") [corrected D5-KNOBS-INVENTORY-FIX; was OFF (unset -> off)] | d | no owner, no default comment; named in verify/d2off_20261020/PREREG_d2off.md |  | verify/d2off_20261020/PREREG_d2off.md |
| MASHED_A6B_ACOS_CRT | mashedmod/src/mashed_re/Vehicle/AeroStabilize.cpp:95 | 1 | ON (seeded "1") [corrected D5-KNOBS-INVENTORY-FIX; was OFF (unset -> off)] | d | no owner, no default comment; named in verify/dcphys2_20261022/PREREG_dcphys2.md |  | verify/dcphys2_20261022/PREREG_dcphys2.md |
| MASHED_A6B_NORM_LUT | mashedmod/src/mashed_re/Vehicle/AeroStabilize.cpp:96 | 1 | ON (seeded "1") [corrected D5-KNOBS-INVENTORY-FIX; was OFF (unset -> off)] | d | no owner, no default comment; named in verify/dcphys2_20261022/PREREG_dcphys2.md |  | verify/dcphys2_20261022/PREREG_dcphys2.md |
| MASHED_ABIL_KEYS | mashedmod/src/mashed_re/exe_main.cpp:6323 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_AILEAF_PC | mashedmod/src/mashed_re/Ai/AiLeafTargets.cpp:281 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/d3_bulk_20261011/aileaf/PREREG_AILEAF.md |  | verify/d3_bulk_20261011/aileaf/PREREG_AILEAF.md |
| MASHED_AI_CONTACT_EVENT | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:2313 | 1 | ON (seeded "1") [corrected D5-KNOBS-INVENTORY-FIX; was OFF (unset -> off)] | d | no owner, no default comment; named in verify/a4order_20261017/PREREG_a4order.md |  | verify/a4order_20261017/PREREG_a4order.md |
| MASHED_AI_DIFFFLAG | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:2389 | 1 | value (parsed) | d | no owner, no default comment; named in verify/d3_gatefire_20261008/PREREG_414180.md |  | verify/d3_gatefire_20261008/PREREG_414180.md |
| MASHED_AI_TICK_FIRST | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:5508 | 2 | ON (seeded "1") [corrected D5-KNOBS-INVENTORY-FIX; was OFF (unset -> off)] | d | no owner, no default comment; named in verify/a4order_20261017/PREREG_a4order.md |  | verify/a4order_20261017/PREREG_a4order.md |
| MASHED_C4N | mashedmod/src/mashed_re/Collision/NarrowC4Probe.cpp:54 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_C4N_STRIDE | mashedmod/src/mashed_re/Collision/NarrowC4Probe.cpp:62 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_CAM_FOV | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:9759 | 1 | value (parsed) | d | no owner, no default comment; named in verify/d1_lens/RESULT.md |  | verify/d1_lens/RESULT.md |
| MASHED_CARPARTS_LAW | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:4637 | 2 | OFF (unset -> off) | d | no owner, no default comment; named in verify/librwrest_checks_20261016/PREREG_librwrest_checks.md |  | verify/librwrest_checks_20261016/PREREG_librwrest_checks.md |
| MASHED_CARPARTS_LAW_OUT | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:4638 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/librwrest_checks_20261016/PREREG_librwrest_checks.md |  | verify/librwrest_checks_20261016/PREREG_librwrest_checks.md |
| MASHED_CAR_BRAKE_ROWS | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:10229 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/carlod2_20261016/PREREG_carlod2.md |  | verify/carlod2_20261016/PREREG_carlod2.md |
| MASHED_CAR_COLOUR | mashedmod/src/mashed_re/exe_main.cpp:2114 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_CAR_ENV | mashedmod/src/mashed_re/LibRw/RwCarFx.cpp:153 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/carlod_20261016/PREREG_carlod.md |  | verify/carlod_20261016/PREREG_carlod.md |
| MASHED_CAR_ENV_PREVCAM | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:10218 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/carlod_20261016/PREREG_carlod.md |  | verify/carlod_20261016/PREREG_carlod.md |
| MASHED_CAR_LIVERY | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:4287 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/librwrest_20261016/RESULT_librwrest.md |  | verify/librwrest_20261016/RESULT_librwrest.md |
| MASHED_CAR_PARTS | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:4311 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/librwrest_20261016/RESULT_librwrest.md |  | verify/librwrest_20261016/RESULT_librwrest.md |
| MASHED_CAR_SEL | mashedmod/src/mashed_re/exe_main.cpp:2120 | 1 | value (parsed) | d | no owner, no default comment; named in verify/aiveh_20261026/PREREG_D3-AI-VEHICLES.md |  | verify/aiveh_20261026/PREREG_D3-AI-VEHICLES.md |
| MASHED_CC_PC24 | mashedmod/src/mashed_re/Collision/CarCarContacts.cpp:97 | 1 | ON (seeded "1") [corrected D5-KNOBS-INVENTORY-FIX; was OFF (set, not "0")] | d | no owner, no default comment; named in verify/u9769_20261019/PREREG_u9769.md |  | verify/u9769_20261019/PREREG_u9769.md |
| MASHED_CUP | mashedmod/src/mashed_re/exe_main.cpp:1657 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_D2BSP_KEY0 | mashedmod/src/mashed_re/Collision/CollisionWorld.cpp:239 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/d2bsp_20261016/PREREG_d2bsp.md |  | verify/d2bsp_20261016/PREREG_d2bsp.md |
| MASHED_D2CHUNK_FRAMEMS | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1666 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/d2bsp_20261016/PREREG_d2bsp.md |  | verify/d2bsp_20261016/PREREG_d2bsp.md |
| MASHED_D2LAT_SPAWN_YAW | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:1716 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/d2lateral_20261016/PREREG_d2lateral.md |  | verify/d2lateral_20261016/PREREG_d2lateral.md |
| MASHED_D2SINK_SM | mashedmod/src/mashed_re/Vehicle/D2SinkProbe.cpp:40 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/d2_wheelstate_20261002/PREREG_STEP2.md |  | verify/d2_wheelstate_20261002/PREREG_STEP2.md |
| MASHED_D3D_NOVSYNC | mashedmod/src/mashed_re/exe_main.cpp:3121 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_D3D_REF | mashedmod/src/mashed_re/exe_main.cpp:3120 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_DECOUPLE | mashedmod/src/qol_asi/mashed_qol.cpp:1494 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_FONT_FLATRGB | mashedmod/src/mashed_re/D3d9Render/MashedFont.cpp:227 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_FONT_GAMMA | mashedmod/src/mashed_re/D3d9Render/MashedFont.cpp:210 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_FRAME_PROF | mashedmod/src/mashed_re/exe_main.cpp:3644 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_FX_BODYPOSE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:10307 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/rfx2_20261016/RESULT_rfx2.md |  | verify/rfx2_20261016/RESULT_rfx2.md |
| MASHED_FX_DEBUG | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:9178 | 1 | value (parsed) | d | no owner, no default comment; named in verify/fe_residuals_20261016/fx/RESULT_FX.md |  | verify/fe_residuals_20261016/fx/RESULT_FX.md |
| MASHED_FX_RAWSPEED | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:9106 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/fe_residuals_20261016/fx/RESULT_FX.md |  | verify/fe_residuals_20261016/fx/RESULT_FX.md |
| MASHED_GAME_LENGTH | mashedmod/src/mashed_re/exe_main.cpp:2152 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_GOTO | mashedmod/src/mashed_re/exe_main.cpp:9985 | 1 | value (parsed) | d | no owner, no default comment; named in verify/aiveh_20261026/RESULT_D3-AI-VEHICLES.md |  | verify/aiveh_20261026/RESULT_D3-AI-VEHICLES.md |
| MASHED_HIDE_SHADOW | mashedmod/src/qol_asi/mashed_qol.cpp:1262 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_HOOK_ALLOW_BOOTPATCH | mashedmod/src/mashed_re/Core/HookSystem.cpp:110 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/savelink2_20261023/PREREG_settingsconfig_v2.md |  | verify/savelink2_20261023/PREREG_settingsconfig_v2.md |
| MASHED_HOOK_HI | mashedmod/src/mashed_re/Core/HookSystem.cpp:229 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_HOOK_LANE2 | mashedmod/src/mashed_re/Core/HookSystem.cpp:252 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/d4wsbportbroad1_20261029/PREREG_D4-WSB-PORT-BROAD-1.md |  | verify/d4wsbportbroad1_20261029/PREREG_D4-WSB-PORT-BROAD-1.md |
| MASHED_HOOK_LO | mashedmod/src/mashed_re/Core/HookSystem.cpp:228 | 1 | value (parsed) | d | no owner, no default comment; named in verify/d3_gatefire_20261008/RESULT_LEG1A.md |  | verify/d3_gatefire_20261008/RESULT_LEG1A.md |
| MASHED_HOOK_MANIFEST | mashedmod/src/mashed_re/Core/HookSystem.cpp:235 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/dupinstall_20261016/RESULT_dupinstall.md |  | verify/dupinstall_20261016/RESULT_dupinstall.md |
| MASHED_HOOK_SKIP | mashedmod/src/mashed_re/Core/HookSystem.cpp:234 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/d3_gatefire_20261008/RESULT_LEG1A.md |  | verify/d3_gatefire_20261008/RESULT_LEG1A.md |
| MASHED_INTERP | mashedmod/src/qol_asi/mashed_qol.cpp:1495 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_INTERP_SHADOW | mashedmod/src/qol_asi/mashed_qol.cpp:699 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_INTERP_WHEELS | mashedmod/src/qol_asi/mashed_qol.cpp:708 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_INTRO_PRESS | mashedmod/src/mashed_re/exe_main.cpp:2923 | 1 | value (parsed) | d | no owner, no default comment; named in verify/intromovies_20261028/PREREG_D5-INTROMOVIES.md |  | verify/intromovies_20261028/PREREG_D5-INTROMOVIES.md |
| MASHED_JUMPFIX | mashedmod/src/qol_asi/mashed_qol.cpp:1496 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_L1B_INPUTS | mashedmod/src/mashed_re/Race/SubStateCallees.cpp:471 | 2 | OFF (unset -> off) | d | no owner, no default comment; named in verify/d3_gatefire_20261008/PREREG_LEG1B.md |  | verify/d3_gatefire_20261008/PREREG_LEG1B.md |
| MASHED_LAPS | mashedmod/src/mashed_re/exe_main.cpp:2168 | 1 | value (parsed) | d | no owner, no default comment; named in verify/d4dc77b0lap_20261030/RESULT_D4-DC-77b0-LAP.md |  | verify/d4dc77b0lap_20261030/RESULT_D4-DC-77b0-LAP.md |
| MASHED_LIBRW_AFTERWORLDORDER | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:9378 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/rstates_20261016/PREREG_rstates.md |  | verify/rstates_20261016/PREREG_rstates.md |
| MASHED_LIBRW_BINMESHORDER | mashedmod/src/mashed_re/Track/DffModel.cpp:436 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/rstates_20261016/PREREG_rstates.md |  | verify/rstates_20261016/PREREG_rstates.md |
| MASHED_LIBRW_CLUMPORDER | mashedmod/src/mashed_re/LibRw/RwRaceSubmit.cpp:994 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/rstates_20261016/PREREG_rstates.md |  | verify/rstates_20261016/PREREG_rstates.md |
| MASHED_LIBRW_DUAL | mashedmod/src/mashed_re/LibRw/RwSceneBuild.cpp:52 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/d1shadows_20261024/PREREG_D1-SHADOWS.md |  | verify/d1shadows_20261024/PREREG_D1-SHADOWS.md |
| MASHED_LIBRW_LIFT | mashedmod/src/mashed_re/LibRw/RwRaceSubmit.cpp:1716 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_LIBRW_LINEAR | mashedmod/src/mashed_re/LibRw/RwRasterBridge.cpp:289 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_LIBRW_NODRAW | mashedmod/src/mashed_re/LibRw/RwRaceSubmit.cpp:1479 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_LIBRW_NOMIP | mashedmod/src/mashed_re/LibRw/RwRasterBridge.cpp:103 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_LIBRW_PAIRORDER | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:10059 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/rstates_20261016/PREREG_rstates.md |  | verify/rstates_20261016/PREREG_rstates.md |
| MASHED_LIBRW_RAINCULL | mashedmod/src/mashed_re/LibRw/RwRaceSubmit.cpp:993 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/rstates_20261016/PREREG_rstates.md |  | verify/rstates_20261016/PREREG_rstates.md |
| MASHED_LIBRW_SECTORWALK | mashedmod/src/mashed_re/LibRw/RwRaceSubmit.cpp:991 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/rstates_20261016/PREREG_rstates.md |  | verify/rstates_20261016/PREREG_rstates.md |
| MASHED_LIBRW_SMOKE | mashedmod/src/mashed_re/LibRw/RwBridge.cpp:89 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_LIBRW_STICKYVA | mashedmod/src/mashed_re/LibRw/RwRaceSubmit.cpp:992 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/rstates_20261016/PREREG_rstates.md |  | verify/rstates_20261016/PREREG_rstates.md |
| MASHED_LIBRW_SUPERG_VA | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:3565 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/rres_20261016/PREREG_rres.md |  | verify/rres_20261016/PREREG_rres.md |
| MASHED_MISSILE_FORCE | mashedmod/src/mashed_re/Powerup/PowerupMissile.cpp:39 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_MODE7_PC | mashedmod/src/mashed_re/Ai/AiTargetClosest.cpp:420 | 2 | OFF (unset -> off) | d | no owner, no default comment; named in verify/d3_mode7_m4_20261010/PREREG_M4.md |  | verify/d3_mode7_m4_20261010/PREREG_M4.md |
| MASHED_MORTAR_FORCE | mashedmod/src/mashed_re/Powerup/PowerupMortar.cpp:28 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_MSG_IDS | mashedmod/src/mashed_re/exe_main.cpp:10066 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_MUSIC_NO_RACE_CDAUDIO | mashedmod/src/mashed_re/Audio/AudioEngine.cpp:437 | 1 | value (parsed) | d | no owner, no default comment; named in verify/wsjmusic_20261026/PREREG_D4-WSJ-MUSIC.md |  | verify/wsjmusic_20261026/PREREG_D4-WSJ-MUSIC.md |
| MASHED_NEGCTL | mashedmod/src/mashed_re/Harness/NegCtl.cpp:340 | 2 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_OBJBSP_ORDER | mashedmod/src/mashed_re/LibRw/RwRaceSubmit.cpp:452 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/rsky_20261016/RESULT_rsky.md |  | verify/rsky_20261016/RESULT_rsky.md |
| MASHED_OBJSHAD_PLANES | mashedmod/src/mashed_re/LibRw/RwRaceFx.cpp:1118 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_P3_MEASURE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:5353 | 1 | value (parsed) | d | no owner, no default comment; named in verify/d3_gatefire_20261008/PREREG_LEG2.md |  | verify/d3_gatefire_20261008/PREREG_LEG2.md |
| MASHED_PATHPROG_LATE | mashedmod/src/mashed_re/Race/PathProgStep.cpp:257 | 1 | ON (seeded "1") [corrected D5-KNOBS-INVENTORY-FIX; was OFF (unset -> off)] | d | no owner, no default comment; named in verify/cev_20261018/PREREG_cev.md |  | verify/cev_20261018/PREREG_cev.md |
| MASHED_PATHPROG_PC | mashedmod/src/mashed_re/Race/PathProgStep.cpp:173 | 2 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_PHYS_NOCONTACT | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:49 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_PHYS_PROF | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:48 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_PICKUPS_PC | mashedmod/src/mashed_re/Gameplay/PickupTick.cpp:201 | 3 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_PICKUPS_PREGO_NO_REFDIST | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:286 | 1 | OFF (set, not "0") | d | no owner, no default comment; named in verify/iconcube_20261028/PREREG3_D5-ICONCUBE-SPIN.md |  | verify/iconcube_20261028/PREREG3_D5-ICONCUBE-SPIN.md |
| MASHED_PLAYERS | mashedmod/src/mashed_re/exe_main.cpp:6221 | 1 | value (parsed) | d | no owner, no default comment; named in verify/fe_residuals_20261016/u9672/RESULT.md |  | verify/fe_residuals_20261016/u9672/RESULT.md |
| MASHED_PONTOON_ANIM | mashedmod/src/mashed_re/Track/WarzonePontoon.cpp:717 | 1 | ON already; knob forces ON a default-ON feature (WarzonePontoon.cpp:717) [corrected D5-KNOBS-INVENTORY-FIX; was OFF (unset -> off)] | d | no owner, no default comment; named in verify/pontoon_20261016/PREREG_pontoon.md |  | verify/pontoon_20261016/PREREG_pontoon.md |
| MASHED_PONTOON_CARRY | mashedmod/src/mashed_re/Track/WarzonePontoon.cpp:715 | 1 | ON already; knob forces ON a default-ON feature (WarzonePontoon.cpp:715) [corrected D5-KNOBS-INVENTORY-FIX; was OFF (unset -> off)] | d | no owner, no default comment; named in verify/dflip_20261017/PREREG_dflip.md |  | verify/dflip_20261017/PREREG_dflip.md |
| MASHED_POS_CONTACTS | mashedmod/src/mashed_re/Vehicle/PosModelKnobs.h:36 | 1 | ON (via PosModelAll, PosModelKnobs.h:36) [corrected D5-KNOBS-INVENTORY-FIX; was OFF (unset -> off)] | d | no owner, no default comment; named in verify/d2engage_20261016/PREREG_d2engage.md |  | verify/d2engage_20261016/PREREG_d2engage.md |
| MASHED_POS_MODEL | mashedmod/src/mashed_re/Vehicle/PosModelKnobs.h:31 | 1 | ON (seeded "1") [corrected D5-KNOBS-INVENTORY-FIX; was OFF (unset -> off)] | d | no owner, no default comment; named in verify/cev_20261018/PREREG_cev.md |  | verify/cev_20261018/PREREG_cev.md |
| MASHED_POS_ORDER | mashedmod/src/mashed_re/Vehicle/PosModelKnobs.h:35 | 1 | ON (via PosModelAll, PosModelKnobs.h:35) [corrected D5-KNOBS-INVENTORY-FIX; was OFF (unset -> off)] | d | no owner, no default comment; named in verify/d2engage_20261016/PREREG_d2engage.md |  | verify/d2engage_20261016/PREREG_d2engage.md |
| MASHED_POS_TRANS | mashedmod/src/mashed_re/Vehicle/PosModelKnobs.h:34 | 1 | ON (via PosModelAll, PosModelKnobs.h:34) [corrected D5-KNOBS-INVENTORY-FIX; was OFF (unset -> off)] | d | no owner, no default comment; named in verify/d2bsp_20261016/RESULT_d2bsp.md |  | verify/d2bsp_20261016/RESULT_d2bsp.md |
| MASHED_POS_WHEELS | mashedmod/src/mashed_re/Vehicle/PosModelKnobs.h:33 | 1 | ON (via PosModelAll, PosModelKnobs.h:33) [corrected D5-KNOBS-INVENTORY-FIX; was OFF (unset -> off)] | d | no owner, no default comment; named in verify/d2posmodel_20261016/PREREG_d2posmodel.md |  | verify/d2posmodel_20261016/PREREG_d2posmodel.md |
| MASHED_QHULL_OPT | mashedmod/src/mashed_re/Collision/b5b_qhull_selftest.cpp:27 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_QHULL_PC | mashedmod/src/mashed_re/Collision/b5b_qhull_selftest.cpp:103 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_R7_INPUT_PAD_PRESS | mashedmod/src/mashed_re/exe_main.cpp:2848 | 1 | value (parsed) | d | no owner, no default comment; named in verify/padcoopflip_20261026/PREREG_D3-CF-PADCOOP-FLIP.md |  | verify/padcoopflip_20261026/PREREG_D3-CF-PADCOOP-FLIP.md |
| MASHED_R7_INPUT_PRESS | mashedmod/src/mashed_re/exe_main.cpp:2847 | 1 | value (parsed) | d | no owner, no default comment; named in verify/padcoopflip_20261026/PREREG_D3-CF-PADCOOP-FLIP.md |  | verify/padcoopflip_20261026/PREREG_D3-CF-PADCOOP-FLIP.md |
| MASHED_REGISTRY_PC | mashedmod/src/mashed_re/Race/WorldRegistry.cpp:131 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_RENDER_PROF | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:9442 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_RFLAME_SORT | mashedmod/src/mashed_re/Powerup/PowerupEffects.cpp:380 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/d3rflame_20261024/PREREG_D3-CF-RFLAME.md |  | verify/d3rflame_20261024/PREREG_D3-CF-RFLAME.md |
| MASHED_RING_DBUF | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:543 | 1 | ON (seeded "1") [corrected D5-KNOBS-INVENTORY-FIX; was OFF (unset -> off)] | d | no owner, no default comment; named in verify/u9152_20261018/PREREG_u9152.md |  | verify/u9152_20261018/PREREG_u9152.md |
| MASHED_RING_DBUF_DISCARD | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:547 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/u9152_20261018/PREREG_u9152.md |  | verify/u9152_20261018/PREREG_u9152.md |
| MASHED_ROUND_COLOURS | mashedmod/src/mashed_re/exe_main.cpp:9768 | 1 | value (parsed) | d | no owner, no default comment; named in verify/librwrest_checks_20261016/PREREG_librwrest_checks.md |  | verify/librwrest_checks_20261016/PREREG_librwrest_checks.md |
| MASHED_ROUND_GRID_ORDER | mashedmod/src/mashed_re/Race/RoundGrid.cpp:239 | 1 | ON (seeded "1") [corrected D5-KNOBS-INVENTORY-FIX; was OFF (unset -> off)] | d | no owner, no default comment; named in verify/c3k44_20261017/PREREG_c3k44.md |  | verify/c3k44_20261017/PREREG_c3k44.md |
| MASHED_ROUND_SCORES | mashedmod/src/mashed_re/exe_main.cpp:4196 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_RULE_ENGINE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:7749 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/d3close_20261021/RESULT_d3close.md |  | verify/d3close_20261021/RESULT_d3close.md |
| MASHED_RW_WHEELPOS | mashedmod/src/mashed_re/Collision/WheelContactSolver.cpp:174 | 1 | ON (via PosModelWheels, WheelContactSolver.cpp:174-175) [corrected D5-KNOBS-INVENTORY-FIX; was OFF (unset -> off)] | d | no owner, no default comment; named in verify/d2posmodel_20261016/PREREG_d2posmodel.md |  | verify/d2posmodel_20261016/PREREG_d2posmodel.md |
| MASHED_SAVE_FIRSTRUN_WRITERS | mashedmod/src/mashed_re/Race/GameFlow.cpp:262 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/spanwriters_20261027/PREREG_D4-FU-SPANINIT-WRITERS.md |  | verify/spanwriters_20261027/PREREG_D4-FU-SPANINIT-WRITERS.md |
| MASHED_SAVE_PATH | mashedmod/src/mashed_re/Race/GameFlow.cpp:708 | 3 | OFF (unset -> off) | d | no owner, no default comment; named in verify/d4_save_20261005/PREREG_GAMESAVE_EXE.md |  | verify/d4_save_20261005/PREREG_GAMESAVE_EXE.md |
| MASHED_SAVE_RT_IN | mashedmod/src/mashed_re/Race/GameFlow.cpp:725 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/spanwriters_20261027/PREREG_D4-FU-SPANINIT-WRITERS.md |  | verify/spanwriters_20261027/PREREG_D4-FU-SPANINIT-WRITERS.md |
| MASHED_SAVE_RT_OUT | mashedmod/src/mashed_re/Race/GameFlow.cpp:707 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/wsfsave_20261026/PREREG_D4-WSF-SAVE-RT.md |  | verify/wsfsave_20261026/PREREG_D4-WSF-SAVE-RT.md |
| MASHED_SHADOW_NEED_PAGES | mashedmod/src/mashed_re/Core/ShadowTrack.h:768 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/wsbbroadws_20261027/PREREG_R2_D4-WSB-C4-BROAD-WS.md |  | verify/wsbbroadws_20261027/PREREG_R2_D4-WSB-C4-BROAD-WS.md |
| MASHED_SHADOW_PRIV_HI | mashedmod/src/mashed_re/Core/ShadowTrack.h:458 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_SHADOW_PRIV_LO | mashedmod/src/mashed_re/Core/ShadowTrack.h:457 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_SHADOW_STW | mashedmod/src/mashed_re/Core/ShadowTrack.h:629 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/d4wsbportbroad1_20261029/PREREG_D4-WSB-PORT-BROAD-1.md |  | verify/d4wsbportbroad1_20261029/PREREG_D4-WSB-PORT-BROAD-1.md |
| MASHED_SKY_D3D9 | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:9904 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/rprops_20261016/PREREG_rprops.md |  | verify/rprops_20261016/PREREG_rprops.md |
| MASHED_SPAWN_BASIS_RW | mashedmod/src/mashed_re/Vehicle/BodyOrientationIntegrate.cpp:136 | 1 | ON (seeded "1") [corrected D5-KNOBS-INVENTORY-FIX; was OFF (unset -> off)] | d | no owner, no default comment; named in verify/dc4d20grid_20261025/PREREG_D4-DC-4d20-GRID.md |  | verify/dc4d20grid_20261025/PREREG_D4-DC-4d20-GRID.md |
| MASHED_SPEED | mashedmod/src/qol_asi/mashed_qol.cpp:317 | 2 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_STEER_ACOS_CRT | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:274 | 1 | ON (seeded "1") [corrected D5-KNOBS-INVENTORY-FIX; was OFF (set, not "0")] | d | no owner, no default comment; named in verify/d3bc_20261019/PREREG_d3bc.md |  | verify/d3bc_20261019/PREREG_d3bc.md |
| MASHED_STEER_AXIS_TERRAIN | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:705 | 1 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_SUBSTEP_OUTER_ORDER | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:2648 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/soflip_20261021/PREREG_soflip.md |  | verify/soflip_20261021/PREREG_soflip.md |
| MASHED_SUPERG_REFLECT_FIRST | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:2937 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/rres_20261016/RESULT_rres.md |  | verify/rres_20261016/RESULT_rres.md |
| MASHED_TEAMS | mashedmod/src/mashed_re/exe_main.cpp:9824 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_TEAM_KEYS | mashedmod/src/mashed_re/exe_main.cpp:6322 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_TEAM_PLAY | mashedmod/src/mashed_re/exe_main.cpp:9955 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/d3_gatefire_20261008/RESULT_LEG0.md |  | verify/d3_gatefire_20261008/RESULT_LEG0.md |
| MASHED_TERRAIN_NOLIGHT | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:801 | 2 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_TEXANIM_MASKTEX | mashedmod/src/mashed_re/LibRw/RwRaceSubmit.cpp:558 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/hbtexanimrefs_20261027/PREREG2.md |  | verify/hbtexanimrefs_20261027/PREREG2.md |
| MASHED_TGT3_PC | mashedmod/src/mashed_re/Ai/AiTargetClosest.cpp:245 | 2 | OFF (unset -> off) | d | no owner, no default comment |  |  |
| MASHED_THROTTLE_K | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:5220 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_U9186_GATES | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:6694 | 1 | OFF (unset -> off) | d | no owner, no default comment; named in verify/d3_bscore_20261009/PREREG_B2C.md |  | verify/d3_bscore_20261009/PREREG_B2C.md |
| MASHED_WORLDVEL | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:689 | 1 | value (parsed) | d | no owner, no default comment |  |  |
| MASHED_A364_RESET | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:75 | 0 | n/a | x | comment/define only, no env read | NEXT_SESSION.md:953 | verify/d3_b2flip_20261009/PREREG_B2FLIP.md |
| MASHED_A5_RACE_TIMER | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1689 | 0 | n/a | x | comment/define only, no env read | L-17(D3-CF-A5TIMER) L-18(D3-CF-COLLLUT) L-23(D3-CF-A5TIMER-FLIP) | verify/d2ref_20261017/PREREG_d2ref.md |
| MASHED_A5_RACE_TIMER_ADD | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:2231 | 0 | n/a | x | comment/define only, no env read | L-17(D3-CF-A5TIMER) L-18(D3-CF-COLLLUT) L-23(D3-CF-A5TIMER-FLIP) | verify/a5flip_20261026/RESULT_D3-CF-A5TIMER-FLIP.md |
| MASHED_A5_RACE_TIMER_RUN | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:2218 | 0 | n/a | x | comment/define only, no env read | L-23(D3-CF-A5TIMER-FLIP) ledger:D3-CF-A5TIMER-FLIP | verify/a5flip_20261026/RESULT_D3-CF-A5TIMER-FLIP.md |
| MASHED_A5_SUSP_LUT | mashedmod/src/mashed_re/Vehicle/ForceIntegrator.cpp:113 | 0 | n/a | x | comment/define only, no env read |  | verify/a6flip_20261019/PREREG_a6flip.md |
| MASHED_A6A_F32 | mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:198 | 0 | n/a | x | comment/define only, no env read |  | verify/a6flip_20261019/PREREG_a6flip.md |
| MASHED_A6A_LUT | mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:56 | 0 | n/a | x | comment/define only, no env read |  | verify/a6flip_20261019/PREREG_a6flip.md |
| MASHED_AI_LEADER_SELFTEST | mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp:32 | 0 | n/a | x | comment/define only, no env read |  |  |
| MASHED_AI_TICK | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:5622 | 0 | n/a | x | comment/define only, no env read | ROADMAP.md:2227 | verify/d3close_20261021/PREREG_d3close.md |
| MASHED_ALIGNRATE | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:2298 | 0 | n/a | x | comment/define only, no env read |  |  |
| MASHED_BOX_PRODUCERS | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:200 | 0 | n/a | x | comment/define only, no env read | L-11(D3-CF-BOX) ledger:D3-CF-BOX-FLIP | verify/d3cfboxflip_20261025/RESULT_D3-CF-BOX-FLIP.md |
| MASHED_CAM_ROTATE_RW | mashedmod/src/mashed_re/Race/RaceCamera.cpp:80 | 0 | n/a | x | comment/define only, no env read | L-15(D4-DC-MATH-4d20) | verify/dcl13_20261025/RESULT_dcl13.md |
| MASHED_CC_MATRIX_INVERT | mashedmod/src/mashed_re/Collision/ContactStubs.cpp:166 | 0 | n/a | x | comment/define only, no env read |  | verify/ccinv_20261019/PREREG_ccinv.md |
| MASHED_CHAINSCALE | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:2297 | 0 | n/a | x | comment/define only, no env read |  |  |
| MASHED_CHASE_FOG | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:9957 | 0 | n/a | x | comment/define only, no env read |  |  |
| MASHED_CHASE_RIG | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:9600 | 0 | n/a | x | comment/define only, no env read |  | verify/d1r2_20261016/PREREG_d1r2.md |
| MASHED_COUNTDOWN_SPAWN | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:2089 | 0 | n/a | x | comment/define only, no env read |  | verify/u9747_20261018/PREREG_u9747.md |
| MASHED_D2BSP | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:1774 | 0 | n/a | x | comment/define only, no env read |  | verify/d2bsp_20261016/PREREG_d2bsp.md |
| MASHED_D2CHUNK | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1120 | 0 | n/a | x | comment/define only, no env read |  | verify/d2bsp_20261016/PREREG_d2bsp.md |
| MASHED_D2CLS | mashedmod/src/mashed_re/Collision/CarWorldContacts.cpp:36 | 0 | n/a | x | comment/define only, no env read |  | verify/d2bsp_20261016/PREREG_d2bsp.md |
| MASHED_D2LAT_REFRESH | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:1682 | 0 | n/a | x | comment/define only, no env read | ROADMAP.md:644 | verify/c2k93_20261017/PREREG_c2k93.md |
| MASHED_D2_BOOSTHOLD | mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:518 | 0 | n/a | x | comment/define only, no env read | ROADMAP.md:927 | verify/d2_sched_20261001/PREREG_STEP2C.md |
| MASHED_DAT_005 | mashedmod/src/mashed_re/Collision/RwpVtableKV1.cpp:100 | 0 | n/a | x | comment/define only, no env read |  |  |
| MASHED_DAT_007 | mashedmod/src/mashed_re/Collision/RwpBuildExterns.cpp:70 | 0 | n/a | x | comment/define only, no env read |  |  |
| MASHED_DEAD_COAST | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:6070 | 0 | n/a | x | comment/define only, no env read | L-34(D4-FX-INPUTS) ledger:D4-FX-FLIP-DEADCOAST | verify/fxflip_20261027/RESULT_D4-FX-FLIP-DEADCOAST.md |
| MASHED_ELIM_RESOLVE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:8080 | 0 | n/a | x | comment/define only, no env read | L-4(D3-CF-U9779) | verify/d3cfbox_20261025/PREREG_D3-CF-BOX.md |
| MASHED_FIX_5CEA60 | mashedmod/src/mashed_re/Collision/WheelContactSolver.cpp:59 | 0 | n/a | x | comment/define only, no env read |  | verify/faithflip_20261020/PREREG_faithflip.md |
| MASHED_FLIP_D11077 | mashedmod/src/mashed_re/Core/FaithDefaults.cpp:25 | 0 | n/a | x | comment/define only, no env read | L-62(D5-PARTMORPH-FLIPPREP) ledger:D5-PARTMORPH-MEASURE [open: D5-PARTMORPH-MEASURE] | verify/d5partmorphflip_20261030/PREREG_D5-PARTMORPH-FLIPPREP.md |
| MASHED_FWD_AT | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:524 | 0 | n/a | x | comment/define only, no env read |  | verify/faithflip_20261020/PREREG_faithflip.md |
| MASHED_FX_ORIG_GATES | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:9208 | 0 | n/a | x | comment/define only, no env read | L-25(D4-FX-GATES) ledger:D4-FX-GATES-FLIP | verify/fxgates_20261026/PREREG_fxgates.md |
| MASHED_GRID_DIR | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:5018 | 0 | n/a | x | comment/define only, no env read | ROADMAP.md:644 | verify/a4order_20261017/PREREG_a4order.md |
| MASHED_HEADING_ASSIST_PLAYER | mashedmod/src/mashed_re/Race/HeadingAssist.cpp:227 | 0 | n/a | x | comment/define only, no env read |  | verify/u9749_20261017/PREREG_u9749.md |
| MASHED_INTERP_REGISTRY | mashedmod/src/qol_asi/mashed_qol.cpp:1104 | 0 | n/a | x | comment/define only, no env read |  |  |
| MASHED_KILL_DAMAGE | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:949 | 0 | n/a | x | comment/define only, no env read | L-34(D4-FX-INPUTS) ledger:D4-FX-FLIP-DEADCOAST | verify/fxflip_20261027/RESULT_D4-FX-FLIP-DEADCOAST.md |
| MASHED_LEAVES1_X87 | mashedmod/src/mashed_re/Collision/RwpSolverLeaves1.cpp:123 | 0 | n/a | x | comment/define only, no env read | L-37(D4-WSB-NARROW-FIX2) ledger:D4-WSB-NARROW-FLIP | verify/hb6_20261026/RESULT_HB-6.md |
| MASHED_MENU_ANIM_SPEED | mashedmod/src/mashed_re/Frontend/MenuNavSM.cpp:781 | 0 | n/a | x | comment/define only, no env read | L-35(D4-FU-MENUANIM) L-36(D4-FU-MENUSPEED) ledger:D4-FU-MENUSPEED | verify/menuanimspeed_20261027/RESULT_D5-MENUANIMSPEED.md |
| MASHED_MENU_CLOCK_FAITHFUL | mashedmod/src/mashed_re/Frontend/MenuNavSM.cpp:782 | 0 | n/a | x | comment/define only, no env read | L-36(D4-FU-MENUSPEED) ledger:D4-FU-MENUCLOCK-FLIP ledger:D5-MENUANIMSPEED | verify/menuclockflip_20261027/PREREG_D4-FU-MENUCLOCK-FLIP.md |
| MASHED_NARROW_DOT_ORDER | mashedmod/src/mashed_re/Collision/NarrowFixKnobs.h:5 | 0 | n/a | x | comment/define only, no env read | L-37(D4-WSB-NARROW-FIX2) ledger:D4-WSB-NARROW-FIX2 ledger:D4-WSB-NARROW-FLIP | verify/wsbbvhfu_20261027/PREREG_D4-WSB-BVH-FU.md |
| MASHED_NARROW_PAIR_INT | mashedmod/src/mashed_re/Collision/NarrowFixKnobs.h:5 | 0 | n/a | x | comment/define only, no env read | L-37(D4-WSB-NARROW-FIX2) ledger:D4-WSB-NARROW-FLIP | verify/wsbbvhfu_20261027/PREREG_D4-WSB-BVH-FU.md |
| MASHED_NO_A5_RUBBER_LIVE | mashedmod/src/mashed_re/Vehicle/ForceIntegratorLeaves.cpp:35 | 0 | n/a | x | comment/define only, no env read |  | verify/d3close_20261021/PREREG_d3close.md |
| MASHED_NO_A6A_DOT_ORDER | mashedmod/src/mashed_re/Vehicle/Integrate2.cpp:232 | 0 | n/a | x | comment/define only, no env read | ROADMAP.md:2981 NEXT_SESSION.md:498 | verify/dcphys_20261021/PREREG_dcphys.md |
| MASHED_NO_CAR_STATE0 | mashedmod/src/mashed_re/Core/FaithDefaults.cpp:70 | 0 | n/a | x | comment/define only, no env read | L-62(D5-PARTMORPH-FLIPPREP) ledger:D5-PARTMORPH-FLIPPREP | verify/d5partmorphflip_20261030/PREREG_D5-PARTMORPH-FLIPPREP.md |
| MASHED_NO_ORTHO_LUT | mashedmod/src/mashed_re/Vehicle/BodyOrientationIntegrate.cpp:105 | 0 | n/a | x | comment/define only, no env read | L-14(D4-DC-MATH-4680) | verify/dcl13_20261025/RESULT_dcl13.md |
| MASHED_NO_P3_RESET_PAIR | mashedmod/src/mashed_re/Core/FaithDefaults.cpp:65 | 0 | n/a | x | comment/define only, no env read |  | verify/d3subflip_20261028/PREREG_D3-SUBSTATE-FLIP.md |
| MASHED_NO_PARTMORPH | mashedmod/src/mashed_re/Core/FaithDefaults.cpp:70 | 0 | n/a | x | comment/define only, no env read | L-62(D5-PARTMORPH-FLIPPREP) ledger:D5-PARTMORPH-FLIPPREP | verify/d5partmorphflip_20261030/PREREG_D5-PARTMORPH-FLIPPREP.md |
| MASHED_NO_RACEPCT_SLOT | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:7921 | 0 | n/a | x | comment/define only, no env read | ledger:D4-DC-8ad0 NEXT_SESSION.md:233 | verify/d4dc8ad0_20261025/RESULT_D4-DC-8ad0.md |
| MASHED_NO_SNAP_POST | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:1694 | 0 | n/a | x | comment/define only, no env read | L-46(HB-FULLCHECK-R17) | verify/hbfullcheck_20261028/RESULT_HB-FULLCHECK-R17.md |
| MASHED_NO_SUBSTATE | mashedmod/src/mashed_re/Core/FaithDefaults.cpp:65 | 0 | n/a | x | comment/define only, no env read |  | verify/d3subflip_20261028/PREREG_D3-SUBSTATE-FLIP.md |
| MASHED_NO_WCS_COMBINE_PC24 | mashedmod/src/mashed_re/Collision/WheelContactSolver.cpp:441 | 0 | n/a | x | comment/define only, no env read |  | verify/cpcflip_20261020/PREREG_cpcflip.md |
| MASHED_NO_WCS_TRINORMAL | mashedmod/src/mashed_re/Collision/WheelContactSolver.cpp:84 | 0 | n/a | x | comment/define only, no env read |  | verify/trinflip_20261020/PREREG_trinflip.md |
| MASHED_ORTHO_LUT | mashedmod/src/mashed_re/Math/RwMatrixOrthoNormalize.cpp:4 | 0 | n/a | x | comment/define only, no env read | L-14(D4-DC-MATH-4680) ledger:D4-DC-MATH-4680 | verify/dcl13_20261025/RESULT_dcl13.md |
| MASHED_OWN_RAND_564310 | mashedmod/src/mashed_re/Collision/RwpSolverLeaves1.cpp:53 | 0 | n/a | x | comment/define only, no env read | L-47(D4-WSB-RAND-D11064) ledger:D4-WSB-RAND-FLIP | verify/d4wsb_randflip_20261028/RESULT_D4-WSB-RAND-FLIP.md |
| MASHED_PAD_COOP_ORIG | mashedmod/src/mashed_re/exe_main.cpp:1046 | 0 | n/a | x | comment/define only, no env read | L-20(D3-CF-PADCOOP-FLIP) ledger:D3-CF-PADCOOP-FLIP | verify/padcoopflip_20261026/PREREG_D3-CF-PADCOOP-FLIP.md |
| MASHED_PATCHES | mashedmod/src/mashed_re/LibRw/RwBridge.cpp:316 | 0 | n/a | x | comment/define only, no env read | ROADMAP.md:3061 | verify/d1r1_20261016/RESULT_d1r1.md |
| MASHED_PHYS_DIAG | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:28 | 0 | n/a | x | comment/define only, no env read |  |  |
| MASHED_PONTOON_ | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:20 | 0 | n/a | x | comment/define only, no env read |  |  |
| MASHED_PTR_00624058 | mashedmod/src/mashed_re/Collision/RwpVtableKV2.cpp:68 | 0 | n/a | x | comment/define only, no env read |  |  |
| MASHED_QOL | mashedmod/src/qol_asi/mashed_qol.cpp:3 | 0 | n/a | x | comment/define only, no env read |  |  |
| MASHED_R7_EXIT | mashedmod/src/mashed_re/exe_main.cpp:1051 | 0 | n/a | x | comment/define only, no env read | L-10(D3-CF-K357) L-12(D3-CF-R7INPUT) L-16(D3-CF-R7JOYPAD) | verify/k357_20261025/PREREG_D3-CF-K357.md |
| MASHED_R7_INPUT | mashedmod/src/mashed_re/exe_main.cpp:1038 | 0 | n/a | x | comment/define only, no env read | L-12(D3-CF-R7INPUT) L-16(D3-CF-R7JOYPAD) ledger:D3-CF-R7JOYPAD | verify/padcoop_20261025/PREREG_D3-CF-PADCOOP.md |
| MASHED_R7_PHYS | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:1537 | 0 | n/a | x | comment/define only, no env read | L-4(D3-CF-U9779) | verify/fxinputs_20261026/PREREG_deadcoast.md |
| MASHED_RACEPCT_BRIDGE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:8770 | 0 | n/a | x | comment/define only, no env read | NEXT_SESSION.md:1621 | verify/d3_b2flip_20261009/PREREG_B2FLIP.md |
| MASHED_RACEPCT_FUN408610 | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:8772 | 0 | n/a | x | comment/define only, no env read |  | verify/d3_u9685_20261016/PREREG_u9685.md |
| MASHED_RACE_TAIL_LATE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:184 | 0 | n/a | x | comment/define only, no env read | L-4(D3-CF-U9779) | verify/substep_20261021/RESULT_substep.md |
| MASHED_REAL_AI | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:103 | 0 | n/a | x | comment/define only, no env read | ROADMAP.md:2299 |  |
| MASHED_REFDIST | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:8729 | 0 | n/a | x | comment/define only, no env read | NEXT_SESSION.md:960 | verify/d3_b2flip_20261009/PREREG_B2FLIP.md |
| MASHED_REFDIST_AFTER_AI | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:6675 | 0 | n/a | x | comment/define only, no env read |  | verify/d3_u9683_20261016/PREREG_u9683.md |
| MASHED_RENDER_LIBRW | mashedmod/src/mashed_re/exe_main.cpp:9488 | 0 | n/a | x | comment/define only, no env read | ROADMAP.md:43 | verify/city_blackroad/RESULT.md |
| MASHED_RE_COLLISION_BUILD_DEPS_H | mashedmod/src/mashed_re/Collision/CollisionBuildDeps.h:14 | 0 | n/a | x | comment/define only, no env read |  |  |
| MASHED_RE_FRONTEND_MENUNAVSM_H | mashedmod/src/mashed_re/Frontend/MenuNavSM.h:12 | 0 | n/a | x | comment/define only, no env read |  |  |
| MASHED_RE_FRONTEND_MENUNAV_H | mashedmod/src/mashed_re/Frontend/MenuNav.h:6 | 0 | n/a | x | comment/define only, no env read |  |  |
| MASHED_ROUND_END7 | mashedmod/src/mashed_re/exe_main.cpp:2792 | 0 | n/a | x | comment/define only, no env read | L-4(D3-CF-U9779) ledger:D3-CF-U9779 | verify/d3cfbox_20261025/PREREG_D3-CF-BOX.md |
| MASHED_RW_SSE_POINTS | mashedmod/src/mashed_re/Math/RwV3dTransformPointsCPU.cpp:58 | 0 | n/a | x | comment/define only, no env read |  | verify/faithflip_20261020/PREREG_faithflip.md |
| MASHED_SLOTSTATE_SEED | mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp:99 | 0 | n/a | x | comment/define only, no env read | NEXT_SESSION.md:1165 | verify/d3_gatefire_20261008/PREREG_H2.md |
| MASHED_SLOT_PLAYER | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:1135 | 0 | n/a | x | comment/define only, no env read | NEXT_SESSION.md:1096 | verify/d3_gatefire_20261008/PREREG_H2.md |
| MASHED_SNAP_POST | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:1693 | 0 | n/a | x | comment/define only, no env read | L-43(D3-CF-SNAP) L-46(HB-FULLCHECK-R17) ledger:D3-CF-SNAP-FLIP | verify/hbfullcheck_20261028/RESULT_HB-FULLCHECK-R17.md |
| MASHED_SPAWN_Y_LINE | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:1554 | 0 | n/a | x | comment/define only, no env read |  | verify/u9755_20261018/PREREG_u9755.md |
| MASHED_STANDALONE | mashedmod/src/mashed_re/Ai/AiBankSwitch.cpp:54 | 0 | n/a | x | comment/define only, no env read | ROADMAP.md:2958 NEXT_SESSION.md:36 | verify/audiorest_20261024/RESULT_D4-AUDIO-REST.md |
| MASHED_STEER_ACOS64 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:191 | 0 | n/a | x | comment/define only, no env read |  | verify/d3bc_20261019/PREREG_d3bc.md |
| MASHED_SUBSTEP_INTERLEAVE | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.h:156 | 0 | n/a | x | comment/define only, no env read |  | verify/brecipe_20261021/PREREG_brecipe.md |
| MASHED_TEXANIM | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:1592 | 0 | n/a | x | comment/define only, no env read | L-39(HB-TEXANIM-FLIP) L-42(HB-TEXANIM-REFS) ledger:HB-TEXANIM-FLIP | verify/hbforestwater_20261027/PREREG.md |
| MASHED_TOPSPEED | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:2298 | 0 | n/a | x | comment/define only, no env read |  |  |
| MASHED_VOLTODS_LOG | mashedmod/src/mashed_re/Audio/AudioEngine.cpp:98 | 0 | n/a | x | comment/define only, no env read |  | verify/audiovoltodsflip_20261024/PREREG_voltodsflip.md |
| MASHED_VOLTODS_ORIG | mashedmod/src/mashed_re/Audio/AudioEngine.cpp:98 | 0 | n/a | x | comment/define only, no env read | L-6(D4-AUDIO-VOLTODS) ledger:D4-AUDIO-VOLTODS-FLIP | verify/audiovoltodsflip_20261024/PREREG_voltodsflip.md |
| MASHED_WIRE_B2 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp:951 | 0 | n/a | x | comment/define only, no env read | NEXT_SESSION.md:1096 | verify/d3_b2flip_20261009/PREREG_B2FLIP.md |
