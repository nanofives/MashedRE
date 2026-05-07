# SESSION_END — rw_engine_teardown_d3

**Session ID:** rw_engine_teardown_d3-20260503  
**Pool slot used:** Mashed_pool10 (pre-assigned Mashed_pool4 was locked; pool10 was truly free)  
**Date:** 2026-05-03

## Functions analyzed this session

| RVA | Name | Result |
|---|---|---|
| 0x004ccce0 | FUN_004ccce0 | C1/mapped — bitmap-block linked-list iterator with vtable alloc/free and callback dispatch |

## Cross-references resolved (no new analysis needed)

| Stub/Deferred | RVA | Resolution |
|---|---|---|
| D-0580 | 0x004ccce0 | Analyzed this session |
| D-0581 | 0x004cc9f0 | Already mapped in render_d3d9_device |
| S-0220 | 0x004d7ff0 | Already mapped in audio_rws_loader_d2 (identity error-code pass-through) |
| S-0221 | 0x004d8480 | Already mapped in audio_rws_loader_d2 (last-RWS-error record setter) |
| S-0223 | 0x004cc9f0 | Already mapped in render_d3d9_device |

## New trackers

| ID | Type | Subject |
|---|---|---|
| U-1687 | UNCERTAIN/structural | param_1 struct layout for FUN_004ccce0 |
| U-1688 | UNCERTAIN/structural | +7 constant in address alignment formula at 0x004ccd7d |
| U-1689 | UNCERTAIN/structural | vtable+0x108 allocator identity |
| U-1690 | UNCERTAIN/structural | Linked-list node layout |
| S-1680 | STUB | LAB_004d7d70 — unrecognized callback function |
| D-4960 | DEFERRED | LAB_004d7d70 depth-4; pickup in rw_engine_teardown_d3-cont1 |

## Depth-4 queue

One function deferred for depth-4 analysis:
- **D-4960**: `LAB_004d7d70` at 0x004d7d70 — 37-byte unrecognized callback; Ghidra has no FUN_ entry. Body: reads param_1+0x38 sub-struct; conditionally clears fields +0x10/+0x14; calls vtable `DAT_007d3ff8+0x11c(param_1, param_2)`. Pickup bucket: `rw_engine_teardown_d3-cont1`.

## Notes

- Pre-assigned slot Mashed_pool4 had both `.lock` and `.lock~` files (active Ghidra NIO channel lock from a prior session). The pool script's `status` command only checks `.lock` and missed the `.lock~`; pool10 was the first truly free slot.
- S-0220, S-0221, S-0223 in STUBS.md are cross-refs to functions already analyzed in other subsystem sessions. These stubs can be cleared by re-classify when the render subsystem reaches that promotion gate.
