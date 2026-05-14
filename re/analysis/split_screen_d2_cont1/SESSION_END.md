# Session End — split_screen_d2_cont1-20260513

**Date:** 2026-05-13  
**Slot:** Mashed_pool3 (acquired as pool0, upgraded to pool3 — pool0 locked)  
**Model:** claude-sonnet-4-6  
**RVA cap:** 24 Sonnet; 19 RVAs analyzed (within cap)  
**Early finish:** no

## Functions analyzed

### DEFERRED drains (10 RVAs, 7 rows)

| RVA | Assigned name | Confidence | Notes |
|-----|--------------|-----------|-------|
| 0x004c1340 | RwFrameTranslate | C1 | D-8440 resolved; calls RwMatrixTranslate + dirty-list insert |
| 0x004b42c0 | CameraLookAt | C1 | D-8441 resolved; cross-product LookAt on RwFrame |
| 0x004c1680 | RwFrameUpdateObjects | C1 | D-8442 resolved; self-sync + dirty-list insert |
| 0x00422ac0 | PerPlayerTextureArraySet | C2 | D-8443a resolved; copies 4-elem color array into GroundOverlayBatch |
| 0x00422af0 | PerPlayerCameraHandleSet | C2 | D-8443b resolved; stores camera handle in GroundOverlayBatch |
| 0x00422570 | PerPlayerGroundTriangleCollect | C1 | D-8444 resolved; camera-gated triangle collector |
| 0x00426e00 | GlobalViewScaleGet | C1 | D-8445 resolved; returns _DAT_00644368 as float10 |
| 0x004cd070 | RwRenderPrimitiveSubmit | C1 | D-8446a resolved; fills RW command buffer + submits |
| 0x004cd2d0 | RwRenderIndexedSubmit | C1 | D-8446b resolved; indexed draw dispatch (5-case switch on topology) |
| 0x004cd140 | RwRenderCommandBufferReset | C1 | D-8446c resolved; zeros 15 undefined4s in command slot |

### Fill slots (9 RVAs)

| RVA | Assigned name | Confidence | Notes |
|-----|--------------|-----------|-------|
| 0x004c0ed0 | RwCameraGetFrame | C1 | Checks LTM dirty, returns param_1+0x50 |
| 0x004c1480 | RwFrameSetMatrix | C1 | RwMatrixCombine + dirty-list insert |
| 0x004b4ad0 | WorldSectorTriangleQueryWrapper | C1 | Adds type=3 flag, calls FUN_004b4a80 |
| 0x004b4a80 | WorldSectorTriangleQueryInner | C1 | Wraps FUN_00538c80 with callback context |
| 0x004c39b0 | RwV3dNormalize | C1 | Dot-product + fast-inv-sqrt tables; returns magnitude |
| 0x004c52f0 | RwMatrixCombine | C1 | Replace/preconcat/postconcat with orthonormal fast-path |
| 0x00477e60 | TrianglePlaneCompute | C1 | Cross-product plane from 3 vertices; normalizes normal |
| 0x00477f50 | PointPlaneClassify | C1 | Signed distance → returns 0/1/2 (front/on/back) |
| 0x00472500 | PlayerColorGet | C1 | Toggle-selected RGBA from per-player color table |

## DEFERRED resolved

D-8440, D-8441, D-8442, D-8443, D-8444, D-8445, D-8446 — all 7 rows marked RESOLVED in DEFERRED.md.

## Struct drafted (STOP-AND-ASK condition triggered)

≥3 plates share per-player viewport and ground-overlay state → `re/analysis/structs/split_screen_viewport.md` drafted:
- **SplitScreenViewportState** (base DAT_0063dc38, stride 0x2AC, 14 fields documented)
- **GroundOverlayBatch** (fragmented flat arrays, stride 0xf40, 12 slots documented)

## New callees (C0, to defer if out of scope)

| RVA | Context |
|-----|---------|
| 0x004c4680 (thunk) | Matrix update/sync, called from RwFrameUpdateObjects |
| 0x004c45f0 | Rotation matrix from right-vector, called in CameraLookAt |
| 0x004c3d90 | Matrix × 3 vertices transform, called in PerPlayerGroundTriangleCollect |
| 0x00538c80 | RpWorldSectorForAllTriangles (BSP iterator), called in WorldSectorTriangleQueryInner |
| LAB_004b49b0 | Triangle accumulator callback (not a function entry) |
| 0x004c3ac0 | RwV3dLength, called in PointPlaneClassify |

## Uncertainties unchanged

- U-2847 (DAT_0063d850 guard role) — partially narrowed: it holds a position-source object at +4; full role still unclear.
- U-2848 (DAT_0063e490/0063d854 camera tables) — mode-flag-indexed (bVar5 = mode==2 selects [1] vs [0]); table contents still unknown.

## Scribe queue

Entry added to re/SCRIBE_QUEUE.md: 19 RVAs, 7 DEFERRED resolved, 1 struct doc, pool=Mashed_pool3.
