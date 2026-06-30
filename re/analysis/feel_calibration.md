# Player feel calibration vs the original (2026-06-30)

Goal: make the standalone player car *drive like* the original. The standalone player uses
a kinematic scaffold (`TrackRenderer::UpdateCar`, the non-`MASHED_REAL_PHYSICS` branch) with
"HARVESTED" constants (HANDLING_V2_2026-06-10.md). Only `kDrag` (coast decay) had been
measured; accel/top-speed/turn were heuristic, because the input-matched in-race capture was
**deferred** ("needs an in-race input injector").

## Breakthrough: the in-race input injector (closes the deferred blocker)
`re/frida/capture_player_dynamics.py` — warps the ORIGINAL into a race (reusing the
scenario_launch path), and DRIVES the in-race car by forcing the cooked input descriptor.
- The control resolver override (`FUN_00497310`, control 4) only skips the intro/confirms — it
  does NOT drive the car (the physics reads the cooked descriptor, not the raw resolver). That
  was the long-standing "input doesn't reach the in-race car" wall.
- Real injector: hook the input cook `FUN_00496530` **onLeave** and force player-0's descriptor
  block (`0x007f1038 + p*0x4c`): **block[4] = accel**, block[5] = brake, **block[2]/[3] = steer**
  (block[0]/[1] are up/down — verified empirically; the cook writes 4 directional controls at
  block[0..3] with active-flags at block[0xc..0xf]). Forcing block[4]=0xff drives; block[2]/[3]
  steer. This is the same descriptor format the AI writes (FUN_00416250), so it is the real
  in-race control path. **Unblocks all future physics A/B**, not just feel.
- Sampling (player record `0x008815a0`): speed `+0x9e4`, vel `+0x9b0..b8`, ang-vel `+0x9bc..c4`,
  forward `+0x9d4/+0x9dc` (heading = atan2), grounded `+0x9e0`. Boot = subprocess+attach (never
  frida.spawn). Yaw rate is taken from d(heading)/dt (the `+0x9c0` angvel field reads ~0).

## Reference measured (Arctic, full throttle from GO)
- **Top speed (+0x9e4) ≈ 4324** internal units (matches the 2026-06-10 harvest; +0x190=34 const).
- **Acceleration is PUNCHY**: from GO the car hits ~63% of top in **~1 s** and ~top in **~2.2 s**,
  then drives off the Arctic dock and stops (same edge as WS-A). The old standalone "weighty
  ~3.3 s spool" was ~1.4× too sluggish.
- Steer: full-lock + full-throttle SPINS the car (transient yaw rate ~6.9 rad/s) — a dynamic
  behaviour the kinematic scaffold can't reproduce as a single number.

## Calibration applied (verified)
`kThrottle = kTop * 0.58` (was `* 0.42`); `kDrag` unchanged at 0.21. Raising the throttle factor
only speeds the approach to the `kTop` clamp — **top speed and coast decay are preserved**.
Env `MASHED_THROTTLE_K` overrides (=0.42 reverts).

Standalone accel curve (play-demo, world speed, max 20.11), before → after:
| metric | before (0.42) | after (0.58) | original (ref) |
|---|---|---|---|
| time to 63% of top | ~1.8 s | **~1.25 s** | ~1 s |
| time to ~top | ~3.1 s | **~2.5 s** | ~2.2 s |
| top speed | 20.11 | 20.11 (unchanged) | n/a (internal) |

The standalone's acceleration now closely matches the original; top speed + weighty coast intact.

## Deferred (documented, not blocking)
- **World top-speed mapping**: the original's top speed is internal (~4324); converting to world
  units/sec to recalibrate `kTop` needs the original's WORLD position over time (the record has
  no plain world-pos field; `FUN_0046d4a0` derefs to the RW frame matrix +0x30/+0x38 — call it
  via the injector next time). `kTop = radius_*0.25` (heuristic) kept.
- **Steady-state turn radius**: full-lock spins the original; a clean cornering rate needs a
  throttle-modulated steer on a non-crashing stretch. `kSteer = 2.2` (kinematic) kept.
- These are refinements; the injector tool now makes both straightforward.
