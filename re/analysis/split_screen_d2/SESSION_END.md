# Session End — split_screen_d2-20260507-1852

**Date:** 2026-05-07  
**Slot:** Mashed_pool1 (pool14 stale-locked; alternate used)  
**Model:** claude-sonnet-4-6  
**RVA cap:** 20 Sonnet; 5 RVAs processed (well within cap)  
**Early finish:** no

## Functions analyzed

| RVA | Name | Confidence | Notes |
|-----|------|-----------|-------|
| 0x0041f8f0 | FUN_0041f8f0 | C1 | ESI-based per-player render-pass setup; camera selection from 2-entry table; D-5620 cleared |
| 0x004228f0 | FUN_004228f0 | C1 | Per-player geometry batch draw (15→27 vert expand); alpha-blend bracketed; D-5621 cleared |
| 0x00426060 | FUN_00426060 | C1 | Returns DAT_0065742c; pre-existing C1 from track_loader_d3; D-5622 cleared |
| 0x004260c0 | FUN_004260c0 | C1 | Returns DAT_00657490; D-5623 cleared |
| 0x004e4900 | FUN_004e4900 | C1 | Writes float4 to obj+0x18..+0x24; sets uniform-scale byte at +3; D-5624 cleared |

## Deferred callees filed

D-8440..D-8446 (7 entries): depth-3 callees of PerPlayerViewportRender via 0x0041f8f0 (6 entries) and 0x004228f0 (1 cluster of 3). See DEFERRED.md.

## Uncertainties filed

- **U-2847** — DAT_0063d850 role in FUN_0041f8f0  
- **U-2848** — DAT_0063e490 / DAT_0063d854 camera-selection tables in FUN_0041f8f0

## Pre-existing duplicate note

`hooks.csv` rows 937 (C0/deferred from split_screen session) and 959 (C1 from track_loader_d3) both describe 0x00426060. Row 937 updated to C1 this session; row 959 remains. Duplicate should be de-duplicated by sweep or re-classify.

## U-1908 partial update

U-1908 resolution path included "decomp FUN_004228f0". FUN_004228f0 is now C1 but does NOT answer the viewport-rect question (it draws geometry, not sets camera rasters at +0x60/+0x64). U-1908 remains open.

## Scribe queue

Entry added to re/SCRIBE_QUEUE.md: 5 RVAs, 2 new uncertainties, 7 new deferred, pool=Mashed_pool1.
