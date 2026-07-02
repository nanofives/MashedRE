# R6 demand map — 2026-07-01

Branch: `ws-r6-ai-control-chain`. Generated 2026-07-02 by a single scripted pass (`build_demand_map.py`, session scratchpad) over pre-extracted inputs; no Ghidra, no Frida, no game process.

**Method.** Demand roots are every RVA citation extracted from the ported race-slice sources `mashedmod/src/mashed_re/Race/` and `mashedmod/src/mashed_re/Ai/` (Race/tests/ excluded). Root RVAs present in `hooks.csv` and of kind ported-function / call-original / other were used as BFS seeds over a static call graph union-merged from the `callees` columns of `re/analysis/plans/*.tsv` (36 files with a callees column; 2,711 raw nodes, 13,337 edges). BFS is unlimited-depth; for every reached RVA the script records depth (roots = 0), first caller (BFS discovery order), and in-degree = number of DISTINCT callers inside roots ∪ closure (self-loops excluded). Closure members are annotated from `hooks.csv`; library code (subsystem `third-party-library*` or the four known address bands) is excluded from gap RANKING only, not from closure totals. hooks.csv contains 13 RVAs with two rows each (after +0x400000 normalization of the file-offset rows); for those, the FIRST hooks.csv occurrence wins the name/subsystem annotation — the four such RVAs inside the closure are 0x00495280 (FUN_00495280/render, alt OpenPizFile/util), 0x004952f0 (FUN_004952f0/render, alt ClosePizFile/util), 0x004b6570 (PizOpen/util, alt FUN_004b6570/render), 0x004b6940 (PizOpenAndParse/util, alt FUN_004b6940/render), all C2 in both rows. Gap RVAs that appear as an "RVA called" row in `STUBS.md` are flagged — those are direct call-original demand from the ported sources, the strongest static signal.

**Disclaimer (stated plainly): NO per-RVA in-race call-frequency dataset exists in this repo — the ranking below uses STATIC call-graph in-degree as a proxy, computed from the merged callees columns of `re/analysis/plans/*.tsv`; it is not measured runtime frequency.** See section 5 for the full dynamic-evidence inventory.

### Evidence sources

| Input | Exact path(s) | Role |
|---|---|---|
| Ported race-slice sources (roots) | `mashedmod/src/mashed_re/Race/*.cpp/.h`, `mashedmod/src/mashed_re/Ai/*.cpp/.h` | RVA citations = demand roots |
| Function tracker | `hooks.csv` (5,864 data rows → 5,851 unique RVAs; 21 file-offset rows normalized +0x400000) | name/subsystem/confidence/status/file annotation |
| Stub tracker | `STUBS.md` (Active-stubs table: 1,100 data rows, of which 1,087 parsed — the 13 malformed 7-cell short rows were excluded by the 9-cell pipe-shape parse) | direct call-original demand flag |
| Static call graph | `re/analysis/plans/*.tsv` callees columns (36 files, e.g. `c3_survey_passed.tsv`, `c3_batch_ah_probe_passed.tsv`, `loop_round_31_passed.tsv`, `c3_measure_core_passed.tsv`) | BFS edges |
| Dynamic artifacts (inventory only) | `re/scenarios/00{1,2,3,4}-*/coverage.tsv`, `re/analysis/phys_c4_evidence/COVERAGE_SCENARIO_FINDINGS_2026-06-17.md`, `log/auto_count_at_menu*.txt`, `log/install_observe_*.txt`, `log/handling_telemetry.jsonl`, `re/analysis/plans/mass_observe_r4_curation.md` | section 5 |
| Intermediates | session scratchpad `roots_race.json`, `roots_ai_a.json`, `roots_ai_b.json`, `hooks_map.json`, `stubs.json`, `callgraph.json` | machine-readable staging of the above |

Input anomalies handled mechanically: 10 call-graph node keys and 0 callee values below the 0x00400000 image base were normalized by +0x400000 (same convention as the 21 hooks.csv file-offset rows); 10 resulting self-loop edges were dropped for in-degree purposes.

## 1. Demand roots (by source file)

Root citations: 664 entries across 24 source files; 116 distinct function-root RVAs (BFS seeds), 277 data-global citations (170 distinct RVAs), 137 unmapped citations (128 distinct RVAs).

### `mashedmod/src/mashed_re/Ai/AiController.cpp` — 25 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x00407a40 | call-original | Table8a9640Get | C3 | per-vehicle data getter call-through |
| 0x0040e350 | call-original | GetRenderSubMode | C3 | sub-game-mode getter call-through |
| 0x0040e470 | call-original | CarSlotStateGet | C3 | vehicle-type getter call-through |
| 0x0040e480 | call-original | CarSlotStateSet | C3 | CarSlotStateSet call-through |
| 0x0040e4a0 | call-original | ElapsedTimeGet | C3 | elapsed-time getter call-through |
| 0x00413fe0 | call-original | FUN_00413fe0 | C3 | AI state reset call-through |
| 0x00416250 | call-original | FUN_00416250 | C2 | primary control step called via RVA (call_00416250, arg4=0x42c80000) |
| 0x00416a30 | call-original | FUN_00416a30 | C2 | mode-4/9 control-step variant called via RVA |
| 0x00417180 | call-original | FUN_00417180 | C2 | bank switcher called via RVA |
| 0x00417640 | call-original | FUN_00417640 | C2 | post-step powerup-brake called via own RVA (diff-toggle transparent; also ported here) |
| 0x00417640 | ported-function | FUN_00417640 | C2 | AiPostStepPowerupBrake verbatim port; RH_ScopedInstall L139 |
| 0x004177b0 | call-original | FUN_004177b0 | C2 | pre-tick rubber-band called via RVA |
| 0x00417da0 | call-original | FUN_00417da0 | C2 | mode-8 control-step variant called via RVA |
| 0x00418560 | call-original | FUN_00418560 | C2 | per-vehicle step called via own RVA from AiTickLoop (also ported here) |
| 0x00418560 | ported-function | FUN_00418560 | C2 | AiVehicleStep verbatim port; RH_ScopedInstall L294 |
| 0x00418860 | ported-function | FUN_00418860 | C2 | AiTickLoop verbatim port; RH_ScopedInstall L336 |
| 0x00426c00 | call-original | FUN_00426c00 | C3 | track-index getter call-through (0x21 gate) |
| 0x0042f6a0 | call-original | GetRaceSubMode | C3 | round-type getter call-through |
| 0x00443080 | call-original | AiTargetEnableGet | C3 | AI-targeting enable call-through |
| 0x00452160 | call-original | PowerupTargetPtrGet | C3 | powerup position ptr getter call-through |
| 0x00452ea0 | call-original | Table88ff50Get | C3 | powerup predicate call-through |
| 0x00452eb0 | call-original | PowerupRangeGet | C3 | held-type float getter call-through |
| 0x0046c7b0 | call-original | VehicleSlotGetter | C3 | car-alive getter call-through |
| 0x0046d4a0 | call-original | PtrCompute881ec8 | C3 | own-struct-ptr getter call-through |
| 0x0046d570 | call-original | FUN_0046d570 | C2 | range-like float writer call-through |
| 0x0046d6d0 | call-original | VehTbl881f84Get1 | C3 | rate-like float writer call-through |
| 0x004c3ac0 | call-original | Vec3Magnitude | C4 | vector length call-through (return discarded, as in original) |

### `mashedmod/src/mashed_re/Ai/AiData.h` — 5 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x00418560 | other | FUN_00418560 | C2 | citing function for spline stride/count offsets |
| 0x00418860 | other | FUN_00418860 | C2 | controller that indexes this .AI format |
| 0x00423540 | other | FUN_00423540 | C2 | RW chunk writer (payload size arg 0x11884) |
| 0x004235b0 | other | FUN_004235b0 | C3 | original .AI loader used by dev .asi (RW chunk reader; tag check) |
| 0x004cc5e0 | other | FUN_004cc5e0 | C2 | chunk-tag helper cited with reader (0x13269902 tag) |

### `mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp` — 6 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x0040e470 | call-original | CarSlotStateGet | C3 | FUN_0040e470 active-vehicle test forwarded via fn-ptr cast line 69 |
| 0x004148b0 | ported-function | FUN_004148b0 | C3 | FUN_004148b0 AI last-place catch-up (leader) timer, verbatim port; RH_ScopedInstall(AiLeader_Entry) line 235 |
| 0x00416250 | ported-function | FUN_00416250 | C2 | FUN_00416250 orchestrator entry; RH_ScopedInstall(OrchLeaderCoverage_Entry) line 236 = coverage driver, NOT a reimpl |
| 0x00442cc0 | call-original | AiVehicleFloat4Get | C3 | FUN_00442cc0 per-vehicle progress getter (float10->float) forwarded via fn-ptr cast line 70 |
| 0x0046d4a0 | call-original | PtrCompute881ec8 | C3 | FUN_0046d4a0 vehicle-record ptr getter forwarded via fn-ptr cast line 71 |
| 0x004a2c48 | call-original | FUN_004a2c48 | C2 | FUN_004a2c48 __ftol forwarded via naked x87 shim (g_ftol_4a2c48 line 76) for bit-identical rounding |

### `mashedmod/src/mashed_re/Ai/AiLineOfSight.cpp` — 5 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x00416060 | ported-function | FUN_00416060 | C3 | FUN_00416060 AI line-of-sight check, verbatim port from raw asm; RH_ScopedInstall(AiLos_Entry) line 162 |
| 0x00416250 | other | FUN_00416250 | C2 | FUN_00416250 orchestrator that calls this LOS gate (scope note); not hooked or called in this file |
| 0x00443dc0 | other | FUN_00443dc0 | C2 | FUN_00443dc0's WallAt cited as using the OPPOSITE X-row grid convention; comparison note only |
| 0x004a2c48 | other | FUN_004a2c48 | C2 | FUN_004a2c48 __ftol semantics cited (truncate toward zero); replicated via static_cast, NOT called here |
| 0x004c3bf0 | call-original | Vec2Length | C4 | FUN_004c3bf0 Vec2Length (float10 in ST0, declared float) forwarded via cast line 51 |

### `mashedmod/src/mashed_re/Ai/AiNavHooks.cpp` — 8 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x00416060 | other | FUN_00416060 | C3 | AiLineOfSight grid-role reference (X=row *0x80, Z=col) for WallAt |
| 0x00416230 | call-original | Table89a500Set | C3 | Table89a500Set arm-flag setter forwarded via fn-ptr (Arm L77) |
| 0x00443300 | call-original | FUN_00443300 | C3 | Catmull-Rom eval forwarded via fn-ptr cast (Spline helper L76) |
| 0x00443dc0 | ported-function | FUN_00443dc0 | C2 | NavLookahead greenfield reimpl; RH_ScopedInstall(AiNav_Entry, 0x00443dc0) at L431 |
| 0x004671a0 | other | sub_004671a0 | C2 | debug-render callee NOT ported (stub, debugRender path) |
| 0x004b55a0 | other | FUN_004b55a0 | C2 | debug-render callee NOT ported (stub, debugRender path) |
| 0x004c3bf0 | call-original | Vec2Length | C4 | Vec2Length forwarded; returns float10 in ST0 (fn-ptr declared float) |
| 0x004c3c60 | call-original | Vec2Normalize | C4 | Vec2Normalize forwarded; float10 ST0 return (void decl leaked x87 stack) |

### `mashedmod/src/mashed_re/Ai/AiSplineHooks.cpp` — 3 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x00443300 | ported-function | FUN_00443300 | C3 | AiSplineEval/AiSplineEval_x87 verbatim port; RH_ScopedInstall(AiSpline_Entry) L351 |
| 0x00443440 | other | FUN_00443440 | C2 | AI lookahead consumer this leaf is built for |
| 0x00443dc0 | other | FUN_00443dc0 | C2 | AI lookahead consumer this leaf is built for |

### `mashedmod/src/mashed_re/Ai/AiStandalone.cpp` — 35 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x00408af0 | other | AiVehicleFieldPtrGet | C3 | cross-product align needed by mode-2 tail, TODO |
| 0x0040e350 | other | GetRenderSubMode | C3 | original sub-game-mode getter behind Host.game_sub_mode (local_34) |
| 0x0040e480 | other | CarSlotStateSet | C3 | CarSlotStateSet alive-poke, host/entity TODO |
| 0x00414570 | other | FUN_00414570 | C2 | stubbed targeting/LOS helper (modes 1..10), RVA TODO port |
| 0x004148b0 | other | FUN_004148b0 | C3 | stubbed targeting/LOS helper (bare-hex slash-list form) |
| 0x00414a70 | other | FUN_00414a70 | C2 | stubbed targeting/LOS helper (bare-hex slash-list form) |
| 0x00414c30 | other | FUN_00414c30 | C2 | stubbed targeting/LOS helper (bare-hex slash-list form) |
| 0x00414f00 | other | FUN_00414f00 | C2 | stubbed targeting/LOS helper (bare-hex slash-list form) |
| 0x00415020 | other | AiLastPlaceFrustration | C3 | stubbed targeting/LOS helper (bare-hex slash-list form) |
| 0x004150e0 | other | FUN_004150e0 | C3 | stubbed targeting/LOS helper (bare-hex slash-list form) |
| 0x00415220 | other | FUN_00415220 | C2 | stubbed powerup activation |
| 0x00415880 | other | FUN_00415880 | C2 | stubbed targeting/LOS helper (bare-hex slash-list form) |
| 0x00415d00 | other | FUN_00415d00 | C3 | stubbed wall helper (FUN_00415d00) |
| 0x00415e20 | ported-function | FUN_00415e20 | C2 | steering-angle error reimplemented as SteerAngleError |
| 0x00416060 | other | FUN_00416060 | C3 | stubbed LOS helper (FUN_00416060) |
| 0x004161e0 | ported-function | FUN_004161e0 | C2 | target seed - ledger DONE (faithful) |
| 0x00416250 | ported-function | FUN_00416250 | C2 | primary control step reimplemented as ControlStep (bands verbatim, targeting stubbed) |
| 0x00416a30 | other | FUN_00416a30 | C2 | stubbed control-step variant (modes 4/9) |
| 0x00417180 | other | FUN_00417180 | C2 | stubbed bank-switch timer/RNG |
| 0x00417640 | other | FUN_00417640 | C2 | stubbed powerup-brake (post-step) |
| 0x004177b0 | other | FUN_004177b0 | C2 | stubbed pre-tick rubber-banding |
| 0x00417cf0 | other | FUN_00417cf0 | C2 | stubbed mode-8 helper |
| 0x00417da0 | other | FUN_00417da0 | C2 | stubbed control-step variant (mode 8) |
| 0x00418560 | ported-function | FUN_00418560 | C2 | AiVehicleStep bank-select/dispatch reimplemented (SelectSpline/VehicleStep) |
| 0x00418860 | ported-function | FUN_00418860 | C2 | AiTickLoop reimplemented as Ai_Standalone_Tick (per-frame tick, spline-count guard) |
| 0x00443300 | ported-function | FUN_00443300 | C3 | Catmull-Rom spline interp reimplemented as CatmullRom (ledger line also lists tail as stubbed) |
| 0x00443dc0 | ported-function | FUN_00443dc0 | C2 | spline lookahead reimplemented as SplineLookahead (Phases 1-8) |
| 0x0046d4a0 | other | PtrCompute881ec8 | C3 | original own-pos getter (+0x30/+0x38) abstracted behind Host.own_xz |
| 0x0046d510 | other | VehicleVelocityWorldGet | C3 | original velocity getter abstracted behind Host.own_vel_xz |
| 0x0046d6a0 | other | VehTbl8820acGet1 | C3 | [U-C-RATE0] rate float, speed substituted |
| 0x0046d6d0 | other | VehTbl881f84Get1 | C3 | [U-C-RATE1] rate float, rate1=0 substituted |
| 0x004a2c48 | ported-function | FUN_004a2c48 | C2 | ROUND(ST0) ctrl-byte quantizer reimplemented as RoundST0 |
| 0x004a3384 | other | CRT::acos | C2 | original acos callee replaced by std::acos |
| 0x004c39b0 | other | RwV3dNormalize | C4 | original normalize callee reproduced inline (x kept; *0 terms drop) |
| 0x004c3c60 | ported-function | Vec2Normalize | C4 | Normalize2 = standalone equiv of original fast-rsqrt 2-float normalize |

### `mashedmod/src/mashed_re/Ai/AiStandalone.h` — 16 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x0040e350 | other | GetRenderSubMode | C3 | Host.game_sub_mode replaces this original getter (race=6) |
| 0x0040e470 | other | CarSlotStateGet | C3 | Host.veh_type replaces this (0/1 = human) |
| 0x00415220 | other | FUN_00415220 | C2 | powerup activation stubbed |
| 0x00415e20 | ported-function | FUN_00415e20 | C2 | steering-angle calc ported (structural; constants were flagged) |
| 0x00417180 | other | FUN_00417180 | C2 | bank-switch timer/RNG stubbed |
| 0x004177b0 | other | FUN_004177b0 | C2 | rubber-banding stubbed |
| 0x00418560 | ported-function | FUN_00418560 | C2 | spline-bank select ported (used by Ai_ComputeTarget) |
| 0x00418860 | ported-function | FUN_00418860 | C2 | Ai_Standalone_Tick = standalone reimpl of AiTickLoop |
| 0x00426c00 | other | FUN_00426c00 | C3 | Host.track_index replaces this (0x21 = powerup-seek track) |
| 0x0042f6a0 | other | GetRaceSubMode | C3 | Host.round_type replaces this (3/4/5/10 = AI rounds) |
| 0x00443080 | other | AiTargetEnableGet | C3 | Host.ai_target_enable replaces this |
| 0x00443300 | other | FUN_00443300 | C3 | spline interpolation refinement listed stubbed |
| 0x00443dc0 | ported-function | FUN_00443dc0 | C2 | lookahead ported (Ai_ComputeTarget L63/Host LOS L46); tail refinement listed stubbed at L21 |
| 0x0046c7b0 | other | VehicleSlotGetter | C3 | Host.car_alive replaces this |
| 0x0046d4a0 | other | PtrCompute881ec8 | C3 | Host.own_xz replaces this (struct ptr +0x30/+0x38) |
| 0x0046d510 | other | VehicleVelocityWorldGet | C3 | Host.own_vel_xz replaces this (velocity) |

### `mashedmod/src/mashed_re/Ai/AiState.h` — 6 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x00415220 | other | FUN_00415220 | C2 | fire/powerup byte producer (citation) |
| 0x00415e20 | other | FUN_00415e20 | C2 | angle-error source feeding steer bytes (citation) |
| 0x00416250 | other | FUN_00416250 | C2 | producer of steer ctrl pair (citation) |
| 0x00418560 | other | FUN_00418560 | C2 | citing function for ctrl-block/slot-table/spline addresses |
| 0x00418860 | other | FUN_00418860 | C2 | cluster anchor: header maps globals for the FUN_00418860 opponent-AI cluster |
| 0x00467650 | other | VehicleWheelDrivetrainUpdate | C4 | accel/brake byte consumer *(blk+4)/*(blk+5) (citation) |

### `mashedmod/src/mashed_re/Ai/AiTargeting.cpp` — 14 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x004150e0 | other | FUN_004150e0 | C3 | FUN_004150e0 wall-lateral query explicitly NOT ported here (x87-stack args); carried for a later pass |
| 0x00415d00 | ported-function | FUN_00415d00 | C3 | AiWallAhead 2x-velocity ray-march wall check; RH_ScopedInstall line 253 |
| 0x00415e20 | ported-function | FUN_00415e20 | C2 | AiSteeringAngleError signed angular error (x87 ST0 return); RH_ScopedInstall line 210 |
| 0x00416060 | ported-function | FUN_00416060 | C3 | AiLineOfSight XZ ray-march tile LOS; RH_ScopedInstall line 101 |
| 0x004161e0 | ported-function | FUN_004161e0 | C2 | AiSplineTargetInit seed target from own XZ; RH_ScopedInstall line 120 |
| 0x00418860 | other | FUN_00418860 | C2 | FUN_00418860 named as the parent AI cluster whose leaves are ported here; not hooked or called in this file |
| 0x00443d10 | call-original | FUN_00443d10 | C2 | tile query (float x,z -> char) forwarded via cast; FUN_00443d10 named line 216 |
| 0x00443dc0 | call-original | FUN_00443dc0 | C2 | FUN_00443dc0 spline lookahead target finder, forwarded via cast line 46 (f(spline,xz,outIdx,v,1,0)) |
| 0x0046d4a0 | call-original | PtrCompute881ec8 | C3 | own-vehicle struct ptr getter (C3) forwarded via cast line 42 |
| 0x0046d510 | call-original | VehicleVelocityWorldGet | C3 | velocity vec3 getter also forwarded via cast for AiSteeringAngleError/AiWallAhead (same RVA this file ports) |
| 0x0046d510 | ported-function | VehicleVelocityWorldGet | C3 | AiVehicleVelocity3 per-vehicle velocity float3 getter; RH_ScopedInstall line 143 |
| 0x004a3384 | call-original | CRT::acos | C2 | original acos (float10 in ST0) forwarded via cast; used instead of MSVC acos (comment line 167) |
| 0x004c39b0 | call-original | RwV3dNormalize | C4 | RwV3dNormalize forwarded via cast (fn_norm_t) |
| 0x004c3bf0 | call-original | Vec2Length | C4 | Vec2Length (C4, float10 in ST0) forwarded via cast line 39 |
| 0x004c3df0 | call-original | RwV3dTransformPoints | C4 | RW transform-points f(dst,src,1,matrix) forwarded via cast line 51 |

### `mashedmod/src/mashed_re/Ai/AiWallAhead.cpp` — 7 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x00415d00 | ported-function | FUN_00415d00 | C3 | FUN_00415d00 AI wall-ahead check, verbatim port; RH_ScopedInstall line 167; MASHED_HOOK_ONLY value line 42 |
| 0x00416060 | other | FUN_00416060 | C3 | AiLineOfSight (0x00416060) cited as the structural mirror of this port; reference only |
| 0x00416250 | other | FUN_00416250 | C2 | FUN_00416250 orchestrator (mode-2 suppression user); REMOVED temp coverage driver hook site (lines 154-163) |
| 0x00443d10 | call-original | FUN_00443d10 | C2 | FUN_00443d10 tile-type getter (C2) forwarded via cast line 54 (opposite X/Z grid convention sidestepped) |
| 0x0046d4a0 | call-original | PtrCompute881ec8 | C3 | FUN_0046d4a0 PtrCompute881ec8 (C3) vehicle-record ptr getter forwarded via cast line 51 |
| 0x0046d510 | call-original | VehicleVelocityWorldGet | C3 | FUN_0046d510 VehicleVelocityWorldGet (C3) forwarded via cast line 52 |
| 0x004c3bf0 | call-original | Vec2Length | C4 | FUN_004c3bf0 Vec2Length (C4, float10 in ST0) forwarded via cast line 53 |

### `mashedmod/src/mashed_re/Ai/AiWallLateral.cpp` — 6 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x004150e0 | ported-function | FUN_004150e0 | C3 | FUN_004150e0 AI track-wall lateral-zone query (surface byte == 2), verbatim port; RH_ScopedInstall line 145 |
| 0x00416060 | other | FUN_00416060 | C3 | FUN_00416060 AiLineOfSight cited as the same tile-check minus loop/length; comparison note only |
| 0x00416250 | other | FUN_00416250 | C2 | FUN_00416250 orchestrator: this leaf is one of its mode-9 gate callees (caller citation) |
| 0x00416a30 | other | FUN_00416a30 | C2 | FUN_00416a30 additional caller of FUN_004150e0 (caller citation) |
| 0x00417da0 | other | FUN_00417da0 | C2 | FUN_00417da0 additional caller of FUN_004150e0 (caller citation) |
| 0x004a2c48 | other | FUN_004a2c48 | C2 | FUN_004a2c48 __ftol semantics cited (truncate toward zero); replicated via static_cast, NOT called here |

### `mashedmod/src/mashed_re/Ai/PromoLoop_round1.cpp` — 18 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x00408af0 | ported-function | AiVehicleFieldPtrGet | C3 | AiVehicleFieldPtrGet pointer-return field getter (&DAT_008a96dc + idx*0x30c); RH_ScopedInstall line 56 |
| 0x0040e350 | ported-function | GetRenderSubMode | C3 | GetRenderSubMode global getter (returns DAT_0063ba8c); RH_ScopedInstall line 158 |
| 0x00411170 | other | TimeTrialRecordPlayback | C3 | FUN_00411170 cited as writer of DAT_0063ba8c (writes 7 at race-over) |
| 0x00414030 | ported-function | AiSplineBankTimerReset | C3 | AiSplineBankTimerReset writes 1000 per slot / all slots; RH_ScopedInstall line 135 |
| 0x004148b0 | other | FUN_004148b0 | C3 | FUN_004148b0 listed as caller of FUN_00442cc0 (caller citation) |
| 0x00414c30 | other | FUN_00414c30 | C2 | FUN_00414c30 listed as caller of FUN_00442cc0 (caller citation) |
| 0x00415020 | other | AiLastPlaceFrustration | C3 | FUN_00415020 listed as caller of FUN_00442cc0 (caller citation) |
| 0x00415190 | other | FUN_00415190 | C2 | FUN_00415190 listed as caller of FUN_00442cc0 (caller citation) |
| 0x00415200 | other | FUN_00415200 | C2 | FUN_00415200 listed as caller of FUN_00442cc0 (caller citation) |
| 0x00415220 | other | FUN_00415220 | C2 | FUN_00415220 listed as caller of FUN_00408af0 and FUN_00442cc0 (caller citation) |
| 0x00415880 | other | FUN_00415880 | C2 | FUN_00415880 listed as caller of FUN_00408af0 (caller citation) |
| 0x00416250 | other | FUN_00416250 | C2 | FUN_00416250 listed as caller of FUN_00408af0 (caller citation) |
| 0x00416a30 | other | FUN_00416a30 | C2 | FUN_00416a30 listed as caller of FUN_00408af0 (caller citation) |
| 0x00417180 | other | FUN_00417180 | C2 | FUN_00417180 (AI line-bank switches) listed as caller of FUN_00414030 (caller citation) |
| 0x00417da0 | other | FUN_00417da0 | C2 | FUN_00417da0 listed as caller of FUN_00408af0 (caller citation) |
| 0x00429300 | ported-function | HudOverlayFloatGet | C3 | HudOverlayFloatGet FILD int32->float getter (subsystem hud, kept in this cluster file); RH_ScopedInstall line 191 |
| 0x00429620 | other | FUN_00429620 | C2 | FUN_00429620 cited as the caller of FUN_00429300 that passes an unread arg (caller citation) |
| 0x00442cc0 | ported-function | AiVehicleFloat4Get | C3 | AiVehicleFloat4Get bounds-checked per-vehicle float getter; RH_ScopedInstall line 92 |

### `mashedmod/src/mashed_re/Ai/VehicleVelocityWorldGet.cpp` — 3 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x0046d510 | ported-function | VehicleVelocityWorldGet | C3 | 0x0046d510 per-vehicle world-space velocity getter, ported from verbatim disasm; RH_ScopedInstall line 54 |
| 0x0046d700 | other | VehicleVec3At9C8Get | C3 | 0x0046d700 VehicleVec3At9C8Get cited as the direct twin of this getter; reference only |
| 0x004c3df0 | call-original | RwV3dTransformPoints | C4 | FUN_004c3df0 RwV3dTransformPoints (C4) called via fn-ptr cast line 44 |

### `mashedmod/src/mashed_re/Race/CameraClusterHooks.cpp` — 7 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x0040e180 | ported-function | MostSeparatedPair | C4 | MostSepPair_0040e180 verbatim transcription (0x0040e180..0x0040e330); RH_ScopedInstall line 87 |
| 0x00410d10 | other | FUN_00410d10 | C2 | caller reference: elimination rule 0x00410d10 |
| 0x00446520 | other | FUN_00446520 | C2 | caller reference: race camera director 0x00446520 |
| 0x0046c7b0 | call-original | VehicleSlotGetter | C3 | Alive getter forwarder FUN_0046c7b0 (reads 0x008815a4 + i*0xd04) |
| 0x0046cbb0 | call-original | CarStatePairGet | C3 | Dead getter forwarder FUN_0046cbb0 (reads 0x00881f90 + i*0xd04) |
| 0x0046d4a0 | call-original | PtrCompute881ec8 | C3 | car-struct ptr getter forwarder FUN_0046d4a0; pos at +0x30/+0x34/+0x38 |
| 0x004c3ac0 | call-original | Vec3Magnitude | C4 | Vec3Magnitude forwarder FUN_004c3ac0 (RW fast-sqrt LUT) — forwarded for bit-exactness |

### `mashedmod/src/mashed_re/Race/GameFlow.cpp` — 3 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x00404e80 | other | Save::DeserializeFromBuffer | C4 | cited format source: DeserializeFromBuffer 0x00404e80 (ported in Save/GameSaveFormat.h) |
| 0x00404ee0 | other | Save::SerializeToBuffer | C4 | cited format source: Save::SerializeToBuffer 0x00404ee0 (ported in Save/GameSaveFormat.h) |
| 0x0040b6c0 | other | FrontendArrayGet | C4 | TODO-RE comment: binary cup track-name table FUN_0040b6c0 (not ported) |

### `mashedmod/src/mashed_re/Race/GameFlow.h` — 1 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x0040b6c0 | other | FrontendArrayGet | C4 | TODO-RE: FUN_0040b6c0 track-name table for cup loading |

### `mashedmod/src/mashed_re/Race/RaceCamera.cpp` — 18 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x00408ad0 | other | RaceScoreFloatGetBySlot | C3 | 0x00408ad0 race progress percent getter (adapter source for race_pct) |
| 0x00409790 | other | FUN_00409790 | C2 | cmd-stream opcodes 0xC..0xF feeder of the LED override table (original code) |
| 0x0040e180 | ported-function | MostSeparatedPair | C4 | MostSeparatedPair ports 0x0040e180 (3D dist, active+alive+!dying; also cited 179/421) |
| 0x0040eee0 | other | FUN_0040eee0 | C3 | points award FUN_0040eee0(victim,1) (ported with the points system) |
| 0x00410d10 | ported-function | FUN_00410d10 | C2 | EliminationCheck ports the 0x00410d10 standard path (elimination rule) |
| 0x00422fd0 | other | FrontendRaceResultsDispatch | C3 | caller applies FUN_00422fd0 equivalent (kill victim) — not in this port |
| 0x00426cc0 | other | VehicleTable4cPtrGet | C3 | FUN_00426cc0 node race-direction getter (fallback source) |
| 0x00426d00 | other | FrontendArraySlotGet | C3 | FUN_00426d00(n,0/3) gate corner getter (fallback tilt axis) |
| 0x00441760 | other | Camera::Apply | C2 | Camera::Apply 0x00441760 RW frame rebuild (host builds LookAt instead) |
| 0x00441820 | ported-function | CameraPath::SamplePoint | C2 | NodeDir ports 0x00441820 per-node camera direction + height |
| 0x00446520 | ported-function | FUN_00446520 | C2 | Update ports the 0x00446520 race branch (camera director) |
| 0x004922e0 | other | CarEventTrigger | C3 | hit FX FUN_004922e0 (caller-side) |
| 0x004a2c48 | ported-function | FUN_004a2c48 | C2 | BankersRound ports 0x004a2c48 FPU round-to-nearest-even (also cited line 131) |
| 0x004c39b0 | ported-function | RwV3dNormalize | C4 | Vec3Norm ports 0x004c39b0 RW normalize (call site cited line 249) |
| 0x004c3ac0 | ported-function | Vec3Magnitude | C4 | Vec3Mag ports 0x004c3ac0 Vec3Magnitude (plain-sqrt form; bit-exact C4 ref in Math/Vec3.cpp) |
| 0x004c3df0 | ported-function | RwV3dTransformPoints | C4 | RotateAboutAxis fuses 0x004c3df0 transform |
| 0x004c4d20 | ported-function | RwMatrixRotate | C4 | RotateAboutAxis fuses 0x004c4d20 RwMatrix_SetRotAxisAngle |
| 0x00534870 | other | FUN_00534870 | C1 | original PRNG FUN_00534870 (replaced by documented xorshift ADAPTER) |

### `mashedmod/src/mashed_re/Race/RaceCamera.h` — 15 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x00408a50 | other | FUN_00408a50 | C3 | ADAPTER source: FUN_00408a50 camera-path progress getter |
| 0x00408ad0 | other | RaceScoreFloatGetBySlot | C3 | ADAPTER source: 0x00408ad0 race progress percent getter |
| 0x0040e180 | ported-function | MostSeparatedPair | C4 | verbatim-port list: FUN_0040e180 most-separated pair finder; also line 70 |
| 0x0040e370 | other | FUN_0040e370 | C3 | active flag source IsCarSlotActive 0x0040e370 (C3-ported) |
| 0x00410d10 | ported-function | FUN_00410d10 | C2 | verbatim-port list: FUN_00410d10 elimination / round-end core; also line 65 |
| 0x00426cc0 | other | VehicleTable4cPtrGet | C3 | node unit race-direction source FUN_00426cc0 |
| 0x00426d00 | other | FrontendArraySlotGet | C3 | gate corner source FUN_00426d00(n,0) / (n,3) |
| 0x00441820 | ported-function | CameraPath::SamplePoint | C2 | verbatim-port list: FUN_00441820 per-node camera direction; also 51/82 |
| 0x00442a60 | ported-function | Spectator::ComputeDistances | C2 | verbatim-port list: FUN_00442a60 per-player distance array |
| 0x00446520 | ported-function | FUN_00446520 | C2 | verbatim-port list: FUN_00446520 race branch (camera director); also line 57 |
| 0x0046c7b0 | other | VehicleSlotGetter | C3 | ADAPTER source: FUN_0046c7b0 alive getter (== 1) |
| 0x0046cb30 | other | Player::GetOffset3D | C2 | ADAPTER source: FUN_0046cb30 world velocity getter |
| 0x0046cbb0 | other | CarStatePairGet | C3 | ADAPTER source: FUN_0046cbb0 dead-flag + timer getter |
| 0x0046d4a0 | other | PtrCompute881ec8 | C3 | ADAPTER source: FUN_0046d4a0 car pos getter (host fills RaceCamCar) |
| 0x00534870 | other | FUN_00534870 | C1 | original jitter PRNG FUN_00534870 (adapter xorshift stands in) |

### `mashedmod/src/mashed_re/Race/RaceModes.cpp` — 4 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x00410d10 | ported-function | FUN_00410d10 | C2 | IsEliminationRule derives objective split from FUN_00410d10 control flow (cases 5/7 early-return) |
| 0x0042f6b0 | ported-function | MenuModeSync | C3 | SelectionToGameMode transcribes the 0x0042f6b0 switch (writes DAT_0067e9fc) |
| 0x00430b60 | other | MenuSlotCount | C3 | comment: FUN_00430b60 gate / push-screen branches do not write DAT_007f0fd0 |
| 0x0043dfd0 | ported-function | FUN_0043dfd0 | C2 | RaceRuleFromEvent transcribes the 0x0043dfd0 race-launch event->rule switch -> DAT_007f0fd0 |

### `mashedmod/src/mashed_re/Race/RaceModes.h` — 3 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x00410d10 | ported-function | FUN_00410d10 | C2 | IsEliminationRule: proven objective split from FUN_00410d10 control flow |
| 0x0042f6b0 | ported-function | MenuModeSync | C3 | SelectionToGameMode = 0x0042f6b0 FUN_0042f6b0 (MenuModeSync) |
| 0x0043dfd0 | ported-function | FUN_0043dfd0 | C2 | RaceRuleFromEvent = 0x0043dfd0 race-launch action 0xff240000 |

### `mashedmod/src/mashed_re/Race/RaceSession.cpp` — 4 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x0040d110 | other | FUN_0040d110 | C2 | comment: AI-car spawn mirrors FUN_0040d110 selection shape |
| 0x00446520 | other | FUN_00446520 | C2 | stub note: RaceCamera subsystem is REAL via the 0x00446520 verbatim port |
| 0x004625b0 | other | FUN_004625b0 | C2 | citation for character-bank mapping FUN_004625b0 (REmap note) |
| 0x00462dd0 | other | FUN_00462dd0 | C2 | citation for character-bank mapping FUN_00462dd0 (REmap note) |

### `mashedmod/src/mashed_re/Race/RaceSession.h` — 4 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x0040e480 | other | CarSlotStateSet | C3 | FUN_0040e480(slot,2) marks AI; TODO-RE AI/ dir family (line 74) |
| 0x0042fab0 | other | FUN_0042fab0 | C2 | carIndex = FUN_0042fab0 index (comment ref) |
| 0x00430670 | other | FUN_00430670 | C2 | TODO-RE: FUN_00430670 + pickup logic (IPowerupSystem) |
| 0x004b6940 | other | PizOpenAndParse | C2 | TODO-RE: FUN_004b6940 track load chain (ITrackRuntime::Load) |

### `mashedmod/src/mashed_re/Race/ScoringHooks.cpp` — 30 function-root RVAs

| RVA | kind | name (hooks.csv) | C | context |
|---|---|---|---|---|
| 0x00405890 | call-original | Pred405890 | C3 | forwarder FUN_00405890 (0x00410510-only callee) |
| 0x00408a50 | call-original | FUN_00408a50 | C3 | forwarder FUN_00408a50 float path progress (x87 return) |
| 0x00408a70 | call-original | FrontendC2RoundI | C3 | forwarder FUN_00408a70(i, float) progress write |
| 0x00408ad0 | call-original | RaceScoreFloatGetBySlot | C3 | forwarder FUN_00408ad0 float progress percent (x87 return) |
| 0x0040b290 | call-original | FUN_0040b290 | C4 | forwarder to the score adder — routes through this file's own installed hook |
| 0x0040b290 | ported-function | FUN_0040b290 | C4 | ScoreAdd_0040b290 verbatim port; RH_ScopedInstall(ScoreAdd_0040b290, 0x0040b290) line 167 |
| 0x0040b6d0 | call-original | FUN_0040b6d0 | C3 | forwarder FUN_0040b6d0 (score read) |
| 0x0040d590 | call-original | FUN_0040d590 | C2 | forwarder FUN_0040d590(a,b,c,d) |
| 0x0040e340 | call-original | GetLiveCarCount | C4 | forwarder FUN_0040e340 |
| 0x0040e350 | call-original | GetRenderSubMode | C3 | forwarder FUN_0040e350 (event-ring gate) |
| 0x0040e370 | call-original | FUN_0040e370 | C3 | forwarder FUN_0040e370 (slot active) |
| 0x0040e470 | call-original | CarSlotStateGet | C3 | forwarder FUN_0040e470 |
| 0x0040eee0 | ported-function | FUN_0040eee0 | C3 | ScoreElim_0040eee0 round-end elimination scoring; RH_ScopedInstall line 506 |
| 0x00410510 | ported-function | Race::EvaluateResult | C3 | EvalResult_00410510 match-end evaluator; RH_ScopedInstall line 617 |
| 0x00410d10 | other | FUN_00410d10 | C2 | caller reference: delta = 1 comes from 0x00410d10 |
| 0x00417730 | call-original | VehicleRaceAngleGet | C3 | forwarder FUN_00417730 float (0x00410510-only callee) |
| 0x00417740 | call-original | FUN_00417740 | C2 | forwarder FUN_00417740 (0x00410510-only callee) |
| 0x00422fd0 | call-original | FrontendRaceResultsDispatch | C3 | forwarder FUN_00422fd0 (kill car) |
| 0x00426c00 | call-original | FUN_00426c00 | C3 | forwarder FUN_00426c00 (course id; ==0x26 check) |
| 0x0042f500 | call-original | GetDat0067ea64 | C4 | forwarder FUN_0042f500 (teams flag) |
| 0x0042f6a0 | call-original | GetRaceSubMode | C3 | forwarder FUN_0042f6a0 (race type) |
| 0x00431d80 | call-original | TiebreakFlagGet | C3 | forwarder; callee hooked elsewhere (TiebreakFlagGet) — routes through live inline-JMP |
| 0x00448700 | call-original | FUN_00448700 | C2 | forwarder FUN_00448700(a,b) (0x00410510-only callee) |
| 0x0044df80 | call-original | FUN_0044df80 | C2 | forwarder FUN_0044df80(0) (0x00410510-only callee) |
| 0x00458f80 | call-original | FUN_00458f80 | C3 | forwarder FUN_00458f80(0) (0x00410510-only callee) |
| 0x0045bed0 | call-original | PowerupRoundCleanup | C2 | forwarder FUN_0045bed0 (0x00410510-only callee) |
| 0x0045bf30 | call-original | PowerupTeardownAll | C2 | forwarder FUN_0045bf30 (0x00410510-only callee) |
| 0x0046c700 | call-original | EntityScoreFieldAdd | C3 | forwarder; callee hooked elsewhere (EntityScoreFieldAdd) — routes through live inline-JMP |
| 0x0046c7b0 | call-original | VehicleSlotGetter | C3 | forwarder FUN_0046c7b0 (alive) |
| 0x0046cbb0 | call-original | CarStatePairGet | C3 | forwarder FUN_0046cbb0(i, int*, void*) dead-flag getter |
| 0x004a2cbd | other | FID_conflict:_wprintf | C2 | debug wprintf callee FUN_004a2cbd — omitted (side-effect-only stdout) |

### Data-global citations (all source files)

These are data addresses, not functions; they are NOT BFS seeds.

| RVA | source file | in hooks.csv | context |
|---|---|---|---|
| 0x005cc31c | mashedmod/src/mashed_re/Ai/AiSplineHooks.cpp | no | float 3.0 (0x40400000) Catmull-Rom const k3_0 |
| 0x005cc31c | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 3.0 Catmull-Rom const |
| 0x005cc31c | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | float threshold constant (GameMode case 7) |
| 0x005cc320 | mashedmod/src/mashed_re/Ai/AiLineOfSight.cpp | no | _DAT_005cc320 1.0; replicated locally as kOne |
| 0x005cc320 | mashedmod/src/mashed_re/Ai/AiNavHooks.cpp | no | float 1.0 kOne |
| 0x005cc320 | mashedmod/src/mashed_re/Ai/AiSplineHooks.cpp | no | float 1.0 t-clamp upper k1_0 |
| 0x005cc320 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 1.0 kSteerClampHi / steer deadband / walk clamp |
| 0x005cc320 | mashedmod/src/mashed_re/Ai/AiTargeting.cpp | no | 1.0 float constant read via F32 (1/length; clamp value) |
| 0x005cc320 | mashedmod/src/mashed_re/Ai/AiWallAhead.cpp | no | _DAT_005cc320 1.0; replicated locally as kOne |
| 0x005cc320 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 1.0f constant (keep = 1 - frac; also 202/217/311) |
| 0x005cc32c | mashedmod/src/mashed_re/Ai/AiLineOfSight.cpp | no | _DAT_005cc32c 0.5 round-half bias; replicated locally as Cf(0x3f000000) kHalf |
| 0x005cc32c | mashedmod/src/mashed_re/Ai/AiSplineHooks.cpp | no | float 0.5 final factor k0_5 |
| 0x005cc32c | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 0.5 Catmull-Rom const |
| 0x005cc32c | mashedmod/src/mashed_re/Ai/AiWallLateral.cpp | no | _DAT_005cc32c 0.5 round-half bias; replicated locally as Cf(0x3f000000) kHalf |
| 0x005cc32c | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 0.5f constant (path zoom average) |
| 0x005cc33c | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float -1.0 kSteerClampLo |
| 0x005cc33c | mashedmod/src/mashed_re/Ai/AiTargeting.cpp | no | clamp replacement value when below 0x005cd0d0, read via F32 |
| 0x005cc33c | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | -1.0f clamp constant (pitch dot) |
| 0x005cc358 | mashedmod/src/mashed_re/Ai/AiSplineHooks.cpp | no | float 5.0 quadratic P1*5 k5_0 |
| 0x005cc358 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 5.0 Catmull-Rom const |
| 0x005cc358 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 5.0f constant (pitch -5 / dist divisor / snap threshold / sway divisor) |
| 0x005cc35c | mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp | no | _DAT_005cc35c float threshold (4.0), read live via G_35c line 59 |
| 0x005cc35c | mashedmod/src/mashed_re/Ai/AiLineOfSight.cpp | no | _DAT_005cc35c 4.0 tile coord scale; replicated locally as Cf(0x40800000) k4_0 |
| 0x005cc35c | mashedmod/src/mashed_re/Ai/AiNavHooks.cpp | no | float 4.0 tile coord scale k4_0 |
| 0x005cc35c | mashedmod/src/mashed_re/Ai/AiSplineHooks.cpp | no | float 4.0 quadratic P2*4 k4_0 |
| 0x005cc35c | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 4.0 Catmull-Rom const |
| 0x005cc35c | mashedmod/src/mashed_re/Ai/AiWallLateral.cpp | no | _DAT_005cc35c 4.0 tile coord scale; replicated locally as Cf(0x40800000) k4_0; also FMUL operand in trampoline note |
| 0x005cc55c | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 10.0 brake min speed |
| 0x005cc55c | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 10.0f zoom cap (also the fcomp saturation constant at 0x00410ee3, line 416) |
| 0x005cc564 | mashedmod/src/mashed_re/Ai/AiLineOfSight.cpp | no | _DAT_005cc564 0.25 ray-march step; replicated locally as k0_25 |
| 0x005cc564 | mashedmod/src/mashed_re/Ai/AiNavHooks.cpp | no | float 0.25 ray-march step k0_25 |
| 0x005cc564 | mashedmod/src/mashed_re/Ai/AiTargeting.cpp | no | _DAT_005cc564 ray-march step constant, read via F32 lines 95/247 |
| 0x005cc564 | mashedmod/src/mashed_re/Ai/AiWallAhead.cpp | no | _DAT_005cc564 0.25 ray-march step; replicated locally as k0_25 |
| 0x005cc568 | mashedmod/src/mashed_re/Ai/AiController.cpp | no | rate gate const (full-brake branch) |
| 0x005cc568 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 100.0 lap-wrap subtract constant |
| 0x005cc568 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | 100.0 _DAT_005cc568 lap-wrap subtract |
| 0x005cc574 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 2.0 constant (midpoint halve) |
| 0x005cc574 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | float threshold constant (GameMode case 10) |
| 0x005cc72c | mashedmod/src/mashed_re/Ai/AiController.cpp | no | held-type float gate const (_DAT_005cc72c) |
| 0x005cc72c | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 30.0 accel err lo |
| 0x005cc730 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 80.0 lap-wrap window constant |
| 0x005cc730 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | 80.0 _DAT_005cc730 lap-wrap window (F32 read line 186) |
| 0x005cc970 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | double 0x404ca5dc20000000 ~=57.29578 rad->deg (kSteerScale) |
| 0x005cc970 | mashedmod/src/mashed_re/Ai/AiTargeting.cpp | no | _DAT_005cc970 scale applied to acos result (F32 reads lines 187/199) |
| 0x005cc9a0 | mashedmod/src/mashed_re/Ai/AiNavHooks.cpp | no | float 0.05 forward-walk step k0_05 |
| 0x005cc9a0 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 0.05 forward-walk step / kSteerExtra |
| 0x005cc9a0 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 0.05 spring gain constant |
| 0x005cc9b0 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 15.0 brake speed delta |
| 0x005cc9b0 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 15.0f overhead camera height |
| 0x005cc9bc | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 0.8f constant (pair zoom scale / distance law / spring damp) |
| 0x005cc9c0 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 0.2f sway amplitude constant |
| 0x005cc9c8 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | float threshold constant (GameMode case 9) |
| 0x005cc9fc | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 1000.0 constant (dead_ms/1000 zoom-out) |
| 0x005ccabc | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 1.1f path-zoom multiplier |
| 0x005ccac4 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 360.0 kWrap |
| 0x005ccac4 | mashedmod/src/mashed_re/Ai/AiTargeting.cpp | no | two-pi wrap constant (kTwoPi) read via F32 |
| 0x005ccad0 | mashedmod/src/mashed_re/Ai/AiController.cpp | no | range gate const (full-brake branch) |
| 0x005ccad0 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 90.0f constant |
| 0x005ccae0 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | kRadToDeg f64 57.295799255371094 @0x005ccae0 |
| 0x005ccd18 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 0.00015 velocity-lead constant |
| 0x005ccd6c | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 20.0 (k20f) |
| 0x005ccd6c | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 20.0 lap-wrap window constant |
| 0x005ccd6c | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | 20.0 _DAT_005ccd6c lap-wrap window |
| 0x005ccdb4 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | omitted wprintf format string @0x5ccdb4 (side-effect-only) |
| 0x005ccdcc | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | omitted wprintf format string @0x5ccdcc (side-effect-only) |
| 0x005cd030 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 5.0 offset-scale divisor |
| 0x005cd04c | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 255.0 steer-byte clamp |
| 0x005cd074 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 1.25f City zoom multiplier |
| 0x005cd088 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 2.5f height-mix constant |
| 0x005cd09c | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 180.0 steer split (kSteerSplit) |
| 0x005cd09c | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 180.0f constant (azim + 180) |
| 0x005cd0a0 | mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp | no | _DAT_005cd0a0 float threshold, read live via G_a0 |
| 0x005cd0a4 | mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp | no | _DAT_005cd0a4 float threshold, read live via G_a4 |
| 0x005cd0a8 | mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp | no | _DAT_005cd0a8 float threshold, read live via G_a8 |
| 0x005cd0b8 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 2000.0 rate1 brake gate |
| 0x005cd0c8 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | double +1.0 acos-domain clamp hi (kSteerThrHi) |
| 0x005cd0c8 | mashedmod/src/mashed_re/Ai/AiTargeting.cpp | no | upper clamp threshold for acos input, read via F32 |
| 0x005cd0d0 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | double -1.0 acos-domain clamp lo (kSteerThrLo) |
| 0x005cd0d0 | mashedmod/src/mashed_re/Ai/AiTargeting.cpp | no | lower clamp threshold for acos input, read via F32 |
| 0x005cd0d8 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 1250.0 mode-9 brake B |
| 0x005cd0dc | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 1750.0 mode-9 brake A |
| 0x005cd0e0 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 330.0 accel err hi |
| 0x005cd0e4 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 359.0 steer active bound |
| 0x005cd0e8 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 0.0030034 steer mag scale |
| 0x005cd0ec | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 0.005 counter-steer scale |
| 0x005cd120 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 50.0f pitch-extra constant |
| 0x005cd7a8 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | pi/180 constant 0.01745329252 in orig .rdata |
| 0x005cd9e8 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 155.0 sway divisor |
| 0x005ce034 | mashedmod/src/mashed_re/Ai/AiNavHooks.cpp | no | subdivision bisection weight ~1/3 k0_333 |
| 0x005ce034 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float ~1/3 ternary-search weight (kThird) |
| 0x005ce040 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 420.0 sway divisor |
| 0x005ce044 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 400.0 sway divisor |
| 0x005ce190 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 16.5 sway divisor |
| 0x005ce194 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 200.0 sway divisor |
| 0x005ce198 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 33.9 sway divisor |
| 0x005ce19c | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 139.5 sway divisor |
| 0x005ce1a0 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 83.0 sway divisor |
| 0x005ce1a4 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 175.0 sway divisor |
| 0x005ce1a8 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 635.0 sway divisor |
| 0x005ce1ac | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 2.25f City height-mix constant |
| 0x005ce1b0 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | -2.5f height-mix constant |
| 0x005ce1b4 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 89.9f pitch clamp |
| 0x005ce1b8 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 67.5f City pitch constant |
| 0x005ce1c0 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | -3.95 offset-scale constant |
| 0x005ce1c8 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 0.25 offset-scale constant |
| 0x005d757c | mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp | no | DAT_005d757c 0.0 float constant; replicated locally as Cf(0x00000000) kZero |
| 0x005d757c | mashedmod/src/mashed_re/Ai/AiLineOfSight.cpp | no | DAT_005d757c 0.0; replicated locally as Cf(0x00000000) kZero |
| 0x005d757c | mashedmod/src/mashed_re/Ai/AiNavHooks.cpp | no | float 0.0 kZero |
| 0x005d757c | mashedmod/src/mashed_re/Ai/AiSplineHooks.cpp | no | float 0.0 t-clamp lower k0_0 |
| 0x005d757c | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | float 0.0 (zero multiplier / clamp lower / metric init) |
| 0x005d757c | mashedmod/src/mashed_re/Ai/AiTargeting.cpp | no | 0.0 float constant read via F32 (length>0 test, kZero in AiSteeringAngleError) |
| 0x005d757c | mashedmod/src/mashed_re/Ai/AiWallAhead.cpp | no | DAT_005d757c 0.0; replicated locally as Cf(0x00000000) kZero |
| 0x005d757c | mashedmod/src/mashed_re/Ai/PromoLoop_round1.cpp | no | DAT_005d757c project-wide 0.0 float, returned for idx >= 4 (code line 89) |
| 0x005d757c | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | 0.0f constant DAT_005d757c (blend weight clamp) |
| 0x005d757c | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | DAT_005d757c compare constant (case 10) |
| 0x005f2770 | mashedmod/src/mashed_re/Race/CameraClusterHooks.cpp | no | PTR_PTR_005f2770 slot-active table double-pointer |
| 0x005f2770 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | PTR_PTR_005f2770 slot table double-pointer (SlotActive probe) |
| 0x005f2dd8 | mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp | no | DAT_005f2dd8 int limit table indexed by (bias + iVar1*5)*4 (kLimitTbl) |
| 0x005f65c8 | mashedmod/src/mashed_re/Race/RaceModes.cpp | no | cup event-type table DAT_005f65c8 (.rdata), baked into kCupEvent from Ghidra bytes |
| 0x005f65c8 | mashedmod/src/mashed_re/Race/RaceModes.h | no | cup event-type table at 0x005f65c8 (.rdata; image-pad zeroed in standalone) |
| 0x006041f0 | mashedmod/src/mashed_re/Race/RaceSession.cpp | no | character-voice bank index global DAT_006041f0 (colour 0..5) |
| 0x006146f0 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | X-axis constant (1,0,0) DAT_006146f0 |
| 0x006146fc | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | Y-axis constant (0,1,0) DAT_006146fc |
| 0x00614708 | mashedmod/src/mashed_re/Ai/AiTargeting.cpp | no | matrix used by the 0x004c3df0 transform in AiVehicleVelocity3 |
| 0x00614708 | mashedmod/src/mashed_re/Ai/VehicleVelocityWorldGet.cpp | no | matrix at 0x00614708 used by the transform (kMatrix_614708 line 30) |
| 0x0063a5f0 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | LED override table DAT_0063a5f0 (on-disk image = LE<id>.LED) |
| 0x0063a5f0 | mashedmod/src/mashed_re/Race/RaceCamera.h | no | LED format verified against the live DAT_0063a5f0 table |
| 0x0063b90c | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | result state DAT_0063b90c (cleared then set 1) |
| 0x0063b910 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | _DAT_0063b910 = 0x442f0000 (700.0f) |
| 0x0063b914 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | winner index result field |
| 0x0063b918 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | result field write (= 0) |
| 0x0063b91c | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | result field write (= 0) |
| 0x0063ba8c | mashedmod/src/mashed_re/Ai/PromoLoop_round1.cpp | no | DAT_0063ba8c game-mode state global returned by GetRenderSubMode (code line 155) |
| 0x0063ba8c | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | result state DAT_0063ba8c = 0xb on conclusion |
| 0x00663658 | mashedmod/src/mashed_re/Race/RaceCamera.h | no | gate-ribbon node array DAT_00663658 stride 0x4c |
| 0x0067e9fc | mashedmod/src/mashed_re/Race/RaceModes.cpp | no | game-mode global DAT_0067e9fc written by the selection switch |
| 0x0067e9fc | mashedmod/src/mashed_re/Race/RaceModes.h | no | real game-mode global DAT_0067e9fc |
| 0x0067e9fc | mashedmod/src/mashed_re/Race/RaceSession.h | no | RaceConfig.gameMode mirrors DAT_0067e9fc |
| 0x0067ea7c | mashedmod/src/mashed_re/Race/RaceSession.h | no | RaceConfig.difficulty mirrors DAT_0067ea7c |
| 0x0067ea80 | mashedmod/src/mashed_re/Race/RaceSession.h | no | RaceConfig.powerUps mirrors DAT_0067ea80 |
| 0x0067ea88 | mashedmod/src/mashed_re/Race/RaceModes.cpp | no | game-length global DAT_0067ea88 (0xff420000 action source) |
| 0x0067ea88 | mashedmod/src/mashed_re/Race/RaceModes.h | no | game length DAT_0067ea88 -> rule (MP/quick-race path, WS-G2) |
| 0x0067f17c | mashedmod/src/mashed_re/Race/RaceSession.h | no | RaceConfig.trackId mirrors DAT_0067f17c |
| 0x007e96fc | mashedmod/src/mashed_re/Race/RaceSession.h | no | device id global DAT_007e96fc (1=joypad 2=keyboard) |
| 0x007e9de0 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | event ring write pointer |
| 0x007e9de4 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | event ring: type array (stride 4) |
| 0x007ea1e4 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | event ring: ctx array (stride 4) |
| 0x007ea5e4 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | event ring: car array (stride 4) |
| 0x007ea9e4 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | event ring: delta array (stride 32, SHL 5) |
| 0x007f0a40 | mashedmod/src/mashed_re/Race/GameFlow.cpp | no | 13x12 cup/unlock table DAT_007f0a40; read live via reinterpret_cast (line 178) |
| 0x007f0a50 | mashedmod/src/mashed_re/Race/GameFlow.cpp | no | track-unlock column base 0x007f0a50 = row*12 + 4 in the cup/unlock table |
| 0x007f0a50 | mashedmod/src/mashed_re/Race/GameFlow.h | no | cup/unlock arrays DAT_007f0a50.. cited for future cup load |
| 0x007f0f2c | mashedmod/src/mashed_re/Race/GameFlow.cpp | no | DAT_007f0f2c savedata gate written into the championship span |
| 0x007f0f38 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | overhead flag DAT_007f0f38 |
| 0x007f0fc8 | mashedmod/src/mashed_re/Race/RaceCamera.h | no | jitter_amp = DAT_007f0fc8 (0.0 live) |
| 0x007f0fcc | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | player-won flag DAT_007f0fcc |
| 0x007f0fd0 | mashedmod/src/mashed_re/Ai/AiController.cpp | no | DAT_007f0fd0 dispatch selector; ==7 force-steps vehicle 0 |
| 0x007f0fd0 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | game_mode_fd0 dispatch selector (NOT the throttle-gate mode) |
| 0x007f0fd0 | mashedmod/src/mashed_re/Ai/AiStandalone.h | no | Host.game_mode_fd0 = DAT_007f0fd0 dispatch selector (4/8/9 variants) |
| 0x007f0fd0 | mashedmod/src/mashed_re/Ai/AiState.h | no | kGameModeFd0 dispatch selector |
| 0x007f0fd0 | mashedmod/src/mashed_re/Race/RaceModes.cpp | no | race-rule global DAT_007f0fd0 the launch action writes (also lines 73/79/97) |
| 0x007f0fd0 | mashedmod/src/mashed_re/Race/RaceModes.h | no | derives the original race-rule global DAT_007f0fd0 (0..10) |
| 0x007f0fd0 | mashedmod/src/mashed_re/Race/RaceSession.h | no | RaceConfig.raceRule mirrors real DAT_007f0fd0 race rule (0..10) |
| 0x007f0fd0 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | GameMode() accessor = DAT_007f0fd0 |
| 0x007f0fd4 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | PrimaryPl() accessor = DAT_007f0fd4 |
| 0x007f0fd8 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | ModePl8() accessor = DAT_007f0fd8 |
| 0x007f0fe4 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | DAT_007f0fe4 compared vs DAT_005d757c (case-10 NaN-aware compare) |
| 0x007f0ff4 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | frame counter (host-ticked) |
| 0x007f0ff8 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | race timer |
| 0x007f0ff8 | mashedmod/src/mashed_re/Ai/AiState.h | no | kFrame0ff8 mode-5 zero |
| 0x007f1008 | mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp | no | DAT_007f1008 int frame-delta accumulated into the timer (kFrameDt line 53) |
| 0x007f1008 | mashedmod/src/mashed_re/Ai/AiState.h | no | kOverrideStep override decrement |
| 0x007f100c | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | dt_blend global DAT_007f100c (pair-switch hysteresis += 2x) |
| 0x007f100c | mashedmod/src/mashed_re/Race/RaceCamera.h | no | dt_blend = DAT_007f100c (1/60 live) |
| 0x007f1014 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | NetFlag() accessor = DAT_007f1014 |
| 0x007f1030 | mashedmod/src/mashed_re/Race/RaceCamera.h | no | time_ticks = DAT_007f1030 equivalent (sway phase) |
| 0x007f1030 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | CtxWord() accessor = DAT_007f1030 |
| 0x007f1038 | mashedmod/src/mashed_re/Ai/AiController.cpp | no | ctrl/cooked-input block base (byte [1] writes) |
| 0x007f1038 | mashedmod/src/mashed_re/Ai/AiState.h | no | kCtrlBlockBase control-output/cooked-input block, stride 0x4c |
| 0x007f103c | mashedmod/src/mashed_re/Ai/AiController.cpp | no | ctrl block bytes [4]/[5]/[6]/[7] (base+4) zero/write |
| 0x007f1a14 | mashedmod/src/mashed_re/Ai/AiState.h | no | kSlotTableBase: slot = *(int*)(0x007f1a14 + v*0x10) |
| 0x007f1a14 | mashedmod/src/mashed_re/Race/RaceSession.h | no | per-car slot state arrays DAT_007f1a14.. mirrored by RaceCar |
| 0x007f1a18 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | TeamId array DAT_007f1a18 (stride 16; also raw pointer walks lines 298+) |
| 0x007f1a1c | mashedmod/src/mashed_re/Race/RaceSession.cpp | no | Player Colour/Car Select writes DAT_007f1a1c.. (TODO-RE vehicle table) |
| 0x007f1a50 | mashedmod/src/mashed_re/Ai/AiState.h | no | kDbgSplineEn debug override enable |
| 0x007f1a58 | mashedmod/src/mashed_re/Ai/AiNavHooks.cpp | no | debug screen extent read (param_6==1 path) |
| 0x007f1a58 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | team-array end bound 0x7f1a58 (pointer-walk loop terminator) |
| 0x007f1a5c | mashedmod/src/mashed_re/Ai/AiNavHooks.cpp | no | debug screen extent read (param_6==1 path) |
| 0x007f1a68 | mashedmod/src/mashed_re/Ai/AiState.h | no | kDbgSplineType debug line type |
| 0x007f1a6c | mashedmod/src/mashed_re/Ai/AiState.h | no | kDbgSplineIdx debug spline index |
| 0x007f1a9c | mashedmod/src/mashed_re/Ai/AiData.h | no | controller expected memory image base = tile grid 128x128 int16 |
| 0x007f1a9c | mashedmod/src/mashed_re/Ai/AiLineOfSight.cpp | no | tile grid 128x128 shorts (kTileGrid), read in LosWallAt |
| 0x007f1a9c | mashedmod/src/mashed_re/Ai/AiNavHooks.cpp | no | kTileGrid 128x128 shorts |
| 0x007f1a9c | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | AI tile grid (128x128 shorts) |
| 0x007f1a9c | mashedmod/src/mashed_re/Ai/AiStandalone.h | no | AI tile grid marched by original LOS wall-march |
| 0x007f1a9c | mashedmod/src/mashed_re/Ai/AiTargeting.cpp | no | track tile grid, 128x128 shorts (kTileGrid); cited at 0x00416060 / 0x004150e0 |
| 0x007f1a9c | mashedmod/src/mashed_re/Ai/AiWallLateral.cpp | no | DAT_007f1a9c tile grid 128x128 shorts (kTileGrid), read in WallLatAt |
| 0x007f9a9c | mashedmod/src/mashed_re/Ai/AiData.h | no | sub-cell region abs addr (payload +0x08000) |
| 0x007f9a9c | mashedmod/src/mashed_re/Ai/AiLineOfSight.cpp | no | 8x8 sub-cell chars per tile (kSubCellGrid), read in LosWallAt |
| 0x007f9a9c | mashedmod/src/mashed_re/Ai/AiNavHooks.cpp | no | kSubCellGrid 8x8 sub-cell chars/tile |
| 0x007f9a9c | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | AI sub-cell grid (Phase-8 LOS wall-march) |
| 0x007f9a9c | mashedmod/src/mashed_re/Ai/AiStandalone.h | no | AI sub-cell grid |
| 0x007f9a9c | mashedmod/src/mashed_re/Ai/AiTargeting.cpp | no | 8x8 sub-cell chars per tile (kSubCellGrid) |
| 0x007f9a9c | mashedmod/src/mashed_re/Ai/AiWallLateral.cpp | no | DAT_007f9a9c 8x8 sub-cell chars per tile (kSubCellGrid), read in WallLatAt |
| 0x00801aa0 | mashedmod/src/mashed_re/Ai/AiData.h | no | race bank abs addr (payload +0x10004) |
| 0x00801aa0 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | race-line spline arrays populated by .AI parser |
| 0x00801aa0 | mashedmod/src/mashed_re/Ai/AiState.h | no | kSplineRace type 0, stride 0x204, count +0x200 |
| 0x00801ca0 | mashedmod/src/mashed_re/Ai/AiController.cpp | no | DAT_00801ca0 race-line count guard > 3 |
| 0x00801ca0 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | race-line spline count guard (>3 = .AI loaded) |
| 0x00801ca0 | mashedmod/src/mashed_re/Ai/AiStandalone.h | no | race-line spline count guard (DAT_00801ca0 > 3) |
| 0x00801ca0 | mashedmod/src/mashed_re/Ai/AiState.h | no | kSplineRaceCnt |
| 0x008020ac | mashedmod/src/mashed_re/Ai/AiData.h | no | inside bank abs addr (payload +0x10610) |
| 0x008020ac | mashedmod/src/mashed_re/Ai/AiState.h | no | kSplineInside type 1 |
| 0x008022ac | mashedmod/src/mashed_re/Ai/AiState.h | no | kSplineInsideCnt |
| 0x008026b8 | mashedmod/src/mashed_re/Ai/AiData.h | no | slow bank abs addr (payload +0x10c1c) |
| 0x008026b8 | mashedmod/src/mashed_re/Ai/AiState.h | no | kSplineSlow type 2 |
| 0x008028b8 | mashedmod/src/mashed_re/Ai/AiState.h | no | kSplineSlowCnt |
| 0x00802cc4 | mashedmod/src/mashed_re/Ai/AiData.h | no | cheat bank abs addr (payload +0x11228) |
| 0x00802cc4 | mashedmod/src/mashed_re/Ai/AiState.h | no | kSplineCheat type 3 |
| 0x00802ec4 | mashedmod/src/mashed_re/Ai/AiState.h | no | kSplineCheatCnt |
| 0x008032d4 | mashedmod/src/mashed_re/Ai/AiNavHooks.cpp | no | kNearestCache per-car int cache, stride 5 ints |
| 0x008032d4 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | per-vehicle nearest-index cache, stride 0x14 (read/written Phase 2) |
| 0x008032d4 | mashedmod/src/mashed_re/Ai/PromoLoop_round1.cpp | no | DAT_008032d4 timer table base, per-slot stride 5 dwords; written 1000 (code lines 124/132) |
| 0x008032d8 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | per-vehicle hist(other) float, stride 0x14, written by steer bands |
| 0x008032dc | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | per-vehicle hist(this) float, stride 0x14 |
| 0x008032e0 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | per-vehicle prevSpeed, stride 0x14 |
| 0x00803324 | mashedmod/src/mashed_re/Ai/PromoLoop_round1.cpp | no | 'all' loop upper bound (exclusive) of the 0x008032d4 table — exactly 5 slots (code line 125) |
| 0x008815a4 | mashedmod/src/mashed_re/Race/CameraClusterHooks.cpp | no | alive field base 0x008815a4 (stride 0xd04) — read via the forwarded getter |
| 0x00881ec8 | mashedmod/src/mashed_re/Ai/AiTargeting.cpp | no | per-vehicle velocity source base + v*0xd04 (AiVehicleVelocity3 transform src) |
| 0x00881ec8 | mashedmod/src/mashed_re/Ai/VehicleVelocityWorldGet.cpp | no | per-vehicle struct base (kVehBase_881ec8), src of transform, + idx*0xd04 |
| 0x00881f74 | mashedmod/src/mashed_re/Ai/AiTargeting.cpp | no | transformed velocity float3 dst base + v*0xd04 (stride 0x341 dwords); out[0] read line 137 |
| 0x00881f74 | mashedmod/src/mashed_re/Ai/VehicleVelocityWorldGet.cpp | no | velocity float3 X (+0xac) dst/read (kVelX_881f74) |
| 0x00881f78 | mashedmod/src/mashed_re/Ai/AiTargeting.cpp | no | velocity Y dword copied to out[1] |
| 0x00881f78 | mashedmod/src/mashed_re/Ai/VehicleVelocityWorldGet.cpp | no | velocity float3 Y (kVelY_881f78) |
| 0x00881f7c | mashedmod/src/mashed_re/Ai/AiTargeting.cpp | no | velocity Z dword copied to out[2] |
| 0x00881f7c | mashedmod/src/mashed_re/Ai/VehicleVelocityWorldGet.cpp | no | velocity float3 Z (kVelZ_881f7c) |
| 0x00881f90 | mashedmod/src/mashed_re/Race/CameraClusterHooks.cpp | no | dead field base 0x00881f90 (stride 0xd04) — read via the forwarded getter |
| 0x00897fe0 | mashedmod/src/mashed_re/Race/RaceCamera.h | no | camera struct base 0x00897fe0 (member offsets cited against its fields) |
| 0x00898980 | mashedmod/src/mashed_re/Race/RaceCamera.cpp | no | required-zoom output cam[0x268] = DAT_00898980 |
| 0x008989b0 | mashedmod/src/mashed_re/Ai/PromoLoop_round1.cpp | no | DAT_008989b0 4-entry per-vehicle float array base, stride 4 (code line 86) |
| 0x008991b8 | mashedmod/src/mashed_re/Ai/PromoLoop_round1.cpp | no | int32 source of HudOverlayFloatGet FILD (0x008991B8 in instruction cite; code line 188) |
| 0x0089a360 | mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp | no | DAT_0089a360 float, __ftol'd at function entry (kFlt360) |
| 0x0089a364 | mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp | no | DAT_0089a364 index global (kIdx364, -1 sentinel) |
| 0x0089a368 | mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp | no | DAT_0089a368 mode global (==2 early-out); also re-exec'd in trampoline asm lines 138/203 |
| 0x0089a374 | mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp | no | DAT_0089a374 bias added into limit-table index (kBias374) |
| 0x0089a4c4 | mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp | no | DAT_0089a4c4 per-vehicle rank counter int at +v*0x74 (kRankBase line 52) |
| 0x0089a4c8 | mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp | no | DAT_0089a4c8 per-vehicle catch-up timer int at +v*0x74 (kTimerBase line 51); part of A/B snapshot surface |
| 0x0089a4cc | mashedmod/src/mashed_re/Ai/AiState.h | no | kAiStateBase per-vehicle behavior/path record, stride 0x74; +0x00 line TYPE |
| 0x0089a4d0 | mashedmod/src/mashed_re/Ai/AiState.h | no | spline INDEX field +0x04 |
| 0x0089a4e4 | mashedmod/src/mashed_re/Ai/AiState.h | no | frustration timer +0x1c |
| 0x0089a4ec | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | per-vehicle timerState, stride 0x74 |
| 0x0089a4f0 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | per-vehicle timerStart, stride 0x74 |
| 0x0089a4f4 | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | per-vehicle storedSteer, stride 0x74 |
| 0x0089a4f8 | mashedmod/src/mashed_re/Ai/AiState.h | no | kAiMode5Countdown +0x2c |
| 0x0089a4fc | mashedmod/src/mashed_re/Ai/AiController.cpp | no | input-override countdown (per-vehicle +0x30) |
| 0x0089a4fc | mashedmod/src/mashed_re/Ai/AiState.h | no | kAiOverrideTimer +0x30 |
| 0x0089a500 | mashedmod/src/mashed_re/Ai/AiNavHooks.cpp | no | kArmFlagBase FUN_00416230 slot, stride 0x74 |
| 0x0089a50c | mashedmod/src/mashed_re/Ai/AiController.cpp | no | stored ctrl replay byte [0] |
| 0x0089a50c | mashedmod/src/mashed_re/Ai/AiState.h | no | kAiReplayB0 stored ctrl replay +0x40 |
| 0x0089a510 | mashedmod/src/mashed_re/Ai/AiController.cpp | no | stored ctrl replay byte [1] |
| 0x0089a510 | mashedmod/src/mashed_re/Ai/AiState.h | no | kAiReplayB1 +0x44 |
| 0x0089a514 | mashedmod/src/mashed_re/Ai/AiController.cpp | no | stored ctrl replay byte [4] |
| 0x0089a514 | mashedmod/src/mashed_re/Ai/AiState.h | no | kAiReplayB4 +0x48 |
| 0x0089a518 | mashedmod/src/mashed_re/Ai/AiController.cpp | no | stored ctrl replay byte [5] |
| 0x0089a518 | mashedmod/src/mashed_re/Ai/AiState.h | no | kAiReplayB5 +0x4c |
| 0x0089a52c | mashedmod/src/mashed_re/Ai/AiController.cpp | no | committed behaviour mode DAT_0089a52c[v] (mode-10 skip) |
| 0x0089a52c | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | no | per-vehicle behaviour mode, stride 0x74 (committed in ControlStep) |
| 0x0089a52c | mashedmod/src/mashed_re/Ai/AiState.h | no | behavior MODE +0x60 (FUN_00416250 commit) |
| 0x008a94c0 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | ElimOrder array DAT_008a94c0 |
| 0x008a94d0 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | Participants() accessor = DAT_008a94d0 |
| 0x008a94e0 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | score array DAT_008a94e0 (stride 4) |
| 0x008a9500 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | PostClamp array DAT_008a9500 |
| 0x008a9510 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | Timer array DAT_008a9510 (set to 6000) |
| 0x008a9520 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | DeltaDisp array DAT_008a9520 |
| 0x008a9570 | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | no | PrevScore array DAT_008a9570 |
| 0x008a9640 | mashedmod/src/mashed_re/Ai/PromoLoop_round1.cpp | no | per-vehicle record base cited in derivation: 0x008a96dc = 0x008a9640 + 0x9c |
| 0x008a96dc | mashedmod/src/mashed_re/Ai/PromoLoop_round1.cpp | no | DAT_008a96dc field base inside per-vehicle record, stride 0x30c; returned as VA constant (code line 53) |

### Unmapped citations

[UNCERTAIN] These cited RVAs are neither data-global-kind citations nor present in hooks.csv; the missing evidence is a hooks.csv row (or a Ghidra function-existence check) for each address. They are reported as-is and were NOT used as BFS seeds. Note: `0x00000000` entries are float-bit-pattern literals that matched the extraction regex (flagged non-addresses in the extraction pass). Wildcard-masked address prefixes were excluded from extraction entirely; the one occurrence in the roots sources is `0x0043f3xx` in `mashedmod/src/mashed_re/Race/RaceModes.cpp` line 44 (masked prefix, not a complete RVA).

| RVA | kind | source file | context |
|---|---|---|---|
| 0x00000000 | other | mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp | float32 bit pattern 0.0f passed to Cf(); NOT a binary address (regex-matched literal) |
| 0x00000000 | other | mashedmod/src/mashed_re/Ai/AiLineOfSight.cpp | float32 bit pattern 0.0f passed to Cf(); NOT a binary address (regex-matched literal) |
| 0x00000000 | other | mashedmod/src/mashed_re/Ai/AiNavHooks.cpp | float32 bit pattern 0.0f passed to Cf() (kZero, line 44); NOT a binary address (regex-matched literal) |
| 0x00000000 | other | mashedmod/src/mashed_re/Ai/AiSplineHooks.cpp | float32 bit pattern 0.0 in DAT_005d757c value comment (line 55); NOT a binary address (regex-matched literal) |
| 0x00000000 | other | mashedmod/src/mashed_re/Ai/AiWallAhead.cpp | float32 bit pattern 0.0f passed to Cf(); NOT a binary address (regex-matched literal) |
| 0x00408aff | other | mashedmod/src/mashed_re/Ai/PromoLoop_round1.cpp | end of FUN_00408af0 body range (15 bytes) — range bound |
| 0x0040b2a4 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: param_2 clamp < 0 |
| 0x0040b2b1 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: FUN_0040e370 loop call site |
| 0x0040b2c4 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: all-4-active check |
| 0x0040b2ca | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: FUN_0042f500 call site |
| 0x0040b2db | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: FUN_0042f500 call site |
| 0x0040b2e4 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: team compare |
| 0x0040b300 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: primary-player compare |
| 0x0040b308 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: zero delta on non-primary |
| 0x0040b30a | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: mode-2 branch |
| 0x0040b31e | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: TeamId==1 zero delta |
| 0x0040b329 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: FUN_0040e340 call site |
| 0x0040b344 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: delta 2 -> 1 |
| 0x0040b349 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: prev-score snapshot |
| 0x0040b350 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: prev-score snapshot |
| 0x0040b357 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: NetFlag read |
| 0x0040b360 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: DeltaDisp write |
| 0x0040b367 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: network negative clamp |
| 0x0040b36f | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: PostClamp write |
| 0x0040b376 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: FUN_0040e350 call site |
| 0x0040b394 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: RingPtr read |
| 0x0040b39a | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: CtxWord read before branch |
| 0x0040b39f | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | ring stride byte-verified start / RingType write site (also line 155) |
| 0x0040b3b0 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: RingCtx write |
| 0x0040b3bd | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: RingCar write |
| 0x0040b3cd | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | ring stride byte-verified end / RingDelta write site SHL 5 (also line 158) |
| 0x0040b3d8 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: RingPtr increment |
| 0x0040b3d9 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: RingPtr wrap at 0xff |
| 0x0040b3eb | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: score accumulate |
| 0x0040b3f2 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: Timer = 6000 (0x1770) |
| 0x0040b3fd | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: floor score at 0 |
| 0x0040e2f3 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | tail-fixup block start in FUN_0040e180 (0x0040e2f3..0x0040e330) |
| 0x0040e330 | other | mashedmod/src/mashed_re/Race/CameraClusterHooks.cpp | decomp range end (0x0040e180..0x0040e330) |
| 0x0040e330 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | tail-fixup block end |
| 0x0040e355 | other | mashedmod/src/mashed_re/Ai/PromoLoop_round1.cpp | end of FUN_0040e350 body range (5 bytes) — range bound |
| 0x0040eeed | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: FUN_0042f6a0 race-type call site |
| 0x0040ef00 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: omitted wprintf call site |
| 0x0040ef20 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: count-alive loop |
| 0x0040ef45 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: omitted wprintf call site |
| 0x0040ef56 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | code cite: numcarsremaining == 1 branch |
| 0x0040f69e | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | LAB_0040f69e straggler-pick label (teams, 3 remaining) |
| 0x0040fbbb | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | LAB_0040fbbb progress-equalize tail (goto target; fVar1 hoist note) |
| 0x0041062a | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | LAB_0041062a conclusion block label (result-state writes) |
| 0x004107e3 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | LAB_004107e3 player-won label |
| 0x00410801 | other | mashedmod/src/mashed_re/Race/ScoringHooks.cpp | LAB_00410801 draw-result label |
| 0x00410ee3 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | fcomp [0x005cc55c] exact-equality saturation compare in FUN_00410d10 |
| 0x00410ee3 | other | mashedmod/src/mashed_re/Race/RaceCamera.h | exact-equality fcomp at 0x00410ee3 (10.0 saturation) |
| 0x00410efb | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | call site of 0x0040e180 inside the elimination check |
| 0x00410f4a | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | lap-wrap adjust block start (0x00410f4a..0x00410fa6) |
| 0x00410fa6 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | lap-wrap adjust block end |
| 0x00410fe2 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | straggler swap-dance block start (0x00410fe2..0x00411014) |
| 0x00411014 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | straggler swap-dance block end |
| 0x00411082 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | round-end tail at <=1 alive |
| 0x0041405f | other | mashedmod/src/mashed_re/Ai/PromoLoop_round1.cpp | end of FUN_00414030 body range (47 bytes) — range bound |
| 0x004148b5 | call-original | mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp | trampoline continuation: jmp into original after re-exec of clobbered MOV EAX prologue (g_orig_4148b5 line 135) |
| 0x00414a68 | other | mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp | end of FUN_004148b0 body range (0x004148b0..0x00414a68, 440 bytes) — range bound, not a function root |
| 0x004150e4 | other | mashedmod/src/mashed_re/Ai/AiWallLateral.cpp | end of hook clobber range 0x004150e0..0x004150e4 (FLD first byte); earlier jmp-here bug documented line 80 |
| 0x004150e7 | call-original | mashedmod/src/mashed_re/Ai/AiWallLateral.cpp | trampoline continuation: jmp to first intact instr FMUL [0x005cc35c] after re-exec'd MOV+FLD (g_orig_4150e7 line 82) |
| 0x0041518f | other | mashedmod/src/mashed_re/Ai/AiWallLateral.cpp | end of FUN_004150e0 body range (0x004150e0..0x0041518f, 175 bytes) — range bound |
| 0x00415d04 | other | mashedmod/src/mashed_re/Ai/AiWallAhead.cpp | end of 5-byte hook clobber range 0x00415d00..0x00415d04 (MOV ESI first byte begins here) — trampoline doc |
| 0x00415d08 | call-original | mashedmod/src/mashed_re/Ai/AiWallAhead.cpp | trampoline continuation: jmp to first intact instruction after re-exec'd SUB/PUSH/MOV (g_orig_415d08 line 94) |
| 0x00415e19 | other | mashedmod/src/mashed_re/Ai/AiWallAhead.cpp | end of FUN_00415d00 body range (0x00415d00..0x00415e19, 281 bytes) — range bound |
| 0x00416067 | call-original | mashedmod/src/mashed_re/Ai/AiLineOfSight.cpp | trampoline continuation: jmp after re-exec'd SUB ESP,0x18 + MOV EAX (g_orig_416067 line 96) |
| 0x004161df | other | mashedmod/src/mashed_re/Ai/AiLineOfSight.cpp | end of FUN_00416060 body range (0x00416060..0x004161df) — range bound |
| 0x00416256 | call-original | mashedmod/src/mashed_re/Ai/AiLeaderTimer.cpp | orchestrator trampoline continuation g_orch_416256; jmp after re-exec'd PUSH EBP/MOV EBP,ESP/AND ESP,-8 |
| 0x00416256 | other | mashedmod/src/mashed_re/Ai/AiWallAhead.cpp | jmp target in the re-add recipe for the REMOVED 0x00416250 coverage driver (comment only, no live code) |
| 0x004165c0 | other | mashedmod/src/mashed_re/Ai/AiStandalone.cpp | asm listing anchor of ControlStep steer-band code inside FUN_00416250 |
| 0x0041856c | other | mashedmod/src/mashed_re/Ai/AiState.h | instruction anchor citing kSlotTableBase |
| 0x00418572 | other | mashedmod/src/mashed_re/Ai/AiState.h | instruction anchor citing kCtrlBlockStride 0x4c |
| 0x00418575 | other | mashedmod/src/mashed_re/Ai/AiState.h | instruction anchor citing kCtrlBlockBase |
| 0x0041861f | other | mashedmod/src/mashed_re/Ai/AiData.h | instruction anchor citing kAiSplineStride 0x204 |
| 0x00418625 | other | mashedmod/src/mashed_re/Ai/AiData.h | instruction anchor citing kAiSplineCountOff 0x200 |
| 0x0042f6b8 | other | mashedmod/src/mashed_re/Race/RaceModes.cpp | switch base 0x0042f6b8 inside 0x0042f6b0 |
| 0x0042f6bc | other | mashedmod/src/mashed_re/Race/RaceModes.cpp | case address: sel 0 -> mode 2 |
| 0x0042f6c6 | other | mashedmod/src/mashed_re/Race/RaceModes.cpp | case address: sel 1 -> mode 3 |
| 0x0042f6d0 | other | mashedmod/src/mashed_re/Race/RaceModes.cpp | case address: sel 2 -> mode 4 |
| 0x0042f6da | other | mashedmod/src/mashed_re/Race/RaceModes.cpp | case address: sel 3 -> mode 6 |
| 0x0042f6e4 | other | mashedmod/src/mashed_re/Race/RaceModes.cpp | case address: sel 5 -> mode 5 |
| 0x0042f6ee | other | mashedmod/src/mashed_re/Race/RaceModes.cpp | case address: sel 8 -> mode 7 |
| 0x0042f6f8 | other | mashedmod/src/mashed_re/Race/RaceModes.cpp | case address: sel 9 -> mode 8 |
| 0x0042f702 | other | mashedmod/src/mashed_re/Race/RaceModes.cpp | case address: sel 10 -> mode 9 |
| 0x0042f70c | other | mashedmod/src/mashed_re/Race/RaceModes.cpp | case address: sel 11 -> mode 10 |
| 0x0043f7a7 | other | mashedmod/src/mashed_re/Race/RaceModes.cpp | pre-switch default write address (DAT_007f0fd0 = 0) |
| 0x00442cd6 | other | mashedmod/src/mashed_re/Ai/PromoLoop_round1.cpp | end of FUN_00442cc0 body range (23 bytes) — range bound |
| 0x00443304 | other | mashedmod/src/mashed_re/Ai/AiSplineHooks.cpp | re-exec'd PUSH EBX instruction address in trampoline (line 278) — not a function root |
| 0x00443305 | call-original | mashedmod/src/mashed_re/Ai/AiSplineHooks.cpp | trampoline continue addr after re-executed first 5 bytes (g_orig_443305) |
| 0x0044330a | other | mashedmod/src/mashed_re/Ai/AiSplineHooks.cpp | instruction anchor in C reimpl (segIdx decrement) |
| 0x00443311 | other | mashedmod/src/mashed_re/Ai/AiSplineHooks.cpp | instruction anchor (P0 wrap +count) |
| 0x00443317 | other | mashedmod/src/mashed_re/Ai/AiSplineHooks.cpp | instruction anchor (count load) |
| 0x0044331d | other | mashedmod/src/mashed_re/Ai/AiSplineHooks.cpp | instruction anchor (&P0) |
| 0x00443327 | other | mashedmod/src/mashed_re/Ai/AiSplineHooks.cpp | instruction anchor (&P1) |
| 0x00443331 | other | mashedmod/src/mashed_re/Ai/AiSplineHooks.cpp | instruction anchor (&P2) |
| 0x004433cf | other | mashedmod/src/mashed_re/Ai/AiSplineHooks.cpp | FSTP float32 store of X inside FUN_00443300 |
| 0x00443427 | other | mashedmod/src/mashed_re/Ai/AiSplineHooks.cpp | FSTP float32 store of Z inside FUN_00443300 |
| 0x00443432 | other | mashedmod/src/mashed_re/Ai/AiSplineHooks.cpp | end of FUN_00443300 body range |
| 0x00443dc6 | call-original | mashedmod/src/mashed_re/Ai/AiNavHooks.cpp | trampoline continue addr after re-executed SUB ESP,0x1a0 (g_orig_443dc6) |
| 0x00444b5b | other | mashedmod/src/mashed_re/Ai/AiNavHooks.cpp | end of FUN_00443dc0 body range (0x00443dc0..0x00444b5b) |
| 0x00446b30 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | velocity-lead block start (0x00446b30..0x00446b6a) |
| 0x00446b6a | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | velocity-lead block end |
| 0x00446bf0 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | separation block start (0x00446bf0..0x00446c50) |
| 0x00446c50 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | separation block end |
| 0x00446c70 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | degenerate-pair (sole dying car -> 8.0) block |
| 0x00446cd0 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | pair-switch hysteresis block start (0x00446cd0..0x00446ec0) |
| 0x00446ec0 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | pair-switch hysteresis block end |
| 0x00446f91 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | path-sample block start in FUN_00446520 (0x00446f91..0x00447081; also line 241) |
| 0x00446f91 | other | mashedmod/src/mashed_re/Race/RaceCamera.h | path-sample block start (banker's round; 0x00446f91..0x00447081) |
| 0x00446fd5 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | node wrap compare address |
| 0x00447081 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | path-sample block end |
| 0x00447081 | other | mashedmod/src/mashed_re/Race/RaceCamera.h | path-sample block end |
| 0x004471f0 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | last-survivor zoom-out block |
| 0x004473a1 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | view-dir pitch block start (0x004473a1..0x004473ff) |
| 0x004473f3 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | dir_elev = ang - 90 block (cited 0x004473f3..f9) |
| 0x004473ff | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | view-dir pitch block end |
| 0x00447402 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | camera-offset scale block start (0x00447402..0x0044741a) |
| 0x0044741a | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | camera-offset scale block end |
| 0x0044744a | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | offset normalize call site |
| 0x0044745c | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | axis-cross block start (0x0044745c..0x004474a8) |
| 0x004474a8 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | axis-cross block end |
| 0x004474e0 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | City (track_type 0x1a) branch address |
| 0x00447536 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | overhead pitch = 90 write address |
| 0x00447557 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | RotateAboutAxis pitch call site |
| 0x00447570 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | RotateAboutAxis pitch call site |
| 0x00447578 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | distance-law block (the SciLor block) |
| 0x004475d6 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | overhead cam-pos block |
| 0x00447614 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | height-mix block start (0x00447614..) |
| 0x004477e6 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | snap-vs-spring block start (0x004477e6..0x0044798f) |
| 0x0044798f | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | snap-vs-spring block end |
| 0x00447c26 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | sway oscillator block start (0x00447c26..0x00447ee4) |
| 0x00447ee4 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | sway oscillator block end |
| 0x00447eea | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | target-jitter block start (0x00447eea..0x00447f4a) |
| 0x00447f4a | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | target-jitter block end |
| 0x00447f50 | other | mashedmod/src/mashed_re/Race/RaceCamera.cpp | write-out block start (0x00447f50..) |

## 2. Executed-set closure

Static reachability closure (NOT measured execution — see disclaimer and section 6):

- Function roots (BFS seeds): **116**
- Closure size (roots ∪ reachable): **2962** RVAs — 1174 with a hooks.csv row, 1788 without
- Depth histogram: d0: 116, d1: 466, d2: 219, d3: 263, d4: 439, d5: 355, d6: 408, d7: 278, d8: 131, d9: 108, d10: 75, d11: 16, d12: 45, d13: 28, d14: 15
- Closure truncation leaves (no callee data in the merged call graph): **2134** (346 of them have hooks.csv rows)
- Library share (of the 1174 hooks.csv closure members): **36** (3.1%) — counted in closure totals, excluded from gap ranking
- Confidence split of hooks.csv closure members: C1: 6, C2: 760, C3: 352, C4: 56
- In closure but not in hooks.csv: **1788** (full list in section 4)

Full closure table (hooks.csv members), sorted by subsystem then RVA:

| RVA | name | subsystem | C | depth | in-degree | first caller |
|---|---|---|---|---|---|---|
| 0x00407a40 | Table8a9640Get | ai | C3 | 0 | 5 | (root) |
| 0x00408af0 | AiVehicleFieldPtrGet | ai | C3 | 0 | 4 | (root) |
| 0x0040e350 | GetRenderSubMode | ai | C3 | 0 | 19 | (root) |
| 0x0040e4a0 | ElapsedTimeGet | ai | C3 | 0 | 4 | (root) |
| 0x00413fe0 | FUN_00413fe0 | ai | C3 | 0 | 2 | (root) |
| 0x00414030 | AiSplineBankTimerReset | ai | C3 | 0 | 1 | (root) |
| 0x00414300 | FUN_00414300 | ai | C2 | 1 | 3 | 0x00414a70 |
| 0x00414490 | FUN_00414490 | ai | C2 | 1 | 1 | 0x00414c30 |
| 0x00414570 | FUN_00414570 | ai | C2 | 0 | 3 | (root) |
| 0x004148b0 | FUN_004148b0 | ai | C3 | 0 | 3 | (root) |
| 0x00414a70 | FUN_00414a70 | ai | C2 | 0 | 4 | (root) |
| 0x00414c30 | FUN_00414c30 | ai | C2 | 0 | 8 | (root) |
| 0x00414f00 | FUN_00414f00 | ai | C2 | 0 | 7 | (root) |
| 0x00415020 | AiLastPlaceFrustration | ai | C3 | 0 | 3 | (root) |
| 0x004150e0 | FUN_004150e0 | ai | C3 | 0 | 3 | (root) |
| 0x00415190 | FUN_00415190 | ai | C2 | 0 | 2 | (root) |
| 0x00415200 | FUN_00415200 | ai | C2 | 0 | 2 | (root) |
| 0x00415220 | FUN_00415220 | ai | C2 | 0 | 8 | (root) |
| 0x00415860 | InteractionCooldownSet | ai | C3 | 1 | 2 | 0x00415880 |
| 0x00415880 | FUN_00415880 | ai | C2 | 0 | 6 | (root) |
| 0x00415d00 | FUN_00415d00 | ai | C3 | 0 | 2 | (root) |
| 0x00415e20 | FUN_00415e20 | ai | C2 | 0 | 2 | (root) |
| 0x00416060 | FUN_00416060 | ai | C3 | 0 | 3 | (root) |
| 0x004161e0 | FUN_004161e0 | ai | C2 | 0 | 3 | (root) |
| 0x00416250 | FUN_00416250 | ai | C2 | 0 | 14 | (root) |
| 0x00416a30 | FUN_00416a30 | ai | C2 | 0 | 8 | (root) |
| 0x00417180 | FUN_00417180 | ai | C2 | 0 | 2 | (root) |
| 0x00417640 | FUN_00417640 | ai | C2 | 0 | 4 | (root) |
| 0x004177b0 | FUN_004177b0 | ai | C2 | 0 | 1 | (root) |
| 0x00417cf0 | FUN_00417cf0 | ai | C2 | 0 | 1 | (root) |
| 0x00417da0 | FUN_00417da0 | ai | C2 | 0 | 10 | (root) |
| 0x00418560 | FUN_00418560 | ai | C2 | 0 | 5 | (root) |
| 0x00418860 | FUN_00418860 | ai | C2 | 0 | 1 | (root) |
| 0x0041da90 | DeltaTimeOutGet | ai | C3 | 6 | 1 | 0x00462f50 |
| 0x0041f030 | TriggerStructRead | ai | C3 | 1 | 2 | 0x00414c30 |
| 0x00422b50 | VehicleDamageAccum | ai | C3 | 8 | 1 | 0x0046d780 |
| 0x00422ba0 | FUN_00422ba0 | ai | C2 | 2 | 3 | 0x004252c0 |
| 0x004233e0 | FUN_004233e0 | ai | C2 | 1 | 2 | 0x00443440 |
| 0x00423480 | AiFilenameBuild | ai | C3 | 1 | 1 | 0x004235b0 |
| 0x00423540 | FUN_00423540 | ai | C2 | 0 | 1 | (root) |
| 0x004252c0 | FUN_004252c0 | ai | C2 | 1 | 3 | 0x00407a40 |
| 0x00426c00 | FUN_00426c00 | ai | C3 | 0 | 15 | (root) |
| 0x00442cc0 | AiVehicleFloat4Get | ai | C3 | 0 | 6 | (root) |
| 0x00443080 | AiTargetEnableGet | ai | C3 | 0 | 6 | (root) |
| 0x00443300 | FUN_00443300 | ai | C3 | 0 | 2 | (root) |
| 0x00443440 | FUN_00443440 | ai | C2 | 0 | 5 | (root) |
| 0x00443d10 | FUN_00443d10 | ai | C2 | 0 | 2 | (root) |
| 0x00443dc0 | FUN_00443dc0 | ai | C2 | 0 | 2 | (root) |
| 0x00452160 | PowerupTargetPtrGet | ai | C3 | 0 | 2 | (root) |
| 0x00452ea0 | Table88ff50Get | ai | C3 | 0 | 2 | (root) |
| 0x00452eb0 | PowerupRangeGet | ai | C3 | 0 | 2 | (root) |
| 0x00455b40 | PowerupTable6885e0Get | ai | C3 | 1 | 1 | 0x00415220 |
| 0x0045a0f0 | VehPwrState68ba00Get | ai | C3 | 1 | 2 | 0x00415220 |
| 0x0046c730 | Table882198Get | ai | C3 | 6 | 2 | 0x004642f0 |
| 0x0046c750 | Table882194Get | ai | C3 | 7 | 1 | 0x0046c730 |
| 0x0046cc10 | FUN_0046cc10 | ai | C2 | 1 | 6 | 0x00415880 |
| 0x0046d4a0 | PtrCompute881ec8 | ai | C3 | 0 | 22 | (root) |
| 0x0046d510 | VehicleVelocityWorldGet | ai | C3 | 0 | 11 | (root) |
| 0x0046d570 | FUN_0046d570 | ai | C2 | 0 | 2 | (root) |
| 0x0046d6a0 | VehTbl8820acGet1 | ai | C3 | 0 | 3 | (root) |
| 0x0046d6d0 | VehTbl881f84Get1 | ai | C3 | 0 | 11 | (root) |
| 0x0046d780 | FUN_0046d780 | ai | C2 | 7 | 3 | 0x0046c730 |
| 0x0046d7f0 | FUN_0046d7f0 | ai | C3 | 8 | 1 | 0x0046c750 |
| 0x004715a0 | FUN_004715a0 | ai | C2 | 6 | 1 | 0x00426e10 |
| 0x004783f0 | FUN_004783f0 | ai | C2 | 7 | 1 | 0x00478660 |
| 0x00478660 | FUN_00478660 | ai | C2 | 6 | 1 | 0x00426e10 |
| 0x00484c70 | WorldObjectsBaseGet | ai | C3 | 1 | 1 | 0x00414c30 |
| 0x0048a630 | FUN_0048a630 | ai | C3 | 1 | 1 | 0x00414c30 |
| 0x0042f760 | Global67f19cGet | audio | C3 | 6 | 1 | 0x004631f0 |
| 0x0042f770 | Global67f1a0Get | audio | C3 | 6 | 1 | 0x004631f0 |
| 0x0042f780 | Global67f1a4Get | audio | C3 | 6 | 1 | 0x004631f0 |
| 0x00431b20 | FUN_00431b20 | audio | C2 | 6 | 2 | 0x0045dd60 |
| 0x00432230 | RecEq231 | audio | C3 | 6 | 2 | 0x004631f0 |
| 0x00432260 | RecEq232 | audio | C3 | 6 | 1 | 0x004631f0 |
| 0x004522d0 | FUN_004522d0 | audio | C2 | 6 | 5 | 0x005a71f0 |
| 0x0045d460 | FUN_0045d460 | audio | C2 | 4 | 2 | 0x004669b0 |
| 0x0045daf0 | FUN_0045daf0 | audio | C2 | 4 | 5 | 0x00465c10 |
| 0x0045dc80 | FUN_0045dc80 | audio | C2 | 5 | 3 | 0x00466b50 |
| 0x0045dce0 | FUN_0045dce0 | audio | C2 | 12 | 1 | 0x00406160 |
| 0x0045dd60 | FUN_0045dd60 | audio | C2 | 5 | 1 | 0x00466b50 |
| 0x0045e040 | FUN_0045e040 | audio | C2 | 7 | 1 | 0x00467010 |
| 0x0045e0f0 | FUN_0045e0f0 | audio | C2 | 2 | 11 | 0x004645c0 |
| 0x004623e0 | FUN_004623e0 | audio | C2 | 5 | 1 | 0x00466b50 |
| 0x004624c0 | FUN_004624c0 | audio | C2 | 5 | 1 | 0x004627f0 |
| 0x00462520 | FUN_00462520 | audio | C2 | 5 | 1 | 0x00466b50 |
| 0x004625b0 | FUN_004625b0 | audio | C2 | 0 | 1 | (root) |
| 0x004627b0 | AudioStateActive | audio | C3 | 1 | 3 | 0x00462dd0 |
| 0x004627f0 | FUN_004627f0 | audio | C2 | 4 | 1 | 0x004669b0 |
| 0x00462dd0 | FUN_00462dd0 | audio | C2 | 0 | 2 | (root) |
| 0x00462ec0 | FUN_00462ec0 | audio | C2 | 5 | 2 | 0x00466b50 |
| 0x00462f50 | FUN_00462f50 | audio | C2 | 5 | 1 | 0x00466b50 |
| 0x004631f0 | FUN_004631f0 | audio | C2 | 5 | 1 | 0x00466b50 |
| 0x004633d0 | FUN_004633d0 | audio | C2 | 3 | 1 | 0x004039f0 |
| 0x00463590 | FUN_00463590 | audio | C2 | 5 | 1 | 0x00466b50 |
| 0x00463640 | FUN_00463640 | audio | C2 | 5 | 1 | 0x00466b50 |
| 0x00463c80 | FUN_00463c80 | audio | C2 | 5 | 1 | 0x00466b50 |
| 0x00463f40 | FUN_00463f40 | audio | C2 | 5 | 1 | 0x00466b50 |
| 0x004642f0 | FUN_004642f0 | audio | C2 | 5 | 2 | 0x00466b50 |
| 0x004644a0 | FUN_004644a0 | audio | C2 | 5 | 2 | 0x0045daf0 |
| 0x004645c0 | FUN_004645c0 | audio | C2 | 1 | 2 | 0x0046cbb0 |
| 0x00464670 | FUN_00464670 | audio | C2 | 6 | 1 | 0x004648b0 |
| 0x004647f0 | FUN_004647f0 | audio | C2 | 5 | 1 | 0x00466b50 |
| 0x004648b0 | FUN_004648b0 | audio | C2 | 5 | 1 | 0x00466b50 |
| 0x00464a50 | FUN_00464a50 | audio | C2 | 5 | 1 | 0x00466b50 |
| 0x00465940 | FUN_00465940 | audio | C2 | 13 | 1 | 0x00453eb0 |
| 0x00465c10 | FUN_00465c10 | audio | C2 | 3 | 1 | 0x004661a0 |
| 0x00465ca0 | FUN_00465ca0 | audio | C2 | 10 | 1 | 0x00405ab0 |
| 0x00465f40 | FUN_00465f40 | audio | C2 | 13 | 2 | 0x0045dce0 |
| 0x00466000 | FUN_00466000 | audio | C2 | 13 | 1 | 0x0045dce0 |
| 0x004661a0 | FUN_004661a0 | audio | C2 | 2 | 2 | 0x00419760 |
| 0x004669b0 | FUN_004669b0 | audio | C2 | 3 | 1 | 0x00402750 |
| 0x00466a50 | FUN_00466a50 | audio | C2 | 7 | 4 | 0x00467010 |
| 0x005a5f00 | FUN_005a5f00 | audio | C2 | 5 | 4 | 0x004627f0 |
| 0x005a5f60 | FUN_005a5f60 | audio | C2 | 6 | 2 | 0x005a5f00 |
| 0x005a6030 | FUN_005a6030 | audio | C2 | 6 | 5 | 0x005a60b0 |
| 0x005a6070 | FUN_005a6070 | audio | C2 | 7 | 1 | 0x005a6030 |
| 0x005a60b0 | FUN_005a60b0 | audio | C2 | 5 | 3 | 0x0045d3f0 |
| 0x005a60e0 | FUN_005a60e0 | audio | C2 | 5 | 3 | 0x0045d430 |
| 0x005a6110 | FUN_005a6110 | audio | C2 | 5 | 2 | 0x004627f0 |
| 0x005a6150 | FUN_005a6150 | audio | C2 | 6 | 2 | 0x005a6110 |
| 0x005a6190 | Clamp5a6190 | audio | C3 | 6 | 1 | 0x005a5f00 |
| 0x005a6280 | FUN_005a6280 | audio | C2 | 5 | 1 | 0x0045d460 |
| 0x005a6340 | FUN_005a6340 | audio | C2 | 8 | 2 | 0x005a6670 |
| 0x005a65d0 | FUN_005a65d0 | audio | C2 | 8 | 3 | 0x005a6670 |
| 0x005a6670 | FUN_005a6670 | audio | C2 | 7 | 2 | 0x005a6e10 |
| 0x005a66d0 | FUN_005a66d0 | audio | C2 | 3 | 5 | 0x004661a0 |
| 0x005a6710 | FUN_005a6710 | audio | C2 | 5 | 3 | 0x0045d460 |
| 0x005a6b70 | FUN_005a6b70 | audio | C2 | 6 | 1 | 0x005a71f0 |
| 0x005a6c60 | FUN_005a6c60 | audio | C2 | 7 | 2 | 0x005a6b70 |
| 0x005a6c90 | FUN_005a6c90 | audio | C2 | 7 | 2 | 0x005a6b70 |
| 0x005a6cb0 | FUN_005a6cb0 | audio | C2 | 6 | 2 | 0x005a71f0 |
| 0x005a6d60 | FUN_005a6d60 | audio | C2 | 4 | 9 | 0x005a6dc0 |
| 0x005a6d90 | FUN_005a6d90 | audio | C2 | 7 | 1 | 0x005a6e10 |
| 0x005a6dc0 | FUN_005a6dc0 | audio | C2 | 3 | 12 | 0x0045e0f0 |
| 0x005a6df0 | FUN_005a6df0 | audio | C2 | 6 | 5 | 0x005a7460 |
| 0x005a6e10 | FUN_005a6e10 | audio | C2 | 6 | 3 | 0x005a7460 |
| 0x005a6ea0 | FUN_005a6ea0 | audio | C2 | 6 | 2 | 0x005a60b0 |
| 0x005a6f30 | FUN_005a6f30 | audio | C2 | 6 | 1 | 0x005a60b0 |
| 0x005a71f0 | FUN_005a71f0 | audio | C2 | 5 | 3 | 0x0045d460 |
| 0x005a73b0 | FUN_005a73b0 | audio | C2 | 8 | 1 | 0x00466a50 |
| 0x005a7420 | FUN_005a7420 | audio | C3 | 9 | 1 | 0x005a65d0 |
| 0x005a7460 | FUN_005a7460 | audio | C2 | 5 | 4 | 0x005a7520 |
| 0x005a7520 | FUN_005a7520 | audio | C2 | 4 | 3 | 0x005a66d0 |
| 0x005a7560 | FUN_005a7560 | audio | C2 | 5 | 1 | 0x005a7520 |
| 0x005a75b0 | FUN_005a75b0 | audio | C2 | 5 | 2 | 0x005a7520 |
| 0x005a79a0 | FUN_005a79a0 | audio | C2 | 8 | 1 | 0x00466a50 |
| 0x005a7aa0 | FUN_005a7aa0 | audio | C2 | 5 | 2 | 0x0045d460 |
| 0x005a7af0 | FUN_005a7af0 | audio | C3 | 5 | 1 | 0x0045d460 |
| 0x005a7b60 | FUN_005a7b60 | audio | C2 | 5 | 2 | 0x0045d460 |
| 0x005a7e70 | FUN_005a7e70 | audio | C2 | 9 | 1 | 0x005abcf0 |
| 0x005a7f70 | FUN_005a7f70 | audio | C2 | 1 | 2 | 0x00462dd0 |
| 0x005a8890 | FUN_005a8890 | audio | C2 | 1 | 2 | 0x00462dd0 |
| 0x005a8960 | FUN_005a8960 | audio | C3 | 6 | 1 | 0x0045dc80 |
| 0x005a89a0 | FUN_005a89a0 | audio | C3 | 5 | 4 | 0x00466b50 |
| 0x005a89b0 | FUN_005a89b0 | audio | C3 | 6 | 2 | 0x005a89a0 |
| 0x005a89c0 | FUN_005a89c0 | audio | C3 | 6 | 2 | 0x005a89a0 |
| 0x005a8e70 | FUN_005a8e70 | audio | C2 | 5 | 1 | 0x004627f0 |
| 0x005a9e10 | AudioSubStructTwoCallInit | audio | C3 | 9 | 1 | 0x005a6340 |
| 0x005a9e40 | FUN_005a9e40 | audio | C3 | 5 | 4 | 0x004627f0 |
| 0x005a9ff0 | FUN_005a9ff0 | audio | C2 | 6 | 2 | 0x005afa00 |
| 0x005aa060 | FUN_005aa060 | audio | C2 | 5 | 5 | 0x004627f0 |
| 0x005aa0c0 | AudioTreeWalkPredicateSearch | audio | C3 | 6 | 3 | 0x005aa060 |
| 0x005aa1e0 | FUN_005aa1e0 | audio | C3 | 6 | 1 | 0x005aa060 |
| 0x005aa560 | FUN_005aa560 | audio | C2 | 2 | 8 | 0x005a7f70 |
| 0x005aaa00 | FUN_005aaa00 | audio | C2 | 6 | 2 | 0x00462ec0 |
| 0x005aab00 | FUN_005aab00 | audio | C2 | 6 | 3 | 0x005a60b0 |
| 0x005aab70 | FUN_005aab70 | audio | C2 | 6 | 2 | 0x005a60e0 |
| 0x005aad40 | FUN_005aad40 | audio | C3 | 2 | 3 | 0x005a8890 |
| 0x005ab070 | FUN_005ab070 | audio | C3 | 7 | 1 | 0x005a6ea0 |
| 0x005abcb0 | AudioWaveNodeFree | audio | C3 | 9 | 1 | 0x005abcf0 |
| 0x005abcf0 | FUN_005abcf0 | audio | C2 | 8 | 1 | 0x00466a50 |
| 0x005ac650 | FUN_005ac650 | audio | C2 | 6 | 3 | 0x005afcf0 |
| 0x005ac740 | AudioSubStructBufCleanup | audio | C3 | 9 | 1 | 0x005abcf0 |
| 0x005ac9e0 | AudioFmtEntryMatch | audio | C3 | 6 | 1 | 0x005afa00 |
| 0x005acda0 | FUN_005acda0 | audio | C2 | 7 | 1 | 0x005a6f30 |
| 0x005ace70 | FUN_005ace70 | audio | C2 | 6 | 2 | 0x005afcf0 |
| 0x005ad080 | FUN_005ad080 | audio | C2 | 6 | 4 | 0x005a6110 |
| 0x005ad0b0 | FUN_005ad0b0 | audio | C2 | 7 | 2 | 0x005a6b70 |
| 0x005ad320 | FUN_005ad320 | audio | C2 | 8 | 1 | 0x005ad0b0 |
| 0x005ad6a0 | FUN_005ad6a0 | audio | C2 | 6 | 3 | 0x005a71f0 |
| 0x005ade10 | AudioListRemoveByValue | audio | C3 | 10 | 1 | 0x005a7e70 |
| 0x005aded0 | AudioListNodeCount | audio | C3 | 6 | 1 | 0x005a7af0 |
| 0x005adf30 | AudioFmtKeyCompare | audio | C3 | 6 | 9 | 0x005af180 |
| 0x005adf60 | FUN_005adf60 | audio | C2 | 6 | 1 | 0x005a7aa0 |
| 0x005ae030 | AudioSubStructCleanup | audio | C3 | 9 | 1 | 0x005abcf0 |
| 0x005ae170 | FUN_005ae170 | audio | C3 | 6 | 1 | 0x005afa00 |
| 0x005ae800 | AudioPoolBlockAlloc | audio | C3 | 8 | 1 | 0x005a6c60 |
| 0x005ae920 | AudioPoolFree | audio | C3 | 8 | 1 | 0x005a6c90 |
| 0x005aea00 | FUN_005aea00 | audio | C2 | 6 | 4 | 0x005a71f0 |
| 0x005aee20 | AudioBitScanForward | audio | C4 | 9 | 1 | 0x005a6340 |
| 0x005aefa0 | FUN_005aefa0 | audio | C3 | 6 | 1 | 0x005af070 |
| 0x005af070 | FUN_005af070 | audio | C2 | 5 | 4 | 0x005af740 |
| 0x005af180 | FUN_005af180 | audio | C2 | 5 | 5 | 0x005af260 |
| 0x005af200 | FUN_005af200 | audio | C2 | 5 | 3 | 0x005af260 |
| 0x005af230 | FUN_005af230 | audio | C2 | 5 | 1 | 0x005af260 |
| 0x005af260 | FUN_005af260 | audio | C2 | 4 | 3 | 0x005af510 |
| 0x005af300 | FUN_005af300 | audio | C2 | 6 | 2 | 0x005b1110 |
| 0x005af430 | FUN_005af430 | audio | C2 | 6 | 1 | 0x005b1110 |
| 0x005af510 | FUN_005af510 | audio | C2 | 3 | 3 | 0x005b0c70 |
| 0x005af600 | FUN_005af600 | audio | C2 | 4 | 3 | 0x005af510 |
| 0x005af690 | FUN_005af690 | audio | C2 | 4 | 3 | 0x005af510 |
| 0x005af700 | FUN_005af700 | audio | C3 | 5 | 1 | 0x005af690 |
| 0x005af740 | FUN_005af740 | audio | C2 | 4 | 3 | 0x005af510 |
| 0x005af7a0 | FUN_005af7a0 | audio | C2 | 4 | 2 | 0x005af510 |
| 0x005af7d0 | FUN_005af7d0 | audio | C2 | 4 | 3 | 0x005af510 |
| 0x005af860 | FUN_005af860 | audio | C2 | 6 | 2 | 0x005afcf0 |
| 0x005af8f0 | FUN_005af8f0 | audio | C2 | 4 | 4 | 0x005af510 |
| 0x005afa00 | FUN_005afa00 | audio | C2 | 5 | 2 | 0x005af8f0 |
| 0x005afcf0 | FUN_005afcf0 | audio | C2 | 5 | 6 | 0x005af8f0 |
| 0x005b0360 | FUN_005b0360 | audio | C2 | 6 | 1 | 0x005afcf0 |
| 0x005b03e0 | FUN_005b03e0 | audio | C2 | 7 | 1 | 0x005b0360 |
| 0x005b04b0 | FUN_005b04b0 | audio | C2 | 6 | 1 | 0x005afcf0 |
| 0x005b0970 | FUN_005b0970 | audio | C2 | 6 | 1 | 0x005afcf0 |
| 0x005b09c0 | FUN_005b09c0 | audio | C2 | 5 | 2 | 0x005af8f0 |
| 0x005b0a90 | FUN_005b0a90 | audio | C2 | 6 | 4 | 0x005afcf0 |
| 0x005b0b10 | FUN_005b0b10 | audio | C2 | 6 | 1 | 0x005afcf0 |
| 0x005b0b40 | FUN_005b0b40 | audio | C2 | 6 | 2 | 0x005afcf0 |
| 0x005b0b60 | FUN_005b0b60 | audio | C3 | 6 | 1 | 0x005afcf0 |
| 0x005b0b90 | Init5b0b90 | audio | C3 | 6 | 1 | 0x005af070 |
| 0x005b0bb0 | FUN_005b0bb0 | audio | C3 | 5 | 1 | 0x005af8f0 |
| 0x005b0c70 | Init5b0c70 | audio | C3 | 2 | 1 | 0x005a8890 |
| 0x005b0ca0 | CmdBuild5b0ca0Set | audio | C3 | 3 | 1 | 0x005b0c70 |
| 0x005b0cf0 | FUN_005b0cf0 | audio | C3 | 3 | 1 | 0x005b0c70 |
| 0x005b0dc0 | CmdBuild5b0dc0Set | audio | C3 | 3 | 2 | 0x005b0c70 |
| 0x005b1030 | FUN_005b1030 | audio | C2 | 6 | 2 | 0x005afcf0 |
| 0x005b10a0 | FUN_005b10a0 | audio | C2 | 5 | 6 | 0x005af600 |
| 0x005b10d0 | thunk_FUN_005b10a0 | audio | C2 | 6 | 1 | 0x005b10a0 |
| 0x005b10e0 | FUN_005b10e0 | audio | C2 | 6 | 3 | 0x005b10a0 |
| 0x005b1110 | FUN_005b1110 | audio | C2 | 5 | 5 | 0x005af8f0 |
| 0x005b1140 | FUN_005b1140 | audio | C2 | 5 | 3 | 0x005af7a0 |
| 0x005b73b0 | AudioFindExtension | audio | C3 | 6 | 1 | 0x005afa00 |
| 0x005b8570 | FUN_005b8570 | audio | C2 | 5 | 1 | 0x0045d460 |
| 0x005b9e30 | FUN_005b9e30 | audio | C2 | 5 | 1 | 0x004627f0 |
| 0x005baf00 | MusicGroupVolumeSet | audio | C3 | 6 | 1 | 0x0045dd60 |
| 0x005baf40 | AudioRendererField3cSet | audio | C3 | 5 | 1 | 0x004627f0 |
| 0x005baf60 | AudioBufFieldSet | audio | C3 | 5 | 1 | 0x004627f0 |
| 0x005bbb20 | FUN_005bbb20 | audio | C2 | 5 | 2 | 0x004627f0 |
| 0x005bbb70 | FUN_005bbb70 | audio | C2 | 6 | 2 | 0x005bbb20 |
| 0x004014b0 | FUN_004014b0 | boot | C2 | 6 | 1 | 0x00402a40 |
| 0x004015a0 | FUN_004015a0 | boot | C2 | 4 | 2 | 0x00402f50 |
| 0x004025f0 | FUN_004025f0 | boot | C2 | 3 | 2 | 0x00402750 |
| 0x004026d0 | FUN_004026d0 | boot | C3 | 3 | 1 | 0x00402750 |
| 0x00402750 | sub_00402750 | boot | C2 | 2 | 12 | 0x00495280 |
| 0x00402a40 | sub_00402a40 | boot | C2 | 5 | 4 | 0x0040bd00 |
| 0x00402f50 | FUN_00402f50 | boot | C3 | 3 | 1 | 0x00402750 |
| 0x00403640 | FUN_00403640 | boot | C2 | 3 | 1 | 0x00402750 |
| 0x00403660 | FUN_00403660 | boot | C2 | 6 | 1 | 0x00402a40 |
| 0x00404830 | FUN_00404830 | boot | C2 | 3 | 1 | 0x00402750 |
| 0x0040bb30 | FUN_0040bb30 | boot | C3 | 3 | 14 | 0x00402750 |
| 0x0040bbb0 | FUN_0040bbb0 | boot | C2 | 3 | 4 | 0x00402750 |
| 0x0040bd00 | FUN_0040bd00 | boot | C2 | 4 | 5 | 0x0040bbb0 |
| 0x0040cf80 | FUN_0040cf80 | boot | C2 | 6 | 4 | 0x00402a40 |
| 0x0040cfd0 | FUN_0040cfd0 | boot | C2 | 6 | 2 | 0x00402a40 |
| 0x004113b0 | FUN_004113b0 | boot | C2 | 3 | 2 | 0x00402750 |
| 0x004114c0 | FUN_004114c0 | boot | C3 | 3 | 2 | 0x00402750 |
| 0x00412890 | FUN_00412890 | boot | C2 | 3 | 1 | 0x00402750 |
| 0x00418980 | thunk_FUN_0041a060 | boot | C2 | 3 | 2 | 0x00402750 |
| 0x004189e0 | thunk_FUN_004196f0 | boot | C2 | 6 | 1 | 0x00402a40 |
| 0x0041a1e0 | FUN_0041a1e0 | boot | C2 | 3 | 2 | 0x00402750 |
| 0x0041a3d0 | FUN_0041a3d0 | boot | C2 | 4 | 3 | 0x0041a1e0 |
| 0x0041b450 | FUN_0041b450 | boot | C2 | 3 | 5 | 0x00402750 |
| 0x0041b660 | FUN_0041b660 | boot | C2 | 4 | 3 | 0x0041b450 |
| 0x0041bec0 | FUN_0041bec0 | boot | C2 | 3 | 4 | 0x00402750 |
| 0x0041c0e0 | FUN_0041c0e0 | boot | C2 | 4 | 2 | 0x0041bec0 |
| 0x0041c100 | FUN_0041c100 | boot | C2 | 3 | 1 | 0x00402750 |
| 0x0041c2c0 | FUN_0041c2c0 | boot | C2 | 6 | 3 | 0x00402a40 |
| 0x0041cb10 | FUN_0041cb10 | boot | C2 | 3 | 5 | 0x00402750 |
| 0x0041ccf0 | FUN_0041ccf0 | boot | C2 | 6 | 1 | 0x00402a40 |
| 0x0041d6e0 | FUN_0041d6e0 | boot | C2 | 3 | 3 | 0x00402750 |
| 0x0041d890 | FUN_0041d890 | boot | C2 | 4 | 2 | 0x0041d6e0 |
| 0x0041d8b0 | FUN_0041d8b0 | boot | C2 | 3 | 1 | 0x00402750 |
| 0x0041da80 | FUN_0041da80 | boot | C2 | 6 | 1 | 0x00402a40 |
| 0x0041db90 | FUN_0041db90 | boot | C2 | 3 | 1 | 0x00402750 |
| 0x0041de70 | FUN_0041de70 | boot | C2 | 6 | 1 | 0x00402a40 |
| 0x0041def0 | FUN_0041def0 | boot | C2 | 3 | 2 | 0x00402750 |
| 0x0041e0d0 | FUN_0041e0d0 | boot | C2 | 6 | 1 | 0x00402a40 |
| 0x0041eaa0 | FUN_0041eaa0 | boot | C2 | 3 | 1 | 0x00402750 |
| 0x0041ffb0 | FUN_0041ffb0 | boot | C2 | 6 | 2 | 0x00402a40 |
| 0x00420d00 | FUN_00420d00 | boot | C2 | 3 | 1 | 0x00402750 |
| 0x00421590 | thunk_FUN_004210f0 | boot | C2 | 6 | 2 | 0x00402a40 |
| 0x00425bc0 | FUN_00425bc0 | boot | C2 | 3 | 1 | 0x00402750 |
| 0x00425ed0 | FUN_00425ed0 | boot | C2 | 6 | 1 | 0x00402a40 |
| 0x004275d0 | FUN_004275d0 | boot | C2 | 3 | 1 | 0x00402750 |
| 0x004283a0 | FUN_004283a0 | boot | C2 | 3 | 2 | 0x00402750 |
| 0x00428400 | FUN_00428400 | boot | C2 | 6 | 1 | 0x00402a40 |
| 0x0042c2a0 | FUN_0042c2a0 | boot | C2 | 6 | 1 | 0x00402a40 |
| 0x00431b40 | Set67eaa8 | boot | C3 | 3 | 1 | 0x00402750 |
| 0x0045b930 | FUN_0045b930 | boot | C2 | 3 | 2 | 0x0045bae0 |
| 0x00467010 | FUN_00467010 | boot | C2 | 6 | 1 | 0x00402a40 |
| 0x00467020 | FUN_00467020 | boot | C2 | 6 | 1 | 0x00402a40 |
| 0x00467070 | FUN_00467070 | boot | C2 | 6 | 1 | 0x00402a40 |
| 0x00471df0 | FUN_00471df0 | boot | C2 | 3 | 2 | 0x00402750 |
| 0x004723d0 | Init691500 | boot | C3 | 3 | 1 | 0x00402750 |
| 0x0047ba00 | FUN_0047ba00 | boot | C2 | 3 | 1 | 0x00402750 |
| 0x00484130 | FUN_00484130 | boot | C2 | 6 | 1 | 0x00402a40 |
| 0x00484170 | FUN_00484170 | boot | C2 | 3 | 2 | 0x00402750 |
| 0x004841d0 | FUN_004841d0 | boot | C2 | 3 | 1 | 0x00402750 |
| 0x004881d0 | FUN_004881d0 | boot | C2 | 3 | 2 | 0x00402750 |
| 0x00489250 | FUN_00489250 | boot | C2 | 6 | 1 | 0x00402a40 |
| 0x00494bc0 | FUN_00494bc0 | boot | C2 | 6 | 1 | 0x00402a40 |
| 0x00496ce0 | FUN_00496ce0 | boot | C2 | 6 | 1 | 0x00402a40 |
| 0x00496e40 | FUN_00496e40 | boot | C2 | 3 | 1 | 0x00402750 |
| 0x004a3258 | CrtExitCore | boot | C3 | 5 | 1 | 0x004a332b |
| 0x004a332b | CrtExitNoReturn_j5 | boot | C3 | 4 | 1 | 0x0040bbb0 |
| 0x004b4880 | FUN_004b4880 | boot | C2 | 6 | 2 | 0x00402a40 |
| 0x004b6540 | thunk_FUN_004b6640 | boot | C2 | 4 | 1 | 0x004b6640 |
| 0x004b6560 | BootGlobalPairSetThunk | boot | C3 | 4 | 1 | 0x004b6610 |
| 0x004b6610 | BootGlobalPairSet | boot | C3 | 3 | 2 | 0x00402750 |
| 0x004b6640 | FUN_004b6640 | boot | C2 | 3 | 2 | 0x00402750 |
| 0x004c5930 | sub_004c5930 | boot | C2 | 3 | 8 | 0x0041e140 |
| 0x005581f0 | FUN_005581f0 | boot | C2 | 6 | 1 | 0x00402a40 |
| 0x00558240 | FUN_00558240 | boot | C2 | 3 | 2 | 0x00402750 |
| 0x00426bb0 | CameraPath::GetCount | camera | C3 | 1 | 3 | 0x00414570 |
| 0x004414b0 | FUN_004414b0 | camera | C2 | 8 | 1 | 0x0040d470 |
| 0x00471ec0 | sub_00471ec0 | camera | C2 | 1 | 2 | 0x00407a40 |
| 0x0047c160 | sub_0047c160 | camera | C2 | 7 | 2 | 0x00426ab0 |
| 0x00408a50 | FUN_00408a50 | frontend | C3 | 0 | 7 | (root) |
| 0x00408a70 | FrontendC2RoundI | frontend | C3 | 0 | 2 | (root) |
| 0x00408ad0 | RaceScoreFloatGetBySlot | frontend | C3 | 0 | 4 | (root) |
| 0x0040acd0 | FUN_0040acd0 | frontend | C2 | 1 | 1 | 0x0043dfd0 |
| 0x0040ad20 | FrontendGlobalGet | frontend | C4 | 1 | 1 | 0x0043dfd0 |
| 0x0040b290 | FUN_0040b290 | frontend | C4 | 0 | 3 | (root) |
| 0x0040b6c0 | FrontendArrayGet | frontend | C4 | 0 | 0 | (root) |
| 0x0040b810 | TimerGlobalsReset | frontend | C3 | 1 | 1 | 0x0043dfd0 |
| 0x0040bb50 | FUN_0040bb50 | frontend | C3 | 3 | 2 | 0x00402750 |
| 0x0040bb70 | SpriteLookupTableA | frontend | C4 | 7 | 1 | 0x0042e5b0 |
| 0x0040bb90 | SpriteLookupTableB | frontend | C4 | 1 | 1 | 0x0042fab0 |
| 0x0040d590 | FUN_0040d590 | frontend | C2 | 0 | 1 | (root) |
| 0x0040e3a0 | PlayerColorTableGet | frontend | C3 | 7 | 1 | 0x0040e590 |
| 0x0040e470 | CarSlotStateGet | frontend | C3 | 0 | 13 | (root) |
| 0x0040e480 | CarSlotStateSet | frontend | C3 | 0 | 2 | (root) |
| 0x0040eee0 | FUN_0040eee0 | frontend | C3 | 0 | 3 | (root) |
| 0x00414120 | CopyTable005f2a70To0089a384 | frontend | C3 | 1 | 1 | 0x0043dfd0 |
| 0x004215c0 | FUN_004215c0 | frontend | C3 | 1 | 2 | 0x00422fd0 |
| 0x00422b30 | TimerArrayClear | frontend | C3 | 1 | 1 | 0x0043dfd0 |
| 0x00422fd0 | FrontendRaceResultsDispatch | frontend | C3 | 0 | 6 | (root) |
| 0x00425b70 | SlotFieldSetter | frontend | C3 | 7 | 1 | 0x004262f0 |
| 0x00425bf0 | FUN_00425bf0 | frontend | C2 | 7 | 2 | 0x004262f0 |
| 0x00425c00 | FUN_00425c00 | frontend | C2 | 7 | 1 | 0x004262f0 |
| 0x00425ee0 | SlotWordPtrGet | frontend | C3 | 6 | 1 | 0x00464a50 |
| 0x00425ef0 | ActiveSlotCount | frontend | C3 | 6 | 1 | 0x00464a50 |
| 0x00426090 | GlobalDat0066ce58Get | frontend | C3 | 3 | 7 | 0x00409330 |
| 0x00426b40 | FUN_00426b40 | frontend | C2 | 7 | 1 | 0x0040cfd0 |
| 0x00426bc0 | GetDat0066d6e0 | frontend | C3 | 6 | 1 | 0x00409290 |
| 0x00426bd0 | GetTableEntry0066d658 | frontend | C3 | 6 | 1 | 0x00409290 |
| 0x00426cb0 | FUN_00426cb0 | frontend | C3 | 5 | 1 | 0x00407e20 |
| 0x00426cf0 | GetDat0066d6e4 | frontend | C3 | 12 | 1 | 0x004046a0 |
| 0x00426d00 | FrontendArraySlotGet | frontend | C3 | 0 | 1 | (root) |
| 0x00426dc0 | FrontendRaycastForward | frontend | C3 | 13 | 1 | 0x0041f590 |
| 0x004274d0 | LangIndexSeedFromCli | frontend | C3 | 3 | 1 | 0x00402750 |
| 0x004274e0 | FUN_004274e0 | frontend | C2 | 3 | 1 | 0x00402750 |
| 0x00427580 | FUN_00427580 | frontend | C3 | 4 | 1 | 0x004275d0 |
| 0x00427880 | FUN_00427880 | frontend | C2 | 4 | 1 | 0x00427ca0 |
| 0x00427e00 | FUN_00427e00 | frontend | C2 | 2 | 3 | 0x00429e10 |
| 0x00428390 | FrontendStateSet | frontend | C3 | 3 | 1 | 0x00402750 |
| 0x00428d30 | FUN_00428d30 | frontend | C2 | 9 | 1 | 0x00473220 |
| 0x00429240 | FUN_00429240 | frontend | C2 | 4 | 1 | 0x00429290 |
| 0x00429290 | FUN_00429290 | frontend | C2 | 3 | 4 | 0x00402750 |
| 0x004298c0 | Clear67d99c_x4 | frontend | C3 | 1 | 1 | 0x0043dfd0 |
| 0x00429a70 | LapFracGetBySlot | frontend | C3 | 2 | 3 | 0x00448220 |
| 0x00429a80 | LapLapsGetBySlot | frontend | C3 | 2 | 3 | 0x00448220 |
| 0x00429a90 | LapSecsGetBySlot | frontend | C3 | 2 | 3 | 0x00448220 |
| 0x00429b70 | FUN_00429b70 | frontend | C3 | 3 | 1 | 0x00429bd0 |
| 0x0042a640 | FUN_0042a640 | frontend | C2 | 5 | 1 | 0x0042a5d0 |
| 0x0042aa00 | FUN_0042aa00 | frontend | C3 | 1 | 2 | 0x0043dfd0 |
| 0x0042ac90 | FUN_0042ac90 | frontend | C3 | 1 | 1 | 0x0043dfd0 |
| 0x0042ae10 | MenuReadinessCheckA | frontend | C4 | 1 | 1 | 0x0043dfd0 |
| 0x0042aeb0 | MenuReadinessCheckB | frontend | C3 | 1 | 1 | 0x0043dfd0 |
| 0x0042af50 | MenuReadinessCheckC | frontend | C3 | 1 | 2 | 0x0043dfd0 |
| 0x0042aff0 | FUN_0042aff0 | frontend | C4 | 1 | 1 | 0x0043dfd0 |
| 0x0042b180 | FUN_0042b180 | frontend | C4 | 1 | 1 | 0x0043dfd0 |
| 0x0042b310 | FUN_0042b310 | frontend | C4 | 1 | 1 | 0x0043dfd0 |
| 0x0042b540 | FUN_0042b540 | frontend | C3 | 1 | 1 | 0x0043dfd0 |
| 0x0042b770 | FUN_0042b770 | frontend | C4 | 1 | 1 | 0x0043dfd0 |
| 0x0042b930 | MenuAlphaGet | frontend | C4 | 1 | 3 | 0x004671a0 |
| 0x0042b960 | FUN_0042b960 | frontend | C3 | 1 | 1 | 0x0043dfd0 |
| 0x0042b9e0 | FUN_0042b9e0 | frontend | C3 | 1 | 1 | 0x0043dfd0 |
| 0x0042bb60 | FUN_0042bb60 | frontend | C3 | 1 | 1 | 0x0043dfd0 |
| 0x0042bfb0 | MenuStateParamStore | frontend | C3 | 2 | 1 | 0x0040acd0 |
| 0x0042e590 | SpriteAnimFrameThunk | frontend | C4 | 7 | 1 | 0x0042e5b0 |
| 0x0042e5b0 | FUN_0042e5b0 | frontend | C3 | 6 | 1 | 0x00473c20 |
| 0x0042f020 | FUN_0042f020 | frontend | C2 | 1 | 1 | 0x0043dfd0 |
| 0x0042f400 | FUN_0042f400 | frontend | C2 | 1 | 1 | 0x0043dfd0 |
| 0x0042f6b0 | MenuModeSync | frontend | C3 | 0 | 2 | (root) |
| 0x0042fab0 | FUN_0042fab0 | frontend | C2 | 0 | 0 | (root) |
| 0x00430670 | FUN_00430670 | frontend | C2 | 0 | 0 | (root) |
| 0x00430760 | IsMultiplayerMode | frontend | C3 | 1 | 1 | 0x0043dfd0 |
| 0x004307a0 | ElapsedVsThresholdCheck | frontend | C3 | 1 | 1 | 0x0043dfd0 |
| 0x00430830 | SplitScreenTrackAssignment | frontend | C3 | 1 | 2 | 0x0043dfd0 |
| 0x00430910 | MenuOptionSlotGet | frontend | C3 | 1 | 2 | 0x0043dfd0 |
| 0x004309b0 | FrontendModeIndex | frontend | C3 | 1 | 1 | 0x0043dfd0 |
| 0x00430b60 | MenuSlotCount | frontend | C3 | 0 | 1 | (root) |
| 0x00431b80 | FUN_00431b80 | frontend | C2 | 1 | 1 | 0x0043dfd0 |
| 0x00431d00 | FUN_00431d00 | frontend | C2 | 1 | 1 | 0x0043dfd0 |
| 0x00431d80 | TiebreakFlagGet | frontend | C3 | 0 | 1 | (root) |
| 0x004322c0 | FUN_004322c0 | frontend | C2 | 1 | 1 | 0x0043dfd0 |
| 0x004323c0 | MenuCursorBack | frontend | C3 | 1 | 1 | 0x0043dfd0 |
| 0x004324a0 | FUN_004324a0 | frontend | C2 | 1 | 1 | 0x0043dfd0 |
| 0x004325c0 | FUN_004325c0 | frontend | C4 | 1 | 1 | 0x0043dfd0 |
| 0x00432800 | FUN_00432800 | frontend | C2 | 1 | 1 | 0x0043dfd0 |
| 0x0043d2a0 | FUN_0043d2a0 | frontend | C2 | 1 | 1 | 0x0043dfd0 |
| 0x0043dfd0 | FUN_0043dfd0 | frontend | C2 | 0 | 0 | (root) |
| 0x00448220 | Frontend::PostRaceResultCamera | frontend | C2 | 1 | 3 | 0x0040e4a0 |
| 0x0045ba00 | RaceResultIndexedStore | frontend | C3 | 1 | 3 | 0x00422fd0 |
| 0x0046c5c0 | VehicleSlotInit | frontend | C3 | 1 | 2 | 0x00422fd0 |
| 0x0046c700 | EntityScoreFieldAdd | frontend | C3 | 0 | 1 | (root) |
| 0x0046c790 | VehicleSlotFieldSet | frontend | C3 | 1 | 1 | 0x00422fd0 |
| 0x0046dc00 | EntityFieldSet | frontend | C3 | 1 | 1 | 0x0043dfd0 |
| 0x00472c60 | ChromeBaseDraw | frontend | C3 | 2 | 2 | 0x00429e10 |
| 0x00473ee0 | LogoOverlayDraw | frontend | C4 | 7 | 3 | 0x0042e5b0 |
| 0x004893d0 | FUN_004893d0 | frontend | C3 | 5 | 2 | 0x0045ae80 |
| 0x00492340 | CarSlotInit | frontend | C3 | 1 | 1 | 0x0043dfd0 |
| 0x00492d20 | IntroSplashFrameTickShim | frontend | C3 | 4 | 1 | 0x00495350 |
| 0x00493f70 | VideoStateFlagGet | frontend | C4 | 1 | 6 | 0x0043dfd0 |
| 0x00493f80 | IntroVideoDimGetter | frontend | C4 | 4 | 2 | 0x00495350 |
| 0x00493fc0 | AspectRatioGlobalGet | frontend | C4 | 2 | 3 | 0x00494480 |
| 0x00493fd0 | sub_00493fd0 | frontend | C2 | 2 | 4 | 0x004c75e0 |
| 0x00494460 | sub_00494460 | frontend | C2 | 1 | 3 | 0x0043dfd0 |
| 0x00494a80 | sub_00494a80 | frontend | C2 | 1 | 2 | 0x0043dfd0 |
| 0x00494f30 | AspectRatioSnapshot | frontend | C4 | 1 | 1 | 0x0043dfd0 |
| 0x00495080 | FUN_00495080 | frontend | C2 | 1 | 1 | 0x0043dfd0 |
| 0x00495350 | IntroSplashOrchestrator | frontend | C2 | 3 | 6 | 0x004c1a00 |
| 0x004a2b60 | FUN_004a2b60 | frontend | C2 | 1 | 7 | 0x004625b0 |
| 0x004a2c48 | FUN_004a2c48 | frontend | C2 | 0 | 31 | (root) |
| 0x004c19f0 | RwVtableSlot07Call | frontend | C3 | 2 | 4 | 0x00429e10 |
| 0x004c1a00 | IntroSplashVtableSlot6 | frontend | C3 | 2 | 4 | 0x00429e10 |
| 0x004c1bb0 | IntroSplashRenderState | frontend | C3 | 2 | 3 | 0x00429e10 |
| 0x004c1be0 | sub_004c1be0 | frontend | C2 | 4 | 2 | 0x00495350 |
| 0x004c5c00 | FUN_004c5c00 | frontend | C3 | 4 | 2 | 0x0040bb30 |
| 0x004c75e0 | ViewportOriginGetter | frontend | C3 | 1 | 2 | 0x004b55a0 |
| 0x004cc160 | FUN_004cc160 | frontend | C3 | 1 | 7 | 0x004235b0 |
| 0x004cc230 | FUN_004cc230 | frontend | C2 | 1 | 8 | 0x004235b0 |
| 0x004d8c40 | FUN_004d8c40 | frontend | C3 | 6 | 1 | 0x004c7730 |
| 0x00405400 | Clear639d70x3 | gameplay | C3 | 7 | 2 | 0x0040cfd0 |
| 0x00405430 | Pred405430 | gameplay | C3 | 8 | 2 | 0x0040d470 |
| 0x00405540 | FUN_00405540 | gameplay | C2 | 8 | 2 | 0x00405400 |
| 0x00405920 | FUN_00405920 | gameplay | C2 | 9 | 1 | 0x00405540 |
| 0x00405ab0 | FUN_00405ab0 | gameplay | C2 | 9 | 3 | 0x0040ad40 |
| 0x00406130 | FUN_00406130 | gameplay | C2 | 10 | 2 | 0x00406370 |
| 0x00406160 | FUN_00406160 | gameplay | C2 | 11 | 2 | 0x004173a0 |
| 0x00406370 | Clear10x3_63a494 | gameplay | C3 | 9 | 3 | 0x00407800 |
| 0x004069d0 | FUN_004069d0 | gameplay | C2 | 9 | 1 | 0x00405540 |
| 0x00406ae0 | FUN_00406ae0 | gameplay | C2 | 11 | 1 | 0x004173a0 |
| 0x004075a0 | Global63a5d0Get | gameplay | C3 | 6 | 1 | 0x00464a50 |
| 0x00407800 | FUN_00407800 | gameplay | C2 | 8 | 1 | 0x0047cdc0 |
| 0x00407a00 | ReadAiCheckpointField | gameplay | C3 | 2 | 1 | 0x00407a20 |
| 0x00407a20 | Table8a9648Get | gameplay | C3 | 1 | 1 | 0x004177b0 |
| 0x00407b00 | FUN_00407b00 | gameplay | C2 | 3 | 6 | 0x00409330 |
| 0x00407be0 | FUN_00407be0 | gameplay | C2 | 4 | 2 | 0x00407cd0 |
| 0x00407cd0 | FUN_00407cd0 | gameplay | C2 | 3 | 2 | 0x00409330 |
| 0x00407d70 | RangeTableCountGroup | gameplay | C3 | 5 | 1 | 0x00407e20 |
| 0x00407db0 | RangeTableGroupOffset | gameplay | C3 | 5 | 1 | 0x00407e20 |
| 0x00407e20 | FUN_00407e20 | gameplay | C2 | 4 | 3 | 0x00407cd0 |
| 0x00408610 | FUN_00408610 | gameplay | C2 | 2 | 3 | 0x00407a20 |
| 0x00409300 | FUN_00409300 | gameplay | C3 | 3 | 3 | 0x00409330 |
| 0x00409330 | FUN_00409330 | gameplay | C3 | 2 | 4 | 0x00448220 |
| 0x004093b0 | FUN_004093b0 | gameplay | C2 | 3 | 2 | 0x00409330 |
| 0x004095a0 | FUN_004095a0 | gameplay | C2 | 2 | 1 | 0x00409710 |
| 0x004096a0 | FUN_004096a0 | gameplay | C2 | 1 | 1 | 0x00409790 |
| 0x00409790 | FUN_00409790 | gameplay | C2 | 0 | 0 | (root) |
| 0x004098d0 | FUN_004098d0 | gameplay | C2 | 2 | 4 | 0x0040acd0 |
| 0x00409950 | FUN_00409950 | gameplay | C2 | 2 | 3 | 0x0040acd0 |
| 0x00409970 | FUN_00409970 | gameplay | C2 | 3 | 1 | 0x004098d0 |
| 0x00409a80 | FUN_00409a80 | gameplay | C2 | 2 | 2 | 0x0040acd0 |
| 0x0040ad40 | FUN_0040ad40 | gameplay | C2 | 8 | 1 | 0x0040b090 |
| 0x0040ad90 | FUN_0040ad90 | gameplay | C2 | 8 | 1 | 0x0040b090 |
| 0x0040ae30 | FUN_0040ae30 | gameplay | C2 | 8 | 2 | 0x0040b090 |
| 0x0040aef0 | FUN_0040aef0 | gameplay | C2 | 8 | 2 | 0x0040b090 |
| 0x0040be50 | FUN_0040be50 | gameplay | C2 | 2 | 2 | 0x004215c0 |
| 0x0040ce80 | PtrTable5f2770Get | gameplay | C3 | 8 | 1 | 0x0046b540 |
| 0x0040cf40 | FUN_0040cf40 | gameplay | C2 | 7 | 1 | 0x0040cf80 |
| 0x00412010 | FUN_00412010 | gameplay | C2 | 4 | 1 | 0x0045b930 |
| 0x00416230 | Table89a500Set | gameplay | C3 | 0 | 2 | (root) |
| 0x00417370 | FUN_00417370 | gameplay | C2 | 1 | 3 | 0x00416230 |
| 0x004173a0 | Float89a360Get | gameplay | C3 | 10 | 3 | 0x00405ab0 |
| 0x004173b0 | FUN_004173b0 | gameplay | C2 | 1 | 2 | 0x00416230 |
| 0x00417740 | FUN_00417740 | gameplay | C2 | 0 | 2 | (root) |
| 0x00417750 | FUN_00417750 | gameplay | C2 | 11 | 1 | 0x004173a0 |
| 0x004196f0 | FUN_004196f0 | gameplay | C2 | 6 | 1 | 0x00402a40 |
| 0x00419a00 | FUN_00419a00 | gameplay | C2 | 13 | 1 | 0x0045dce0 |
| 0x0041a060 | FUN_0041a060 | gameplay | C2 | 3 | 2 | 0x00402750 |
| 0x0041e140 | Global63d7e0Get | gameplay | C3 | 2 | 1 | 0x00429e10 |
| 0x0041e8d0 | FUN_0041e8d0 | gameplay | C2 | 3 | 3 | 0x0041e140 |
| 0x0041e950 | FUN_0041e950 | gameplay | C2 | 4 | 4 | 0x0041e8d0 |
| 0x0041e9f0 | GlobalField63d7e4_1c | gameplay | C3 | 4 | 1 | 0x0041e8d0 |
| 0x0041ea70 | GlobalField63d7e4_3c | gameplay | C3 | 3 | 4 | 0x0041e140 |
| 0x0041ec00 | FUN_0041ec00 | gameplay | C2 | 3 | 3 | 0x0041e140 |
| 0x0041f060 | FUN_0041f060 | gameplay | C2 | 4 | 2 | 0x0045bba0 |
| 0x0041f220 | FUN_0041f220 | gameplay | C2 | 11 | 1 | 0x00421630 |
| 0x0041f290 | FUN_0041f290 | gameplay | C2 | 3 | 1 | 0x004039f0 |
| 0x0041f590 | FUN_0041f590 | gameplay | C2 | 12 | 1 | 0x0041f220 |
| 0x0041fe10 | FUN_0041fe10 | gameplay | C2 | 12 | 1 | 0x0041f220 |
| 0x00420d80 | FUN_00420d80 | gameplay | C3 | 6 | 2 | 0x00463640 |
| 0x00421060 | FUN_00421060 | gameplay | C3 | 6 | 1 | 0x00421690 |
| 0x00421080 | Fill63e5a4 | gameplay | C3 | 5 | 1 | 0x004210b0 |
| 0x004210b0 | FUN_004210b0 | gameplay | C2 | 4 | 2 | 0x0040bbb0 |
| 0x004210f0 | FUN_004210f0 | gameplay | C2 | 6 | 2 | 0x00402a40 |
| 0x00452ec0 | FUN_00452ec0 | gameplay | C2 | 14 | 1 | 0x00452f30 |
| 0x00452f00 | FUN_00452f00 | gameplay | C2 | 14 | 1 | 0x00452f30 |
| 0x00452f30 | FUN_00452f30 | gameplay | C2 | 13 | 1 | 0x00453eb0 |
| 0x00453eb0 | FUN_00453eb0 | gameplay | C2 | 12 | 1 | 0x00406160 |
| 0x00454a30 | Table688304Get | gameplay | C3 | 1 | 1 | 0x004177b0 |
| 0x00456040 | FUN_00456040 | gameplay | C2 | 11 | 1 | 0x00456140 |
| 0x00456140 | FUN_00456140 | gameplay | C2 | 10 | 1 | 0x00405ab0 |
| 0x004568a0 | FUN_004568a0 | gameplay | C2 | 11 | 2 | 0x00456140 |
| 0x004568d0 | FUN_004568d0 | gameplay | C2 | 2 | 2 | 0x0045a0f0 |
| 0x00456c30 | FUN_00456c30 | gameplay | C2 | 11 | 1 | 0x00456140 |
| 0x00456c70 | FUN_00456c70 | gameplay | C2 | 12 | 3 | 0x00456c30 |
| 0x00456c90 | thunk_FUN_00456c70 | gameplay | C2 | 13 | 1 | 0x00456c70 |
| 0x004576b0 | AnyActive4576b0 | gameplay | C3 | 3 | 2 | 0x00459000 |
| 0x004584e0 | FUN_004584e0 | gameplay | C2 | 4 | 1 | 0x00458a40 |
| 0x00458880 | FUN_00458880 | gameplay | C2 | 4 | 1 | 0x0045b930 |
| 0x00458a40 | FUN_00458a40 | gameplay | C2 | 3 | 1 | 0x00459000 |
| 0x00458b10 | FUN_00458b10 | gameplay | C2 | 13 | 2 | 0x0045b990 |
| 0x00458d00 | FUN_00458d00 | gameplay | C2 | 3 | 2 | 0x00459000 |
| 0x00458dd0 | FUN_00458dd0 | gameplay | C2 | 3 | 1 | 0x00459000 |
| 0x00458e00 | FUN_00458e00 | gameplay | C2 | 4 | 1 | 0x00458a40 |
| 0x00458f20 | FUN_00458f20 | gameplay | C3 | 1 | 1 | 0x00458f80 |
| 0x00458f80 | FUN_00458f80 | gameplay | C3 | 0 | 1 | (root) |
| 0x00459000 | FUN_00459000 | gameplay | C2 | 2 | 5 | 0x00458f20 |
| 0x004593b0 | FUN_004593b0 | gameplay | C2 | 4 | 1 | 0x0045b930 |
| 0x00459400 | FUN_00459400 | gameplay | C2 | 5 | 1 | 0x0045a190 |
| 0x00459480 | FUN_00459480 | gameplay | C2 | 5 | 1 | 0x0045a190 |
| 0x004595c0 | FUN_004595c0 | gameplay | C2 | 12 | 1 | 0x00456040 |
| 0x00459620 | FUN_00459620 | gameplay | C2 | 3 | 2 | 0x004568d0 |
| 0x0045a110 | PtrGet68ba1c | gameplay | C3 | 3 | 1 | 0x004568d0 |
| 0x0045a130 | FUN_0045a130 | gameplay | C2 | 13 | 1 | 0x0045b990 |
| 0x0045a190 | FUN_0045a190 | gameplay | C2 | 4 | 1 | 0x0045bba0 |
| 0x0045a530 | FUN_0045a530 | gameplay | C2 | 5 | 1 | 0x0045ae80 |
| 0x0045a590 | FUN_0045a590 | gameplay | C2 | 5 | 1 | 0x0045ae80 |
| 0x0045a950 | FUN_0045a950 | gameplay | C2 | 1 | 2 | 0x00407a40 |
| 0x0045ac40 | FUN_0045ac40 | gameplay | C2 | 5 | 1 | 0x0045ae80 |
| 0x0045ae80 | FUN_0045ae80 | gameplay | C2 | 4 | 1 | 0x00474d80 |
| 0x0045c860 | Clear88f0a0x4 | gameplay | C3 | 6 | 1 | 0x0045dc80 |
| 0x00461e90 | FUN_00461e90 | gameplay | C3 | 6 | 2 | 0x00463c80 |
| 0x0046be10 | FUN_0046be10 | gameplay | C3 | 8 | 2 | 0x004725f0 |
| 0x0046bf50 | FUN_0046bf50 | gameplay | C2 | 2 | 6 | 0x0045a950 |
| 0x0046d2e0 | WheelGet46d2e0 | gameplay | C3 | 6 | 5 | 0x00463c80 |
| 0x0046d320 | Idx2Wheel881790Get | gameplay | C3 | 8 | 2 | 0x0040b090 |
| 0x0046d360 | Idx2Wheel881738Get | gameplay | C3 | 8 | 1 | 0x0040b090 |
| 0x0046d660 | VehTbl881f50Get3 | gameplay | C3 | 9 | 2 | 0x0040ad40 |
| 0x0046d740 | FUN_0046d740 | gameplay | C3 | 10 | 1 | 0x00405ab0 |
| 0x0046dd80 | Float61313cGet | gameplay | C3 | 1 | 1 | 0x004177b0 |
| 0x0046dd90 | VehField8816f4Set | gameplay | C3 | 1 | 1 | 0x004177b0 |
| 0x00471490 | FUN_00471490 | gameplay | C2 | 6 | 1 | 0x00471560 |
| 0x00471530 | ClearTable471530 | gameplay | C3 | 4 | 1 | 0x00471df0 |
| 0x00471560 | FUN_00471560 | gameplay | C2 | 5 | 2 | 0x00471530 |
| 0x00472500 | CondGet691500 | gameplay | C3 | 8 | 1 | 0x004725f0 |
| 0x00472560 | FUN_00472560 | gameplay | C3 | 8 | 1 | 0x004725f0 |
| 0x004725f0 | FUN_004725f0 | gameplay | C3 | 7 | 1 | 0x0040cfd0 |
| 0x004728f0 | FUN_004728f0 | gameplay | C2 | 9 | 1 | 0x0044c4f0 |
| 0x00473c20 | FUN_00473c20 | gameplay | C2 | 5 | 2 | 0x00493f80 |
| 0x00474890 | FUN_00474890 | gameplay | C2 | 7 | 1 | 0x0042e5b0 |
| 0x0047ba20 | FUN_0047ba20 | gameplay | C2 | 9 | 1 | 0x0047bb10 |
| 0x0047bb10 | FUN_0047bb10 | gameplay | C2 | 8 | 2 | 0x0047c160 |
| 0x0047cf20 | FUN_0047cf20 | gameplay | C2 | 9 | 1 | 0x00407800 |
| 0x0047d130 | Table6c71d8Get | gameplay | C3 | 9 | 1 | 0x00407800 |
| 0x0047d180 | FUN_0047d180 | gameplay | C2 | 9 | 2 | 0x00407800 |
| 0x0047f290 | FUN_0047f290 | gameplay | C2 | 10 | 1 | 0x0047d130 |
| 0x0047f380 | FUN_0047f380 | gameplay | C2 | 10 | 1 | 0x0047d130 |
| 0x0047fad0 | FUN_0047fad0 | gameplay | C2 | 7 | 3 | 0x00412050 |
| 0x004893a0 | FUN_004893a0 | gameplay | C3 | 5 | 2 | 0x00485d90 |
| 0x004893b0 | FUN_004893b0 | gameplay | C3 | 5 | 3 | 0x0045ae80 |
| 0x00489450 | ContRecSet450 | gameplay | C3 | 5 | 6 | 0x0045ae80 |
| 0x00489480 | ContRecSet480 | gameplay | C3 | 5 | 6 | 0x0045ae80 |
| 0x004894a0 | ContRecSet4a0 | gameplay | C3 | 5 | 4 | 0x0045ae80 |
| 0x004894f0 | Clear894f0 | gameplay | C3 | 6 | 1 | 0x00489890 |
| 0x00489500 | FUN_00489500 | gameplay | C2 | 6 | 4 | 0x004893b0 |
| 0x00489890 | FUN_00489890 | gameplay | C2 | 5 | 3 | 0x0045ae80 |
| 0x004898d0 | FUN_004898d0 | gameplay | C2 | 6 | 2 | 0x00489890 |
| 0x00489910 | FUN_00489910 | gameplay | C2 | 6 | 3 | 0x00489890 |
| 0x00401000 | FUN_00401000 | hud | C2 | 4 | 1 | 0x0040bbb0 |
| 0x004011f0 | FUN_004011f0 | hud | C2 | 4 | 1 | 0x004013f0 |
| 0x00401340 | CupSpinSpeedAndColor | hud | C3 | 3 | 1 | 0x00429bd0 |
| 0x004013f0 | FUN_004013f0 | hud | C2 | 3 | 1 | 0x00429bd0 |
| 0x00404820 | FUN_00404820 | hud | C2 | 4 | 1 | 0x0045b930 |
| 0x00413b80 | FUN_00413b80 | hud | C2 | 4 | 3 | 0x0040bbb0 |
| 0x00413bb0 | FUN_00413bb0 | hud | C2 | 5 | 1 | 0x0040bd00 |
| 0x00426ba0 | HudDrawEnabled | hud | C3 | 6 | 2 | 0x00402a40 |
| 0x00427620 | FontText_HudShutdown | hud | C2 | 6 | 2 | 0x00402a40 |
| 0x00427780 | FontText_StringTableLookup | hud | C4 | 4 | 1 | 0x0042c7c0 |
| 0x00427ca0 | FontText_HudInit | hud | C2 | 3 | 2 | 0x00402750 |
| 0x00429300 | HudOverlayFloatGet | hud | C3 | 0 | 0 | (root) |
| 0x00429620 | FUN_00429620 | hud | C2 | 0 | 3 | (root) |
| 0x00429660 | FUN_00429660 | hud | C2 | 2 | 3 | 0x00429e10 |
| 0x00429820 | Clear8991b0Pair | hud | C3 | 7 | 1 | 0x0040e590 |
| 0x0042b8b0 | ScreenWidthGet | hud | C3 | 3 | 6 | 0x00472c60 |
| 0x0042b8c0 | ScreenHeightGet | hud | C3 | 3 | 7 | 0x00472c60 |
| 0x004c1c80 | FUN_004c1c80 | hud | C3 | 2 | 3 | 0x00429e10 |
| 0x004c57a0 | FontCtxMatrix_AllocInit | hud | C2 | 3 | 1 | 0x00459290 |
| 0x00552750 | FontCtx_ResetTransform | hud | C3 | 4 | 1 | 0x004275d0 |
| 0x00552a60 | FontSys_SetActiveCamera | hud | C3 | 4 | 2 | 0x004275d0 |
| 0x00552b60 | FontSys_InitSeq | hud | C2 | 4 | 2 | 0x00427ca0 |
| 0x00552b90 | FontSys_Shutdown | hud | C2 | 7 | 2 | 0x00427620 |
| 0x00552d10 | FontMatrix_Push | hud | C3 | 4 | 1 | 0x004275d0 |
| 0x00554050 | FontCanvas_Shutdown | hud | C2 | 8 | 1 | 0x00552b90 |
| 0x00555280 | FontSys_ShutdownContextPool | hud | C2 | 8 | 1 | 0x00552b90 |
| 0x00555830 | FUN_00555830 | hud | C2 | 7 | 1 | 0x00427620 |
| 0x00556cc0 | SetDat00912a20 | hud | C3 | 4 | 1 | 0x00427ca0 |
| 0x00556cd0 | GetDat00912a20 | hud | C3 | 7 | 1 | 0x00427620 |
| 0x00556ce0 | FontSys_ShutdownBuffers | hud | C2 | 8 | 2 | 0x00552b90 |
| 0x00556d20 | FontSys_InitBuffers | hud | C2 | 9 | 1 | 0x00556ce0 |
| 0x00556d70 | FUN_00556d70 | hud | C2 | 4 | 1 | 0x00427ca0 |
| 0x00556e40 | FUN_00556e40 | hud | C2 | 7 | 1 | 0x00427620 |
| 0x00557110 | FUN_00557110 | hud | C3 | 5 | 2 | 0x00556d70 |
| 0x005571c0 | FontSys_ShutdownFontPool | hud | C2 | 8 | 3 | 0x00552b90 |
| 0x005571e0 | FontSys_InitFontPool | hud | C2 | 9 | 2 | 0x005571c0 |
| 0x00557220 | FontSys_ShutdownDataPools | hud | C2 | 8 | 2 | 0x00552b90 |
| 0x00557250 | FontSys_InitDataPools | hud | C2 | 9 | 1 | 0x00557220 |
| 0x005572c0 | FUN_005572c0 | hud | C2 | 6 | 1 | 0x00557110 |
| 0x0047b860 | FUN_0047b860 | input | C2 | 4 | 3 | 0x0047b9b0 |
| 0x0047b880 | FUN_0047b880 | input | C2 | 4 | 2 | 0x0047b9b0 |
| 0x0047b8a0 | FUN_0047b8a0 | input | C2 | 5 | 1 | 0x0047b860 |
| 0x0047b8d0 | FUN_0047b8d0 | input | C2 | 4 | 3 | 0x0047b9b0 |
| 0x00495790 | Global772facGet | input | C3 | 6 | 1 | 0x004972b0 |
| 0x004957a0 | FUN_004957a0 | input | C3 | 6 | 5 | 0x00495fe0 |
| 0x00495870 | FUN_00495870 | input | C2 | 6 | 1 | 0x00495fe0 |
| 0x00495e80 | FUN_00495e80 | input | C2 | 6 | 2 | 0x00495fe0 |
| 0x00495fe0 | FUN_00495fe0 | input | C2 | 5 | 1 | 0x004967e0 |
| 0x00496040 | FUN_00496040 | input | C2 | 7 | 1 | 0x00495790 |
| 0x00496100 | FUN_00496100 | input | C2 | 5 | 2 | 0x004967e0 |
| 0x00496530 | FUN_00496530 | input | C2 | 5 | 2 | 0x004967e0 |
| 0x004967e0 | FUN_004967e0 | input | C2 | 4 | 3 | 0x00429290 |
| 0x004972b0 | FUN_004972b0 | input | C2 | 5 | 1 | 0x004967e0 |
| 0x00497310 | FUN_00497310 | input | C2 | 6 | 1 | 0x00496530 |
| 0x00497450 | Pred497450 | input | C3 | 5 | 1 | 0x004967e0 |
| 0x004987b0 | FUN_004987b0 | input | C2 | 4 | 5 | 0x004113b0 |
| 0x004b6480 | BitArrayClear | input | C3 | 5 | 4 | 0x004210b0 |
| 0x004b64e0 | FUN_004b64e0 | input | C3 | 3 | 1 | 0x004b6520 |
| 0x004b7140 | FUN_004b7140 | input | C2 | 6 | 1 | 0x004c0510 |
| 0x004b7250 | FUN_004b7250 | input | C2 | 6 | 3 | 0x004c0510 |
| 0x004b7330 | FUN_004b7330 | input | C2 | 5 | 2 | 0x0047b860 |
| 0x004b7480 | FUN_004b7480 | input | C2 | 5 | 3 | 0x0047b880 |
| 0x004b7570 | FUN_004b7570 | input | C2 | 7 | 3 | 0x004b7140 |
| 0x004b7960 | FUN_004b7960 | input | C2 | 7 | 1 | 0x004b7a70 |
| 0x004b7a70 | FUN_004b7a70 | input | C2 | 6 | 1 | 0x0047b8a0 |
| 0x004b7aa0 | FUN_004b7aa0 | input | C2 | 7 | 1 | 0x004b7a70 |
| 0x004b7be0 | FUN_004b7be0 | input | C2 | 6 | 2 | 0x004b7330 |
| 0x004b7df0 | FUN_004b7df0 | input | C2 | 7 | 3 | 0x004b9730 |
| 0x004b7ff0 | FUN_004b7ff0 | input | C2 | 7 | 1 | 0x004b7fd0 |
| 0x004b8340 | FUN_004b8340 | input | C2 | 7 | 1 | 0x004b7250 |
| 0x004b9540 | FUN_004b9540 | input | C3 | 7 | 1 | 0x004b9730 |
| 0x004b9600 | FUN_004b9600 | input | C2 | 7 | 2 | 0x004b9730 |
| 0x004b9650 | FUN_004b9650 | input | C2 | 7 | 2 | 0x004b9730 |
| 0x004b9730 | FUN_004b9730 | input | C2 | 6 | 2 | 0x004c0510 |
| 0x004b9850 | FUN_004b9850 | input | C2 | 6 | 1 | 0x004b7480 |
| 0x004b9aa0 | FUN_004b9aa0 | input | C2 | 7 | 1 | 0x004b7250 |
| 0x004ba1b0 | FUN_004ba1b0 | input | C2 | 6 | 3 | 0x004b7330 |
| 0x004ba210 | FUN_004ba210 | input | C2 | 6 | 1 | 0x004b7480 |
| 0x004c0510 | FUN_004c0510 | input | C2 | 5 | 4 | 0x0047b860 |
| 0x004c06c0 | FUN_004c06c0 | input | C2 | 6 | 1 | 0x004c0510 |
| 0x00485d90 | FUN_00485d90 | particle | C2 | 4 | 2 | 0x0040bbb0 |
| 0x00485e10 | FUN_00485e10 | particle | C2 | 5 | 1 | 0x0040bd00 |
| 0x004862d0 | FUN_004862d0 | particle | C2 | 4 | 2 | 0x0040bbb0 |
| 0x00486350 | FUN_00486350 | particle | C2 | 5 | 1 | 0x0040bd00 |
| 0x00486370 | FUN_00486370 | particle | C2 | 7 | 1 | 0x004864f0 |
| 0x00486460 | FUN_00486460 | particle | C2 | 3 | 1 | 0x004039f0 |
| 0x004864f0 | FUN_004864f0 | particle | C2 | 6 | 1 | 0x00486610 |
| 0x00486610 | FUN_00486610 | particle | C3 | 5 | 2 | 0x0045ae80 |
| 0x00486f90 | FUN_00486f90 | particle | C3 | 5 | 1 | 0x00424eb0 |
| 0x00487140 | FUN_00487140 | particle | C2 | 5 | 1 | 0x0040bd00 |
| 0x00487e00 | FUN_00487e00 | particle | C2 | 4 | 2 | 0x0040bb30 |
| 0x00489290 | FUN_00489290 | particle | C3 | 4 | 1 | 0x0040bbb0 |
| 0x004892c0 | FUN_004892c0 | particle | C2 | 5 | 2 | 0x00485d90 |
| 0x00489940 | FUN_00489940 | particle | C2 | 6 | 2 | 0x004893a0 |
| 0x00489a40 | FUN_00489a40 | particle | C2 | 12 | 1 | 0x00406160 |
| 0x00489c30 | FUN_00489c30 | particle | C2 | 11 | 1 | 0x00406130 |
| 0x0048a460 | StridedClear2_709238 | particle | C3 | 4 | 1 | 0x0040bbb0 |
| 0x0048a490 | FUN_0048a490 | particle | C2 | 5 | 1 | 0x0048a5d0 |
| 0x0048a5d0 | FUN_0048a5d0 | particle | C2 | 4 | 1 | 0x00474d80 |
| 0x0048a830 | FUN_0048a830 | particle | C3 | 4 | 1 | 0x0040bbb0 |
| 0x0048a850 | FUN_0048a850 | particle | C2 | 11 | 1 | 0x0048aa20 |
| 0x0048ae00 | FUN_0048ae00 | particle | C2 | 4 | 2 | 0x0040bbb0 |
| 0x0048af70 | FUN_0048af70 | particle | C2 | 5 | 1 | 0x0040bd00 |
| 0x0048bbe0 | FUN_0048bbe0 | particle | C2 | 4 | 2 | 0x0040bbb0 |
| 0x0048bc10 | FUN_0048bc10 | particle | C2 | 5 | 1 | 0x0040bd00 |
| 0x0048cee0 | FUN_0048cee0 | particle | C2 | 5 | 2 | 0x0048e820 |
| 0x0048cf70 | FUN_0048cf70 | particle | C2 | 5 | 1 | 0x0040bd00 |
| 0x0048e820 | FUN_0048e820 | particle | C2 | 4 | 1 | 0x0040bbb0 |
| 0x0048eac0 | FUN_0048eac0 | particle | C2 | 4 | 1 | 0x0040bbb0 |
| 0x0048fdd0 | FUN_0048fdd0 | particle | C2 | 5 | 1 | 0x0040bd00 |
| 0x0048fef0 | FUN_0048fef0 | particle | C2 | 5 | 1 | 0x0048ff20 |
| 0x0048ff20 | FUN_0048ff20 | particle | C2 | 4 | 2 | 0x0040bbb0 |
| 0x0048ffd0 | FUN_0048ffd0 | particle | C2 | 5 | 1 | 0x0040bd00 |
| 0x00478cb0 | FUN_00478cb0 | physics | C3 | 5 | 1 | 0x0047a020 |
| 0x0047b9b0 | FUN_0047b9b0 | physics | C2 | 3 | 4 | 0x0045bae0 |
| 0x0047ce40 | Search47ce40 | physics | C3 | 7 | 1 | 0x004715a0 |
| 0x0047cea0 | FUN_0047cea0 | physics | C2 | 3 | 1 | 0x00422ba0 |
| 0x0047f940 | FUN_0047f940 | physics | C2 | 8 | 1 | 0x0047ce40 |
| 0x0045b9d0 | FUN_0045b9d0 | powerups | C2 | 13 | 2 | 0x00456c70 |
| 0x0045baa0 | PowerupTypeLookup | powerups | C3 | 6 | 2 | 0x0045bfa0 |
| 0x0045bac0 | PowerupSlotDeactivate | powerups | C2 | 1 | 4 | 0x0045bed0 |
| 0x0045bed0 | PowerupRoundCleanup | powerups | C2 | 0 | 2 | (root) |
| 0x0045bf30 | PowerupTeardownAll | powerups | C2 | 0 | 2 | (root) |
| 0x0045bfa0 | PowerupSlotActivate | powerups | C2 | 5 | 2 | 0x0045c010 |
| 0x0045c010 | PowerupPickupWrapper | powerups | C2 | 4 | 1 | 0x0045bba0 |
| 0x00401690 | FUN_00401690 | render | C2 | 4 | 1 | 0x004025f0 |
| 0x00402240 | FUN_00402240 | render | C2 | 4 | 1 | 0x004025f0 |
| 0x00402590 | FUN_00402590 | render | C2 | 4 | 1 | 0x004025f0 |
| 0x004034a0 | FUN_004034a0 | render | C2 | 4 | 1 | 0x00403640 |
| 0x004036a0 | FUN_004036a0 | render | C2 | 4 | 1 | 0x00403910 |
| 0x00403910 | FUN_00403910 | render | C2 | 3 | 1 | 0x004938e0 |
| 0x00404fa0 | FUN_00404fa0 | render | C2 | 8 | 1 | 0x00405460 |
| 0x004053d0 | TrackDataSlotSet | render | C3 | 6 | 1 | 0x00426e10 |
| 0x004072e0 | FUN_004072e0 | render | C2 | 11 | 1 | 0x00406130 |
| 0x00409680 | LedArrayInit | render | C3 | 2 | 2 | 0x00409710 |
| 0x00409710 | FUN_00409710 | render | C2 | 1 | 2 | 0x004235b0 |
| 0x0040b090 | sub_0040b090 | render | C2 | 7 | 2 | 0x00426ab0 |
| 0x00412050 | FUN_00412050 | render | C2 | 6 | 2 | 0x00426e10 |
| 0x0041a980 | FUN_0041a980 | render | C2 | 7 | 1 | 0x0040cf80 |
| 0x0041ad60 | FUN_0041ad60 | render | C2 | 4 | 3 | 0x0041b450 |
| 0x0041b440 | FUN_0041b440 | render | C2 | 5 | 1 | 0x0041b660 |
| 0x0041b690 | FUN_0041b690 | render | C2 | 4 | 2 | 0x0041bec0 |
| 0x0041beb0 | FUN_0041beb0 | render | C2 | 5 | 1 | 0x0041c0e0 |
| 0x0041cb00 | FUN_0041cb00 | render | C2 | 7 | 1 | 0x0041ccf0 |
| 0x0041cd20 | FUN_0041cd20 | render | C2 | 4 | 1 | 0x0041d6e0 |
| 0x0041d6d0 | FUN_0041d6d0 | render | C2 | 5 | 1 | 0x0041d890 |
| 0x0041e870 | TrackNodeRecordScan | render | C3 | 6 | 3 | 0x00426e10 |
| 0x0041e8b0 | TrackNodeDispatch14 | render | C3 | 6 | 4 | 0x00426e10 |
| 0x0041e8c0 | TrackNodeDispatch18 | render | C3 | 6 | 3 | 0x0041e9b0 |
| 0x0041e970 | TrackNodeDispatch44 | render | C3 | 6 | 3 | 0x00426e10 |
| 0x0041e980 | TrackNodeRecordFind | render | C3 | 6 | 1 | 0x00426e10 |
| 0x0041e9b0 | TrackNodeFieldCmp10 | render | C3 | 5 | 4 | 0x00426640 |
| 0x0041e9d0 | TrackNodeFnPtrGet14 | render | C3 | 6 | 3 | 0x00426e10 |
| 0x0041e9e0 | TrackNodeFnPtrGet18 | render | C3 | 6 | 3 | 0x0041e9b0 |
| 0x0041ea90 | TrackNodeFnPtrGet44 | render | C3 | 6 | 2 | 0x00426e10 |
| 0x0041ecc0 | FUN_0041ecc0 | render | C2 | 2 | 1 | 0x0040cea0 |
| 0x00421100 | FUN_00421100 | render | C2 | 6 | 2 | 0x00421160 |
| 0x00421160 | FUN_00421160 | render | C2 | 5 | 1 | 0x004210b0 |
| 0x004215e0 | FUN_004215e0 | render | C2 | 11 | 1 | 0x00421630 |
| 0x00421630 | FUN_00421630 | render | C2 | 10 | 1 | 0x00405ab0 |
| 0x00421690 | FUN_00421690 | render | C2 | 5 | 2 | 0x00424eb0 |
| 0x00421ba0 | FUN_00421ba0 | render | C2 | 9 | 1 | 0x004220d0 |
| 0x00421c80 | FUN_00421c80 | render | C2 | 8 | 1 | 0x00422140 |
| 0x004220d0 | FUN_004220d0 | render | C2 | 8 | 1 | 0x00422140 |
| 0x00422140 | FUN_00422140 | render | C2 | 7 | 1 | 0x0040cf80 |
| 0x004235b0 | FUN_004235b0 | render | C3 | 0 | 3 | (root) |
| 0x00423630 | AiDataBufInit | render | C3 | 1 | 2 | 0x004235b0 |
| 0x00425ab0 | EntryHeaderClear | render | C3 | 6 | 1 | 0x00426e10 |
| 0x00425e40 | FUN_00425e40 | render | C2 | 7 | 1 | 0x00425ab0 |
| 0x00426060 | TrackPhysWorld1Get | render | C3 | 4 | 2 | 0x004013f0 |
| 0x004260e0 | FUN_004260e0 | render | C2 | 6 | 1 | 0x00426e10 |
| 0x004262f0 | FUN_004262f0 | render | C2 | 6 | 1 | 0x00426e10 |
| 0x00426640 | FUN_00426640 | render | C2 | 4 | 2 | 0x0041ea70 |
| 0x00426700 | FUN_00426700 | render | C2 | 7 | 1 | 0x00426ab0 |
| 0x00426780 | FUN_00426780 | render | C2 | 7 | 1 | 0x00426ab0 |
| 0x00426810 | sub_00426810 | render | C2 | 7 | 1 | 0x00426ab0 |
| 0x00426ab0 | sub_00426ab0 | render | C2 | 6 | 2 | 0x0041e9b0 |
| 0x00426cd0 | TrackSlotArrayReset | render | C3 | 6 | 1 | 0x00426e10 |
| 0x00426e10 | FUN_00426e10 | render | C2 | 5 | 8 | 0x0047a020 |
| 0x004270f0 | CourseRenderFrame | render | C2 | 4 | 2 | 0x0041e8d0 |
| 0x00429e10 | FUN_00429e10 | render | C2 | 1 | 3 | 0x0040e4a0 |
| 0x0042a470 | FUN_0042a470 | render | C2 | 5 | 1 | 0x0042a530 |
| 0x0042a530 | FUN_0042a530 | render | C2 | 4 | 5 | 0x0042a6b0 |
| 0x0042a5d0 | FUN_0042a5d0 | render | C2 | 4 | 9 | 0x0041a1e0 |
| 0x0042a8d0 | FUN_0042a8d0 | render | C2 | 6 | 1 | 0x00426e10 |
| 0x0042c010 | FUN_0042c010 | render | C2 | 10 | 1 | 0x00472b10 |
| 0x0042f510 | Vehicle0HandleGet | render | C3 | 1 | 4 | 0x004671a0 |
| 0x0044c4f0 | FUN_0044c4f0 | render | C2 | 8 | 1 | 0x0047cdc0 |
| 0x0044c8b0 | FUN_0044c8b0 | render | C2 | 9 | 1 | 0x0044c4f0 |
| 0x0044df80 | FUN_0044df80 | render | C2 | 0 | 1 | (root) |
| 0x00454f80 | FUN_00454f80 | render | C2 | 9 | 1 | 0x00407800 |
| 0x00455fe0 | FUN_00455fe0 | render | C2 | 12 | 2 | 0x00456c30 |
| 0x0045b350 | RwInitNullStub | render | C3 | 1 | 11 | 0x0043dfd0 |
| 0x0045b990 | FUN_0045b990 | render | C2 | 12 | 1 | 0x00456c30 |
| 0x00462950 | FUN_00462950 | render | C2 | 6 | 2 | 0x005a7af0 |
| 0x004671a0 | sub_004671a0 | render | C2 | 0 | 12 | (root) |
| 0x004671d0 | FUN_004671d0 | render | C2 | 5 | 3 | 0x0045a190 |
| 0x00467210 | sub_00467210 | render | C2 | 6 | 2 | 0x0045d7a0 |
| 0x0046d400 | FUN_0046d400 | render | C2 | 6 | 1 | 0x004704c0 |
| 0x00472b10 | FUN_00472b10 | render | C2 | 9 | 2 | 0x00473220 |
| 0x00473220 | FUN_00473220 | render | C2 | 8 | 2 | 0x00473ee0 |
| 0x004733b0 | FUN_004733b0 | render | C2 | 8 | 1 | 0x00473ee0 |
| 0x00474d80 | ByteFlag474d80 | render | C3 | 3 | 3 | 0x004568d0 |
| 0x00474db0 | FUN_00474db0 | render | C2 | 5 | 4 | 0x00475a00 |
| 0x00474de0 | FUN_00474de0 | render | C2 | 10 | 2 | 0x00405ab0 |
| 0x00474e70 | FUN_00474e70 | render | C2 | 6 | 1 | 0x00459480 |
| 0x00474fb0 | FUN_00474fb0 | render | C2 | 9 | 1 | 0x00475010 |
| 0x00475010 | FUN_00475010 | render | C2 | 8 | 1 | 0x00426780 |
| 0x004752f0 | FUN_004752f0 | render | C2 | 9 | 1 | 0x004756e0 |
| 0x004756e0 | FUN_004756e0 | render | C2 | 8 | 1 | 0x00426700 |
| 0x00475770 | FUN_00475770 | render | C2 | 9 | 1 | 0x00405540 |
| 0x004759b0 | FUN_004759b0 | render | C2 | 4 | 2 | 0x0040bbb0 |
| 0x00475a00 | FUN_00475a00 | render | C2 | 4 | 2 | 0x0040bb30 |
| 0x00475d90 | FUN_00475d90 | render | C2 | 5 | 1 | 0x0040bd00 |
| 0x00475ea0 | FUN_00475ea0 | render | C2 | 5 | 1 | 0x0040bd00 |
| 0x00475ef0 | FUN_00475ef0 | render | C2 | 5 | 1 | 0x00476390 |
| 0x00476320 | FUN_00476320 | render | C2 | 10 | 1 | 0x00405ab0 |
| 0x00476390 | FUN_00476390 | render | C2 | 4 | 1 | 0x0040bbb0 |
| 0x00476650 | FUN_00476650 | render | C2 | 4 | 2 | 0x00476860 |
| 0x00476710 | FUN_00476710 | render | C2 | 5 | 1 | 0x00476880 |
| 0x00476860 | FUN_00476860 | render | C2 | 3 | 2 | 0x004039f0 |
| 0x004768c0 | FUN_004768c0 | render | C2 | 5 | 14 | 0x004593b0 |
| 0x004769a0 | ParticleEmitter_SetPosition | render | C3 | 3 | 4 | 0x00459000 |
| 0x004769d0 | ParticleEmitter_SetVelocity | render | C3 | 3 | 4 | 0x00459000 |
| 0x004769f0 | ParticleEmitter_SetColour | render | C3 | 3 | 4 | 0x00459000 |
| 0x00476a30 | ParticleEmitter_SetScalar | render | C3 | 3 | 4 | 0x00459000 |
| 0x00476a40 | ParticleEmitter_SetRGBA | render | C3 | 6 | 2 | 0x004894a0 |
| 0x00476be0 | Batch476be0 | render | C3 | 5 | 1 | 0x004862d0 |
| 0x00476d00 | FUN_00476d00 | render | C3 | 3 | 4 | 0x00459000 |
| 0x00476df0 | FUN_00476df0 | render | C2 | 3 | 4 | 0x004568d0 |
| 0x004770a0 | FUN_004770a0 | render | C2 | 6 | 5 | 0x00489890 |
| 0x004775b0 | FUN_004775b0 | render | C2 | 4 | 2 | 0x0040bbb0 |
| 0x00477870 | FUN_00477870 | render | C2 | 5 | 1 | 0x0040bd00 |
| 0x00477e40 | FUN_00477e40 | render | C2 | 4 | 1 | 0x0040bbb0 |
| 0x00477e60 | FUN_00477e60 | render | C2 | 9 | 1 | 0x0047bb10 |
| 0x00477f00 | FUN_00477f00 | render | C2 | 9 | 1 | 0x0047bb10 |
| 0x0047cdc0 | StoreDistSq47cdc0 | render | C3 | 7 | 3 | 0x00412050 |
| 0x00480340 | FUN_00480340 | render | C2 | 6 | 1 | 0x00426e10 |
| 0x00487020 | FUN_00487020 | render | C2 | 13 | 1 | 0x00453eb0 |
| 0x0048aa20 | FUN_0048aa20 | render | C2 | 10 | 1 | 0x00405ab0 |
| 0x00490000 | FUN_00490000 | render | C2 | 7 | 1 | 0x0040cfd0 |
| 0x00491070 | RainColorInit | render | C3 | 8 | 1 | 0x00491590 |
| 0x004910c0 | FUN_004910c0 | render | C2 | 8 | 1 | 0x00491490 |
| 0x00491340 | FUN_00491340 | render | C2 | 8 | 1 | 0x00491490 |
| 0x00491490 | FUN_00491490 | render | C2 | 7 | 2 | 0x00426ab0 |
| 0x00491590 | FUN_00491590 | render | C2 | 7 | 1 | 0x00491780 |
| 0x00491780 | FUN_00491780 | render | C2 | 6 | 2 | 0x00426e10 |
| 0x004924c0 | Set6147b4Triple | render | C3 | 6 | 1 | 0x00426e10 |
| 0x00494fd0 | FUN_00494fd0 | render | C2 | 2 | 1 | 0x00495080 |
| 0x00495280 | FUN_00495280 | render | C2 | 1 | 5 | 0x004235b0 |
| 0x004952f0 | FUN_004952f0 | render | C2 | 1 | 7 | 0x004235b0 |
| 0x00496940 | FUN_00496940 | render | C2 | 4 | 2 | 0x0047ba00 |
| 0x00496970 | FUN_00496970 | render | C2 | 6 | 1 | 0x00402a40 |
| 0x004969a0 | FUN_004969a0 | render | C2 | 4 | 1 | 0x00496c10 |
| 0x00496c10 | FUN_00496c10 | render | C2 | 3 | 1 | 0x004938e0 |
| 0x00498bc0 | VideoGetRenderWidth | render | C4 | 3 | 1 | 0x00402750 |
| 0x00498bd0 | VideoGetRenderHeight | render | C4 | 3 | 1 | 0x00402750 |
| 0x00499710 | Global7e9584Get | render | C3 | 4 | 2 | 0x00495350 |
| 0x00499ce0 | FUN_00499ce0 | render | C2 | 3 | 1 | 0x00402750 |
| 0x004a2cbd | FID_conflict:_wprintf | render | C2 | 0 | 0 | (root) |
| 0x004a2d18 | FUN_004a2d18 | render | C2 | 1 | 1 | 0x004a2cbd |
| 0x004a42c5 | FUN_004a42c5 | render | C2 | 5 | 2 | 0x004987b0 |
| 0x004a504f | FUN_004a504f | render | C2 | 1 | 4 | 0x004a2cbd |
| 0x004b3c60 | FUN_004b3c60 | render | C2 | 6 | 1 | 0x0042a640 |
| 0x004b3d20 | FUN_004b3d20 | render | C2 | 4 | 1 | 0x0042a6b0 |
| 0x004b3d80 | FUN_004b3d80 | render | C2 | 3 | 5 | 0x00402750 |
| 0x004b4080 | FUN_004b4080 | render | C2 | 4 | 1 | 0x00401340 |
| 0x004b40c0 | FUN_004b40c0 | render | C3 | 7 | 2 | 0x004b4130 |
| 0x004b4130 | FUN_004b4130 | render | C3 | 6 | 2 | 0x004b5260 |
| 0x004b4140 | FUN_004b4140 | render | C3 | 6 | 2 | 0x004b5260 |
| 0x004b4650 | FUN_004b4650 | render | C3 | 5 | 1 | 0x0045ae80 |
| 0x004b46b0 | FUN_004b46b0 | render | C3 | 7 | 1 | 0x00478660 |
| 0x004b47e0 | FUN_004b47e0 | render | C2 | 9 | 3 | 0x004756e0 |
| 0x004b4a80 | FUN_004b4a80 | render | C2 | 5 | 3 | 0x004b4b60 |
| 0x004b4ad0 | FUN_004b4ad0 | render | C2 | 13 | 1 | 0x004b4d10 |
| 0x004b4b60 | FUN_004b4b60 | render | C2 | 4 | 2 | 0x0045bba0 |
| 0x004b4c80 | FUN_004b4c80 | render | C2 | 13 | 1 | 0x004b4d10 |
| 0x004b4d10 | FUN_004b4d10 | render | C2 | 12 | 1 | 0x00406160 |
| 0x004b5030 | FUN_004b5030 | render | C2 | 7 | 1 | 0x00478660 |
| 0x004b5080 | FUN_004b5080 | render | C2 | 5 | 2 | 0x0045ae80 |
| 0x004b51d0 | FUN_004b51d0 | render | C2 | 4 | 4 | 0x0041a1e0 |
| 0x004b5260 | FUN_004b5260 | render | C2 | 5 | 2 | 0x0041b690 |
| 0x004b52f0 | FUN_004b52f0 | render | C3 | 4 | 3 | 0x0041a1e0 |
| 0x004b5350 | thunk_FUN_00550b00 | render | C2 | 6 | 1 | 0x0042a470 |
| 0x004b53b0 | FUN_004b53b0 | render | C2 | 8 | 1 | 0x0047bf70 |
| 0x004b55a0 | FUN_004b55a0 | render | C2 | 0 | 2 | (root) |
| 0x004b59c0 | FUN_004b59c0 | render | C2 | 12 | 1 | 0x004072e0 |
| 0x004b65c0 | FUN_004b65c0 | render | C3 | 2 | 1 | 0x004952f0 |
| 0x004b6700 | FUN_004b6700 | render | C3 | 6 | 1 | 0x00402a40 |
| 0x004b67a0 | FUN_004b67a0 | render | C2 | 2 | 1 | 0x004952f0 |
| 0x004b68e0 | Global7d3e4cGet | render | C3 | 4 | 3 | 0x00411f30 |
| 0x004b68f0 | FUN_004b68f0 | render | C2 | 4 | 3 | 0x00411f30 |
| 0x004b7fd0 | FUN_004b7fd0 | render | C2 | 6 | 2 | 0x004c0510 |
| 0x004c0e50 | FUN_004c0e50 | render | C3 | 4 | 4 | 0x004b3bf0 |
| 0x004c0ed0 | FUN_004c0ed0 | render | C2 | 2 | 9 | 0x0045a950 |
| 0x004c1340 | FUN_004c1340 | render | C2 | 4 | 2 | 0x00458a40 |
| 0x004c1480 | FUN_004c1480 | render | C2 | 3 | 5 | 0x004568d0 |
| 0x004c1a70 | FUN_004c1a70 | render | C2 | 7 | 1 | 0x004c1b10 |
| 0x004c1b10 | FUN_004c1b10 | render | C2 | 6 | 1 | 0x00426e10 |
| 0x004c2ed0 | RwEngineGetModeInfo | render | C3 | 3 | 1 | 0x00402750 |
| 0x004c2f00 | RwEngineGetCurrentMode | render | C4 | 3 | 1 | 0x00402750 |
| 0x004c39b0 | RwV3dNormalize | render | C4 | 0 | 13 | (root) |
| 0x004c3bf0 | Vec2Length | render | C4 | 0 | 5 | (root) |
| 0x004c3c60 | Vec2Normalize | render | C4 | 0 | 1 | (root) |
| 0x004c3d90 | FUN_004c3d90 | render | C2 | 3 | 4 | 0x004568d0 |
| 0x004c3df0 | RwV3dTransformPoints | render | C4 | 0 | 2 | (root) |
| 0x004c45f0 | FUN_004c45f0 | render | C3 | 6 | 2 | 0x00459480 |
| 0x004c4d20 | RwMatrixRotate | render | C4 | 0 | 7 | (root) |
| 0x004c4eb0 | RwMatrixInvert_CofactorPath | render | C2 | 4 | 1 | 0x004c4dc0 |
| 0x004c5010 | RwMatrixScale | render | C4 | 6 | 1 | 0x00459480 |
| 0x004c51a0 | RwMatrixTranslate | render | C2 | 5 | 8 | 0x00442440 |
| 0x004c52f0 | RwMatrixCombine | render | C2 | 4 | 1 | 0x004c1480 |
| 0x004c5770 | FUN_004c5770 | render | C2 | 5 | 2 | 0x004593b0 |
| 0x004c5a60 | FUN_004c5a60 | render | C2 | 5 | 5 | 0x00556d70 |
| 0x004c5bc0 | FUN_004c5bc0 | render | C3 | 4 | 1 | 0x0040bbb0 |
| 0x004c5c80 | FUN_004c5c80 | render | C3 | 3 | 8 | 0x00459290 |
| 0x004c5cb0 | FUN_004c5cb0 | render | C2 | 4 | 3 | 0x0040bbb0 |
| 0x004c7650 | FUN_004c7650 | render | C2 | 2 | 5 | 0x00494460 |
| 0x004c7730 | FUN_004c7730 | render | C2 | 5 | 2 | 0x004c1be0 |
| 0x004c77c0 | FUN_004c77c0 | render | C2 | 2 | 2 | 0x00494a80 |
| 0x004caea0 | FUN_004caea0 | render | C2 | 3 | 1 | 0x00402750 |
| 0x004cbd30 | RwStreamRead | render | C3 | 1 | 3 | 0x004235b0 |
| 0x004cc050 | RwStreamSkip | render | C3 | 1 | 1 | 0x004cc5e0 |
| 0x004cc400 | FUN_004cc400 | render | C2 | 1 | 1 | 0x004cc5e0 |
| 0x004cc5e0 | FUN_004cc5e0 | render | C2 | 0 | 6 | (root) |
| 0x004cc820 | FUN_004cc820 | render | C2 | 10 | 3 | 0x00556d20 |
| 0x004cc9f0 | RwFreeListDestroy | render | C2 | 9 | 3 | 0x00556ce0 |
| 0x004cd070 | RwRenderPrimitiveSubmit | render | C2 | 13 | 1 | 0x004b59c0 |
| 0x004cd140 | RwRenderCommandBufferReset | render | C3 | 13 | 1 | 0x004b59c0 |
| 0x004cd430 | FUN_004cd430 | render | C2 | 13 | 1 | 0x004b59c0 |
| 0x004ce2d0 | FUN_004ce2d0 | render | C2 | 4 | 4 | 0x004283a0 |
| 0x004cf7d0 | FUN_004cf7d0 | render | C2 | 5 | 1 | 0x004b3d20 |
| 0x004d40d0 | FUN_004d40d0 | render | C2 | 14 | 1 | 0x004cd070 |
| 0x004d41e0 | FUN_004d41e0 | render | C2 | 7 | 1 | 0x00496ce0 |
| 0x004d7ff0 | RwIdentityPassthrough | render | C3 | 1 | 7 | 0x004cc5e0 |
| 0x004d8060 | FUN_004d8060 | render | C2 | 3 | 1 | 0x004c7650 |
| 0x004d8350 | FUN_004d8350 | render | C2 | 3 | 1 | 0x004c0ed0 |
| 0x004d8480 | RwErrSlotWrite | render | C3 | 2 | 6 | 0x004cc160 |
| 0x004e4320 | FUN_004e4320 | render | C2 | 4 | 2 | 0x004013f0 |
| 0x004e4350 | FUN_004e4350 | render | C3 | 4 | 1 | 0x004013f0 |
| 0x004e6680 | RpClumpRender | render | C2 | 4 | 4 | 0x004013f0 |
| 0x004e66d0 | RpClumpForAllAtomics | render | C2 | 4 | 3 | 0x00474d60 |
| 0x00552d70 | ViewportStackPop | render | C3 | 4 | 1 | 0x004275d0 |
| 0x00556e90 | FUN_00556e90 | render | C2 | 4 | 2 | 0x00427ca0 |
| 0x00404e50 | SaveLoad | save | C4 | 2 | 1 | 0x0040acd0 |
| 0x00404e80 | Save::DeserializeFromBuffer | save | C4 | 0 | 1 | (root) |
| 0x00404ee0 | Save::SerializeToBuffer | save | C4 | 0 | 2 | (root) |
| 0x00404f50 | SaveWrite | save | C4 | 2 | 1 | 0x0040acd0 |
| 0x00404f80 | SaveFileExists | save | C4 | 2 | 1 | 0x0040acd0 |
| 0x004099a0 | Save::AutosaveTrigger | save | C4 | 2 | 1 | 0x0040acd0 |
| 0x004099e0 | SaveStatusClear | save | C4 | 2 | 1 | 0x0040acd0 |
| 0x0040dd60 | Race::GuardConcludedAndP1Won | save | C4 | 1 | 1 | 0x00410510 |
| 0x00410510 | Race::EvaluateResult | save | C3 | 0 | 0 | (root) |
| 0x004963e0 | ConfigLogError | save | C4 | 2 | 3 | 0x00495280 |
| 0x00496400 | ConfigLogDebug | save | C4 | 2 | 3 | 0x00495280 |
| 0x004b3b70 | FileReadWrapper_i3 | save | C4 | 4 | 3 | 0x00411f30 |
| 0x00550910 | FUN_00550910 | save | C2 | 2 | 1 | 0x004cc160 |
| 0x00550b00 | VfsFileExists | save | C4 | 6 | 2 | 0x0042a470 |
| 0x004b7740 | FUN_004b7740 | third-party-library[lua-5.0] | C2 | 8 | 1 | 0x004b8340 |
| 0x004b79c0 | FUN_004b79c0 | third-party-library[lua-5.0] | C2 | 8 | 1 | 0x004b7aa0 |
| 0x004b7b00 | FUN_004b7b00 | third-party-library[lua-5.0] | C2 | 7 | 4 | 0x004ba1b0 |
| 0x004b7ba0 | FUN_004b7ba0 | third-party-library[lua-5.0] | C2 | 7 | 2 | 0x004ba1b0 |
| 0x004b7c70 | FUN_004b7c70 | third-party-library[lua-5.0] | C2 | 7 | 1 | 0x004b7be0 |
| 0x004b9630 | FUN_004b9630 | third-party-library[lua-5.0] | C2 | 8 | 1 | 0x004b8340 |
| 0x004b96c0 | FUN_004b96c0 | third-party-library[lua-5.0] | C2 | 8 | 1 | 0x004b9650 |
| 0x004b9950 | FUN_004b9950 | third-party-library[lua-5.0] | C2 | 8 | 1 | 0x004b9aa0 |
| 0x004b9bb0 | FUN_004b9bb0 | third-party-library[lua-5.0] | C2 | 8 | 1 | 0x004b8340 |
| 0x004b9ef0 | FUN_004b9ef0 | third-party-library[lua-5.0] | C2 | 8 | 1 | 0x004b8340 |
| 0x004ba250 | FUN_004ba250 | third-party-library[lua-5.0] | C2 | 7 | 1 | 0x004ba210 |
| 0x004ba290 | FUN_004ba290 | third-party-library[lua-5.0] | C2 | 7 | 1 | 0x004ba210 |
| 0x004ba2d0 | FUN_004ba2d0 | third-party-library[lua-5.0] | C2 | 7 | 1 | 0x004ba210 |
| 0x004ba310 | FUN_004ba310 | third-party-library[lua-5.0] | C2 | 7 | 1 | 0x004ba210 |
| 0x004ba3e0 | FUN_004ba3e0 | third-party-library[lua-5.0] | C2 | 7 | 1 | 0x004ba210 |
| 0x004ba470 | FUN_004ba470 | third-party-library[lua-5.0] | C2 | 7 | 1 | 0x004ba210 |
| 0x004bc440 | FUN_004bc440 | third-party-library[lua-5.0] | C2 | 8 | 1 | 0x004b7aa0 |
| 0x004bee20 | lfunc_newupval | third-party-library[lua-5.0] | C2 | 8 | 1 | 0x004b7ff0 |
| 0x004c0740 | FUN_004c0740 | third-party-library[lua-5.0] | C2 | 7 | 2 | 0x00467070 |
| 0x004c0c20 | FUN_004c0c20 | third-party-library[lua-5.0] | C2 | 7 | 4 | 0x0041ffb0 |
| 0x004c1520 | FUN_004c1520 | third-party-library[lua-5.0] | C2 | 3 | 2 | 0x004568d0 |
| 0x004c1b40 | FUN_004c1b40 | third-party-library[lua-5.0] | C2 | 8 | 1 | 0x0047c160 |
| 0x004c1cf0 | FUN_004c1cf0 | third-party-library[lua-5.0] | C2 | 7 | 3 | 0x00467020 |
| 0x004a0ef0 | FUN_004a0ef0 | third-party-library[msvc-crt-main] | C2 | 7 | 1 | 0x0049dd60 |
| 0x004a1160 | FUN_004a1160 | third-party-library[msvc-crt-main] | C2 | 7 | 1 | 0x0049dd60 |
| 0x004a3090 | FUN_004a3090 | third-party-library[msvc-crt-main] | C2 | 3 | 1 | 0x004039f0 |
| 0x004a39d4 | FUN_004a39d4 | third-party-library[msvc-crt-main] | C2 | 10 | 1 | 0x00475770 |
| 0x004a4ea8 | FUN_004a4ea8 | third-party-library[msvc-crt-main] | C2 | 2 | 1 | 0x004a2b60 |
| 0x004b3f90 | FUN_004b3f90 | third-party-library[msvc-crt-main] | C2 | 4 | 5 | 0x00411f30 |
| 0x004b3fc0 | FUN_004b3fc0 | third-party-library[msvc-crt-main] | C2 | 5 | 5 | 0x004b51d0 |
| 0x004e6e00 | RpClumpDestroy | third-party-library[renderware] | C1 | 5 | 5 | 0x004015a0 |
| 0x00534870 | FUN_00534870 | third-party-library[renderware] | C1 | 0 | 0 | (root) |
| 0x00552920 | FUN_00552920 | third-party-library[renderware] | C1 | 4 | 1 | 0x004275d0 |
| 0x00553cf0 | FUN_00553cf0 | third-party-library[renderware] | C1 | 4 | 1 | 0x004275d0 |
| 0x00553e80 | FUN_00553e80 | third-party-library[renderware] | C1 | 4 | 1 | 0x004275d0 |
| 0x00553ef0 | FUN_00553ef0 | third-party-library[renderware] | C1 | 4 | 1 | 0x004275d0 |
| 0x00401630 | FUN_00401630 | track | C2 | 4 | 1 | 0x004025f0 |
| 0x0040cea0 | FUN_0040cea0 | track | C2 | 1 | 1 | 0x0040d110 |
| 0x0040d110 | FUN_0040d110 | track | C2 | 0 | 0 | (root) |
| 0x00420420 | FUN_00420420 | track | C2 | 1 | 1 | 0x0040d110 |
| 0x004790e0 | FUN_004790e0 | track | C2 | 7 | 1 | 0x00479330 |
| 0x00479330 | FUN_00479330 | track | C2 | 6 | 1 | 0x00426e10 |
| 0x0047a0f0 | FUN_0047a0f0 | track | C2 | 4 | 2 | 0x0047b9b0 |
| 0x0047bcc0 | FUN_0047bcc0 | track | C2 | 8 | 1 | 0x0047bf70 |
| 0x0047be80 | FUN_0047be80 | track | C2 | 8 | 1 | 0x0047bf70 |
| 0x0047bf70 | FUN_0047bf70 | track | C2 | 7 | 1 | 0x0047c0f0 |
| 0x0047c0b0 | ClearColl47c0b0 | track | C3 | 6 | 1 | 0x00426e10 |
| 0x0047c0f0 | FUN_0047c0f0 | track | C2 | 6 | 4 | 0x00426e10 |
| 0x00402b70 | FUN_00402b70 | util | C2 | 2 | 1 | 0x00495280 |
| 0x00402f40 | Util636ad8Get | util | C3 | 1 | 1 | 0x0043dfd0 |
| 0x004039f0 | FUN_004039f0 | util | C2 | 2 | 1 | 0x004215c0 |
| 0x004046a0 | FUN_004046a0 | util | C2 | 11 | 1 | 0x004173a0 |
| 0x00405460 | FUN_00405460 | util | C2 | 7 | 2 | 0x004102f0 |
| 0x004073b0 | FUN_004073b0 | util | C2 | 9 | 1 | 0x00407800 |
| 0x00407600 | Player::GetPositionPtr | util | C2 | 6 | 1 | 0x00464a50 |
| 0x00408b00 | FUN_00408b00 | util | C2 | 2 | 4 | 0x00448220 |
| 0x00409290 | FUN_00409290 | util | C2 | 5 | 2 | 0x00424eb0 |
| 0x004098b0 | LoadingState1Enter | util | C4 | 1 | 1 | 0x0043dfd0 |
| 0x00409900 | LoadingState2Enter | util | C4 | 1 | 2 | 0x0043dfd0 |
| 0x00409930 | LoadingState3Enter | util | C4 | 1 | 1 | 0x0043dfd0 |
| 0x0040ad30 | FUN_0040ad30 | util | C3 | 1 | 1 | 0x0043dfd0 |
| 0x0040b250 | FUN_0040b250 | util | C3 | 8 | 1 | 0x0040b410 |
| 0x0040b410 | RaceScoreTimerGet | util | C3 | 7 | 1 | 0x0040e590 |
| 0x0040b6d0 | FUN_0040b6d0 | util | C3 | 0 | 2 | (root) |
| 0x0040d470 | FUN_0040d470 | util | C2 | 7 | 2 | 0x004102f0 |
| 0x0040dc80 | UtilFloat63b910Get | util | C3 | 2 | 3 | 0x00429e10 |
| 0x0040dc90 | UtilSlotIndexCondGet | util | C3 | 3 | 3 | 0x00429660 |
| 0x0040e170 | Set63ba7c | util | C3 | 1 | 1 | 0x0043dfd0 |
| 0x0040e340 | GetLiveCarCount | util | C4 | 0 | 2 | (root) |
| 0x0040e360 | RaceMode::Set | util | C3 | 1 | 1 | 0x0043dfd0 |
| 0x0040e370 | FUN_0040e370 | util | C3 | 0 | 7 | (root) |
| 0x0040e4b0 | Race::GetSoleFinishedPlayer | util | C2 | 2 | 1 | 0x00448220 |
| 0x0040e590 | FUN_0040e590 | util | C2 | 6 | 2 | 0x0046baa0 |
| 0x004102f0 | FUN_004102f0 | util | C2 | 6 | 3 | 0x0046baa0 |
| 0x004103a0 | TimeTrial::LapFinishProcessor | util | C2 | 7 | 5 | 0x0041da90 |
| 0x00413f90 | TimerGetBasePtr | util | C4 | 1 | 2 | 0x00430670 |
| 0x0041eda0 | SlotBitSet | util | C3 | 6 | 2 | 0x004704c0 |
| 0x0041ede0 | Flag0041ede0 | util | C3 | 7 | 2 | 0x0040e590 |
| 0x0041ee50 | Flag41ee50 | util | C3 | 7 | 2 | 0x0040e590 |
| 0x0041ef60 | Player::WriteFieldZero | util | C3 | 2 | 2 | 0x00448220 |
| 0x0041ef80 | FlagToggle63dc74 | util | C3 | 7 | 2 | 0x0040e590 |
| 0x0041efc0 | Car::GetLapProgress | util | C3 | 6 | 1 | 0x00463640 |
| 0x0041f000 | SlotDataCopy | util | C3 | 7 | 1 | 0x0040e590 |
| 0x0041f320 | Car::GetState | util | C3 | 7 | 1 | 0x0040cf80 |
| 0x004219f0 | FUN_004219f0 | util | C2 | 3 | 2 | 0x00422ba0 |
| 0x00421d20 | FUN_00421d20 | util | C2 | 5 | 1 | 0x00476880 |
| 0x00424eb0 | FUN_00424eb0 | util | C2 | 4 | 1 | 0x0040dc90 |
| 0x004292c0 | sub_004292c0 | util | C3 | 1 | 2 | 0x00409790 |
| 0x004292d0 | sub_004292d0 | util | C2 | 1 | 2 | 0x00409790 |
| 0x004295a0 | HudDualLabelRender | util | C4 | 2 | 2 | 0x00429e10 |
| 0x00429aa0 | GameStateSlotsFill | util | C4 | 1 | 1 | 0x0043dfd0 |
| 0x00429bd0 | FUN_00429bd0 | util | C2 | 2 | 2 | 0x00429e10 |
| 0x0042b8d0 | StatePhaseIsIdle | util | C4 | 6 | 1 | 0x004647f0 |
| 0x0042b8f0 | StatePhaseIsFinal | util | C3 | 5 | 1 | 0x00466b50 |
| 0x0042b900 | Global67eca4Get | util | C3 | 5 | 1 | 0x00466b50 |
| 0x0042b950 | Set7f1a0c_1000 | util | C3 | 1 | 1 | 0x0043dfd0 |
| 0x0042c1a0 | StateAdvance2to3 | util | C3 | 2 | 2 | 0x0040acd0 |
| 0x0042c7c0 | FUN_0042c7c0 | util | C2 | 3 | 1 | 0x00429660 |
| 0x0042f500 | GetDat0067ea64 | util | C4 | 0 | 5 | (root) |
| 0x0042f6a0 | GetRaceSubMode | util | C3 | 0 | 9 | (root) |
| 0x00430790 | GetDat0067f17c | util | C3 | 6 | 3 | 0x00462520 |
| 0x00431b30 | Float67eaa8Get | util | C3 | 1 | 1 | 0x0043dfd0 |
| 0x00431b50 | FUN_00431b50 | util | C2 | 5 | 2 | 0x00466b50 |
| 0x00431b60 | FUN_00431b60 | util | C2 | 5 | 2 | 0x00466b50 |
| 0x00431d70 | Course::GetLeaderIndex | util | C3 | 1 | 2 | 0x0040d110 |
| 0x00432290 | Trigger432290 | util | C3 | 5 | 2 | 0x00466b50 |
| 0x0043c000 | TimerSlotTickDispatcher | util | C4 | 1 | 1 | 0x0043dfd0 |
| 0x00441760 | Camera::Apply | util | C2 | 0 | 0 | (root) |
| 0x004417e0 | FUN_004417e0 | util | C2 | 6 | 2 | 0x0045d7a0 |
| 0x00441820 | CameraPath::SamplePoint | util | C2 | 0 | 0 | (root) |
| 0x00441990 | FUN_00441990 | util | C2 | 2 | 2 | 0x00448220 |
| 0x00441c80 | FUN_00441c80 | util | C2 | 9 | 2 | 0x00445aa0 |
| 0x00441d40 | FUN_00441d40 | util | C2 | 10 | 1 | 0x00441c80 |
| 0x00442440 | FUN_00442440 | util | C2 | 4 | 1 | 0x0040dc90 |
| 0x00442a60 | Spectator::ComputeDistances | util | C2 | 0 | 0 | (root) |
| 0x00442e00 | FUN_00442e00 | util | C2 | 2 | 2 | 0x00448220 |
| 0x004430a0 | Util897fe0Set | util | C3 | 7 | 3 | 0x004102f0 |
| 0x004430b0 | Util897fe0Get | util | C3 | 7 | 2 | 0x004102f0 |
| 0x00445aa0 | FUN_00445aa0 | util | C2 | 8 | 2 | 0x004430a0 |
| 0x004464c0 | CameraEntry::DispatchAll | util | C3 | 1 | 3 | 0x00448700 |
| 0x00446520 | FUN_00446520 | util | C2 | 0 | 2 | (root) |
| 0x0044e0a0 | FUN_0044e0a0 | util | C2 | 3 | 2 | 0x00422ba0 |
| 0x0045bba0 | FUN_0045bba0 | util | C2 | 3 | 2 | 0x004568d0 |
| 0x0045bfe0 | Bezier::GetLocate | util | C3 | 4 | 4 | 0x0045bba0 |
| 0x0045c350 | Bezier::Interpolate | util | C2 | 4 | 3 | 0x0045bba0 |
| 0x0045c480 | FUN_0045c480 | util | C3 | 5 | 1 | 0x00466b50 |
| 0x0045c810 | FUN_0045c810 | util | C3 | 6 | 1 | 0x0045d3a0 |
| 0x0045d1e0 | FUN_0045d1e0 | util | C2 | 6 | 1 | 0x0045d3a0 |
| 0x0045d330 | FUN_0045d330 | util | C2 | 6 | 1 | 0x0045d3a0 |
| 0x0045d3a0 | FUN_0045d3a0 | util | C2 | 5 | 1 | 0x00466b50 |
| 0x0045d3f0 | FUN_0045d3f0 | util | C2 | 4 | 2 | 0x00429290 |
| 0x0045d430 | FUN_0045d430 | util | C2 | 4 | 1 | 0x00429290 |
| 0x0045d7a0 | FUN_0045d7a0 | util | C2 | 5 | 1 | 0x00466b50 |
| 0x0045db50 | FUN_0045db50 | util | C2 | 5 | 1 | 0x00466b50 |
| 0x0045dbe0 | FUN_0045dbe0 | util | C2 | 5 | 1 | 0x00466b50 |
| 0x0045df70 | FloatStep45df70 | util | C3 | 5 | 2 | 0x00466b50 |
| 0x0045dfc0 | FUN_0045dfc0 | util | C2 | 5 | 1 | 0x00466b50 |
| 0x004657b0 | FUN_004657b0 | util | C2 | 6 | 1 | 0x0045d3a0 |
| 0x00466b50 | FUN_00466b50 | util | C2 | 4 | 1 | 0x00429290 |
| 0x0046b4f0 | FUN_0046b4f0 | util | C3 | 2 | 2 | 0x00448220 |
| 0x0046baa0 | FUN_0046baa0 | util | C3 | 5 | 4 | 0x00424eb0 |
| 0x0046cb30 | Player::GetOffset3D | util | C2 | 0 | 1 | (root) |
| 0x00472640 | Set86ecc8 | util | C3 | 1 | 1 | 0x0043dfd0 |
| 0x00472650 | FUN_00472650 | util | C2 | 1 | 19 | 0x00415220 |
| 0x00472690 | FUN_00472690 | util | C3 | 4 | 2 | 0x00458d00 |
| 0x00472820 | FUN_00472820 | util | C3 | 6 | 2 | 0x0045d7a0 |
| 0x004728a0 | FUN_004728a0 | util | C2 | 6 | 1 | 0x0045d7a0 |
| 0x004764f0 | FUN_004764f0 | util | C2 | 6 | 1 | 0x00476710 |
| 0x00476880 | FUN_00476880 | util | C2 | 4 | 1 | 0x0045bba0 |
| 0x00477b40 | FUN_00477b40 | util | C3 | 5 | 1 | 0x00477e40 |
| 0x004785e0 | FUN_004785e0 | util | C2 | 7 | 1 | 0x00478660 |
| 0x00484c90 | FUN_00484c90 | util | C3 | 7 | 1 | 0x0040cfd0 |
| 0x0048f680 | StridedClear76a100 | util | C3 | 7 | 1 | 0x0040e590 |
| 0x0048f6b0 | StridedClear766a00 | util | C3 | 8 | 1 | 0x0048f740 |
| 0x0048f6e0 | StridedClear770718 | util | C3 | 8 | 1 | 0x0048f740 |
| 0x0048f710 | StridedClear769f50 | util | C3 | 8 | 1 | 0x0048f740 |
| 0x0048f740 | FUN_0048f740 | util | C2 | 7 | 1 | 0x0040e590 |
| 0x00492d10 | Global771968Get | util | C3 | 3 | 8 | 0x004661a0 |
| 0x004938e0 | FUN_004938e0 | util | C3 | 2 | 3 | 0x00494480 |
| 0x00493bc0 | FUN_00493bc0 | util | C2 | 4 | 1 | 0x00494c80 |
| 0x00493c00 | FUN_00493c00 | util | C2 | 5 | 1 | 0x00494ac0 |
| 0x00493f00 | FUN_00493f00 | util | C2 | 4 | 1 | 0x00494c80 |
| 0x004944b0 | FUN_004944b0 | util | C3 | 4 | 1 | 0x00494c80 |
| 0x004944c0 | FUN_004944c0 | util | C2 | 2 | 2 | 0x00494a80 |
| 0x00494ac0 | FUN_00494ac0 | util | C2 | 4 | 2 | 0x00494c80 |
| 0x00494c80 | FUN_00494c80 | util | C2 | 3 | 2 | 0x00402750 |
| 0x0049cfb0 | FUN_0049cfb0 | util | C2 | 6 | 1 | 0x0049ec10 |
| 0x0049ec10 | FUN_0049ec10 | util | C2 | 5 | 2 | 0x00494ac0 |
| 0x004a1790 | FUN_004a1790 | util | C3 | 4 | 1 | 0x00494c80 |
| 0x004a3384 | CRT::acos | util | C2 | 0 | 7 | (root) |
| 0x004b4550 | FUN_004b4550 | util | C2 | 7 | 2 | 0x00478660 |
| 0x004b4cd0 | Bezier::QueryWrapper | util | C2 | 5 | 1 | 0x0045ae80 |
| 0x004b6520 | ZeroFillWrapper | util | C3 | 2 | 18 | 0x0045ba10 |
| 0x004b6570 | PizOpen | util | C2 | 2 | 1 | 0x00495280 |
| 0x004b6940 | PizOpenAndParse | util | C2 | 0 | 2 | (root) |
| 0x004c1c10 | Camera::SetProjection | util | C2 | 2 | 3 | 0x00448220 |
| 0x004c3ac0 | Vec3Magnitude | util | C4 | 0 | 15 | (root) |
| 0x004c3b30 | FastSqrt | util | C4 | 2 | 5 | 0x00414300 |
| 0x004c4dc0 | FUN_004c4dc0 | util | C2 | 3 | 3 | 0x004568d0 |
| 0x004d8560 | FUN_004d8560 | util | C3 | 2 | 5 | 0x0040acd0 |
| 0x00550be0 | FUN_00550be0 | util | C2 | 5 | 2 | 0x0045d3f0 |
| 0x0055dec0 | FUN_0055dec0 | util | C3 | 1 | 2 | 0x00409790 |
| 0x005ab040 | FUN_005ab040 | util | C2 | 7 | 1 | 0x005a6ea0 |
| 0x005c9d00 | GetRaceEndTrigger | util | C2 | 2 | 2 | 0x0040acd0 |
| 0x004039c0 | FUN_004039c0 | vehicle | C2 | 3 | 1 | 0x0045bae0 |
| 0x00405890 | Pred405890 | vehicle | C3 | 0 | 3 | (root) |
| 0x0040e180 | MostSeparatedPair | vehicle | C4 | 0 | 4 | (root) |
| 0x00410d10 | FUN_00410d10 | vehicle | C2 | 0 | 4 | (root) |
| 0x00411170 | TimeTrialRecordPlayback | vehicle | C3 | 0 | 0 | (root) |
| 0x004114e0 | ReplayCleanup | vehicle | C3 | 7 | 1 | 0x0040cfd0 |
| 0x004115c0 | ReplayGetCurrentTime | vehicle | C3 | 6 | 1 | 0x00463590 |
| 0x00411f30 | FUN_00411f30 | vehicle | C2 | 3 | 1 | 0x0045bae0 |
| 0x00413c70 | FUN_00413c70 | vehicle | C3 | 2 | 1 | 0x00419760 |
| 0x00417730 | VehicleRaceAngleGet | vehicle | C3 | 0 | 5 | (root) |
| 0x00418a30 | FUN_00418a30 | vehicle | C3 | 2 | 1 | 0x00419760 |
| 0x00418de0 | FUN_00418de0 | vehicle | C2 | 2 | 1 | 0x00419760 |
| 0x00419760 | FUN_00419760 | vehicle | C2 | 1 | 1 | 0x00422fd0 |
| 0x00420de0 | FUN_00420de0 | vehicle | C3 | 2 | 2 | 0x004215c0 |
| 0x00426cc0 | VehicleTable4cPtrGet | vehicle | C3 | 0 | 3 | (root) |
| 0x0042a6b0 | FUN_0042a6b0 | vehicle | C2 | 3 | 7 | 0x0045bae0 |
| 0x0042bf30 | Post0042bf30 | vehicle | C3 | 1 | 1 | 0x0043dfd0 |
| 0x00448700 | FUN_00448700 | vehicle | C2 | 0 | 2 | (root) |
| 0x00453f30 | Fill6870b4 | vehicle | C3 | 3 | 1 | 0x0045bae0 |
| 0x004587a0 | FUN_004587a0 | vehicle | C2 | 3 | 1 | 0x0045bae0 |
| 0x00459290 | FUN_00459290 | vehicle | C2 | 2 | 2 | 0x0045ba10 |
| 0x0045ba10 | FUN_0045ba10 | vehicle | C2 | 1 | 3 | 0x0045bed0 |
| 0x0045bae0 | FUN_0045bae0 | vehicle | C2 | 2 | 5 | 0x0045ba10 |
| 0x00467650 | VehicleWheelDrivetrainUpdate | vehicle | C4 | 0 | 0 | (root) |
| 0x0046b1c0 | FUN_0046b1c0 | vehicle | C2 | 7 | 1 | 0x0040e590 |
| 0x0046b540 | VehicleSpawnInit | vehicle | C4 | 7 | 1 | 0x0040e590 |
| 0x0046c570 | VehicleDampVec3 | vehicle | C3 | 5 | 1 | 0x00424eb0 |
| 0x0046c6d0 | VehicleEntitySlotRead | vehicle | C3 | 7 | 1 | 0x0040e590 |
| 0x0046c7b0 | VehicleSlotGetter | vehicle | C3 | 0 | 12 | (root) |
| 0x0046c7d0 | FUN_0046c7d0 | vehicle | C2 | 5 | 1 | 0x00424eb0 |
| 0x0046cbb0 | CarStatePairGet | vehicle | C3 | 0 | 11 | (root) |
| 0x0046cbe0 | VehicleCarStateSet | vehicle | C3 | 3 | 2 | 0x00422ba0 |
| 0x0046d700 | VehicleVec3At9C8Get | vehicle | C3 | 0 | 0 | (root) |
| 0x0046f6c0 | VehicleWheelContactSolver | vehicle | C2 | 6 | 1 | 0x004704c0 |
| 0x004704c0 | FUN_004704c0 | vehicle | C2 | 5 | 2 | 0x00424eb0 |
| 0x00474d60 | FUN_00474d60 | vehicle | C2 | 3 | 3 | 0x00459290 |
| 0x00476c10 | Batch476c10 | vehicle | C3 | 3 | 1 | 0x00459290 |
| 0x00476cb0 | Batch476cb0 | vehicle | C3 | 3 | 7 | 0x00459290 |
| 0x004770c0 | FUN_004770c0 | vehicle | C2 | 2 | 12 | 0x0045ba10 |
| 0x004781b0 | FUN_004781b0 | vehicle | C2 | 4 | 2 | 0x00411f30 |
| 0x0047a020 | FUN_0047a020 | vehicle | C2 | 4 | 3 | 0x0047b9b0 |
| 0x0047ea60 | FUN_0047ea60 | vehicle | C2 | 6 | 1 | 0x004704c0 |
| 0x00480720 | VehicleRespawnPlace | vehicle | C2 | 3 | 2 | 0x00422ba0 |
| 0x00480d60 | FUN_00480d60 | vehicle | C2 | 3 | 1 | 0x00422ba0 |
| 0x00481750 | FUN_00481750 | vehicle | C2 | 3 | 1 | 0x004039f0 |
| 0x00481780 | FUN_00481780 | vehicle | C2 | 3 | 1 | 0x0046bf50 |
| 0x00482900 | FUN_00482900 | vehicle | C2 | 4 | 1 | 0x004113b0 |
| 0x004840f0 | FUN_004840f0 | vehicle | C3 | 4 | 1 | 0x004841d0 |
| 0x004922e0 | CarEventTrigger | vehicle | C3 | 0 | 2 | (root) |
| 0x004b3bf0 | FUN_004b3bf0 | vehicle | C2 | 3 | 3 | 0x00459290 |
| 0x004b3e40 | FUN_004b3e40 | vehicle | C2 | 4 | 3 | 0x00411f30 |
| 0x004b5190 | FUN_004b5190 | vehicle | C2 | 5 | 4 | 0x004b51d0 |
| 0x004b5320 | FUN_004b5320 | vehicle | C2 | 4 | 2 | 0x00411f30 |
| 0x004b5580 | FUN_004b5580 | vehicle | C2 | 4 | 2 | 0x00411f30 |
| 0x004942b0 | FUN_004942b0 | video | C2 | 2 | 2 | 0x00494480 |
| 0x00494320 | FUN_00494320 | video | C2 | 2 | 2 | 0x00494460 |
| 0x00494480 | FUN_00494480 | video | C2 | 1 | 3 | 0x0043dfd0 |
| 0x0049dd60 | FUN_0049dd60 | video | C2 | 6 | 1 | 0x0049ec10 |
| 0x004847d0 | FUN_004847d0 | world-objects | C2 | 3 | 1 | 0x00484cf0 |
| 0x00484ac0 | FUN_00484ac0 | world-objects | C2 | 3 | 1 | 0x00484cf0 |
| 0x00484cf0 | FUN_00484cf0 | world-objects | C2 | 2 | 3 | 0x00484c70 |

## 3. Ranked gap list — C2 the slice needs

Non-library closure members at confidence C2, ranked by static in-degree (DESC), tie-break depth (ASC) then RVA (ASC). `stub?` = RVA appears as an "RVA called" row in STUBS.md (direct call-original demand). In-degree is a STATIC proxy, not measured call frequency. 730 rows.

| rank | RVA | name | subsystem | in-degree (static proxy) | depth | first caller | stub? | hooks.csv file |
|---|---|---|---|---|---|---|---|---|
| 1 | 0x004a2c48 | FUN_004a2c48 | frontend | 31 | 0 | (root) | STUB | re/analysis/promote_c1_high_ab3/0x004a2c48.md |
| 2 | 0x00472650 | FUN_00472650 | util | 19 | 1 | 0x00415220 | STUB | re/analysis/ai_update_d2/0x00472650.md |
| 3 | 0x00416250 | FUN_00416250 | ai | 14 | 0 | (root) | STUB | re/analysis/bucket_ai_00415d00_00452ea0/0x00416250.md |
| 4 | 0x004768c0 | FUN_004768c0 | render | 14 | 5 | 0x004593b0 | STUB | re/analysis/render_c1_to_c2_s6/FUN_004768c0.md |
| 5 | 0x004671a0 | sub_004671a0 | render | 12 | 0 | (root) | STUB | re/analysis/promote_c2_piz_loader/004671a0.md |
| 6 | 0x00402750 | sub_00402750 | boot | 12 | 2 | 0x00495280 |  | re/analysis/promote_c2_boot_lowrva/0x00402750.md |
| 7 | 0x004770c0 | FUN_004770c0 | vehicle | 12 | 2 | 0x0045ba10 | STUB | re/analysis/bucket_vehicle_00453f30_00482030/004770c0.md |
| 8 | 0x005a6dc0 | FUN_005a6dc0 | audio | 12 | 3 | 0x0045e0f0 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a6dc0.md |
| 9 | 0x0045e0f0 | FUN_0045e0f0 | audio | 11 | 2 | 0x004645c0 |  | re/analysis/bucket_audio_0042f760_00465b20/0045e0f0.md |
| 10 | 0x00417da0 | FUN_00417da0 | ai | 10 | 0 | (root) |  | re/analysis/ai_update/0x00417da0.md |
| 11 | 0x004c0ed0 | FUN_004c0ed0 | render | 9 | 2 | 0x0045a950 |  | re/analysis/render_3_c1_to_c2_s6/FUN_004c0ed0.md |
| 12 | 0x0042a5d0 | FUN_0042a5d0 | render | 9 | 4 | 0x0041a1e0 | STUB | re/analysis/track_loader_d3/0042a5d0.md |
| 13 | 0x005a6d60 | FUN_005a6d60 | audio | 9 | 4 | 0x005a6dc0 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a6d60.md |
| 14 | 0x00414c30 | FUN_00414c30 | ai | 8 | 0 | (root) |  | re/analysis/bucket_ai_00407a40_00415880/0x00414c30.md |
| 15 | 0x00415220 | FUN_00415220 | ai | 8 | 0 | (root) |  | re/analysis/bucket_ai_00407a40_00415880/0x00415220.md |
| 16 | 0x00416a30 | FUN_00416a30 | ai | 8 | 0 | (root) | STUB | re/analysis/bucket_ai_00415d00_00452ea0/0x00416a30.md |
| 17 | 0x004cc230 | FUN_004cc230 | frontend | 8 | 1 | 0x004235b0 |  | re/analysis/promote_c2_rw_engine_init/004cc230.md |
| 18 | 0x005aa560 | FUN_005aa560 | audio | 8 | 2 | 0x005a7f70 | STUB | re/analysis/audio_dsound_d5/0x005aa560.md;re/analysis/promote_c2_audio_dsound/0x005aa560.md |
| 19 | 0x004c5930 | sub_004c5930 | boot | 8 | 3 | 0x0041e140 |  | re/analysis/skeleton_prep_high_leverage/004c5930.md |
| 20 | 0x00426e10 | FUN_00426e10 | render | 8 | 5 | 0x0047a020 |  | re/analysis/render_promote_c2_track_loader/0x00426e10.md |
| 21 | 0x004c51a0 | RwMatrixTranslate | render | 8 | 5 | 0x00442440 | STUB | re/analysis/font_text_d3/font_text_d3-20260506.md |
| 22 | 0x00414f00 | FUN_00414f00 | ai | 7 | 0 | (root) |  | re/analysis/bucket_ai_00407a40_00415880/0x00414f00.md |
| 23 | 0x004a3384 | CRT::acos | util | 7 | 0 | (root) |  | re/analysis/profile_career_d4/REPORT.md |
| 24 | 0x004952f0 | FUN_004952f0 | render | 7 | 1 | 0x004235b0 | STUB | re/analysis/promote_c2_piz_loader/004952f0.md |
| 25 | 0x004a2b60 | FUN_004a2b60 | frontend | 7 | 1 | 0x004625b0 |  | re/analysis/promote_c1_high_ab3/0x004a2b60.md |
| 26 | 0x0042a6b0 | FUN_0042a6b0 | vehicle | 7 | 3 | 0x0045bae0 | STUB | re/analysis/promote_c2_vehicle_lowrva/0x0042a6b0.md |
| 27 | 0x00415880 | FUN_00415880 | ai | 6 | 0 | (root) |  | re/analysis/bucket_ai_00407a40_00415880/0x00415880.md |
| 28 | 0x004cc5e0 | FUN_004cc5e0 | render | 6 | 0 | (root) |  | re/analysis/texture_loader_d2/0x004cc5e0.md |
| 29 | 0x0046cc10 | FUN_0046cc10 | ai | 6 | 1 | 0x00415880 |  | re/analysis/bucket_ai_00452eb0_004c3df0/0046cc10.md |
| 30 | 0x0046bf50 | FUN_0046bf50 | gameplay | 6 | 2 | 0x0045a950 |  | re/analysis/bucket_gameplay_0045dff0_0046dd90/0x0046bf50.md |
| 31 | 0x00407b00 | FUN_00407b00 | gameplay | 6 | 3 | 0x00409330 |  | re/analysis/bucket_gameplay_00407640_0040ad40/00407b00.md |
| 32 | 0x00495350 | IntroSplashOrchestrator | frontend | 6 | 3 | 0x004c1a00 |  | mashedmod/src/mashed_re/Frontend/IntroSplash.cpp |
| 33 | 0x005afcf0 | FUN_005afcf0 | audio | 6 | 5 | 0x005af8f0 |  | re/analysis/bucket_audio_005af070_005b2190/0x005afcf0.md |
| 34 | 0x005b10a0 | FUN_005b10a0 | audio | 6 | 5 | 0x005af600 |  | re/analysis/bucket_audio_005af070_005b2190/0x005b10a0.md |
| 35 | 0x00418560 | FUN_00418560 | ai | 5 | 0 | (root) |  | re/analysis/ai_update/0x00418560.md |
| 36 | 0x00443440 | FUN_00443440 | ai | 5 | 0 | (root) |  | re/analysis/bucket_ai_00415d00_00452ea0/0x00443440.md |
| 37 | 0x00495280 | FUN_00495280 | render | 5 | 1 | 0x004235b0 | STUB | re/analysis/promote_c2_piz_loader/00495280.md |
| 38 | 0x00459000 | FUN_00459000 | gameplay | 5 | 2 | 0x00458f20 |  | re/analysis/bucket_gameplay_00458a40_0045ac40/0x00459000.md |
| 39 | 0x0045bae0 | FUN_0045bae0 | vehicle | 5 | 2 | 0x0045ba10 |  | re/analysis/skeleton_prep_render/0045bae0.md |
| 40 | 0x004c7650 | FUN_004c7650 | render | 5 | 2 | 0x00494460 | STUB | re/analysis/render_4_c1_to_c2_s4/FUN_004c7650.md |
| 41 | 0x0041b450 | FUN_0041b450 | boot | 5 | 3 | 0x00402750 |  | re/analysis/promote_c2_boot_lowrva/0x0041b450.md |
| 42 | 0x0041cb10 | FUN_0041cb10 | boot | 5 | 3 | 0x00402750 |  | re/analysis/promote_c2_boot_lowrva/0x0041cb10.md |
| 43 | 0x004b3d80 | FUN_004b3d80 | render | 5 | 3 | 0x00402750 | STUB | re/analysis/promote_c2_txd_loader/004b3d80.md |
| 44 | 0x004c1480 | FUN_004c1480 | render | 5 | 3 | 0x004568d0 |  | re/analysis/skeleton_prep_high_leverage/004c1480.md |
| 45 | 0x005a66d0 | FUN_005a66d0 | audio | 5 | 3 | 0x004661a0 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a66d0.md |
| 46 | 0x0040bd00 | FUN_0040bd00 | boot | 5 | 4 | 0x0040bbb0 |  | re/analysis/promote_c2_boot_lowrva/0x0040bd00.md |
| 47 | 0x0042a530 | FUN_0042a530 | render | 5 | 4 | 0x0042a6b0 | STUB | re/analysis/texture_loader/0x0042a530.md |
| 48 | 0x0045daf0 | FUN_0045daf0 | audio | 5 | 4 | 0x00465c10 |  | re/analysis/bucket_gameplay_0045ae80_0045dd50/0045daf0.md |
| 49 | 0x004987b0 | FUN_004987b0 | input | 5 | 4 | 0x004113b0 | STUB | re/analysis/promote_c2_dinput_init/004987b0.md |
| 50 | 0x004c5a60 | FUN_004c5a60 | render | 5 | 5 | 0x00556d70 | STUB | re/analysis/boot_subsystem_d3/0x004c5a60.md |
| 51 | 0x005aa060 | FUN_005aa060 | audio | 5 | 5 | 0x004627f0 |  | re/analysis/bucket_audio_005a7b60_005ab620/005aa060.md |
| 52 | 0x005af180 | FUN_005af180 | audio | 5 | 5 | 0x005af260 |  | re/analysis/bucket_audio_005af070_005b2190/0x005af180.md |
| 53 | 0x005b1110 | FUN_005b1110 | audio | 5 | 5 | 0x005af8f0 |  | re/analysis/bucket_audio_005af070_005b2190/0x005b1110.md |
| 54 | 0x004522d0 | FUN_004522d0 | audio | 5 | 6 | 0x005a71f0 | STUB | re/analysis/audio_rws_loader/004522d0.md |
| 55 | 0x004770a0 | FUN_004770a0 | render | 5 | 6 | 0x00489890 |  | re/analysis/render_c1_to_c2_s6/FUN_004770a0.md |
| 56 | 0x005a6030 | FUN_005a6030 | audio | 5 | 6 | 0x005a60b0 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a6030.md |
| 57 | 0x005a6df0 | FUN_005a6df0 | audio | 5 | 6 | 0x005a7460 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a6df0.md |
| 58 | 0x004103a0 | TimeTrial::LapFinishProcessor | util | 5 | 7 | 0x0041da90 |  | re/analysis/leaderboard_d2/0x004103a0.md |
| 59 | 0x00410d10 | FUN_00410d10 | vehicle | 4 | 0 | (root) |  | re/analysis/promote_c2_vehicle_lowrva/0x00410d10.md |
| 60 | 0x00414a70 | FUN_00414a70 | ai | 4 | 0 | (root) |  | re/analysis/bucket_ai_00407a40_00415880/0x00414a70.md |
| 61 | 0x00417640 | FUN_00417640 | ai | 4 | 0 | (root) | STUB | re/analysis/ai_update/0x00417640.md |
| 62 | 0x0045bac0 | PowerupSlotDeactivate | powerups | 4 | 1 | 0x0045bed0 |  | re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x0045bac0.md |
| 63 | 0x004a504f | FUN_004a504f | render | 4 | 1 | 0x004a2cbd |  | re/analysis/rw_engine_init_d2/004a504f.md |
| 64 | 0x00408b00 | FUN_00408b00 | util | 4 | 2 | 0x00448220 |  | re/analysis/game_state_d5_cont2/0x00408b00.md |
| 65 | 0x004098d0 | FUN_004098d0 | gameplay | 4 | 2 | 0x0040acd0 |  | re/analysis/bucket_gameplay_00407640_0040ad40/004098d0.md |
| 66 | 0x00493fd0 | sub_00493fd0 | frontend | 4 | 2 | 0x004c75e0 | STUB | re/analysis/intro_splash/0x00493fd0.md |
| 67 | 0x0040bbb0 | FUN_0040bbb0 | boot | 4 | 3 | 0x00402750 |  | re/analysis/promote_c2_boot_lowrva/0x0040bbb0.md |
| 68 | 0x0041bec0 | FUN_0041bec0 | boot | 4 | 3 | 0x00402750 |  | re/analysis/promote_c2_boot_lowrva/0x0041bec0.md |
| 69 | 0x00429290 | FUN_00429290 | frontend | 4 | 3 | 0x00402750 |  | re/analysis/skeleton_prep_game_state/00429290.md |
| 70 | 0x00476df0 | FUN_00476df0 | render | 4 | 3 | 0x004568d0 | STUB | re/analysis/promote_c2_render_frame/0x00476df0.md |
| 71 | 0x0047b9b0 | FUN_0047b9b0 | physics | 4 | 3 | 0x0045bae0 |  | re/analysis/bucket_physics_smplfzx_00478cb0_0057c4b0/0x0047b9b0.md |
| 72 | 0x004c3d90 | FUN_004c3d90 | render | 4 | 3 | 0x004568d0 |  | re/analysis/track_loader_d4/0x004c3d90.md |
| 73 | 0x0041e950 | FUN_0041e950 | gameplay | 4 | 4 | 0x0041e8d0 |  | re/analysis/bucket_gameplay_0041e140_0041efe0/0041e950.md |
| 74 | 0x004b51d0 | FUN_004b51d0 | render | 4 | 4 | 0x0041a1e0 |  | re/analysis/bucket_004b4a80/0x004b51d0.md |
| 75 | 0x004ce2d0 | FUN_004ce2d0 | render | 4 | 4 | 0x004283a0 |  | re/analysis/bucket_004c4270/0x004ce2d0.md |
| 76 | 0x004e6680 | RpClumpRender | render | 4 | 4 | 0x004013f0 |  | re/analysis/frontend_gate_unblock_u/0x004e6680.md |
| 77 | 0x005af8f0 | FUN_005af8f0 | audio | 4 | 4 | 0x005af510 |  | re/analysis/bucket_audio_005af070_005b2190/0x005af8f0.md |
| 78 | 0x00402a40 | sub_00402a40 | boot | 4 | 5 | 0x0040bd00 |  | re/analysis/promote_c2_boot_lowrva/0x00402a40.md |
| 79 | 0x00474db0 | FUN_00474db0 | render | 4 | 5 | 0x00475a00 |  | re/analysis/bucket_00474d80/0x00474db0.md |
| 80 | 0x004b5190 | FUN_004b5190 | vehicle | 4 | 5 | 0x004b51d0 |  | re/analysis/bucket_vehicle_004922e0_0057c500/0x004b5190.md |
| 81 | 0x004c0510 | FUN_004c0510 | input | 4 | 5 | 0x0047b860 | STUB | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004c0510.md |
| 82 | 0x005a5f00 | FUN_005a5f00 | audio | 4 | 5 | 0x004627f0 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a5f00.md |
| 83 | 0x005a7460 | FUN_005a7460 | audio | 4 | 5 | 0x005a7520 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a7460.md |
| 84 | 0x005af070 | FUN_005af070 | audio | 4 | 5 | 0x005af740 |  | re/analysis/bucket_audio_005af070_005b2190/0x005af070.md |
| 85 | 0x0040cf80 | FUN_0040cf80 | boot | 4 | 6 | 0x00402a40 |  | re/analysis/promote_c2_boot_lowrva/0x0040cf80.md |
| 86 | 0x0047c0f0 | FUN_0047c0f0 | track | 4 | 6 | 0x00426e10 |  | re/analysis/bucket_track_00401630_0047c0f0/0047c0f0.md |
| 87 | 0x00489500 | FUN_00489500 | gameplay | 4 | 6 | 0x004893b0 |  | re/analysis/bucket_gameplay_0047f450_004e4440/0x00489500.md |
| 88 | 0x005ad080 | FUN_005ad080 | audio | 4 | 6 | 0x005a6110 |  | re/analysis/bucket_audio_005ab710_005af040/0x005ad080.md |
| 89 | 0x005aea00 | FUN_005aea00 | audio | 4 | 6 | 0x005a71f0 | STUB | re/analysis/audio_rws_loader/005aea00.md |
| 90 | 0x005b0a90 | FUN_005b0a90 | audio | 4 | 6 | 0x005afcf0 |  | re/analysis/bucket_audio_005af070_005b2190/0x005b0a90.md |
| 91 | 0x00466a50 | FUN_00466a50 | audio | 4 | 7 | 0x00467010 |  | re/analysis/bucket_audio_00465c10_005a7b50/00466a50.md |
| 92 | 0x00414570 | FUN_00414570 | ai | 3 | 0 | (root) |  | re/analysis/bucket_ai_00407a40_00415880/0x00414570.md |
| 93 | 0x004161e0 | FUN_004161e0 | ai | 3 | 0 | (root) |  | re/analysis/bucket_ai_00415d00_00452ea0/0x004161e0.md |
| 94 | 0x00429620 | FUN_00429620 | hud | 3 | 0 | (root) |  | re/analysis/localization_d2/0x00429620.md |
| 95 | 0x00414300 | FUN_00414300 | ai | 3 | 1 | 0x00414a70 |  | re/analysis/bucket_ai_00407a40_00415880/0x00414300.md |
| 96 | 0x00417370 | FUN_00417370 | gameplay | 3 | 1 | 0x00416230 |  | re/analysis/bucket_gameplay_00412100_00418e50/0x00417370.md |
| 97 | 0x004252c0 | FUN_004252c0 | ai | 3 | 1 | 0x00407a40 |  | re/analysis/bucket_ai_00415d00_00452ea0/0x004252c0.md |
| 98 | 0x00429e10 | FUN_00429e10 | render | 3 | 1 | 0x0040e4a0 |  | re/analysis/render_c1_to_c2_s1/FUN_00429e10.md |
| 99 | 0x00448220 | Frontend::PostRaceResultCamera | frontend | 3 | 1 | 0x0040e4a0 |  | re/analysis/profile_career_d2/FUN_00448220.md |
| 100 | 0x0045ba10 | FUN_0045ba10 | vehicle | 3 | 1 | 0x0045bed0 |  | re/analysis/bucket_vehicle_00453f30_00482030/0045ba10.md |
| 101 | 0x00494460 | sub_00494460 | frontend | 3 | 1 | 0x0043dfd0 | STUB | re/analysis/skeleton_prep_boot_winmain_a/00494460.md |
| 102 | 0x00494480 | FUN_00494480 | video | 3 | 1 | 0x0043dfd0 | STUB | re/analysis/bucket_sky_worldobj_dbg_video_hud_00484280_00555910/00494480.md |
| 103 | 0x00408610 | FUN_00408610 | gameplay | 3 | 2 | 0x00407a20 |  | re/analysis/bucket_gameplay_00407640_0040ad40/00408610.md |
| 104 | 0x00409950 | FUN_00409950 | gameplay | 3 | 2 | 0x0040acd0 |  | re/analysis/bucket_gameplay_00407640_0040ad40/00409950.md |
| 105 | 0x00422ba0 | FUN_00422ba0 | ai | 3 | 2 | 0x004252c0 |  | re/analysis/bucket_ai_00415d00_00452ea0/0x00422ba0.md |
| 106 | 0x00427e00 | FUN_00427e00 | frontend | 3 | 2 | 0x00429e10 |  | re/analysis/promote_c1_low_ab1/0x00427e00.md |
| 107 | 0x00429660 | FUN_00429660 | hud | 3 | 2 | 0x00429e10 |  | re/analysis/localization_d2/0x00429660.md |
| 108 | 0x00484cf0 | FUN_00484cf0 | world-objects | 3 | 2 | 0x00484c70 |  | re/analysis/bucket_sky_worldobj_dbg_video_hud_00484280_00555910/00484cf0.md |
| 109 | 0x004c1c10 | Camera::SetProjection | util | 3 | 2 | 0x00448220 |  | re/analysis/profile_career_d4/REPORT.md |
| 110 | 0x0041d6e0 | FUN_0041d6e0 | boot | 3 | 3 | 0x00402750 |  | re/analysis/promote_c2_boot_lowrva/0x0041d6e0.md |
| 111 | 0x0041e8d0 | FUN_0041e8d0 | gameplay | 3 | 3 | 0x0041e140 |  | re/analysis/bucket_gameplay_0041e140_0041efe0/0041e8d0.md |
| 112 | 0x0041ec00 | FUN_0041ec00 | gameplay | 3 | 3 | 0x0041e140 | STUB | re/analysis/bucket_gameplay_0041e140_0041efe0/0041ec00.md |
| 113 | 0x00474d60 | FUN_00474d60 | vehicle | 3 | 3 | 0x00459290 |  | re/analysis/bucket_vehicle_00453f30_00482030/00474d60.md |
| 114 | 0x004b3bf0 | FUN_004b3bf0 | vehicle | 3 | 3 | 0x00459290 |  | re/analysis/bucket_vehicle_004922e0_0057c500/0x004b3bf0.md |
| 115 | 0x004c4dc0 | FUN_004c4dc0 | util | 3 | 3 | 0x004568d0 |  | re/analysis/bucket_c1_strays/0x004c4dc0.md |
| 116 | 0x005af510 | FUN_005af510 | audio | 3 | 3 | 0x005b0c70 |  | re/analysis/bucket_audio_005af070_005b2190/0x005af510.md |
| 117 | 0x00407e20 | FUN_00407e20 | gameplay | 3 | 4 | 0x00407cd0 |  | re/analysis/bucket_gameplay_00407640_0040ad40/00407e20.md |
| 118 | 0x00413b80 | FUN_00413b80 | hud | 3 | 4 | 0x0040bbb0 | STUB | re/analysis/bucket_gameplay_00412100_00418e50/0x00413b80.md |
| 119 | 0x0041a3d0 | FUN_0041a3d0 | boot | 3 | 4 | 0x0041a1e0 |  | re/analysis/promote_c2_boot_lowrva/0x0041a3d0.md |
| 120 | 0x0041ad60 | FUN_0041ad60 | render | 3 | 4 | 0x0041b450 |  | re/analysis/bucket_gameplay_0041a980_0041d910/0041ad60.md |
| 121 | 0x0041b660 | FUN_0041b660 | boot | 3 | 4 | 0x0041b450 |  | re/analysis/promote_c2_boot_lowrva/0x0041b660.md |
| 122 | 0x0045c350 | Bezier::Interpolate | util | 3 | 4 | 0x0045bba0 |  | re/analysis/profile_career_d4/REPORT.md |
| 123 | 0x0047a020 | FUN_0047a020 | vehicle | 3 | 4 | 0x0047b9b0 |  | re/analysis/bucket_vehicle_00453f30_00482030/0047a020.md |
| 124 | 0x0047b860 | FUN_0047b860 | input | 3 | 4 | 0x0047b9b0 | STUB | re/analysis/bucket_input_dinput_0047b860_0049b300/0047b860.md |
| 125 | 0x0047b8d0 | FUN_0047b8d0 | input | 3 | 4 | 0x0047b9b0 | STUB | re/analysis/bucket_input_dinput_0047b860_0049b300/0047b8d0.md |
| 126 | 0x004967e0 | FUN_004967e0 | input | 3 | 4 | 0x00429290 | STUB | re/analysis/cluster_0049_first_pass/004967e0.md |
| 127 | 0x004b3e40 | FUN_004b3e40 | vehicle | 3 | 4 | 0x00411f30 |  | re/analysis/bucket_vehicle_004922e0_0057c500/0x004b3e40.md |
| 128 | 0x004b68f0 | FUN_004b68f0 | render | 3 | 4 | 0x00411f30 |  | re/analysis/render_3_c1_to_c2_s6/FUN_004b68f0.md |
| 129 | 0x004c5cb0 | FUN_004c5cb0 | render | 3 | 4 | 0x0040bbb0 | STUB | re/analysis/bucket_004c4270/0x004c5cb0.md |
| 130 | 0x004e66d0 | RpClumpForAllAtomics | render | 3 | 4 | 0x00474d60 |  | re/analysis/frontend_gate_unblock_u/0x004e66d0.md |
| 131 | 0x005a7520 | FUN_005a7520 | audio | 3 | 4 | 0x005a66d0 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a7520.md |
| 132 | 0x005af260 | FUN_005af260 | audio | 3 | 4 | 0x005af510 |  | re/analysis/bucket_audio_005af070_005b2190/0x005af260.md |
| 133 | 0x005af600 | FUN_005af600 | audio | 3 | 4 | 0x005af510 |  | re/analysis/bucket_audio_005af070_005b2190/0x005af600.md |
| 134 | 0x005af690 | FUN_005af690 | audio | 3 | 4 | 0x005af510 |  | re/analysis/bucket_audio_005af070_005b2190/0x005af690.md |
| 135 | 0x005af740 | FUN_005af740 | audio | 3 | 4 | 0x005af510 |  | re/analysis/bucket_audio_005af070_005b2190/0x005af740.md |
| 136 | 0x005af7d0 | FUN_005af7d0 | audio | 3 | 4 | 0x005af510 |  | re/analysis/bucket_audio_005af070_005b2190/0x005af7d0.md |
| 137 | 0x0045dc80 | FUN_0045dc80 | audio | 3 | 5 | 0x00466b50 |  | re/analysis/bucket_audio_0042f760_00465b20/0045dc80.md |
| 138 | 0x004671d0 | FUN_004671d0 | render | 3 | 5 | 0x0045a190 |  | re/analysis/bucket_gameplay_0045dff0_0046dd90/0x004671d0.md |
| 139 | 0x00489890 | FUN_00489890 | gameplay | 3 | 5 | 0x0045ae80 |  | re/analysis/bucket_gameplay_0047f450_004e4440/0x00489890.md |
| 140 | 0x004b4a80 | FUN_004b4a80 | render | 3 | 5 | 0x004b4b60 |  | re/analysis/render_3_c1_to_c2_s3/FUN_004b4a80.md |
| 141 | 0x004b7480 | FUN_004b7480 | input | 3 | 5 | 0x0047b880 | STUB | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004b7480.md |
| 142 | 0x005a60b0 | FUN_005a60b0 | audio | 3 | 5 | 0x0045d3f0 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a60b0.md |
| 143 | 0x005a60e0 | FUN_005a60e0 | audio | 3 | 5 | 0x0045d430 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a60e0.md |
| 144 | 0x005a6710 | FUN_005a6710 | audio | 3 | 5 | 0x0045d460 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a6710.md |
| 145 | 0x005a71f0 | FUN_005a71f0 | audio | 3 | 5 | 0x0045d460 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a71f0.md |
| 146 | 0x005af200 | FUN_005af200 | audio | 3 | 5 | 0x005af260 |  | re/analysis/bucket_audio_005af070_005b2190/0x005af200.md |
| 147 | 0x005b1140 | FUN_005b1140 | audio | 3 | 5 | 0x005af7a0 |  | re/analysis/bucket_audio_005af070_005b2190/0x005b1140.md |
| 148 | 0x004102f0 | FUN_004102f0 | util | 3 | 6 | 0x0046baa0 |  | re/analysis/game_state_d5/0x004102f0.md |
| 149 | 0x0041c2c0 | FUN_0041c2c0 | boot | 3 | 6 | 0x00402a40 |  | re/analysis/promote_c2_boot_lowrva/0x0041c2c0.md |
| 150 | 0x00489910 | FUN_00489910 | gameplay | 3 | 6 | 0x00489890 |  | re/analysis/bucket_gameplay_0047f450_004e4440/0x00489910.md |
| 151 | 0x004b7250 | FUN_004b7250 | input | 3 | 6 | 0x004c0510 |  | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004b7250.md |
| 152 | 0x004ba1b0 | FUN_004ba1b0 | input | 3 | 6 | 0x004b7330 |  | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004ba1b0.md |
| 153 | 0x005a6e10 | FUN_005a6e10 | audio | 3 | 6 | 0x005a7460 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a6e10.md |
| 154 | 0x005aab00 | FUN_005aab00 | audio | 3 | 6 | 0x005a60b0 |  | re/analysis/bucket_audio_005a7b60_005ab620/005aab00.md |
| 155 | 0x005ac650 | FUN_005ac650 | audio | 3 | 6 | 0x005afcf0 |  | re/analysis/bucket_audio_005ab710_005af040/0x005ac650.md |
| 156 | 0x005ad6a0 | FUN_005ad6a0 | audio | 3 | 6 | 0x005a71f0 |  | re/analysis/bucket_audio_005ab710_005af040/0x005ad6a0.md |
| 157 | 0x005b10e0 | FUN_005b10e0 | audio | 3 | 6 | 0x005b10a0 |  | re/analysis/bucket_audio_005af070_005b2190/0x005b10e0.md |
| 158 | 0x0046d780 | FUN_0046d780 | ai | 3 | 7 | 0x0046c730 |  | re/analysis/bucket_ai_00452eb0_004c3df0/0046d780.md |
| 159 | 0x0047fad0 | FUN_0047fad0 | gameplay | 3 | 7 | 0x00412050 | STUB | re/analysis/bucket_gameplay_0047f450_004e4440/0x0047fad0.md |
| 160 | 0x004b7570 | FUN_004b7570 | input | 3 | 7 | 0x004b7140 |  | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004b7570.md |
| 161 | 0x004b7df0 | FUN_004b7df0 | input | 3 | 7 | 0x004b9730 |  | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004b7df0.md |
| 162 | 0x005571c0 | FontSys_ShutdownFontPool | hud | 3 | 8 | 0x00552b90 |  | re/analysis/font_pools_frontend_ae6/0x005571c0.md |
| 163 | 0x005a65d0 | FUN_005a65d0 | audio | 3 | 8 | 0x005a6670 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a65d0.md |
| 164 | 0x00405ab0 | FUN_00405ab0 | gameplay | 3 | 9 | 0x0040ad40 |  | re/analysis/bucket_gameplay_00405400_00407620/0x00405ab0.md |
| 165 | 0x004b47e0 | FUN_004b47e0 | render | 3 | 9 | 0x004756e0 |  | re/analysis/render_3_c1_to_c2_s3/FUN_004b47e0.md |
| 166 | 0x004cc9f0 | RwFreeListDestroy | render | 3 | 9 | 0x00556ce0 |  | re/analysis/promote_c2_render_d3d9/0x004cc9f0.md |
| 167 | 0x004cc820 | FUN_004cc820 | render | 3 | 10 | 0x00556d20 |  | re/analysis/promote_c2_render_d3d9/0x004cc820.md |
| 168 | 0x00456c70 | FUN_00456c70 | gameplay | 3 | 12 | 0x00456c30 |  | re/analysis/bucket_gameplay_00456040_004588c0/0x00456c70.md |
| 169 | 0x00415190 | FUN_00415190 | ai | 2 | 0 | (root) |  | re/analysis/bucket_ai_00407a40_00415880/0x00415190.md |
| 170 | 0x00415200 | FUN_00415200 | ai | 2 | 0 | (root) |  | re/analysis/bucket_ai_00407a40_00415880/0x00415200.md |
| 171 | 0x00415e20 | FUN_00415e20 | ai | 2 | 0 | (root) |  | re/analysis/bucket_ai_00415d00_00452ea0/0x00415e20.md |
| 172 | 0x00417180 | FUN_00417180 | ai | 2 | 0 | (root) |  | re/analysis/ai_update/0x00417180.md |
| 173 | 0x00417740 | FUN_00417740 | gameplay | 2 | 0 | (root) | STUB | re/analysis/promote_c1_low_ab1/0x00417740.md |
| 174 | 0x00443d10 | FUN_00443d10 | ai | 2 | 0 | (root) |  | re/analysis/bucket_ai_00415d00_00452ea0/0x00443d10.md |
| 175 | 0x00443dc0 | FUN_00443dc0 | ai | 2 | 0 | (root) |  | re/analysis/bucket_ai_00415d00_00452ea0/0x00443dc0.md |
| 176 | 0x00446520 | FUN_00446520 | util | 2 | 0 | (root) |  | re/analysis/profile_career_d3/FUN_00446520.md |
| 177 | 0x00448700 | FUN_00448700 | vehicle | 2 | 0 | (root) | STUB | re/analysis/skeleton_prep_render/00448700.md |
| 178 | 0x0045bed0 | PowerupRoundCleanup | powerups | 2 | 0 | (root) | STUB | re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x0045bed0.md |
| 179 | 0x0045bf30 | PowerupTeardownAll | powerups | 2 | 0 | (root) | STUB | re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x0045bf30.md |
| 180 | 0x00462dd0 | FUN_00462dd0 | audio | 2 | 0 | (root) |  | re/analysis/bucket_audio_0042f760_00465b20/00462dd0.md |
| 181 | 0x0046d570 | FUN_0046d570 | ai | 2 | 0 | (root) |  | re/analysis/bucket_ai_00452eb0_004c3df0/0046d570.md |
| 182 | 0x004b55a0 | FUN_004b55a0 | render | 2 | 0 | (root) |  | re/analysis/render_3_c1_to_c2_s5/FUN_004b55a0.md |
| 183 | 0x004b6940 | PizOpenAndParse | util | 2 | 0 | (root) |  | re/analysis/bucket_util_00095280_0040e460/000b6940.md |
| 184 | 0x00409710 | FUN_00409710 | render | 2 | 1 | 0x004235b0 |  | re/analysis/promote_c2_render_lowrva/00409710.md |
| 185 | 0x004173b0 | FUN_004173b0 | gameplay | 2 | 1 | 0x00416230 |  | re/analysis/bucket_gameplay_00412100_00418e50/0x004173b0.md |
| 186 | 0x004233e0 | FUN_004233e0 | ai | 2 | 1 | 0x00443440 |  | re/analysis/ai_update_d3/0x004233e0.md |
| 187 | 0x004292d0 | sub_004292d0 | util | 2 | 1 | 0x00409790 |  | re/analysis/timer_d3_cont1_b/0x004292d0.md |
| 188 | 0x0045a950 | FUN_0045a950 | gameplay | 2 | 1 | 0x00407a40 |  | re/analysis/bucket_gameplay_00458a40_0045ac40/0x0045a950.md |
| 189 | 0x004645c0 | FUN_004645c0 | audio | 2 | 1 | 0x0046cbb0 |  | re/analysis/bucket_audio_0042f760_00465b20/004645c0.md |
| 190 | 0x00471ec0 | sub_00471ec0 | camera | 2 | 1 | 0x00407a40 |  | re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x00471ec0.md |
| 191 | 0x00494a80 | sub_00494a80 | frontend | 2 | 1 | 0x0043dfd0 | STUB | re/analysis/intro_splash/0x00494a80.md |
| 192 | 0x005a7f70 | FUN_005a7f70 | audio | 2 | 1 | 0x00462dd0 |  | re/analysis/bucket_audio_005a7b60_005ab620/005a7f70.md |
| 193 | 0x005a8890 | FUN_005a8890 | audio | 2 | 1 | 0x00462dd0 |  | re/analysis/bucket_audio_005a7b60_005ab620/005a8890.md |
| 194 | 0x00409a80 | FUN_00409a80 | gameplay | 2 | 2 | 0x0040acd0 |  | re/analysis/bucket_gameplay_00407640_0040ad40/00409a80.md |
| 195 | 0x0040be50 | FUN_0040be50 | gameplay | 2 | 2 | 0x004215c0 |  | re/analysis/bucket_gameplay_0040ad90_00412010/0x0040be50.md |
| 196 | 0x00429bd0 | FUN_00429bd0 | util | 2 | 2 | 0x00429e10 |  | re/analysis/util_c0_promote/0x00429bd0.md |
| 197 | 0x00441990 | FUN_00441990 | util | 2 | 2 | 0x00448220 |  | re/analysis/profile_career_d3/FUN_00441990.md |
| 198 | 0x00442e00 | FUN_00442e00 | util | 2 | 2 | 0x00448220 |  | re/analysis/profile_career_d3/FUN_00442e00.md |
| 199 | 0x004568d0 | FUN_004568d0 | gameplay | 2 | 2 | 0x0045a0f0 |  | re/analysis/bucket_gameplay_00456040_004588c0/0x004568d0.md |
| 200 | 0x00459290 | FUN_00459290 | vehicle | 2 | 2 | 0x0045ba10 |  | re/analysis/bucket_vehicle_00453f30_00482030/00459290.md |
| 201 | 0x004661a0 | FUN_004661a0 | audio | 2 | 2 | 0x00419760 |  | re/analysis/bucket_audio_00465c10_005a7b50/004661a0.md |
| 202 | 0x004942b0 | FUN_004942b0 | video | 2 | 2 | 0x00494480 |  | re/analysis/bucket_sky_worldobj_dbg_video_hud_00484280_00555910/004942b0.md |
| 203 | 0x00494320 | FUN_00494320 | video | 2 | 2 | 0x00494460 | STUB | re/analysis/bucket_sky_worldobj_dbg_video_hud_00484280_00555910/00494320.md |
| 204 | 0x004944c0 | FUN_004944c0 | util | 2 | 2 | 0x00494a80 |  | re/analysis/video_mci/0x004944c0.md |
| 205 | 0x004c77c0 | FUN_004c77c0 | render | 2 | 2 | 0x00494a80 |  | re/analysis/texture_loader_d3/0x004c77c0.md |
| 206 | 0x005c9d00 | GetRaceEndTrigger | util | 2 | 2 | 0x0040acd0 | STUB | mashedmod/src/mashed_re/Util/GameStateGetters.cpp |
| 207 | 0x004025f0 | FUN_004025f0 | boot | 2 | 3 | 0x00402750 |  | re/analysis/promote_c2_perm_piz_callees/004025f0.md |
| 208 | 0x00407cd0 | FUN_00407cd0 | gameplay | 2 | 3 | 0x00409330 |  | re/analysis/bucket_gameplay_00407640_0040ad40/00407cd0.md |
| 209 | 0x004093b0 | FUN_004093b0 | gameplay | 2 | 3 | 0x00409330 |  | re/analysis/bucket_gameplay_00407640_0040ad40/004093b0.md |
| 210 | 0x004113b0 | FUN_004113b0 | boot | 2 | 3 | 0x00402750 |  | re/analysis/promote_c2_perm_piz_callees/004113b0.md |
| 211 | 0x00418980 | thunk_FUN_0041a060 | boot | 2 | 3 | 0x00402750 |  | re/analysis/promote_c2_boot_lowrva/0x00418980.md |
| 212 | 0x0041a060 | FUN_0041a060 | gameplay | 2 | 3 | 0x00402750 |  | re/analysis/bucket_gameplay_00418e70_0041a8d0/0x0041a060.md |
| 213 | 0x0041a1e0 | FUN_0041a1e0 | boot | 2 | 3 | 0x00402750 |  | re/analysis/promote_c2_boot_lowrva/0x0041a1e0.md |
| 214 | 0x0041def0 | FUN_0041def0 | boot | 2 | 3 | 0x00402750 |  | re/analysis/promote_c2_boot_lowrva/0x0041def0.md |
| 215 | 0x004219f0 | FUN_004219f0 | util | 2 | 3 | 0x00422ba0 |  | re/analysis/random_rng_d2/0x004219f0.md |
| 216 | 0x00427ca0 | FontText_HudInit | hud | 2 | 3 | 0x00402750 |  | re/analysis/hud_promote_c2_b/0x00427ca0.md |
| 217 | 0x004283a0 | FUN_004283a0 | boot | 2 | 3 | 0x00402750 |  | re/analysis/promote_c2_piz_loader/004283a0.md |
| 218 | 0x0044e0a0 | FUN_0044e0a0 | util | 2 | 3 | 0x00422ba0 |  | re/analysis/random_rng_d2/0x0044e0a0.md |
| 219 | 0x00458d00 | FUN_00458d00 | gameplay | 2 | 3 | 0x00459000 |  | re/analysis/bucket_gameplay_00458a40_0045ac40/0x00458d00.md |
| 220 | 0x00459620 | FUN_00459620 | gameplay | 2 | 3 | 0x004568d0 |  | re/analysis/bucket_gameplay_00458a40_0045ac40/0x00459620.md |
| 221 | 0x0045b930 | FUN_0045b930 | boot | 2 | 3 | 0x0045bae0 |  | re/analysis/skeleton_prep_render/0045b930.md |
| 222 | 0x0045bba0 | FUN_0045bba0 | util | 2 | 3 | 0x004568d0 |  | re/analysis/leaderboard_d3/0x0045bba0.md |
| 223 | 0x00471df0 | FUN_00471df0 | boot | 2 | 3 | 0x00402750 | STUB | re/analysis/promote_c2_panel_piz_callees/00471df0.md |
| 224 | 0x00476860 | FUN_00476860 | render | 2 | 3 | 0x004039f0 |  | re/analysis/render_c1_to_c2_s6/FUN_00476860.md |
| 225 | 0x00480720 | VehicleRespawnPlace | vehicle | 2 | 3 | 0x00422ba0 |  | re/analysis/vehicle_promote_c2_b/00480720.md |
| 226 | 0x00484170 | FUN_00484170 | boot | 2 | 3 | 0x00402750 |  | re/analysis/skeleton_prep_high_leverage/00484170.md |
| 227 | 0x004881d0 | FUN_004881d0 | boot | 2 | 3 | 0x00402750 |  | re/analysis/boot_app_init_d4/0x004881d0.md |
| 228 | 0x00494c80 | FUN_00494c80 | util | 2 | 3 | 0x00402750 |  | re/analysis/promote_c2_piz_loader/00494c80.md |
| 229 | 0x004b6640 | FUN_004b6640 | boot | 2 | 3 | 0x00402750 |  | re/analysis/promote_c2_txd_loader/004b6640.md |
| 230 | 0x00558240 | FUN_00558240 | boot | 2 | 3 | 0x00402750 |  | re/analysis/boot_app_init_d5/0x00558240.md |
| 231 | 0x004015a0 | FUN_004015a0 | boot | 2 | 4 | 0x00402f50 |  | re/analysis/promote_c2_boot_lowrva/0x004015a0.md |
| 232 | 0x00407be0 | FUN_00407be0 | gameplay | 2 | 4 | 0x00407cd0 |  | re/analysis/bucket_gameplay_00407640_0040ad40/00407be0.md |
| 233 | 0x0041b690 | FUN_0041b690 | render | 2 | 4 | 0x0041bec0 |  | re/analysis/bucket_gameplay_0041a980_0041d910/0041b690.md |
| 234 | 0x0041c0e0 | FUN_0041c0e0 | boot | 2 | 4 | 0x0041bec0 |  | re/analysis/promote_c2_boot_lowrva/0x0041c0e0.md |
| 235 | 0x0041d890 | FUN_0041d890 | boot | 2 | 4 | 0x0041d6e0 |  | re/analysis/promote_c2_boot_lowrva/0x0041d890.md |
| 236 | 0x0041f060 | FUN_0041f060 | gameplay | 2 | 4 | 0x0045bba0 |  | re/analysis/bucket_gameplay_0041f060_004210f0/0x0041f060.md |
| 237 | 0x004210b0 | FUN_004210b0 | gameplay | 2 | 4 | 0x0040bbb0 | STUB | re/analysis/bucket_gameplay_0041f060_004210f0/0x004210b0.md |
| 238 | 0x00426640 | FUN_00426640 | render | 2 | 4 | 0x0041ea70 |  | re/analysis/render_promote_c2_track_loader/0x00426640.md |
| 239 | 0x004270f0 | CourseRenderFrame | render | 2 | 4 | 0x0041e8d0 |  | re/analysis/render_c1_to_c2_s1/FUN_004270f0.md |
| 240 | 0x0045d3f0 | FUN_0045d3f0 | util | 2 | 4 | 0x00429290 |  | re/analysis/timer_d2/0x0045d3f0.md |
| 241 | 0x0045d460 | FUN_0045d460 | audio | 2 | 4 | 0x004669b0 |  | re/analysis/bucket_audio_0042f760_00465b20/0045d460.md |
| 242 | 0x004759b0 | FUN_004759b0 | render | 2 | 4 | 0x0040bbb0 | STUB | re/analysis/render_c1_to_c2_s5/FUN_004759b0.md |
| 243 | 0x00475a00 | FUN_00475a00 | render | 2 | 4 | 0x0040bb30 | STUB | re/analysis/render_c1_to_c2_s5/FUN_00475a00.md |
| 244 | 0x00476650 | FUN_00476650 | render | 2 | 4 | 0x00476860 |  | re/analysis/render_c1_to_c2_s6/FUN_00476650.md |
| 245 | 0x004775b0 | FUN_004775b0 | render | 2 | 4 | 0x0040bbb0 | STUB | re/analysis/render_2_c1_to_c2_s1/FUN_004775b0.md |
| 246 | 0x004781b0 | FUN_004781b0 | vehicle | 2 | 4 | 0x00411f30 | STUB | re/analysis/bucket_vehicle_00453f30_00482030/004781b0.md |
| 247 | 0x0047a0f0 | FUN_0047a0f0 | track | 2 | 4 | 0x0047b9b0 |  | re/analysis/bucket_track_00401630_0047c0f0/0047a0f0.md |
| 248 | 0x0047b880 | FUN_0047b880 | input | 2 | 4 | 0x0047b9b0 | STUB | re/analysis/bucket_input_dinput_0047b860_0049b300/0047b880.md |
| 249 | 0x00485d90 | FUN_00485d90 | particle | 2 | 4 | 0x0040bbb0 | STUB | re/analysis/particle_promote_ac1/0x00485d90.md |
| 250 | 0x004862d0 | FUN_004862d0 | particle | 2 | 4 | 0x0040bbb0 | STUB | re/analysis/particle_promote_ac1/0x004862d0.md |
| 251 | 0x00487e00 | FUN_00487e00 | particle | 2 | 4 | 0x0040bb30 | STUB | re/analysis/particle_promote_ac2/0x00487e00.md |
| 252 | 0x0048ae00 | FUN_0048ae00 | particle | 2 | 4 | 0x0040bbb0 | STUB | re/analysis/particle_promote_ac4/0x0048ae00.md |
| 253 | 0x0048bbe0 | FUN_0048bbe0 | particle | 2 | 4 | 0x0040bbb0 | STUB | re/analysis/particle_promote_ac5/0048bbe0.md |
| 254 | 0x0048ff20 | FUN_0048ff20 | particle | 2 | 4 | 0x0040bbb0 | STUB | re/analysis/particle_promote_ad1/0048ff20.md |
| 255 | 0x00494ac0 | FUN_00494ac0 | util | 2 | 4 | 0x00494c80 |  | re/analysis/video_mci/0x00494ac0.md |
| 256 | 0x00496940 | FUN_00496940 | render | 2 | 4 | 0x0047ba00 |  | re/analysis/cluster_0049_first_pass/00496940.md |
| 257 | 0x004b4b60 | FUN_004b4b60 | render | 2 | 4 | 0x0045bba0 |  | re/analysis/render_3_c1_to_c2_s3/FUN_004b4b60.md |
| 258 | 0x004b5320 | FUN_004b5320 | vehicle | 2 | 4 | 0x00411f30 | STUB | re/analysis/bucket_vehicle_004922e0_0057c500/0x004b5320.md |
| 259 | 0x004b5580 | FUN_004b5580 | vehicle | 2 | 4 | 0x00411f30 | STUB | re/analysis/bucket_vehicle_004922e0_0057c500/0x004b5580.md |
| 260 | 0x004c1340 | FUN_004c1340 | render | 2 | 4 | 0x00458a40 | STUB | re/analysis/render_c0_promote_c/0x004c1340.md |
| 261 | 0x004c1be0 | sub_004c1be0 | frontend | 2 | 4 | 0x00495350 | STUB | re/analysis/promote_c1_high_ab3/0x004c1be0.md |
| 262 | 0x004e4320 | FUN_004e4320 | render | 2 | 4 | 0x004013f0 |  | re/analysis/skeleton_prep_high_leverage/004e4320.md |
| 263 | 0x00552b60 | FontSys_InitSeq | hud | 2 | 4 | 0x00427ca0 |  | mashedmod/src/mashed_re/HUD/FontCtx.cpp |
| 264 | 0x00556e90 | FUN_00556e90 | render | 2 | 4 | 0x00427ca0 | STUB | re/analysis/promote_c1_high_ab3/0x00556e90.md |
| 265 | 0x005af7a0 | FUN_005af7a0 | audio | 2 | 4 | 0x005af510 |  | re/analysis/bucket_audio_005af070_005b2190/0x005af7a0.md |
| 266 | 0x00409290 | FUN_00409290 | util | 2 | 5 | 0x00424eb0 |  | re/analysis/game_state_d5_cont2/0x00409290.md |
| 267 | 0x00421690 | FUN_00421690 | render | 2 | 5 | 0x00424eb0 |  | re/analysis/bucket_gameplay_00421100_004223f0/0x00421690.md |
| 268 | 0x00431b50 | FUN_00431b50 | util | 2 | 5 | 0x00466b50 |  | re/analysis/timer_d2_cont1/0x00431b50.md |
| 269 | 0x00431b60 | FUN_00431b60 | util | 2 | 5 | 0x00466b50 |  | re/analysis/timer_d2_cont1/0x00431b60.md |
| 270 | 0x0045bfa0 | PowerupSlotActivate | powerups | 2 | 5 | 0x0045c010 |  | re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x0045bfa0.md |
| 271 | 0x00462ec0 | FUN_00462ec0 | audio | 2 | 5 | 0x00466b50 |  | re/analysis/bucket_audio_0042f760_00465b20/00462ec0.md |
| 272 | 0x004642f0 | FUN_004642f0 | audio | 2 | 5 | 0x00466b50 |  | re/analysis/bucket_audio_0042f760_00465b20/004642f0.md |
| 273 | 0x004644a0 | FUN_004644a0 | audio | 2 | 5 | 0x0045daf0 |  | re/analysis/bucket_audio_0042f760_00465b20/004644a0.md |
| 274 | 0x004704c0 | FUN_004704c0 | vehicle | 2 | 5 | 0x00424eb0 |  | re/analysis/game_state_d5_cont2/0x004704c0.md |
| 275 | 0x00471560 | FUN_00471560 | gameplay | 2 | 5 | 0x00471530 |  | re/analysis/bucket_gameplay_00471430_0047b6b0/0x00471560.md |
| 276 | 0x00473c20 | FUN_00473c20 | gameplay | 2 | 5 | 0x00493f80 | STUB | re/analysis/promote_c1_mid_ab2/0x00473c20.md |
| 277 | 0x004892c0 | FUN_004892c0 | particle | 2 | 5 | 0x00485d90 |  | re/analysis/particle_promote_ac2/0x004892c0.md |
| 278 | 0x0048cee0 | FUN_0048cee0 | particle | 2 | 5 | 0x0048e820 |  | re/analysis/particle_promote_ac5/0048cee0.md |
| 279 | 0x00496100 | FUN_00496100 | input | 2 | 5 | 0x004967e0 |  | re/analysis/bucket_input_dinput_0047b860_0049b300/00496100.md |
| 280 | 0x00496530 | FUN_00496530 | input | 2 | 5 | 0x004967e0 |  | re/analysis/bucket_input_dinput_0047b860_0049b300/00496530.md |
| 281 | 0x0049ec10 | FUN_0049ec10 | util | 2 | 5 | 0x00494ac0 |  | re/analysis/video_mci_d2/0x0049ec10.md |
| 282 | 0x004a42c5 | FUN_004a42c5 | render | 2 | 5 | 0x004987b0 |  | re/analysis/rw_engine_init_d3/004a42c5.md |
| 283 | 0x004b5080 | FUN_004b5080 | render | 2 | 5 | 0x0045ae80 |  | re/analysis/bucket_004b4a80/0x004b5080.md |
| 284 | 0x004b5260 | FUN_004b5260 | render | 2 | 5 | 0x0041b690 |  | re/analysis/bucket_004b4a80/0x004b5260.md |
| 285 | 0x004b7330 | FUN_004b7330 | input | 2 | 5 | 0x0047b860 | STUB | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004b7330.md |
| 286 | 0x004c5770 | FUN_004c5770 | render | 2 | 5 | 0x004593b0 |  | re/analysis/bucket_004c4270/0x004c5770.md |
| 287 | 0x004c7730 | FUN_004c7730 | render | 2 | 5 | 0x004c1be0 | STUB | re/analysis/render_4_c1_to_c2_s4/FUN_004c7730.md |
| 288 | 0x00550be0 | FUN_00550be0 | util | 2 | 5 | 0x0045d3f0 |  | re/analysis/timer_d3_cont2/0x00550be0.md |
| 289 | 0x005a6110 | FUN_005a6110 | audio | 2 | 5 | 0x004627f0 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a6110.md |
| 290 | 0x005a75b0 | FUN_005a75b0 | audio | 2 | 5 | 0x005a7520 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a75b0.md |
| 291 | 0x005a7aa0 | FUN_005a7aa0 | audio | 2 | 5 | 0x0045d460 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a7aa0.md |
| 292 | 0x005a7b60 | FUN_005a7b60 | audio | 2 | 5 | 0x0045d460 |  | re/analysis/bucket_audio_005a7b60_005ab620/005a7b60.md |
| 293 | 0x005afa00 | FUN_005afa00 | audio | 2 | 5 | 0x005af8f0 |  | re/analysis/bucket_audio_005af070_005b2190/0x005afa00.md |
| 294 | 0x005b09c0 | FUN_005b09c0 | audio | 2 | 5 | 0x005af8f0 |  | re/analysis/bucket_audio_005af070_005b2190/0x005b09c0.md |
| 295 | 0x005bbb20 | FUN_005bbb20 | audio | 2 | 5 | 0x004627f0 |  | re/analysis/bucket_audio_005b8be0_005bcb80/0x005bbb20.md |
| 296 | 0x0040cfd0 | FUN_0040cfd0 | boot | 2 | 6 | 0x00402a40 |  | re/analysis/promote_c2_boot_lowrva/0x0040cfd0.md |
| 297 | 0x0040e590 | FUN_0040e590 | util | 2 | 6 | 0x0046baa0 |  | re/analysis/game_state_d5-cont1/0x0040e590.md |
| 298 | 0x00412050 | FUN_00412050 | render | 2 | 6 | 0x00426e10 |  | re/analysis/promote_c2_render_frame/0x00412050.md |
| 299 | 0x0041ffb0 | FUN_0041ffb0 | boot | 2 | 6 | 0x00402a40 |  | re/analysis/promote_c2_boot_lowrva/0x0041ffb0.md |
| 300 | 0x004210f0 | FUN_004210f0 | gameplay | 2 | 6 | 0x00402a40 |  | re/analysis/bucket_gameplay_0041f060_004210f0/0x004210f0.md |
| 301 | 0x00421100 | FUN_00421100 | render | 2 | 6 | 0x00421160 |  | re/analysis/bucket_gameplay_00421100_004223f0/0x00421100.md |
| 302 | 0x00421590 | thunk_FUN_004210f0 | boot | 2 | 6 | 0x00402a40 |  | re/analysis/promote_c2_boot_lowrva/0x00421590.md |
| 303 | 0x00426ab0 | sub_00426ab0 | render | 2 | 6 | 0x0041e9b0 |  | re/analysis/render_promote_c2_track_loader/0x00426ab0.md |
| 304 | 0x00427620 | FontText_HudShutdown | hud | 2 | 6 | 0x00402a40 |  | re/analysis/hud_promote_c2_b/0x00427620.md |
| 305 | 0x00431b20 | FUN_00431b20 | audio | 2 | 6 | 0x0045dd60 |  | re/analysis/bucket_audio_0042f760_00465b20/00431b20.md |
| 306 | 0x004417e0 | FUN_004417e0 | util | 2 | 6 | 0x0045d7a0 |  | re/analysis/timer_d3/0x004417e0.md |
| 307 | 0x00462950 | FUN_00462950 | render | 2 | 6 | 0x005a7af0 |  | re/analysis/render_c1_to_c2_s3/FUN_00462950.md |
| 308 | 0x00467210 | sub_00467210 | render | 2 | 6 | 0x0045d7a0 |  | re/analysis/camera_follow/0x00467210.md |
| 309 | 0x004898d0 | FUN_004898d0 | gameplay | 2 | 6 | 0x00489890 |  | re/analysis/bucket_gameplay_0047f450_004e4440/0x004898d0.md |
| 310 | 0x00489940 | FUN_00489940 | particle | 2 | 6 | 0x004893a0 |  | re/analysis/particle_promote_ac2/0x00489940.md |
| 311 | 0x00491780 | FUN_00491780 | render | 2 | 6 | 0x00426e10 |  | re/analysis/track_loader_d2/00491780.md |
| 312 | 0x00495e80 | FUN_00495e80 | input | 2 | 6 | 0x00495fe0 |  | re/analysis/bucket_input_dinput_0047b860_0049b300/00495e80.md |
| 313 | 0x004b4880 | FUN_004b4880 | boot | 2 | 6 | 0x00402a40 |  | re/analysis/skeleton_prep_boot_winmain_b/004b4880.md |
| 314 | 0x004b7be0 | FUN_004b7be0 | input | 2 | 6 | 0x004b7330 |  | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004b7be0.md |
| 315 | 0x004b7fd0 | FUN_004b7fd0 | render | 2 | 6 | 0x004c0510 |  | re/analysis/render_3_c1_to_c2_s6/FUN_004b7fd0.md |
| 316 | 0x004b9730 | FUN_004b9730 | input | 2 | 6 | 0x004c0510 |  | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004b9730.md |
| 317 | 0x005a5f60 | FUN_005a5f60 | audio | 2 | 6 | 0x005a5f00 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a5f60.md |
| 318 | 0x005a6150 | FUN_005a6150 | audio | 2 | 6 | 0x005a6110 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a6150.md |
| 319 | 0x005a6cb0 | FUN_005a6cb0 | audio | 2 | 6 | 0x005a71f0 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a6cb0.md |
| 320 | 0x005a6ea0 | FUN_005a6ea0 | audio | 2 | 6 | 0x005a60b0 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a6ea0.md |
| 321 | 0x005a9ff0 | FUN_005a9ff0 | audio | 2 | 6 | 0x005afa00 |  | re/analysis/bucket_audio_005a7b60_005ab620/005a9ff0.md |
| 322 | 0x005aaa00 | FUN_005aaa00 | audio | 2 | 6 | 0x00462ec0 | STUB | re/analysis/bucket_audio_005a7b60_005ab620/005aaa00.md |
| 323 | 0x005aab70 | FUN_005aab70 | audio | 2 | 6 | 0x005a60e0 | STUB | re/analysis/bucket_audio_005a7b60_005ab620/005aab70.md |
| 324 | 0x005ace70 | FUN_005ace70 | audio | 2 | 6 | 0x005afcf0 |  | re/analysis/bucket_audio_005ab710_005af040/0x005ace70.md |
| 325 | 0x005af300 | FUN_005af300 | audio | 2 | 6 | 0x005b1110 |  | re/analysis/bucket_audio_005af070_005b2190/0x005af300.md |
| 326 | 0x005af860 | FUN_005af860 | audio | 2 | 6 | 0x005afcf0 |  | re/analysis/bucket_audio_005af070_005b2190/0x005af860.md |
| 327 | 0x005b0b40 | FUN_005b0b40 | audio | 2 | 6 | 0x005afcf0 |  | re/analysis/bucket_audio_005af070_005b2190/0x005b0b40.md |
| 328 | 0x005b1030 | FUN_005b1030 | audio | 2 | 6 | 0x005afcf0 |  | re/analysis/bucket_audio_005af070_005b2190/0x005b1030.md |
| 329 | 0x005bbb70 | FUN_005bbb70 | audio | 2 | 6 | 0x005bbb20 |  | re/analysis/bucket_audio_005b8be0_005bcb80/0x005bbb70.md |
| 330 | 0x00405460 | FUN_00405460 | util | 2 | 7 | 0x004102f0 |  | re/analysis/game_state_d5-cont1/0x00405460.md |
| 331 | 0x0040b090 | sub_0040b090 | render | 2 | 7 | 0x00426ab0 |  | re/analysis/promote_c2_render_lowrva/0040b090.md |
| 332 | 0x0040d470 | FUN_0040d470 | util | 2 | 7 | 0x004102f0 |  | re/analysis/leaderboard_d3/0x0040d470.md |
| 333 | 0x00425bf0 | FUN_00425bf0 | frontend | 2 | 7 | 0x004262f0 | STUB | re/analysis/frontend_c1_to_c2_s3/FUN_00425bf0.md |
| 334 | 0x0047c160 | sub_0047c160 | camera | 2 | 7 | 0x00426ab0 |  | re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x0047c160.md |
| 335 | 0x00491490 | FUN_00491490 | render | 2 | 7 | 0x00426ab0 |  | re/analysis/cluster_0049_first_pass/00491490.md |
| 336 | 0x004b4550 | FUN_004b4550 | util | 2 | 7 | 0x00478660 |  | re/analysis/random_rng_d2/0x004b4550.md |
| 337 | 0x004b9600 | FUN_004b9600 | input | 2 | 7 | 0x004b9730 |  | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004b9600.md |
| 338 | 0x004b9650 | FUN_004b9650 | input | 2 | 7 | 0x004b9730 |  | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004b9650.md |
| 339 | 0x00552b90 | FontSys_Shutdown | hud | 2 | 7 | 0x00427620 |  | re/analysis/font_sys_promote_ae3/0x00552b90.md |
| 340 | 0x005a6670 | FUN_005a6670 | audio | 2 | 7 | 0x005a6e10 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a6670.md |
| 341 | 0x005a6c60 | FUN_005a6c60 | audio | 2 | 7 | 0x005a6b70 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a6c60.md |
| 342 | 0x005a6c90 | FUN_005a6c90 | audio | 2 | 7 | 0x005a6b70 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a6c90.md |
| 343 | 0x005ad0b0 | FUN_005ad0b0 | audio | 2 | 7 | 0x005a6b70 |  | re/analysis/bucket_audio_005ab710_005af040/0x005ad0b0.md |
| 344 | 0x00405540 | FUN_00405540 | gameplay | 2 | 8 | 0x00405400 |  | re/analysis/bucket_gameplay_00405400_00407620/0x00405540.md |
| 345 | 0x0040ae30 | FUN_0040ae30 | gameplay | 2 | 8 | 0x0040b090 |  | re/analysis/bucket_gameplay_0040ad90_00412010/0x0040ae30.md |
| 346 | 0x0040aef0 | FUN_0040aef0 | gameplay | 2 | 8 | 0x0040b090 | STUB | re/analysis/bucket_gameplay_0040ad90_00412010/0x0040aef0.md |
| 347 | 0x00445aa0 | FUN_00445aa0 | util | 2 | 8 | 0x004430a0 |  | re/analysis/game_state_d4/0x00445aa0.md |
| 348 | 0x00473220 | FUN_00473220 | render | 2 | 8 | 0x00473ee0 |  | re/analysis/bucket_gameplay_00471430_0047b6b0/0x00473220.md |
| 349 | 0x0047bb10 | FUN_0047bb10 | gameplay | 2 | 8 | 0x0047c160 | STUB | re/analysis/bucket_gameplay_0047ba20_0047f380/0047bb10.md |
| 350 | 0x00556ce0 | FontSys_ShutdownBuffers | hud | 2 | 8 | 0x00552b90 |  | re/analysis/font_atlas_promote_ae5/0x00556ce0.md |
| 351 | 0x00557220 | FontSys_ShutdownDataPools | hud | 2 | 8 | 0x00552b90 |  | re/analysis/font_pools_frontend_ae6/0x00557220.md |
| 352 | 0x005a6340 | FUN_005a6340 | audio | 2 | 8 | 0x005a6670 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a6340.md |
| 353 | 0x00441c80 | FUN_00441c80 | util | 2 | 9 | 0x00445aa0 |  | re/analysis/game_state_d5/0x00441c80.md |
| 354 | 0x00472b10 | FUN_00472b10 | render | 2 | 9 | 0x00473220 |  | re/analysis/bucket_gameplay_00471430_0047b6b0/0x00472b10.md |
| 355 | 0x0047d180 | FUN_0047d180 | gameplay | 2 | 9 | 0x00407800 |  | re/analysis/bucket_gameplay_0047ba20_0047f380/0047d180.md |
| 356 | 0x005571e0 | FontSys_InitFontPool | hud | 2 | 9 | 0x005571c0 |  | re/analysis/font_pools_frontend_ae6/0x005571e0.md |
| 357 | 0x00406130 | FUN_00406130 | gameplay | 2 | 10 | 0x00406370 |  | re/analysis/bucket_gameplay_00405400_00407620/0x00406130.md |
| 358 | 0x00474de0 | FUN_00474de0 | render | 2 | 10 | 0x00405ab0 | STUB | re/analysis/bucket_00474d80/0x00474de0.md |
| 359 | 0x00406160 | FUN_00406160 | gameplay | 2 | 11 | 0x004173a0 |  | re/analysis/bucket_gameplay_00405400_00407620/0x00406160.md |
| 360 | 0x004568a0 | FUN_004568a0 | gameplay | 2 | 11 | 0x00456140 |  | re/analysis/bucket_gameplay_00456040_004588c0/0x004568a0.md |
| 361 | 0x00455fe0 | FUN_00455fe0 | render | 2 | 12 | 0x00456c30 |  | re/analysis/bucket_gameplay_00454130_00455fe0/00455fe0.md |
| 362 | 0x00458b10 | FUN_00458b10 | gameplay | 2 | 13 | 0x0045b990 |  | re/analysis/bucket_gameplay_00458a40_0045ac40/0x00458b10.md |
| 363 | 0x0045b9d0 | FUN_0045b9d0 | powerups | 2 | 13 | 0x00456c70 |  | re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x0045b9d0.md |
| 364 | 0x00465f40 | FUN_00465f40 | audio | 2 | 13 | 0x0045dce0 |  | re/analysis/bucket_gameplay_0045dff0_0046dd90/0x00465f40.md |
| 365 | 0x0040d590 | FUN_0040d590 | frontend | 1 | 0 | (root) |  | re/analysis/frontend_c1_to_c2_s1/0040d590.md |
| 366 | 0x004177b0 | FUN_004177b0 | ai | 1 | 0 | (root) |  | re/analysis/ai_path_following/0x004177b0.md |
| 367 | 0x00417cf0 | FUN_00417cf0 | ai | 1 | 0 | (root) |  | re/analysis/ai_update_d2/0x00417cf0.md |
| 368 | 0x00418860 | FUN_00418860 | ai | 1 | 0 | (root) |  | re/analysis/ai_path_following/0x00418860.md |
| 369 | 0x00423540 | FUN_00423540 | ai | 1 | 0 | (root) |  | re/analysis/ai_path_following/0x00423540.md |
| 370 | 0x0044df80 | FUN_0044df80 | render | 1 | 0 | (root) |  | re/analysis/bucket_gameplay_00422440_0044e070/0x0044df80.md |
| 371 | 0x004625b0 | FUN_004625b0 | audio | 1 | 0 | (root) |  | re/analysis/bucket_audio_0042f760_00465b20/004625b0.md |
| 372 | 0x0046cb30 | Player::GetOffset3D | util | 1 | 0 | (root) |  | re/analysis/profile_career_d4/REPORT.md |
| 373 | 0x004096a0 | FUN_004096a0 | gameplay | 1 | 1 | 0x00409790 |  | re/analysis/bucket_gameplay_00407640_0040ad40/004096a0.md |
| 374 | 0x0040acd0 | FUN_0040acd0 | frontend | 1 | 1 | 0x0043dfd0 |  | re/analysis/c0_promotion_frontend_a/0x0040acd0.md |
| 375 | 0x0040cea0 | FUN_0040cea0 | track | 1 | 1 | 0x0040d110 |  | re/analysis/bucket_track_00401630_0047c0f0/0040cea0.md |
| 376 | 0x00414490 | FUN_00414490 | ai | 1 | 1 | 0x00414c30 |  | re/analysis/bucket_ai_00407a40_00415880/0x00414490.md |
| 377 | 0x00419760 | FUN_00419760 | vehicle | 1 | 1 | 0x00422fd0 |  | re/analysis/vehicle_damage_d4/0x00419760.md |
| 378 | 0x00420420 | FUN_00420420 | track | 1 | 1 | 0x0040d110 |  | re/analysis/bucket_track_00401630_0047c0f0/00420420.md |
| 379 | 0x0042f020 | FUN_0042f020 | frontend | 1 | 1 | 0x0043dfd0 |  | re/analysis/frontend_c0_promote/0x0042f020.md |
| 380 | 0x0042f400 | FUN_0042f400 | frontend | 1 | 1 | 0x0043dfd0 |  | re/analysis/game_mode_cont1/notes.md |
| 381 | 0x00431b80 | FUN_00431b80 | frontend | 1 | 1 | 0x0043dfd0 |  | re/analysis/game_mode_cont1/0x00431b80.md |
| 382 | 0x00431d00 | FUN_00431d00 | frontend | 1 | 1 | 0x0043dfd0 |  | re/analysis/game_mode_cont2/0x00431d00.md |
| 383 | 0x004322c0 | FUN_004322c0 | frontend | 1 | 1 | 0x0043dfd0 |  | re/analysis/game_mode_cont1/notes.md |
| 384 | 0x004324a0 | FUN_004324a0 | frontend | 1 | 1 | 0x0043dfd0 |  | re/analysis/game_mode_cont1/notes.md |
| 385 | 0x00432800 | FUN_00432800 | frontend | 1 | 1 | 0x0043dfd0 |  | re/analysis/game_mode_cont1/notes.md |
| 386 | 0x0043d2a0 | FUN_0043d2a0 | frontend | 1 | 1 | 0x0043dfd0 |  | re/analysis/game_mode_cont1/notes.md |
| 387 | 0x00495080 | FUN_00495080 | frontend | 1 | 1 | 0x0043dfd0 |  | re/analysis/game_mode_cont2/0x00495080.md |
| 388 | 0x004a2d18 | FUN_004a2d18 | render | 1 | 1 | 0x004a2cbd |  | re/analysis/rw_engine_init_d2/004a2d18.md |
| 389 | 0x004cc400 | FUN_004cc400 | render | 1 | 1 | 0x004cc5e0 |  | re/analysis/texture_loader_d3/0x004cc400.md |
| 390 | 0x00402b70 | FUN_00402b70 | util | 1 | 2 | 0x00495280 |  | re/analysis/bucket_util_00095280_0040e460/00402b70.md |
| 391 | 0x004039f0 | FUN_004039f0 | util | 1 | 2 | 0x004215c0 |  | re/analysis/leaderboard_d3/0x004039f0.md |
| 392 | 0x004095a0 | FUN_004095a0 | gameplay | 1 | 2 | 0x00409710 | STUB | re/analysis/bucket_gameplay_00407640_0040ad40/004095a0.md |
| 393 | 0x0040e4b0 | Race::GetSoleFinishedPlayer | util | 1 | 2 | 0x00448220 |  | re/analysis/profile_career_d2/FUN_00448220.md |
| 394 | 0x00418de0 | FUN_00418de0 | vehicle | 1 | 2 | 0x00419760 |  | re/analysis/vehicle_damage_d4/0x00418de0.md |
| 395 | 0x0041ecc0 | FUN_0041ecc0 | render | 1 | 2 | 0x0040cea0 |  | re/analysis/bucket_gameplay_0041e140_0041efe0/0041ecc0.md |
| 396 | 0x00494fd0 | FUN_00494fd0 | render | 1 | 2 | 0x00495080 |  | re/analysis/cluster_0049_first_pass/00494fd0.md |
| 397 | 0x004b6570 | PizOpen | util | 1 | 2 | 0x00495280 |  | re/analysis/bucket_util_00095280_0040e460/000b6570.md |
| 398 | 0x004b67a0 | FUN_004b67a0 | render | 1 | 2 | 0x004952f0 |  | re/analysis/render_3_c1_to_c2_s5/FUN_004b67a0.md |
| 399 | 0x00550910 | FUN_00550910 | save | 1 | 2 | 0x004cc160 |  | re/analysis/save_gamesave_d3/00550910.md |
| 400 | 0x004013f0 | FUN_004013f0 | hud | 1 | 3 | 0x00429bd0 |  | re/analysis/boot_hud_promote_ae1/0x004013f0.md |
| 401 | 0x00403640 | FUN_00403640 | boot | 1 | 3 | 0x00402750 |  | re/analysis/promote_c2_perm_piz_callees/00403640.md |
| 402 | 0x00403910 | FUN_00403910 | render | 1 | 3 | 0x004938e0 |  | re/analysis/promote_c2_render_lowrva/00403910.md |
| 403 | 0x004039c0 | FUN_004039c0 | vehicle | 1 | 3 | 0x0045bae0 |  | re/analysis/promote_c2_vehicle_lowrva/0x004039c0.md |
| 404 | 0x00404830 | FUN_00404830 | boot | 1 | 3 | 0x00402750 |  | re/analysis/promote_c2_perm_piz_callees/00404830.md |
| 405 | 0x00409970 | FUN_00409970 | gameplay | 1 | 3 | 0x004098d0 |  | re/analysis/bucket_gameplay_00407640_0040ad40/00409970.md |
| 406 | 0x00411f30 | FUN_00411f30 | vehicle | 1 | 3 | 0x0045bae0 |  | re/analysis/promote_c2_vehicle_lowrva/0x00411f30.md |
| 407 | 0x00412890 | FUN_00412890 | boot | 1 | 3 | 0x00402750 |  | re/analysis/promote_c2_panel_piz_callees/00412890.md |
| 408 | 0x0041c100 | FUN_0041c100 | boot | 1 | 3 | 0x00402750 |  | re/analysis/promote_c2_panel_piz_callees/0041c100.md |
| 409 | 0x0041d8b0 | FUN_0041d8b0 | boot | 1 | 3 | 0x00402750 |  | re/analysis/promote_c2_panel_piz_callees/0041d8b0.md |
| 410 | 0x0041db90 | FUN_0041db90 | boot | 1 | 3 | 0x00402750 |  | re/analysis/promote_c2_panel_piz_callees/0041db90.md |
| 411 | 0x0041eaa0 | FUN_0041eaa0 | boot | 1 | 3 | 0x00402750 |  | re/analysis/promote_c2_panel_piz_callees/0041eaa0.md |
| 412 | 0x0041f290 | FUN_0041f290 | gameplay | 1 | 3 | 0x004039f0 |  | re/analysis/bucket_gameplay_0041f060_004210f0/0x0041f290.md |
| 413 | 0x00420d00 | FUN_00420d00 | boot | 1 | 3 | 0x00402750 |  | re/analysis/promote_c2_panel_piz_callees/00420d00.md |
| 414 | 0x00425bc0 | FUN_00425bc0 | boot | 1 | 3 | 0x00402750 |  | re/analysis/promote_c2_perm_piz_callees/00425bc0.md |
| 415 | 0x004274e0 | FUN_004274e0 | frontend | 1 | 3 | 0x00402750 |  | re/analysis/skeleton_prep_game_state/004274e0.md |
| 416 | 0x004275d0 | FUN_004275d0 | boot | 1 | 3 | 0x00402750 |  | re/analysis/promote_c2_boot_lowrva/0x004275d0.md |
| 417 | 0x0042c7c0 | FUN_0042c7c0 | util | 1 | 3 | 0x00429660 |  | re/analysis/localization_d2/0x0042c7c0.md |
| 418 | 0x004587a0 | FUN_004587a0 | vehicle | 1 | 3 | 0x0045bae0 |  | re/analysis/bucket_vehicle_00453f30_00482030/004587a0.md |
| 419 | 0x00458a40 | FUN_00458a40 | gameplay | 1 | 3 | 0x00459000 |  | re/analysis/bucket_gameplay_00458a40_0045ac40/0x00458a40.md |
| 420 | 0x00458dd0 | FUN_00458dd0 | gameplay | 1 | 3 | 0x00459000 |  | re/analysis/bucket_gameplay_00458a40_0045ac40/0x00458dd0.md |
| 421 | 0x004633d0 | FUN_004633d0 | audio | 1 | 3 | 0x004039f0 |  | re/analysis/bucket_audio_0042f760_00465b20/004633d0.md |
| 422 | 0x00465c10 | FUN_00465c10 | audio | 1 | 3 | 0x004661a0 |  | re/analysis/bucket_audio_00465c10_005a7b50/00465c10.md |
| 423 | 0x004669b0 | FUN_004669b0 | audio | 1 | 3 | 0x00402750 |  | re/analysis/promote_c2_perm_piz_callees/004669b0.md |
| 424 | 0x0047ba00 | FUN_0047ba00 | boot | 1 | 3 | 0x00402750 |  | re/analysis/promote_c2_perm_piz_callees/0047ba00.md |
| 425 | 0x0047cea0 | FUN_0047cea0 | physics | 1 | 3 | 0x00422ba0 |  | re/analysis/bucket_physics_smplfzx_00478cb0_0057c4b0/0x0047cea0.md |
| 426 | 0x00480d60 | FUN_00480d60 | vehicle | 1 | 3 | 0x00422ba0 |  | re/analysis/bucket_vehicle_00453f30_00482030/00480d60.md |
| 427 | 0x00481750 | FUN_00481750 | vehicle | 1 | 3 | 0x004039f0 |  | re/analysis/bucket_vehicle_00453f30_00482030/00481750.md |
| 428 | 0x00481780 | FUN_00481780 | vehicle | 1 | 3 | 0x0046bf50 |  | re/analysis/bucket_vehicle_00453f30_00482030/00481780.md |
| 429 | 0x004841d0 | FUN_004841d0 | boot | 1 | 3 | 0x00402750 |  | re/analysis/skeleton_prep_render/004841d0.md |
| 430 | 0x004847d0 | FUN_004847d0 | world-objects | 1 | 3 | 0x00484cf0 |  | re/analysis/bucket_sky_worldobj_dbg_video_hud_00484280_00555910/004847d0.md |
| 431 | 0x00484ac0 | FUN_00484ac0 | world-objects | 1 | 3 | 0x00484cf0 |  | re/analysis/bucket_sky_worldobj_dbg_video_hud_00484280_00555910/00484ac0.md |
| 432 | 0x00486460 | FUN_00486460 | particle | 1 | 3 | 0x004039f0 |  | re/analysis/particle_promote_ac1/0x00486460.md |
| 433 | 0x00496c10 | FUN_00496c10 | render | 1 | 3 | 0x004938e0 |  | re/analysis/cluster_0049_first_pass/00496c10.md |
| 434 | 0x00496e40 | FUN_00496e40 | boot | 1 | 3 | 0x00402750 |  | re/analysis/boot_subsystem_d3/0x00496e40.md |
| 435 | 0x00499ce0 | FUN_00499ce0 | render | 1 | 3 | 0x00402750 |  | re/analysis/skeleton_prep_boot_winmain_b/00499ce0.md |
| 436 | 0x004c57a0 | FontCtxMatrix_AllocInit | hud | 1 | 3 | 0x00459290 |  | re/analysis/font_text_d2/font_text_d2-20260503.md |
| 437 | 0x004caea0 | FUN_004caea0 | render | 1 | 3 | 0x00402750 |  | re/analysis/promote_c2_rw_engine_init/004caea0.md |
| 438 | 0x004d8060 | FUN_004d8060 | render | 1 | 3 | 0x004c7650 |  | re/analysis/render_6_c1_to_c2_s3/004d8060.md |
| 439 | 0x004d8350 | FUN_004d8350 | render | 1 | 3 | 0x004c0ed0 |  | re/analysis/render_6_c1_to_c2_s3/004d8350.md |
| 440 | 0x00401000 | FUN_00401000 | hud | 1 | 4 | 0x0040bbb0 | STUB | re/analysis/boot_hud_promote_ae1/0x00401000.md |
| 441 | 0x004011f0 | FUN_004011f0 | hud | 1 | 4 | 0x004013f0 |  | re/analysis/boot_hud_promote_ae1/0x004011f0.md |
| 442 | 0x00401630 | FUN_00401630 | track | 1 | 4 | 0x004025f0 | STUB | re/analysis/bucket_track_00401630_0047c0f0/00401630.md |
| 443 | 0x00401690 | FUN_00401690 | render | 1 | 4 | 0x004025f0 | STUB | re/analysis/bucket_render_00401690_004dc5b0/00401690.md |
| 444 | 0x00402240 | FUN_00402240 | render | 1 | 4 | 0x004025f0 | STUB | re/analysis/render_c1_to_c2_s1/FUN_00402240.md |
| 445 | 0x00402590 | FUN_00402590 | render | 1 | 4 | 0x004025f0 | STUB | re/analysis/render_c1_to_c2_s1/FUN_00402590.md |
| 446 | 0x004034a0 | FUN_004034a0 | render | 1 | 4 | 0x00403640 | STUB | re/analysis/render_c1_to_c2_s1/FUN_004034a0.md |
| 447 | 0x004036a0 | FUN_004036a0 | render | 1 | 4 | 0x00403910 |  | re/analysis/render_c1_to_c2_s1/FUN_004036a0.md |
| 448 | 0x00404820 | FUN_00404820 | hud | 1 | 4 | 0x0045b930 |  | re/analysis/boot_hud_promote_ae1/0x00404820.md |
| 449 | 0x00412010 | FUN_00412010 | gameplay | 1 | 4 | 0x0045b930 |  | re/analysis/bucket_gameplay_0040ad90_00412010/0x00412010.md |
| 450 | 0x0041cd20 | FUN_0041cd20 | render | 1 | 4 | 0x0041d6e0 |  | re/analysis/bucket_gameplay_0041a980_0041d910/0041cd20.md |
| 451 | 0x00424eb0 | FUN_00424eb0 | util | 1 | 4 | 0x0040dc90 |  | re/analysis/util_c0_promote/0x00424eb0.md |
| 452 | 0x00427880 | FUN_00427880 | frontend | 1 | 4 | 0x00427ca0 |  | re/analysis/frontend_c1_to_c2_s5/FUN_00427880.md |
| 453 | 0x00429240 | FUN_00429240 | frontend | 1 | 4 | 0x00429290 |  | re/analysis/frontend_c1_to_c2_s6/FUN_00429240.md |
| 454 | 0x00442440 | FUN_00442440 | util | 1 | 4 | 0x0040dc90 |  | re/analysis/util_c0_promote/0x00442440.md |
| 455 | 0x004584e0 | FUN_004584e0 | gameplay | 1 | 4 | 0x00458a40 |  | re/analysis/bucket_gameplay_00456040_004588c0/0x004584e0.md |
| 456 | 0x00458880 | FUN_00458880 | gameplay | 1 | 4 | 0x0045b930 |  | re/analysis/bucket_gameplay_00456040_004588c0/0x00458880.md |
| 457 | 0x00458e00 | FUN_00458e00 | gameplay | 1 | 4 | 0x00458a40 |  | re/analysis/bucket_gameplay_00458a40_0045ac40/0x00458e00.md |
| 458 | 0x004593b0 | FUN_004593b0 | gameplay | 1 | 4 | 0x0045b930 |  | re/analysis/bucket_gameplay_00458a40_0045ac40/0x004593b0.md |
| 459 | 0x0045a190 | FUN_0045a190 | gameplay | 1 | 4 | 0x0045bba0 |  | re/analysis/bucket_gameplay_00458a40_0045ac40/0x0045a190.md |
| 460 | 0x0045ae80 | FUN_0045ae80 | gameplay | 1 | 4 | 0x00474d80 |  | re/analysis/bucket_gameplay_0045ae80_0045dd50/0045ae80.md |
| 461 | 0x0045c010 | PowerupPickupWrapper | powerups | 1 | 4 | 0x0045bba0 |  | re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x0045c010.md |
| 462 | 0x0045d430 | FUN_0045d430 | util | 1 | 4 | 0x00429290 |  | re/analysis/timer_d2/0x0045d430.md |
| 463 | 0x004627f0 | FUN_004627f0 | audio | 1 | 4 | 0x004669b0 |  | re/analysis/bucket_audio_0042f760_00465b20/004627f0.md |
| 464 | 0x00466b50 | FUN_00466b50 | util | 1 | 4 | 0x00429290 |  | re/analysis/timer_d2/0x00466b50.md |
| 465 | 0x00476390 | FUN_00476390 | render | 1 | 4 | 0x0040bbb0 | STUB | re/analysis/render_c1_to_c2_s5/FUN_00476390.md |
| 466 | 0x00476880 | FUN_00476880 | util | 1 | 4 | 0x0045bba0 |  | re/analysis/random_rng_d2/0x00476880.md |
| 467 | 0x00477e40 | FUN_00477e40 | render | 1 | 4 | 0x0040bbb0 | STUB | re/analysis/render_2_c1_to_c2_s1/FUN_00477e40.md |
| 468 | 0x00482900 | FUN_00482900 | vehicle | 1 | 4 | 0x004113b0 | STUB | re/analysis/bucket_vehicle_004820e0_00485420/00482900.md |
| 469 | 0x0048a5d0 | FUN_0048a5d0 | particle | 1 | 4 | 0x00474d80 |  | re/analysis/particle_promote_ac3/0x0048a5d0.md |
| 470 | 0x0048e820 | FUN_0048e820 | particle | 1 | 4 | 0x0040bbb0 | STUB | re/analysis/particle_promote_ac6/0x0048e820.md |
| 471 | 0x0048eac0 | FUN_0048eac0 | particle | 1 | 4 | 0x0040bbb0 | STUB | re/analysis/particle_promote_ac6/0x0048eac0.md |
| 472 | 0x00493bc0 | FUN_00493bc0 | util | 1 | 4 | 0x00494c80 |  | re/analysis/video_mci/0x00493bc0.md |
| 473 | 0x00493f00 | FUN_00493f00 | util | 1 | 4 | 0x00494c80 |  | re/analysis/video_mci/0x00493f00.md |
| 474 | 0x004969a0 | FUN_004969a0 | render | 1 | 4 | 0x00496c10 |  | re/analysis/cluster_0049_first_pass/004969a0.md |
| 475 | 0x004b3d20 | FUN_004b3d20 | render | 1 | 4 | 0x0042a6b0 | STUB | re/analysis/texture_loader/0x004b3d20.md |
| 476 | 0x004b4080 | FUN_004b4080 | render | 1 | 4 | 0x00401340 |  | re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x004b4080.md |
| 477 | 0x004b6540 | thunk_FUN_004b6640 | boot | 1 | 4 | 0x004b6640 |  | re/analysis/promote_c2_txd_loader/004b6540.md |
| 478 | 0x004c4eb0 | RwMatrixInvert_CofactorPath | render | 1 | 4 | 0x004c4dc0 |  | re/analysis/render_pipeline_d3/004c4eb0.md |
| 479 | 0x004c52f0 | RwMatrixCombine | render | 1 | 4 | 0x004c1480 |  | re/analysis/vehicle_update_d3_cont/004c52f0.md |
| 480 | 0x00556d70 | FUN_00556d70 | hud | 1 | 4 | 0x00427ca0 |  | re/analysis/font_atlas_promote_ae5/0x00556d70.md |
| 481 | 0x00413bb0 | FUN_00413bb0 | hud | 1 | 5 | 0x0040bd00 | STUB | re/analysis/bucket_gameplay_00412100_00418e50/0x00413bb0.md |
| 482 | 0x0041b440 | FUN_0041b440 | render | 1 | 5 | 0x0041b660 |  | re/analysis/bucket_gameplay_0041a980_0041d910/0041b440.md |
| 483 | 0x0041beb0 | FUN_0041beb0 | render | 1 | 5 | 0x0041c0e0 |  | re/analysis/bucket_gameplay_0041a980_0041d910/0041beb0.md |
| 484 | 0x0041d6d0 | FUN_0041d6d0 | render | 1 | 5 | 0x0041d890 |  | re/analysis/bucket_gameplay_0041a980_0041d910/0041d6d0.md |
| 485 | 0x00421160 | FUN_00421160 | render | 1 | 5 | 0x004210b0 |  | re/analysis/bucket_gameplay_00421100_004223f0/0x00421160.md |
| 486 | 0x00421d20 | FUN_00421d20 | util | 1 | 5 | 0x00476880 |  | re/analysis/random_rng_d2/0x00421d20.md |
| 487 | 0x0042a470 | FUN_0042a470 | render | 1 | 5 | 0x0042a530 |  | re/analysis/render_c1_to_c2_s1/FUN_0042a470.md |
| 488 | 0x0042a640 | FUN_0042a640 | frontend | 1 | 5 | 0x0042a5d0 |  | re/analysis/frontend_c1_to_c2_s6/FUN_0042a640.md |
| 489 | 0x00459400 | FUN_00459400 | gameplay | 1 | 5 | 0x0045a190 |  | re/analysis/bucket_gameplay_00458a40_0045ac40/0x00459400.md |
| 490 | 0x00459480 | FUN_00459480 | gameplay | 1 | 5 | 0x0045a190 |  | re/analysis/bucket_gameplay_00458a40_0045ac40/0x00459480.md |
| 491 | 0x0045a530 | FUN_0045a530 | gameplay | 1 | 5 | 0x0045ae80 |  | re/analysis/bucket_gameplay_00458a40_0045ac40/0x0045a530.md |
| 492 | 0x0045a590 | FUN_0045a590 | gameplay | 1 | 5 | 0x0045ae80 |  | re/analysis/bucket_gameplay_00458a40_0045ac40/0x0045a590.md |
| 493 | 0x0045ac40 | FUN_0045ac40 | gameplay | 1 | 5 | 0x0045ae80 |  | re/analysis/bucket_gameplay_00458a40_0045ac40/0x0045ac40.md |
| 494 | 0x0045d3a0 | FUN_0045d3a0 | util | 1 | 5 | 0x00466b50 |  | re/analysis/timer_d2_cont1/0x0045d3a0.md |
| 495 | 0x0045d7a0 | FUN_0045d7a0 | util | 1 | 5 | 0x00466b50 |  | re/analysis/timer_d2_cont1/0x0045d7a0.md |
| 496 | 0x0045db50 | FUN_0045db50 | util | 1 | 5 | 0x00466b50 |  | re/analysis/timer_d3/0x0045db50.md |
| 497 | 0x0045dbe0 | FUN_0045dbe0 | util | 1 | 5 | 0x00466b50 |  | re/analysis/timer_d3/0x0045dbe0.md |
| 498 | 0x0045dd60 | FUN_0045dd60 | audio | 1 | 5 | 0x00466b50 |  | re/analysis/bucket_audio_0042f760_00465b20/0045dd60.md |
| 499 | 0x0045dfc0 | FUN_0045dfc0 | util | 1 | 5 | 0x00466b50 |  | re/analysis/timer_d3_cont1_b/0x0045dfc0.md |
| 500 | 0x004623e0 | FUN_004623e0 | audio | 1 | 5 | 0x00466b50 |  | re/analysis/bucket_audio_0042f760_00465b20/004623e0.md |
| 501 | 0x004624c0 | FUN_004624c0 | audio | 1 | 5 | 0x004627f0 |  | re/analysis/bucket_audio_0042f760_00465b20/004624c0.md |
| 502 | 0x00462520 | FUN_00462520 | audio | 1 | 5 | 0x00466b50 |  | re/analysis/bucket_audio_0042f760_00465b20/00462520.md |
| 503 | 0x00462f50 | FUN_00462f50 | audio | 1 | 5 | 0x00466b50 |  | re/analysis/bucket_audio_0042f760_00465b20/00462f50.md |
| 504 | 0x004631f0 | FUN_004631f0 | audio | 1 | 5 | 0x00466b50 |  | re/analysis/bucket_audio_0042f760_00465b20/004631f0.md |
| 505 | 0x00463590 | FUN_00463590 | audio | 1 | 5 | 0x00466b50 |  | re/analysis/bucket_audio_0042f760_00465b20/00463590.md |
| 506 | 0x00463640 | FUN_00463640 | audio | 1 | 5 | 0x00466b50 |  | re/analysis/bucket_audio_0042f760_00465b20/00463640.md |
| 507 | 0x00463c80 | FUN_00463c80 | audio | 1 | 5 | 0x00466b50 |  | re/analysis/bucket_audio_0042f760_00465b20/00463c80.md |
| 508 | 0x00463f40 | FUN_00463f40 | audio | 1 | 5 | 0x00466b50 |  | re/analysis/bucket_audio_0042f760_00465b20/00463f40.md |
| 509 | 0x004647f0 | FUN_004647f0 | audio | 1 | 5 | 0x00466b50 |  | re/analysis/bucket_audio_0042f760_00465b20/004647f0.md |
| 510 | 0x004648b0 | FUN_004648b0 | audio | 1 | 5 | 0x00466b50 |  | re/analysis/bucket_audio_0042f760_00465b20/004648b0.md |
| 511 | 0x00464a50 | FUN_00464a50 | audio | 1 | 5 | 0x00466b50 |  | re/analysis/bucket_audio_0042f760_00465b20/00464a50.md |
| 512 | 0x0046c7d0 | FUN_0046c7d0 | vehicle | 1 | 5 | 0x00424eb0 |  | re/analysis/util_c0_promote/0x0046c7d0.md |
| 513 | 0x00475d90 | FUN_00475d90 | render | 1 | 5 | 0x0040bd00 | STUB | re/analysis/render_c1_to_c2_s5/FUN_00475d90.md |
| 514 | 0x00475ea0 | FUN_00475ea0 | render | 1 | 5 | 0x0040bd00 | STUB | re/analysis/render_c1_to_c2_s5/FUN_00475ea0.md |
| 515 | 0x00475ef0 | FUN_00475ef0 | render | 1 | 5 | 0x00476390 |  | re/analysis/render_c1_to_c2_s5/FUN_00475ef0.md |
| 516 | 0x00476710 | FUN_00476710 | render | 1 | 5 | 0x00476880 |  | re/analysis/render_c1_to_c2_s6/FUN_00476710.md |
| 517 | 0x00477870 | FUN_00477870 | render | 1 | 5 | 0x0040bd00 | STUB | re/analysis/render_2_c1_to_c2_s1/FUN_00477870.md |
| 518 | 0x0047b8a0 | FUN_0047b8a0 | input | 1 | 5 | 0x0047b860 | STUB | re/analysis/bucket_input_dinput_0047b860_0049b300/0047b8a0.md |
| 519 | 0x00485e10 | FUN_00485e10 | particle | 1 | 5 | 0x0040bd00 | STUB | re/analysis/particle_promote_ac1/0x00485e10.md |
| 520 | 0x00486350 | FUN_00486350 | particle | 1 | 5 | 0x0040bd00 | STUB | re/analysis/particle_promote_ac1/0x00486350.md |
| 521 | 0x00487140 | FUN_00487140 | particle | 1 | 5 | 0x0040bd00 | STUB | re/analysis/particle_promote_ac2/0x00487140.md |
| 522 | 0x0048a490 | FUN_0048a490 | particle | 1 | 5 | 0x0048a5d0 | STUB | re/analysis/particle_promote_ac3/0x0048a490.md |
| 523 | 0x0048af70 | FUN_0048af70 | particle | 1 | 5 | 0x0040bd00 | STUB | re/analysis/particle_promote_ac4/0x0048af70.md |
| 524 | 0x0048bc10 | FUN_0048bc10 | particle | 1 | 5 | 0x0040bd00 | STUB | re/analysis/particle_promote_ac5/0048bc10.md |
| 525 | 0x0048cf70 | FUN_0048cf70 | particle | 1 | 5 | 0x0040bd00 | STUB | re/analysis/particle_promote_ac5/0048cf70.md |
| 526 | 0x0048fdd0 | FUN_0048fdd0 | particle | 1 | 5 | 0x0040bd00 | STUB | re/analysis/particle_promote_ad1/0048fdd0.md |
| 527 | 0x0048fef0 | FUN_0048fef0 | particle | 1 | 5 | 0x0048ff20 |  | re/analysis/particle_promote_ad1/0048fef0.md |
| 528 | 0x0048ffd0 | FUN_0048ffd0 | particle | 1 | 5 | 0x0040bd00 | STUB | re/analysis/particle_promote_ad1/0048ffd0.md |
| 529 | 0x00493c00 | FUN_00493c00 | util | 1 | 5 | 0x00494ac0 |  | re/analysis/video_mci/0x00493c00.md |
| 530 | 0x00495fe0 | FUN_00495fe0 | input | 1 | 5 | 0x004967e0 |  | re/analysis/bucket_input_dinput_0047b860_0049b300/00495fe0.md |
| 531 | 0x004972b0 | FUN_004972b0 | input | 1 | 5 | 0x004967e0 |  | re/analysis/promote_c2_dinput_init/004972b0.md |
| 532 | 0x004b4cd0 | Bezier::QueryWrapper | util | 1 | 5 | 0x0045ae80 |  | re/analysis/profile_career_d4/REPORT.md |
| 533 | 0x004cf7d0 | FUN_004cf7d0 | render | 1 | 5 | 0x004b3d20 |  | re/analysis/texture_loader_d2/0x004cf7d0.md |
| 534 | 0x005a6280 | FUN_005a6280 | audio | 1 | 5 | 0x0045d460 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a6280.md |
| 535 | 0x005a7560 | FUN_005a7560 | audio | 1 | 5 | 0x005a7520 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a7560.md |
| 536 | 0x005a8e70 | FUN_005a8e70 | audio | 1 | 5 | 0x004627f0 |  | re/analysis/bucket_audio_005a7b60_005ab620/005a8e70.md |
| 537 | 0x005af230 | FUN_005af230 | audio | 1 | 5 | 0x005af260 |  | re/analysis/bucket_audio_005af070_005b2190/0x005af230.md |
| 538 | 0x005b8570 | FUN_005b8570 | audio | 1 | 5 | 0x0045d460 |  | re/analysis/bucket_audio_005b2220_005b8570/0x005b8570.md |
| 539 | 0x005b9e30 | FUN_005b9e30 | audio | 1 | 5 | 0x004627f0 |  | re/analysis/bucket_audio_005b8be0_005bcb80/0x005b9e30.md |
| 540 | 0x004014b0 | FUN_004014b0 | boot | 1 | 6 | 0x00402a40 |  | re/analysis/promote_c2_boot_lowrva/0x004014b0.md |
| 541 | 0x00403660 | FUN_00403660 | boot | 1 | 6 | 0x00402a40 |  | re/analysis/promote_c2_boot_lowrva/0x00403660.md |
| 542 | 0x00407600 | Player::GetPositionPtr | util | 1 | 6 | 0x00464a50 |  | re/analysis/profile_career_d4/REPORT.md |
| 543 | 0x004189e0 | thunk_FUN_004196f0 | boot | 1 | 6 | 0x00402a40 |  | re/analysis/promote_c2_boot_lowrva/0x004189e0.md |
| 544 | 0x004196f0 | FUN_004196f0 | gameplay | 1 | 6 | 0x00402a40 |  | re/analysis/bucket_gameplay_00418e70_0041a8d0/0x004196f0.md |
| 545 | 0x0041ccf0 | FUN_0041ccf0 | boot | 1 | 6 | 0x00402a40 |  | re/analysis/promote_c2_boot_lowrva/0x0041ccf0.md |
| 546 | 0x0041da80 | FUN_0041da80 | boot | 1 | 6 | 0x00402a40 |  | re/analysis/promote_c2_boot_lowrva/0x0041da80.md |
| 547 | 0x0041de70 | FUN_0041de70 | boot | 1 | 6 | 0x00402a40 |  | re/analysis/promote_c2_boot_lowrva/0x0041de70.md |
| 548 | 0x0041e0d0 | FUN_0041e0d0 | boot | 1 | 6 | 0x00402a40 |  | re/analysis/promote_c2_boot_lowrva/0x0041e0d0.md |
| 549 | 0x00425ed0 | FUN_00425ed0 | boot | 1 | 6 | 0x00402a40 |  | re/analysis/promote_c2_boot_lowrva/0x00425ed0.md |
| 550 | 0x004260e0 | FUN_004260e0 | render | 1 | 6 | 0x00426e10 |  | re/analysis/render_promote_c2_track_loader/0x004260e0.md |
| 551 | 0x004262f0 | FUN_004262f0 | render | 1 | 6 | 0x00426e10 |  | re/analysis/render_promote_c2_track_loader/0x004262f0.md |
| 552 | 0x00428400 | FUN_00428400 | boot | 1 | 6 | 0x00402a40 |  | re/analysis/promote_c2_boot_lowrva/0x00428400.md |
| 553 | 0x0042a8d0 | FUN_0042a8d0 | render | 1 | 6 | 0x00426e10 |  | re/analysis/track_loader_d2/0042a8d0.md |
| 554 | 0x0042c2a0 | FUN_0042c2a0 | boot | 1 | 6 | 0x00402a40 |  | re/analysis/promote_c2_boot_lowrva/0x0042c2a0.md |
| 555 | 0x0045d1e0 | FUN_0045d1e0 | util | 1 | 6 | 0x0045d3a0 |  | re/analysis/timer_d3/0x0045d1e0.md |
| 556 | 0x0045d330 | FUN_0045d330 | util | 1 | 6 | 0x0045d3a0 |  | re/analysis/timer_d3/0x0045d330.md |
| 557 | 0x00464670 | FUN_00464670 | audio | 1 | 6 | 0x004648b0 |  | re/analysis/bucket_audio_0042f760_00465b20/00464670.md |
| 558 | 0x004657b0 | FUN_004657b0 | util | 1 | 6 | 0x0045d3a0 |  | re/analysis/timer_d3/0x004657b0.md |
| 559 | 0x00467010 | FUN_00467010 | boot | 1 | 6 | 0x00402a40 |  | re/analysis/skeleton_prep_render/00467010.md |
| 560 | 0x00467020 | FUN_00467020 | boot | 1 | 6 | 0x00402a40 |  | re/analysis/skeleton_prep_render/00467020.md |
| 561 | 0x00467070 | FUN_00467070 | boot | 1 | 6 | 0x00402a40 |  | re/analysis/skeleton_prep_render/00467070.md |
| 562 | 0x0046d400 | FUN_0046d400 | render | 1 | 6 | 0x004704c0 |  | re/analysis/util_c0_promote/0x0046d400.md |
| 563 | 0x0046f6c0 | VehicleWheelContactSolver | vehicle | 1 | 6 | 0x004704c0 |  | re/analysis/vehicle_update_d3/0046f6c0.md |
| 564 | 0x00471490 | FUN_00471490 | gameplay | 1 | 6 | 0x00471560 |  | re/analysis/bucket_gameplay_00471430_0047b6b0/0x00471490.md |
| 565 | 0x004715a0 | FUN_004715a0 | ai | 1 | 6 | 0x00426e10 |  | re/analysis/bucket_ai_00452eb0_004c3df0/004715a0.md |
| 566 | 0x004728a0 | FUN_004728a0 | util | 1 | 6 | 0x0045d7a0 |  | re/analysis/timer_d3_cont1_b/0x004728a0.md |
| 567 | 0x00474e70 | FUN_00474e70 | render | 1 | 6 | 0x00459480 |  | re/analysis/bucket_00474d80/0x00474e70.md |
| 568 | 0x004764f0 | FUN_004764f0 | util | 1 | 6 | 0x00476710 |  | re/analysis/random_rng_d2/0x004764f0.md |
| 569 | 0x00478660 | FUN_00478660 | ai | 1 | 6 | 0x00426e10 |  | re/analysis/bucket_ai_00452eb0_004c3df0/00478660.md |
| 570 | 0x00479330 | FUN_00479330 | track | 1 | 6 | 0x00426e10 |  | re/analysis/bucket_track_00401630_0047c0f0/00479330.md |
| 571 | 0x0047ea60 | FUN_0047ea60 | vehicle | 1 | 6 | 0x004704c0 |  | re/analysis/bucket_vehicle_00453f30_00482030/0047ea60.md |
| 572 | 0x00480340 | FUN_00480340 | render | 1 | 6 | 0x00426e10 |  | re/analysis/track_loader_d2/00480340.md |
| 573 | 0x00484130 | FUN_00484130 | boot | 1 | 6 | 0x00402a40 |  | re/analysis/skeleton_prep_render/00484130.md |
| 574 | 0x004864f0 | FUN_004864f0 | particle | 1 | 6 | 0x00486610 |  | re/analysis/particle_promote_ac1/0x004864f0.md |
| 575 | 0x00489250 | FUN_00489250 | boot | 1 | 6 | 0x00402a40 |  | re/analysis/boot_app_init_d5/0x00489250.md |
| 576 | 0x00494bc0 | FUN_00494bc0 | boot | 1 | 6 | 0x00402a40 |  | re/analysis/boot_app_init_d5/0x00494bc0.md |
| 577 | 0x00495870 | FUN_00495870 | input | 1 | 6 | 0x00495fe0 |  | re/analysis/bucket_input_dinput_0047b860_0049b300/00495870.md |
| 578 | 0x00496970 | FUN_00496970 | render | 1 | 6 | 0x00402a40 |  | re/analysis/skeleton_prep_boot_winmain_b/00496970.md |
| 579 | 0x00496ce0 | FUN_00496ce0 | boot | 1 | 6 | 0x00402a40 |  | re/analysis/boot_app_init_d5/0x00496ce0.md |
| 580 | 0x00497310 | FUN_00497310 | input | 1 | 6 | 0x00496530 |  | re/analysis/promote_c2_video_cfg/00497310.md |
| 581 | 0x0049cfb0 | FUN_0049cfb0 | util | 1 | 6 | 0x0049ec10 |  | re/analysis/timer/0x0049cfb0.md |
| 582 | 0x0049dd60 | FUN_0049dd60 | video | 1 | 6 | 0x0049ec10 |  | re/analysis/bucket_sky_worldobj_dbg_video_hud_00484280_00555910/0049dd60.md |
| 583 | 0x004b3c60 | FUN_004b3c60 | render | 1 | 6 | 0x0042a640 |  | re/analysis/track_loader_d4/0x004b3c60.md |
| 584 | 0x004b5350 | thunk_FUN_00550b00 | render | 1 | 6 | 0x0042a470 |  | re/analysis/bucket_004b4a80/0x004b5350.md |
| 585 | 0x004b7140 | FUN_004b7140 | input | 1 | 6 | 0x004c0510 |  | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004b7140.md |
| 586 | 0x004b7a70 | FUN_004b7a70 | input | 1 | 6 | 0x0047b8a0 |  | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004b7a70.md |
| 587 | 0x004b9850 | FUN_004b9850 | input | 1 | 6 | 0x004b7480 |  | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004b9850.md |
| 588 | 0x004ba210 | FUN_004ba210 | input | 1 | 6 | 0x004b7480 |  | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004ba210.md |
| 589 | 0x004c06c0 | FUN_004c06c0 | input | 1 | 6 | 0x004c0510 |  | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004c06c0.md |
| 590 | 0x004c1b10 | FUN_004c1b10 | render | 1 | 6 | 0x00426e10 |  | re/analysis/track_loader_d2/004c1b10.md |
| 591 | 0x005572c0 | FUN_005572c0 | hud | 1 | 6 | 0x00557110 |  | re/analysis/bucket_gameplay_004e4800_00558030/005572c0.md |
| 592 | 0x005581f0 | FUN_005581f0 | boot | 1 | 6 | 0x00402a40 |  | re/analysis/skeleton_prep_high_leverage/005581f0.md |
| 593 | 0x005a6b70 | FUN_005a6b70 | audio | 1 | 6 | 0x005a71f0 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a6b70.md |
| 594 | 0x005a6f30 | FUN_005a6f30 | audio | 1 | 6 | 0x005a60b0 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a6f30.md |
| 595 | 0x005adf60 | FUN_005adf60 | audio | 1 | 6 | 0x005a7aa0 |  | re/analysis/bucket_audio_005ab710_005af040/0x005adf60.md |
| 596 | 0x005af430 | FUN_005af430 | audio | 1 | 6 | 0x005b1110 |  | re/analysis/bucket_audio_005af070_005b2190/0x005af430.md |
| 597 | 0x005b0360 | FUN_005b0360 | audio | 1 | 6 | 0x005afcf0 |  | re/analysis/bucket_audio_005af070_005b2190/0x005b0360.md |
| 598 | 0x005b04b0 | FUN_005b04b0 | audio | 1 | 6 | 0x005afcf0 |  | re/analysis/bucket_audio_005af070_005b2190/0x005b04b0.md |
| 599 | 0x005b0970 | FUN_005b0970 | audio | 1 | 6 | 0x005afcf0 |  | re/analysis/bucket_audio_005af070_005b2190/0x005b0970.md |
| 600 | 0x005b0b10 | FUN_005b0b10 | audio | 1 | 6 | 0x005afcf0 |  | re/analysis/bucket_audio_005af070_005b2190/0x005b0b10.md |
| 601 | 0x005b10d0 | thunk_FUN_005b10a0 | audio | 1 | 6 | 0x005b10a0 |  | re/analysis/bucket_audio_005af070_005b2190/0x005b10d0.md |
| 602 | 0x0040cf40 | FUN_0040cf40 | gameplay | 1 | 7 | 0x0040cf80 | STUB | re/analysis/bucket_gameplay_0040ad90_00412010/0x0040cf40.md |
| 603 | 0x0041a980 | FUN_0041a980 | render | 1 | 7 | 0x0040cf80 | STUB | re/analysis/bucket_gameplay_0041a980_0041d910/0041a980.md |
| 604 | 0x0041cb00 | FUN_0041cb00 | render | 1 | 7 | 0x0041ccf0 |  | re/analysis/bucket_gameplay_0041a980_0041d910/0041cb00.md |
| 605 | 0x00422140 | FUN_00422140 | render | 1 | 7 | 0x0040cf80 | STUB | re/analysis/bucket_gameplay_00421100_004223f0/0x00422140.md |
| 606 | 0x00425c00 | FUN_00425c00 | frontend | 1 | 7 | 0x004262f0 | STUB | re/analysis/frontend_c1_to_c2_s3/FUN_00425c00.md |
| 607 | 0x00425e40 | FUN_00425e40 | render | 1 | 7 | 0x00425ab0 |  | re/analysis/promote_c2_render_lowrva/00425e40.md |
| 608 | 0x00426700 | FUN_00426700 | render | 1 | 7 | 0x00426ab0 |  | re/analysis/render_promote_c2_track_loader/0x00426700.md |
| 609 | 0x00426780 | FUN_00426780 | render | 1 | 7 | 0x00426ab0 |  | re/analysis/render_promote_c2_track_loader/0x00426780.md |
| 610 | 0x00426810 | sub_00426810 | render | 1 | 7 | 0x00426ab0 |  | re/analysis/render_promote_c2_track_loader/0x00426810.md |
| 611 | 0x00426b40 | FUN_00426b40 | frontend | 1 | 7 | 0x0040cfd0 | STUB | re/analysis/frontend_c1_to_c2_s4/FUN_00426b40.md |
| 612 | 0x0045e040 | FUN_0045e040 | audio | 1 | 7 | 0x00467010 |  | re/analysis/bucket_audio_0042f760_00465b20/0045e040.md |
| 613 | 0x0046b1c0 | FUN_0046b1c0 | vehicle | 1 | 7 | 0x0040e590 |  | re/analysis/bucket_vehicle_00453f30_00482030/0046b1c0.md |
| 614 | 0x00474890 | FUN_00474890 | gameplay | 1 | 7 | 0x0042e5b0 | STUB | re/analysis/promote_c1_mid_ab2/0x00474890.md |
| 615 | 0x004783f0 | FUN_004783f0 | ai | 1 | 7 | 0x00478660 |  | re/analysis/bucket_ai_00452eb0_004c3df0/004783f0.md |
| 616 | 0x004785e0 | FUN_004785e0 | util | 1 | 7 | 0x00478660 |  | re/analysis/track_loader_d3/004785e0.md |
| 617 | 0x004790e0 | FUN_004790e0 | track | 1 | 7 | 0x00479330 |  | re/analysis/bucket_track_00401630_0047c0f0/004790e0.md |
| 618 | 0x0047bf70 | FUN_0047bf70 | track | 1 | 7 | 0x0047c0f0 |  | re/analysis/bucket_track_00401630_0047c0f0/0047bf70.md |
| 619 | 0x00486370 | FUN_00486370 | particle | 1 | 7 | 0x004864f0 |  | re/analysis/particle_promote_ac1/0x00486370.md |
| 620 | 0x0048f740 | FUN_0048f740 | util | 1 | 7 | 0x0040e590 |  | re/analysis/game_state_d5_cont2/0x0048f740.md |
| 621 | 0x00490000 | FUN_00490000 | render | 1 | 7 | 0x0040cfd0 | STUB | re/analysis/breadth_unmapped_0049x/0x00490000.md |
| 622 | 0x00491590 | FUN_00491590 | render | 1 | 7 | 0x00491780 |  | re/analysis/track_loader_d3/00491590.md |
| 623 | 0x00496040 | FUN_00496040 | input | 1 | 7 | 0x00495790 |  | re/analysis/bucket_input_dinput_0047b860_0049b300/00496040.md |
| 624 | 0x004b5030 | FUN_004b5030 | render | 1 | 7 | 0x00478660 |  | re/analysis/track_loader_d3/004b5030.md |
| 625 | 0x004b7960 | FUN_004b7960 | input | 1 | 7 | 0x004b7a70 |  | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004b7960.md |
| 626 | 0x004b7aa0 | FUN_004b7aa0 | input | 1 | 7 | 0x004b7a70 |  | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004b7aa0.md |
| 627 | 0x004b7ff0 | FUN_004b7ff0 | input | 1 | 7 | 0x004b7fd0 |  | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004b7ff0.md |
| 628 | 0x004b8340 | FUN_004b8340 | input | 1 | 7 | 0x004b7250 |  | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004b8340.md |
| 629 | 0x004b9aa0 | FUN_004b9aa0 | input | 1 | 7 | 0x004b7250 |  | re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004b9aa0.md |
| 630 | 0x004c1a70 | FUN_004c1a70 | render | 1 | 7 | 0x004c1b10 |  | re/analysis/fun_00426810_callees/0x004c1a70.md |
| 631 | 0x004d41e0 | FUN_004d41e0 | render | 1 | 7 | 0x00496ce0 |  | re/analysis/render_5_c1_to_c2_s6/004d41e0.md |
| 632 | 0x00555830 | FUN_00555830 | hud | 1 | 7 | 0x00427620 |  | re/analysis/bucket_gameplay_004e4800_00558030/00555830.md |
| 633 | 0x00556e40 | FUN_00556e40 | hud | 1 | 7 | 0x00427620 |  | re/analysis/bucket_gameplay_004e4800_00558030/00556e40.md |
| 634 | 0x005a6070 | FUN_005a6070 | audio | 1 | 7 | 0x005a6030 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a6070.md |
| 635 | 0x005a6d90 | FUN_005a6d90 | audio | 1 | 7 | 0x005a6e10 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a6d90.md |
| 636 | 0x005ab040 | FUN_005ab040 | util | 1 | 7 | 0x005a6ea0 |  | re/analysis/timer/0x005ab040.md |
| 637 | 0x005acda0 | FUN_005acda0 | audio | 1 | 7 | 0x005a6f30 |  | re/analysis/bucket_audio_005ab710_005af040/0x005acda0.md |
| 638 | 0x005b03e0 | FUN_005b03e0 | audio | 1 | 7 | 0x005b0360 |  | re/analysis/bucket_audio_005af070_005b2190/0x005b03e0.md |
| 639 | 0x00404fa0 | FUN_00404fa0 | render | 1 | 8 | 0x00405460 |  | re/analysis/promote_c2_render_lowrva/00404fa0.md |
| 640 | 0x00407800 | FUN_00407800 | gameplay | 1 | 8 | 0x0047cdc0 |  | re/analysis/bucket_gameplay_00407640_0040ad40/00407800.md |
| 641 | 0x0040ad40 | FUN_0040ad40 | gameplay | 1 | 8 | 0x0040b090 |  | re/analysis/bucket_gameplay_00407640_0040ad40/0040ad40.md |
| 642 | 0x0040ad90 | FUN_0040ad90 | gameplay | 1 | 8 | 0x0040b090 |  | re/analysis/bucket_gameplay_0040ad90_00412010/0x0040ad90.md |
| 643 | 0x00421c80 | FUN_00421c80 | render | 1 | 8 | 0x00422140 |  | re/analysis/bucket_gameplay_00421100_004223f0/0x00421c80.md |
| 644 | 0x004220d0 | FUN_004220d0 | render | 1 | 8 | 0x00422140 |  | re/analysis/bucket_gameplay_00421100_004223f0/0x004220d0.md |
| 645 | 0x004414b0 | FUN_004414b0 | camera | 1 | 8 | 0x0040d470 |  | re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x004414b0.md |
| 646 | 0x0044c4f0 | FUN_0044c4f0 | render | 1 | 8 | 0x0047cdc0 |  | re/analysis/bucket_gameplay_00422440_0044e070/0x0044c4f0.md |
| 647 | 0x004733b0 | FUN_004733b0 | render | 1 | 8 | 0x00473ee0 |  | re/analysis/bucket_gameplay_00471430_0047b6b0/0x004733b0.md |
| 648 | 0x00475010 | FUN_00475010 | render | 1 | 8 | 0x00426780 | STUB | re/analysis/bucket_00474d80/0x00475010.md |
| 649 | 0x004756e0 | FUN_004756e0 | render | 1 | 8 | 0x00426700 | STUB | re/analysis/render_c1_to_c2_s5/FUN_004756e0.md |
| 650 | 0x0047bcc0 | FUN_0047bcc0 | track | 1 | 8 | 0x0047bf70 |  | re/analysis/bucket_track_00401630_0047c0f0/0047bcc0.md |
| 651 | 0x0047be80 | FUN_0047be80 | track | 1 | 8 | 0x0047bf70 |  | re/analysis/bucket_track_00401630_0047c0f0/0047be80.md |
| 652 | 0x0047f940 | FUN_0047f940 | physics | 1 | 8 | 0x0047ce40 |  | re/analysis/bucket_physics_smplfzx_00478cb0_0057c4b0/0x0047f940.md |
| 653 | 0x004910c0 | FUN_004910c0 | render | 1 | 8 | 0x00491490 |  | re/analysis/breadth_unmapped_0049x/0x004910c0.md |
| 654 | 0x00491340 | FUN_00491340 | render | 1 | 8 | 0x00491490 |  | re/analysis/breadth_unmapped_0049x/0x00491340.md |
| 655 | 0x004b53b0 | FUN_004b53b0 | render | 1 | 8 | 0x0047bf70 |  | re/analysis/render_3_c1_to_c2_s5/FUN_004b53b0.md |
| 656 | 0x00554050 | FontCanvas_Shutdown | hud | 1 | 8 | 0x00552b90 |  | re/analysis/font_sys_promote_ae3/0x00554050.md |
| 657 | 0x00555280 | FontSys_ShutdownContextPool | hud | 1 | 8 | 0x00552b90 |  | re/analysis/font_sys_promote_ae3/0x00555280.md |
| 658 | 0x005a73b0 | FUN_005a73b0 | audio | 1 | 8 | 0x00466a50 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a73b0.md |
| 659 | 0x005a79a0 | FUN_005a79a0 | audio | 1 | 8 | 0x00466a50 |  | re/analysis/bucket_audio_00465c10_005a7b50/005a79a0.md |
| 660 | 0x005abcf0 | FUN_005abcf0 | audio | 1 | 8 | 0x00466a50 |  | re/analysis/promote_c2_audio_rws/005abcf0.md |
| 661 | 0x005ad320 | FUN_005ad320 | audio | 1 | 8 | 0x005ad0b0 |  | re/analysis/bucket_audio_005ab710_005af040/0x005ad320.md |
| 662 | 0x00405920 | FUN_00405920 | gameplay | 1 | 9 | 0x00405540 |  | re/analysis/bucket_gameplay_00405400_00407620/0x00405920.md |
| 663 | 0x004069d0 | FUN_004069d0 | gameplay | 1 | 9 | 0x00405540 |  | re/analysis/bucket_gameplay_00405400_00407620/0x004069d0.md |
| 664 | 0x004073b0 | FUN_004073b0 | util | 1 | 9 | 0x00407800 |  | re/analysis/timer_d3_cont1_a/0x004073b0.md |
| 665 | 0x00421ba0 | FUN_00421ba0 | render | 1 | 9 | 0x004220d0 |  | re/analysis/bucket_gameplay_00421100_004223f0/0x00421ba0.md |
| 666 | 0x00428d30 | FUN_00428d30 | frontend | 1 | 9 | 0x00473220 |  | re/analysis/frontend_c1_to_c2_s6/FUN_00428d30.md |
| 667 | 0x0044c8b0 | FUN_0044c8b0 | render | 1 | 9 | 0x0044c4f0 |  | re/analysis/bucket_gameplay_00422440_0044e070/0x0044c8b0.md |
| 668 | 0x00454f80 | FUN_00454f80 | render | 1 | 9 | 0x00407800 |  | re/analysis/bucket_gameplay_00454130_00455fe0/00454f80.md |
| 669 | 0x004728f0 | FUN_004728f0 | gameplay | 1 | 9 | 0x0044c4f0 |  | re/analysis/bucket_gameplay_00471430_0047b6b0/0x004728f0.md |
| 670 | 0x00474fb0 | FUN_00474fb0 | render | 1 | 9 | 0x00475010 |  | re/analysis/track_loader_d4/0x00474fb0.md |
| 671 | 0x004752f0 | FUN_004752f0 | render | 1 | 9 | 0x004756e0 |  | re/analysis/bucket_00474d80/0x004752f0.md |
| 672 | 0x00475770 | FUN_00475770 | render | 1 | 9 | 0x00405540 |  | re/analysis/render_c1_to_c2_s5/FUN_00475770.md |
| 673 | 0x00477e60 | FUN_00477e60 | render | 1 | 9 | 0x0047bb10 |  | re/analysis/render_2_c1_to_c2_s1/FUN_00477e60.md |
| 674 | 0x00477f00 | FUN_00477f00 | render | 1 | 9 | 0x0047bb10 |  | re/analysis/render_2_c1_to_c2_s1/FUN_00477f00.md |
| 675 | 0x0047ba20 | FUN_0047ba20 | gameplay | 1 | 9 | 0x0047bb10 |  | re/analysis/bucket_gameplay_0047ba20_0047f380/0047ba20.md |
| 676 | 0x0047cf20 | FUN_0047cf20 | gameplay | 1 | 9 | 0x00407800 |  | re/analysis/bucket_gameplay_0047ba20_0047f380/0047cf20.md |
| 677 | 0x00556d20 | FontSys_InitBuffers | hud | 1 | 9 | 0x00556ce0 |  | re/analysis/font_atlas_promote_ae5/0x00556d20.md |
| 678 | 0x00557250 | FontSys_InitDataPools | hud | 1 | 9 | 0x00557220 |  | re/analysis/font_pools_frontend_ae6/0x00557250.md |
| 679 | 0x005a7e70 | FUN_005a7e70 | audio | 1 | 9 | 0x005abcf0 |  | re/analysis/bucket_audio_005a7b60_005ab620/005a7e70.md |
| 680 | 0x00421630 | FUN_00421630 | render | 1 | 10 | 0x00405ab0 |  | re/analysis/bucket_gameplay_00421100_004223f0/0x00421630.md |
| 681 | 0x0042c010 | FUN_0042c010 | render | 1 | 10 | 0x00472b10 |  | re/analysis/render_frame_d4/0x0042c010.md |
| 682 | 0x00441d40 | FUN_00441d40 | util | 1 | 10 | 0x00441c80 |  | re/analysis/game_state_d4/0x00441d40.md |
| 683 | 0x00456140 | FUN_00456140 | gameplay | 1 | 10 | 0x00405ab0 |  | re/analysis/bucket_gameplay_00456040_004588c0/0x00456140.md |
| 684 | 0x00465ca0 | FUN_00465ca0 | audio | 1 | 10 | 0x00405ab0 | STUB | re/analysis/bucket_audio_00465c10_005a7b50/00465ca0.md |
| 685 | 0x00476320 | FUN_00476320 | render | 1 | 10 | 0x00405ab0 | STUB | re/analysis/render_c1_to_c2_s5/FUN_00476320.md |
| 686 | 0x0047f290 | FUN_0047f290 | gameplay | 1 | 10 | 0x0047d130 |  | re/analysis/bucket_gameplay_0047ba20_0047f380/0047f290.md |
| 687 | 0x0047f380 | FUN_0047f380 | gameplay | 1 | 10 | 0x0047d130 |  | re/analysis/bucket_gameplay_0047ba20_0047f380/0047f380.md |
| 688 | 0x0048aa20 | FUN_0048aa20 | render | 1 | 10 | 0x00405ab0 |  | re/analysis/ai_update_d6/0x0048aa20.md |
| 689 | 0x004046a0 | FUN_004046a0 | util | 1 | 11 | 0x004173a0 |  | re/analysis/timer_d3_cont1_a/0x004046a0.md |
| 690 | 0x00406ae0 | FUN_00406ae0 | gameplay | 1 | 11 | 0x004173a0 |  | re/analysis/bucket_gameplay_00405400_00407620/0x00406ae0.md |
| 691 | 0x004072e0 | FUN_004072e0 | render | 1 | 11 | 0x00406130 |  | re/analysis/promote_c2_render_lowrva/004072e0.md |
| 692 | 0x00417750 | FUN_00417750 | gameplay | 1 | 11 | 0x004173a0 |  | re/analysis/bucket_gameplay_00412100_00418e50/0x00417750.md |
| 693 | 0x0041f220 | FUN_0041f220 | gameplay | 1 | 11 | 0x00421630 |  | re/analysis/bucket_gameplay_0041f060_004210f0/0x0041f220.md |
| 694 | 0x004215e0 | FUN_004215e0 | render | 1 | 11 | 0x00421630 |  | re/analysis/bucket_gameplay_00421100_004223f0/0x004215e0.md |
| 695 | 0x00456040 | FUN_00456040 | gameplay | 1 | 11 | 0x00456140 |  | re/analysis/bucket_gameplay_00456040_004588c0/0x00456040.md |
| 696 | 0x00456c30 | FUN_00456c30 | gameplay | 1 | 11 | 0x00456140 |  | re/analysis/bucket_gameplay_00456040_004588c0/0x00456c30.md |
| 697 | 0x00489c30 | FUN_00489c30 | particle | 1 | 11 | 0x00406130 |  | re/analysis/particle_promote_ac3/0x00489c30.md |
| 698 | 0x0048a850 | FUN_0048a850 | particle | 1 | 11 | 0x0048aa20 |  | re/analysis/particle_promote_ac4/0x0048a850.md |
| 699 | 0x0041f590 | FUN_0041f590 | gameplay | 1 | 12 | 0x0041f220 |  | re/analysis/bucket_gameplay_0041f060_004210f0/0x0041f590.md |
| 700 | 0x0041fe10 | FUN_0041fe10 | gameplay | 1 | 12 | 0x0041f220 |  | re/analysis/bucket_gameplay_0041f060_004210f0/0x0041fe10.md |
| 701 | 0x00453eb0 | FUN_00453eb0 | gameplay | 1 | 12 | 0x00406160 |  | re/analysis/bucket_gameplay_0044e190_00453eb0/0x00453eb0.md |
| 702 | 0x004595c0 | FUN_004595c0 | gameplay | 1 | 12 | 0x00456040 |  | re/analysis/bucket_gameplay_00458a40_0045ac40/0x004595c0.md |
| 703 | 0x0045b990 | FUN_0045b990 | render | 1 | 12 | 0x00456c30 |  | re/analysis/render_c1_to_c2_s3/FUN_0045b990.md |
| 704 | 0x0045dce0 | FUN_0045dce0 | audio | 1 | 12 | 0x00406160 |  | re/analysis/bucket_gameplay_0045ae80_0045dd50/0045dce0.md |
| 705 | 0x00489a40 | FUN_00489a40 | particle | 1 | 12 | 0x00406160 |  | re/analysis/particle_promote_ac3/0x00489a40.md |
| 706 | 0x004b4d10 | FUN_004b4d10 | render | 1 | 12 | 0x00406160 |  | re/analysis/bucket_004b4a80/0x004b4d10.md |
| 707 | 0x004b59c0 | FUN_004b59c0 | render | 1 | 12 | 0x004072e0 |  | re/analysis/render_3_c1_to_c2_s5/FUN_004b59c0.md |
| 708 | 0x00419a00 | FUN_00419a00 | gameplay | 1 | 13 | 0x0045dce0 |  | re/analysis/bucket_gameplay_00418e70_0041a8d0/0x00419a00.md |
| 709 | 0x00452f30 | FUN_00452f30 | gameplay | 1 | 13 | 0x00453eb0 |  | re/analysis/bucket_gameplay_0044e190_00453eb0/0x00452f30.md |
| 710 | 0x00456c90 | thunk_FUN_00456c70 | gameplay | 1 | 13 | 0x00456c70 |  | re/analysis/bucket_gameplay_00456040_004588c0/0x00456c90.md |
| 711 | 0x0045a130 | FUN_0045a130 | gameplay | 1 | 13 | 0x0045b990 |  | re/analysis/bucket_gameplay_00458a40_0045ac40/0x0045a130.md |
| 712 | 0x00465940 | FUN_00465940 | audio | 1 | 13 | 0x00453eb0 |  | re/analysis/bucket_audio_0042f760_00465b20/00465940.md |
| 713 | 0x00466000 | FUN_00466000 | audio | 1 | 13 | 0x0045dce0 |  | re/analysis/bucket_gameplay_0045dff0_0046dd90/0x00466000.md |
| 714 | 0x00487020 | FUN_00487020 | render | 1 | 13 | 0x00453eb0 |  | re/analysis/ai_update_d6/0x00487020.md |
| 715 | 0x004b4ad0 | FUN_004b4ad0 | render | 1 | 13 | 0x004b4d10 |  | re/analysis/render_3_c1_to_c2_s3/FUN_004b4ad0.md |
| 716 | 0x004b4c80 | FUN_004b4c80 | render | 1 | 13 | 0x004b4d10 |  | re/analysis/render_3_c1_to_c2_s3/FUN_004b4c80.md |
| 717 | 0x004cd070 | RwRenderPrimitiveSubmit | render | 1 | 13 | 0x004b59c0 |  | re/analysis/promote_c2_rw_render_submit/004cd070.md |
| 718 | 0x004cd430 | FUN_004cd430 | render | 1 | 13 | 0x004b59c0 |  | re/analysis/render_5_c1_to_c2_s1/FUN_004cd430.md |
| 719 | 0x00452ec0 | FUN_00452ec0 | gameplay | 1 | 14 | 0x00452f30 |  | re/analysis/bucket_gameplay_0044e190_00453eb0/0x00452ec0.md |
| 720 | 0x00452f00 | FUN_00452f00 | gameplay | 1 | 14 | 0x00452f30 |  | re/analysis/bucket_gameplay_0044e190_00453eb0/0x00452f00.md |
| 721 | 0x004d40d0 | FUN_004d40d0 | render | 1 | 14 | 0x004cd070 |  | re/analysis/render_5_c1_to_c2_s6/004d40d0.md |
| 722 | 0x00409790 | FUN_00409790 | gameplay | 0 | 0 | (root) |  | re/analysis/bucket_gameplay_00407640_0040ad40/00409790.md |
| 723 | 0x0040d110 | FUN_0040d110 | track | 0 | 0 | (root) |  | re/analysis/bucket_track_00401630_0047c0f0/0040d110.md |
| 724 | 0x0042fab0 | FUN_0042fab0 | frontend | 0 | 0 | (root) |  | re/analysis/bucket_0041dc30/0x0042fab0.md |
| 725 | 0x00430670 | FUN_00430670 | frontend | 0 | 0 | (root) |  | re/analysis/hud_frontend_d5/0x00430670.md |
| 726 | 0x0043dfd0 | FUN_0043dfd0 | frontend | 0 | 0 | (root) | STUB | re/analysis/timer_d2/0x0043dfd0.md |
| 727 | 0x00441760 | Camera::Apply | util | 0 | 0 | (root) |  | re/analysis/profile_career_d4/REPORT.md |
| 728 | 0x00441820 | CameraPath::SamplePoint | util | 0 | 0 | (root) |  | re/analysis/profile_career_d4/REPORT.md |
| 729 | 0x00442a60 | Spectator::ComputeDistances | util | 0 | 0 | (root) |  | re/analysis/profile_career_d4/REPORT.md |
| 730 | 0x004a2cbd | FID_conflict:_wprintf | render | 0 | 0 | (root) |  | re/analysis/rw_engine_init/004a2cbd.md |

## 4. C1/C0 holes inside the closure

The closure contains 6 C1 members and ALL of them are library-classified (excluded from gap ranking): 0x004e6e00 RpClumpDestroy (third-party-library[renderware]); 0x00534870 FUN_00534870 (third-party-library[renderware]); 0x00552920 FUN_00552920 (third-party-library[renderware]); 0x00553cf0 FUN_00553cf0 (third-party-library[renderware]); 0x00553e80 FUN_00553e80 (third-party-library[renderware]); 0x00553ef0 FUN_00553ef0 (third-party-library[renderware]). The non-library C1 table below is therefore empty.

### C1 closure members (non-library), same ranking — 0 rows

| rank | RVA | name | subsystem | in-degree (static proxy) | depth | first caller | stub? | hooks.csv file |
|---|---|---|---|---|---|---|---|---|

hooks.csv carries no C0 rows; there are no closure members with a hooks.csv confidence below C1.

### Closure members with NO hooks.csv row — 1788 RVAs

[UNCERTAIN] These addresses appear in `callees` columns of `re/analysis/plans/*.tsv` but have no hooks.csv row; the callees columns mix function targets with data addresses read at call sites, and no artifact in the inputs distinguishes the two per address. Missing evidence: a Ghidra function-existence check per address. Sorted by in-degree DESC then RVA.

| address/value (raw callee entry) | depth | in-degree (static proxy) | first caller |
|---|---|---|---|
| 0x007d3ff8 | 1 | 53 | 0x0043dfd0 |
| 0x005d757c | 1 | 48 | 0x004148b0 |
| 0x005cc320 | 1 | 43 | 0x00414570 |
| 0x005cc32c | 1 | 20 | 0x00414c30 |
| 0x0063d7e4 | 3 | 13 | 0x0041e140 |
| 0x00616038 | 2 | 12 | 0x00495280 |
| 0x007f0fd0 | 1 | 12 | 0x00405890 |
| 0x005cc31c | 1 | 11 | 0x00410510 |
| 0x005cc33c | 1 | 11 | 0x00414570 |
| 0x005ccac4 | 1 | 10 | 0x00414570 |
| 0x0068f640 | 3 | 10 | 0x0045e0f0 |
| 0x007f1a14 | 1 | 10 | 0x00415220 |
| 0x007f1008 | 1 | 9 | 0x004148b0 |
| 0x005cc9a0 | 1 | 8 | 0x00443dc0 |
| 0x0063ba8c | 1 | 8 | 0x0040e350 |
| 0x005cc56c | 1 | 7 | 0x00414570 |
| 0x005f9bd8 | 1 | 7 | 0x0045bed0 |
| 0x00604570 | 3 | 7 | 0x004661a0 |
| 0x0068f4c8 | 5 | 7 | 0x004627f0 |
| 0x005cc35c | 1 | 6 | 0x004148b0 |
| 0x005cc560 | 3 | 6 | 0x00472c60 |
| 0x005cc564 | 1 | 6 | 0x00414f00 |
| 0x005cc72c | 1 | 6 | 0x00414570 |
| 0x005cc948 | 1 | 6 | 0x004177b0 |
| 0x005ce6b8 | 4 | 6 | 0x004633d0 |
| 0x0069049c | 1 | 6 | 0x00462dd0 |
| 0x007f0ff8 | 1 | 6 | 0x00416250 |
| 0x007f1a1c | 1 | 6 | 0x0040d110 |
| 0x007f1a9c | 1 | 6 | 0x004150e0 |
| 0x005cc328 | 1 | 5 | 0x004177b0 |
| 0x005cc55c | 1 | 5 | 0x00415220 |
| 0x005cc568 | 1 | 5 | 0x0040d590 |
| 0x005cc950 | 1 | 5 | 0x00414c30 |
| 0x005cc99c | 5 | 5 | 0x0045a190 |
| 0x005cd09c | 1 | 5 | 0x00414570 |
| 0x005cd5a8 | 3 | 5 | 0x00472c60 |
| 0x006146fc | 3 | 5 | 0x004039f0 |
| 0x00639d78 | 7 | 5 | 0x004053d0 |
| 0x0063a5d0 | 1 | 5 | 0x00405890 |
| 0x0067e9fc | 1 | 5 | 0x00410510 |
| 0x00771a04 | 2 | 5 | 0x00493f70 |
| 0x00772fac | 6 | 5 | 0x00495fe0 |
| 0x007dc57c | 4 | 5 | 0x00476c10 |
| 0x007f100c | 3 | 5 | 0x004568d0 |
| 0x007f1030 | 1 | 5 | 0x0040b290 |
| 0x007f103c | 1 | 5 | 0x00415220 |
| 0x007f9a9c | 1 | 5 | 0x004150e0 |
| 0xffffffff | 2 | 5 | 0x0040dd60 |
| 0x00440bc0 | 3 | 4 | 0x0045bae0 |
| 0x005cc348 | 1 | 4 | 0x00414a70 |
| 0x005cc558 | 2 | 4 | 0x00414300 |
| 0x005cc730 | 1 | 4 | 0x0040d590 |
| 0x005cc9a4 | 1 | 4 | 0x004177b0 |
| 0x005cc9c0 | 1 | 4 | 0x00415880 |
| 0x005cc9fc | 1 | 4 | 0x00414570 |
| 0x005ccad0 | 1 | 4 | 0x00417640 |
| 0x005ccae0 | 1 | 4 | 0x0046d570 |
| 0x005ccd6c | 1 | 4 | 0x0040d590 |
| 0x005cd050 | 1 | 4 | 0x00443440 |
| 0x005cd0ec | 1 | 4 | 0x00417cf0 |
| 0x005cf9d0 | 5 | 4 | 0x00494ac0 |
| 0x005f29b8 | 1 | 4 | 0x0040e4a0 |
| 0x005f9998 | 1 | 4 | 0x0045bed0 |
| 0x00639d70 | 7 | 4 | 0x004053d0 |
| 0x00639d74 | 7 | 4 | 0x004053d0 |
| 0x0067e9f8 | 1 | 4 | 0x0043dfd0 |
| 0x0068b9b8 | 3 | 4 | 0x00459290 |
| 0x0069045c | 1 | 4 | 0x00462dd0 |
| 0x006904e8 | 4 | 4 | 0x004669b0 |
| 0x00771a18 | 2 | 4 | 0x00494460 |
| 0x007dc95c | 6 | 4 | 0x005a60b0 |
| 0x007e9de0 | 1 | 4 | 0x0040b290 |
| 0x007e9de4 | 1 | 4 | 0x0040b290 |
| 0x007f1a18 | 1 | 4 | 0x0040b290 |
| 0x005af2a0 | 5 | 3 | 0x005af260 |
| 0x005cc574 | 1 | 3 | 0x00410510 |
| 0x005cc728 | 2 | 3 | 0x004252c0 |
| 0x005cc8f0 | 1 | 3 | 0x004177b0 |
| 0x005cc94c | 8 | 3 | 0x00426700 |
| 0x005cc990 | 4 | 3 | 0x00458a40 |
| 0x005cc9b0 | 1 | 3 | 0x00415220 |
| 0x005cc9b8 | 1 | 3 | 0x00414570 |
| 0x005cc9e0 | 1 | 3 | 0x00415220 |
| 0x005cc9f4 | 1 | 3 | 0x00414c30 |
| 0x005ccac0 | 1 | 3 | 0x00414c30 |
| 0x005cd0c8 | 1 | 3 | 0x00415e20 |
| 0x005cd8f0 | 6 | 3 | 0x00431b50 |
| 0x005ce034 | 1 | 3 | 0x00443440 |
| 0x005ce18c | 5 | 3 | 0x0045ae80 |
| 0x005cfd30 | 5 | 3 | 0x00494ac0 |
| 0x005d0e74 | 5 | 3 | 0x00494ac0 |
| 0x005d0e88 | 5 | 3 | 0x00494ac0 |
| 0x005e6454 | 7 | 3 | 0x005ac650 |
| 0x005e64b4 | 7 | 3 | 0x005ac650 |
| 0x005f2770 | 1 | 3 | 0x0040e480 |
| 0x005f99ac | 3 | 3 | 0x0045bae0 |
| 0x006036c0 | 5 | 3 | 0x00466b50 |
| 0x00636ac8 | 3 | 3 | 0x00402750 |
| 0x00636b70 | 4 | 3 | 0x00403910 |
| 0x00636b78 | 4 | 3 | 0x00403640 |
| 0x0063a490 | 9 | 3 | 0x00407800 |
| 0x0063a5f0 | 1 | 3 | 0x00409790 |
| 0x0063b8f8 | 4 | 3 | 0x0040bb30 |
| 0x0063b8fc | 4 | 3 | 0x0040bb50 |
| 0x0063b910 | 1 | 3 | 0x00410510 |
| 0x0063cab8 | 4 | 3 | 0x0041bec0 |
| 0x0063d298 | 4 | 3 | 0x0041d6e0 |
| 0x0063e548 | 5 | 3 | 0x004210b0 |
| 0x00644158 | 5 | 3 | 0x0047a020 |
| 0x00644378 | 5 | 3 | 0x0047a020 |
| 0x0067ea64 | 1 | 3 | 0x00410510 |
| 0x0067eab0 | 2 | 3 | 0x0042bf30 |
| 0x0067ed3c | 1 | 3 | 0x0043dfd0 |
| 0x0067ed40 | 2 | 3 | 0x0042aa00 |
| 0x0067f17c | 1 | 3 | 0x00430670 |
| 0x006887d0 | 3 | 3 | 0x004568d0 |
| 0x0068b968 | 3 | 3 | 0x00459000 |
| 0x0068ba08 | 5 | 3 | 0x0045a190 |
| 0x0068bb58 | 3 | 3 | 0x00459290 |
| 0x0068f61c | 5 | 3 | 0x004627f0 |
| 0x006904a0 | 5 | 3 | 0x0045d460 |
| 0x006905b0 | 1 | 3 | 0x004671a0 |
| 0x00691500 | 4 | 3 | 0x004723d0 |
| 0x00691504 | 4 | 3 | 0x004723d0 |
| 0x00691604 | 5 | 3 | 0x00475a00 |
| 0x00691608 | 5 | 3 | 0x00475a00 |
| 0x00691610 | 5 | 3 | 0x00475a00 |
| 0x006bf1e0 | 5 | 3 | 0x0047b860 |
| 0x00766f00 | 5 | 3 | 0x0048eac0 |
| 0x0076a0b8 | 5 | 3 | 0x0048eac0 |
| 0x0076d900 | 5 | 3 | 0x0048eac0 |
| 0x007d4028 | 4 | 3 | 0x004c4dc0 |
| 0x007dca34 | 6 | 3 | 0x005a71f0 |
| 0x007dca38 | 6 | 3 | 0x005a71f0 |
| 0x007dca40 | 6 | 3 | 0x005a7460 |
| 0x007dca50 | 4 | 3 | 0x005a66d0 |
| 0x007e96fc | 6 | 3 | 0x00496530 |
| 0x007ea1e4 | 1 | 3 | 0x0040b290 |
| 0x007ea5e4 | 1 | 3 | 0x0040b290 |
| 0x007ea9e4 | 1 | 3 | 0x0040b290 |
| 0x007f0f60 | 4 | 3 | 0x004274d0 |
| 0x007f1038 | 1 | 3 | 0x00418560 |
| 0x007f1042 | 6 | 3 | 0x00496530 |
| 0x007f105c | 1 | 3 | 0x00404e80 |
| 0x007f1a50 | 1 | 3 | 0x00418560 |
| 0x004483a3 | 2 | 2 | 0x00448220 |
| 0x005a6010 | 6 | 2 | 0x005a5f00 |
| 0x005af670 | 5 | 2 | 0x005af600 |
| 0x005cc318 | 1 | 2 | 0x00415880 |
| 0x005cc4de | 3 | 2 | 0x00402b70 |
| 0x005cc750 | 11 | 2 | 0x00441d40 |
| 0x005cc970 | 1 | 2 | 0x00415e20 |
| 0x005cc98c | 8 | 2 | 0x004783f0 |
| 0x005cc9b4 | 9 | 2 | 0x0040aef0 |
| 0x005cc9c8 | 1 | 2 | 0x00410510 |
| 0x005ccac8 | 1 | 2 | 0x00414c30 |
| 0x005ccacc | 6 | 2 | 0x0046c7d0 |
| 0x005ccd04 | 1 | 2 | 0x00415880 |
| 0x005ccdf4 | 6 | 2 | 0x00462f50 |
| 0x005cd058 | 8 | 2 | 0x00474890 |
| 0x005cd088 | 1 | 2 | 0x00415020 |
| 0x005cd094 | 1 | 2 | 0x00414570 |
| 0x005cd098 | 1 | 2 | 0x00414570 |
| 0x005cd0ac | 1 | 2 | 0x00414a70 |
| 0x005cd0b0 | 1 | 2 | 0x00414c30 |
| 0x005cd0b4 | 1 | 2 | 0x00414c30 |
| 0x005cd0b8 | 1 | 2 | 0x00415220 |
| 0x005cd0d8 | 1 | 2 | 0x0046d6a0 |
| 0x005cd0dc | 1 | 2 | 0x0046d6a0 |
| 0x005cd0e0 | 1 | 2 | 0x00416250 |
| 0x005cd0e8 | 6 | 2 | 0x00459480 |
| 0x005cd0fc | 1 | 2 | 0x00417cf0 |
| 0x005cd278 | 8 | 2 | 0x00474890 |
| 0x005cd92c | 2 | 2 | 0x00448220 |
| 0x005cd930 | 2 | 2 | 0x00448220 |
| 0x005cd934 | 2 | 2 | 0x00448220 |
| 0x005cd938 | 2 | 2 | 0x00448220 |
| 0x005cd944 | 2 | 2 | 0x00448220 |
| 0x005cd950 | 2 | 2 | 0x00448220 |
| 0x005ce018 | 5 | 2 | 0x0045ae80 |
| 0x005cf9e8 | 5 | 2 | 0x00494ac0 |
| 0x005cfaac | 5 | 2 | 0x00494ac0 |
| 0x005cfd70 | 5 | 2 | 0x00494ac0 |
| 0x005e6414 | 7 | 2 | 0x005ac650 |
| 0x005e6424 | 7 | 2 | 0x005ac650 |
| 0x005e6434 | 7 | 2 | 0x005ac650 |
| 0x005e6444 | 7 | 2 | 0x005ac650 |
| 0x005e6474 | 7 | 2 | 0x005ac650 |
| 0x005e6ce0 | 6 | 2 | 0x005af180 |
| 0x005ea0a8 | 3 | 2 | 0x004039f0 |
| 0x005ea0ac | 3 | 2 | 0x004039f0 |
| 0x005ea0b0 | 3 | 2 | 0x004039f0 |
| 0x005f33f8 | 7 | 2 | 0x0041e870 |
| 0x005f37a0 | 7 | 2 | 0x0041e870 |
| 0x006036d0 | 1 | 2 | 0x00462dd0 |
| 0x00604574 | 5 | 2 | 0x0045d460 |
| 0x00604a10 | 2 | 2 | 0x004645c0 |
| 0x00604e1c | 6 | 2 | 0x0045dd60 |
| 0x00604eb0 | 6 | 2 | 0x0045dd60 |
| 0x00605918 | 4 | 2 | 0x004633d0 |
| 0x00605d24 | 6 | 2 | 0x0045dd60 |
| 0x00605db8 | 6 | 2 | 0x0045dd60 |
| 0x0061313c | 2 | 2 | 0x0046dd80 |
| 0x00613294 | 4 | 2 | 0x00476c10 |
| 0x006146f0 | 4 | 2 | 0x004093b0 |
| 0x006146f4 | 4 | 2 | 0x004093b0 |
| 0x006146f8 | 4 | 2 | 0x004093b0 |
| 0x006147dc | 4 | 2 | 0x00494c80 |
| 0x00617f34 | 6 | 2 | 0x004c0510 |
| 0x00632ba8 | 6 | 2 | 0x005a5f00 |
| 0x00635194 | 6 | 2 | 0x005bbb20 |
| 0x006351dc | 6 | 2 | 0x005bbb20 |
| 0x006351ec | 6 | 2 | 0x005bbb20 |
| 0x00636564 | 4 | 2 | 0x00401340 |
| 0x00636578 | 4 | 2 | 0x004025f0 |
| 0x00636b90 | 3 | 2 | 0x004039f0 |
| 0x00636b94 | 3 | 2 | 0x004039f0 |
| 0x0063a5d4 | 1 | 2 | 0x00405890 |
| 0x0063a5d8 | 10 | 2 | 0x004073b0 |
| 0x0063b7f0 | 2 | 2 | 0x00409710 |
| 0x0063b900 | 4 | 2 | 0x0040bbb0 |
| 0x0063b904 | 4 | 2 | 0x0040bbb0 |
| 0x0063b914 | 1 | 2 | 0x00410510 |
| 0x0063ba90 | 2 | 2 | 0x0040cea0 |
| 0x0063ba94 | 2 | 2 | 0x0040cea0 |
| 0x0063bb60 | 7 | 2 | 0x00412050 |
| 0x0063bc40 | 4 | 2 | 0x00411f30 |
| 0x0063bd50 | 5 | 2 | 0x00413b80 |
| 0x0063c620 | 4 | 2 | 0x0041a1e0 |
| 0x0063c888 | 4 | 2 | 0x0041b450 |
| 0x0063c8d0 | 4 | 2 | 0x0041b450 |
| 0x0063cdb4 | 4 | 2 | 0x0041c100 |
| 0x0063d558 | 4 | 2 | 0x0041d6e0 |
| 0x0063d57c | 4 | 2 | 0x0041d8b0 |
| 0x0063d5e0 | 4 | 2 | 0x0041db90 |
| 0x0063d830 | 2 | 2 | 0x00420420 |
| 0x0063d9e0 | 2 | 2 | 0x00420420 |
| 0x0063dc74 | 8 | 2 | 0x0041ede0 |
| 0x0063e49c | 4 | 2 | 0x0041eaa0 |
| 0x0063e4a0 | 4 | 2 | 0x0041eaa0 |
| 0x0063e4a8 | 5 | 2 | 0x004210b0 |
| 0x0063e4b8 | 4 | 2 | 0x00420d00 |
| 0x0063e5a4 | 6 | 2 | 0x00421080 |
| 0x0063fb88 | 6 | 2 | 0x00421080 |
| 0x0063fb90 | 8 | 2 | 0x00422140 |
| 0x00644110 | 1 | 2 | 0x004235b0 |
| 0x00644370 | 6 | 2 | 0x00426e10 |
| 0x00646e58 | 6 | 2 | 0x00426e10 |
| 0x0066d6e4 | 6 | 2 | 0x00426e10 |
| 0x0066d704 | 5 | 2 | 0x00426640 |
| 0x0066d728 | 1 | 2 | 0x00409790 |
| 0x0067d960 | 4 | 2 | 0x00428390 |
| 0x0067d964 | 4 | 2 | 0x004283a0 |
| 0x0067d968 | 4 | 2 | 0x004283a0 |
| 0x0067d96c | 4 | 2 | 0x004283a0 |
| 0x0067d970 | 4 | 2 | 0x004283a0 |
| 0x0067dfa8 | 4 | 2 | 0x0042a6b0 |
| 0x0067e1a8 | 4 | 2 | 0x0042a6b0 |
| 0x0067ea94 | 1 | 2 | 0x00430670 |
| 0x0067eaa8 | 2 | 2 | 0x00431b30 |
| 0x0067eabc | 2 | 2 | 0x0042bf30 |
| 0x0067eca4 | 1 | 2 | 0x0043dfd0 |
| 0x00684dac | 1 | 2 | 0x00414f00 |
| 0x006870b4 | 4 | 2 | 0x00453f30 |
| 0x006870b8 | 4 | 2 | 0x0045bba0 |
| 0x006886b0 | 12 | 2 | 0x00456040 |
| 0x006886c4 | 12 | 2 | 0x00456040 |
| 0x0068b1b4 | 2 | 2 | 0x00458f20 |
| 0x0068b9a8 | 3 | 2 | 0x00459000 |
| 0x0068ba30 | 3 | 2 | 0x00459290 |
| 0x0068ba4c | 13 | 2 | 0x004595c0 |
| 0x0068bd00 | 2 | 2 | 0x0045a950 |
| 0x0068d1f0 | 2 | 2 | 0x0045ba00 |
| 0x0068d4d0 | 1 | 2 | 0x0045bf30 |
| 0x0068d4e0 | 3 | 2 | 0x0045bae0 |
| 0x0068f560 | 5 | 2 | 0x0045d460 |
| 0x0068f644 | 3 | 2 | 0x0045e0f0 |
| 0x0068fc84 | 5 | 2 | 0x0045d460 |
| 0x0068fc88 | 5 | 2 | 0x0045d460 |
| 0x0068fc8c | 6 | 2 | 0x0045db50 |
| 0x0068fcd0 | 5 | 2 | 0x0045d460 |
| 0x006900d4 | 5 | 2 | 0x00466b50 |
| 0x00690158 | 1 | 2 | 0x004625b0 |
| 0x0069047c | 5 | 2 | 0x0045d460 |
| 0x00690490 | 4 | 2 | 0x004669b0 |
| 0x00690494 | 4 | 2 | 0x004669b0 |
| 0x00690498 | 4 | 2 | 0x004669b0 |
| 0x006904a4 | 5 | 2 | 0x00466b50 |
| 0x006904c4 | 6 | 2 | 0x0045dc80 |
| 0x006904e0 | 5 | 2 | 0x00466b50 |
| 0x006904e4 | 5 | 2 | 0x00466b50 |
| 0x006904ec | 5 | 2 | 0x0045d3f0 |
| 0x006904f0 | 1 | 2 | 0x00462dd0 |
| 0x006905c8 | 4 | 2 | 0x00471df0 |
| 0x0069064c | 4 | 2 | 0x00471df0 |
| 0x00691508 | 9 | 2 | 0x00472500 |
| 0x00691600 | 5 | 2 | 0x004759b0 |
| 0x00691614 | 5 | 2 | 0x004759b0 |
| 0x00692498 | 5 | 2 | 0x00476390 |
| 0x006924d8 | 4 | 2 | 0x00476a30 |
| 0x006924dc | 4 | 2 | 0x004769d0 |
| 0x00692528 | 4 | 2 | 0x004769a0 |
| 0x00692554 | 4 | 2 | 0x004769f0 |
| 0x00692598 | 4 | 2 | 0x00476d00 |
| 0x006c1228 | 7 | 2 | 0x0047c0f0 |
| 0x006c2fe8 | 7 | 2 | 0x0047c0f0 |
| 0x006dccb8 | 2 | 2 | 0x00484c70 |
| 0x006e70cc | 3 | 2 | 0x00484cf0 |
| 0x006e70d8 | 2 | 2 | 0x00484c70 |
| 0x006fa654 | 4 | 2 | 0x00486460 |
| 0x006fe700 | 5 | 2 | 0x004862d0 |
| 0x006fe740 | 5 | 2 | 0x004862d0 |
| 0x0070303c | 4 | 2 | 0x00486460 |
| 0x00703110 | 6 | 2 | 0x00489890 |
| 0x00703170 | 5 | 2 | 0x00489290 |
| 0x00709238 | 5 | 2 | 0x0048a5d0 |
| 0x007151f0 | 5 | 2 | 0x0048a830 |
| 0x0071fa34 | 5 | 2 | 0x0048a830 |
| 0x00722134 | 5 | 2 | 0x0048bbe0 |
| 0x00722470 | 6 | 2 | 0x0048cf70 |
| 0x007668f8 | 5 | 2 | 0x0048eac0 |
| 0x00766938 | 5 | 2 | 0x0048eac0 |
| 0x00766978 | 5 | 2 | 0x0048eac0 |
| 0x007669b8 | 5 | 2 | 0x0048eac0 |
| 0x00766a00 | 8 | 2 | 0x0048f740 |
| 0x00769f50 | 8 | 2 | 0x0048f740 |
| 0x0076d940 | 5 | 2 | 0x0048eac0 |
| 0x007706d8 | 5 | 2 | 0x0048eac0 |
| 0x00770718 | 8 | 2 | 0x0048f740 |
| 0x00771534 | 7 | 2 | 0x00491780 |
| 0x00771964 | 4 | 2 | 0x004283a0 |
| 0x00771a08 | 2 | 2 | 0x00494a80 |
| 0x00771a10 | 2 | 2 | 0x00494480 |
| 0x00771a28 | 3 | 2 | 0x00494320 |
| 0x00771a2c | 3 | 2 | 0x00494320 |
| 0x00771a50 | 2 | 2 | 0x00494f30 |
| 0x00771a54 | 2 | 2 | 0x00494f30 |
| 0x00771e88 | 6 | 2 | 0x00495fe0 |
| 0x0077307c | 4 | 2 | 0x00496c10 |
| 0x007730d4 | 6 | 2 | 0x004972b0 |
| 0x0077311c | 6 | 2 | 0x004972b0 |
| 0x0077313c | 6 | 2 | 0x004972b0 |
| 0x00773920 | 2 | 2 | 0x004627b0 |
| 0x007d3ffc | 1 | 2 | 0x004c3df0 |
| 0x007d4054 | 4 | 2 | 0x004c5c80 |
| 0x007d716c | 5 | 2 | 0x004e4320 |
| 0x007dc754 | 6 | 2 | 0x00550be0 |
| 0x007dc94c | 6 | 2 | 0x005a5f00 |
| 0x007dc958 | 6 | 2 | 0x005a60e0 |
| 0x007dc9e8 | 8 | 2 | 0x005a6c60 |
| 0x007dca18 | 7 | 2 | 0x005a6f30 |
| 0x007dca20 | 6 | 2 | 0x005a71f0 |
| 0x007dca24 | 6 | 2 | 0x005a7460 |
| 0x007dca30 | 6 | 2 | 0x005a71f0 |
| 0x007dca3c | 6 | 2 | 0x005a7460 |
| 0x007dcae0 | 2 | 2 | 0x005a8890 |
| 0x007dcae8 | 2 | 2 | 0x005a7f70 |
| 0x007dcd20 | 2 | 2 | 0x005a8890 |
| 0x007dce00 | 8 | 2 | 0x005ab040 |
| 0x007e95c0 | 6 | 2 | 0x00496530 |
| 0x007e9700 | 6 | 2 | 0x00496530 |
| 0x007ea9e8 | 3 | 2 | 0x00422ba0 |
| 0x007ea9ec | 3 | 2 | 0x00422ba0 |
| 0x007ec9e8 | 3 | 2 | 0x00422ba0 |
| 0x007f09e4 | 3 | 2 | 0x00402750 |
| 0x007f0a30 | 3 | 2 | 0x00402750 |
| 0x007f0a40 | 1 | 2 | 0x00404e80 |
| 0x007f0f04 | 6 | 2 | 0x00431b50 |
| 0x007f0fd8 | 1 | 2 | 0x0040eee0 |
| 0x007f0fe0 | 3 | 2 | 0x004039f0 |
| 0x007f0fe4 | 3 | 2 | 0x004039f0 |
| 0x007f101c | 9 | 2 | 0x00445aa0 |
| 0x007f1044 | 1 | 2 | 0x0043dfd0 |
| 0x007f108b | 8 | 2 | 0x00491490 |
| 0x007f1502 | 2 | 2 | 0x0042b960 |
| 0x007f1a0c | 2 | 2 | 0x0042b950 |
| 0x3f800000 | 4 | 2 | 0x004669b0 |
| 0xff323232 | 4 | 2 | 0x0041c100 |
| 0x004019c3 | 5 | 1 | 0x00401690 |
| 0x004039d6 | 4 | 1 | 0x004039c0 |
| 0x00404e82 | 1 | 1 | 0x00404e80 |
| 0x00404e91 | 1 | 1 | 0x00404e80 |
| 0x00404ea0 | 1 | 1 | 0x00404e80 |
| 0x00404eb2 | 1 | 1 | 0x00404e80 |
| 0x00404ec1 | 1 | 1 | 0x00404e80 |
| 0x00404ec8 | 1 | 1 | 0x00404e80 |
| 0x00404ed3 | 1 | 1 | 0x00404e80 |
| 0x00405416 | 8 | 1 | 0x00405400 |
| 0x00405456 | 9 | 1 | 0x00405430 |
| 0x0040572e | 9 | 1 | 0x00405540 |
| 0x004058a9 | 1 | 1 | 0x00405890 |
| 0x00405f57 | 10 | 1 | 0x00405ab0 |
| 0x0040615d | 11 | 1 | 0x00406130 |
| 0x00406369 | 12 | 1 | 0x00406160 |
| 0x00406408 | 10 | 1 | 0x00406370 |
| 0x00406ad7 | 10 | 1 | 0x004069d0 |
| 0x004075a5 | 7 | 1 | 0x004075a0 |
| 0x00407a50 | 1 | 1 | 0x00407a40 |
| 0x00407c6f | 5 | 1 | 0x00407be0 |
| 0x00407c82 | 5 | 1 | 0x00407be0 |
| 0x00407d4e | 4 | 1 | 0x00407cd0 |
| 0x00408528 | 5 | 1 | 0x00407e20 |
| 0x00408a60 | 1 | 1 | 0x00408a50 |
| 0x00408acf | 1 | 1 | 0x00408a70 |
| 0x00408aff | 1 | 1 | 0x00408af0 |
| 0x004099aa | 3 | 1 | 0x004099a0 |
| 0x004099b7 | 3 | 1 | 0x004099a0 |
| 0x004099c0 | 3 | 1 | 0x004099a0 |
| 0x004099c9 | 3 | 1 | 0x004099a0 |
| 0x00409b0e | 2 | 1 | 0x0040acd0 |
| 0x0040a13e | 2 | 1 | 0x0040acd0 |
| 0x0040a3c1 | 2 | 1 | 0x0040acd0 |
| 0x0040ad00 | 2 | 1 | 0x0040acd0 |
| 0x0040ae23 | 9 | 1 | 0x0040ad90 |
| 0x0040aeec | 9 | 1 | 0x0040ae30 |
| 0x0040b088 | 9 | 1 | 0x0040aef0 |
| 0x0040b0cd | 8 | 1 | 0x0040b090 |
| 0x0040b296 | 1 | 1 | 0x0040b290 |
| 0x0040b2d5 | 1 | 1 | 0x0040b290 |
| 0x0040b2f9 | 1 | 1 | 0x0040b290 |
| 0x0040b305 | 1 | 1 | 0x0040b290 |
| 0x0040b315 | 1 | 1 | 0x0040b290 |
| 0x0040b31f | 1 | 1 | 0x0040b290 |
| 0x0040b325 | 1 | 1 | 0x0040b290 |
| 0x0040b32b | 1 | 1 | 0x0040b290 |
| 0x0040b331 | 1 | 1 | 0x0040b290 |
| 0x0040b34b | 1 | 1 | 0x0040b290 |
| 0x0040cf78 | 8 | 1 | 0x0040cf40 |
| 0x0040dc86 | 3 | 1 | 0x0040dc80 |
| 0x0040dca7 | 4 | 1 | 0x0040dc90 |
| 0x0040e330 | 1 | 1 | 0x0040e180 |
| 0x0040e345 | 1 | 1 | 0x0040e340 |
| 0x0040e355 | 1 | 1 | 0x0040e350 |
| 0x0040e363 | 2 | 1 | 0x0040e360 |
| 0x0040e391 | 1 | 1 | 0x0040e370 |
| 0x0040e4a5 | 1 | 1 | 0x0040e4a0 |
| 0x0040eef5 | 1 | 1 | 0x0040eee0 |
| 0x0040ef00 | 1 | 1 | 0x0040eee0 |
| 0x0040ef1c | 1 | 1 | 0x0040eee0 |
| 0x0040ef26 | 1 | 1 | 0x0040eee0 |
| 0x0040ef2b | 1 | 1 | 0x0040eee0 |
| 0x0040ef5c | 1 | 1 | 0x0040eee0 |
| 0x0040ef60 | 1 | 1 | 0x0040eee0 |
| 0x0040efa5 | 1 | 1 | 0x0040eee0 |
| 0x0040efba | 1 | 1 | 0x0040eee0 |
| 0x0040efcd | 1 | 1 | 0x0040eee0 |
| 0x0040efd3 | 1 | 1 | 0x0040eee0 |
| 0x0040efee | 1 | 1 | 0x0040eee0 |
| 0x0040f003 | 1 | 1 | 0x0040eee0 |
| 0x0040f00c | 1 | 1 | 0x0040eee0 |
| 0x0040f065 | 1 | 1 | 0x0040eee0 |
| 0x0040f09e | 1 | 1 | 0x0040eee0 |
| 0x0040f1e8 | 1 | 1 | 0x0040eee0 |
| 0x0040f219 | 1 | 1 | 0x0040eee0 |
| 0x0040f387 | 1 | 1 | 0x0040eee0 |
| 0x0040f4b0 | 1 | 1 | 0x0040eee0 |
| 0x0040f55c | 1 | 1 | 0x0040eee0 |
| 0x0040f5a0 | 1 | 1 | 0x0040eee0 |
| 0x0040f5b6 | 1 | 1 | 0x0040eee0 |
| 0x0040f8bc | 1 | 1 | 0x0040eee0 |
| 0x0040f948 | 1 | 1 | 0x0040eee0 |
| 0x0040fbbb | 1 | 1 | 0x0040eee0 |
| 0x0040fbc1 | 1 | 1 | 0x0040eee0 |
| 0x0040fbef | 1 | 1 | 0x0040eee0 |
| 0x0040fbf8 | 1 | 1 | 0x0040eee0 |
| 0x0040fbfc | 1 | 1 | 0x0040eee0 |
| 0x00410540 | 1 | 1 | 0x00410510 |
| 0x00410549 | 1 | 1 | 0x00410510 |
| 0x00410568 | 1 | 1 | 0x00410510 |
| 0x0041056b | 1 | 1 | 0x00410510 |
| 0x00410577 | 1 | 1 | 0x00410510 |
| 0x00410579 | 1 | 1 | 0x00410510 |
| 0x00410589 | 1 | 1 | 0x00410510 |
| 0x004105b1 | 1 | 1 | 0x00410510 |
| 0x004105b3 | 1 | 1 | 0x00410510 |
| 0x004105f8 | 1 | 1 | 0x00410510 |
| 0x004105fa | 1 | 1 | 0x00410510 |
| 0x0041062a | 1 | 1 | 0x00410510 |
| 0x004106a9 | 2 | 1 | 0x0040dd60 |
| 0x00410841 | 1 | 1 | 0x00410510 |
| 0x004111b5 | 1 | 1 | 0x00411170 |
| 0x0041152c | 8 | 1 | 0x004114e0 |
| 0x004115f9 | 7 | 1 | 0x004115c0 |
| 0x00412007 | 4 | 1 | 0x00411f30 |
| 0x00413caa | 3 | 1 | 0x00413c70 |
| 0x00414028 | 1 | 1 | 0x00413fe0 |
| 0x0041405f | 1 | 1 | 0x00414030 |
| 0x0041448d | 2 | 1 | 0x00414300 |
| 0x00414565 | 2 | 1 | 0x00414490 |
| 0x004148a4 | 1 | 1 | 0x00414570 |
| 0x00414a2b | 1 | 1 | 0x004148b0 |
| 0x00414a68 | 1 | 1 | 0x004148b0 |
| 0x00414c2e | 1 | 1 | 0x00414a70 |
| 0x00414ef0 | 1 | 1 | 0x00414c30 |
| 0x00415011 | 1 | 1 | 0x00414f00 |
| 0x004150d6 | 1 | 1 | 0x00415020 |
| 0x0041518f | 1 | 1 | 0x004150e0 |
| 0x004151f8 | 1 | 1 | 0x00415190 |
| 0x0041521f | 1 | 1 | 0x00415200 |
| 0x00415733 | 1 | 1 | 0x00415220 |
| 0x0041582a | 1 | 1 | 0x00415220 |
| 0x00415871 | 2 | 1 | 0x00415860 |
| 0x00415b68 | 1 | 1 | 0x00415880 |
| 0x00415cf0 | 1 | 1 | 0x00415880 |
| 0x00415e18 | 1 | 1 | 0x00415d00 |
| 0x00416055 | 1 | 1 | 0x00415e20 |
| 0x004161de | 1 | 1 | 0x00416060 |
| 0x00416224 | 1 | 1 | 0x004161e0 |
| 0x00416a2d | 1 | 1 | 0x00416250 |
| 0x0041717d | 1 | 1 | 0x00416a30 |
| 0x0041774e | 1 | 1 | 0x00417740 |
| 0x00417cff | 1 | 1 | 0x00417cf0 |
| 0x00417d04 | 1 | 1 | 0x00417cf0 |
| 0x00417d0a | 1 | 1 | 0x00417cf0 |
| 0x00417d12 | 1 | 1 | 0x00417cf0 |
| 0x00417d28 | 1 | 1 | 0x00417cf0 |
| 0x00417d34 | 1 | 1 | 0x00417cf0 |
| 0x00417d3e | 1 | 1 | 0x00417cf0 |
| 0x0041856c | 1 | 1 | 0x00418560 |
| 0x00418572 | 1 | 1 | 0x00418560 |
| 0x00418575 | 1 | 1 | 0x00418560 |
| 0x00418583 | 1 | 1 | 0x00418560 |
| 0x0041858b | 1 | 1 | 0x00418560 |
| 0x0041858e | 1 | 1 | 0x00418560 |
| 0x00418593 | 1 | 1 | 0x00418560 |
| 0x00418598 | 1 | 1 | 0x00418560 |
| 0x0041859e | 1 | 1 | 0x00418560 |
| 0x004185a3 | 1 | 1 | 0x00418560 |
| 0x004185ab | 1 | 1 | 0x00418560 |
| 0x004185b5 | 1 | 1 | 0x00418560 |
| 0x004185ca | 1 | 1 | 0x00418560 |
| 0x004185cb | 1 | 1 | 0x00418560 |
| 0x004185e4 | 1 | 1 | 0x00418560 |
| 0x004185e5 | 1 | 1 | 0x00418560 |
| 0x004185e7 | 1 | 1 | 0x00418560 |
| 0x004185ee | 1 | 1 | 0x00418560 |
| 0x004185f1 | 1 | 1 | 0x00418560 |
| 0x004185fa | 1 | 1 | 0x00418560 |
| 0x00418604 | 1 | 1 | 0x00418560 |
| 0x0041860a | 1 | 1 | 0x00418560 |
| 0x00418619 | 1 | 1 | 0x00418560 |
| 0x0041863f | 1 | 1 | 0x00418560 |
| 0x00418649 | 1 | 1 | 0x00418560 |
| 0x0041867e | 1 | 1 | 0x00418560 |
| 0x00418689 | 1 | 1 | 0x00418560 |
| 0x004186be | 1 | 1 | 0x00418560 |
| 0x004186c9 | 1 | 1 | 0x00418560 |
| 0x004186fe | 1 | 1 | 0x00418560 |
| 0x00418704 | 1 | 1 | 0x00418560 |
| 0x0041870d | 1 | 1 | 0x00418560 |
| 0x00418763 | 1 | 1 | 0x00418560 |
| 0x00418805 | 1 | 1 | 0x00418560 |
| 0x0041883e | 1 | 1 | 0x00418560 |
| 0x00418e08 | 3 | 1 | 0x00418de0 |
| 0x00418e12 | 3 | 1 | 0x00418de0 |
| 0x00418e24 | 3 | 1 | 0x00418de0 |
| 0x0041976d | 2 | 1 | 0x00419760 |
| 0x00419774 | 2 | 1 | 0x00419760 |
| 0x00419788 | 2 | 1 | 0x00419760 |
| 0x0041979a | 2 | 1 | 0x00419760 |
| 0x004197a3 | 2 | 1 | 0x00419760 |
| 0x004197ad | 2 | 1 | 0x00419760 |
| 0x004197b6 | 2 | 1 | 0x00419760 |
| 0x004197bf | 2 | 1 | 0x00419760 |
| 0x0041b717 | 5 | 1 | 0x0041b690 |
| 0x0041cdaa | 5 | 1 | 0x0041cd20 |
| 0x0041e145 | 3 | 1 | 0x0041e140 |
| 0x0041e8b6 | 7 | 1 | 0x0041e8b0 |
| 0x0041e8c6 | 7 | 1 | 0x0041e8c0 |
| 0x0041e8d6 | 4 | 1 | 0x0041e8d0 |
| 0x0041e8d8 | 4 | 1 | 0x0041e8d0 |
| 0x0041e958 | 5 | 1 | 0x0041e950 |
| 0x0041e976 | 7 | 1 | 0x0041e970 |
| 0x0041e9b3 | 6 | 1 | 0x0041e9b0 |
| 0x0041e9e3 | 7 | 1 | 0x0041e9e0 |
| 0x0041e9f5 | 5 | 1 | 0x0041e9f0 |
| 0x0041e9f8 | 5 | 1 | 0x0041e9f0 |
| 0x0041ea78 | 4 | 1 | 0x0041ea70 |
| 0x0041efc5 | 7 | 1 | 0x0041efc0 |
| 0x0041efcd | 7 | 1 | 0x0041efc0 |
| 0x0041f08d | 5 | 1 | 0x0041f060 |
| 0x0041f252 | 12 | 1 | 0x0041f220 |
| 0x0041f2b3 | 4 | 1 | 0x0041f290 |
| 0x0041f324 | 8 | 1 | 0x0041f320 |
| 0x0041f70f | 13 | 1 | 0x0041f590 |
| 0x00420d98 | 7 | 1 | 0x00420d80 |
| 0x00420deb | 3 | 1 | 0x00420de0 |
| 0x00420df1 | 3 | 1 | 0x00420de0 |
| 0x00421079 | 7 | 1 | 0x00421060 |
| 0x004210aa | 6 | 1 | 0x00421080 |
| 0x004210e6 | 5 | 1 | 0x004210b0 |
| 0x004210fb | 7 | 1 | 0x004210f0 |
| 0x00421152 | 7 | 1 | 0x00421100 |
| 0x004212a6 | 6 | 1 | 0x00421160 |
| 0x004215c6 | 2 | 1 | 0x004215c0 |
| 0x004215dc | 2 | 1 | 0x004215c0 |
| 0x00421620 | 12 | 1 | 0x004215e0 |
| 0x0042168e | 11 | 1 | 0x00421630 |
| 0x004216a5 | 6 | 1 | 0x00421690 |
| 0x0042211f | 9 | 1 | 0x004220d0 |
| 0x0042215e | 8 | 1 | 0x00422140 |
| 0x00422fcd | 3 | 1 | 0x00422ba0 |
| 0x00423032 | 1 | 1 | 0x00422fd0 |
| 0x00425a30 | 2 | 1 | 0x004252c0 |
| 0x00425bc2 | 4 | 1 | 0x00425bc0 |
| 0x00425bd4 | 4 | 1 | 0x00425bc0 |
| 0x00425bda | 4 | 1 | 0x00425bc0 |
| 0x00426ccc | 1 | 1 | 0x00426cc0 |
| 0x004274de | 4 | 1 | 0x004274d0 |
| 0x00428398 | 4 | 1 | 0x00428390 |
| 0x00429306 | 1 | 1 | 0x00429300 |
| 0x0042a730 | 4 | 1 | 0x0042a6b0 |
| 0x0042bfa0 | 2 | 1 | 0x0042bf30 |
| 0x0042f765 | 7 | 1 | 0x0042f760 |
| 0x0042f775 | 7 | 1 | 0x0042f770 |
| 0x0042f785 | 7 | 1 | 0x0042f780 |
| 0x0043067f | 1 | 1 | 0x00430670 |
| 0x004306a9 | 1 | 1 | 0x00430670 |
| 0x004306b5 | 1 | 1 | 0x00430670 |
| 0x004306d3 | 1 | 1 | 0x00430670 |
| 0x00430794 | 7 | 1 | 0x00430790 |
| 0x00431b2e | 7 | 1 | 0x00431b20 |
| 0x00431b48 | 4 | 1 | 0x00431b40 |
| 0x00431d72 | 2 | 1 | 0x00431d70 |
| 0x00432252 | 7 | 1 | 0x00432230 |
| 0x00432282 | 7 | 1 | 0x00432260 |
| 0x0043dffe | 1 | 1 | 0x0043dfd0 |
| 0x00440aa9 | 1 | 1 | 0x0043dfd0 |
| 0x00441803 | 7 | 1 | 0x004417e0 |
| 0x00441808 | 7 | 1 | 0x004417e0 |
| 0x0044180f | 7 | 1 | 0x004417e0 |
| 0x00442cd6 | 1 | 1 | 0x00442cc0 |
| 0x00443084 | 1 | 1 | 0x00443080 |
| 0x004430a9 | 8 | 1 | 0x004430a0 |
| 0x004430b5 | 8 | 1 | 0x004430b0 |
| 0x00443431 | 1 | 1 | 0x00443300 |
| 0x00443d0c | 1 | 1 | 0x00443440 |
| 0x00443db5 | 1 | 1 | 0x00443d10 |
| 0x00444b5a | 1 | 1 | 0x00443dc0 |
| 0x00445d74 | 9 | 1 | 0x00445aa0 |
| 0x00448702 | 1 | 1 | 0x00448700 |
| 0x00448718 | 1 | 1 | 0x00448700 |
| 0x0044c5e9 | 9 | 1 | 0x0044c4f0 |
| 0x00452167 | 1 | 1 | 0x00452160 |
| 0x004522d6 | 7 | 1 | 0x004522d0 |
| 0x004522da | 7 | 1 | 0x004522d0 |
| 0x00452eaa | 1 | 1 | 0x00452ea0 |
| 0x00452eb6 | 1 | 1 | 0x00452eb0 |
| 0x00452f63 | 14 | 1 | 0x00452f30 |
| 0x00453f24 | 13 | 1 | 0x00453eb0 |
| 0x00453f35 | 4 | 1 | 0x00453f30 |
| 0x00453f60 | 4 | 1 | 0x0045bba0 |
| 0x00454a3d | 2 | 1 | 0x00454a30 |
| 0x00455b4d | 2 | 1 | 0x00455b40 |
| 0x00456c94 | 14 | 1 | 0x00456c90 |
| 0x00458920 | 3 | 1 | 0x00459000 |
| 0x00458b0e | 4 | 1 | 0x00458a40 |
| 0x00458bea | 14 | 1 | 0x00458b10 |
| 0x00458dc0 | 4 | 1 | 0x00458d00 |
| 0x00458f70 | 2 | 1 | 0x00458f20 |
| 0x00458f98 | 1 | 1 | 0x00458f80 |
| 0x0045928e | 3 | 1 | 0x00459000 |
| 0x004593f3 | 5 | 1 | 0x004593b0 |
| 0x00459532 | 6 | 1 | 0x00459480 |
| 0x0045961a | 13 | 1 | 0x004595c0 |
| 0x0045a10a | 2 | 1 | 0x0045a0f0 |
| 0x0045a128 | 4 | 1 | 0x0045a110 |
| 0x0045a180 | 14 | 1 | 0x0045a130 |
| 0x0045a392 | 5 | 1 | 0x0045a190 |
| 0x0045aba1 | 2 | 1 | 0x0045a950 |
| 0x0045b1f5 | 5 | 1 | 0x0045ae80 |
| 0x0045b932 | 4 | 1 | 0x0045b930 |
| 0x0045b937 | 4 | 1 | 0x0045b930 |
| 0x0045b93c | 4 | 1 | 0x0045b930 |
| 0x0045b941 | 4 | 1 | 0x0045b930 |
| 0x0045b946 | 4 | 1 | 0x0045b930 |
| 0x0045b94b | 4 | 1 | 0x0045b930 |
| 0x0045b954 | 4 | 1 | 0x0045b930 |
| 0x0045b970 | 4 | 1 | 0x0045b930 |
| 0x0045ba03 | 2 | 1 | 0x0045ba00 |
| 0x0045ba0e | 2 | 1 | 0x0045ba00 |
| 0x0045bae3 | 3 | 1 | 0x0045bae0 |
| 0x0045bae7 | 3 | 1 | 0x0045bae0 |
| 0x0045baf5 | 3 | 1 | 0x0045bae0 |
| 0x0045bafd | 3 | 1 | 0x0045bae0 |
| 0x0045bb0f | 3 | 1 | 0x0045bae0 |
| 0x0045bb1a | 3 | 1 | 0x0045bae0 |
| 0x0045bb30 | 3 | 1 | 0x0045bae0 |
| 0x0045bb35 | 3 | 1 | 0x0045bae0 |
| 0x0045bb3a | 3 | 1 | 0x0045bae0 |
| 0x0045bb47 | 3 | 1 | 0x0045bae0 |
| 0x0045bb52 | 3 | 1 | 0x0045bae0 |
| 0x0045bb57 | 3 | 1 | 0x0045bae0 |
| 0x0045c876 | 7 | 1 | 0x0045c860 |
| 0x0045d795 | 5 | 1 | 0x0045d460 |
| 0x0045db4e | 5 | 1 | 0x0045daf0 |
| 0x0045dcdb | 6 | 1 | 0x0045dc80 |
| 0x0045dd44 | 13 | 1 | 0x0045dce0 |
| 0x0045de7a | 6 | 1 | 0x0045dd60 |
| 0x0045e0ef | 7 | 1 | 0x00467010 |
| 0x0045e15f | 3 | 1 | 0x0045e0f0 |
| 0x004624b4 | 6 | 1 | 0x004623e0 |
| 0x0046257b | 6 | 1 | 0x00462520 |
| 0x0046273f | 1 | 1 | 0x004625b0 |
| 0x004627d1 | 2 | 1 | 0x004627b0 |
| 0x0046294e | 5 | 1 | 0x004627f0 |
| 0x00462ebf | 1 | 1 | 0x00462dd0 |
| 0x00462f43 | 6 | 1 | 0x00462ec0 |
| 0x00462ffb | 6 | 1 | 0x00462f50 |
| 0x004630e8 | 6 | 1 | 0x00462f50 |
| 0x00463131 | 6 | 1 | 0x00462f50 |
| 0x004633cb | 6 | 1 | 0x004631f0 |
| 0x00463589 | 4 | 1 | 0x004633d0 |
| 0x00463630 | 6 | 1 | 0x00463590 |
| 0x00463c30 | 6 | 1 | 0x00463640 |
| 0x00463f36 | 6 | 1 | 0x00463c80 |
| 0x004642e9 | 6 | 1 | 0x00463f40 |
| 0x00464495 | 6 | 1 | 0x004642f0 |
| 0x004645b5 | 6 | 1 | 0x004644a0 |
| 0x0046466a | 2 | 1 | 0x004645c0 |
| 0x004647e0 | 7 | 1 | 0x00464670 |
| 0x004648a9 | 6 | 1 | 0x004647f0 |
| 0x00464a09 | 6 | 1 | 0x004648b0 |
| 0x00464c9a | 6 | 1 | 0x00464a50 |
| 0x00465a23 | 14 | 1 | 0x00465940 |
| 0x00466b44 | 7 | 1 | 0x00467010 |
| 0x00466f5c | 5 | 1 | 0x00466b50 |
| 0x00467011 | 7 | 1 | 0x00467010 |
| 0x00467016 | 7 | 1 | 0x00467010 |
| 0x00467024 | 7 | 1 | 0x00467020 |
| 0x0046702c | 7 | 1 | 0x00467020 |
| 0x00467072 | 7 | 1 | 0x00467070 |
| 0x0046707e | 7 | 1 | 0x00467070 |
| 0x00467089 | 7 | 1 | 0x00467070 |
| 0x00467213 | 7 | 1 | 0x00467210 |
| 0x00467225 | 7 | 1 | 0x00467210 |
| 0x00467231 | 7 | 1 | 0x00467210 |
| 0x0046723a | 7 | 1 | 0x00467210 |
| 0x00467240 | 7 | 1 | 0x00467210 |
| 0x0046c5c4 | 2 | 1 | 0x0046c5c0 |
| 0x0046c5cb | 2 | 1 | 0x0046c5c0 |
| 0x0046c5d3 | 2 | 1 | 0x0046c5c0 |
| 0x0046c5df | 2 | 1 | 0x0046c5c0 |
| 0x0046c5ec | 2 | 1 | 0x0046c5c0 |
| 0x0046c704 | 1 | 1 | 0x0046c700 |
| 0x0046c717 | 1 | 1 | 0x0046c700 |
| 0x0046c726 | 1 | 1 | 0x0046c700 |
| 0x0046c748 | 7 | 1 | 0x0046c730 |
| 0x0046c768 | 8 | 1 | 0x0046c750 |
| 0x0046c794 | 2 | 1 | 0x0046c790 |
| 0x0046c79e | 2 | 1 | 0x0046c790 |
| 0x0046c7ac | 2 | 1 | 0x0046c790 |
| 0x0046cc38 | 2 | 1 | 0x0046cc10 |
| 0x0046d4cd | 1 | 1 | 0x0046d4a0 |
| 0x0046d563 | 1 | 1 | 0x0046d510 |
| 0x0046d653 | 1 | 1 | 0x0046d570 |
| 0x0046d6c3 | 1 | 1 | 0x0046d6a0 |
| 0x0046d6f3 | 1 | 1 | 0x0046d6d0 |
| 0x0046d7ec | 8 | 1 | 0x0046d780 |
| 0x0046d87f | 9 | 1 | 0x0046d7f0 |
| 0x004714e3 | 7 | 1 | 0x00471490 |
| 0x00471556 | 5 | 1 | 0x00471530 |
| 0x0047159c | 6 | 1 | 0x00471560 |
| 0x0047177c | 7 | 1 | 0x004715a0 |
| 0x0047251d | 9 | 1 | 0x00472500 |
| 0x004725af | 9 | 1 | 0x00472560 |
| 0x0047263a | 8 | 1 | 0x004725f0 |
| 0x00472c59 | 10 | 1 | 0x00472b10 |
| 0x00472c68 | 3 | 1 | 0x00472c60 |
| 0x00472c7a | 3 | 1 | 0x00472c60 |
| 0x00472c8c | 3 | 1 | 0x00472c60 |
| 0x00472c9e | 3 | 1 | 0x00472c60 |
| 0x00472ca4 | 3 | 1 | 0x00472c60 |
| 0x00472cac | 3 | 1 | 0x00472c60 |
| 0x00472cb8 | 3 | 1 | 0x00472c60 |
| 0x00472cbc | 3 | 1 | 0x00472c60 |
| 0x00472cc0 | 3 | 1 | 0x00472c60 |
| 0x00472cc4 | 3 | 1 | 0x00472c60 |
| 0x00472cc8 | 3 | 1 | 0x00472c60 |
| 0x00472cd2 | 3 | 1 | 0x00472c60 |
| 0x00472cd8 | 3 | 1 | 0x00472c60 |
| 0x00472cf6 | 3 | 1 | 0x00472c60 |
| 0x00472cfe | 3 | 1 | 0x00472c60 |
| 0x00472d07 | 3 | 1 | 0x00472c60 |
| 0x00472d0e | 3 | 1 | 0x00472c60 |
| 0x00472d15 | 3 | 1 | 0x00472c60 |
| 0x00472d1c | 3 | 1 | 0x00472c60 |
| 0x00472d27 | 3 | 1 | 0x00472c60 |
| 0x004733a8 | 9 | 1 | 0x00473220 |
| 0x00473538 | 9 | 1 | 0x004733b0 |
| 0x00474d30 | 4 | 1 | 0x00474d60 |
| 0x00474f50 | 9 | 1 | 0x00475010 |
| 0x00474f90 | 10 | 1 | 0x00474fb0 |
| 0x00474fb6 | 10 | 1 | 0x00474fb0 |
| 0x004756f0 | 9 | 1 | 0x004756e0 |
| 0x004756f5 | 9 | 1 | 0x004756e0 |
| 0x004756fd | 9 | 1 | 0x004756e0 |
| 0x00475703 | 9 | 1 | 0x004756e0 |
| 0x00475707 | 9 | 1 | 0x004756e0 |
| 0x00475714 | 9 | 1 | 0x004756e0 |
| 0x00475716 | 9 | 1 | 0x004756e0 |
| 0x0047571c | 9 | 1 | 0x004756e0 |
| 0x0047577d | 10 | 1 | 0x00475770 |
| 0x0047578d | 10 | 1 | 0x00475770 |
| 0x004759b6 | 5 | 1 | 0x004759b0 |
| 0x004759bc | 5 | 1 | 0x004759b0 |
| 0x004759c8 | 5 | 1 | 0x004759b0 |
| 0x004759d4 | 5 | 1 | 0x004759b0 |
| 0x004759e0 | 5 | 1 | 0x004759b0 |
| 0x00475a06 | 5 | 1 | 0x00475a00 |
| 0x00475a16 | 5 | 1 | 0x00475a00 |
| 0x00475a21 | 5 | 1 | 0x00475a00 |
| 0x00475a27 | 5 | 1 | 0x00475a00 |
| 0x00475a2d | 5 | 1 | 0x00475a00 |
| 0x00475d9e | 6 | 1 | 0x00475d90 |
| 0x00475da5 | 6 | 1 | 0x00475d90 |
| 0x00475dba | 6 | 1 | 0x00475d90 |
| 0x00475dc9 | 6 | 1 | 0x00475d90 |
| 0x00475de8 | 6 | 1 | 0x00475d90 |
| 0x00475de9 | 6 | 1 | 0x00475d90 |
| 0x00475e43 | 6 | 1 | 0x00475d90 |
| 0x00475ea2 | 6 | 1 | 0x00475ea0 |
| 0x00475eb8 | 6 | 1 | 0x00475ea0 |
| 0x00475eb9 | 6 | 1 | 0x00475ea0 |
| 0x00475ece | 6 | 1 | 0x00475ea0 |
| 0x00475ed0 | 6 | 1 | 0x00475ea0 |
| 0x00475ef6 | 6 | 1 | 0x00475ef0 |
| 0x00475eff | 6 | 1 | 0x00475ef0 |
| 0x00475f08 | 6 | 1 | 0x00475ef0 |
| 0x00476392 | 5 | 1 | 0x00476390 |
| 0x00476398 | 5 | 1 | 0x00476390 |
| 0x004763a4 | 5 | 1 | 0x00476390 |
| 0x004763c2 | 5 | 1 | 0x00476390 |
| 0x004763c4 | 5 | 1 | 0x00476390 |
| 0x004763d0 | 5 | 1 | 0x00476390 |
| 0x004763e8 | 5 | 1 | 0x00476390 |
| 0x004763ea | 5 | 1 | 0x00476390 |
| 0x0047689e | 5 | 1 | 0x00476880 |
| 0x004770bb | 7 | 1 | 0x004770a0 |
| 0x004785da | 8 | 1 | 0x004783f0 |
| 0x00478ca5 | 7 | 1 | 0x00478660 |
| 0x0047ba2c | 10 | 1 | 0x0047ba20 |
| 0x0047ba3a | 10 | 1 | 0x0047ba20 |
| 0x0047ba84 | 10 | 1 | 0x0047ba20 |
| 0x0047ba8f | 10 | 1 | 0x0047ba20 |
| 0x0047baad | 10 | 1 | 0x0047ba20 |
| 0x0047bad8 | 10 | 1 | 0x0047ba20 |
| 0x0047baf2 | 10 | 1 | 0x0047ba20 |
| 0x0047bb05 | 10 | 1 | 0x0047ba20 |
| 0x0047bb55 | 9 | 1 | 0x0047bb10 |
| 0x0047bb6e | 9 | 1 | 0x0047bb10 |
| 0x0047bb8f | 9 | 1 | 0x0047bb10 |
| 0x0047bbc6 | 9 | 1 | 0x0047bb10 |
| 0x0047bbd0 | 9 | 1 | 0x0047bb10 |
| 0x0047bc2a | 9 | 1 | 0x0047bb10 |
| 0x0047bc63 | 9 | 1 | 0x0047bb10 |
| 0x0047bc74 | 9 | 1 | 0x0047bb10 |
| 0x0047bc81 | 9 | 1 | 0x0047bb10 |
| 0x0047cdd3 | 8 | 1 | 0x0047cdc0 |
| 0x0047d144 | 10 | 1 | 0x0047d130 |
| 0x0047d149 | 10 | 1 | 0x0047d130 |
| 0x0048290e | 5 | 1 | 0x00482900 |
| 0x00482918 | 5 | 1 | 0x00482900 |
| 0x0048292f | 5 | 1 | 0x00482900 |
| 0x00484134 | 7 | 1 | 0x00484130 |
| 0x0048413f | 7 | 1 | 0x00484130 |
| 0x0048417b | 4 | 1 | 0x00484170 |
| 0x00484186 | 4 | 1 | 0x00484170 |
| 0x0048418a | 4 | 1 | 0x00484170 |
| 0x0048418e | 4 | 1 | 0x00484170 |
| 0x00484192 | 4 | 1 | 0x00484170 |
| 0x0048419e | 4 | 1 | 0x00484170 |
| 0x004841a3 | 4 | 1 | 0x00484170 |
| 0x004841a9 | 4 | 1 | 0x00484170 |
| 0x004841ad | 4 | 1 | 0x00484170 |
| 0x004841b5 | 4 | 1 | 0x00484170 |
| 0x004841c1 | 4 | 1 | 0x00484170 |
| 0x004841d7 | 4 | 1 | 0x004841d0 |
| 0x00484200 | 4 | 1 | 0x004841d0 |
| 0x0048421b | 4 | 1 | 0x004841d0 |
| 0x00484231 | 4 | 1 | 0x004841d0 |
| 0x00484245 | 4 | 1 | 0x004841d0 |
| 0x00484c81 | 2 | 1 | 0x00484c70 |
| 0x00484ddc | 3 | 1 | 0x00484cf0 |
| 0x0048701b | 6 | 1 | 0x00486f90 |
| 0x0048714b | 6 | 1 | 0x00487140 |
| 0x004892b3 | 5 | 1 | 0x00489290 |
| 0x00489399 | 6 | 1 | 0x004892c0 |
| 0x004893af | 6 | 1 | 0x004893a0 |
| 0x004893ce | 6 | 1 | 0x004893b0 |
| 0x00489472 | 6 | 1 | 0x00489450 |
| 0x00489497 | 6 | 1 | 0x00489480 |
| 0x004894e0 | 6 | 1 | 0x004894a0 |
| 0x004894ff | 7 | 1 | 0x004894f0 |
| 0x0048988a | 7 | 1 | 0x00489500 |
| 0x004898c6 | 6 | 1 | 0x00489890 |
| 0x00489930 | 7 | 1 | 0x00489910 |
| 0x004899be | 7 | 1 | 0x00489940 |
| 0x0048a6e8 | 2 | 1 | 0x0048a630 |
| 0x0048ea34 | 5 | 1 | 0x0048e820 |
| 0x0048ea4e | 5 | 1 | 0x0048e820 |
| 0x0048ea54 | 5 | 1 | 0x0048e820 |
| 0x0048fed0 | 5 | 1 | 0x0048ff20 |
| 0x004922e7 | 1 | 1 | 0x004922e0 |
| 0x004922f6 | 1 | 1 | 0x004922e0 |
| 0x00492305 | 1 | 1 | 0x004922e0 |
| 0x00492311 | 1 | 1 | 0x004922e0 |
| 0x0049431d | 3 | 1 | 0x004942b0 |
| 0x004943c7 | 3 | 1 | 0x00494320 |
| 0x004944a5 | 2 | 1 | 0x00494480 |
| 0x00494cdb | 4 | 1 | 0x00494c80 |
| 0x00494ce3 | 4 | 1 | 0x00494c80 |
| 0x00494d2c | 4 | 1 | 0x00494c80 |
| 0x00495408 | 4 | 1 | 0x00495350 |
| 0x00495fed | 6 | 1 | 0x00495fe0 |
| 0x00495ff2 | 6 | 1 | 0x00495fe0 |
| 0x00496008 | 6 | 1 | 0x00495fe0 |
| 0x00496717 | 6 | 1 | 0x00496530 |
| 0x004972bc | 6 | 1 | 0x004972b0 |
| 0x004972d5 | 6 | 1 | 0x004972b0 |
| 0x004972f5 | 6 | 1 | 0x004972b0 |
| 0x004972ff | 6 | 1 | 0x004972b0 |
| 0x00497307 | 6 | 1 | 0x004972b0 |
| 0x00498809 | 5 | 1 | 0x004987b0 |
| 0x0049de57 | 7 | 1 | 0x0049dd60 |
| 0x004a2b6f | 2 | 1 | 0x004a2b60 |
| 0x004a2b73 | 2 | 1 | 0x004a2b60 |
| 0x004a2b97 | 2 | 1 | 0x004a2b60 |
| 0x004b3bff | 4 | 1 | 0x004b3bf0 |
| 0x004b3d31 | 5 | 1 | 0x004b3d20 |
| 0x004b3d45 | 5 | 1 | 0x004b3d20 |
| 0x004b3d5e | 5 | 1 | 0x004b3d20 |
| 0x004b3e54 | 5 | 1 | 0x004b3e40 |
| 0x004b4646 | 8 | 1 | 0x004b4550 |
| 0x004b5300 | 5 | 1 | 0x004b5320 |
| 0x004b5335 | 5 | 1 | 0x004b5320 |
| 0x004b5560 | 5 | 1 | 0x004b5580 |
| 0x004b5584 | 5 | 1 | 0x004b5580 |
| 0x004b6544 | 5 | 1 | 0x004b6540 |
| 0x004b6564 | 5 | 1 | 0x004b6560 |
| 0x004b73e0 | 6 | 1 | 0x004b7330 |
| 0x004b79a0 | 8 | 1 | 0x004b7960 |
| 0x004c0560 | 6 | 1 | 0x004c0510 |
| 0x004c1483 | 4 | 1 | 0x004c1480 |
| 0x004c148c | 4 | 1 | 0x004c1480 |
| 0x004c1496 | 4 | 1 | 0x004c1480 |
| 0x004c149a | 4 | 1 | 0x004c1480 |
| 0x004c14a5 | 4 | 1 | 0x004c1480 |
| 0x004c14b1 | 4 | 1 | 0x004c1480 |
| 0x004c14be | 4 | 1 | 0x004c1480 |
| 0x004c14ca | 4 | 1 | 0x004c1480 |
| 0x004c14d6 | 4 | 1 | 0x004c1480 |
| 0x004c1511 | 4 | 1 | 0x004c1480 |
| 0x004c1a08 | 3 | 1 | 0x004c1a00 |
| 0x004c3d97 | 4 | 1 | 0x004c3d90 |
| 0x004c3e1a | 1 | 1 | 0x004c3df0 |
| 0x004c4de1 | 4 | 1 | 0x004c4dc0 |
| 0x004c4e7e | 4 | 1 | 0x004c4dc0 |
| 0x004c4e88 | 4 | 1 | 0x004c4dc0 |
| 0x004c4ea0 | 4 | 1 | 0x004c4dc0 |
| 0x004c7663 | 3 | 1 | 0x004c7650 |
| 0x004c7679 | 3 | 1 | 0x004c7650 |
| 0x004c773b | 6 | 1 | 0x004c7730 |
| 0x004c7742 | 6 | 1 | 0x004c7730 |
| 0x004cc601 | 1 | 1 | 0x004cc5e0 |
| 0x004cc624 | 1 | 1 | 0x004cc5e0 |
| 0x004cc637 | 1 | 1 | 0x004cc5e0 |
| 0x004cc664 | 1 | 1 | 0x004cc5e0 |
| 0x004cc66e | 1 | 1 | 0x004cc5e0 |
| 0x004d7ff4 | 2 | 1 | 0x004d7ff0 |
| 0x004e4324 | 5 | 1 | 0x004e4320 |
| 0x004e4329 | 5 | 1 | 0x004e4320 |
| 0x004e433e | 5 | 1 | 0x004e4320 |
| 0x004e4346 | 5 | 1 | 0x004e4320 |
| 0x004e4354 | 5 | 1 | 0x004e4350 |
| 0x004e435a | 5 | 1 | 0x004e4350 |
| 0x004e4360 | 5 | 1 | 0x004e4350 |
| 0x004e4366 | 5 | 1 | 0x004e4350 |
| 0x004e4370 | 5 | 1 | 0x004e4350 |
| 0x00556cf3 | 9 | 1 | 0x00556ce0 |
| 0x00556cf8 | 9 | 1 | 0x00556ce0 |
| 0x00556d00 | 9 | 1 | 0x00556ce0 |
| 0x00556d0e | 9 | 1 | 0x00556ce0 |
| 0x00556d2d | 10 | 1 | 0x00556d20 |
| 0x00556d4a | 10 | 1 | 0x00556d20 |
| 0x00556d67 | 10 | 1 | 0x00556d20 |
| 0x00556d7e | 5 | 1 | 0x00556d70 |
| 0x00556d97 | 5 | 1 | 0x00556d70 |
| 0x00556db2 | 5 | 1 | 0x00556d70 |
| 0x00556dc7 | 5 | 1 | 0x00556d70 |
| 0x00556dce | 5 | 1 | 0x00556d70 |
| 0x00556de3 | 5 | 1 | 0x00556d70 |
| 0x00556de8 | 5 | 1 | 0x00556d70 |
| 0x00556df4 | 5 | 1 | 0x00556d70 |
| 0x00556e00 | 5 | 1 | 0x00556d70 |
| 0x00556e06 | 5 | 1 | 0x00556d70 |
| 0x005571c9 | 9 | 1 | 0x005571c0 |
| 0x005571d3 | 9 | 1 | 0x005571c0 |
| 0x005571d8 | 9 | 1 | 0x005571c0 |
| 0x005571eb | 10 | 1 | 0x005571e0 |
| 0x005571ff | 10 | 1 | 0x005571e0 |
| 0x00557210 | 10 | 1 | 0x005571e0 |
| 0x00557229 | 9 | 1 | 0x00557220 |
| 0x00557233 | 9 | 1 | 0x00557220 |
| 0x0055723b | 9 | 1 | 0x00557220 |
| 0x00557241 | 9 | 1 | 0x00557220 |
| 0x00557246 | 9 | 1 | 0x00557220 |
| 0x00557257 | 10 | 1 | 0x00557250 |
| 0x0055727e | 10 | 1 | 0x00557250 |
| 0x005572a2 | 10 | 1 | 0x00557250 |
| 0x005581f5 | 7 | 1 | 0x005581f0 |
| 0x005581fa | 7 | 1 | 0x005581f0 |
| 0x005581fb | 7 | 1 | 0x005581f0 |
| 0x00558200 | 7 | 1 | 0x005581f0 |
| 0x00558209 | 7 | 1 | 0x005581f0 |
| 0x00558210 | 7 | 1 | 0x005581f0 |
| 0x00558235 | 7 | 1 | 0x005581f0 |
| 0x005a5f9d | 7 | 1 | 0x005a5f60 |
| 0x005a5fd0 | 6 | 1 | 0x005a5f00 |
| 0x005a6640 | 9 | 1 | 0x005a6340 |
| 0x005a66c0 | 9 | 1 | 0x005a6340 |
| 0x005a73a0 | 6 | 1 | 0x005a71f0 |
| 0x005aa180 | 7 | 1 | 0x005a9ff0 |
| 0x005aa1e4 | 7 | 1 | 0x005aa1e0 |
| 0x005aa1e8 | 7 | 1 | 0x005aa1e0 |
| 0x005aa1e9 | 7 | 1 | 0x005aa1e0 |
| 0x005aa1eb | 7 | 1 | 0x005aa1e0 |
| 0x005aa1ec | 7 | 1 | 0x005aa1e0 |
| 0x005aa1f1 | 7 | 1 | 0x005aa1e0 |
| 0x005aa1f4 | 7 | 1 | 0x005aa1e0 |
| 0x005aa1f6 | 7 | 1 | 0x005aa1e0 |
| 0x005aa1f8 | 7 | 1 | 0x005aa1e0 |
| 0x005aa1f9 | 7 | 1 | 0x005aa1e0 |
| 0x005aa1fa | 7 | 1 | 0x005aa1e0 |
| 0x005aa1ff | 7 | 1 | 0x005aa1e0 |
| 0x005ab063 | 8 | 1 | 0x005ab040 |
| 0x005ace44 | 8 | 1 | 0x005acda0 |
| 0x005aea06 | 7 | 1 | 0x005aea00 |
| 0x005aea0a | 7 | 1 | 0x005aea00 |
| 0x005af140 | 6 | 1 | 0x005af070 |
| 0x005af780 | 5 | 1 | 0x005af740 |
| 0x005af9a7 | 5 | 1 | 0x005af8f0 |
| 0x005af9b0 | 5 | 1 | 0x005af8f0 |
| 0x005afbd7 | 6 | 1 | 0x005afa00 |
| 0x005b0a70 | 7 | 1 | 0x005b0970 |
| 0x005c4d19 | 2 | 1 | 0x004d7ff0 |
| 0x005ca3be | 7 | 1 | 0x0049dd60 |
| 0x005cc358 | 7 | 1 | 0x00489500 |
| 0x005cc470 | 3 | 1 | 0x00402750 |
| 0x005cc4b8 | 3 | 1 | 0x00402b70 |
| 0x005cc4bc | 3 | 1 | 0x00402b70 |
| 0x005cc4dc | 7 | 1 | 0x004260e0 |
| 0x005cc554 | 3 | 1 | 0x00402b70 |
| 0x005cc734 | 2 | 1 | 0x0045a950 |
| 0x005cc740 | 12 | 1 | 0x004046a0 |
| 0x005cc744 | 12 | 1 | 0x004046a0 |
| 0x005cc748 | 12 | 1 | 0x004046a0 |
| 0x005cc74c | 12 | 1 | 0x004046a0 |
| 0x005cc754 | 8 | 1 | 0x0046b540 |
| 0x005cc7dc | 3 | 1 | 0x004099a0 |
| 0x005cc8f4 | 1 | 1 | 0x004177b0 |
| 0x005cc980 | 9 | 1 | 0x00407800 |
| 0x005cc994 | 10 | 1 | 0x00405ab0 |
| 0x005cc998 | 10 | 1 | 0x00405ab0 |
| 0x005cc9a8 | 10 | 1 | 0x00405ab0 |
| 0x005cc9bc | 14 | 1 | 0x00487020 |
| 0x005cc9d0 | 8 | 1 | 0x00473ee0 |
| 0x005cc9d4 | 10 | 1 | 0x004069d0 |
| 0x005cc9d8 | 10 | 1 | 0x004069d0 |
| 0x005cc9dc | 4 | 1 | 0x00458a40 |
| 0x005cca40 | 5 | 1 | 0x00407e20 |
| 0x005ccad8 | 4 | 1 | 0x004093b0 |
| 0x005ccca8 | 4 | 1 | 0x0040bbb0 |
| 0x005ccd48 | 1 | 1 | 0x0040eee0 |
| 0x005ccd98 | 1 | 1 | 0x0040eee0 |
| 0x005ccdb4 | 1 | 1 | 0x0040eee0 |
| 0x005ccdcc | 1 | 1 | 0x0040eee0 |
| 0x005cd004 | 7 | 1 | 0x00412050 |
| 0x005cd090 | 2 | 1 | 0x00414490 |
| 0x005cd0a0 | 1 | 1 | 0x004148b0 |
| 0x005cd0a4 | 1 | 1 | 0x004148b0 |
| 0x005cd0a8 | 1 | 1 | 0x004148b0 |
| 0x005cd0bc | 1 | 1 | 0x00415220 |
| 0x005cd0c0 | 1 | 1 | 0x00415220 |
| 0x005cd0d0 | 1 | 1 | 0x00415e20 |
| 0x005cd114 | 5 | 1 | 0x0045ae80 |
| 0x005cd120 | 3 | 1 | 0x00429bd0 |
| 0x005cd18c | 1 | 1 | 0x0043dfd0 |
| 0x005cd230 | 6 | 1 | 0x00462f50 |
| 0x005cd234 | 6 | 1 | 0x00462f50 |
| 0x005cd238 | 6 | 1 | 0x00462f50 |
| 0x005cd280 | 6 | 1 | 0x00426e10 |
| 0x005cd29c | 5 | 1 | 0x0042a5d0 |
| 0x005cd2b4 | 7 | 1 | 0x00421100 |
| 0x005cd324 | 2 | 1 | 0x004233e0 |
| 0x005cd40c | 2 | 1 | 0x004252c0 |
| 0x005cd4ac | 7 | 1 | 0x004260e0 |
| 0x005cd640 | 4 | 1 | 0x004283a0 |
| 0x005cd694 | 6 | 1 | 0x0046c7d0 |
| 0x005cd6cc | 3 | 1 | 0x00429bd0 |
| 0x005cd6d0 | 3 | 1 | 0x00429bd0 |
| 0x005cd6d4 | 3 | 1 | 0x00429bd0 |
| 0x005cd6d8 | 3 | 1 | 0x00429bd0 |
| 0x005cd6e0 | 3 | 1 | 0x00429bd0 |
| 0x005cd75c | 7 | 1 | 0x0042a8d0 |
| 0x005cd784 | 9 | 1 | 0x004414b0 |
| 0x005cd7a8 | 3 | 1 | 0x00459000 |
| 0x005cd984 | 6 | 1 | 0x0046c7d0 |
| 0x005ce01c | 9 | 1 | 0x004414b0 |
| 0x005ce020 | 9 | 1 | 0x004414b0 |
| 0x005ce024 | 9 | 1 | 0x004414b0 |
| 0x005ce184 | 9 | 1 | 0x00445aa0 |
| 0x005ce1b0 | 3 | 1 | 0x004568d0 |
| 0x005ce26c | 6 | 1 | 0x00463640 |
| 0x005ce2f4 | 3 | 1 | 0x00459000 |
| 0x005ce4d4 | 3 | 1 | 0x004568d0 |
| 0x005ce4f0 | 2 | 1 | 0x0045a950 |
| 0x005ce540 | 3 | 1 | 0x00459000 |
| 0x005ce548 | 3 | 1 | 0x00459000 |
| 0x005ce5d4 | 14 | 1 | 0x00458b10 |
| 0x005ce6e0 | 6 | 1 | 0x00463640 |
| 0x005ce734 | 1 | 1 | 0x004625b0 |
| 0x005ce738 | 1 | 1 | 0x004625b0 |
| 0x005ce748 | 1 | 1 | 0x004625b0 |
| 0x005ce74c | 1 | 1 | 0x004625b0 |
| 0x005ce75c | 1 | 1 | 0x004625b0 |
| 0x005ce760 | 1 | 1 | 0x004625b0 |
| 0x005ce990 | 4 | 1 | 0x004633d0 |
| 0x005ce998 | 6 | 1 | 0x00463640 |
| 0x005ce9a0 | 6 | 1 | 0x00463640 |
| 0x005ce9a4 | 6 | 1 | 0x00463c80 |
| 0x005ce9a8 | 6 | 1 | 0x00463c80 |
| 0x005ce9ac | 6 | 1 | 0x00463c80 |
| 0x005ce9b0 | 6 | 1 | 0x00463f40 |
| 0x005ce9b4 | 6 | 1 | 0x00463f40 |
| 0x005ce9b8 | 7 | 1 | 0x00464670 |
| 0x005ce9bc | 7 | 1 | 0x00464670 |
| 0x005cea3c | 9 | 1 | 0x0046d7f0 |
| 0x005cea44 | 8 | 1 | 0x0046b540 |
| 0x005cea54 | 8 | 1 | 0x0046b540 |
| 0x005cea58 | 6 | 1 | 0x0046c7d0 |
| 0x005ceac8 | 8 | 1 | 0x00473ee0 |
| 0x005ceacc | 8 | 1 | 0x00473ee0 |
| 0x005cead0 | 8 | 1 | 0x00473ee0 |
| 0x005cead4 | 8 | 1 | 0x00474890 |
| 0x005ceae0 | 10 | 1 | 0x00475770 |
| 0x005ceae4 | 10 | 1 | 0x00475770 |
| 0x005ceb90 | 6 | 1 | 0x00496530 |
| 0x005cf4a8 | 5 | 1 | 0x0048bbe0 |
| 0x005cfac4 | 4 | 1 | 0x00494c80 |
| 0x005d0abc | 4 | 1 | 0x00494c80 |
| 0x005d0bbc | 4 | 1 | 0x00494c80 |
| 0x005d0bcc | 4 | 1 | 0x00494c80 |
| 0x005d0c4c | 4 | 1 | 0x00494c80 |
| 0x005d0d84 | 7 | 1 | 0x0049dd60 |
| 0x005d0dc0 | 7 | 1 | 0x0049dd60 |
| 0x005d0eb0 | 6 | 1 | 0x0049ec10 |
| 0x005d1280 | 2 | 1 | 0x004a504f |
| 0x005d12a0 | 2 | 1 | 0x004a504f |
| 0x005d8808 | 8 | 1 | 0x004b8340 |
| 0x005d8880 | 8 | 1 | 0x004b9540 |
| 0x005e614c | 9 | 1 | 0x005a6340 |
| 0x005e61a4 | 6 | 1 | 0x005a71f0 |
| 0x005e6354 | 6 | 1 | 0x005a71f0 |
| 0x005e6484 | 7 | 1 | 0x005ace70 |
| 0x005e6494 | 7 | 1 | 0x005ace70 |
| 0x005e64a4 | 7 | 1 | 0x005ace70 |
| 0x005e64c4 | 8 | 1 | 0x005acda0 |
| 0x005e6cf0 | 6 | 1 | 0x005afa00 |
| 0x005e6d50 | 6 | 1 | 0x005afa00 |
| 0x005e6d60 | 6 | 1 | 0x005afcf0 |
| 0x005e6d90 | 6 | 1 | 0x005afcf0 |
| 0x005e6de0 | 6 | 1 | 0x005afcf0 |
| 0x005e6e50 | 5 | 1 | 0x004627f0 |
| 0x005ea0b4 | 12 | 1 | 0x004046a0 |
| 0x005f1c18 | 5 | 1 | 0x0042a530 |
| 0x005f2568 | 8 | 1 | 0x0040b090 |
| 0x005f29d0 | 4 | 1 | 0x004113b0 |
| 0x005f2a70 | 2 | 1 | 0x00414120 |
| 0x005f2b10 | 1 | 1 | 0x00430670 |
| 0x005f2dd8 | 1 | 1 | 0x004148b0 |
| 0x005f2fc0 | 1 | 1 | 0x00415880 |
| 0x005f30a0 | 1 | 1 | 0x004177b0 |
| 0x005f3180 | 1 | 1 | 0x004177b0 |
| 0x005f3298 | 2 | 1 | 0x00419760 |
| 0x005f65b0 | 5 | 1 | 0x0042a5d0 |
| 0x005f65b4 | 6 | 1 | 0x0042a640 |
| 0x005f65b8 | 4 | 1 | 0x0042a6b0 |
| 0x005f76c4 | 1 | 1 | 0x0043dfd0 |
| 0x005f76c8 | 1 | 1 | 0x0043dfd0 |
| 0x005f7a40 | 9 | 1 | 0x004414b0 |
| 0x005f9730 | 9 | 1 | 0x0044c4f0 |
| 0x005f9838 | 3 | 1 | 0x00459000 |
| 0x005f9898 | 5 | 1 | 0x0045ae80 |
| 0x005f98a0 | 5 | 1 | 0x0045ae80 |
| 0x005f99b0 | 4 | 1 | 0x0045b930 |
| 0x005f99b4 | 4 | 1 | 0x0045bba0 |
| 0x005f99b8 | 13 | 1 | 0x0045b990 |
| 0x005f99bc | 14 | 1 | 0x0045b9d0 |
| 0x005f99c4 | 1 | 1 | 0x0045bed0 |
| 0x005f99c8 | 1 | 1 | 0x0045bf30 |
| 0x006036b8 | 6 | 1 | 0x004647f0 |
| 0x006036c4 | 5 | 1 | 0x00466b50 |
| 0x006036cc | 6 | 1 | 0x0045dfc0 |
| 0x006036d4 | 6 | 1 | 0x004644a0 |
| 0x006036e8 | 4 | 1 | 0x004669b0 |
| 0x00603768 | 4 | 1 | 0x004669b0 |
| 0x006037e8 | 4 | 1 | 0x004669b0 |
| 0x006041f0 | 1 | 1 | 0x004625b0 |
| 0x006044f0 | 5 | 1 | 0x0045d460 |
| 0x00604578 | 5 | 1 | 0x0045d460 |
| 0x00604580 | 6 | 1 | 0x00463640 |
| 0x00604aa4 | 7 | 1 | 0x00464670 |
| 0x00604d88 | 6 | 1 | 0x00464a50 |
| 0x00604f44 | 6 | 1 | 0x00462f50 |
| 0x006053e4 | 6 | 1 | 0x00463590 |
| 0x00605478 | 6 | 1 | 0x00462f50 |
| 0x0060575c | 6 | 1 | 0x004644a0 |
| 0x006059ac | 4 | 1 | 0x004633d0 |
| 0x00605a40 | 4 | 1 | 0x004633d0 |
| 0x00605b68 | 6 | 1 | 0x00464a50 |
| 0x00605bfc | 6 | 1 | 0x0045dd60 |
| 0x00613020 | 6 | 1 | 0x00463640 |
| 0x006130bc | 6 | 1 | 0x00463640 |
| 0x006130c0 | 6 | 1 | 0x00463640 |
| 0x006130c4 | 6 | 1 | 0x00463640 |
| 0x006130dc | 7 | 1 | 0x00464670 |
| 0x00613108 | 8 | 1 | 0x0046b540 |
| 0x00613114 | 8 | 1 | 0x0046b540 |
| 0x00613130 | 8 | 1 | 0x0046b540 |
| 0x00613134 | 6 | 1 | 0x0046c7d0 |
| 0x00613140 | 8 | 1 | 0x0046b540 |
| 0x00613148 | 8 | 1 | 0x0046b540 |
| 0x00613254 | 6 | 1 | 0x00475ef0 |
| 0x00613288 | 4 | 1 | 0x004769d0 |
| 0x00613290 | 4 | 1 | 0x004769f0 |
| 0x006146b0 | 8 | 1 | 0x00491590 |
| 0x006146c0 | 8 | 1 | 0x00491590 |
| 0x006146c4 | 8 | 1 | 0x00491590 |
| 0x006146c8 | 8 | 1 | 0x00491590 |
| 0x006146cc | 8 | 1 | 0x00491590 |
| 0x00614708 | 1 | 1 | 0x0046d510 |
| 0x00614718 | 2 | 1 | 0x004307a0 |
| 0x0061471c | 2 | 1 | 0x004307a0 |
| 0x00614720 | 2 | 1 | 0x004307a0 |
| 0x006147b4 | 7 | 1 | 0x004924c0 |
| 0x006147b5 | 7 | 1 | 0x004924c0 |
| 0x006147b6 | 7 | 1 | 0x004924c0 |
| 0x006147d8 | 2 | 1 | 0x00494480 |
| 0x006147e0 | 4 | 1 | 0x00494c80 |
| 0x006160d8 | 2 | 1 | 0x004a504f |
| 0x00616110 | 1 | 1 | 0x004a2cbd |
| 0x00617380 | 8 | 1 | 0x004b7aa0 |
| 0x00617530 | 6 | 1 | 0x004c0510 |
| 0x00617ff8 | 9 | 1 | 0x005a6340 |
| 0x00618180 | 3 | 1 | 0x004c7650 |
| 0x00623f60 | 10 | 1 | 0x00556d20 |
| 0x00623f64 | 10 | 1 | 0x00556d20 |
| 0x00623f68 | 10 | 1 | 0x005571e0 |
| 0x00623f6c | 10 | 1 | 0x005571e0 |
| 0x00623f70 | 10 | 1 | 0x00557250 |
| 0x00623f74 | 10 | 1 | 0x00557250 |
| 0x00623f78 | 10 | 1 | 0x00557250 |
| 0x00623f7c | 10 | 1 | 0x00557250 |
| 0x00632ba0 | 5 | 1 | 0x004627f0 |
| 0x00632c28 | 6 | 1 | 0x005a6110 |
| 0x00632c2c | 6 | 1 | 0x005a6110 |
| 0x00632c38 | 6 | 1 | 0x005a6110 |
| 0x00632c3c | 7 | 1 | 0x005a6150 |
| 0x00632c40 | 7 | 1 | 0x005a6150 |
| 0x00632c4c | 7 | 1 | 0x005a6150 |
| 0x00632f08 | 7 | 1 | 0x005a6f30 |
| 0x006351b8 | 7 | 1 | 0x005bbb70 |
| 0x006351e4 | 7 | 1 | 0x005bbb70 |
| 0x006351fc | 7 | 1 | 0x005bbb70 |
| 0x00636560 | 4 | 1 | 0x004013f0 |
| 0x00636574 | 4 | 1 | 0x00401340 |
| 0x00636ac0 | 5 | 1 | 0x004015a0 |
| 0x00636acc | 7 | 1 | 0x0041ffb0 |
| 0x00636ad8 | 2 | 1 | 0x00402f40 |
| 0x00636ae8 | 4 | 1 | 0x00402f50 |
| 0x00636aec | 4 | 1 | 0x00402f50 |
| 0x00636af0 | 4 | 1 | 0x00402f50 |
| 0x00636af4 | 4 | 1 | 0x00402f50 |
| 0x00636af8 | 4 | 1 | 0x00402f50 |
| 0x00636afc | 4 | 1 | 0x00402f50 |
| 0x00636b74 | 4 | 1 | 0x00403910 |
| 0x00636b88 | 12 | 1 | 0x004046a0 |
| 0x00636b8c | 12 | 1 | 0x004046a0 |
| 0x00636b98 | 12 | 1 | 0x004046a0 |
| 0x00636bf4 | 3 | 1 | 0x004039f0 |
| 0x00636c00 | 4 | 1 | 0x004039c0 |
| 0x00636c08 | 4 | 1 | 0x00404830 |
| 0x006390b0 | 4 | 1 | 0x00404830 |
| 0x00639cc8 | 4 | 1 | 0x00404830 |
| 0x00639cd0 | 4 | 1 | 0x00404830 |
| 0x00639cd4 | 4 | 1 | 0x00404830 |
| 0x00639cf4 | 4 | 1 | 0x00404830 |
| 0x00639d2c | 4 | 1 | 0x00404830 |
| 0x00639d30 | 4 | 1 | 0x00404830 |
| 0x00639d40 | 4 | 1 | 0x00404830 |
| 0x00639d4c | 4 | 1 | 0x00404830 |
| 0x00639d50 | 4 | 1 | 0x00404830 |
| 0x00639d58 | 4 | 1 | 0x00404830 |
| 0x00639d5c | 4 | 1 | 0x00404830 |
| 0x00639d60 | 4 | 1 | 0x00404830 |
| 0x00639d64 | 4 | 1 | 0x00404830 |
| 0x00639d98 | 9 | 1 | 0x00407800 |
| 0x00639dfc | 12 | 1 | 0x004072e0 |
| 0x00639e50 | 10 | 1 | 0x004073b0 |
| 0x0063a494 | 10 | 1 | 0x00406370 |
| 0x0063a498 | 12 | 1 | 0x00406160 |
| 0x0063a4a0 | 11 | 1 | 0x00406130 |
| 0x0063a4b4 | 10 | 1 | 0x00406370 |
| 0x0063a4d4 | 10 | 1 | 0x00406370 |
| 0x0063a4f4 | 10 | 1 | 0x00406370 |
| 0x0063a514 | 10 | 1 | 0x00406370 |
| 0x0063a534 | 10 | 1 | 0x00406370 |
| 0x0063a554 | 10 | 1 | 0x00406370 |
| 0x0063a574 | 10 | 1 | 0x00406370 |
| 0x0063a594 | 10 | 1 | 0x00406370 |
| 0x0063a5b4 | 10 | 1 | 0x00406370 |
| 0x0063a5f4 | 1 | 1 | 0x00409790 |
| 0x0063a5f8 | 1 | 1 | 0x00409790 |
| 0x0063b830 | 1 | 1 | 0x00409790 |
| 0x0063b838 | 9 | 1 | 0x0040aef0 |
| 0x0063b83c | 9 | 1 | 0x0040aef0 |
| 0x0063b840 | 9 | 1 | 0x0040aef0 |
| 0x0063b844 | 9 | 1 | 0x0040aef0 |
| 0x0063b848 | 9 | 1 | 0x0040aef0 |
| 0x0063b84c | 9 | 1 | 0x0040aef0 |
| 0x0063b850 | 9 | 1 | 0x0040aef0 |
| 0x0063b854 | 9 | 1 | 0x0040aef0 |
| 0x0063b858 | 9 | 1 | 0x0040aef0 |
| 0x0063b85c | 9 | 1 | 0x0040aef0 |
| 0x0063b8e8 | 8 | 1 | 0x0040b090 |
| 0x0063b8f0 | 4 | 1 | 0x0040bbb0 |
| 0x0063b8f4 | 4 | 1 | 0x0040bbb0 |
| 0x0063b90c | 1 | 1 | 0x00410510 |
| 0x0063b918 | 1 | 1 | 0x00410510 |
| 0x0063b91c | 1 | 1 | 0x00410510 |
| 0x0063b9b8 | 7 | 1 | 0x0040e590 |
| 0x0063ba80 | 7 | 1 | 0x0040e590 |
| 0x0063ba88 | 8 | 1 | 0x0040d470 |
| 0x0063bb3c | 4 | 1 | 0x00411f30 |
| 0x0063bc34 | 4 | 1 | 0x00411f30 |
| 0x0063bc54 | 4 | 1 | 0x00412890 |
| 0x0063bc60 | 3 | 1 | 0x00413c70 |
| 0x0063bc68 | 3 | 1 | 0x00413c70 |
| 0x0063bd90 | 1 | 1 | 0x00417180 |
| 0x0063bf30 | 2 | 1 | 0x00419760 |
| 0x0063bfd8 | 2 | 1 | 0x00419760 |
| 0x0063c018 | 2 | 1 | 0x00419760 |
| 0x0063c028 | 2 | 1 | 0x00419760 |
| 0x0063c04c | 2 | 1 | 0x00419760 |
| 0x0063c06c | 2 | 1 | 0x00419760 |
| 0x0063c610 | 4 | 1 | 0x0041a1e0 |
| 0x0063c614 | 4 | 1 | 0x0041a1e0 |
| 0x0063c624 | 4 | 1 | 0x0041a1e0 |
| 0x0063c628 | 4 | 1 | 0x0041a1e0 |
| 0x0063c62b | 4 | 1 | 0x0041a1e0 |
| 0x0063caa0 | 4 | 1 | 0x0041b450 |
| 0x0063cd90 | 4 | 1 | 0x0041bec0 |
| 0x0063cda4 | 4 | 1 | 0x0041c100 |
| 0x0063cda8 | 4 | 1 | 0x0041c100 |
| 0x0063cdb8 | 4 | 1 | 0x0041c100 |
| 0x0063cdbc | 4 | 1 | 0x0041c100 |
| 0x0063cdd0 | 7 | 1 | 0x0041ccf0 |
| 0x0063ce20 | 7 | 1 | 0x0041ccf0 |
| 0x0063cf28 | 7 | 1 | 0x0041ccf0 |
| 0x0063d270 | 7 | 1 | 0x0041ccf0 |
| 0x0063d55c | 4 | 1 | 0x0041d8b0 |
| 0x0063d574 | 4 | 1 | 0x0041d8b0 |
| 0x0063d580 | 4 | 1 | 0x0041d8b0 |
| 0x0063d588 | 7 | 1 | 0x0041da90 |
| 0x0063d5a0 | 4 | 1 | 0x0041db90 |
| 0x0063d5e4 | 4 | 1 | 0x0041db90 |
| 0x0063d5e8 | 4 | 1 | 0x0041db90 |
| 0x0063d7e0 | 3 | 1 | 0x0041e140 |
| 0x0063d7e8 | 3 | 1 | 0x0041e140 |
| 0x0063d840 | 7 | 1 | 0x0041ffb0 |
| 0x0063d850 | 4 | 1 | 0x0041eaa0 |
| 0x0063d854 | 7 | 1 | 0x0041ffb0 |
| 0x0063d858 | 7 | 1 | 0x0041ffb0 |
| 0x0063da38 | 5 | 1 | 0x0041f060 |
| 0x0063dc38 | 2 | 1 | 0x0041f030 |
| 0x0063dc48 | 7 | 1 | 0x0041efc0 |
| 0x0063dc4c | 12 | 1 | 0x0041f220 |
| 0x0063e490 | 7 | 1 | 0x0041ffb0 |
| 0x0063e494 | 7 | 1 | 0x0041ffb0 |
| 0x0063e4bc | 7 | 1 | 0x00420d80 |
| 0x0063e4cc | 4 | 1 | 0x0041eaa0 |
| 0x0063e4d0 | 4 | 1 | 0x00420d00 |
| 0x0063e4f4 | 4 | 1 | 0x00420d00 |
| 0x0063e518 | 4 | 1 | 0x00420d00 |
| 0x0063e53c | 4 | 1 | 0x00420d00 |
| 0x0063e588 | 6 | 1 | 0x00421160 |
| 0x0063e58c | 6 | 1 | 0x00421160 |
| 0x0063e590 | 6 | 1 | 0x00421160 |
| 0x0063e59c | 6 | 1 | 0x00421160 |
| 0x0063e5a0 | 6 | 1 | 0x00421160 |
| 0x0063e5a8 | 6 | 1 | 0x00421160 |
| 0x0063e5ac | 6 | 1 | 0x00421160 |
| 0x0063e5b0 | 6 | 1 | 0x00421160 |
| 0x0064410c | 2 | 1 | 0x00423480 |
| 0x00644150 | 4 | 1 | 0x00425bc0 |
| 0x00646c38 | 7 | 1 | 0x004262f0 |
| 0x00646e38 | 7 | 1 | 0x004262f0 |
| 0x00656ed8 | 6 | 1 | 0x00426e10 |
| 0x00657408 | 7 | 1 | 0x004262f0 |
| 0x0065742c | 5 | 1 | 0x00426060 |
| 0x00657430 | 6 | 1 | 0x00426e10 |
| 0x00657448 | 6 | 1 | 0x00426e10 |
| 0x0065744c | 6 | 1 | 0x00426e10 |
| 0x0065744d | 6 | 1 | 0x00426e10 |
| 0x0065744e | 6 | 1 | 0x00426e10 |
| 0x00657450 | 6 | 1 | 0x00426e10 |
| 0x00657454 | 6 | 1 | 0x00426e10 |
| 0x00657498 | 6 | 1 | 0x00426e10 |
| 0x00663658 | 1 | 1 | 0x00426cc0 |
| 0x0066d6fb | 7 | 1 | 0x00426cd0 |
| 0x0066d828 | 4 | 1 | 0x004274e0 |
| 0x0067d828 | 4 | 1 | 0x004275d0 |
| 0x0067d84c | 4 | 1 | 0x004274e0 |
| 0x0067d98c | 2 | 1 | 0x004298c0 |
| 0x0067d994 | 2 | 1 | 0x004298c0 |
| 0x0067d99c | 2 | 1 | 0x004298c0 |
| 0x0067d9a8 | 5 | 1 | 0x0042a5d0 |
| 0x0067dba8 | 6 | 1 | 0x0042a640 |
| 0x0067dca8 | 7 | 1 | 0x0042a8d0 |
| 0x0067e3a8 | 6 | 1 | 0x0042a640 |
| 0x0067e6a8 | 5 | 1 | 0x0042a5d0 |
| 0x0067e840 | 1 | 1 | 0x0043dfd0 |
| 0x0067ea5c | 2 | 1 | 0x0042bf30 |
| 0x0067ea7c | 1 | 1 | 0x00431d80 |
| 0x0067ea98 | 1 | 1 | 0x00430670 |
| 0x0067eab4 | 2 | 1 | 0x0042bf30 |
| 0x0067eab8 | 2 | 1 | 0x0042bf30 |
| 0x0067eac0 | 2 | 1 | 0x0042bf30 |
| 0x0067eac8 | 2 | 1 | 0x0042bf30 |
| 0x0067eacc | 2 | 1 | 0x0042bf30 |
| 0x0067ead0 | 2 | 1 | 0x0042bf30 |
| 0x0067ead4 | 2 | 1 | 0x0042bf30 |
| 0x0067eca8 | 1 | 1 | 0x0043dfd0 |
| 0x0067ed74 | 2 | 1 | 0x0042aa00 |
| 0x0067ed7c | 1 | 1 | 0x0043dfd0 |
| 0x0067ed84 | 2 | 1 | 0x0042aa00 |
| 0x0067f18c | 6 | 1 | 0x00402a40 |
| 0x0067f190 | 6 | 1 | 0x00402a40 |
| 0x0067f19c | 7 | 1 | 0x0042f760 |
| 0x0067f1a0 | 7 | 1 | 0x0042f770 |
| 0x0067f1a4 | 7 | 1 | 0x0042f780 |
| 0x00684de0 | 1 | 1 | 0x00452eb0 |
| 0x00684e38 | 14 | 1 | 0x00452f30 |
| 0x00684ea8 | 14 | 1 | 0x00452f30 |
| 0x006870a8 | 13 | 1 | 0x00453eb0 |
| 0x006870ac | 13 | 1 | 0x00453eb0 |
| 0x006870b0 | 13 | 1 | 0x00453eb0 |
| 0x00687eb8 | 13 | 1 | 0x00453eb0 |
| 0x00687ec4 | 4 | 1 | 0x00453f30 |
| 0x00688250 | 2 | 1 | 0x00454a30 |
| 0x00688300 | 2 | 1 | 0x00454a30 |
| 0x00688304 | 2 | 1 | 0x00454a30 |
| 0x006885e0 | 2 | 1 | 0x00455b40 |
| 0x006886b8 | 12 | 1 | 0x00456040 |
| 0x006886bc | 12 | 1 | 0x00456040 |
| 0x006886c0 | 12 | 1 | 0x00456040 |
| 0x006886dc | 3 | 1 | 0x004568d0 |
| 0x006887e4 | 12 | 1 | 0x00456c30 |
| 0x006887fc | 3 | 1 | 0x004568d0 |
| 0x0068a254 | 4 | 1 | 0x004576b0 |
| 0x0068a274 | 4 | 1 | 0x004576b0 |
| 0x0068a300 | 4 | 1 | 0x004576b0 |
| 0x0068b1a0 | 4 | 1 | 0x004587a0 |
| 0x0068b1b8 | 2 | 1 | 0x00458f20 |
| 0x0068b1d0 | 14 | 1 | 0x00458b10 |
| 0x0068b970 | 4 | 1 | 0x004587a0 |
| 0x0068b9ac | 4 | 1 | 0x004587a0 |
| 0x0068b9f8 | 5 | 1 | 0x0045a190 |
| 0x0068b9fc | 13 | 1 | 0x004595c0 |
| 0x0068ba00 | 2 | 1 | 0x0045a0f0 |
| 0x0068ba0c | 5 | 1 | 0x0045a190 |
| 0x0068ba1c | 4 | 1 | 0x0045a110 |
| 0x0068ba2c | 13 | 1 | 0x004595c0 |
| 0x0068bb60 | 5 | 1 | 0x0045ae80 |
| 0x0068bb64 | 5 | 1 | 0x0045ae80 |
| 0x0068bb90 | 3 | 1 | 0x00459290 |
| 0x0068bbac | 14 | 1 | 0x0045a130 |
| 0x0068bd04 | 5 | 1 | 0x0045ae80 |
| 0x0068bd0c | 2 | 1 | 0x0045a950 |
| 0x0068bd18 | 5 | 1 | 0x0045ae80 |
| 0x0068bd28 | 2 | 1 | 0x0045a950 |
| 0x0068bd30 | 2 | 1 | 0x0045a950 |
| 0x0068d54c | 7 | 1 | 0x0045c810 |
| 0x0068d76c | 5 | 1 | 0x0045ae80 |
| 0x0068f4c0 | 6 | 1 | 0x00463c80 |
| 0x0068f620 | 6 | 1 | 0x0045d7a0 |
| 0x0068f624 | 6 | 1 | 0x0045d7a0 |
| 0x0068f628 | 6 | 1 | 0x0045d7a0 |
| 0x0068f634 | 6 | 1 | 0x0045d7a0 |
| 0x0068f660 | 6 | 1 | 0x00463640 |
| 0x0068fc90 | 6 | 1 | 0x00463c80 |
| 0x0068fc94 | 6 | 1 | 0x004623e0 |
| 0x0068fc98 | 1 | 1 | 0x00462dd0 |
| 0x0068fc9c | 1 | 1 | 0x00462dd0 |
| 0x0068fca4 | 1 | 1 | 0x00462dd0 |
| 0x0068fcb0 | 1 | 1 | 0x00462dd0 |
| 0x0068fcb4 | 1 | 1 | 0x00462dd0 |
| 0x0068fcb8 | 1 | 1 | 0x00462dd0 |
| 0x0068fcbc | 4 | 1 | 0x004669b0 |
| 0x0068fccc | 4 | 1 | 0x004669b0 |
| 0x006900d0 | 4 | 1 | 0x004669b0 |
| 0x006904b0 | 5 | 1 | 0x00466b50 |
| 0x006904b4 | 6 | 1 | 0x00463640 |
| 0x006904f4 | 5 | 1 | 0x00466b50 |
| 0x006904f8 | 5 | 1 | 0x00466b50 |
| 0x006904fc | 5 | 1 | 0x00466b50 |
| 0x00690500 | 6 | 1 | 0x004644a0 |
| 0x00690510 | 5 | 1 | 0x00466b50 |
| 0x00690514 | 5 | 1 | 0x00466b50 |
| 0x00690518 | 5 | 1 | 0x00466b50 |
| 0x0069051c | 5 | 1 | 0x00466b50 |
| 0x00690524 | 4 | 1 | 0x004633d0 |
| 0x00690528 | 4 | 1 | 0x004633d0 |
| 0x0069052c | 6 | 1 | 0x00463640 |
| 0x0069054c | 6 | 1 | 0x00463640 |
| 0x0069055c | 6 | 1 | 0x00463640 |
| 0x00690560 | 6 | 1 | 0x004642f0 |
| 0x006905b4 | 7 | 1 | 0x00467070 |
| 0x006905b8 | 7 | 1 | 0x00467070 |
| 0x006905cc | 7 | 1 | 0x00471490 |
| 0x0069060c | 7 | 1 | 0x00471490 |
| 0x00690a30 | 6 | 1 | 0x00471560 |
| 0x00690ab0 | 5 | 1 | 0x00471530 |
| 0x00691480 | 4 | 1 | 0x00471df0 |
| 0x00691484 | 5 | 1 | 0x00471530 |
| 0x0069150c | 8 | 1 | 0x004725f0 |
| 0x00691510 | 4 | 1 | 0x004723d0 |
| 0x00691520 | 4 | 1 | 0x004723d0 |
| 0x00691530 | 4 | 1 | 0x004723d0 |
| 0x00691540 | 4 | 1 | 0x004723d0 |
| 0x00691550 | 4 | 1 | 0x004723d0 |
| 0x00691560 | 4 | 1 | 0x004723d0 |
| 0x00691570 | 4 | 1 | 0x004723d0 |
| 0x00691580 | 4 | 1 | 0x004723d0 |
| 0x00691590 | 4 | 1 | 0x004723d0 |
| 0x0069159c | 4 | 1 | 0x004723d0 |
| 0x0069160c | 5 | 1 | 0x00475a00 |
| 0x0069183c | 6 | 1 | 0x00476710 |
| 0x006924e0 | 4 | 1 | 0x004769d0 |
| 0x006924e8 | 4 | 1 | 0x00476d00 |
| 0x0069252c | 4 | 1 | 0x004769a0 |
| 0x00692530 | 4 | 1 | 0x004769a0 |
| 0x00692534 | 4 | 1 | 0x00476d00 |
| 0x00692564 | 2 | 1 | 0x0045ba10 |
| 0x0069259c | 7 | 1 | 0x00476a40 |
| 0x006925a0 | 7 | 1 | 0x00476a40 |
| 0x006925a4 | 7 | 1 | 0x00476a40 |
| 0x006925a8 | 4 | 1 | 0x004769a0 |
| 0x00693198 | 6 | 1 | 0x00477b40 |
| 0x006bf198 | 6 | 1 | 0x00477b40 |
| 0x006bf1c8 | 5 | 1 | 0x0047a020 |
| 0x006bf1cc | 5 | 1 | 0x0047a020 |
| 0x006bf1d0 | 5 | 1 | 0x0047a020 |
| 0x006bf1d4 | 5 | 1 | 0x0047a020 |
| 0x006bf1d8 | 5 | 1 | 0x0047a0f0 |
| 0x006bf1dc | 5 | 1 | 0x0047a0f0 |
| 0x006bfca8 | 7 | 1 | 0x0047c0f0 |
| 0x006c27a8 | 8 | 1 | 0x0047c160 |
| 0x006c2fa8 | 8 | 1 | 0x0047c160 |
| 0x006c2fec | 7 | 1 | 0x0047c0f0 |
| 0x006c6870 | 8 | 1 | 0x0047cdc0 |
| 0x006c6b90 | 8 | 1 | 0x0047ce40 |
| 0x006c71d8 | 10 | 1 | 0x0047d130 |
| 0x006ce840 | 4 | 1 | 0x004841d0 |
| 0x006dccbc | 8 | 1 | 0x00484c90 |
| 0x006dccd4 | 3 | 1 | 0x00484cf0 |
| 0x006dccd8 | 3 | 1 | 0x00484cf0 |
| 0x006dcd1c | 3 | 1 | 0x00484cf0 |
| 0x006dcd20 | 3 | 1 | 0x00484cf0 |
| 0x006e70c8 | 3 | 1 | 0x00484cf0 |
| 0x006e70d4 | 3 | 1 | 0x00484cf0 |
| 0x006fa5d0 | 4 | 1 | 0x00486460 |
| 0x006fa5e4 | 4 | 1 | 0x00486460 |
| 0x006fa5e8 | 4 | 1 | 0x00486460 |
| 0x006fa5ec | 4 | 1 | 0x00486460 |
| 0x006fa5f0 | 4 | 1 | 0x00486460 |
| 0x006fa5f8 | 4 | 1 | 0x00486460 |
| 0x006fa5fc | 4 | 1 | 0x00486460 |
| 0x006fa600 | 4 | 1 | 0x00486460 |
| 0x006fa604 | 4 | 1 | 0x00486460 |
| 0x006fa610 | 4 | 1 | 0x00486460 |
| 0x006fa658 | 4 | 1 | 0x00486460 |
| 0x006fa65c | 4 | 1 | 0x00486460 |
| 0x006fde14 | 6 | 1 | 0x00485e10 |
| 0x006fe048 | 5 | 1 | 0x00485d90 |
| 0x006fe780 | 7 | 1 | 0x004864f0 |
| 0x006fe784 | 7 | 1 | 0x004864f0 |
| 0x006fe788 | 7 | 1 | 0x004864f0 |
| 0x006fe800 | 6 | 1 | 0x00486f90 |
| 0x006fe804 | 5 | 1 | 0x004862d0 |
| 0x006fe938 | 5 | 1 | 0x00485d90 |
| 0x00703038 | 6 | 1 | 0x00486f90 |
| 0x00703068 | 6 | 1 | 0x00487140 |
| 0x007030a8 | 6 | 1 | 0x00486f90 |
| 0x007030ac | 5 | 1 | 0x004862d0 |
| 0x00703118 | 6 | 1 | 0x004892c0 |
| 0x00705a10 | 5 | 1 | 0x00489290 |
| 0x00713198 | 5 | 1 | 0x0048a460 |
| 0x00715220 | 12 | 1 | 0x0048a850 |
| 0x00715230 | 12 | 1 | 0x0048a850 |
| 0x00715234 | 12 | 1 | 0x0048a850 |
| 0x00715238 | 12 | 1 | 0x0048a850 |
| 0x0071523c | 12 | 1 | 0x0048a850 |
| 0x00715240 | 12 | 1 | 0x0048a850 |
| 0x00715244 | 12 | 1 | 0x0048a850 |
| 0x00715248 | 12 | 1 | 0x0048a850 |
| 0x0071524b | 12 | 1 | 0x0048a850 |
| 0x0071524c | 12 | 1 | 0x0048a850 |
| 0x00715250 | 12 | 1 | 0x0048a850 |
| 0x00715254 | 12 | 1 | 0x0048a850 |
| 0x00715258 | 12 | 1 | 0x0048a850 |
| 0x00722140 | 5 | 1 | 0x0048e820 |
| 0x00722144 | 5 | 1 | 0x0048e820 |
| 0x00722250 | 5 | 1 | 0x0048e820 |
| 0x007268e4 | 5 | 1 | 0x0048e820 |
| 0x007668b0 | 6 | 1 | 0x0048cee0 |
| 0x00766f40 | 8 | 1 | 0x0048f740 |
| 0x0076a100 | 8 | 1 | 0x0048f680 |
| 0x00771528 | 9 | 1 | 0x0048f6e0 |
| 0x0077152c | 8 | 1 | 0x00491590 |
| 0x00771530 | 8 | 1 | 0x00491590 |
| 0x00771538 | 8 | 1 | 0x00491590 |
| 0x0077153c | 8 | 1 | 0x00491590 |
| 0x00771968 | 4 | 1 | 0x00492d10 |
| 0x007719e8 | 4 | 1 | 0x004274d0 |
| 0x007719ec | 5 | 1 | 0x00493f80 |
| 0x007719f0 | 5 | 1 | 0x00493f80 |
| 0x007719f4 | 5 | 1 | 0x00493f80 |
| 0x007719f8 | 5 | 1 | 0x00493f80 |
| 0x00771a0c | 3 | 1 | 0x00402750 |
| 0x00771a14 | 3 | 1 | 0x00494320 |
| 0x00771a20 | 3 | 1 | 0x00494320 |
| 0x00771a24 | 3 | 1 | 0x00494320 |
| 0x00771a30 | 3 | 1 | 0x00494320 |
| 0x00771a34 | 3 | 1 | 0x00494320 |
| 0x00771a38 | 4 | 1 | 0x00494c80 |
| 0x00771a3c | 4 | 1 | 0x00494c80 |
| 0x00771a40 | 4 | 1 | 0x00494c80 |
| 0x00771a44 | 4 | 1 | 0x00494c80 |
| 0x00771a48 | 4 | 1 | 0x00494c80 |
| 0x00771e80 | 7 | 1 | 0x004957a0 |
| 0x00772150 | 7 | 1 | 0x004957a0 |
| 0x00772154 | 7 | 1 | 0x004957a0 |
| 0x007721d0 | 7 | 1 | 0x004957a0 |
| 0x007721d4 | 7 | 1 | 0x004957a0 |
| 0x00772290 | 7 | 1 | 0x004957a0 |
| 0x00772fa8 | 7 | 1 | 0x00495870 |
| 0x00772ffc | 5 | 1 | 0x004967e0 |
| 0x00773000 | 5 | 1 | 0x004967e0 |
| 0x00773030 | 5 | 1 | 0x004967e0 |
| 0x00773054 | 5 | 1 | 0x004967e0 |
| 0x00773058 | 5 | 1 | 0x004967e0 |
| 0x00773078 | 4 | 1 | 0x00496c10 |
| 0x00773088 | 7 | 1 | 0x00496ce0 |
| 0x00773094 | 7 | 1 | 0x00496ce0 |
| 0x00773120 | 7 | 1 | 0x00497310 |
| 0x00773928 | 4 | 1 | 0x00499ce0 |
| 0x0077392c | 4 | 1 | 0x00499ce0 |
| 0x00773930 | 4 | 1 | 0x00499ce0 |
| 0x00773934 | 4 | 1 | 0x00499ce0 |
| 0x00773938 | 4 | 1 | 0x00499ce0 |
| 0x0077393c | 4 | 1 | 0x00499ce0 |
| 0x00773940 | 4 | 1 | 0x00499ce0 |
| 0x00773944 | 4 | 1 | 0x00499ce0 |
| 0x00773948 | 4 | 1 | 0x00499ce0 |
| 0x0077394c | 4 | 1 | 0x00499ce0 |
| 0x00773950 | 4 | 1 | 0x00499ce0 |
| 0x00773954 | 4 | 1 | 0x00499ce0 |
| 0x00773958 | 4 | 1 | 0x00499ce0 |
| 0x0077395c | 4 | 1 | 0x00499ce0 |
| 0x00773960 | 4 | 1 | 0x00499ce0 |
| 0x00773964 | 4 | 1 | 0x00499ce0 |
| 0x007d3e4c | 5 | 1 | 0x004b68e0 |
| 0x007d3e54 | 7 | 1 | 0x004b6700 |
| 0x007d3e5c | 4 | 1 | 0x004b6610 |
| 0x007d3e60 | 4 | 1 | 0x004b6610 |
| 0x007d3e6c | 7 | 1 | 0x004b9730 |
| 0x007d40a8 | 3 | 1 | 0x004c7650 |
| 0x007d45c4 | 2 | 1 | 0x004cc160 |
| 0x007d4628 | 5 | 1 | 0x004ce2d0 |
| 0x007d6c5c | 3 | 1 | 0x004d8480 |
| 0x007dc768 | 7 | 1 | 0x004b5350 |
| 0x007dc76c | 7 | 1 | 0x004b5350 |
| 0x007dc7e8 | 10 | 1 | 0x00556d20 |
| 0x007dc81c | 10 | 1 | 0x005571e0 |
| 0x007dc840 | 10 | 1 | 0x00557250 |
| 0x007dc864 | 10 | 1 | 0x00557250 |
| 0x007dc950 | 7 | 1 | 0x005a6030 |
| 0x007dc954 | 6 | 1 | 0x005a5f00 |
| 0x007dc960 | 6 | 1 | 0x005a5f00 |
| 0x007dc980 | 6 | 1 | 0x005a6280 |
| 0x007dca0c | 7 | 1 | 0x005a6b70 |
| 0x007dca44 | 7 | 1 | 0x005a6f30 |
| 0x007dca54 | 7 | 1 | 0x005a6ea0 |
| 0x007dcaa8 | 5 | 1 | 0x0045d460 |
| 0x007dcb78 | 6 | 1 | 0x005a8e70 |
| 0x007dcdf8 | 8 | 1 | 0x005ab070 |
| 0x007dd6a8 | 7 | 1 | 0x005ad6a0 |
| 0x007dde70 | 6 | 1 | 0x005b8570 |
| 0x007e9584 | 5 | 1 | 0x004c1be0 |
| 0x007e96c8 | 7 | 1 | 0x00497310 |
| 0x007ec9e4 | 3 | 1 | 0x00422ba0 |
| 0x007eca14 | 3 | 1 | 0x00422ba0 |
| 0x007f0a04 | 7 | 1 | 0x0040e590 |
| 0x007f0a08 | 8 | 1 | 0x0046c750 |
| 0x007f0a5c | 1 | 1 | 0x0043dfd0 |
| 0x007f0eec | 1 | 1 | 0x0043dfd0 |
| 0x007f0ef0 | 1 | 1 | 0x0043dfd0 |
| 0x007f0f00 | 7 | 1 | 0x00431b20 |
| 0x007f0f08 | 6 | 1 | 0x00431b60 |
| 0x007f0f30 | 6 | 1 | 0x00496530 |
| 0x007f0f34 | 8 | 1 | 0x0040d470 |
| 0x007f0f54 | 1 | 1 | 0x00404e80 |
| 0x007f0fcc | 1 | 1 | 0x00410510 |
| 0x007f0fd4 | 1 | 1 | 0x0040b290 |
| 0x007f0ff0 | 4 | 1 | 0x00401340 |
| 0x007f0ff4 | 1 | 1 | 0x00416250 |
| 0x007f1014 | 1 | 1 | 0x0040b290 |
| 0x007f1040 | 6 | 1 | 0x00496530 |
| 0x007f104b | 6 | 1 | 0x00496530 |
| 0x007f104c | 6 | 1 | 0x00496530 |
| 0x007f1050 | 6 | 1 | 0x00496530 |
| 0x007f1055 | 4 | 1 | 0x0045bba0 |
| 0x007f1058 | 1 | 1 | 0x004922e0 |
| 0x007f1064 | 1 | 1 | 0x004922e0 |
| 0x007f1068 | 1 | 1 | 0x004922e0 |
| 0x007f106c | 1 | 1 | 0x004922e0 |
| 0x007f1077 | 6 | 1 | 0x00496530 |
| 0x007f1088 | 6 | 1 | 0x00496530 |
| 0x007f108c | 9 | 1 | 0x004414b0 |
| 0x007f10a1 | 8 | 1 | 0x0040d470 |
| 0x007f12a2 | 9 | 1 | 0x00445aa0 |
| 0x007f14f8 | 6 | 1 | 0x00496530 |
| 0x007f14ff | 4 | 1 | 0x0045bba0 |
| 0x007f1515 | 4 | 1 | 0x0045bba0 |
| 0x007f1561 | 8 | 1 | 0x0040d470 |
| 0x007f19b8 | 6 | 1 | 0x0046c7d0 |
| 0x007f1a24 | 2 | 1 | 0x0042b960 |
| 0x007f1a28 | 4 | 1 | 0x0042c7c0 |
| 0x007f1a2c | 4 | 1 | 0x0042c7c0 |
| 0x007f1a34 | 2 | 1 | 0x0042b960 |
| 0x007f1a38 | 4 | 1 | 0x0042c7c0 |
| 0x007f1a3c | 4 | 1 | 0x0042c7c0 |
| 0x007f1a44 | 2 | 1 | 0x0042b960 |
| 0x007f1a48 | 4 | 1 | 0x0042c7c0 |
| 0x007f1a4c | 4 | 1 | 0x0042c7c0 |
| 0x007f1a54 | 2 | 1 | 0x00423630 |
| 0x007f1a58 | 1 | 1 | 0x00443440 |
| 0x007f1a64 | 2 | 1 | 0x00423630 |
| 0x007f1a68 | 1 | 1 | 0x00418560 |
| 0x007f1a6c | 1 | 1 | 0x00418560 |
| 0x00803358 | 1 | 1 | 0x00404e80 |
| 0x0080335c | 1 | 1 | 0x00404e80 |
| 0x00881ec8 | 6 | 1 | 0x0046c570 |
| 0x00881ed0 | 6 | 1 | 0x004704c0 |
| 0x00881edc | 6 | 1 | 0x004704c0 |
| 0x00881ef0 | 6 | 1 | 0x004704c0 |
| 0x00881f1c | 6 | 1 | 0x004704c0 |
| 0x00881f30 | 6 | 1 | 0x004704c0 |
| 0x00881f44 | 6 | 1 | 0x004704c0 |
| 0x00881f48 | 6 | 1 | 0x004704c0 |
| 0x00899260 | 4 | 1 | 0x00425bc0 |
| 0x008a94a8 | 4 | 1 | 0x004113b0 |
| 0x008a94ac | 4 | 1 | 0x004113b0 |
| 0x008aa300 | 4 | 1 | 0x004669b0 |
| 0x008aa304 | 4 | 1 | 0x004669b0 |
| 0x008aa308 | 4 | 1 | 0x004669b0 |
| 0x008aa30c | 4 | 1 | 0x004669b0 |
| 0x009124e0 | 3 | 1 | 0x00550910 |
| 0x10000000 | 4 | 1 | 0x004881d0 |
| 0x10000087 | 4 | 1 | 0x004881d0 |
| 0x3c23d70a | 3 | 1 | 0x00413c70 |
| 0x3f400000 | 4 | 1 | 0x0041eaa0 |
| 0x41200000 | 4 | 1 | 0x004881d0 |
| 0x42480000 | 3 | 1 | 0x00420de0 |
| 0xdeadbeef | 1 | 1 | 0x00404e80 |
| 0xffffff00 | 7 | 1 | 0x00497310 |

## 5. Dynamic-coverage evidence inventory

**Conclusion: NO per-RVA in-race call-frequency dataset exists in the repo.** All in-race dynamic artifacts record either first-hit timestamps (6 pre-selected RVAs), quota-capped self-test branch samples (4 RVAs), or vehicle-state telemetry (no RVAs). The only per-RVA invocation-count artifact is menu-scoped (3 RVAs).

| Artifact | What it measures | RVAs |
|---|---|---|
| `re/scenarios/002-race-drive/coverage.tsv` (+ `003-race-drive`, `004-ramp-airborne`, `001-nav-demo`) | First-hit only per candidate RVA (`first_frame`, `first_t_ms`); README states "the first frame each candidate RVA fired (one-shot)" and the candidate set is "targeted per session … not the whole image". `meta.json` in all four: `coverage_candidates` = the same 6 RVAs, `coverage_hits: 6`. No counts. | 0x0046b540, 0x00470c70, 0x00470670, 0x0046ddb0, 0x00467650, 0x00468980 (identical list in all 4 scenarios) |
| `re/analysis/phys_c4_evidence/COVERAGE_SCENARIO_FINDINGS_2026-06-17.md` | Per-branch bit-confirm sample counts under per-branch sampling quotas (installed hooks, 60–75 s AI-opponent race), not free-running frequency. Documents NOT-REACHED branches: A5 airborne, A5 random-surface RNG, A6a airborne, A6b airborne aero body — cause stated: "The canonical Quick-Battle Arctic arena is geometrically flat" (0 airborne frames across all 4 car slots, 75 s). | A4 0x00470670 (brake 53 calls GREEN, in5>128 16 calls GREEN), A5 0x0046ddb0 (grounded 48/48), A6a 0x00467650 (brake 32 calls, susp-force float10 residual), A6b 0x00468980 (grounded 8/8) |
| `log/auto_count_at_menu.txt` (+ 11 sibling `auto_count_at_menu_*.txt`, all dated 2026-05-15) | Invocation counts over 25 s **at the main menu only**. No in-race variant exists in `log/`. | Vec3Magnitude 1076, FastSqrt 16140, FastInvSqrt 5364 (main file); siblings are menu/boot phases |
| `log/install_observe_r1b_20260609.txt`, `log/install_observe_wsa2_20260616.txt`, `log/install_observe_wsa2_inner_20260616.txt` | Inline-JMP install verification at menu (first byte 0xE9 + 25 s survival + manifest flag). No call counts. | 85 RVAs (r1b, 6 batches) / 3 RW-math RVAs (wsa2) |
| `re/analysis/plans/mass_observe_r4_curation.md` | Candidate-curation plan (2026-05-23) for menu/boot mass-observe rounds; records that PIZ loaders were "count=0 across R2+R3" at boot-to-menu. Menu/boot-scoped; no in-race data. | 15 boot/frontend candidate RVAs (plan list, not measurements) |
| `log/handling_telemetry.jsonl` (2.5 MB, 2026-06-10) | Per-vehicle state rows (`t, thr, rev, vx, vy, vz, ms, tf, sl, id`). Vehicle-state telemetry; contains no RVAs. | none |
| `re/parity/` | **Absent from disk and from `main` and `ws-r6-ai-control-chain`.** Exists only in commit fab63d6f (tracked files: `.gitignore`, `README.md`, `report.py`, `scenarios.json`, `sweep.py`, `target_c4.py`), contained only in branches `promote-c4` and `ws-visual-polish` (`git branch -a --contains fab63d6f`). Its `.gitignore` excludes `runs/` and `C4_TARGETS.md`, so run outputs are not in git on any branch. | n/a (no outputs in repo) |

Explicit gaps this leaves for the R6 demand map:
- No artifact records how OFTEN any RVA fires during a race (per-frame or total counts): scenario `coverage.tsv` is one-shot first-hit; phys_c4 counts are quota-capped self-test samples.
- No in-race coverage measurement exists for any RVA outside the 6 scenario candidates + 4 physics-chain hook entries above; AI-control-chain RVAs have zero recorded dynamic coverage.
- The only frequency-style counts (auto_count_at_menu) cover 3 util RVAs at the main menu, dated 2026-05-15.

What a real frequency measurement would take, mechanically: `re/frida/count_natural_invocations.py` already implements per-RVA invocation counting (it produced the menu-scoped counts in `log/auto_count_at_menu.txt`) and would need to be pointed at the closure RVAs in this file instead of the 3 util RVAs it was run on. The scenario replay layer under `re/scenarios/` (see `re/scenarios/README.md` and the four existing scenario directories) already drives deterministic in-race runs and records per-RVA first-hit rows in `coverage.tsv`; extending its candidate set from 6 RVAs to the closure set and replacing one-shot first-hit with a counter yields the missing dataset. The Frida-overhead constraint documented in CLAUDE.md (Interceptor on >1000 calls/s paths destabilizes MASHED in ~6 s) bounds any such run to sampled or batched candidate sets rather than a single all-RVA pass.

## 6. Method limits

- **Static-only closure.** Every row above is static reachability from source-cited roots. No artifact in this file demonstrates that any closure member executes during a race; section 5 documents that no per-RVA in-race frequency dataset exists.
- **Call-graph coverage.** The merged TSV call graph carries callee data for 2707 nodes after +0x400000 normalization (2707 of which have hooks.csv rows). hooks.csv has 5,864 data rows / 5,851 unique RVAs, so callee data exists for 46.3% of tracked functions; the remaining 3144 tracked functions contribute no out-edges. (The task brief cited 5,896 hooks.csv rows; the extraction pass measured 5,864 data rows — the numbers here use the measured extraction.)
- **Truncation leaves.** 2134 closure members have no callee data; the closure is truncated at each of them, so the true static closure is at least as large as reported, never smaller.
- **Under-approximation: indirect calls.** The TSV callees columns record direct static callees. Virtual/indirect calls (function-pointer tables, vtables, callbacks) are absent unless a plan file resolved them; edges through them are missing from this closure.
- **Over-approximation: dead branches.** A static callee is counted even if the calling branch never executes in the R6 slice (error paths, other game modes, debug paths). In-degree therefore over-counts demand from code the slice never runs.
- **Callee-column mixing.** 0 callee values below the image base were +0x400000-normalized, and callee lists include data addresses (e.g. members of section 4's no-hooks-row list); 10 self-loop edges produced by the offset-row merge were dropped. No semantic meaning was assigned to any unmatched address (NO-GUESSING).
- **First-caller is BFS order, not importance.** `first caller` records which node discovered the RVA first in a deterministic sorted BFS; it is not the dominant or most frequent caller.
- **Roots reflect current sources.** The root set is exactly the RVA citations present in the Race/ and Ai/ sources on branch `ws-r6-ai-control-chain` at generation time; citations in comments (TODO-RE, adapters) seed the closure identically to executed call-original forwards — the closure does not distinguish them.
