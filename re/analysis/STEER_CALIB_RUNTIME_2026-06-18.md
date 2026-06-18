# Steer-calibration runtime finding (wave-15a, 2026-06-18)

First inline runtime drive of the standalone under real physics. Branch `ws-steer-calib` (off main).

## Run
`mashed_re.exe` with `MASHED_REAL_PHYSICS=1 MASHED_PLAY_DEMO=1 MASHED_RACE_DEMO=1 MASHED_GOTO=6
MASHED_TRACK_SEL=0 MASHED_CAR_SEL=0`, ~6.6s in-race window, PID-scoped (spawned PID killed by id;
no blanket-kill). Standalone **self-exited code 0 — NO CRASH** under real-physics + steer. ✅

## PLAY-DEMO yaw log (steer di.steer=1 = full RIGHT, real physics)
```
t=1.1 car_yaw=1.550 pos=(-25.2,15.8) speed=0.00
t=2.2 car_yaw=1.550 pos=(-25.2,15.8) speed=0.00
t=3.3 car_yaw=1.550 pos=(-24.7,40.5) speed=260.99
t=4.4 car_yaw=1.550 pos=(-24.3,59.3) speed=0.00
t=5.4 car_yaw=1.550 pos=(-24.3,59.3) speed=56.27
t=6.5 car_yaw=1.550 pos=(-24.3,59.3) speed=0.00
```
- **car_yaw FLAT at 1.550** — steering not observable (no yaw response to steer=1).
- **speed ERRATIC** (0/0/260/0/56/0) — not a smooth ramp; pos barely advances then sticks.

## Diagnosis (hypotheses, NOT yet root-caused)
1. **RACE_DEMO + PLAY_DEMO conflation:** RACE_DEMO (`RunRaceDemoStep`) drives capture phases at
   el=1800/4200/6600ms and Escs out at 6.6s — its captures / state-poking likely perturb the
   per-frame drive, producing the speed spikes/zeros. There is NO clean "enter InRace + hold +
   scripted-drive" mode; PLAY_DEMO alone doesn't enter the race.
2. **Possible real-physics drive instability:** the speed pattern (spike 260 then 0) is unlike the
   crash-fix agent's reported smooth "0→112" (which used a different probe path, possibly pre the
   d753e701 const back-port). Whether the const back-port or the substep/contact feed destabilized
   the drive is UNCONFIRMED.
3. **car_yaw source:** A8-STEER set `car_yaw_ = io.yaw` (physics-integrated +0x9c0) under physics;
   with speed ~0 most frames, there's no lateral force → no yaw, so flat yaw may be a *consequence*
   of the unstable/zero speed, not a steer-wiring bug per se.

## What's needed (wave-16) before kYawScale calibration is meaningful
A **clean sustained-drive harness**: enter InRace and HOLD (no RACE_DEMO captures/Esc), with
PLAY_DEMO scripted accel+steer, logging yaw/speed for ~20s. Then: (a) confirm a STABLE speed ramp
(if erratic, root-cause the drive instability first — substep loop / contact feed / const back-port);
(b) observe steering response to di.steer; (c) verify/flip the steer sign; (d) calibrate kYawScale
(VehiclePhysicsRun.cpp:241, currently 1.0). All PID-scoped (CLAUDE.md MASHED hygiene).

## UPDATE 2026-06-18 — PRECISE root-cause (sustained-drive harness + steer-chain diag)
Built MASHED_DRIVE_HOLD (exe_main 4159bc1e) + a steer-chain diag (VehiclePhysicsRun
MASHED_PHYS_DIAG block). A 30s real-physics run pinned it:
- **Steer WIRING is CORRECT.** input[0]/[1] track the steer ramp (255 R / 255 L / 128 half) and A4
  (FUN_00470670) writes the front-wheel steer angle correctly: steerAng(+0x1a8/+0x26c) =
  +16.93 / -16.93 / +8.5 deg. NOT a steer-wiring bug.
- **Dead yaw is downstream:** body angular velocity (+0x9bc/+0x9c0/+0x9c4) = (0,0,0) EVERY frame ->
  A6a never converts the steer angle into yaw rate -> car_yaw flat -> can't turn.
