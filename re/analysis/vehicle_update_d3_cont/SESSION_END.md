# Session End Report — vehicle_update_d3_cont-20260513

**Date:** 2026-05-13  
**Slot:** Mashed_pool2  
**Session:** vehicle_update_d3 DEFERRED drain + depth-1 fill

## Functions analyzed (20 total)

### DEFERRED drain (D-7961..D-7966)

| RVA | Name | Notes file | D-row |
|-----|------|------------|-------|
| 0x0047d3c0 | VehiclePhysicsWorldCreate | 0047d3c0.md | D-7961 |
| 0x0047ea40 | PhysicsSceneStepWrapper | 0047ea40.md | D-7961 |
| 0x00442ce0 | VehicleRubberBandSpeedModifier | 00442ce0.md | D-7963 |
| 0x004c52f0 | RwMatrixCombine | 004c52f0.md | D-7964 |
| 0x0046d4d0 | VehiclePhysicsMatrixSet | 0046d4d0.md | D-7965 |
| 0x0046c5f0 | — drift | D-7962 (already C1 in physics_collision_d4_breadth) |
| 0x00442c80 | — drift | D-7963 (already C2 in util_c0_promote) |
| 0x0046cb30 | — drift | D-7963 (already C1 in profile_career_d4) |
| 0x004a3384 | — drift | D-7966 (already C1 in profile_career_d4) |
| 0x0046cc40 | — drift | D-7966 (already C1 in physics_collision_d4_breadth) |

### Depth-1 fill (callees of DEFERRED functions)

| RVA | Name | Notes file |
|-----|------|------------|
| 0x0047e9c0 | PhysicsSceneInitSequence | 0047e9c0.md |
| 0x0046dbe0 | VehicleGetField0 | 0046dbe0.md |
| 0x0047d240 | VehiclePhysicsBodyPlace | 0047d240.md |
| 0x0055c810 | PhysicsBodySetLinearVelocity | 0055c810.md |
| 0x0055c4f0 | PhysicsBodySetFriction | 0055c4f0.md |
| 0x0055c4a0 | PhysicsBodySetRestitution | 0055c4a0.md |
| 0x0055bab0 | PhysicsBodySetDamping | 0055bab0.md |
| 0x0055b940 | PhysicsBodySetMass | 0055b940.md |
| 0x0057c500 | RWP37WorldCreate | 0057c500.md |
| 0x0057c300 | RWP37BodyCreate | 0057c300.md |
| 0x004826d0 | PhysicsCollisionBodyCreate | 004826d0.md |
| 0x00482730 | PhysicsBodySetMaterialColor | 00482730.md |
| 0x0057c220 | RWP37WorldSlotAssign | 0057c220.md |
| 0x00559c40 | PhysicsSceneBodyRegister | 00559c40.md |
| 0x0055ae70 | PhysicsSceneBodySetShapeFlags | 0055ae70.md |

## Vehicle physics world init chain

```
VehiclePhysicsWorldStep (0x0047eb30) [C2]
  ├── VehiclePhysicsWorldCreate (0x0047d3c0) [C1 NEW]
  │     ├── RWP37WorldCreate (0x0057c500) [C1 NEW]
  │     │     └── allocates world (0x54b) + body list (0x10 × 0x60b)
  │     ├── PhysicsBodySet* cluster (4 setters) [C1 NEW]
  │     └── [4× vehicle body loop]
  │           ├── RWP37BodyCreate (0x0057c300) [C1 NEW]
  │           ├── PhysicsCollisionBodyCreate (0x004826d0) [C1 NEW]
  │           ├── PhysicsBodySetMaterialColor (0x00482730) [C1 NEW]
  │           ├── RWP37WorldSlotAssign (0x0057c220) [C1 NEW]
  │           ├── VehiclePhysicsBodyPlace (0x0047d240) [C1 NEW]
  │           │     └── places body at Y=-10f (spawn-below-ground init)
  │           ├── PhysicsSceneBodyRegister (0x00559c40) [C1 NEW]
  │           └── PhysicsSceneBodySetShapeFlags (0x0055ae70) [C1 NEW]
  └── PhysicsSceneStepWrapper (0x0047ea40) [C1 NEW]
        └── PhysicsSceneInitSequence (0x0047e9c0) [C1 NEW]
              └── 12 scene setup calls (all stubbed S-3724..S-3729)
```

## Vehicle struct layout (consolidated)

Base: `DAT_008815a8`, stride 0xD04 bytes per vehicle:

| Byte offset | Absolute (vehicle[0]) | Content |
|-------------|----------------------|---------|
| +0x000 | 0x008815a8 | Race position int (0=1st … 3=4th) |
| +0xA08 | 0x00881f50 | Position XYZ (3 floats) |
| +0xA20 | 0x00881ec8 | Matrix A (4×4, 64 bytes) |
| +0xA60 | 0x00881f08 | Matrix B (identical copy of A on write) |

## Rubber-band speed modifier (0x00442ce0)

- Mode gate: `DAT_007f0fd0 ∈ {4,8,9}` → 1.0f (no assist)
- Reads race positions via `VehicleGetField0` (0x0046dbe0)
- 3-tier multiplier based on rank gap (1/2/3 positions behind)
- Constants at `DAT_005cc318 / DAT_005cc32c / DAT_005cc564`
- `DAT_008989c8` = reference vehicle global index

## RwMatrixCombine (0x004c52f0)

- 3 modes: 0=copy, 1=pre-mul, 2=post-mul
- Identity flag: `*(DAT_007d4028 + 4 + DAT_007d3ff8) & 0x20000`
- Matrix multiply via fn ptr at `DAT_007d4028 + 8 + DAT_007d3ff8`

## Physics body field layout (from property setters)

| Offset | Content |
|--------|---------|
| +0x40 | Linear velocity X |
| +0x44 | Linear velocity Y |
| +0x48 | Linear velocity Z |
| +0x4c | Damping (0.01f) |
| +0x50 | Friction (0.45f, recursive compound) |
| +0x54 | Restitution (0.5f, recursive compound) |
| +0x04..+0x0c | Inertia tensor column |
| +0x10..+0x2c | Full inertia matrix (mode=1) |
| +0x30 (at param_1[0xc]) | State/sleep counter |
| +0x48 (at param_1[0x12]) | Flags; bit1 = full-tensor flag |

## New stubs

S-3724..S-3747 (24 stubs):
- S-3724..S-3729: PhysicsSceneInitSequence 12 callees (grouped)
- S-3730..S-3735: VehiclePhysicsBodyPlace callees
- S-3736..S-3741: property setter helpers + pool allocators
- S-3742..S-3744: PhysicsCollisionBodyCreate callees
- S-3745..S-3747: scene registration/shape-flag helpers

## [UNCERTAIN] markers

- `DAT_008989c8` purpose: lies at `DAT_008989b0 + 0x18` (index 6 of 4-vehicle array); may be a separate leader-index global
- `DAT_005cc318 / DAT_005cc32c / DAT_005cc564` exact float values: not memory-read in this session
- Vehicle struct matrix B purpose: dual identical write from same source — may be current/previous transform pair
- `PhysicsSceneBodyRegister` call signature: 1 arg visible at call site but decompiler shows 4 — likely 3 implicit ECX/EDX/__fastcall args

## Tracker mutations applied

- **hooks.csv:** 20 new rows (C1/new)
- **DEFERRED.md:** D-7961..D-7966 RESOLVED (6 rows, 5 drift-cleared)
- **STUBS.md:** S-2625..S-2634 resolved; S-3724..S-3747 added (24 new stubs)
- **SCRIBE_QUEUE.md:** queued row added
