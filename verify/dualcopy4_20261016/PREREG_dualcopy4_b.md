# PREREG dualcopy4 group (b), block 1: four mechanical consolidations (0x00420de0, 0x00405890, 0x0042f6b0, 0x0042ad90)

Committed BEFORE the first measuring run of these four. Group (a) ran first (`wa/`, results in RESULT_dualcopy4.md); the code for this block
was written while window A waited on foreign MASHED.exe processes, and no run touched it. Worker survey that fixed the facts below:
`survey_b_result.txt` (account2, read-only, file:line citations, 7 RVAs).

## 1. Facts (from the survey, re-read in the tree)

| RVA | bodies before | what differs | exe reachability |
|---|---|---|---|
| 0x00420de0 | `Util/PromoLoop_sessionB.cpp:Accum420de0` (naked x87, installed, key `accum_420de0`) and `Vehicle/Damage.cpp:VehicleDamageAccumulatorAdd` (plain C, MASS-DISABLED, no caller, asi only) | the C copy compares the rounded f32 sum, the original compares the unrounded 80-bit st0 (fcomp at 0x00420df1) | neither in the exe |
| 0x00405890 | `Util/PromoLoop_round78.cpp:Pred405890` (asi), `Race/RuleEngine.cpp:CollectAllDone(Persist)` (exe), `Gameplay/PickupCollect_naked.inc:PK_405890` (exe, verbatim naked, called by `PK_458920` at 0x0045894f) | nothing in logic: `total == 0 ? 0 : done == total` over three data sources | the pickup tick calls PK_405890; RuleEngine calls CollectAllDone |
| 0x0042f6b0 | `Frontend/FrontendState.cpp:MenuModeSync` (both targets, installed in the asi) and `Race/RaceModes.cpp:SelectionToGameMode` (exe, pure, -1 = no write) | same nine-case table; one writes `0x0067e9fc`, one returns | MenuNavSM `RowModeSync` calls SelectionToGameMode |
| 0x0042ad90 | `Util/PromoLoop_sessionB.cpp:Find42ad90` (+ naked EDX/EBX/EDI shim, installed, key `find_42ad90`) and `Frontend/MenuNavSM.cpp:KvLookup` (exe) | none for any input (terminator is never matched against the key in either; null table = -1; negative occurrence never equals the counter) | 14 call sites in MenuNavSM |

## 2. Changes

1. **0x00420de0**: delete the dead C copy and its `/export` pragma from `Vehicle/Damage.cpp` (the other three functions in that file stay).
2. **0x00405890**: `Race/CollectDone.h:CollectDoneBody405890(total, done)` is the one body (header-inline, so the host `ruleengine_test` still builds
   with RuleEngine.cpp alone). `Race/CollectDone.cpp:Pred405890` (new, both rsp lists) reads the two globals and carries the install and the registry
   export; `PK_405890` is deleted and `PK_458920` calls `Pred405890`; `RuleEngine::CollectAllDone` calls the header. `Util/PromoLoop_round78.cpp` deleted.
3. **0x0042f6b0**: `Race/SelectionToGameMode.h:SelectionToGameModeBody` is the one table; `MenuModeSync` calls it and writes only when the result is >= 0;
   `RaceModes::SelectionToGameMode` is a one-line wrapper (no RVA anchor).
4. **0x0042ad90**: the C body `Find42ad90` moves to `Frontend/KvFind.cpp` (new, both rsp lists, stack ABI kept for the registry driver); the naked shim in
   sessionB calls it; `KvLookup` is a call into it.

## 3. Gates and predictions

| gate | pass criterion | prediction |
|---|---|---|
| G-LINT | NEW = 0, asm-lint OK, both targets build | PASS. Allowlist 27 -> **23** (00420de0 DUP, 00405890 CROSS, 0042f6b0 DUP, 0042ad90 CROSS go; measured already after the edits: 23) |
| G-HOST | `run_tests.bat`: navsm, ruleengine, racemodes GREEN; menustr and badges GREEN when run with the main checkout's assets as cwd | PASS (navsm exercises KvLookup over the 34-screen BFS, racemodes the table) |
| G-PATH1 on the new asi (window B) | `accum_420de0`, `pred_405890`, `menu_mode_sync`, `find_42ad90`: GREEN and non-degenerate (the CSV has more than one distinct value) | GREEN. The `menu_mode_sync` run uses the negctl fixture the registry applies at import (2026-10-11), which seeds `DAT_0067f184` |
| G-PATH2 | `run_verify_hook.py` PASS (E9, rel32, entries) | PASS for 00420de0 and 00405890. For `find_42ad90` and `menu_mode_sync` the call-through may hit the "bad argument count" harness class (see wa/); then the report is E9 + rel32 site-verified, entries 0, and the row says so |
| G-CTRL (exe, TRAINING rule 4) | `2b4d47bb / 356821c8 / f7a155e1` | UNCHANGED: AI/physics path does not read the changed code. The frontend table and KvLookup run in the menu before the race, so a wrong table would change the race the harness launches |
| G-BOOT + G-NAV (asi) | install_observe ALL 45 s survives, nav boot reaches a race | PASS; `Pred405890` and `Find42ad90_RegAbi` each once in the manifest, 1290 installed |
| M-CTRL | an asi whose `CollectDoneBody405890` tests `done != total` | `pred_405890` path1 RED (so GREEN means something) |
| Rows | `exe_file == file` for 0x00405890 (`Race/CollectDone.cpp`), 0x0042f6b0 (`Frontend/FrontendState.cpp`), 0x0042ad90 (`Frontend/KvFind.cpp`); 0x00420de0 `file` -> `Util/PromoLoop_sessionB.cpp` | after the transaction |

## 4. Decision rule

A consolidation lands when G-LINT, G-HOST, G-PATH1 pass and G-CTRL is unchanged. A path2 report that is site-verified-only is recorded as that, not as a
full pass. If a path1 key is RED or degenerate the change for that RVA is reverted and the row is reported.

## 5. Still open after this block (pre-registered separately before their runs)

0x0042fa00 (decomp adjudication of which body is faithful), RaceCamera Vec3Mag / Vec3Norm / RotateAboutAxis (d1r2 replay), 0x0042d3e0 and 0x00410510
(judged by reading; expected to be skipped with the reason stated).
