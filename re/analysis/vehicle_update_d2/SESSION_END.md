# Session End Report — vehicle_update_d2-20260503-1744

**Date:** 2026-05-03  
**Slot:** Mashed_pool2  
**Session:** UUU (vehicle_update depth-2, re-anchored from halted session V)

## Context

Session V (vehicle_update depth-1) did not produce DEFERRED rows or analysis notes —
no `re/analysis/vehicle_update*/` folder existed and no vehicle-subsystem entries were
in hooks.csv or DEFERRED.md. Session UUU re-anchored from scratch using:

1. String search for "toastart/vehicles/" → `FUN_00420230` → vehicle loader path
2. String search for speed debug strings → `FUN_00480720` → speed-correction function
3. Caller walk: `FUN_00480720` → 6 callers → identified `FUN_00470c70` (dispatcher)
4. Caller walk: `FUN_00470c70` → `FUN_00425a40` → `FUN_0040fc00` (game loop)

## Functions analyzed (U-1407..U-1415) — all C1

| U-ID  | RVA        | Name                     | Notes file |
|-------|------------|--------------------------|------------|
| U-1407 | 0x00470c70 | VehicleUpdateDispatcher  | 00470c70.md |
| U-1408 | 0x00470670 | VehicleControlUpdate     | 00470670.md |
| U-1409 | 0x004709a0 | VehicleCollisionBroadPhase | 004709a0.md |
| U-1410 | 0x00480720 | VehicleSpeedCorrection   | 00480720.md |
| U-1411 | 0x00425a40 | PhysicsTickDispatcher    | 00425a40.md |
| U-1412 | 0x00422ba0 | CollisionEventProcessor  | 00422ba0.md |
| U-1413 | 0x0046da80 | VehicleTrackInteraction  | 0046da80.md |
| U-1414 | 0x00480b70 | VehicleWheelParticles    | 00480b70.md |
| U-1415 | 0x00420230 | VehiclePathBuilder       | 00420230.md |

## Vehicle physics call chain (established)

```
FUN_0040fc00  (0x0040fc00) — game loop [D-4128]
  └── FUN_00425a40  (0x00425a40) — PhysicsTickDispatcher  [U-1411]
        ├── FUN_00423b00  (0x00423b00) — pre-tick setup [not in scope]
        ├── FUN_00418860  (0x00418860) — input update [not in scope]
        ├── FUN_00424eb0  (0x00424eb0) — AI update [not in scope]
        ├── FUN_00470c70  (0x00470c70) — VehicleUpdateDispatcher [U-1407]
        │     ├── FUN_00467350  (0x00467350) — [D-4126]
        │     ├── FUN_0047eb30  (0x0047eb30) — [D-4127]
        │     ├── FUN_00470670  (0x00470670) — VehicleControlUpdate [U-1408]
        │     │     ├── FUN_0046ddb0  (0x0046ddb0) — [D-4120]
        │     │     ├── FUN_00467650  (0x00467650) — [D-4121]
        │     │     └── FUN_00468980  (0x00468980) — [D-4122]
        │     ├── FUN_004709a0  (0x004709a0) — VehicleCollisionBroadPhase [U-1409]
        │     │     └── FUN_00469df0  (0x00469df0) — vehicle-vehicle response [not filed]
        │     ├── FUN_0046da80  (0x0046da80) — VehicleTrackInteraction [U-1413]
        │     ├── FUN_0046f6c0  (0x0046f6c0) — [D-4124]
        │     ├── FUN_00480b70  (0x00480b70) — VehicleWheelParticles [U-1414]
        │     ├── FUN_00480720  (0x00480720) — VehicleSpeedCorrection [U-1410]
        │     ├── FUN_004809e0  (0x004809e0) — [D-4123]
        │     └── FUN_0046dc20  (0x0046dc20) — [D-4125]
        ├── FUN_00422ba0  (0x00422ba0) — CollisionEventProcessor [U-1412]
        └── FUN_004252c0  (0x004252c0) — post-collision [not in scope]
```

## Vehicle data layout (confirmed)

