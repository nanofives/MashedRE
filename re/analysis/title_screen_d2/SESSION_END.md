# Session End — title_screen_d2

**Session ID**: title_screen_d2-20260506  
**Date**: 2026-05-06  
**Pool slot**: Mashed_pool1 (pre-assigned pool15 had stale lock from prior session)  
**Parent**: title_screen-20260506-VVVVV (title_screen-cont1)

## Functions Analyzed

| RVA | Name | Confidence | Notes |
|-----|------|-----------|-------|
| 0x004288a0 | FUN_004288a0 | C1 | Menu layout renderer; 8 elements (image+7 sprites); S-2461 cleared |
| 0x0042e590 | FUN_0042e590 | C1 | 2-insn wrapper → FUN_0040bb70 → FUN_004c5c00 string search; S-2463 cleared; U-2547 filed |
| 0x0040d250 | FUN_0040d250 | C1 | Indexed ptr dereference getter 16b; S-2464 cleared |
| 0x00401ee0 | FUN_00401ee0 | C1 | Object-select + RW matrix + RpClumpRender; S-2465 cleared; U-2548 filed |
| 0x0042f0b0 | FUN_0042f0b0 | C1 | int getter DAT_0067f17c+0x49; S-2466 cleared |

## Previously-C1 Stubs Confirmed and Cleared

| RVA | Stub | Notes |
|-----|------|-------|
| 0x00428450 FUN_00428450 | S-2460 | Already C1 (hud_ingame_d3); U-2470 resolved (param_1=X_offset, param_2=Y_offset) |
| 0x00428320 FUN_00428320 | S-2462 | Already C1 (hud_frontend_d3 text-width variant B) |

## Inline Callee Classifications (depth+1)

| RVA | Name | Confidence | Notes |
|-----|------|-----------|-------|
| 0x004c5c00 | FUN_004c5c00 | C1 | Case-insensitive linked-list string search 114b; S-2540 |
| 0x00401570 | FUN_00401570 | C1 | Table scan 36b (DAT_00636578, stride 0x68, 13 entries); S-2541 |
| 0x00401da0 | FUN_00401da0 | C1 | RW matrix setup+dirty 308b; S-2542 S-2543 |
| 0x00426cf0 | FUN_00426cf0 | C1 | Addr getter returns &DAT_0066d6e4 5b; S-2544 |
| 0x004c1480 | FUN_004c1480 | C1 | RW frame dirty-list insert 145b; S-2543; D-7540 for FUN_004c52f0 |

## New Tracker IDs

- **U-2547**: FUN_0042e590 string key arrives via untracked register (not visible in Ghidra decomp)
- **U-2548**: RpClumpRender arg not visible in FUN_00401ee0 decompilation
- **S-2540**: FUN_004c5c00 (string search, depth-3 of D-7303)
- **S-2541**: FUN_00401570 (table scan, depth-3 of D-7305)
- **S-2542**: FUN_00401da0 (RW matrix setup, depth-3 of D-7305)
- **S-2543**: FUN_004c1480 (RW frame dirty insert, depth-4 of D-7305)
- **S-2544**: FUN_00426cf0 (addr getter, depth-3 of D-7305)
- **D-7540**: FUN_004c52f0 (unknown RW op, depth-4; bucket title_screen_d2-cont1)

## Uncertainties Updated

- **U-2469**: Partially resolved — call chain confirmed as asset lookup (FUN_004c5c00 string search); remaining: how attract renderer uses the returned node pointer for the car sprite draw
- **U-2470**: Resolved — param_1=X_offset, param_2=Y_offset (additive, before screen-dimension scaling)

## Queue Changes

- `title_screen-cont1` → **COMPLETED** (all 7 items D-7300..D-7306 drained)
- `title_screen_d2-cont1` → **queued** (D-7540 for FUN_004c52f0)
