# G3 — real-physics + real-AI 4-car race (2026-06-18)

Goal: the 3 opponents follow their `AI%d.AI` race line under `MASHED_REAL_AI` and drive
through the SAME physics chain as the player (fix the ctrl→motion approximations). Done
when a 4-car real-physics+real-AI race runs.

## Starting state (WS-AI-BRIDGE, 2026-06-17)
The bridge already loaded `AI<course>.AI` from `Common/AI.piz` into the controller image
(`Ai_BridgeLoad`), bound the `Ai::Host`, and ran `Ai_Standalone_Tick` to produce per-vehicle
ctrl blocks — BUT the opponent ctrl→motion was a crude KINEMATIC approximation:
`a.yaw += steer*2.4*dt` + a speed lerp, not the physics chain.

## Two fixes
1. **ctrl→motion = real physics.** Generalized `VehiclePhysics_StepPlayer` →
   `VehiclePhysics_StepCar(slot, dt, io)` (slot 0 = player, 1..3 = AI). The opponent adapter
   in `TrackRenderer::UpdateCar` now feeds each AI's descriptor ctrl bytes (ctrl[0]/[1]=steer,
   ctrl[4]=accel, ctrl[5]=brake — already the chain's `input[]` format, no lossy remap) into
   `StepCar` and integrates pos with the same `kWorldVel` + GroundHeight as the player. Added
   `AiCar.vel[3]` to persist each opponent's world velocity. (Kinematic path kept as the
   physics-off fallback.)
2. **Throttle-gate port bug.** `ControlStep`'s final game-mode gate (keeps accel for race-class
   sub-modes {6,5,9,10,11}) was calling `s_host.game_mode_fd0()` (= `DAT_007f0fd0` = 0) instead
   of the SUB-mode `FUN_0040e350` (= `game_sub_mode`, race=6) the decomp comment cites. With
   gameMode=0 the gate zeroed ctrl[4]/ctrl[5] every frame → AI cars never accelerated. Fixed to
   call `game_sub_mode()`.

## Verified (MASHED_REAL_PHYSICS=1 MASHED_REAL_AI=1, AI-DIAG)
- `splineCnt=31` — the `.AI` race line loaded (controller non-inert).
- Before fix 2: `ctrl[0,255,0,0]` accel=0 → `spd=0` (stuck). After: `ctrl[*,*,255,*]` accel=255,
  `spd` ramps 25→38→45 (the same physics speed cap as the player), and all 3 opponents'
  positions advance steadily along the race line (`z:16→18→22→28→33`, `x` curving) with no
  respawns — they drive a proper on-track racing line via the verbatim chain.

=> A 4-car real-physics + real-AI race runs. G3 Done-when satisfied.

## Residual (not blockers; future polish)
- All 3 opponents are assigned line type 0 / index 0 (race line) so they cluster on the same
  line. The bank-switch timer/RNG (FUN_00417180) + the targeting behaviour tree (modes 1..10,
  FUN_00414570.. helpers) are still STUBBED (mode 0 race-follow) — porting them spreads cars
  across inside/slow/cheat lines + adds overtaking/blocking. See Ai/AiStandalone.cpp ledger.
- Opponent record world-position isn't fed back to the chain, so A5 drafting/proximity grip
  (Phase 2) reads stale cross-car positions — a minor grip effect, not a driving blocker.
- Steering at standstill (speed 0) is degenerate (heading from velocity) — cars correct once
  moving; the full FUN_00415e20 heading uses the body matrix in the original.
