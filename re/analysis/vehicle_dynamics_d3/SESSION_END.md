# SESSION_END — vehicle_dynamics_d3

**Session ID:** vehicle_dynamics_d3-20260512  
**Pool slot:** Mashed_pool0  
**Date:** 2026-05-12  
**Anchor verified:** n/a (read-only decomp; no hook authoring this session)

## Functions analyzed this session

### New C1 plates (8 new functions)

| RVA | Name | Bytes | Notes |
|-----|------|-------|-------|
| 0x00468d80 | FUN_00468d80 | 1876 | VehicleTerrainContactSolver; outer terrain loop + 18-slot inner; event type=6; clears S-3440 |
| 0x004694e0 | FUN_004694e0 | 1467 | VehicleObjectContactSolver; dynamic object list iteration; vehicle-ID match; clears S-3441 |
| 0x00468b40 | FUN_00468b40 | 52 | VehicleContactHistoryLookup; 32-entry scan at vehicle+0xBFC |
| 0x00485360 | FUN_00485360 | 5 | DynamicObjectList_GetCount; returns DAT_006fa0f8 |
| 0x00485370 | FUN_00485370 | 5 | DynamicObjectList_GetBase; returns &DAT_006e87b8 |
| 0x00485420 | FUN_00485420 | 58 | PhysicsObject_GetWorldPosition; validates handle, reads state+0x60 pos |
| 0x00485340 | FUN_00485340 | 27 | PhysicsObject_ValidateHandle; bounds check + table lookup |
| 0x00485380 | FUN_00485380 | 40 | PhysicsObject_GetStateBlock; EAX-convention accessor; stride 0x10 state table |

### Tracker fixes (existing plates, missing hooks.csv rows)

| RVA | Action | Existing plate |
|-----|--------|----------------|
| 0x004c4600 | Add C1 row (was missing; Ghidra comment [C1 2026-05-07] exists) | render_pipeline_d3/004c4600.md |
| 0x004c4dc0 | Promote C0 stub → C1; Ghidra comment [C1 2026-05-07] exists | render_pipeline_d3/004c4dc0.md |
| 0x004c4eb0 | Add C1 row (was missing from hooks.csv) | render_pipeline_d3/004c4eb0.md |

## DEFERRED resolved

- **D-3460** (0x004c4600 FUN_004c4600 matrix_math): Filed in render_frame_tree_d2 SCRIBE_QUEUE entry but never written to DEFERRED.md (tracker drift). Resolved by adding proper hooks.csv row for 0x004c4600 in this session. No DEFERRED.md change needed (entry never existed there).

## STUBS cleared

| Stub ID | RVA | Cleared by |
|---------|-----|------------|
| S-3440 | 0x00468d80 | This session — C1 plate written |
| S-3441 | 0x004694e0 | This session — C1 plate written |
| S-3442 | 0x004c4dc0 | This session — promoted from C0 stub to C1 (plate was render_pipeline_d3/004c4dc0.md) |
| S-2125 | 0x004c4dc0 | Same as S-3442; also present in FontCtx stub list |

## New UNCERTAINTIES: U-3573..U-3577

| ID | Type | Function | Unknown |
|----|------|----------|---------|
| U-3573 | structural | 0x00468d80 | DAT_0088e60c — terrain geometry entry count; update path unknown |
| U-3574 | structural | 0x00468d80 | geometry entry layout: 10-float plane/tri data per 0x90-byte entry; sentinel float values at [0xb] |
| U-3575 | structural | 0x004694e0 | DAT_006e87b8/DAT_006fa0f8 — dynamic physics object list base/count; entry structure unknown |
| U-3576 | structural | 0x00485340/0x00485380 | DAT_006e71c4/cc — physics object manager; state table stride 0x10; calling convention of FUN_00485380 (in_EAX) |
| U-3577 | structural | 0x00468d80 | param_1[0x26f..0x271] (+0x9BC) — 3-float field used as rotation axis at 90°; exact semantic unknown |

## New DEFERRED: none

All callees at depth-3 are either already in hooks.csv or analyzed this session.
S-1483 (FUN_004c4d20 = RwMatrix_SetRotAxisAngle) remains open from prior session — not this session's scope.

## Struct draft

`re/analysis/structs/vehicle_dynamics.md` — initial draft with confirmed byte offsets.

## Key structural findings (new this session)

- **VehicleTerrainContactSolver (0x00468d80)**: iterates DAT_0088e60c terrain mesh entries × 18 wheel slots; contact depth via plane-normal dot product; impulse with optional 90-degree rotation (FUN_004c4d20) for tangential response; enqueues debug events type=6 to ring DAT_007e9de4; calls contact history lookup FUN_00468b40.
- **VehicleObjectContactSolver (0x004694e0)**: iterates dynamic physics objects from DAT_006e87b8/DAT_006fa0f8; vehicle-ID match at entry[0xE]; modifies vehicle linear velocity AND object entry impulse fields directly; accesses object world position via FUN_00485420.
- **Contact history table**: `vehicle_struct + 0xBFC`, 32 sequential int entries; active flags at +0x80 per-slot.
- **Physics object manager**: `DAT_006e71cc`; handle table at +0x10 (int[]), state-block table at +0xc (stride 16), max index at `DAT_006e71c4`.
- **RwMatrixInvert (0x004c4dc0)**: identity fast-path (copy); orthonormal fast-path (3×3 transpose + corrected translation); general cofactor path via FUN_004c4eb0. Shared with leaderboard actor (0x00406ce0) and vehicle-vehicle collision (0x00469df0).
- **Render shared dependency confirmed**: 0x004c4600 (RwMatrixMultiply) and 0x004c4dc0 (RwMatrixInvert) are shared with render (render_pipeline_d3 analyzed them). Vehicle dynamics d3 documents their vehicle-side usage.
