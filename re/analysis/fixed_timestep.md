# WS-A s4 — fixed 60 Hz simulation timestep (standalone)

**Goal.** Make the standalone (`mashed_re.exe`) drivable-race physics run at a fixed,
render-fps-independent cadence that matches the original's feel. The original MASHED is
frame-COUPLED with no native cap; on this hardware the d3d9 shim paces it to 60 fps
(`MASHED_FPS_CAP`, default 60 — see memory `project-frame-limiter-speed-fix`). So the
original's "correct" feel is a 60 Hz fixed step.

## The problem (before)

The standalone's drivable car is updated by `RenderFrame() -> TrackRenderer::UpdateCar(di)`
(`exe_main.cpp`), **not** by the `GameFlow_Update(0.016f)` scaffold (that drives the
RaceSession state machine, a no-op until the frontend launches a race — `exe_main.cpp:5531`).

`UpdateCar` consumed a per-render-frame `dt` computed from `GetTickCount()`
(`exe_main.cpp:1958-1962`):
- `GetTickCount()` has ~15.6 ms granularity (no `timeBeginPeriod` anywhere). So at high
  render fps most frames see `dt == 0` and early-return (`TrackRenderer.cpp:1735`,
  `if (in.dt <= 0.f) return;`), while occasional frames see a 15 or 31 ms step.
- Net: the sim ran a **lumpy ~64 Hz** — jittery, occasionally double-stepping, and
  non-deterministic run-to-run (wall-clock dt varies).
- Several damping terms use the linear-clamp approximation of exponential decay
  (`1 - clamp(k·dt,1)` and `x += err·clamp(k·dt,1)`): `TrackRenderer.cpp:1887` (lateral
  grip), `:1997/:2075` (AI speed ramp), `:2070` (AI turn rate), `:2095` (camera follow).
  These are only *approximately* framerate-independent, so feel drifts with dt jitter.

All dt consumers are dt-scaled (no UNSCALED per-frame bugs found — full map in the
session investigation). So the fix is purely the timestep source, not the integrators.

## The fix

A standard fixed-timestep accumulator at the `UpdateCar` call site (`exe_main.cpp`):
- A high-resolution **QPC** frame-delta `sim_real_dt` (reusing `s_qf`), clamped to 0.25 s
  (no spiral-of-death after a stall).
- `s_simAccum += sim_real_dt;` then `while (s_simAccum >= 1/60) { UpdateCar(di, dt=1/60); s_simAccum -= 1/60; }`
  capped at 6 sub-steps (10 fps floor).
- Input (`di.accel/di.steer`) is sampled once per render frame and held across sub-steps.
- The GetTickCount `dt` is **unchanged** as the clock for camera input, the demo schedule,
  and the `t` wall-clock (presentation/timing, not physics).
- `MASHED_SIM_HZ` overrides the rate; `=0` reverts to the original variable-dt single step
  (A/B hatch). Default 60.

Timers that decrement by `in.dt` (countdown, powerup boost/shield, AI spin/slow, race
clock, wheel spin, pickup cooldowns) all ride the fixed step unchanged — now deterministic.

## Verification (A/B, same instrumented binary, env-toggled)

Run: muted Arctic play-demo (`MASHED_TRACK_VIEW=1 MASHED_CAR=1 MASHED_PLAY_DEMO=1`),
`after` = default (60 Hz), `before` = `MASHED_SIM_HZ=0` (variable dt). A once-per-second
`SIMHZ` trace line logs render-fps vs effective sim-Hz.

- **Decoupling proof (measured 2026-06-29, Arctic play-demo):**
  - AFTER (default): `render_fps≈180, sim_hz=60.0` steady for the whole run — sim locked
    at exactly 60 Hz while the render loop spins at ~180 fps.
  - BEFORE (`MASHED_SIM_HZ=0`): `render_fps=180, sim_hz=180` — the sim tracks render 1:1
    (render-coupled; the actual integration cadence is the lumpy GetTickCount ~64 Hz with
    ~116 no-op `UpdateCar` early-returns/sec discarded).
- **No-regression (feel), same scripted input:** max speed **20.11 both**, mean 1.17 vs
  1.19, **final pos (-25.2, 59.9) both**, trajectory matches (end yaw 1.3730 vs 1.3839 —
  the only delta, from per-frame input sampling). The fixed step preserves feel exactly.
  Note: the variable-dt GetTickCount path summed dt to real time, so it was already
  ~real-time (not "too fast"); the win is uniform 16.67 ms steps + determinism + an exact
  60 Hz cadence matching the original, and dropping the ~116 wasted no-op calls/sec.
- **Surfaced (not caused by this change):** the play-demo drives straight off the Arctic
  dock to z≈59.9 and freezes at the mesh edge (speed 0) — identical before/after — i.e.
  the pre-existing gates-less off-track recovery gap (`TrackRenderer.cpp:1851-1868`). Repro
  for the next workstream.

## Notes / follow-ups

- `GameFlow_Update(0.016f)` (the RaceSession scaffold) still steps once per render frame;
  it does not move the drivable TrackRenderer car, so it's out of scope here. If the
  frontend-launched race path becomes the primary sim, fold it into the same accumulator.
- Standalone replay is not implemented yet (`Vehicle/Replay.cpp` hooks the original only).
  When added, it must key off **sim-step count**, not render frames (the accumulator emits
  0..N steps per frame).
- ~~Egypt off-track freeze (gates-less recovery, `TrackRenderer.cpp:1851-1868`)~~ **FIXED
  2026-06-29 (WS-A s5).** Added `TrackRenderer::RecoverOffMesh()` + `FindOnMeshHeading()`:
  when the next step lands off-mesh and `gates_` is empty, a 16-ray ring probe finds an
  on-mesh escape heading near the current one and nudges the position inward (committing
  only an on-mesh landing) — gate-independent, so the car never permanently freezes against
  an edge. Wired into both off-mesh branches (real-physics + scaffold). Measured (same
  Arctic play-demo): post-td15 window went from frozen (1 position, speed 0) to **77/77
  unique positions, max speed 15.5, mean 5.77** — the car drives off the dock, recovers,
  and keeps going (ends at (-48.1,-59.9) instead of stuck at (-25.2,59.9)).
