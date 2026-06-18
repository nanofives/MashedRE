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
