# physics_collision_d4_breadth — Session Report

**Date:** 2026-05-12  
**Slot:** Mashed_pool0  
**Session:** physics_collision_d4_breadth-20260512

## Mission result: 2 new C1 plates + STOP-AND-ASK flag

### New plates

| RVA | Name | Size | Subsystem |
|-----|------|------|-----------|
| 0x0046c5f0 | TriangleFaceNormal | 209b | vehicle |
| 0x0046cc40 | WheelTerrainContactClassifier | 1686b | vehicle |

Both are callees of `VehicleWheelContactSolver` (0x0046f6c0, C2) that were missing from
hooks.csv. They complete the wheel-terrain contact classification → response pipeline.

### Full physics call chain (confirmed)

```
FUN_004111c0  (race-phase state machine, C1)
  └─ FUN_00425a40  (pause/online-gate update, C1)
       └─ FUN_00470c70  (VEHICLE_UPDATE_FN 16-vehicle, C2)
            ├─ FUN_00467350  (VehicleSlipTimerTick, C1) — pre-loop
            ├─ FUN_0047eb30  (VehiclePhysicsWorldStep — RWP37 step, C1) — pre-loop
            ├─ [per-vehicle loop × 16]
            │    └─ FUN_00470670  (VehicleControlUpdate, C2)
            │         ├─ FUN_0046ddb0  (VehicleWheelForceIntegrator, C2)
            │         ├─ FUN_00467650  (VehicleWheelDrivetrainUpdate, C1)
            │         └─ FUN_00468980  (VehicleAeroStabilizer, C1)
            └─ [per-substep loop ÷25 ticks per call]
                 └─ FUN_004709a0  (VehicleCollisionBroadPhase, C2)
                      ├─ FUN_0046e9e0  (per-substep rigid body integration, C2)
                      ├─ FUN_0046ef70  (wheel contact spring/damper resolver, C2)
                      ├─ FUN_00469aa0  (vehicle contact history update, C1)
                      │    ├─ FUN_00468d80  (VehicleTerrainContactSolver, C1)
                      │    └─ FUN_004694e0  (VehicleObjectContactSolver, C1)
                      ├─ FUN_00469df0  (vehicle-vehicle convex-hull impulse, C1)
                      └─ FUN_0046f6c0  (VehicleWheelContactSolver, C2)
                           ├─ FUN_0046cc40  (WheelTerrainContactClassifier, C1) ← NEW
                           └─ FUN_0046c5f0  (TriangleFaceNormal, C1) ← NEW
```

All runtime physics functions are confirmed **C1 or C2**. The physics surface is fully mapped
at depth-1 from `VehicleCollisionBroadPhase`.

---

## ⚠ STOP-AND-ASK: Physics subsystem taxonomy

**Observation:** Mashed has **no separate runtime physics layer**. The impulse integrators,
contact classifiers, spring/damper resolvers, and convex-hull SAT solvers are all embedded
in the vehicle update chain and tagged under subsystem=`vehicle` in hooks.csv.

The four rows currently tagged subsystem=`physics` cover only **static track setup**:
- 0x0047ce40 — physics body slot lookup by polygon ID
- 0x0047b9b0 — Lua executor wrapper (COURSE.LUA)
- 0x00478cb0 — BSP struct zeroing
- 0x004715a0 — physics scenario BSP linker

The "physics" subsystem in hooks.csv is therefore:  
`physics ≡ track collision geometry setup (static, load-time)`  
`vehicle ≡ runtime physics simulation (dynamic, per-tick)`

The new plates from this session (`0046c5f0`, `0046cc40`) are tagged `vehicle` for consistency
with existing sibling rows. **No rows have been retagged.**

**Question for user:** Should the runtime physics functions be split into a dedicated
`physics_runtime` subsystem? Or should `vehicle` remain the home for all runtime physics?
This affects future subsystem DOD accounting and the ROADMAP Phase 5 scope.

---

## Dispatch hub (FUN_0040ce00) unmapped callees

The simulation dispatch hub has 22 callees. 16 are unanalyzed. Based on address ranges and
the known callees (particle FX, skidmarks, animation, active-object dispatch), the unanalyzed
16 are **not vehicle physics** — they appear to be game-simulation concerns (AI sync, UI,
score, lap timing, replay). They belong to future game_state/leaderboard/AI sweeps, not the
physics surface.

Stubs or DEFERRED rows for these were NOT filed here — they predate this session and are
tracked under `D-7723` (dispatch hub deferred rows from leaderboard_d3).

---

## Conclusions

1. The physics surface is **fully mapped** at C1+ through the complete vehicle update chain.
2. Two new C1 plates filed (WheelTerrainContactClassifier + TriangleFaceNormal).
3. No separate physics integrator / constraint solver exists outside RW Physics 3.7 middleware.
4. The "physics state struct" is the **vehicle struct itself** (stride 0xD04, contains velocity
   at +0x9B0, angular velocity at +0x144, wheel contact flags at +0x198/+0x25C/+0x320/+0x3E4).
   No separate physics_state.md is warranted — it would duplicate the vehicle struct layout.
5. Taxonomy question filed (see STOP-AND-ASK above). No action taken pending user decision.
