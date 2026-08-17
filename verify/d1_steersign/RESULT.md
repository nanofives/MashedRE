# AI steer-sign contradiction: real, provable, and DEAD CODE on the default path (2026-08-16)

Follow-up to `verify/d1_mirrorfix/AUDIT.md` item 1. The contradiction is confirmed by static
reading; it **cannot** be resolved by observation today, and the reason is worth recording.

## The contradiction is certain

`Ai/AiStandalone.cpp` contains two steer-sign conventions that disagree by construction:

- **Verbatim bands** (`:519-560`, ported from asm `0x004165c0..`): `err < kSteerSplit(180)` ->
  `ctrl[0]`; `err > 180` -> `ctrl[1]`. `err` comes from `SteerAngleError` (`FUN_00415e20`,
  verbatim including its negated-Z heading frame, `hz = -vz` at `:148`), range `[0,360)`.
- **Pure-pursuit** (`:424-494`): `se = err; if (se > 180) se -= 360` -> `toCtrl0 = (se >= 0)`.
  Its own comment states the intent explicitly:
  *"toCtrl0 = (se>0): err<180 (se>0) -> ctrl[0], matching the bands."*
- **`[G4]` then flips it** (`:488-492`, `c0 = s_flip ? !toCtrl0 : toCtrl0`, default ON),
  breaking the match the line above documents.

So flipped-pursuit and the bands are opposites. **At most one can be faithful to the
original.** That much needs no experiment.

## Why it cannot be settled by running it

Attempted: `MASHED_AI_PUREPURSUIT=1 MASHED_AI_NAV=1` with `MASHED_AI_STEERFLIP=1` and `=0`,
intending to score each by whether distance-to-target shrinks in **world space** (`ai_nav.log`
logs `own=`/`tgt=`), which is convention-independent and immune to the day's mirror question.

`ai_nav.log` was **never written** on either run. Cause, at `TrackRenderer.cpp:2610-2614` and
`:2679-2687`: once the `.AI` banks load — which is the **default** — the AI cars are steered by
the standalone's own motion model, not by `ctrl[]` at all:

```c
float yerr = std::atan2(dz, dx) - a.yaw;      // world space, standard frame
while (yerr >  PI) yerr -= 2PI;  while (yerr < -PI) yerr += 2PI;
a.yaw += yerr * clamp(6.0f * dt, 0, 1);
```

The ported `Ai_ComputeTarget` (SelectSpline + `FUN_00443dc0` lookahead) supplies the *target*;
the *steering* is this closed-loop proportional controller. It always reduces `yerr`, so it is
structurally incapable of steering the wrong way and has no sign ambiguity to inherit. The
comment at `:2612-2614` says why the verbatim path is bypassed: the ControlStep bands'
"accel+brake deadlock against the approximate physics chain."

**Therefore neither the bands nor the pure-pursuit `ctrl[]` code drives anything in the
default build.** The contradiction is latent, not live. Nothing is currently steering
backwards because of it.

## What that means for the [G4] evidence

`[G4]`'s justification — a nav trace showing the car driving away from its target — was taken
when the pursuit path did drive cars. It is world-space evidence and today's mirror fix does
not touch it. But it only establishes that *something* in that chain was inverted, and
flipping the branch was one of at least two places to fix it: the other candidate is the
`ctrl[]` -> physics steer mapping (`Vehicle/VehiclePhysicsRun.cpp:391-405`,
`input[0] = (st > 0) ? m : 0`, documented as `+steer -> input[0] (sign A, +angle)`), which is
shared by **both** paths. If the defect is there, `[G4]` fixed the symptom in one path and
left the bands wrong — which is exactly the state the code is in.

I am not picking between these. Doing so needs the ported ControlStep actually driving a car,
which is blocked on the accel/brake deadlock above.

## Recommendation

Do **not** resolve by choosing a sign now — there is no evidence that can distinguish them
while the code is unreachable, and guessing would put an unfalsifiable claim into a file that
already contains two contradictory ones.

Record it instead, and attach the resolution to the work that unblocks it: when the verbatim
ControlStep is wired to drive cars (i.e. when the accel/brake deadlock is addressed), run the
world-space A/B above as the first acceptance check. The harness is ready — `MASHED_AI_NAV`
already logs `own`/`tgt`/`vel`, and `MASHED_AI_STEERFLIP=0/1` is the toggle.

A tracker row belongs here (`UNCERTAINTIES.md`, via `re-classify` — not hand-edited): two
opposite steer conventions coexist in `AiStandalone.cpp`, both currently unreachable,
resolution gated on the ControlStep wiring.
