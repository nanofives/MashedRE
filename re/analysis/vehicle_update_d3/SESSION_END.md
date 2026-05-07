# Session End Report — vehicle_update_d3-20260506

**Date:** 2026-05-06  
**Slot:** Mashed_pool14  
**Session:** G6 (vehicle_update depth-3)

## Context

Depth-3 session draining D-4120..D-4128 from vehicle_update_d2 (session UUU / 2026-05-03). Those entries were listed in `re/analysis/vehicle_update_d2/SESSION_END.md` as "tracker mutations required" but had not been applied to DEFERRED.md or hooks.csv at session time. This session analyzed all 9 target RVAs from the decompiler outputs of Mashed_pool14.

## Functions analyzed (U-2687..U-2694)

| U-ID | RVA | Name | Notes file |
|------|-----|------|------------|
| U-2687 | 0x0046ddb0 | VehicleWheelForceIntegrator | 0046ddb0.md |
| U-2688 | 0x00467650 | VehicleWheelDrivetrainUpdate | 00467650.md |
| U-2689 | 0x00468980 | VehicleAeroStabilizer | 00468980.md |
| U-2690 | 0x004809e0 | VehicleRespawnTeleport | 004809e0.md |
| U-2691 | 0x0046f6c0 | VehicleWheelContactSolver | 0046f6c0.md (clears S-2620) |
| U-2692 | 0x0046dc20 | VehicleOOBDebugDraw | 0046dc20.md |
| U-2693 | 0x00467350 | VehicleSlipTimerTick | 00467350.md |
| U-2694 | 0x0047eb30 | VehiclePhysicsWorldStep | 0047eb30.md |
| (existing) | 0x0040fc00 | Race::Tick | Already C1/new; confirmed: pause toggle via keys 5/6, calls PhysicsTickDispatcher (FUN_00425A40) only when `DAT_005F29C0 != 0` |

## Vehicle physics call chain (depth-3 expanded)

```
VehicleUpdateDispatcher (0x00470C70)
  ├── FUN_00467350  VehicleSlipTimerTick          [U-2693]  ← was D-4126
  ├── FUN_0047EB30  VehiclePhysicsWorldStep        [U-2694]  ← was D-4127
  │     └── FUN_0047D3C0  RWP37 physics step      [S-2626; D-7961]
  ├── FUN_00470670  VehicleControlUpdate           [U-1408]
  │     ├── FUN_0046DDB0  VehicleWheelForceIntegrator    [U-2687]  ← was D-4120
  │     ├── FUN_00467650  VehicleWheelDrivetrainUpdate   [U-2688]  ← was D-4121
  │     └── FUN_00468980  VehicleAeroStabilizer          [U-2689]  ← was D-4122
  ├── FUN_004709A0  VehicleCollisionBroadPhase     [U-1409]
  │     └── FUN_0046F6C0  VehicleWheelContactSolver [U-2691] ← was D-4124 / S-2620
  ├── FUN_0046DA80  VehicleTrackInteraction        [U-1413]
  ├── FUN_0046DC20  VehicleOOBDebugDraw            [U-2692]  ← was D-4125
  ├── FUN_00480B70  VehicleWheelParticles          [U-1414]
  └── FUN_00480720  VehicleSpeedCorrection         [U-1410]
```

`FUN_004809E0` (VehicleRespawnTeleport, U-2690) is called from VehicleUpdateDispatcher when `piVar14[0x272] == 0x40800000 && piVar14[0x276] == 0` per d2 session notes (D-4123).

## New struct layout findings

### VehicleControlUpdate callee mappings
- `+0x278` = contact mode float; 0x40800000 (4.0f) = all 4 wheels grounded
- `+0x279` = throttle float
- `+0x2B4` = position ring buffer index (0 or 1)
- `+0x2B5..0x2BA` = 2-slot position history buffer
- `+0x83, 0xB4, 0xE5, 0x116` = steer values, wheels 0..3
- `+0x2C0..0x2C2` = random impulse per wheel
- `+0x2C8` = airborne flag
- `+0x5C` = angular inertia scale (gear inertia)
- `+0x54..0x56` = gear torque constants

