# RESULT MATCH-TOPDOWN-T13 (2026-11-01)

Verdict: **GREEN.** 4 of the 16 RVAs swapped in and called by `mashed_re.exe`: 0x00407a20, 0x00407cd0,
0x0040e350, 0x0046c5f0. 12 left, each with a one-line reason below. One branch, one build, default gates unchanged on this build (G-CTRL 8284be46 / ba407a16 / 31fe043e, B 1d5bf4f9 x3 = merge_train.json refs).

L-72 (Mariano 2026-11-01): swap matched callers in, top-down. T13 is the ready-now batch from
`verify/match_topdown_20261101/RESULT_MATCH-TOPDOWN-MAP.md` (T13 row). The stated blocker for every row: the
matched source reads the original globals, so a swap is sound only where the exe keeps those addresses live
(an exe-target TU writes them, or the caller's state lives there), not port host objects. In the exe the
original data range is plain zero-filled exe memory, so an address no exe TU writes reads 0.

## Per-RVA table

`match.py` = `re/tools/match/match.py <src> --toolchain ddk --bytes` (flags `/O2 /GS /Ob1`, cl 13.00.9176 /
c2 13.00.9178), re-run on this branch. `exe callers` = E8 `call` sites to the symbol in this branch's
`mashed_re.exe`, counted by scanning `.text` against the `/MAP` address.

| rva | outcome | matched source (sha256/16) | match.py | relocheck | exe body now | exe callers | targets | reason / note |
|---|---|---|---|---|---|---|---|---|
| 0x00407a20 | **swapped** | `re/match/s01/00407a20.c` (6a52a3fa02d7e5ff, new) | 100.0% 4/4, 17 B identical | OK checked=1 unchecked=0 | `GetAiLapCounter`, `Match/MatchS01_00407a20.cpp` | 1 (`AiPreTickRubberBand`, 0x004177b0 port, `Ai/AiStandalone.cpp`) | both | caller read `0x008a9648 + v*0x30c` inline; now calls the matched body. 0x008a9648 is live in the exe (PathProg `PP_408610` writes it, `PathProg_RaceInit` seeds -1). Old asi body `Gameplay/RangeTable_ah1.cpp` removed. The new source is the raw `re/match/s01/raw/00407a20.c` body with an `MGLOBAL` line instead of the Ghidra preamble (not C++-safe) |
| 0x00407cd0 | **swapped** | `re/match/s01/00407cd0.c` (7f0a987da6d57d71) | 100.0% 73/73, 157 B identical | OK checked=1 unchecked=0 | `PP_407cd0`, `Match/MatchS01_00407cd0.cpp` (exe only) | 4 (`PP_407e20` at 0x00407f9f / 0x00407faf / 0x004080e6 / 0x004080f3) | exe | replaces the naked verbatim `PP_407cd0` (`Race/PathProgStep_naked.inc`; spec entry dropped from `re/tools/naked_specs/pathprog_p2.json` so a regenerate does not re-emit it). Callee 0x00426090 binds to `GlobalDat0066ce58Get`; the table at 0x0066ce58 is written in the exe by `Race/PathNodes.cpp`. Integer only |
| 0x0040e350 | **swapped** | `re/match/s02/0040e350.c` (854327d98c485250) | 100.0% 2/2, 6 B identical | OK checked=1 unchecked=0 | `GetRenderSubMode`, `Match/MatchS02_0040e350.cpp` | 2 (`TG_SubMode` tail-jmp + one inlined copy; `TG_SubMode` is `TG_414490`'s callee at 0x00414493 / 0x0041449d) | both | `TG_SubMode` returned the host `aib_game_sub_mode` (0 -> 6, 7 on round end); now the raw `DAT_0063ba8c`. `TG_414490` only compares it with 0xb and 0xa, and no exe TU stores either (exe writers: `TrackRenderer.cpp` 3/4/0, `SubStateMachine.cpp` 5/6), so the branch outcome is the same. Replay branch kept. Old asi body `Ai/PromoLoop_round1.cpp` removed |
| 0x0046c5f0 | **swapped** | `re/match/pilot/a5_0046c5f0.c` (3bfb7cc877c1e909, was 14236286df8ae68e) | 100.0% 71/71, 210 B identical | OK checked=4 unchecked=0 | `Collision::TriangleFaceNormal`, `Match/MatchPilot_0046c5f0.cpp` (exe only, x87) | 1 (`VehicleWheelForceIntegrator` 0x0046ddb0 port, `A5FaceNormal`, original site 0x0046e6f3) | exe | no globals (pointer args from the caller). Source edit: the unmatched caller 0x0046da80 (50.6%) in the same file is now under `#ifndef MATCH_PORT` (S04 precedent: no unmatched function as a port body); match.py re-run after the edit. `TriangleFaceNormal(apex, p1, p2)` = `FUN_0046c5f0(apex, p2, p1)`: same products, same operand order (derivation in the wrapper header; the original site loads ECX = scratch[6] = 3rd param, EDX = scratch[3] = 2nd). `FUN_004c3b30` binds to `Collision::FastSqrt` (LUT, the callee the old body used). TU added to `x87_tus.txt` |
| 0x0040cda0 | left | `re/match/s02/0040cda0.c` | 100.0% | OK | none | 0 | - | no exe caller: `PK_4576b0` (0x004576b0..0x004576e7) has no call; the original call 0x00457772 is in an unlisted function at 0x004576f0 with no port body (the map folded it into 0x004576b0). `DAT_00636ad0` is a port constant (`g_dat_00636ad0`, U-9697); `FUN_0040bb10` / `FUN_004672d0` have no exe body |
| 0x0040ce80 | left | `re/match/s02/0040ce80.c` | 100.0% | OK | none (asi `Util/PromoLoop_round49.cpp`) | 0 | - | the exe arm of `VehicleInit.cpp` never calls it (`A3HandlingType()` = `HandlingTypeIndex()`); the table behind `0x005f2770` -> 0x005f2728+v*4 has no exe writer, so the read would dereference 0+4 (U-9783) |
| 0x0040e180 | left | `re/match/s02/0040e180.c` | 95.1% insn / bytes identical (433 B obj vs 423 B orig; insn count 143/136) | OK | `RaceCamera::MostSeparatedPair`, `Race/MostSepPair.cpp` | (3 exe callers of the port body) | - | caller state is port-only (`RaceCamCar[4]` filled from `car_pos_`, `ai_cars_[]`), and the matched callees read `g_records`, not the original addresses (S02: "port-owned RaceCamCar[4], exact sqrt instead of 0x004c3ac0"). Also match.py reports 95.1% insn on this branch, which needs a look before any C4 |
| 0x00414490 | left | `re/match/s03/h2_00414490.c` | 100.0% | OK | naked `TG_414490` (`Ai/AiModeSeven_naked.inc`) | (1, `TG_414490_w`) | - | reads `DAT_0088fc88[car*0x2d]` (held powerup word), which no exe TU writes (the exe reads `PowerupSystem pw_` through `TG_HeldWord`), and calls 0x0046d510 (dead, below) |
| 0x004173a0 | left | `re/match/s03/h1_004173a0.c` | 100.0% | OK | none (asi `Util/PromoLoop_round35.cpp`) | 0 | - | live global (`DAT_0089a360` = 2.5f from `Ai_BridgeLoad`), but the exe caller `Rule10Seed` gets a literal `0.f` from `TrackRenderer.cpp:7450/7745`; wiring it changes course 0xb / 0x1a / 0x27 seeds (factor 0.75 for 1.0). A behaviour change: default-OFF knob + PREREG + decision, not a swap |
| 0x0041ae20 | left (already swapped) | `re/match/s03/h1_0041ae20.c` | 100.0% | OK | `Copy41ae20`, `Race/SubStateCallees.cpp` (S03) | via 0x0041b520 | both | see 0x0041b520 |
| 0x0041b520 | left (already swapped) | `re/match/s03/h1_0041ae20.c` | 100.0% | OK | `Update41b520`, `Race/SubStateCallees.cpp` (S03) | 1 (`SubStateEnter4`, `Race/SubStateMachine.cpp:157`) | both | the exe already runs the matched pair, but on dead data: `DAT_005f3304` (Vec3 table) reads 0 and nothing in the exe reads what it writes to `0x0063c8d0` (the exe's panel state is `g_nt[4]` in `LibRw/RwRaceFx.cpp`). Not proposed for C4 here: the body runs, the inputs are not the original's. Lead to decide whether the L-68 letter (identical + called + gates unchanged) is enough |
| 0x0041f030 | left | `re/match/s04/0041f030.c` | 100.0% | OK | none (asi `Match/MatchS04_0041f030.cpp`) | 0 (exe calls the stub `TG_Stub41f030`) | - | reads the 0x2ac-byte car record at 0x0063d9e0, which the exe keeps in port-only `s_rec` (`Race/CarRegistrant.cpp`); its only consumer 0x0048a630 is also a stub |
| 0x0041f1e0 | left | `re/match/s04/0041f1e0.c` | 100.0% | OK | none (asi `Lane2/L2_0041f1e0.cpp`) | 0 | - | no exe call site: the original call is in the RWP-body branch the exe leaves out (U-9738, `Track/WarzonePontoon.cpp`); table 0x0063d9e0 dead in the exe, callee 0x004c0ed0 has no exe body |
| 0x00422b50 | left | `re/match/pilot/a5_00422b50.c` | 100.0% | OK (unchecked `_DAT_008995bc`, `__ftol`) | `VehicleDamageAccum`, `HUD/ScenarioWriters_sa2s2.cpp` | 0 (both `LaunchRevCharge.cpp` calls are `#ifndef MASHED_STANDALONE`) | - | the 0x008995bc per-car block does not exist in the exe (header of `Vehicle/LaunchRevCharge.cpp`); scores live in host `score[]` |
| 0x0046d510 | left | `re/match/pilot/a2_0046d510.c` | 100.0% | OK | `VehicleVelocityWorldGet`, `Ai/VehicleVelocityWorldGet.cpp` | 0 by symbol (the AI calls `TG_Fwd` / `AL_Fwd`) | - | reads/writes `DAT_00881e80[]` records; exe vehicle records live in port `g_records` (`VehRecord` = `g_vehicleArrayBase`), and through 0x004c3df0 it would write 0x00881f74+i*0xd04 |
| 0x0046dbe0 | left | `re/match/pilot/f_0046dbe0.c` | 100.0% | OK | `VehicleRacePositionGet`, `Vehicle/VehicleState.cpp` | (1, `RubberBandGrip`) | - | reads `DAT_008815a8[i*0x341]`; the exe reads `VehRecord(i)+8` in `g_records`, so the matched body would return 0 for the race position |

Map correction for the lead: the T13 edge 0x004576b0 -> 0x0040cda0 (`topdown_ranked.tsv` row 97) is not real; the
original call is inside 0x004576f0. The other given callers' exe bodies: 0x004177b0 is `AiPreTickRubberBand` in
`Ai/AiStandalone.cpp` (not `Race/RuleEngine.cpp`); 0x00407cd0's existing exe body was the naked `PP_407cd0`
(S01 listed it as "no exe body").

## Build

Worktree `.worktrees/MATCH-TOPDOWN-T13`, `MASHED_NO_DEPLOY=1`, test-slot wrapped. `=== Build OK ===` both targets.
`[rva-lint] 47 finding(s) ... allowlisted=47 NEW=0`, `[asm-lint] OK`. `rva_body_scan`: 0x00407a20 and 0x0040e350
one body each, `Match/` wrapper, both targets; 0x00407cd0 and 0x0046c5f0 one body each, `Match/` wrapper, exe only.
exe `/MAP`: `_GetAiLapCounter` (MatchS01_00407a20.obj), `_PP_407cd0` (MatchS01_00407cd0.obj), `_GetRenderSubMode`
(MatchS02_0040e350.obj), `?TriangleFaceNormal@Collision@mashed_re@@...` (MatchPilot_0046c5f0.obj).

## Gates

Runner `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, A4_ROOT
`D:/MashedWT/MATCH-TOPDOWN-T13_root` (robocopy of `original/`), A4_EXE = the build copied out first
(`D:/MashedWT/MATCH-TOPDOWN-T13_data/gates.sh`), wrapped in `test-slot` + `mashed_lock.py run --label MATCH-TOPDOWN-T13`.

| run | build | exe sha256/16 | G-CTRL step / gates / ptrace | B K0 / K1 / K26 | vs merge_train.json ref |
|---|---|---|---|---|---|
| 1 | this branch's source (commit below) | `d36c7193935a33a9` | 8284be46 / ba407a16 / 31fe043e | 1d5bf4f9 x3 | equal |

H_DEF self-exited in 125 s (13962 step lines), each B arm in 12 s (762 lines), exit 0 for all four. No gate hash
moved, so no DECISION. Runtime reach on the gate scenario was not measured per swap: `PP_407cd0` (PathProg),
`AiPreTickRubberBand` and `A5FaceNormal` (ForceIntegrator) are per-frame race code; `TG_414490` (0x0040e350's
caller) is the mode-7 target leaf. The C4 rule counts static exe callers (above), not runtime hits.

## -Full

Not run here. The branch touches `mashedmod/src/mashed_re/Ai/`, `Collision/` (in `full_set_paths`) and `Match/`,
`Race/`, `Gameplay/`: the train's `-Full` pass (vsum / e_launch, D2 x3, 13 tracks) is the train's leg for this row.

## Prepared re-classify (C4 by match)

For the `re-classify` leg after merge. All four: route `match`, toolchain ddk (cl 13.00.9176 / c2 13.00.9178),
flags `/O2 /GS /Ob1`, gates unchanged on this branch's build (G-CTRL 8284be46/ba407a16/31fe043e, B 1d5bf4f9 x3),
evidence `verify/match_topdown_20261101/RESULT_MATCH-TOPDOWN-T13.md`.

| rva | name | level | file (new) | notes text |
|---|---|---|---|---|
| 0x00407a20 | GetAiLapCounter | C4 -> C4 (route match added to the existing frida C4) | `mashedmod/src/mashed_re/Match/MatchS01_00407a20.cpp` | `match evidence: identical (re/match/s01/00407a20.c 6a52a3fa02d7e5ff, match.py 100% 4/4, 17 B), relocheck OK checked=1 unchecked=0, swapped, exe-callers=1 (AiPreTickRubberBand 0x004177b0), gates unchanged (MATCH-TOPDOWN-T13)` |
| 0x00407cd0 | FUN_00407cd0 (PP_407cd0) | C2 -> C4 | `mashedmod/src/mashed_re/Match/MatchS01_00407cd0.cpp` | `match evidence: identical (re/match/s01/00407cd0.c 7f0a987da6d57d71, match.py 100% 73/73, 157 B), relocheck OK checked=1 unchecked=0, swapped (replaces naked PP_407cd0), exe-callers=4 (PP_407e20), gates unchanged (MATCH-TOPDOWN-T13)` |
| 0x0040e350 | GetRenderSubMode | C3 -> C4 | `mashedmod/src/mashed_re/Match/MatchS02_0040e350.cpp` | `match evidence: identical (re/match/s02/0040e350.c 854327d98c485250, match.py 100% 2/2, 6 B), relocheck OK checked=1 unchecked=0, swapped, exe-callers=2 (TG_SubMode -> TG_414490), gates unchanged (MATCH-TOPDOWN-T13)` |
| 0x0046c5f0 | FUN_0046c5f0 (Collision::TriangleFaceNormal) | C2 -> C4 | `mashedmod/src/mashed_re/Match/MatchPilot_0046c5f0.cpp` | `match evidence: identical (re/match/pilot/a5_0046c5f0.c 3bfb7cc877c1e909, match.py 100% 71/71, 210 B), relocheck OK checked=4 unchecked=0, swapped (x87 TU), exe-callers=1 (VehicleWheelForceIntegrator A5FaceNormal), gates unchanged (MATCH-TOPDOWN-T13)` |

`exe_file` for each = the same `Match/` path. The 12 left rows: no level change.

## Raw data (D:/MashedWT/MATCH-TOPDOWN-T13_data, not committed)

```
d36c7193935a33a97c4ffb6236fda60cfb62cad8b31e13471635b8f451d66c9f  exe_g1/mashed_re.exe
7f377002d81f61c0811ddce46c955a4a2e04a8448d9d733b64ffc344f499a173  build1.log
30d4300e2885e9f56760e2651f0bd4a70d9280e95467ce822815d0f09eb49447  gates_g1.log
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198  arms_g1/H_DEF.step.csv
ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb  arms_g1/H_DEF.gates.csv
31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3  arms_g1/H_DEF.ptrace.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K0.step.csv
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K1.step.csv
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K26.step.csv
c2e861c4afb7179be6d767635bb074ae970058e8a044cbd715e5d450d4f3a5ce  arms_g1/H_DEF.log
```
