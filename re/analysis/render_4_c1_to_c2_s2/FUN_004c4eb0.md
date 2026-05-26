---
rva: 0x004c4eb0
name_in_ghidra: FUN_004c4eb0
name_identified: RwMatrixInvert_CofactorPath
size_bytes: 337
confidence: C2
callees_depth1: []
callers_noted: [0x004c4dc0 (RwMatrixInvert dispatcher), 0x004c5470 (RwMatrixQueryRotate)]
opened_in_slot: Mashed_pool10
session_date: 2026-05-26
session_id: batch-render-4-s2
subsystem: render
promoted_from: C1
promoted_by: batch-render-4-s2
---

## Promotion rationale

Full decompilation available. Prior plate (render_pipeline_d3/004c4eb0.md)
fully documented this. This session confirms the decompilation matches
exactly. Two callers: the RwMatrixInvert dispatcher (0x004c4dc0 — C1
orthonormal fast-path with this as general fallback) and the matrix
query-rotate function (0x004c5470 — uses this for translation extraction).
Leaf function (no callees), deterministic algorithm.

## Mechanical description

- Signature: `void FUN_004c4eb0(float *dst, float *src)`.
- Computes 3×3 cofactor matrix of src's rotation sub-block:
  - C00 =  src[10]*src[5]  - src[9]*src[6]
  - C01 = -(src[10]*src[1] - src[9]*src[2])
  - C02 =  src[6]*src[1]  - src[5]*src[2]
- Determinant: `det = src[0]*C00 + src[4]*C01 + src[8]*C02`.
- Guard: `inv = (det != 0) ? (1.0f / det) : 1.0f` (constant at _DAT_005cc320).
- Scales all 9 cofactor matrix elements by inv.
- Computes remaining 6 cofactors for the inverse rotation subblock.
- Inverse translation: `T_inv = -(R_inv · T)`.
- Sets `dst[3] = 0.0f` (zero flags in output).
- Mechanical shape: **RwMatrixInvert** general path (cofactor expansion).

## Constants

| Address | Value | Meaning |
|---------|-------|---------|
| _DAT_005cc320 | 1.0f | fallback for det=0 and numerator |

## Uncertainties

- [UNCERTAIN] U-4930: Decompiler mixes `inv` scalar and cofactor indices
  in the last two translation lines — formula is mechanically correct but
  exact indexing should be verified against librw cofactor table.
