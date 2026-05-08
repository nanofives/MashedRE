# SESSION_END — game_state_d5

**Date:** 2026-05-07  
**Slot used:** Mashed_pool1  
**Session ID:** game_state_d5  
**SHA-256 anchor:** BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓

## Items processed

| D | RVA | File | Confidence | Notes |
|---|---|---|---|---|
| D-7420 | 0x00441c80 | 0x00441c80.md | C1 | lerp of two vehicle XYZ positions; S-2500 resolved |
| D-7421 | 0x004430a0 | 0x004430a0.md | C1 | 9B setter DAT_00897fe0=param_1; S-2501 resolved |
| (new) | 0x004102f0 | 0x004102f0.md | C1 | race-start camera-init coordinator; found as additional caller of FUN_004430a0 |
| (new) | 0x004430b0 | 0x004430b0.md | C1 | 5B getter paired with FUN_004430a0; found as callee of FUN_004102f0 |
| (new) | 0x0046baa0 | 0x0046baa0.md | C1 | per-vehicle physics state reset; 571b leaf; found as callee of FUN_004102f0 |

## Stubs resolved

| S | RVA | Resolution |
|---|---|---|
| S-2500 | 0x00441c80 | Analyzed as C1 — D-7420 complete |
| S-2501 | 0x004430a0 | Analyzed as C1 — D-7421 complete |

## New stubs

| S | D | RVA | Description |
|---|---|---|---|
| S-2929 | D-8689 | 0x00405460 FUN_00405460 | camera spline step; 222 bytes; callee of FUN_004102f0; one unanalyzed callee FUN_00404fa0 (spline matrix eval, 1068b) |
| S-2930 | D-8690 | 0x0040e590 FUN_0040e590 | race setup/car-placement coordinator; 2360 bytes; too large for this pass |

Note: FUN_0040d470 (0x0040d470) was found as a callee of FUN_004102f0 but is already deferred at D-7724 (leaderboard_d3-cont1). Not re-filed.

## New uncertainties

| U | Function | Topic |
|---|---|---|
| U-3010 | 0x004102f0 | DAT_005f29b8 init value 100000: what sets it and what the counter represents |
| U-3011 | 0x004102f0 | `unaff_EBX * 0x44c / 0x3c` delta formula: EBX source unknown |
| U-3012 | 0x0046baa0 | Per-vehicle physics SOA field map at stride 0xd04 |
| U-3013 | 0x00441c80 | slot-override flag at param_2+0x1c: who sets it and when |

## Scope expansion note

The original two stubs (S-2500/S-2501) had all callees already C1 mapped, producing no new stubs directly. Expansion came from tracing callers of FUN_004430a0, which surfaced FUN_004102f0 as an additional call site. FUN_004102f0 in turn produced five new functions (three fully analyzed as leaves, two stubbed as too large).

## Queue changes

- Removed: D-7420, D-7421 (both drained here).
- Added: `game_state_d5-cont1` → D-8689 (0x00405460) + D-8690 (0x0040e590).
