# WS-PHYS drive-hold runs — wave-16a measurement (2026-07-06)

Session: foundation-reset follow-on. Executes `re/SESSION_PROMPTS_WAVE16.md` items
WS-PHYS-DRIVE-HARNESS + the *measurement* half of WS-PHYS-DRIVE-STABILIZE.

**Harness discovery (survey, account2):** the wave-16 harness already existed in code —
`MASHED_DRIVE_HOLD=1` makes `RunRaceDemoStep` hold InRace ~30 s instead of Esc'ing at 6.6 s
(exe_main.cpp:1129-1132), and `MASHED_PLAY_DEMO=1` injects the scripted steer ramp
(0 → +0.5 → −0.5 → +1 → −1, exe_main.cpp:2264-2276) and logs
`PLAY-DEMO td steer car_yaw pos speed` every 0.25 s (exe_main.cpp:2337-2352). No code was
added or changed this session — runs used the existing binary (main @ d5e6a8d9 build).

## Runs (all PID-scoped, self-exited code 0, cwd = repo root)

| run | env (beyond MASHED_RACE_DEMO=1 GOTO=6 DRIVE_HOLD=1 PLAY_DEMO=1 TRACK_SEL=0 CAR_SEL=0) | log |
|---|---|---|
| A scaffold | — | `log/drivehold_scaffold_20260706.log` |
| B real | `MASHED_REAL_PHYSICS=1` | `log/drivehold_realphys_20260706.log` |
| C real, no rules | `MASHED_REAL_PHYSICS=1 MASHED_RULE_ENGINE=0` | `log/drivehold_realphys_norules_20260706.log` |

## Findings

### 1. Wave-15a's "erratic 0/260/0" scaffold diagnosis is CORRECTED: round resets, not physics
Run A telemetry is smooth between resets: accel ramps 0→1.72→4.65→7.41→…→**20.11 cap**;
yaw tracks every steer step with sign and rate. The periodic snap-backs to
`pos=(-25.2,15.8) yaw=1.5498 speed=0` + ~2.9 s freeze line up with the live rule engine's
round boundaries — the same log shows `R6 ROUND END winner=car1 … round 2 OVER t=15.5s …
round 3 OVER t=24.3s`. The 2026-06-18 STEER_CALIB_RUNTIME hypothesis 1 (RACE_DEMO capture
conflation) is superseded: captures were not the perturbation; **the race structure was**.

### 2. Real physics: unbounded acceleration, reproducing WS-A8 exactly
Run B/C: first throttled tick jumps speed 0→41.81, then 618.27, then **pinned at the 1500
safety clamp ~1.3 s after throttle onset** (WS_A8_REALPHYS_2026-07-01 reported the same
"saturates 1500 in ~1.5 s"). Run B additionally shows the race ending at td≈3.4 s (log jumps
straight to `race-demo done`) — the rule engine reacts to the runaway and ends the match,
which is why B produced only 14 telemetry lines.

### 3. Real physics: steering is dead
Run C: `car_yaw` flat at 1.5498 through steer +0.5 and +1.0 (scaffold turns cleanly under
the same ramp). The only yaw changes are discontinuous flips (−1.635 @td 8.47, −1.114
@td 18.02, −0.941 @td 26.47) co-timed with position reversals at the arena edge (y≈53-59) —
mechanically consistent with wall impacts, not steering.

### 4. Metric divergence: `speed`=1500 while position advances ~12.5 u/s
At the clamp, successive positions advance ≈12.5 units/s — scaffold-comparable — while
`car_speed()` reads 1500. The internal velocity state explodes while rendered motion stays
modest. This matches the D1 brief's diagnosis (`COLLISION_GATE_BRIEF_D1_2026-07.md`,
`vehicle_coupling.md`): the original stabilizes through a **two-body loop** (render body +
RW-Physics proxy body, PD gain 20.0 @0x005ccd6c); the standalone's single-body reduction
diverges without the proxy's damping/turn coupling.

### 5. Respawns persist with `MASHED_RULE_ENGINE=0`
Run C still snaps to spawn every ~8.5-9 s (td 10.59 / 19.08 / 27.53), each ~1-2 s after a
wall-impact flip. `[UNCERTAIN]` which mechanism regrids the car with rules off —
candidate: out-of-world/fall recovery after the 1500-speed wall exit; not traced this
session (blocked on the same runaway making the pre-reset state degenerate).

## Status vs wave-16

- **WS-PHYS-DRIVE-HARNESS: DONE** (existed; verified working end-to-end, 3 clean runs).
- **WS-PHYS-DRIVE-STABILIZE: measurement DONE, repair BLOCKED ON GATE D1.** The instability
  is not harness conflation and not a scaffold regression — it is the WS-A8 coupling
  divergence. Fixing it *is* the D1 fork: Option A (reconstruct the RW-Physics proxy-body
  world) vs Option B (calibrate the single-body reduction via the recovered constants).
  See `re/analysis/COLLISION_GATE_BRIEF_D1_2026-07.md`; its Open-Unknown #1 spike (Frida:
  proxy body live-vs-bypassed on the ORIGINAL) is the cheap decisive pre-decision test.
- **WS-PHYS-STEER-CALIB: blocked** behind the same gate (nothing to calibrate until the
  drive is stable).
