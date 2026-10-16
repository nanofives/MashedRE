# PREREG dualcopy4 group (b), block 2: 0x0042fa00 (which body is the original?) and the controls for blocks 1 and (a)

Committed BEFORE the first measuring run of these. Read this block against `PREREG_dualcopy4_b.md`.

## 1. 0x0042fa00: the two bodies disagree with each other, one of them with the original

The decompilation of the original (`decomp_42fa00.txt`, `re/tools/decomp_pc.py 0x0042fa00`) guards the decrement with `*piVar5 != 0`, the increment
with `*piVar5 < 2`, and has no clamp. `Frontend/MenuNav.cpp:MenuTeamSelectTick` (both targets, exe's caller `exe_main.cpp:5945`) matches that.
`Frontend/SkeletonAndScatter_t6.cpp:PlayerSlotEdgeAdjust` (asi only, the INSTALLED hook until now, hooks.csv C2) guards `*team > 0` and adds a
trailing `[0, 2]` clamp. The two agree for every team value in [0, 2]. The existing registry key `menu_team_select_tick` (7 vectors, all in [0, 2])
therefore could not tell them apart.

Change: install `MenuTeamSelectTick` (RH_ScopedInstall in MenuNav.cpp), delete `PlayerSlotEdgeAdjust`, retire key `player_slot_edge_adjust`. Two
discriminating path1 vectors are added to `menu_team_select_tick` (v7: team -1 with UP pressed -> original -2, clamped twin 0; v8: team 3, no press
-> original 3, clamped twin 2) and `path2_tests` (the entry had none because the install was the other twin).

| gate | pass criterion | prediction |
|---|---|---|
| G-PATH1 on the new asi | `menu_team_select_tick` 9/9 GREEN, v7 and v8 non-degenerate (observed values differ from the seeds) | GREEN: the body is the one in MenuNav.cpp that already diffed 7/7 |
| G-MCTRL-twin (asi = base.asi, retired export) | the same 9 vectors run against `PlayerSlotEdgeAdjust` exported by base.asi (temporary registry key `mctrl_pse`, not committed) | RED on exactly v7 and v8, GREEN on v0..v6. If it is GREEN on v7/v8 the vectors do not discriminate and the claim "the installed twin differed" is withdrawn |
| G-PATH2 | PASS with entries >= 1 (E9 + rel32 at 0x0042fa00 -> `MenuTeamSelectTick`) | PASS |
| G-BOOT / G-NAV / G-CTRL (exe) | as block 1 | exe unchanged: the exe already ran this body; `2b4d47bb / 356821c8 / f7a155e1` |
| Row | 0042fa00: `file` and `exe_file` -> `Frontend/MenuNav.cpp`; C2 -> C3 only if G-PATH1/G-PATH2/G-BOOT pass; level stays C2 otherwise | |

## 2. Controls for earlier blocks (mutant asi, one build, two mutations)

| control | mutation | expected |
|---|---|---|
| M-CTRL timer | `Util/TimerInit.cpp:TimerBitFieldSet` tests `flag == 0` instead of `flag != 0` | `run_diff.py timer_bit_field_set` RED |
| M-CTRL pred | `Race/CollectDone.h:CollectDoneBody405890` compares `done != total` | `run_diff.py pred_405890` RED |

## 3. Search4c5c00 (0x004c5c00)

`search_4c5c00` has no `run_diff.py` handler (`circular_str_search_ci` exists only in the early-window lane), so its path1 is
`re/frida/early_window_leaf_diff.py search_4c5c00` (the lane the 2026-09-06 evidence used; log/diff_search_4c5c00.csv). path2 for this key has
no drivable call-through (a circular list needs a heap ring the harness cannot build), so the accepted evidence is the E9 + rel32 site check, as
recorded on 2026-09-06. Prediction: path1 GREEN 5/5, path2 site-verified, entries 0, reported as that.
