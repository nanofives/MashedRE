# SESSION_END — vehicle_dynamics d1 expanded

**Session ID:** vehicle_dynamics-20260506-expand  
**Pool slot:** Mashed_pool11  
**Date:** 2026-05-06  
**Anchor verified:** MASHED.exe SHA256 bdcae093... ✓ size 2,846,720 ✓

## Trigger

vehicle_dynamics_d2 halted at pre-flight: only 2 DEFERRED rows (D-5140, D-5141). Rule requires ≥5.
This session expands d1 to generate the required rows.

## Functions analyzed this session (all C1 new)

| RVA | Name | Bytes | Notes |
|-----|------|-------|-------|
| 0x004709a0 | FUN_004709a0 | 704 | VehicleCollisionBroadPhase outer loop; 16 vehicles; 2 substeps |
| 0x0046ef70 | FUN_0046ef70 | 1871 | Wheel contact spring/damper resolver; resolves D-5141 |
| 0x004c3b90 | FUN_004c3b90 | 86 | Fast reciprocal-sqrt via LUT; resolves D-5140 |
| 0x00467300 | FUN_00467300 | 73 | Collision win event trigger |

## Functions already tracked (skipped)

- 0x0040e340 — FUN_0040e340 (util/vehicle C1 mapped)
- 0x004a2c48 — FUN_004a2c48 (frontend/render C1 mapped)
- 0x004c3ac0 — FUN_004c3ac0 (ai C1 mapped)

## DEFERRED resolved

- D-5140: 0x004c3b90 → C1 new (fast reciprocal-sqrt)
- D-5141: 0x0046ef70 → C1 new (wheel contact spring/damper)

## DEFERRED generated (d2 seeds)

| ID | RVA | Size | Bucket |
|----|-----|------|--------|
| D-7780 | 0x0046f6c0 | 3580b | vehicle_dynamics_d2-cont1 |
| D-7781 | 0x00469aa0 | 847b | vehicle_dynamics_d2-cont1 |
| D-7782 | 0x00469df0 | 5062b | vehicle_dynamics_d2-cont1 |
| D-7783 | 0x004c39b0 | 270b | vehicle_dynamics_d2-cont1 |
| D-7784 | 0x00413c70 | 58b | vehicle_dynamics_d2-cont1 |

## New STUBS: S-2620..S-2624  
## New UNCERTAINTIES: U-2627..U-2632

## Key structural findings

- Vehicle slot stride: **0xD04 bytes** (0x341 dwords) in DAT_00881000 region
- Contact record block: `DAT_008815A0 + v*0xD04 + 0x24A`
- Contact record stride: **64 bytes** (0x40), 24 records per vehicle
- Position (in contact block): offset +0x256 from slot base
- Angular velocity: `vehicle_struct + 0x144..0x14C`
- Linear velocity: `vehicle_struct + 0x9B0..0x9B8`
- Forward direction: `vehicle_struct + 0x9C8..0x9D0`
- Contact double-buffer: `DAT_00881F48` (write) / `DAT_00881F4C` (read) flip-flop `& 1`
- Collision win countdown: DAT_007f0fdc = 6000 (unit unknown U-2628)
- Fast math LUT shared by FUN_004c3ac0/004c3b90: at DAT_007d3ff8/007d3ffc

## d2 status

Active vehicle_dynamics DEFERRED rows after this session: **5** (D-7780..D-7784). Threshold ≥5 met.
vehicle_dynamics_d2 may proceed.
