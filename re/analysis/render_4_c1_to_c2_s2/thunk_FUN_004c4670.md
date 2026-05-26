---
rva: 0x004c4670
name_in_ghidra: thunk_FUN_004c4670 (thunk to 0x004c4680)
size_bytes: 5
confidence: C2
callees_depth1: [0x004c4680]
callers_noted: [0x00457100, 0x0046e9e0, 0x004c1680, 0x004cf2b0, 0x004fb2a0]
opened_in_slot: Mashed_pool10
session_date: 2026-05-26
session_id: batch-render-4-s2
subsystem: render
promoted_from: C1
promoted_by: batch-render-4-s2
---

## Promotion rationale

Ghidra marks this as a thunk. The 5-byte body is a single JMP to
0x004c4680 (FUN_004c4680, the matrix orthonormalize function). Prior
plate (bucket_004c4270/0x004c4670.md, batch-x-s3) fully documented the
target semantics. This session confirms the thunk is a simple forwarding
stub.

The target 0x004c4680 is identified as RwMatrixOrthoNormalize: normalizes
all three rows of an RwMatrix, then re-orthogonalizes via cross products,
and sets flags bits 0+1 (rwMATRIXTYPEORTHONORMAL).

## Mechanical description

- 5-byte JMP to 0x004c4680 (RwMatrixOrthoNormalize).
- Ghidra inlines the target during decompilation.
- 5 callers use this thunk entry point instead of calling 0x004c4680 directly.
  - 0x004c1680: frame orthonormalize-and-dirty (documented as C1).
  - 0x00457100, 0x0046e9e0, 0x004cf2b0, 0x004fb2a0: various subsystems.
- Mechanical shape: forwarding thunk for RwMatrixOrthoNormalize.

## Constants

None (pure thunk).

## Uncertainties

None — thunk with fully identified target.
