# SESSION_END — effects_particle_d3

**Session ID:** effects_particle_d3-20260506
**Slot used:** Mashed_pool2 (~8h stale; no master sync available; all slots stale)
**SHA-256:** BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓

## Functions analyzed (4)

| RVA | Name | Confidence | Notes |
|-----|------|-----------|-------|
| 0x004e6100 | FUN_004e6100 | C1 | atomic world-transform sync; D-2803 cleared |
| 0x00538d60 | FUN_00538d60 | C1 | line-segment vs geom type-1 (BVH + Möller–Trumbore); D-2800 cleared |
| 0x00539900 | FUN_00539900 | C1 | AABB vs geom type-4 (BVH + SAT pre-test); D-2801 cleared |
| 0x00539ec0 | FUN_00539ec0 | C1 | sphere vs geom types 3+5 (BVH + sphere-AABB + sphere-tri); D-2802 cleared |

## Deferred cleared
D-2800, D-2801, D-2802, D-2803 — all removed from DEFERRED.md

## New tracker entries
- Uncertainties: U-1967..U-1982 (16 entries)
- Stubs: S-1960..S-1965 (6 entries)
- Deferred (new): none (no further recursion required per session spec)

## New stubs (depth-4 callees, not recursed)
| Stub | RVA | Caller | Notes |
|------|-----|--------|-------|
| S-1960 | 0x004e5fc0 | FUN_004e6100 | called when *(atomic+0x4c) & 2 |
| S-1961 | 0x004c0b10 | FUN_004e6100 | predicate on secondary object |
| S-1962 | 0x004c0ed0 | FUN_004e6100 | get matrix from secondary object |
| S-1963 | 0x004c3d60 | FUN_004e6100 | matrix concat |
| S-1964 | 0x00547bf0 | FUN_00539900 | AABB vs triangle SAT pre-test |
| S-1965 | 0x00547450 | FUN_00539ec0 | sphere vs triangle pre-test |

## Key findings
1. **FUN_00538d60 / FUN_00539900 / FUN_00539ec0** are all geometry-query traversal handlers with identical BVH-traversal skeletons. They differ only in query shape (line-segment, AABB, sphere) and inner triangle test.
2. **FUN_004e6100** computes the world-space bounding sphere of an atomic-like object: it gets the frame matrix, matrix-concatenates local→world, finds the maximum column scale via sqrt(max_sq_col_magnitude), and scales the local radius to produce the world-space radius.
3. **Global `DAT_007dc5b4`** confirmed as collision-data table base (same offset-indexing pattern as DAT_007dc57c from d2).
4. Constants confirmed at @0x005d757c=0.0f, @0x005cc320=1.0f, @0x005ce54c≈1e-6 (BVH epsilon), @0x005e4574≈1e-8 (Möller–Trumbore det threshold), @0x005cea1c≈-1e-5 (barycentric tolerance), @0x005e4578≈0.570f (sphere contraction).
5. No further depth-4 recursion is required for this batch per prompt spec.

## Subsystem overview update
The effects_particle subsystem now has 8 analyzed functions (d1: 4, d2: 4, d3: 4). The geometry-query dispatcher (FUN_00538c80) is fully mapped with all 4 case handlers at C1.
