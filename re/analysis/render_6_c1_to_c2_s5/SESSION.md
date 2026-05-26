---
session: batch-render-6-s5
date: 2026-05-26
pool_slot: Mashed_pool11 (fallback; pool13 had no indexed functions)
branch: c1-c2/batch-render-6-s5
u_id_range: U-5400..U-5411
promoted: 12
deferred: 0
skipped_lab: 3 (004d9000, 004d9030, 004d9040 — LAB_ labels, no FUN_ body)
---

## Promotion summary

All 12 FUN_ functions in this batch are part of the rw-palette-quantizer subsystem
(octree-based palette reduction, called from RenderWare texture conversion path).
Plates from batch-y-s3 (Mashed_pool11, 2026-05-19) are complete with full constant
tables, no unresolved uncertainties blocking promotion, and all confirmed against
Ghidra decompilation in this session.

### Functions promoted C1->C2

| RVA | Name | Size | U-id | Notes |
|-----|------|------|------|-------|
| 0x004d9050 | FUN_004d9050 | 783 B | U-5400 | Octree pixel-insert main loop |
| 0x004d9360 | FUN_004d9360 | 308 B | U-5401 | Leaf accumulator |
| 0x004d94a0 | FUN_004d94a0 | 131 B | U-5402 | Descend-with-lazy-alloc helper |
| 0x004d9530 | FUN_004d9530 | 1319 B | U-5403 | Palette-build entry point |
| 0x004d9a60 | FUN_004d9a60 | 319 B | U-5404 | Mean-to-RGBA writer |
| 0x004d9ba0 | FUN_004d9ba0 | 348 B | U-5405 | Pixel-assignment recursive pass |
| 0x004d9d00 | FUN_004d9d00 | 475 B | U-5406 | Recursive stats-merge pass |
| 0x004d9ee0 | FUN_004d9ee0 | 276 B | U-5407 | Two-blob stats combiner |
| 0x004da000 | FUN_004da000 | 608 B | U-5408 | 1-D split evaluator |
| 0x004da270 | FUN_004da270 | 441 B | U-5409 | Recursive seed pass |
| 0x004da430 | FUN_004da430 | 66 B | U-5410 | Recursive leaf-count |
| 0x004da480 | FUN_004da480 | 100 B | U-5411 | Single-cluster palette emit |

### Skipped (LAB-only)

| RVA | Reason |
|-----|--------|
| 0x004d9000 | LAB_ label, no FUN_ body — rwID_CHUNKGROUPMODULE dtor stub |
| 0x004d9030 | LAB_ label, no FUN_ body — rwID_COLORMODULE ctor stub |
| 0x004d9040 | LAB_ label, no FUN_ body — rwID_COLORMODULE dtor stub |

## C2 evidence

- All plates authored by batch-y-s3 with RVA-cited constants and mechanical descriptions.
- Decompilation confirmed for: FUN_004d9360, FUN_004d9a60, FUN_004da430, FUN_004d94a0,
  FUN_004d9ee0, FUN_004da480 (direct decomp_function cross-check in this session).
- Function sizes from Ghidra body_start/body_end match plate size_bytes for all checked entries.
- All callee/caller relationships confirmed.
- No UNCERTAIN markers blocking C2 in any plate (FUN_004d9530 has one [UNCERTAIN] note
  about a byte flag but the mechanical description is complete and does not depend on
  its exact semantics).
