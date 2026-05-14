# Session End — title_screen_cont1

**Session ID**: title_screen_cont1-20260513  
**Date**: 2026-05-13  
**Pool slot**: Mashed_pool6 (pool1 locked; pool6 used as fallback)  
**Parent batch**: Session 62 (parallel-fanout discover-c1-batch)

## Result: Full drift-skip

All 7 DEFERRED rows D-7300..D-7306 were already resolved by prior sessions before this session ran:
- D-7300 (0x00428450): already C2 in `hud_ingame_promote_c2` (2026-05-11)
- D-7301 (0x004288a0): already C1 in `title_screen_d2` (2026-05-06)
- D-7302 (0x00428320): already C2 in `frontend_promote_menus_b` (2026-05-11)
- D-7303 (0x0042e590): already C1 in `title_screen_d2` (2026-05-06)
- D-7304 (0x0040d250): already C1 in `title_screen_d2` (2026-05-06)
- D-7305 (0x00401ee0): already C1 in `title_screen_d2` (2026-05-06)
- D-7306 (0x0042f0b0): already C1 in `title_screen_d2` (2026-05-06)

All 7 functions have Ghidra plates with `[C1/C2 2026-05-xx]` comments already applied. No new plates are needed.

## Depth-1 callee check

All 13 depth-1 callees across the 7 functions are already in hooks.csv with C1+ confidence:

| RVA | Name | Confidence | From |
|-----|------|-----------|------|
| 0x0042b8b0 | FUN_0042b8b0 | C1 | 0x00428450 |
| 0x0042b8c0 | FUN_0042b8c0 | C1 | 0x00428450 |
| 0x00450b10 | FUN_00450b10 | C2 | 0x00428450 |
| 0x00428760 | FUN_00428760 | C1 | 0x004288a0 |
| 0x00427e00 | FUN_00427e00 | C1 | 0x004288a0 |
| 0x004282a0 | FUN_004282a0 | C2 | 0x004288a0 |
| 0x00427840 | FontText_UTF16WidenCopy | C2 | 0x00428320 |
| 0x005554d0 | FUN_005554d0 | C1 | 0x00428320 |
| 0x0040bb70 | FUN_0040bb70 | C2 | 0x0042e590 |
| 0x00401570 | FUN_00401570 | C1 | 0x00401ee0 |
| 0x00401da0 | FUN_00401da0 | C1 | 0x00401ee0 |
| 0x004e6680 | FUN_004e6680 | C1 | 0x00401ee0 |
| 0x00426cf0 | FUN_00426cf0 | C1 | 0x00401ee0 |

0 new stubs filed. 0 new uncertainties filed.

## STUBS resolved

S-2460..S-2466 all cleared 2026-05-06 (by title_screen_d2 session).

## DEFERRED rows resolved

D-7300..D-7306 marked RESOLVED in DEFERRED.md.

## Queue Changes

- `title_screen_cont1-20260513` → **Queued** in SCRIBE_QUEUE.md (drift-skip; sweep action = skip)
