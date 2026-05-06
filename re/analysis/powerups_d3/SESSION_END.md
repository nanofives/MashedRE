---
session: powerups_d3
session_id: powerups_d3-20260506-0504
slot: Mashed_pool4
date: 2026-05-06
status: COMPLETE
parent: powerups_d2 (D-4240..D-4243)
---

## Summary

Depth-3 pass on powerups subsystem. Analyzed all 17 RVAs from D-4240..D-4243: 3 named powerups callees (D-4240..D-4242) and 14 RW hierarchy internals (D-4243). One RVA (0x004d8060) was already C1 from rw_engine_teardown_d2; 16 new hooks.csv rows added.

## Functions analyzed (17)

| RVA | Name | Notes |
|-----|------|-------|
| 0x00454170 | FUN_00454170 | ESI-implicit struct-B zero+teardown (D-4241) |
| 0x004547c0 | FUN_004547c0 | EDI-implicit struct-A reset loop (D-4240) |
| 0x00534b60 | FUN_00534b60 | flags normalizer → FUN_00534d00 (D-4242) |
| 0x004c0790 | FUN_004c0790 | DLL node unlink (D-4243) |
| 0x004c0870 | FUN_004c0870 | lookup+DLL insert into global list (D-4243) |
| 0x004c0a60 | FUN_004c0a60 | root-reference propagation through children (D-4243) |
| 0x004c0de0 | FUN_004c0de0 | teardown: children + list + DLL unlink + vtable dispatch (D-4243) |
| 0x004d8060 | FUN_004d8060 | already C1 (rw_engine_teardown_d2); cross-ref only (D-4243) |
| 0x004e4440 | FUN_004e4440 | table lookup *(0x7d7170+param_1) (D-4243) |
| 0x004e4d90 | FUN_004e4d90 | list ForAll + DLL unlink + vtable dispatch (D-4243) |
| 0x004e68a0 | FUN_004e68a0 | set +0x18 field; copy 4 dwords from sub-obj; dirty-path (D-4243) |
| 0x004e6710 | FUN_004e6710 | circular intrusive list ForAll with embedding offset (D-4243) |
| 0x004e6920 | FUN_004e6920 | ForAll + FUN_004d8bd0 + clear +0x18 + vtable dispatch (D-4243) |
| 0x004e6d00 | FUN_004e6d00 | DLL unlink+zero node at (0x7d7230+param_1) + FUN_004e4d90 (D-4243) |
| 0x004e6d80 | FUN_004e6d80 | alloc type 0x30010; 3 circular sentinels; list insert (D-4243) |
| 0x004e6f80 | FUN_004e6f80 | DLL insert: node at param_2+0x40/+0x44 after param_1+8 (D-4243) |
| 0x004e6fe0 | FUN_004e6fe0 | DLL unlink+zero at (0x7d7230+param_2); return param_1 (D-4243) |

## Tracker changes

- **Cleared**: D-4240, D-4241, D-4242, D-4243 (all drained); S-1444
- **New C1 rows**: 16 (0x004d8060 skipped — already C1)
- **New stubs**: S-1920..S-1930 (11 entries)
- **New uncertainties**: U-1927..U-1945 (19 entries)
- **New depth-4 deferrals**: D-5680..D-5686 (7 groups)

## Key observations

1. **Recurring vtable dispatch pattern**: Functions 004c0de0, 004e4d90, 004e6920 all end with `(**(code **)(DAT_007d3ff8 + 0x11c))(...)` — same fn-ptr slot, different first-arg sources (DAT_007d3e70, DAT_007d71a0, DAT_007d7200). Consistent with an RW-style object-type dispatch table. U-1937/U-1939 track this.

2. **Global offset constants**: DAT_007d7230 (embedding offset, used in FUN_004e6d00 and FUN_004e6fe0) and DAT_007d722c (FUN_004e6710 container-offset calc) are runtime values that determine struct field layouts. Both need memory_read to resolve.

3. **RW scene-graph pattern**: FUN_004c0a60 propagates a root-reference field (+0xa0) through a parent/child/sibling tree (fields +4/+0x98/+0x9c). Very consistent with RwFrame scene-graph structure.

4. **FUN_00534b60 is a flags normalizer**: Not an allocator itself — it canonicalizes a flags word via multi-bit conditional masking then passes to FUN_00534d00. The DAT_00623c78 global passed as 4th arg is unknown (U-1931).