### WheelContactSolver findings
- `+0x9AC` = wheel ring index (0 or 1); used to index `0x9AC * 0x40 + 0x928 + vehicleBase`
- `+0x9E0` = grounded wheel count (output of contact solver)
- `+0x9F0` = no-contact-pending flag
- `+0x144..0x14C` = angular impulse accumulator XYZ

### SlipTimerTick / CooldownTimers
- `+0x38..+0x4C` = 6 float countdown timers (for player vehicles 0..3 only)
- `+0x20..+0x34` = 6 paired integer charge counters
- Only processes 4 player vehicles (loop stride 0xD04, 4 iterations)

### PhysicsWorldStep (RWP37 binding)
- `DAT_006CE274` = RWP37 world ptr (allocated at physics init)
- `DAT_0086CAA0` = physics sub-step counter (rolls over at 123 = 0x7B)
- `DAT_0061331C` = last dt value, capped to 50ms

### Race::Tick (0040fc00) pause mechanism
- `DAT_005F29C0` = physics-run flag (non-zero = run physics)
- Key 6 (`FUN_00496930(6)`) → sets to 1; key 5 → clears to 0
- Default initialization presumably 1 (physics runs by default)

## New stubs

| S-ID | RVA | Description |
|------|-----|-------------|
| S-2625 | 0x0046C5F0 | Wheel state sync helper; purpose [UNCERTAIN] |
| S-2626 | 0x0047D3C0 | RWP37 physics world step (D-7961) |
| S-2627 | 0x0047EA40 | Physics step completion flag (D-7961) |
| S-2628 | 0x0046CB30 | Vehicle body position getter (D-7963) |
| S-2629 | 0x004C52F0 | RW matrix rotation setter (D-7964) |
| S-2630 | 0x0046D4D0 | Apply matrix to vehicle rigid body (D-7965) |
| S-2631 | 0x00442CE0 | Vehicle speed modifier callback (D-7963) |
| S-2632 | 0x00442C80 | Vehicle crowding bool (D-7963) |
| S-2633 | 0x004A3384 | FP acos/atan approximation (D-7966) |
| S-2634 | 0x0046CC40 | Wheel contact sub-helper (D-7966) |

## DEFERRED rows added (D-7960..D-7966)

| D-ID | Content |
|------|---------|
| D-7960 | FUN_00429310 — Race::Tick depth-2 callee missing from prior D-7734..D-7755 batch |
| D-7961 | FUN_0047D3C0 + FUN_0047EA40 — RWP37 physics world step + completion |
| D-7962 | FUN_0046C5F0 — wheel state sync |
| D-7963 | FUN_00442CE0 + FUN_00442C80 + FUN_0046CB30 — force modifier trio |
| D-7964 | FUN_004C52F0 — RW matrix rotation setter |
| D-7965 | FUN_0046D4D0 — apply matrix to vehicle body |
| D-7966 | FUN_004A3384 + FUN_0046CC40 — FP trig + wheel sub-helper |

## Tracker mutations applied

- **hooks.csv:** Added 8 rows (U-2687..U-2694), all `vehicle / C1 / new`
- **DEFERRED.md:** D-4120..D-4128 were never in DEFERRED.md (d2 session mutations not applied); added D-7960..D-7966
- **STUBS.md:** Added S-2625..S-2634

## [UNCERTAIN] markers

- `FUN_0046C5F0` — purpose not confirmed; name "wheel state sync" inferred from call sites
- `FUN_00442CE0` — "proximity speed modifier" inferred from taking (idx, float, float); not confirmed
- Vehicle type constants in VehicleWheelDrivetrainUpdate (-0x5F7F80 etc.) — enum not fully documented
- `FUN_0040E340` vs `FUN_0040E350` — two distinct mode queries; exact distinction unknown
