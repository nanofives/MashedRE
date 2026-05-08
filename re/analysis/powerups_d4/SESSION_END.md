---
session: powerups_d4
session_id: powerups_d4-20260508-1823
slot: Mashed_pool5
date: 2026-05-08
status: COMPLETE
parent: powerups_d3 (D-5680..D-5686)
---

## Summary

Depth-4 pass on powerups subsystem. Analyzed all 10 uncovered callees from D-5680..D-5686. S-1930 (0x004d8000) was already C1 from intro_splash_d3 — drift-cleared without re-analysis. Pool4 was stale-locked; switched to pool5.

## Functions analyzed (10)

| RVA | Name | Notes |
|-----|------|-------|
| 0x004c1210 | FUN_004c1210 | detach-from-parent + self-root propagation (D-5680) |
| 0x004c15c0 | FUN_004c15c0 | identity-matrix frame reset / RwFrameIdentity (D-5680) |
| 0x004e43b0 | FUN_004e43b0 | detach geometry + release list refs via FUN_004d8bd0 (D-5680) |
| 0x004e4800 | FUN_004e4800 | table lookup *(DAT_007d7174+param_1); 13 bytes (D-5681) |
| 0x00534d00 | FUN_00534d00 | particle system constructor: alloc frame+descriptor; decode stream flags; register callbacks (D-5682) |
| 0x004c0910 | FUN_004c0910 | deep-clone frame node tree (recursive); FUN_004d8000+FUN_004d8090 on success (D-5683) |
| 0x004c0d70 | FUN_004c0d70 | recursive frame node destructor (D-5684) |
| 0x004e8e90 | FUN_004e8e90 | AddRef: *(short*)(+0xe) += 1; 8 bytes (D-5685) |
| 0x004e8ea0 | FUN_004e8ea0 | Release/decref: full destructor at refcount ≤ 1 (D-5685) |
| 0x004d8bd0 | FUN_004d8bd0 | particle descriptor destructor: cleanup cb + DLL unlink + counter subtract + free (D-5686) |

## Tracker changes

- **Cleared**: D-5680..D-5686 (all 7 groups drained); S-1920..S-1930 (11 stubs resolved, S-1930 was drift)
- **New C1 rows**: 10
- **New stubs**: S-3520..S-3525 (6 entries)
- **New uncertainties**: U-3527..U-3537 (11 entries)
- **New depth-5 deferrals**: D-10480..D-10483 (4 groups)

## Key observations

1. **Frame/matrix infrastructure**: FUN_004c1210 (detach-reparent) and FUN_004c15c0 (frame identity reset) share an identical dirty-mark epilogue — DLL insert into DAT_007d3ff8+0xbc when root's bits+3 low 2 == 0. This is the RW dirty-frame propagation mechanism.

2. **Particle system constructor (FUN_00534d00) is the hot path**: Takes 4 args including a 4-entry callback table. Allocates an RW frame and a 0x138-byte descriptor. The bit-decode path (flag 0x20000000) decodes 9 stream types from low 9 bits of param_2, computing per-stream offsets within a single aligned vertex buffer. This is likely a full RW particle emitter init.

3. **AddRef/Release pair (FUN_004e8e90 / FUN_004e8ea0)**: Classic reference-counted object. The Release destructor clears 3 resource fields (+0x54, +0x58, +0x5c), removes from global list at 0x006186d0, tears down sub-object at +0x20, and deallocates. The global list at 6186d0 is a separate particle object pool from the frame list at 617f78.

4. **FUN_004d8bd0 global counter**: Subtracts param_1[2] from a counter at DAT_00911ad8+4+DAT_007d3ff8. This implies the particle system tracks total allocated memory/object weight in a global accounting field. U-3536 tracks this.

5. **Pool4 stale lock**: Mashed_pool4.lock existed with no MCP session open. Removed and fell back to pool5. Pool4 may need investigation before next use.
