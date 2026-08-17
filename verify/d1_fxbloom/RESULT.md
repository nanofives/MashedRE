# D1 — the orange bloom isolated to FX particles, and a derived guard applied (2026-08-16)

> **CORRECTION, same day — superseded by `verify/d1_fxcut/RESULT.md`.** The isolation table
> and the mechanism below stand and are load-bearing. Two things are wrong: the coverage
> guard was only a partial fix (the class was subsequently cut from the default build), and
> the line about `01_action` exposing "the underlying sky-colour divergence" is false — there
> is no sky divergence. Every "after vs librw" number in this file is measured against a
> librw capture that still contained the bloom, which is why they looked like a residual.

Follow-on to `verify/d1_nopart/RESULT.md`. `MASHED_NO_PARTICLES=1` kills the particle pass
**and** the pickup orbs together, so it named a pass, not a cause. Two new isolation gates
split it apart:

- `MASHED_PARTS_KINDS=<mask>` (`ParticleSystem.cpp`) — bit0 ambient, bit1 car-spray, bit2 fx.
- `MASHED_NO_PICKUPS` (`TrackRenderer.cpp:4601`) — the additive glow orbs alone
  (`PickupField.cpp:231` blends `DESTBLEND_ONE`, so they were a live suspect).

Same recipe as `verify/allmode`, four runs on one build.

## Isolation — it is entirely kind==2 (FX)

| shot | ctl vs old baseline | no-FX delta | no-pickups delta |
|---|---:|---:|---:|
| `01_inrace_track` | 0.00% | **87.83%** | 0.04% |
| `01_action` | 0.00% | **99.40%** | 0.00% |
| `round2_result` | 0.00% | **98.57%** | 0.00% |
| `round3_result` | 0.00% | **98.37%** | 0.00% |
| `01_grid` | 0.00% | 0.00% | 0.01% |

The control reproduces the pre-change baseline at 0.00% on every shot, so the source edits
did not perturb the run and the determinism gate still holds.

Suppressing kind==2 alone reproduces the **entire** `MASHED_NO_PARTICLES` effect to the
decimal (87.83 / 99.40 / 98.57 / 98.37 are the same four numbers). Ambient weather, car
spray and the pickup orbs move the same frames by 0.00-0.04%. The bloom is the FX class and
nothing else.

## Mechanism

`ParticleSystem.cpp:246` deliberately exempted kind==2 from the WS-E s4 near-camera fade,
on the reasoning that "FX are deliberate and brief". That exemption is the defect.
`TrackRenderer::SpinOut` (`:2796`) spawns 36 **fully opaque** (`0xffffd060`, alpha 0xFF)
billboards of half-extent up to `track_radius_ * 0.025` (~1.4u on Arctic, R=55) **at the
car**, and the chase rig sits ~1.3u behind it. Each quad subtends more than the whole
viewport; 36 alpha-composite to a solid wall. Every FX colour in the file is orange or
yellow (`0xffffd060`, `0xffffe080`, `0xffff8020`, `0xffff6020`, `0xE0FFC850`).

This also explains the accumulation the 2026-08-15 writeup flagged: spin-outs are
eliminations, so the frames that diverge are the ones captured just after one.

## The guard applied

A screen-coverage budget on kind==2, not another distance band: distance alone does not say
how much of the lens a billboard eats, `(size/dist)/tan(fovy/2)` does. Derived from the
projection at `TrackRenderer.cpp:4021/4025` (60 deg vertical), fed through the new
`ParticleSystem::SetFovY` so there is one source of truth. Full alpha up to a quarter of the
viewport half-height, zero by 0.75.

## Result — partial

| shot | before vs librw | after vs librw | after vs no-FX |
|---|---:|---:|---:|
| `01_inrace_track` | 71.61% | **41.31%** | 44.22% |
| `round3_result` | 69.15% | **36.54%** | 18.44% |
| `round2_result` | 68.94% | **49.16%** | 35.93% |
| `01_action` | 21.69% | 77.72% | 2.56% |
| `r5/car_5_chase` | 0.92% | 0.35% | 0.20% |
| all 11 others | unchanged | unchanged | 0.00% |

The world is legible again — `01_inrace_track` now shows track, props, copter and cars
through the FX. But a mid-distance orange glow cloud remains: those billboards sit **below**
the coverage threshold individually and still stack, because every FX particle is emitted at
alpha 0xFF. A burst of 36 opaque quads saturates at any distance where they overlap.

`01_action` rising to 77.72% is the masking effect already recorded in `d1_nopart`: with the
bloom removed, the underlying sky-colour divergence is exposed rather than cancelled. Its
2.56% against the no-FX run confirms the guard removed nearly all FX on that frame.

## Open

The residual is stacked opacity, not billboard size. Bounding it means tuning the emitters
(count / per-particle alpha) — and those emitters are `[SCAFFOLD]` invented presentation
(`ParticleSystem.h:11-13`), not RE'd from the original `Particle/` system. So the next step
is a scope decision, not a measurement. Recorded rather than guessed at.