- **ROOT = speed runaway + grounded oscillation:** speed RAMPS 62 -> 182 -> 300 in 3 frames
  (UNCLAMPED — the original maxspeed cap +0x190=34 vs +0x9e4 isn't limiting the port), and `gnd`
  oscillates 1/0 EVERY frame (airborne frames show vel.y=-33.9). The runaway speed makes the
  next-pos probe overshoot the GroundHeight region -> grounded=0 half the frames -> A6a's
  grounded-gated grip blocks fire intermittently -> no sustained lateral grip -> +0x9c0 stays 0 ->
  dead steering -> car plows straight (+z) off the strip at z~59 -> probe-ahead-fail zeroes velocity
  (UpdateCar 1496-1497) -> stick/oscillate/start-reset.
- FLOW bug too: UpdateCar probes + commits pos with the PRE-chain velocity, and io.grounded is a
  probe-AHEAD with that runaway velocity, not the car's CURRENT-pos grounding.

## WAVE-16 FIX (the real DRIVE-STABILIZE, focused):
1. **Speed clamp / scale** (prime mover): find why the port doesn't cap speed — the original gates
   drive force by +0x9e4 vs maxspeed +0x190, or A5/A6a drag/grip caps it; the 62->300 runaway
   suggests a missing/ineffective maxspeed gate OR a units/scale mismatch (chain internal units vs
   the standalone world coords).
2. **Grounded gate**: ground from the CURRENT pos (on-ground-now), not a probe-ahead with the
   runaway velocity; integrate pos from the chain's OUTPUT velocity (StepPlayer first, then pos);
   slide/clamp at the strip edge instead of hard-zeroing velocity.
3. Once speed is stable + wheels stay grounded, A6a grip should yield +0x9c0 -> steering works ->
   THEN calibrate kYawScale. (Steer wiring already confirmed correct.)

## UPDATE 2026-06-18 (part 2) — fixes landed + remaining blocker localized
FIXED (committed 88af2f8e, gated behind MASHED_REAL_PHYSICS):
- **Speed runaway** -> [U-A8-SPEEDCAP] clamp horizontal speed to kTop in UpdateCar. Speed now
  caps at 20.11 (was 300). Documented standalone stabilization (units RE still pending).
- **Grounded oscillation** -> ground from the car's CURRENT pos (not a probe-ahead); integrate
  pos from the chain's OUTPUT velocity. Grounded now stable, no crash, clean exit.
RESULT: drive is now SANE-SPEED + STABLE-GROUNDED, but the car STILL can't TURN (car_yaw flat).
LOCALIZED the remaining blocker:
- A4 writes the front-wheel steer angle to wheel +0x3c (=+0x1a8/+0x26c). CONFIRMED.
- A5 Phase-0 (ForceIntegrator.cpp:49-56) READS wheel +0x3c (piVar12[0xf]) and, when !=0, builds a
  rotation about kUpAxis by the steer angle via Rw_MatrixFromAxisAngle(...,mode 0) [CPU Rodrigues,
  works standalone] and rotates the wheel forward axis -> steered_fwd (+0xb4). So the WHEELS DO STEER.
- BUT body angular velocity +0x9bc/+0x9c0/+0x9c4 stays (0,0,0) -> no yaw. So the blocker is the
  ANGULAR-VELOCITY PRODUCTION: A5 Phase-5 "steer torque from velocity delta (when any wheel
  grounded)" (ForceIntegrator.cpp:157+) and/or A6a (Integrate2.cpp) accrual into +0x9bc/+0x9c0.
NEXT (focused): diag the steered_fwd (+0xb4) + the Phase-5 steer-torque + the +0x9c0 accrual to
find why the steered wheels don't produce a body yaw rate (candidate: Phase-5 torque needs a
lateral velocity-delta the straight-line drive doesn't yet have; or the +0x9c0 integrate is
gated/inert). THEN calibrate kYawScale. Steer wiring + wheel rotation are confirmed working.
