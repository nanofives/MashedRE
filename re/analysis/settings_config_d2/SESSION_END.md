# SESSION_END — settings_config_d2
**Date:** 2026-05-03  
**Session ID:** settings_config_d2-20260503-0353  
**Pool slot used:** Mashed_pool4 (pool5 Ghidra-locked from prior session; pool4 already open read-only)  
**Bucket:** re/analysis/settings_config_d2/  

## Functions processed

All 8 DEFERRED rows D-2380..D-2387 resolved:

| D-ID | RVA | Proposed name | Classification | Notes file |
|------|-----|---------------|---------------|-----------|
| D-2380 | 0x004a42c5 | sprintf_to_buf | C1 | 004a42c5.md |
| D-2381 | 0x00498c00 | VideoModeTableInit | C1 | 00498c00.md |
| D-2382 | 0x00498a00 | FormatDisplayModeString | C2 | 00498a00.md |
| D-2383 | 0x00498e40 | FindDefaultFullscreenMode | C2 | 00498e40.md |
| D-2384 | 0x00498ea0 | SnapshotCurrentVideoSettings | C1 | 00498ea0.md |
| D-2385 | 0x004c2e70 | RW_SetCurrentSubsystem | C1 | 004c2e70.md |
| D-2386 | 0x004c2f30 | RW_SetCurrentMode | C1 | 004c2f30.md |
| D-2387 | 0x004c2ed0 | RW_GetModeInfo | C1 | 004c2ed0.md |

## Key findings

**Display mode table structure** (from FUN_00498c00 / VideoModeTableInit):
Three parallel global arrays enumerate all RW display subsystems and their modes:
- `DAT_007731fc` — array of 0x50-byte subsystem-info structs (one per subsystem)
- `DAT_007731f8` — int array of mode counts per subsystem
- `DAT_00773408` — ptr array to malloc'd mode-info arrays (0x18 bytes/entry = 6 ints)

**Mode struct layout** (0x18 bytes, from FUN_00498a00 + FUN_00498e40):
| Offset | Field | Type |
|--------|-------|------|
| +0x00 | width | int |
| +0x04 | height | int |
| +0x08 | depth | int |
| +0x0C | flags | uint (bit 0 = fullscreen, bit 1 = interlaced, bit 2 = flicker-free, bits 8-9 = FSAA) |
| +0x10 | (unknown) | int |
| +0x14 | (unknown) | int |

**Settings snapshot area** (from FUN_00498ea0 / SnapshotCurrentVideoSettings), written before save:
- 0x0077338c — snapshot_subsystem_index
- 0x00773208 — version/type tag = 5 [UNCERTAIN: U-1227]
- 0x00773390 — snapshot_mode_index
- 0x0077320c — subsystem-info copy (via RW vtable 0xCC) [UNCERTAIN: U-1228]
- 0x0077330c — mode name string (e.g. "800 x 600 x 32 [Fullscreen]")
- 0x00773394/98/9c/a0 — width, height, depth, fullscreen flag

**RW video command IDs** (from FUN_004c2e70 / 004c2f30 / 004c2ed0, all via FUN_004c2c90 D-0229):
- ID 0x10 = select subsystem
- ID 7 = select mode  
- ID 6 = get mode info (fill 0x18-byte mode struct)

**Minimum viable fullscreen mode** (FUN_00498e40): first mode with fullscreen flag AND width ≥ 640.

## New DEFERRED (depth-3)

D-3580..D-3585 filed; bucket settings_config_d2-cont1:
- D-3580: 0x004a504f FUN_004a504f — CRT _output (no recursion)
- D-3581: 0x004c2e40 FUN_004c2e40 — RW default-subsystem getter
- D-3582: 0x004c2f00 FUN_004c2f00 — RW default-mode getter [OVERLAP: re/DEFERRED D-0040]
- D-3583: 0x004c2de0 FUN_004c2de0 — RW subsystem-count getter
- D-3584: 0x004c2e10 FUN_004c2e10 — subsystem-info struct populator
- D-3585: 0x004c2ea0 FUN_004c2ea0 — mode-count-per-subsystem getter

## New UNCERTAINTIES

- U-1227: constant `5` at DAT_00773208 in FUN_00498ea0 — settings format version or type tag?
- U-1228: RW vtable slot 0xCC semantics in FUN_00498ea0

## Shared-helper-with-P check

No shared helper overlap with save_gamesave detected. FUN_004c2c90 (D-0229) is shared with rw_engine_init, not save_gamesave. Depth-3 walk here would re-cover rw_engine_init territory (D-0229 bucket).

## Cap usage

8 functions processed. No cap hit.
