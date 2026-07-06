# D1 spike — proxy-body step live-vs-bypassed on the ORIGINAL (2026-07-06)

Answers `COLLISION_GATE_BRIEF_D1_2026-07.md` **Open-Unknown #1**: is the RW-Physics
proxy-body world (system 2) load-bearing for *rendered* car motion, or only internal
stability?

## Method

Extension of `re/frida/scenario_launch.py` (committed this session):
- **Bypass:** `Interceptor.replace` of `VehiclePhysicsWorldStep` `0x0047eb30`
  (`bool(void)`, globals-only; its own `DAT_006ce274==0` guard path returns 0, so a
  constant-0 replacement is the "no physics world" path callers already tolerate —
  `re/analysis/vehicle_promote_c2_b/0047eb30.md`). Armed at **+3 s** into the race so
  world init + the one-shot frame-0x7b qhull hull build complete normally; only the
  steady-state per-tick step dies. Scope note: the replacement removes the WHOLE
  function — body-sync PD drag, `FUN_0055deb0` solver step, position-correction
  readback (`FUN_0046d4d0` → record `+0x928` matrix), the off-track/respawn recovery
  branches, the post-step velocity clamp, and the `FUN_0047ea40` post-step handler.
  The verdict therefore attaches to the *function* (system 2's entire per-frame
  surface), not to one half of it.
- **Drive:** in-race injector ported from `capture_player_dynamics.py` (cook
  `FUN_00496530` onLeave forces player-0 descriptor `0x007f1038`: `[4]`=accel,
  `[2]/[3]`+`[0xe]/[0xf]`=steer). Full accel from race start; full right steer added
  at **+8 s**. (The launcher's control-4 press override does NOT drive the in-race
  car — confirmed this session, speed stayed 0.00 for 18 s under it.)
- **Telemetry:** 10 Hz agent-side samples of the player record: render pos
  (`+0x958/95c/960` — the surface the proxy readback writes), vel `+0x9b0..b8`,
  scalar speed `+0x9e4`, yaw rate `+0x9c0`, heading (`+0x9d4/+0x9dc`), grounded
  `+0x9e0`, step-call counter, world ptr. Control runs count `0x0047eb30` calls via
  a counter-only attach (measured **60.0/s** — once per tick, as documented).
- **Scenario:** track 0 (Training), mode 10, cars=1, 18 s hold, 2 runs per leg.
  PID-scoped; all four runs self-exited clean (no crash in any leg).

## Results (log/d1_spike_{control,control2,bypass,bypass2}.json)

| run | longest position-freeze | freeze location | grounded at freeze | heading span after steer |
|---|---|---|---|---|
| control  | 1.8 s (t=0.1, start countdown at spawn) | spawn | 4 | 3.20 rad |
| control2 | 1.8 s (t=0.1, start countdown at spawn) | spawn | 4 | 3.24 rad |
| bypass   | **5.9 s (t=12.1 → END OF CAPTURE)** | (13.63, −0.54, −3.63) ≠ spawn | **3** | 1.75 rad |
| bypass2  | **6.7 s (t=11.3 → END OF CAPTURE)** | (15.98, −0.12, −13.92) ≠ spawn | **3** | 1.45 rad |

- **2/2 bypass runs end in a terminal wedge** ~8–9 s after the bypass arms: zero
  motion under sustained full throttle, 3 of 4 wheels grounded, sunk below the
  control-run ground height (y −0.54 vs ~−0.25), at a different non-spawn position
  each run, never recovering for the rest of the capture. 2/2 controls never wedge.
- Pre-wedge, bypassed motion is broadly control-like (drives, turns, bounces off the
  arena walls — car↔world contact is system 1 and still works). Speed envelope stays
  bounded pre-wedge in both legs; the standalone-style unbounded runaway did NOT
  appear in the bypassed original.
- Steering: heading span after the +8 s steer input is ~half of control in both
  bypass runs (1.45–1.75 vs 3.20–3.24 rad), degraded even before the wedge.
- Mechanism consistent with the notes: the off-track/respawn recovery branches and
  the position-correction readback live INSIDE `0x0047eb30`
  (`vehicle_coupling.md:91-99`) — with them gone, the first wedge state is permanent.

## VERDICT (feeds gate D1)

**System 2 is load-bearing for rendered car motion.** Bypassing its per-frame driver
in the original produces a reproducible, terminal behavioral failure (permanent
wedge + degraded turning) within seconds. It is NOT merely internal bookkeeping, so
"skip system 2 entirely" is not a faithful option. Nuances the decision should
weigh:
- The failure mode is wedge/no-recovery, **not** the standalone's unbounded runaway —
  so the standalone's runaway is not *solely* "missing system 2"; the single-body
  coupling reduction itself diverges (per `vehicle_coupling.md` analysis) — two
  distinct gaps.
- What system 2 observably provides: wedge recovery/position correction and a
  material share of heading response. Basic propulsion + wall contact survive on
  system 1 alone.

Decision remains the owner's (Option A: reconstruct system 2; Option B: calibrated
reduction — which must now reproduce recovery + turn assist too, not just damping).