- **Vehicle slot stride:** 0x341 DWORDs = 3332 bytes
- **Max vehicles:** 16 (0x10)
- **Vehicle array base:** `DAT_008815a0` (multiple sub-arrays at fixed offsets)
- **Key per-vehicle offsets:**
  - `+0x4` from array base: active flag
  - `+0x190`: max speed constant (float)
  - `+0x9a8`: wheel-set ring index (0 or 1)
  - `+0x9b0/b4/b8`: linear velocity XYZ
  - `+0x9d4/d8/dc`: forward direction XYZ
  - `+0x9e4`: speed magnitude
  - `+0x9f0`: contact mode (0=grounded, 2=airborne)
  - `+0xbf0`: boost active flag
- **Respawn flag:** `(&DAT_008815b4)[idx * 0x341]`
- **Respawn timer:** `(&DAT_008815b8)[idx * 0x341]` — set to 600 on speed-correction trigger

## DEFERRED rows to file (D-4120..D-4128)

| D-ID  | RVA        | Name           | Subsystem | Pickup condition | Bucket |
|-------|------------|----------------|-----------|-----------------|--------|
| D-4120 | 0046ddb0 | FUN_0046ddb0 | vehicle | Decompile; classify torque/steer application callee of VehicleControlUpdate | vehicle_update_d2-cont1 |
| D-4121 | 00467650 | FUN_00467650 | vehicle | Decompile; classify steering/lateral force callee of VehicleControlUpdate | vehicle_update_d2-cont1 |
| D-4122 | 00468980 | FUN_00468980 | vehicle | Decompile; classify aero/drag step callee of VehicleControlUpdate | vehicle_update_d2-cont1 |
| D-4123 | 004809e0 | FUN_004809e0 | vehicle | Decompile; classify final-frame per-vehicle step (fires when piVar14[0x272]==0x40800000 && piVar14[0x276]==0) | vehicle_update_d2-cont1 |
| D-4124 | 0046f6c0 | FUN_0046f6c0 | vehicle | Decompile; classify checkpoint/track-query flush (called when VehicleTrackInteraction returns _DAT_005cc35c) | vehicle_update_d2-cont1 |
| D-4125 | 0046dc20 | FUN_0046dc20 | vehicle | Decompile; classify out-of-bounds/speed-kill step called from VehicleUpdateDispatcher | vehicle_update_d2-cont1 |
| D-4126 | 00467350 | FUN_00467350 | vehicle | Decompile; classify once-per-tick pre-vehicle-loop function in VehicleUpdateDispatcher | vehicle_update_d2-cont1 |
| D-4127 | 0047eb30 | FUN_0047eb30 | vehicle | Decompile; classify once-per-tick function called after FUN_00467350 in VehicleUpdateDispatcher | vehicle_update_d2-cont1 |
| D-4128 | 0040fc00 | FUN_0040fc00 | vehicle | Full decompile; classify game loop (single caller of PhysicsTickDispatcher) | vehicle_update_d2-cont1 |

## Tracker mutations required

- **hooks.csv:** Add 9 rows (U-1407..U-1415), all `vehicle / C1 / mapped`
- **DEFERRED.md:** Add D-4120..D-4128 (9 entries) to Active
- **STUBS.md:** No new stubs
- **UNCERTAINTIES.md:** 5 [UNCERTAIN] markers:
  - `FUN_0046ddb0` — purpose not confirmed (torque/steer [UNCERTAIN])
  - `FUN_00467650` — purpose not confirmed (steering/lateral [UNCERTAIN])
  - `FUN_00468980` — purpose not confirmed (aero/drag [UNCERTAIN])
  - `FUN_0046f6c0` — purpose not confirmed (checkpoint flush [UNCERTAIN])
  - `FUN_00420230` callers — not checked in this session

## Notes

- Session V had halted with no output; re-anchor strategy succeeded on first pass via speed-correction debug strings.
- `FUN_00470c70` is the vehicle update dispatcher — confirmed by 16-vehicle iteration, stride 0x341, respawn counter logic, speed-correction call.
- `FUN_00470670` uses `in_EAX` as object pointer (thiscall-style), not a named param — decompiler artifact.
- Vehicle stride confirmed as 0x341 DWORDs across 4 independent array bases.
- String `"SPEED ADJUSTMENT OCCURRED!\n"` (0x005cf1d4) is sole evidence for `FUN_00480720` naming.
- `FUN_00420230` callers not checked — deferred to depth-3 via [UNCERTAIN] note in its file.
