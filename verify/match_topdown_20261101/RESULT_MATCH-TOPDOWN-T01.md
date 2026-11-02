# RESULT MATCH-TOPDOWN-T01 (2026-11-01)

Verdict: **GREEN.** 1 of 50 swapped: the exe now calls the matched 0x00407a60 body. 49 left, each with a reason. Of those, 0x0041d820 is already C4 and 0x00414180 is already swapped and called (held at C3 by stub S-3669).

T01, T02, T09, T12 and T08 share the branch `orch/MATCH-TOPDOWN-T01` (one build, one gate run). T01 / T02 / T09 / T12 hang off Race::Tick 0x0040fc00 / race frame 0x004111c0; T08's one swap edits `AiPreTickRubberBand`, a race-frame function, so it rides the same build.

L-72 (Mariano 2026-11-01). T01 = map unit 1: Race::Tick 0x0040fc00 under the race frame 0x004111c0, 50 matched functions in 24 roots, 7 x87. Blocking gaps 0x0040fc00 (621 B) and 0x004111c0 (400 B), both S02 "not attempted", no source. Not attempted here: neither is a one-run match (no g2s source exists for either).

## Method

`re/tools/match/match.py <src> --toolchain ddk --bytes` (cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`) and
`re/tools/match/relocheck.py`, re-run 2026-11-01 on every source (`D:/MashedWT/MATCH-TOPDOWN-T01_data/t01.tsv`). `exe body`
= `scripts/rva_body_scan.py`. `exe callers` = call edges to that symbol in the baseline `mashed_re.exe` of
orch/integration `3a3186c4a` (/MAP + `verify/match_topdown_20261101/cg_build.py`), the `s_AutoReg_*` entry not
counted; the two swapped rows give this branch's count. `orig callers` = the map's original call graph; the bracket
says whether that caller has an exe body (`none`, `unreached`, `reached`, `port body` = a body whose symbol the /MAP
lookup did not bind). A default reason "no exe call site" means no original caller has an exe body that could make
the call; "state ... has no exe TU reference" lists original addresses no exe TU names (zero in the exe, T13 RESULT).

The blocker for the whole race group is the same as T05 / T10: the exe runs the race frame and Race::Tick as
RVA-less port code (`D3d9Render/TrackRenderer.cpp`, `Race/SubStateMachine.cpp`, `Ai/AiStandalone.cpp`), whose state
lives in port objects. A matched body is a sound swap only where an exe port function already does the same work on
the same live original addresses. Two such sites exist in this group: `PathProg_RaceInit` (0x00407a60) and
`AiPreTickRubberBand`'s reads of `0x0089a880` (0x00417730, T08).

## Per-RVA table

| rva | outcome | matched source | match.py | relocheck | exe body | exe callers | orig callers [exe body of caller] | reason |
|---|---|---|---|---|---|---|---|---|
| 0x00403250 | left | `re/match/s01/00403250.c` | 100.0% 176/176, 583 B identical | OK 16/44 | none | 0 | 0x0040fc00[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x00406370 | left | `re/match/s01/raw/00406370.c` | 100.0% 32/32, 153 B identical | OK 0/30 | none (asi `Clear10x3_63a494`) | 0 | 0x004073b0[none] 0x00407800[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x0063a494`, `0x0063a498`, `0x0063a49c`, `0x0063a4b4` has no exe TU reference |
| 0x004073b0 | left | `re/match/s01/004073b0.c` | 100.0% 63/63, 237 B identical | OK 4/5 | none | 0 | 0x00407800[none] 0x0040fc00[none] 0x00426340[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x0063a5d8` has no exe TU reference |
| 0x00407a60 | **swapped (caller)** | `re/match/s01/00407a60.c` | 100.0% 12/12, 40 B identical | OK 2/0 | `UtilTableInit8a9620`, `Match/MatchS01.cpp` | 2 (`PathProg_RaceInit` tail-jmp, its inlined copy in `PathProg_MaybeReplay`; this branch's build) | 0x0040fc00[none] | `PathProg_RaceInit` (`Race/PathProgStep.cpp:221`, RVA-less copy, called from `TrackRenderer.cpp:658`) did the same writes (4 x 0x30c bytes at the live `0x008a9620` zeroed, +0x28 = -1); it now calls the matched body. The baseline linker had already folded the two (identical code) |
| 0x0040b180 | left | `re/match/s02/0040b180.c` | 100.0% 46/46, 195 B identical | OK 24/0 | `TrackRenderer::SeedMatchScores`, `D3d9Render/TrackRenderer.cpp` | 1 | 0x0040fc00[none] | port body `TrackRenderer::SeedMatchScores` computes the seed from port state (`ParticipantCount()`, `team_play_`, `rule_`) into `scores_[]`; the matched body reads `0x008a94d0` (no exe writer) and `0x0067ea64` (0 in the exe) and writes `0x008a94e0..` instead |
| 0x0040b540 | left | `re/match/s02/0040b540.c` | 100.0% 78/78, 221 B identical | OK 7/0 | none (asi `PlayerScoreRankOrder`) | 0 | 0x0041cc50[unreached] 0x0041d830[none] 0x0042c960[none] 0x00434720[none] 0x004929d0[none] | callers unmatched or unreached; reads `0x008a94e0` scores (no exe writer outside unreached bodies); the exe standings sort is port code (`exe_main.cpp:4238`) |
| 0x0040b6d0 | left | `re/match/s02/0040b6d0.c` | 100.0% 3/3, 12 B identical | OK 1/0 | `UtilArrayGet8a94e0`, `Match/MatchS02_0040b6d0.cpp` | 0 | 0x0040e500[none] 0x0040eee0[reached] 0x00410510[reached] 0x00410860[none] 0x0041af50[none] 0x0041b7a0[none] 0x0041c410[none] 0x0041ce00[none] 0x00424b80[none] 0x00434720[none] 0x004929d0[none] | the reached caller port does not make this call (port state) |
| 0x0040b930 | left | `re/match/s02/0040b930.c` | 100.0% 19/19, 51 B identical | OK 2/0 | none (asi `PlayerScoreThresholdMask`) | 0 | 0x0041b540[none] 0x0041c090[none] 0x0041cc50[unreached] 0x0041d830[none] | the reached caller port does not make this call (port state) |
| 0x0040b970 | left | `re/match/s02/0040b970.c` | 100.0% 12/12, 41 B identical | OK 1/0 | none (asi `Bool0Out8a94e0`) | 0 | 0x0041b540[none] 0x0041cc50[unreached] | the reached caller port does not make this call (port state) |
| 0x0040b9a0 | left | `re/match/s02/0040b9a0.c` | 95 B whole symbol identical | OK 3/0 | none (asi `MaxScoreFlags40b9a0`) | 0 | 0x0041b540[none] 0x0041cc50[unreached] | the reached caller port does not make this call (port state) |
| 0x0040ba00 | left | `re/match/s02/0040ba00.c` | 100.0% 31/31, 91 B identical | OK 5/0 | none (asi `BoolMinOut8a94e0`) | 0 | 0x0041b540[none] 0x0041cc50[unreached] | the reached caller port does not make this call (port state) |
| 0x0040ba60 | left | `re/match/s02/0040ba60.c` | 52 B whole symbol identical | OK 1/0 | none (asi `Active4Slots40ba60`) | 0 | 0x0041b540[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x0040bd80 | left | `re/match/s02/0040bd80.c` | 100.0% 17/17, 85 B identical | OK 17/0 | none | 0 | 0x0040fc00[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x0040c5b0 | left | `re/match/s02/0040c5b0.c` | 100.0% 64/64, 209 B identical | OK 8/0 | none | 0 | 0x0040ce00[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x0040cd90 | left | `re/match/s02/0040cd90.c` | 100.0% 1/1, 5 B identical | OK 1/0 | none (asi `CountNonZeroPairs`) | 0 | 0x0040ce00[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x0040ce00 | left | `re/match/s02/0040ce00.c` | 100.0% 27/27, 126 B identical | OK 24/0 | none | 0 | 0x0040fc00[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x0063b8f0`, `0x0063b8f4` has no exe TU reference |
| 0x0040da50 | left | `re/match/s02/0040da50.c` | 321 B whole symbol identical | OK 18/0 | none | 0 | 0x0040fc00[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x0040dba0 | left | `re/match/s02/0040dba0.c` | 100.0% 14/14, 41 B identical | OK 3/0 | none | 0 | 0x0040fc00[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x0040e340 | left (already swapped, S02) | `re/match/s02/0040e340.c` | 100.0% 2/2, 6 B identical | OK 1/0 | `GetLiveCarCount`, `Match/MatchS02_0040e340.cpp` | 0 | 0x004019d0[none] 0x00401f10[none] 0x0040b180[reached] 0x0040b290[reached] 0x0040b700[none] 0x0040b890[reached] 0x0040b8e0[reached] 0x0040fc00[none] 0x00410510[reached] 0x00410d10[reached] 0x00412f30[none] 0x0041b4d0[none] 0x0041cb80[none] 0x00424270[none] 0x00424b80[none] 0x00424eb0[none] 0x0042c960[none] 0x00434720[none] 0x00442ce0[reached] 0x0044cb00[none] 0x00451820[port body] 0x00451ad0[reached] 0x00466b50[none] 0x00467650[reached] 0x004709a0[none] 0x00470c70[none] 0x0047eb30[unreached] 0x004929d0[none] | `0x008a94d0` has no exe writer; the exe uses `g_participantCount = 4` (T10 RESULT). Keeps its C4 |
| 0x0040e350 | left (swapped on orch/MATCH-TOPDOWN-T13) | `re/match/s02/0040e350.c` | 100.0% 2/2, 6 B identical | OK 1/0 | none (asi `GetRenderSubMode`) | 0 | 0x00404320[none] 0x00406ae0[none] 0x0040b290[reached] 0x0040fc00[none] 0x00410b30[none] 0x00414490[reached] 0x00416250[none] 0x00416a30[none] 0x004177b0[reached] 0x00417da0[none] 0x00418560[none] 0x004189a0[none] 0x004189c0[none] 0x0041e1e0[none] 0x0041e6c0[none] 0x00424eb0[none] 0x004252c0[none] 0x00429310[none] 0x00429e10[none] 0x00442c80[reached] 0x00446520[none] 0x00448220[none] 0x0044b000[unreached] 0x0044c370[none] 0x0044ecc0[none] 0x00450300[none] 0x00455b50[none] 0x00458920[reached] 0x0045bba0[none] 0x0045c7f0[none] 0x0045cc50[none] 0x0045cc80[none] 0x0045d250[none] 0x0045d330[none] 0x0045d3a0[none] 0x00462f50[none] 0x00463640[none] 0x00463c80[none] 0x00463f40[none] 0x004642f0[none] 0x00467350[none] 0x00467650[reached] 0x00468d80[reached] 0x00469aa0[reached] 0x00469df0[reached] 0x0046c7d0[none] 0x0046cc40[reached] 0x0046dc20[none] 0x0046e9e0[none] 0x00470670[reached] 0x004709a0[none] 0x00470c70[none] 0x0047d640[port body] 0x0047def0[none] 0x0047eb30[unreached] 0x004922e0[unreached] 0x004929d0[none] | T13 swaps it with 2 exe callers (`TG_SubMode`); not on this base |
| 0x00412880 | left | `re/match/s03/h1_00412880.c` | 100.0% 1/1, 5 B identical | OK 1/0 | none | 0 | 0x0040bd80[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x00413f20 | left | `re/match/s03/h1_00413f20.c` | 43 B whole symbol identical | OK 0/2 | none (asi `ZeroFill60_63bc60`) | 0 | 0x0040bd80[none] 0x0040fc00[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x0063bc60` has no exe TU reference |
| 0x00413fa0 | left | `re/match/s03/h2_00413fa0.c` | 100.0% 20/20, 49 B identical | OK 2/0 | none (asi `Calc413fa0`) | 0 | 0x00414060[port body] 0x00417750[none] | the reached caller port does not make this call (port state) |
| 0x00414060 | left | `re/match/s03/h3_00414060.c` | 100.0% 46/46, 186 B identical | OK 5/15 | `AiDifficultyResetTail`, `Race/AiDifficultyReset.cpp` | 0 | 0x0040fc00[none] | exe port `AiDifficultyResetTail` (`Race/AiDifficultyReset.cpp`, exe only) is step 6 only; steps 1-5 left out on purpose (inputs 0 in the exe, a faithful port writes `0x0089a360 = 0.0`, header of that file). A body swap is a behaviour change |
| 0x00414180 | left (already swapped, S03) | `re/match/s03/h2_00414180.c` | 100.0% 41/41, 155 B identical | OK 1/12 | `FUN_00414180`, `Race/SubStateMachine.cpp` | 1 (`TrackRenderer.cpp:5260`, hooks.csv) | 0x0040fc00[none] | already the matched body with an exe caller; held at C3 by open stub S-3669 (the .asi arm binds 0x00472650 raw), not by T01 |
| 0x00417740 | left | `re/match/s03/h1_00417740.c` | 100.0% 3/3, 16 B identical | OK 0/2 | none | 0 | 0x0040b460[none] 0x0040b540[none] 0x00410510[reached] 0x00429e10[none] 0x004427c0[none] | reads the live `0x0089a870`, but the reached caller port (0x00410510, `EvalResultCore`) reads the port snapshot, and the modes 4/7 override is unmodelled (U-9080). See T11 RESULT |
| 0x004189a0 | left | `re/match/s03/raw_004189a0.c` | 100.0% 8/8, 25 B identical | OK 3/0 | none | 0 | 0x0040fc00[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x00418e70 | left | `re/match/s03/h1_00418e70.c` | 100.0% 50/50, 183 B identical | OK 9/9 | none | 0 | 0x0041a180[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x005f3300`, `0x0063c1c8`, `0x0063c208`, `0x0063c508` has no exe TU reference |
| 0x00419fd0 | left | `re/match/s03/raw_00419fd0.c` | 100.0% 8/8, 24 B identical | OK 1/1 | none | 0 | 0x0041a180[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x0063bda0` has no exe TU reference |
| 0x0041a180 | left | `re/match/s03/h1_00418a30.c` | 100.0% 26/26, 87 B identical | OK 6/5 | none | 0 | 0x004189a0:tail[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x005f3298`, `0x005f32b0`, `0x0063bd98`, `0x0063bda0` has no exe TU reference |
| 0x0041adb0 | left | `re/match/s03/h1_grp_0041ad60.c` | 100.0% 33/33, 102 B identical | OK 2/2 | none | 0 | 0x0041b4d0[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x0063c888`, `0x0063c89c`, `0x0063c8c4`, `0x007f1a10` has no exe TU reference |
| 0x0041b4d0 | left | `re/match/s03/h1_grp_0041ad60.c` | 100.0% 13/13, 50 B identical | OK 3/4 | none | 0 | 0x0040fc00[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x0063c888`, `0x0063c89c`, `0x0063c8c4`, `0x007f1a10` has no exe TU reference |
| 0x0041b510 | left | `re/match/s03/raw_0041b510.c` | 100.0% 2/2, 11 B identical | OK 0/1 | none (asi `HudCounterReset`) | 0 | 0x0040fc00[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x0063cab0` has no exe TU reference |
| 0x0041b540 | left | `re/match/s04/0041b540.c` | 100.0% 57/57, 230 B identical | OK 33/0 | none | 0 | 0x0040fc00[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x0063c878`, `0x0063c88c`, `0x0063c8a0`, `0x0063c8a4` has no exe TU reference |
| 0x0041b720 | left | `re/match/s04/0041b720.c` | 100.0% 30/30, 79 B identical | OK 1/0 | none (asi `FUN_0041b720`) | 0 | 0x0041bf20[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x007f1a10` has no exe TU reference |
| 0x0041b770 | left | `re/match/s04/0041b770.c` | 100.0% 11/11, 39 B identical | OK 1/0 | `FUN_0041b770`, `Match/MatchS04_0041b770.cpp` | 0 | 0x0041c010[unreached] | the reached caller port does not make this call (port state) |
| 0x0041b7a0 | left | `re/match/s04/0041b7a0.c` | 1188 B whole symbol identical | OK 7/0 | none | 0 | 0x0041c090[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x0041bf20 | left | `re/match/s04/0041b720.c` | 100.0% 59/59, 226 B identical | OK 18/0 | none (asi `FUN_0041bf20`) | 0 | 0x0040fc00[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x007f1a10` has no exe TU reference |
| 0x0041c010 | left | `re/match/s04/0041b770.c` | 100.0% 25/25, 117 B identical | OK 5/0 | `HudConstTableInitAndSweep`, `Match/MatchS04_0041b770.cpp` | 0 | 0x0040fc00[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x0041c090 | left | `re/match/s04/0041c090.c` | 100.0% 12/12, 46 B identical | OK 6/0 | none | 0 | 0x0040fc00[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x0041c380 | left | `re/match/pilot/a7_0041c380.c` | 100.0% 30/30, 130 B identical | OK 3/2 | none | 0 | 0x0041cb80[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x0063cde4`, `0x0063ce18` has no exe TU reference |
| 0x0041cb80 | left | `re/match/s04/0041c380.c` | 100.0% 13/13, 53 B identical | OK 7/0 | none | 0 | 0x0040fc00[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x0063cde4`, `0x0063ce18`, `0x007f1a10` has no exe TU reference |
| 0x0041cbc0 | left | `re/match/s04/0041cbc0.c` | 100.0% 24/24, 133 B identical | OK 2/0 | `FloatTableInit`, `Match/MatchS04_0041cbc0.cpp` | 0 | 0x0040fc00[none] | no exe call site: every original caller has no exe body (gap or unswapped) |
| 0x0041cc50 | left | `re/match/s04/0041c410.c` | 100.0% 25/25, 101 B identical | OK 14/0 | `HudSlotUpdateCc50`, `HUD/HudDispatch.cpp` | 0 | 0x0040fc00[none] | port body `HudSlotUpdateCc50` (`HUD/HudDispatch.cpp`) has no exe caller; its only original caller is Race::Tick 0x0040fc00 (gap) |
| 0x0041cdb0 | left | `re/match/s04/0041cdb0.c` | 100.0% 30/30, 79 B identical | OK 1/0 | none (asi `FUN_0041cdb0`) | 0 | 0x0041d730[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x0063d298`, `0x007f1a10` has no exe TU reference |
| 0x0041d730 | left | `re/match/s04/0041cdb0.c` | 100.0% 59/59, 226 B identical | OK 18/0 | none (asi `FUN_0041d730`) | 0 | 0x0040fc00[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x0063d298`, `0x007f1a10` has no exe TU reference |
| 0x0041d820 | left (already C4) | `re/match/s04/0041d820.c` | 100.0% 2/2, 11 B identical | OK 1/0 | `TimerSlotClear`, `Match/MatchS04_0041d820.cpp` | 1 (`ExecuteFrontendBootChain`) | 0x0040fc00[none] | already the matched body, called, C4 |
| 0x0041d830 | left | `re/match/s04/0041d830.c` | 100.0% 15/15, 59 B identical | OK 8/0 | none | 0 | 0x0040fc00[none] | no exe call site: every original caller has no exe body (gap or unswapped); state `0x005f33c4`, `0x0063d278`, `0x0063d288`, `0x0063d298` has no exe TU reference |
| 0x0041eda0 | left | `re/match/s04/0041eda0.c` | 100.0% 13/13, 49 B identical | OK 1/0 | `TimerBitFieldSet`, `Match/MatchS04_0041eda0.cpp` | 0 | 0x0040be50[none] 0x0040fc00[none] 0x00410860[none] 0x004704c0[reached] | writes the car record table `0x0063d9e0`, which the exe keeps in port `s_rec` (`Race/CarRegistrant.cpp`, T13 row 0x0041f030); the exe body of 0x004704c0 is a mis-bound `RwMatrixRotate` (`Race/RoundGrid.cpp`), not the original caller |
| 0x0041eeb0 | left | `re/match/s04/0041eeb0.c` | 100.0% 42/42, 172 B identical | OK 1/0 | none (asi `FUN_0041eeb0`) | 0 | 0x0040da50[none] | same table `0x0063d9e0` (port `s_rec` in the exe); caller 0x0040da50 has no exe body |

## What T01 needs

1. 0x0040fc00 and 0x004111c0 matched (an S02 residue pass). 2. Even then, an exe Race::Tick that runs the original chain on original state: the port keeps scores (`scores_[]`), HUD slots, timers and car records in host objects. Deep gaps (map): 0x0040be50 (hides 9), 0x00420870 (5), 0x0040ca00 / 0x00419a00 / 0x0041af50 / 0x00481a30 (4 each).

## Build and gates

Worktree `.worktrees/MATCH-TOPDOWN-T01`, `MASHED_NO_DEPLOY=1`, test-slot wrapped: `=== Build OK ===` both targets,
`[rva-lint] 25 finding(s) ... NEW=0`, `[asm-lint] OK`. Source commit `dd844a168`.

Runner `verify/savelink_20261023/run_arms.py`, A4_ROOT `D:/MashedWT/MATCH-TOPDOWN-T01_root` (robocopy of `original/`), A4_EXE =
the build copied out first (exe sha256/16 `fca9b9fcce892772`), wrapped in `test-slot` + `mashed_lock.py run`.

| run | arm | result | vs merge_train.json ref (G-CTRL 8284be46 / ba407a16 / 31fe043e, B 1d5bf4f9 x3) |
|---|---|---|---|
| g1 | B_DEF_K0 / K1 / K26 | self-exited, 762 lines each, `1d5bf4f9` x3 | **equal** |
| g1 | H_DEF | **killed at the 900 s cap**, not self-exited: 8000 step / 30233 gates / 15116 ptrace lines | full hash not comparable; each file is **byte-identical** to the same number of leading lines of the reference run (T13's `arms_g1`, full hash 8284be46): step `654c2186`, gates `28ea1248`, ptrace `e8ae5994` on both sides |
| g2 (`--solo`) | H_DEF | killed at the 900 s cap again: 5473 / 20129 / 10064 lines | byte-identical to the reference prefix again |

Why the cap: the machine was saturated during both runs (95% CPU on 12 cores: `MsMpEng.exe` 263%, the search indexer and
other sessions' Python jobs), so H_DEF ran about 7x slower than T13's 125 s run and the B arms 15x slower (171-215 s vs 12 s).
A solo lock window holds the game lock only, not the CPU. No step, gate or ptrace line differs from the reference where both
exist, and B is equal. **The full G-CTRL hash is not measured on this build.** The C4 lines below are therefore conditional:
apply them only when the merge train's own G-CTRL run on the merged tree reproduces 8284be46 / ba407a16 / 31fe043e (the
train runs it for every source row). If it does not, the swap moved a gate: file an L-n decision.

Raw data (`D:/MashedWT/MATCH-TOPDOWN-T01_data`, not committed):

```
fca9b9fcce89277242178af0d3f27a32a34a7b88efcb557b75d73b16b6b973ac  exe_g1/mashed_re.exe
76db33f09f5a3b16b87773cd9574c3a374a71cd73317db2bc7c20c45115d1d96  build1.log
8de86361d82c04d6b93e7add140857406a50e87dabd485b410b07eaaa5ba411f  build2.log
f354ec744a69f7460be490eca3f7c675be4f54de6417ad1b81cba226c0197c32  gates_g1.log
e996a967b02b68f9e0477a6bf28e3a00130b3e6578c0d5bf023319e9bcd4f42f  gates_g2.log
5a1fe409fda4a4d33ffdbb443e28cd9d725601f2a4bbfe80639240b55e6f0c29  arms_g1/B_DEF_K0.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K0.step.csv
bf9cea9641532456cc5e6614fbbd387c5a47cbc3883f30555d1f470e23170baa  arms_g1/B_DEF_K1.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K1.step.csv
fe46c5bf451896595d7ed57504bafb82d722775c7d7439ed8fb2e37b02129101  arms_g1/B_DEF_K26.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K26.step.csv
28ea1248d3b7f1e4c707efe15cd063b4e642c8415befc68e313474b8a5912cfc  arms_g1/H_DEF.gates.csv
98ca388cab8f883f87c49d5686b33b541801ff3468ded39b38f8a892b1e549c3  arms_g1/H_DEF.log
e8ae59944b5e2aeb0a66af4c3c6dc6062cb2b273b885f27a9e9ab774c11efc09  arms_g1/H_DEF.ptrace.log
654c218615499de64993daa4f8d41fd0068ee1e278014f9ba305d276ea9c2381  arms_g1/H_DEF.step.csv
79ede613f9f446a1d102c5006f978ee8a603685d69eb7e1700428771a62c4d60  arms_g2/H_DEF.gates.csv
c02361edc3b343f764c1f5848561e30dbbde98216e9da87fd259a752987c8e54  arms_g2/H_DEF.log
1299892f20c889b459c7c04e967eb6b879dae4ab54a66991342fc1cac54539a5  arms_g2/H_DEF.ptrace.log
4c3044b33d9d7097901e9e05cc4c4e249f7950da1ec3668151c258a242436f3d  arms_g2/H_DEF.step.csv
```

## Re-classify (prepared, apply after merge through re-classify)

| rva | name | level | notes text |
|---|---|---|---|
| 0x00407a60 | FUN_00407a60 (UtilTableInit8a9620) | C3 -> C4 when the gates below are unchanged | `match evidence: identical (re/match/s01/00407a60.c, match.py 100% 12/12, 40 B), relocheck OK checked=2 unchecked=0, swapped (Match/MatchS01.cpp), exe-callers=2 (PathProg_RaceInit, PathProg_MaybeReplay inline), gates unchanged (MATCH-TOPDOWN-T01)`; replaces the stale `exe-callers=0` note |
