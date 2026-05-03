---
session: powerups_d2
session_id: powerups_d2-20260503
slot: Mashed_pool4
date: 2026-05-03
status: COMPLETE
parent: powerups (session Z)
---

## Summary

Depth-2 pass on powerups subsystem. Analyzed 22 unique callees from D-1300..D-1303 (DepthCharge, GatlingGun, PowerUpIcons, Laser init callees).

## Functions analyzed (22)

| RVA | Name | Notes |
|-----|------|-------|
| 0x004548a0 | FUN_004548a0 | DepthCharge per-entry activator dispatcher |
| 0x004781b0 | FUN_004781b0 | RW file stream load (open+seek+load+close) |
| 0x004b3bf0 | FUN_004b3bf0 | DFF file loader (mode 2) |
| 0x004b3e40 | FUN_004b3e40 | DFF memory loader (mode 3) |
| 0x004b5190 | FUN_004b5190 | RW sub-object property dispatch; U-1447 |
| 0x004b5240 | FUN_004b5240 | Flag bit 2 toggle on object+2 |
| 0x004b5320 | FUN_004b5320 | RW ForAll dispatch (2-dword userdata) |
| 0x004b5580 | FUN_004b5580 | RW ForAll dispatch (1-dword userdata); U-1448 |
| 0x004b6520 | FUN_004b6520 | memset-zero wrapper |
| 0x004b65a0 | thunk_FUN_004b68e0 | Returns DAT_007d3e4c; U-1451 |
| 0x004b65b0 | thunk_FUN_004b68f0 | Asset dir filename lookup (table 0x0090dac0) |
| 0x004c0b30 | FUN_004c0b30 | RW alloc type 0x3000e + init |
| 0x004c1040 | FUN_004c1040 | RW frame reparent (full scene-graph update) |
| 0x004c39b0 | FUN_004c39b0 | 3D vector normalize (RW fast inv-sqrt LUT) |
| 0x004c57a0 | FUN_004c57a0 | RW alloc type 0x3000d + 15-field init; already FontCtxMatrix_AllocInit in hud |
| 0x00474d60 | FUN_00474d60 | Conditional RW ForAll dispatch |
| 0x00476c10 | FUN_00476c10 | Effect 4-float vector setter (+0xd8..+0xe4) |
| 0x00476cb0 | FUN_00476cb0 | Effect object fields +0xa4/+0xa8 setter |
| 0x004770c0 | FUN_004770c0 | Particle/effect system init (800 bytes) |
| 0x004e69a0 | FUN_004e69a0 | RW sub-object clone |
| 0x004e6ab0 | FUN_004e6ab0 | RW hierarchy instantiate (591 bytes) |
| 0x004e7e30 | FUN_004e7e30 | Set material/texture ref on RW object |

## Tracker changes

- **Cleared**: D-1300, D-1301, D-1302, D-1303 (all drained)
- **New stubs**: S-1440..S-1453 (14 entries)
- **New uncertainties**: U-1447..U-1452 (6 entries)
- **New depth-3 deferrals**: D-4240..D-4243

## Depth-3 deferrals (D=4240..4299)

- D-4240: FUN_004547c0 — DepthCharge struct-A per-entry activator
- D-4241: FUN_00454170 — DepthCharge struct-B per-entry activator
- D-4242: FUN_00534b60 — particle/effect system allocate
- D-4243: 14 RW hierarchy internals from FUN_004e6ab0

## Key findings

1. **RW effect system**: FUN_004770c0 is the master effect/particle setup; translates a 13-bit flags field to RW particle flags and allocates per-channel arrays via FUN_00474db0. Called with flags 0x807/0x817 and counts 0x19/0x32.

2. **Stream API pattern**: File DFF loads use `FUN_004cc230(2,1,filename)` → `FUN_004cc5e0(stream,0x10,0,0)` → `FUN_004e7420(stream)` → `FUN_004cc160(stream,0)`. Memory DFF loads use mode 3 with stack buffer.

3. **Asset directory table**: thunk_FUN_004b68f0 looks up filenames in a pre-loaded table at 0x0090dac0 (count at 0x008ad9a8, stride 0x80, size field at +0x78). Used by icon loader to get DFF size before loading.

4. **0x004c57a0 already named**: Identified as `FontCtxMatrix_AllocInit` in the hud/font_text session. Same function used in two subsystems (identity-matrix object allocation, type 0x3000d).

5. **RW frame reparent**: FUN_004c1040 implements `RwFrameAddChild` — full singly-linked-list management with dirty-flag propagation through the scene graph.
