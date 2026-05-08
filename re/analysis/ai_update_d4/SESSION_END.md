# Session End: ai_update_d4-20260507

## Session summary

Depth-4 pass on the `ai_update` subsystem. Analyzed all 5 functions deferred by ai_update_d3 (D-5560..D-5564, bucket ai_update_d3-cont1). Note: those rows exist in ai_update_d3/SESSION_END.md but are NOT yet in DEFERRED.md (d3 session is queued, not yet scribed — sweep must reconcile).

Pool slot: Mashed_pool13 (read-only). SHA-256 anchor verified.

## Functions analyzed

| RVA | Name | Notes |
|-----|------|-------|
| 0x004c3b30 | FUN_004c3b30 | RW fast sqrt (single float, lookup table; 4096-entry int[] via DAT_007d3ff8 dispatch) |
| 0x00416230 | FUN_00416230 | Lookahead-index writer (DAT_0089a500 stride 0x74, field 0 = uint32 index) |
| 0x004c3bf0 | FUN_004c3bf0 | 2D vector magnitude: sqrt(x²+y²) via RW fast-sqrt lookup |
| 0x004c3c60 | FUN_004c3c60 | 2D vector normalize + return magnitude; inv-sqrt via dispatch entry [1] (+4 offset); abnormal-mag guard calls FUN_004d7ff0(0x19) + FUN_004d8480 |
| 0x004b55a0 | FUN_004b55a0 | 2D line-segment render (RW Im2D, 2 vertices); reads viewport dims from param_1+0x60 struct; RwRenderStateSet via DAT_007d3ff8+0x20; RwIm2DRenderPrimitive via DAT_007d3ff8+0x28 |

## Tracker changes

- hooks.csv: +5 rows (ai subsystem, C1, mapped)
- DEFERRED.md: no changes (cap_count=0; no functions deferred from this session)
- UNCERTAINTIES.md: +5 rows (U-3067..U-3071)
- STUBS.md: no changes

## Key structural findings

- `DAT_007d3ff8` dispatch layout confirmed further:
  - `+0x18`: RHW value for Im2D vertices
  - `+0x20`: fn-ptr = RwRenderStateSet
  - `+0x28`: fn-ptr = RwIm2DRenderPrimitive
  - `DAT_007d3ffc + DAT_007d3ff8`: entry [0] = fast-sqrt table (4096 ints)
  - `DAT_007d3ffc + 4 + DAT_007d3ff8`: entry [1] = fast-inv-sqrt table (different bit inversion pattern)
- `DAT_0089a500` stride 0x74: new per-vehicle array; field 0 = lookahead spline index (uint32).
- FUN_004b55a0 call site in FUN_00443dc0 is a debug/overlay draw — draws a 2-vertex line along the spline target.
- FUN_004c75e0 (0x004c75e0): Ghidra-annotated [C1 2026-05-06] as viewport origin getter (reads uint16 at +0x1c/+0x1e of viewport sub-obj), but absent from hooks.csv. Flagged as U-3071.

## Conflicts / anomalies

- **D-6461 vs D-5560 conflict** (RVA 0x004c3b30): DEFERRED.md D-6461 (bucket `profile_career_d4`) and ai_update_d3 D-5560 (bucket `ai_update_d3-cont1`) both claim this RVA. This session is authoritative (decompiled). Sweep should clear D-6461 when consolidating.
- **ai_update_d3 deferred rows not in DEFERRED.md**: D-5560..D-5564 appear only in ai_update_d3/SESSION_END.md (d3 is queued, unscribed). Sweep must insert + immediately clear these rows together.

## Uncertainties (U-3067..U-3071)

| U-ID | RVA | Description |
|------|-----|-------------|
| U-3067 | 0x004c3c60 | DAT_005d757c: static reads 0x00000000 (0.0f); may be BSS-initialized to positive epsilon at runtime; used as lower-bound in abnormal-magnitude guard |
| U-3068 | 0x004c3c60 | Error code 0x19 (25) semantics in FUN_004d7ff0/004d8480 error subsystem |
| U-3069 | 0x004b55a0 | RwRenderState codes 7 and 1 at `DAT_007d3ff8+0x20` — exact RW3 enum values not confirmed |
| U-3070 | 0x004b55a0 | DAT_005cc320: Z-numerator constant in Im2D depth calc; static value not read |
| U-3071 | 0x004b55a0 | FUN_004c75e0 (viewport origin getter) annotated C1 in Ghidra but absent from hooks.csv; tracker inconsistency |

## Pool slot

Mashed_pool13 — read-only; no writes; releasing after commit.
