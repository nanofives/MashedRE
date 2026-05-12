---
session: ai_powerup_decisions-20260512
date: 2026-05-12
pool_slot: Mashed_pool4
analyst: Claude Sonnet 4.6
plates_written: 13
confidence: C1
subsystem: powerups
---

## Mission

Map the AI side of powerup-use decisions. Find the code that decides WHEN and WHICH
powerups AI drivers use. Hard cap: 20 RVAs.

## Key finding

**AI drivers fire powerups through the same button-state buffer as human players.**
There is no separate AI-only fire dispatch inside the powerup subsystem.

AI must write to `(&DAT_007f103c)[ctrl_idx * 0x4c + 3]` (fire button) and
`(&DAT_007f14ff)[ctrl_idx * 0x4c]` (secondary fire flag) to trigger `FUN_0045bba0`'s
fire path (`(**(code **)(slot+0xa8 + 8))()`).

The decision layer — the code that evaluates race state and *chooses* to fire — is
upstream in the AI driver / controller update subsystem. The powerup subsystem
itself is policy-blind.

## Functions analyzed (13 plates)

| RVA | Name | Size (bytes) | Notes |
|-----|------|--------------|-------|
| 0x0045bba0 | PowerupPerFrameDispatch | 32545 | Central per-frame dispatcher; 4 sections |
| 0x0045bfa0 | PowerupSlotActivate | 34 | Looks up type entry, arms countdown |
| 0x0045baa0 | PowerupTypeLookup | 31 | ESI=type code → entry pointer |
| 0x0045bac0 | PowerupSlotDeactivate | 27 | Calls vtable+0x10, zeroes slot+0xa8/+0xac |
| 0x0045be90 | PowerupPerFrameUpdate | — | Table +0x28 dispatch + per-slot FUN_0045ba10 |
| 0x0045bf30 | PowerupTeardownAll | — | Per-slot deactivate + table +0x30 dispatch |
| 0x0045bed0 | PowerupRoundCleanup | — | Per-slot deactivate+reset + table +0x2c dispatch |
| 0x0045b930 | PowerupSubsystemShutdown | — | 5 shutdowns + table +0x18 dispatch |
| 0x0045b990 | PowerupUnknownDispatch | — | FUN_0045a130 + FUN_00458b10 + table +0x20 |
| 0x0045b9d0 | PowerupPerFrameRender | — | Table +0x24 dispatch |
| 0x004540c0 | DepthChargePerFrameTeardown | — | Two loops; 4 object fn calls each |
| 0x0044d5e0 | DepthChargePathSafetyCheck | — | Obstacle/intersection check; returns 0/1 |
| 0x0045c010 | PowerupPickupWrapper | — | Thin wrapper → PowerupSlotActivate |

## Type table entry layout (confirmed, 0x005f9998 TRUE base, stride 0x40)

| Offset | Role |
|--------|------|
| +0x00 | type code (int) |
| +0x04 | ARM — init countdown |
| +0x08 | FIRE |
| +0x0c | CAN_FIRE query |
| +0x10 | DEACTIVATE |
| +0x14 | init (startup) |
| +0x18 | shutdown |
| +0x1c | per-type teardown (Section 2) |
| +0x20 | unknown |
| +0x24 | per-frame render |
| +0x28 | per-frame update |
| +0x2c | round cleanup |
| +0x30 | teardown all |
| +0x34 | unknown (no xref found) |
| +0x38 | mode-0x21 (network) fn |
| +0x3c | mode-2 (single-player) fn |

Known type codes: 9 = GatlingGun (entry 0x005f9998), DepthCharge (entry 0x005f99d8,
type code [UNCERTAIN]).

## Open uncertainties filed this session

- Type codes for entries 1..4 beyond GatlingGun (`0x0045baa0.md`)
- Purpose of FUN_0045b990 and type table +0x20 slot (`0x0045b990.md`)
- LAB_00457af0: code label inside unanalyzed function; DepthCharge mode-0x21 fn
- DepthCharge object array bases/sizes (`0x004540c0.md`)
- Obstacle array entry count at 0x006848e8 (`0x0044d5e0.md`)
- 5 pre-table calls in FUN_0045b930 (`0x0045b930.md`)

## New stubs

FUN_004b4b60, FUN_00476880, FUN_00484cf0, FUN_0045ba10, FUN_0045a130, FUN_00458b10,
FUN_004e6e00, FUN_004c1210, FUN_004c0c20, FUN_004e6920, FUN_004c18c0, FUN_00547230

## Where to look next (AI decision layer)

The actual AI powerup-use decision is upstream. Trace callers of
`(&DAT_007f103c)` writes (fire button byte) that originate outside player input
processing. Likely in the AI driver update path (FUN_004111c0 callees that update
per-AI-player controller state).
