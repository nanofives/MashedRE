# A8 — velocity-vector motion model: the heading-plus-scalar model is retired

Date: 2026-08-25. Standalone-side change (`mashed_re.exe`). No Ghidra writes.

## VERDICT

The port's motion model now integrates **position from the velocity vector** and
**orientation from omega**, separately, exactly as `FUN_0046e9e0` does. The
body-orientation integrator (`BodyOrientationIntegrate.cpp`, ported and cited on
2026-08-25) is **wired**. Both gates pass:

- **Gate (a) speed profile** — no collapse. Median internal speed 377.15 / 373.61
  across two post-change runs, against a same-binary pre-change control band of
  390.90 / 398.50. Same order; the two controls differ from each other by 2%, the
  change costs a further 4-6%. The two prior attempts at wiring the orientation
  gave 7.28 and 5.56 on this same recipe.
- **Gate (b) yaw-vs-speed shape** — the sign of the relationship **flips from
  falling to rising**, which is the A8 mechanism (`grip = speed * 1/1500` clamped
  to 1.0, multiplying both steer torques, `_DAT_005ce1e8` @0x3a2ec33e).

## What changed, in three coupled parts

All three land together. That coupling is the finding from the two failed
attempts: any one of them alone regresses the drive.

1. **Position integrates from the velocity VECTOR.** `io.drive_speed` (scalar),
   `g_bodySpeed`, the PD relaxation at gain 20, and the env knobs
   `MASHED_CHAINSCALE` / `MASHED_ALIGNRATE` / `MASHED_TOPSPEED` are **deleted**,
   not re-defaulted. The increment is the original's, accumulated per substep:

   ```
   inc = dtMs * _DAT_005cc948 * _DAT_005cea80 * (+0x9b0, +0x9b4, +0x9b8)
   ```

   FMULs at `0x0046e9e8` and `0x0046e9f6`; velocity reads at `0x0046e9fe` /
   `0x0046ea0a` / `0x0046ea14`; stores at `0x0046ea5f` / `0x0046ea69` /
   `0x0046ea73`. `_DAT_005cc948 = 0x39aec33e = 3.33320e-4`,
   `_DAT_005cea80 = 0x3b360bc0 = 2.77804e-3`; product `9.2598e-7` per ms.
   Full decode: `re/analysis/data/A8_position_law_20260825.md`.

2. **`BodyOrient_*` is wired.** Basis seeded at spawn / grid placement / off-mesh
   recovery via the new `VehiclePhysics_ResetOrientation`; per substep
   `BodyOrient_OmegaFromSteer` then `BodyOrient_IntegrateStep`; `io.yaw` is read
   back with `BodyOrient_Heading`. The integrated basis **is** the transform
   handed to A4 (`VehicleControlIntegrate`), replacing `BuildYawMatrix(io.yaw)`.

3. **The velocity-chasing lag is gone**, along with `MASHED_ALIGNRATE`. Its
   default of 7.0 was a fitted number with no address behind it, and it is
   precisely what made slip unrepresentable.

Substep order follows `FUN_004709a0` (`FUN_0046e9e0` -> `FUN_0046f6c0` ->
`FUN_00469aa0`): position + orientation integrate first, contacts after.

## Measurements

Recipe (all runs): `MASHED_REAL_PHYSICS=1 MASHED_RACE_DEMO=1 MASHED_PLAY_DEMO=1
MASHED_GOTO=6 MASHED_TRACK_SEL=0 MASHED_CAR_SEL=0 MASHED_DRIVE_HOLD=1
MASHED_WIN_POS=left-bl`. Artifacts under `verify/a8_velvec_20260825/`.

### Gate (a) — speed profile, same-binary control first

| run | median | max | nonzero samples |
|---|---:|---:|---:|
| archived baseline 2026-08-24 | 401.80 | 3664.81 | 69/114 |
| ctrlA pre-change (today's build) | 390.90 | 3664.81 | 69/114 |
| ctrlB pre-change (today's build) | 398.50 | 3651.56 | 69/114 |
| velvec_A post-change | 377.15 | 3459.32 | 69/114 |
| velvec_B post-change | 373.61 | 3455.12 | 69/114 |

The two controls establish the noise floor: 45 of 114 samples were bit-identical
between them, medians differ by 2%. The post-change runs are tight against each
other (377.15 / 373.61) and sit 4-6% below the control band.

### Gate (b) — yaw rate (rad/s) by speed band, at full lock

Per-sample rates, averaged within band (never a total divided by a span). Source:
`MASHED_MOTION_DIAG` per-sim-step log, `dt = 1/60` from the `SIMHZ` lines
(`sim_hz=60.0`). Original from `verify/a8_steer_20260824/orig_steerR.msd`, body
heading `atan2(+0x9dc, +0x9d4)`, same `dt` assumption.

| speed band | ORIGINAL (n) | port PRE-fix | port POST velvec_A (n) | POST velvec_B (n) |
|---|---|---|---|---|
| 0-500 | 0.341 (230) | 0.0638 | 0.132 (79) | 0.132 (79) |
| 500-1000 | 1.435 (149) | 0.0827 | 0.438 (102) | 0.441 (102) |
| 1000-1500 | 2.312 (188) | 0.0785 | 1.064 (151) | 1.071 (151) |
| 1500-2000 | 2.662 (332) | 0.0681 | 1.286 (86) | 1.281 (85) |
| 2000-3000 | 2.652 (541) | 0.0532 | 1.500 (111) | 1.500 (111) |
| 3000-5000 | -- (0) | -- | 0.850 (86) | 0.850 (87) |

Every band has n > 3. The pre-fix column is the quoted prior measurement, not
re-run on today's binary; a coarser cross-check that WAS run on both binaries
today (PLAY-DEMO samples, 0.25 s cadence, full lock only) reproduces the flip:

| speed band | ctrlA pre | ctrlB pre | velvec_A post | velvec_B post |
|---|---|---|---|---|
| 0-500 | 0.099 (5) | 0.098 (5) | 0.145 (5) | 0.142 (5) |
| 500-1000 | 0.101 (5) | 0.103 (5) | 0.456 (5) | 0.472 (6) |
| 1000-1500 | 0.088 (7) | 0.088 (7) | 0.688 (10) | 0.704 (10) |
| 1500-2000 | 3.814 (8) | 3.732 (8) | 0.774 (5) | 0.776 (5) |
| 2000-4000 | 1.053 (12) | 1.017 (12) | 1.093 (12) | 1.116 (11) |

The pre-fix 1500-2000 cell (3.8) is a sampling artifact, not a yaw rate: at 0.25 s
cadence a round-boundary respawn (`car_yaw` snaps back to the spawn value 1.5498)
lands inside a single sample interval. The per-sim-step metric above does not have
this problem. Both metrics agree on the qualitative result.

### Slip is now representable

Full lock, speed > 500, n = 537: mean `|slip| = 0.0366 rad`, max `0.0741 rad`,
where slip = `velocityHeading - bodyHeading`. Under the old model this quantity
was identically 0 by construction. Both runs agree to 4 decimal places.

## Residuals — stated, not papered over

1. **Magnitude gap.** The port reaches 48-57% of the original's yaw rate in every
   populated band. The SHAPE is right and the SIGN is right; the gain is not.
   Not investigated here.
2. **The 3000-5000 band drops to 0.850** (n=86, both runs) where the original has
   no samples on this capture. Unexplained. Needs an original-side capture that
   reaches that band before anything can be concluded.
3. **[DEVIATION] the position mode gate is not applied.** `FUN_0046e9e0` zeroes
   the position increment unless `FUN_0040e350()` returns 6, 0xb or 0xa (gate
   `0x0046ea1e`..`0x0046ea4c`). The standalone's `Fi_GameMode()` is a **stub
   returning 0** (`ForceIntegratorStubs.cpp:49`), so consuming the gate would zero
   all motion unconditionally — that would be reading our stub, not the original.
   The gate is therefore omitted and the omission is stated in-source at the call
   site. Porting `FUN_0040e350` is not a one-line change: it also gates
   `RubberBandGate` and `Integrate2`'s mode-7 branch.
4. ~~**[UNCERTAIN] world-unit correspondence.**~~ **CLOSED and the reasoning here
   was WRONG — see the second follow-up below.** This paragraph claimed the
   `+0x928` translation is "contact-rebased by `FUN_0046f6c0` and stays bounded",
   so it could not be the world position. It IS the world position. It is bounded
   in that capture because the capture holds FULL LOCK for 38 s and the car drives
   a ~2 world-unit circle. Nothing rebases it. Retained struck through because the
   false premise was fed to a Ghidra subagent and shaped its Q3/Q4 answers.
5. **[UNCERTAIN] basis storage location** and the `0x004c4680` axis-pair tie-break
   remain open, unchanged from `BodyOrientationIntegrate.cpp`'s own comments.

## Method notes (traps avoided)

- Same-binary control run **before** attributing any delta, twice, to establish
  the noise floor.
- The steer input is logged on the same line as the effect it is supposed to
  drive, and the diag is uncapped, so no sample cap can act as a regime filter.
- Rates are averaged per sample within a band; no total is divided by a span.
- Both post-change runs reported; they agree, so the result is not one lucky run.
- Logs normalized with `tr -d '\r'` before any comparison.

---

# Follow-up, same day — the yaw-rate gain gap is a 3x TIME-BASE error

Residual (1) above ("port reaches 48-57% of the original") is **diagnosed**. The
fix is identified and cited, and it is **not applied**, because applying it fails
gate (a). Both arms are recorded here.

## The gap is one constant, not a law error

First, the ported steer law is **confirmed against the original's own telemetry**.
If `dHeading_per_frame = 255 * 100 * grip * 1e-4 * 3.334e-4 * B` with
`grip = min(speed/1500, 1)`, then `dHeading / grip` must be constant across speed.
Measured on `orig_steerR.msd`:

| speed band | n | dHead/frame | implied B | grip |
|---|---:|---:|---:|---:|
| 200-500 | 87 | 0.00966 | 55.29 | 0.213 |
| 500-1000 | 149 | 0.02441 | 54.33 | 0.527 |
| 1000-1500 | 188 | 0.03875 | 53.60 | 0.851 |
| 1500-2000 | 332 | 0.04462 | 52.48 | 1.000 |
| 2000-2600 | 541 | 0.04462 | 52.48 | 1.000 |

`B` is flat to 5.2% while `grip` varies 4.7x, and the saturation begins exactly at
speed 1500 as `_DAT_005ce1e8 = 0x3a2ec33e = 1/1500` predicts. So the grip law and
the clamp are right, and the entire residual is the single scalar `B` — the
per-frame substep budget. Port feeds 16.67; original implies ~52.5.

Two further facts rule out the obvious confound: the `.msd` capture is one
snapshot per render frame (`Interceptor` on `0x004c1be0`,
`scenario_launch.py:536-559`) and 2335 contiguous frames over a 38 s hold is
61.4/s, so `dt = 1/60` for the original is correct within 2%.

## Ghidra: the original's budget is a fixed 50, not the wall frame time

Decode: `re/analysis/data/A8_substep_budget_20260825.md`.

- `FUN_00470c70`'s budget is a fixed immediate **50 (`0x32`)**: `PUSH 0x32`
  @`0x0042c980`, forwarded verbatim `MOV ESI,[ESP+8]` @`0x00425a78` then
  `PUSH ESI; CALL 0x00470c70` @`0x00425a8d`. No wall delta reaches it.
- It is produced by a tick quantizer: `DAT_007f1000` pinned to `0x32`
  @`0x004933d5` (`FUN_00493390`), re-derived as `uVar1*0x32` @`0x00493514`
  (`FUN_00493480`) with the sub-50 remainder carried in `DAT_007719d4`.
- The catch-up loop iterates `ceil(DAT_007f1000/50) = 1` in steady state, so the
  dispatcher runs **once per render frame** — the multi-tick path is not the cause.
- The value is forwarded to `FUN_0046e9e0`'s `param_1` with a single `int->float`
  and **no scale or divide**.
- 50 units **is** one 1/60 s frame, from the engine's own constants:
  `DAT_007f1004 = 0x3c888889 = 1/60` and `_DAT_005cc948 = 0x39aec33e = 1/3000`,
  and `50 * (1/3000) = 1/60`. **So the unit is 1/3000 s, not 1/1000 s.** The port
  called it "ms" and passed `dt*1000`; the correct budget is `dt*3000`.
- Also (Q2): the outer 50 chunk is split into **2 x `FUN_004709a0(25.0)`** (inner
  cap `0x19` = 25), so the substep granularity is 25 units, not 50.

`50 / 16.67 = 3.00x`, against a measured 3.15x. The residual 5% is the quantizer's
remainder accumulator plus band scatter.

## Applying it: gate (b) improves, gate (a) FAILS — so it is reverted

Ran `frameMs = dt*3000` and `kMaxSubstep = 25`, two runs each.

Gate (b) — yaw rate, rad/s, full lock, vs the original:

| band | ORIGINAL | port dt*1000 | port dt*3000 (A / B) |
|---|---|---|---|
| 0-500 | 0.341 | 0.132 | 0.060 (208) / 0.054 (209) |
| 500-1000 | 1.435 | 0.438 | 1.191 (15) / 1.192 (15) |
| 1000-1500 | 2.312 | 1.064 | 2.011 (10) / 2.014 (10) |
| 1500-2000 | 2.662 | 1.286 | 2.613 (12) / 2.609 (12) |
| 2000-3000 | 2.652 | 1.500 | 2.878 (88) / 2.958 (91) |
| 3000-5000 | -- | 0.850 | 3.328 (285) / 3.366 (279) |

The mid and high bands land close to the original. But `n` collapses to 10-15 in
the 500-2000 bands, and the 0-500 band gets **worse** (0.132 -> 0.060).

Gate (a) — speed profile:

| run | median | max | nonzero |
|---|---:|---:|---:|
| ctrlA / ctrlB (control band) | 390.90 / 398.50 | 3664.81 / 3651.56 | 69 / 69 |
| dt*1000 (shipped) | 377.15 / 373.61 | 3459.32 / 3455.12 | 69 / 69 |
| dt*3000 | **33.27 / 26.24** | 4322.49 / 4321.85 | 69 / 70 |

A 12x collapse of the median. The mechanism is visible in the trace: the car
accelerates past the track (peak internal speed 3664 -> 4322), leaves the mesh and
respawns; the median is the near-zero time between respawns. The thin `n` in gate
(b)'s mid bands is the same effect — the car no longer spends time there.

**Reverted.** Post-revert re-run confirms the passing state is restored: gate (a)
median 390.86 (inside the control band), gate (b) identical to `velvec_A` in every
band. Artifacts `tb_A`/`tb_B` (applied) and `revert_C` (restored) are kept.

## What this means

The position increment scales with the **same** budget as the yaw. That the yaw
wants 3x and the position cannot take 3x is direct evidence that the remaining
error is the **internal->world unit scale of our track geometry** — the open
`[UNCERTAIN]` at Q4/Q5 of `A8_position_law_20260825.md`, where the original's
absolute world-position holder could not be located, so the scale has never been
verified against anything.

**Do not close this by putting a compensating factor on the position term.** That
is the `MASHED_CHAINSCALE` mistake this session removed. The order is: locate the
original's world-position holder, verify the world-unit correspondence, and only
then re-apply the time base — at which point both halves should move together.

The time-base finding itself is fully evidenced and stands on its own; it is
recorded in-source as a long comment on `frameMs` in `StepCar` so the next
attempt starts from it rather than re-deriving it.

---

# Second follow-up, same day — the world-position holder, and a retraction

## RETRACTION

The first follow-up said the remaining blocker was the internal->world unit scale,
because "the `+0x928` block translation is contact-rebased by `FUN_0046f6c0` and
stays bounded, so it is not the world position". **That is wrong, and it was my
error, not the subagent's.** I observed the boundedness in
`verify/a8_steer_20260824/orig_steerR.msd`, concluded "rebased", and then handed
that conclusion to a Ghidra child *as an established empirical fact* in its brief.
It went looking for a rebaser, named one, and marked the world-position question
`[UNCERTAIN]` — for a field that was in front of us the whole time.

The capture holds **full lock for 38 s**. The car drives in a circle. A circle is
bounded. This is the regime-artifact trap the session brief warns about, committed
while explicitly trying to avoid it: a constant-input capture manufacturing a
structural conclusion.

## The world-position holder IS record +0x928 + 0x30

Two independent confirmations, both from the original's own capture.

**1. Its per-frame delta is exactly the cited position increment.** Comparing
`d(translation)` against `50 * _DAT_005cc948 * _DAT_005cea80 * velocity` frame by
frame, over all frames with speed > 500:

| axis | n | median ratio | mean ratio |
|---|---:|---:|---:|
| X | 1207 | 1.0147 | 1.0165 |
| Z | 1210 | 1.0077 | 0.9542 |

Ratio 1.00 within measurement error, on both axes. **Zero** per-frame
discontinuities above 0.5 across the whole drive, so nothing rebases it. This also
re-confirms the budget of 50 independently of the yaw path.

**2. The bounded trajectory is a circle of the radius the physics predicts.**
Frames at speed > 1500 (n = 873): fitted centre (0.05, -2.22), mean radius **2.00**
(sd 0.55). Predicted from unrelated quantities, world speed / yaw rate =
5.74 / 2.67 = **2.15**. Agreement.

So `A8_position_law_20260825.md` Q4 is answered: the holder is the `+0x928` /
`+0x968` double-buffer translation, and the position LAW plus the budget of 50
reproduce it to ~1.5% over ~1200 frames.

## The world units MATCH, and dt*3000 is right

With the holder identified, the original's world-space behaviour is directly
comparable. At **matched internal speed** (the control the earlier comparison
lacked), full lock:

| band | metric | ORIGINAL | port dt*1000 | port dt*3000 |
|---|---|---:|---:|---:|
| 1500-2000 | world speed (u/s) | 4.94 | 1.58 | **4.74** |
| 1500-2000 | yaw rate (rad/s) | 2.66 | 1.29 | **2.61** |
| 1500-2000 | turn radius (u) | 1.86 | 1.23 | **1.81** |
| 2000-2600 | world speed (u/s) | 6.10 | 2.09 | **6.54** |
| 2000-2600 | yaw rate (rad/s) | 2.65 | 1.87 | **2.55** |
| 2000-2600 | turn radius (u) | 2.30 | 1.12 | 2.56 |
| n (dt*1000 / dt*3000) | | 332 / 541 | 86 / 71 | 12 / 43 |

Turn radius is a **length**. Its agreement (1.81 vs 1.86) is the unit check, and it
passes. So:

- **The world units match.** Residual (4) is closed, not deferred.
- **`dt*3000` is the correct time base** and reproduces the original's world speed,
  yaw rate and turn radius. `dt*1000` reproduces none of them.
- The shipped port is running the whole vehicle **~3x slow in world time**.

Caveat kept in view: the `dt*3000` cells rest on n = 12 and n = 43. They agree with
each other and with two independent metrics, but they are thin, and a re-measure on
a scenario that spends longer in those bands should confirm before this is treated
as settled.

## So what actually fails gate (a)

Not the physics. At the correct time base the car covers ground 3x faster, leaves
our drivable mesh, and respawns; gate (a) measures the median INTERNAL speed, which
the respawns floor to 33. The original capture it is being compared against is a
38 s full-lock circle — it never has to follow a track, so it cannot exhibit that
failure mode at all.

This means gate (a), as defined (median internal speed against the pre-change
control), is a "did the demo keep driving" guard rather than a fidelity measure
against the original. It was the correct guard for the earlier collapse (7.28 /
5.56 = physics dead). Here it fires while the physics is measurably *closer* to the
original on every quantity that can be compared to it.

**Decision deferred to the user rather than taken unilaterally**, since the session
brief set gate (a) as a hard stop. The options are to re-apply `dt*3000` and treat
the off-mesh/recovery path as the thing to fix, or to keep the 3x-slow shipped
state until that path is fixed first. What is NOT acceptable either way is a
compensating factor on the position term.

## Method note

The matched-internal-speed control is what turned this around. The first pass
compared world speed and yaw rate over unmatched speed distributions and produced
a turn radius of 2.82 for `dt*3000` (vs the original's 2.15), which looked like a
refutation. Controlling for speed gives 1.81 vs 1.86. Same recorded runs, opposite
conclusion — the same "control for speed before comparing a rate" lesson that this
project has now learned three times.

---

# Third follow-up, same day — off-mesh defect fixed, time base APPLIED

## The off-mesh defect (found, fixed, verified)

`TrackRenderer`'s off-mesh branch re-aimed velocity and heading toward the next
gate and multiplied `car_speed_` by 0.6 — **but never moved the car back onto the
drivable surface**. If the re-aim did not itself clear the boundary, the branch ran
again the next frame and the 0.6 **compounded**.

Confirmed numerically against `tb_A.txt`: speed goes 4198.28 -> 21.88, and
`ln(21.88/4198.28)/ln(0.6) = 10.3` — ten consecutive frames of decay with no
recovery, after which the race logic respawned the car. That decay-to-respawn loop
is what floored the gate-(a) median to 33.

It was latent at the old time base and became load-bearing at the corrected one,
where the car reaches the boundary 3x more often. **The bug was the missing
relocation, not the time base.**

Fix: relocate FIRST via `RecoverOffMesh` (which finds a verified on-mesh point and
moves the car there, so the branch cannot re-fire indefinitely), then re-aim toward
the gate only if a short probe along that chord is itself on-mesh, and do not damp
a second time.

**Tested alone, at the old time base, so the two changes are attributable
separately:** `om_A` / `om_B` medians 366.70 / 390.87 against the shipped band
373.61-390.86. No regression.

## Instrumenting heading discontinuities

The first combined run showed yaw-rate bands ABOVE the original's saturation
(4.408, 4.190) and a non-monotonic shape. Cause: an off-mesh re-aim is a heading
**discontinuity**, and the per-frame `|dBodyH|/dt` metric was counting it as a
rate. Rather than filter by eye, `VehiclePhysics_ResetOrientation` now sets a
per-slot flag that `MASHED_MOTION_DIAG` prints as `reseed=` and clears, so the
statistic can drop exactly the pairs that straddle one.

Only 9 / 18 pairs of ~1095 are dropped, but the effect is large: the 2000-3000 band
reads 4.351 including them and 2.534 excluding them, and `fin3_B`'s 0-500 band
reads 4.474 vs 0.359. Small count, large magnitude — the artifact class this
session keeps meeting.

## Gate (b): PASSES, and now matches the original

Yaw rate (rad/s), full lock, reseed-straddling pairs excluded:

| band | ORIGINAL | shipped (dt*1000) | fin3_A | fin3_B |
|---|---:|---:|---:|---:|
| 0-500 | 0.341 (230) | 0.132 | 0.405 (26) | 0.359 (36) |
| 500-1000 | 1.435 (149) | 0.438 | 1.246 (14) | 1.254 (17) |
| 1000-1500 | 2.312 (188) | 1.064 | 2.263 (36) | 2.268 (29) |
| 1500-2000 | 2.662 (332) | 1.286 | 2.550 (74) | 2.550 (69) |
| 2000-3000 | 2.652 (541) | 1.500 | 2.534 (153) | 2.550 (175) |
| 3000-5000 | -- | 0.850 | 2.550 (304) | 2.550 (273) |

Within 4-13% of the original in every band, against 50-70% low before. It saturates
at **2.550**, which is the theoretical value at budget 50 (`255*50*100*1e-4*3.334e-4*60
= 2.551`); the original's 2.662 is 4% higher, plausibly the `+0x144` accumulator,
not chased here.

## Gate (a): FAILS as literally defined, and there is a real new residual

| run | median | max | driving-only median |
|---|---:|---:|---:|
| ORIGINAL (`orig_steerR.msd`) | 687.91 | 2565.19 | **1760.29** |
| control (pre-velvec) | 390.90 | 3664.81 | 1577.15 |
| shipped velvec | 377.15 | 3459.32 | 1394.44 |
| dt*3000 WITHOUT om fix | 33.27 | 4322.49 | -- |
| **om fix + dt*3000 (fin3_A / fin3_B)** | **1519.62 / 1534.38** | 4330.59 | **2589.73 / 2491.55** |

Two readings, and they disagree — so both are given:

- **Against the pre-change control** (the gate as written): 1520 vs 390 is a 3.9x
  miss. FAILS. But that control is the 3x-slow port, so it is not a fidelity
  reference.
- **Against the original**: driving-only median 2590 vs the original's 1760 — our
  chain now over-accelerates by about **1.5x**. Before the change it was 1577 vs
  1760, i.e. 10% LOW. So this is a genuine regression on that measure, just not a
  3x one, and it is in the opposite direction from the old error.

Caveat on the comparison: the original capture is a 38 s full-lock circle and ours
is a race demo with varying steer, so the two speed DISTRIBUTIONS are not
like-for-like. The peak is: ours 4330, original 2565 here but 4275 / 4070 / 4344 in
the archived stock arm, so the peak is in family.

## Where that leaves it

Correcting the time base fixed the yaw and position KINEMATICS (gate b, world
speed, turn radius all now match) and exposed that the chain's FORCE integration
over-accelerates by ~1.5x when given the correct amount of simulated time. That is
a new, better-posed question than the one this session started with, and it is the
obvious next target: some part of A5 / A6a is carrying a compensating error that
the 3x-slow budget was masking.

State: applied, and NOT silently — gate (a) as written does not pass, and that is
flagged rather than reinterpreted away.

---

# Fourth follow-up — the over-acceleration is a DEFEATED GEARBOX SHIFT TIMER

## Reframing first: the "1.5x over-acceleration" was the wrong measure

The third follow-up called this a "~1.5x over-acceleration" from comparing
driving-only medians (port 2590 vs original 1760). That comparison is **unsound**:
the original capture is a 38 s full-lock circle and ours is a race demo, so the
two speed DISTRIBUTIONS differ for scenario reasons. Retracted as a headline.

The sound measure is **acceleration as a function of current speed, at matched
speed, under full throttle** — scenario-independent:

| current speed | ORIGINAL | port dt*1000 | port dt*3000 (A / B) |
|---|---:|---:|---:|
| 1-500 | 770.6 (230) | 377.7 | 1154.1 (76) / 1100.4 (92) |
| 500-1000 | 1696.3 (149) | 802.5 | 2298.3 (40) / 2298.3 (44) |
| 1000-1500 | 1629.3 (188) | 1104.6 | 3343.2 (70) / 3374.7 (64) |
| 1500-2000 | 881.4 (332) | 1048.8 | 3070.5 (144) / 3074.4 (139) |
| 2000-2600 | **351.4** (541) | 891.6 | **2604.0** (194) / 2576.4 (214) |

The original's acceleration **peaks at ~1700 then decays to 351** — a real
terminal-velocity shape. Ours never decays. The error is not a scale factor, it is
the **shape**: at 2000-2600 we are **7.4x** too high. It is wrong at both time
bases; the correct one just makes it visible.

## Two leads checked and killed first

- **Number of A4 calls per frame.** The cited dispatcher order has A4
  (`FUN_00470670` -> A6a) at step 3, OUTSIDE the substep loop, while our port calls
  it inside; at `kMaxSubstep=25` that became 2 calls/frame. Tested by forcing one
  50-unit chunk: driving-median 2701.54 vs 2589.73 / 2491.55 for two calls —
  slightly HIGHER, not lower. **Refuted.** (The structural mismatch is still real
  and worth fixing, but it is not this defect.)
- **The gated velocity drag** `(1 - dt*0.0167*0.01)*0.99` at `Integrate2.cpp:160`,
  gated on `+0x1f0 == -1 || == -0x373738`. Read `+0x1f0` straight out of the
  original capture: it is **-3637120 (0xffc88080)** and **-4947840 (0xffb48080)** —
  packed RGBA surface keys, matching **none** of the special literals. So the drag
  does not apply in the original either, and our `surfaceKey = 0` takes the same
  default branch. **Refuted**, and as a side effect this shows `surfaceKey = 0` is
  harmless for this comparison.

## The actual cause

The per-gear drive table at `+0x478` is `[4.0, 3.0, 2.4, 2.1, 1.8, 6.0]` and the
gearbox at `+0x490` (gear) / `+0x494` (shift timer) picks into it. Measured:

| | ORIGINAL | PORT |
|---|---|---|
| gear distribution (driving) | 0:246 1:305 2:371 3:338 4:181 | 0:39 1:54 2:18 3:11 **4:974** |
| median speed at gear 4 | 2392.5 | 3033.7 |
| speed at which gear 4 is reached | ~2400 | ~940 |
| shift timer `+0x494` | range **-2950 .. 2950** | **0 on all 1096 frames** |

`Integrate2.cpp:140` arms the lockout with `Wi(v, 0x494, 3000)` on an upshift, but
lines 145-146 then do:

```
if (Ri(v,0x494) > 0) { int r = Vc_RoundST0(); if (r < 0) r = 0; Wi(v, 0x494, r); }
```

and `Vc_RoundST0()` is a **stub returning 0** (`ForceIntegratorStubs.cpp:60`,
`FUN_004a2c48`, marked `[UNCERTAIN signature/input]`). So the 3000 is overwritten
with 0 on the very next frame and **the shift lockout never holds**. The port
upshifts every frame it can and is in top gear for **89% of driving frames**,
reaching gear 4 at speed ~940 where the original reaches it at ~2400.

Note 3000 / 50 units-per-frame = **60 frames = exactly 1 second** of shift lockout
at the corrected time base, which is a sane gearbox constant and corroborates both
the budget of 50 and this reading.

Mechanism for the sign: `fVar4b = +0x478[gear]` feeds the traction test
`bGrip = fVar3sel < ((local_cc * fVar4b) / fVar6) * gripIn`, and `bGrip` HALVES
`local_cc`. Stuck in top gear, `fVar4b` is at its smallest (1.8 vs 4.0), so the
product stays under `fVar3sel`, `bGrip` never fires, and the drive force is never
halved. Early upshifting therefore REMOVES a damping term rather than adding
torque — which is why the car over-accelerates while nominally in a lower-torque
gear.

## This is the project's named dominant defect class, third instance

A value that is live in MASHED and unset/stubbed in the standalone, silently
disabling a code path rather than faulting — same shape as `RwMatrixRotate`
(`9cc41fa8`) and the 83 zeroed constants (`53e5c05d`), which
`D2_REALPHYS_REMEASURE_2026-08-21.md` already calls the dominant class in this
port.

## Next step (NOT taken here — needs Ghidra, no guessing)

Decode `FUN_004a2c48` and implement the real timer decrement. The original's
observed range (-2950 .. +2950 against a 3000 arm) is consistent with a per-frame
countdown by the budget, but the exact operand and rounding are **[UNCERTAIN]** and
must come from the instruction stream, not from that inference. Do not substitute
a plausible `timer -= dt`.

---

# Fifth follow-up — FUN_004a2c48 decoded, shift timer fixed

## FUN_004a2c48 is `_ftol2`, not a game routine

CONFIRMED from the instruction stream (`re/analysis/data/A8_ftol_gearbox_timer_20260825.md`):
it is the MSVC CRT helper `_ftol`/`_ftol2` — x87 ST0 -> int64 in EDX:EAX, **truncate
toward zero**, no control-word change — with **154 callers image-wide** across startup,
frontend, menu and input. The vehicle code is 2 of those 154.

So every one of our 8 `Vc_RoundST0` / `Vc_InputFilter` call sites is simply an
`(int)` cast of a float expression, and the real content is the x87 value built
immediately before the CALL. Our stub returning 0 was zeroing eight computed values.

## The shift timer law, verbatim

```
004679bf  FILD dword [ESP+0x34]     ; ST0 = (float)(int)timer
004679c3  FSUB float [ESP+0xf8]     ; ST0 = timer - param_2   (param_2 = dt)
004679ca  CALL 0x004a2c48           ; EAX = (int)(timer - dt)
004679d1  MOV  [ESI+0x494],EAX      ; store, THEN clamp to 0 if negative
```
plus the mirrored `< 0` arm at `0x004679ed` / `0x004679f1` / `0x004679f8`. Arming
constants `0xbb8 = +3000` @`0x00467978`, `0xfffff448 = -3000` @`0x00467990`. The port's
assumed `> 0` / `< 0` guard and clamp-to-0 structure was **correct**; only the value was
missing. The step is exactly the `dt` argument — there is no literal 50 in the function.

**Independently corroborated before implementing**: the original's `+0x494` steps by
exactly **-50 per frame** (n=848 frames) and its observed max is **2950 = 3000 - 50**.
That is this law with dt summing to 50 per frame — a third independent confirmation of
the budget of 50, after the yaw law and the position law.

## Result

| | ORIGINAL | before fix | gt_A | gt_B |
|---|---|---|---|---|
| `+0x494` nonzero | 1371 / 1441 | **0 / 1096** | 889 / 1098 | 890 / 1097 |
| `+0x494` range | -2950 .. 2950 | 0 .. 0 | -2950 .. 3000 | -2950 .. 3000 |
| top-gear share | 12.6% | **89%** | 29% | 32% |

Gate (b), reseed-straddling pairs excluded — improved again and now within 4-9%:

| band | ORIGINAL | gt_A | gt_B |
|---|---:|---:|---:|
| 0-500 | 0.341 | 0.392 (23) | 0.392 (23) |
| 500-1000 | 1.435 | 1.348 (56) | 1.367 (30) |
| 1000-1500 | 2.312 | 2.111 (55) | 2.086 (57) |
| 1500-2000 | 2.662 | 2.505 (57) | 2.550 (58) |
| 2000-3000 | 2.652 | 2.550 (213) | 2.550 (208) |

Gate (a) moved toward the original: all-frames median **1520 -> 1087 / 1220** against the
original's 687.91. Still ~1.6-1.8x high.

## The remaining defect is NOT the gearbox

Acceleration vs current speed, full throttle:

| speed | ORIGINAL | before fix | gt_A | gt_B |
|---|---:|---:|---:|---:|
| 1-500 | 770.6 | 1154.1 | 1298.4 | 1298.4 |
| 500-1000 | 1696.3 | 2298.3 | 2028.0 | 1652.4 |
| 1000-1500 | 1629.3 | 3343.2 | 2106.9 | 2483.4 |
| 1500-2000 | 881.4 | 3070.5 | 2245.8 | 2214.9 |
| 2000-2600 | **351.4** | 2604.0 | **1579.2** | 1642.2 |

Better everywhere above 500, but the original's sharp high-speed **decay** (1696 -> 351)
is still absent: ours still rises to 1500-2000 and is 4.5x high at the top band.

Two candidates checked and eliminated:
- **the per-gear drive table `+0x478`**: read AT RUNTIME (not grepped — a literal-offset
  grep cannot see a dword-index write off a computed base, which has produced a false
  "no writer" claim in this project before). Ours is
  `[4.00, 3.00, 2.40, 2.10, 1.80, 6.00]` on all 1099 frames — **identical** to the
  original's. A3 writes it correctly.
- the gearbox itself, now that the timer holds.

So a force or damping term that decays with speed is still missing somewhere in A5/A6a.
That is the next target. The other six `_ftol2` sites are now known to be real computed
values our stub still zeroes (boost timer x3 at `0x00467cfe`/`0x00467dc6`/`0x00467e25`,
two per-wheel at `0x00467e7e`/`0x00467f93`, and the steer analog `+0xb24`/`+0xb28` at
`0x00470741`/`0x00470763`); `+0xb24` is nonzero on 1430/1441 original frames, so at least
that one is live. They were deliberately NOT fixed in the same change so the shift-timer
result stayed attributable.

---

# Sixth follow-up — the missing damping is LOCALIZED and QUANTIFIED

Not fixed. Localized to one block, with the size of the gap measured.

## The drive side is correct; the opposing side is ~1% of what it needs to be

`Integrate2` ends with `vel += linTerm * (ctrl + accum)` where `ctrl` is the drive
force accumulator `+0xb14/+0xb18/+0xb1c` and `accum` is `l_b8/l_b4/lin_b0`, the
friction / normal-load redistribution term. A 2026-08-22 note in that source already
said `accum` is "the ONLY place a velocity-opposing term can live". Confirmed.

**Drive force, ORIGINAL vs PORT, by speed band** (`|+0xb14..1c|`):

| band | ORIGINAL | PORT |
|---|---:|---:|
| 1-500 | 1328114 | 1318939 |
| 500-1000 | 2374970 | 1903897 |
| 1000-1500 | 2563372 | 3041779 |
| 1500-2000 | 3168173 | 3261542 |
| 2000-2600 | 3612376 | 3476483 |

Same magnitude, same rising trend. **The drive side is not the defect.** (This also
corrects a stale claim in `Integrate2.cpp` that `+0xb14` is speed-independent — it is
not: `local_cc` carries a `+ speed` term, and the original's own force rises 2.7x.)

## Backing the required opposing term out of the original's numbers

`linTerm = dt * Rf(v,0x54) * kDt` is genuinely constant: `+0x54 = 0.0010` on every
one of the original's driving frames, and `dt`/`kDt` are constant. So from
`accel/60 = linTerm * (ctrl + accum)`:

| band | orig accel | orig ctrl | implied accum | as % of ctrl |
|---|---:|---:|---:|---:|
| 1-500 | 770.6 | 1328114 | -249223 | -18.8% |
| 500-1000 | 1696.3 | 2374970 | 0 | 0.0% |
| 1000-1500 | 1629.3 | 2563372 | -282229 | -11.0% |
| 1500-2000 | 881.4 | 3168173 | -1934177 | **-61.1%** |
| 2000-2600 | 351.4 | 3612376 | -3120371 | **-86.4%** |

**Assumption stated:** `linTerm` was calibrated on the 500-1000 band by assuming
`accum` is small there, so that band reads 0 by construction. It is the band where
the original's acceleration peaks, which is where an opposing term should be
smallest relative to drive — but it is a choice, and the other rows inherit it.
Everything else here is measured.

**Our port's `accum`, same bands:** 30641 / 30641 / 30641 / 32818 / 33370 — a nearly
FLAT ~1% of the drive force, growing 11% while the drive force grows 3.7x.

So the term exists in our port but is roughly **two orders of magnitude short at
speed and does not scale with speed at all**. Closing that gap closes the whole
remaining acceleration error.

## What has been eliminated along the way

- gearbox / shift timer (fixed; timer now holds)
- per-gear drive table `+0x478` (read at runtime: identical to the original)
- A4 call count per frame (isolation-tested; refuted)
- gated velocity drag at `Integrate2.cpp:160` (`+0x1f0` is an RGBA surface key
  matching no gate literal on either side)
- `+0xb0c` engine-term input (original's median is 2-68, so `fVar5` only moves
  1484 -> 1432 across the whole speed range — a 3.5% effect)
- `+0x498` / `+0x49c` (constant 40000 / 4000 in the original)
- the drive force itself (matches the original, above)

## Next step

Decode the normal-load / friction redistribution block that produces
`l_b8 / l_b4 / lin_b0` (`Integrate2.cpp:370-382`, the `l_d0 - |l_78,l_74,l_70|` over
`l_d0` redistribution and whatever feeds `l_64/l_68/l_6c` and `l_70/l_74/l_78`)
against the original, and find which input is inert in the standalone. Given the
pattern this session has repeatedly hit, a stubbed or never-written input is the
first thing to check — and check it by READING IT AT RUNTIME, not by grepping an
offset.

---

# Seventh follow-up — friction block probed: one real gap found, three hypotheses refuted, mechanism NOT closed

Stopping point. What is established, what is refuted, and what is explicitly still open.

## METHOD CORRECTION FIRST

The sixth follow-up compared acceleration-vs-speed with the port filtered on **full
throttle only**, while the original capture is **100% full-lock cornering**. That is
an uncontrolled comparison. Re-run with full throttle AND full lock on both sides:

| band | ORIGINAL | port (throttle only) | port (throttle + lock) |
|---|---:|---:|---:|
| 1-500 | 770.6 | 1298.4 | 1441.2 (23) |
| 500-1000 | 1696.3 | 2028.0 | 2043.3 (56) |
| 1000-1500 | 1629.3 | 2106.9 | 1928.7 (58) |
| 1500-2000 | 881.4 | 2245.8 | 2013.3 (58) |
| 2000-2600 | **351.4** | 1579.2 | **1177.8 (123)** |

The top-band gap shrinks 4.5x -> **3.3x**, and the original's falloff (1696 -> 351,
4.8x) is still far steeper than the port's (2043 -> 1178, 1.7x). **The finding
survives the control**, but the earlier magnitudes were inflated.

## ESTABLISHED — the friction accumulator gap is real

`accum = c + (fTot - c)*frac`, `frac = (l_d0 - m78)/l_d0`. Reconstructed the SAME
blend from the original's record (per-wheel force `+0x214` etc., wheel stride 0xc4)
and measured ours with an extended, uncapped diag:

| band | ORIG \|accum\| | PORT \|accum\| | ORIG \|fTot\| | PORT \|fTot\| |
|---|---:|---:|---:|---:|
| 1-500 | 60920 | 30641 | 65683 | 32855 |
| 500-1000 | 65666 | 30641 | 100263 | 43757 |
| 1000-1500 | 90503 | 30641 | 156872 | 51974 |
| 1500-2000 | 128166 | 32818 | 198655 | 56776 |
| 2000-2600 | **152506** | **33370** | 220371 | 95900 |

The original's opposing term **rises 2.5x with speed**; ours is **flat**. Its input
`fTot` (the summed per-wheel force) is 2-3.5x short. This is a genuine defect and the
first concrete thing to fix.

## REFUTED (three, all by measurement)

- **grip-clamp #6 damps forward speed.** No — read from source, it is
  `vel -= lateral * k`, **lateral only**. It cannot produce a forward-acceleration
  falloff.
- **`grip` arrives too small, so the clamp never bites.** No — measured
  2287 -> 2.7e6 across the bands, and the `> 32768` "hi" arm is taken on 3017 of
  3282 samples (92%). The clamp is engaged.
- **`+0x18c` (the grip divisor) is wrong in the port.** No — it is exactly 1.0 on
  every frame in BOTH the original capture and the port.

## OPEN, AND THE HONEST PROBLEM WITH MY OWN ARITHMETIC

The sixth follow-up backed out a required opposing term of **-3.12e6** at 2000-2600
from `accel/60 = linTerm * (ctrl + accum)`. That model does **not** hold, and I am
retracting the figure as a quantity:

- Accounting for the force vector TURNING as the car corners (measured: the cosine
  between `+0xb14..1c` and the velocity direction falls 0.937 -> 0.700), the FORWARD
  PROJECTION of the drive force still rises 1.21e6 -> 2.52e6 while acceleration falls
  770 -> 351.
- Re-deriving on the projection, the needed opposing projection at 2000-2600 is
  about **-2.1e6**.
- But the largest opposing term reconstructible from the original's record is
  `|accum| = 152506`. Off by more than 10x.

So `accel = 60 * linTerm * (ctrl + accum)` cannot reproduce the original's own
numbers, which means one of its premises is wrong. The two candidates, neither
tested: (a) `linTerm` is not constant per frame as assumed (it is
`dt * +0x54 * kDt`, and `+0x54` IS constant at 0.0010, so this would have to be `dt`
varying); (b) `+0xb14` is zeroed and re-accumulated more than once per frame
(`PhysicsChainHooks.cpp:277` does zero it), so the end-of-frame capture snapshot is
NOT the value the integrator actually consumed — in which case every `ctrl` number in
this note is a per-frame residue rather than the integrated force.

**(b) is the more likely of the two and would invalidate the `ctrl` column above.**
Resolve it before doing any more arithmetic on these values.

## Next step

Ghidra on the A6a tail: establish how many times `+0xb14/+0xb18/+0xb1c` is zeroed and
re-accumulated per dispatcher call, and what `dt` A6a actually receives in the
original. Only then re-derive. The friction-accumulator gap (established above) can be
worked on independently and does not depend on that arithmetic.

---

# Eighth follow-up — +0xb14 cadence RESOLVED (my hypothesis refuted), A6b eliminated

## Provenance caveat, stated up front

The Ghidra child assigned this **hit its account's session limit before writing its
deliverable file or final table**. What follows is taken from its in-progress
messages, which do carry cited addresses. It is therefore **single-pass and
unreviewed** — lower-grade than the other decodes in this note. The two load-bearing
claims (A4 zeroes at start; A6a called once inside A4) are independently corroborated
by a runtime measurement below, which is why I am willing to act on them. Anything
here not so corroborated should be re-derived before it is relied on.

## The cadence: my hypothesis is REFUTED, the captures are USABLE

- **A4 (`FUN_00470670`) zeroes `+0xb14/+0xb18/+0xb1c` at its START** —
  `0x004706af / 0x004706b5 / 0x004706bb`, EBX = 0 — then calls `FUN_0046ddb0` (A5),
  then **`FUN_00467650` (A6a) exactly once**, then `FUN_00468980` (A6b).
- The only other writer is `FUN_0046baa0` @`0x0046bba0`, the per-player
  **initializer** (setup, not per-frame).
- **`FUN_004709a0` — the inner chunk called twice with 25.0 — does NOT call A4 or
  A6a.** It runs `FUN_0046e9e0` / `FUN_00469aa0`, i.e. the position/orientation and
  collision phase. So the substep loop never touches `+0xb14`.

Therefore `+0xb14` is zeroed once per frame, accumulated once, consumed once, and not
touched again. **A render-tick snapshot IS the force the integrator consumed. My
hypothesis was wrong and the captured figures in the sixth/seventh follow-ups are
USABLE.**

Independently corroborated in our own port by logging `+0xb14` at both points:

| band | consumption-time | post-substep-loop |
|---|---:|---:|
| 1000-1500 | 2609937 | 2608703 |
| 1500-2000 | 3235558 | 3235558 |
| 2000-2600 | 3436594 | 3441728 |

Identical to within noise, and 0 of 1098 post-step samples are zero.

## CONFIRMED STRUCTURAL MISMATCH in our port

The original calls **A4 once per frame** (and A5/A6a/A6b once each, inside it), while
`FUN_004709a0` runs **twice at 25** for position/orientation/contacts only. Our port
calls A4 *inside* its substep loop — **twice per frame at dt=25**. That is a real
fidelity error. (It was isolation-tested earlier and is NOT the over-acceleration
cause — one call/frame gave 2701 vs 2590 for two — but it should still be corrected,
and it means A6a's `dt` should be **50**, not 25.)

## A6b eliminated as the damping term

`Vehicle_AeroStabilize` (`FUN_00468980`) **returns immediately when
`+0x9e0 != 0.0`**, i.e. whenever the car is grounded — which it is on essentially
every frame of this scenario (`+0x9e0 == 4.0`). It is airborne ATTITUDE stabilization,
not drag. It cannot supply a speed-dependent forward damping term.

## Where that leaves the falloff — still open, and now better bounded

With the cadence confirmed and `dt = 50` pinned, `linTerm = 50 * 0.0010 * 3.334e-4 =
1.667e-5`. Re-deriving on the force's forward projection at 2000-2600:
required opposing projection ≈ **-2.17e6**, against a largest reconstructible
opposing term of **152506**. The gap survives every correction made so far.

Eliminated to date: gearbox/shift timer, per-gear table, A4 call count, the gated
velocity drag, `+0xb0c`, `+0x498`/`+0x49c`, the drive force itself, grip-clamp #6
(lateral-only), `grip` magnitude, `+0x18c`, the `+0xb14` cadence, and A6b.

**The one place not yet examined is A5 (`FUN_0046ddb0`), which runs BEFORE A6a inside
A4 and is entirely outside the model I have been testing.** If it writes `+0x9b0..`
directly, the whole `accel = 60*linTerm*(ctrl+accum)` framing is incomplete rather
than wrong. That is the next thing to check, and it should be checked FIRST by asking
whether A5 writes the velocity triple at all.

---

# Ninth follow-up — RESOLVED: there is no missing damping term. The damping is
# present and correct; what is missing is SLIP.

## The chain, every link measured

`grip-clamp #6` at the tail of A6a does `vel -= lateral * k`, where
`lateral = vel - (vel.fwd)fwd`. It is lateral-only — which is why it looked like it
could not damp forward speed. **But removing the lateral component reduces `|vel|`,
and `|vel|` IS `+0x9e4`, the speed being measured.** So in a corner it is a large
speed-limiting term.

Measured lateral fraction `|lateral| / |vel|` in the ORIGINAL, by speed band:

| band | lateral fraction | slip (rad) |
|---|---:|---:|
| 1-500 | 0.3862 | 0.3965 |
| 500-1000 | 0.0652 | 0.0652 |
| 1000-1500 | 0.0814 | 0.0815 |
| 1500-2000 | 0.1913 | 0.1925 |
| 2000-2600 | **0.2472** | **0.2498** |

**PORT: a flat ~0.037** (slip mean 0.0366 rad at full lock, speed > 500).

And `k` was measured at 0.70-0.80 (the "hi" arm, taken on 92% of samples). So per
frame the original sheds roughly `0.247 * 0.75 = 18.5%` of its speed in a hard corner,
while the port sheds `0.037 * 0.75 = 2.8%` — **6.6x less**. That is the entire
acceleration-falloff gap, and it is enough to account for it.

## Conclusion

**There is no missing force or damping term.** grip-clamp #6 is present, correctly
ported, correctly parameterised (`+0x18c` = 1.0 in both; `grip` reaches 2.7e6 and
takes the hi arm 92% of the time), and it IS the speed-limiting mechanism. It simply
has almost nothing to bite on, because **our car's slip angle is ~6.7x too small at
speed** (0.037 rad vs the original's 0.250).

This closes back onto the session's own premise. The port previously could not
represent slip AT ALL, and this session made slip representable — but it is still an
order of magnitude too small. The body rotates correctly (yaw rate now within 4-9% of
the original) and the world speed and turn radius match, so the ORIENTATION half is
right; what is wrong is that the VELOCITY re-aligns to the body too fast. In physical
terms our lateral grip is too high, so the car never slides, so the lateral bleed never
engages, so it never loses speed in corners.

## Restating the open defect properly

Not "a missing speed-dependent damping term in A5/A6a". It is:

> **The lateral grip is too strong: slip angle reaches ~0.037 rad at speed where the
> original reaches ~0.250. Fixing that engages the existing, already-correct
> grip-clamp #6 and should close the acceleration-falloff gap with it.**

That is a much better-posed target, it lives in the A6a per-wheel lateral-force block
(the same `l_60` grip accumulator path, whose `fTot` input was independently measured
2-3.5x short in the seventh follow-up — consistent with under-sliding), and it is
where the next session should start.

## Everything eliminated in this hunt

gearbox / shift timer (fixed), per-gear drive table, A4 call count, the gated velocity
drag, `+0xb0c`, `+0x498`/`+0x49c`, the drive force magnitude, the `+0xb14`
accumulation cadence (hypothesis refuted; captures confirmed usable), A6b (returns
immediately when grounded), A5 (never writes the velocity triple), `grip` magnitude,
`+0x18c`, and finally grip-clamp #6 itself — which turned out to be correct all along.

---

# Tenth follow-up — A4 CADENCE FIXED. Slip, falloff and speed all move to the original.

## The change (structural, cited, not tuned)

A4 (`FUN_00470670`) was being called INSIDE our substep loop — at the corrected
budget, **twice per frame at dt=25**. The original calls it **once per frame** at
step 3 of `FUN_00470c70`, outside the loop: it zeroes `+0xb14/18/1c`
(`0x004706af/b5/bb`), then runs A5, A6a **exactly once**, and A6b. The substep loop
`FUN_004709a0` (2 x 25) runs `FUN_0046e9e0` / `FUN_00469aa0` and never calls A4 or A6a.

A4 now runs once per frame with the full budget, after the substep loop.

**Why it was load-bearing, not bookkeeping:** grip-clamp #6 lives at the tail of A6a
and does `vel -= lateral * k`. Calling A4 twice applied that lateral bleed TWICE per
frame, which crushed slip — and slip is what the bleed needs in order to limit speed.

## Results (two runs, both reported)

**Slip at full lock** — the defect this was chasing:

| band | ORIGINAL | A4 x2 (before) | a4x_A | a4x_B |
|---|---:|---:|---:|---:|
| 1000-1500 | 0.0815 | 0.0270 | 0.0599 | 0.0587 |
| 1500-2000 | 0.1925 | 0.0452 | 0.1344 | 0.1299 |
| 2000-2600 | **0.2498** | **0.0547** | **0.2084** | **0.2011** |

From **4.6x short to 1.2x short.**

**Acceleration vs speed** (full throttle AND full lock on both sides) — the falloff
that was entirely absent is now present:

| band | ORIGINAL | before | a4x_A | a4x_B |
|---|---:|---:|---:|---:|
| 1-500 | 770.6 | 1441.2 | 1413.6 | 1414.8 |
| 500-1000 | 1696.3 | 2043.3 | 1591.5 | 1594.2 |
| 1000-1500 | 1629.3 | 1928.7 | 2326.8 | 2364.3 |
| 1500-2000 | 881.4 | 2013.3 | 1702.8 | 1812.6 |
| 2000-2600 | **351.4** | **1177.8** | **457.2** | **537.6** |

Both now peak mid-range and decay hard. Top band from **3.4x high to 1.3-1.5x high**.

**Gate (b) yaw rate** — held, slightly better: 2.170 / 2.527 / 2.550 against the
original's 2.312 / 2.662 / 2.652.

**Gate (a)** — moved toward the original: driving-median 2162.80 -> 1936.50 / 2044.31
against the original's 1760.29. Max 4142 / 4134 (was 4311); the original's archived
stock arm peaks at 4275 / 4070 / 4344, so still in family.

## How the diagnosis got here, including the wrong turn

The equilibrium-slip model `slip ~= (w*dt)/k`, with `k = (1e7 - grip)*9.9998e-8` and
`grip = l_60 * speed`, validated on our own port (predicted vs measured ratio 1.03 /
0.97 / 0.86 in the bands >= 1500). Applying it to the original said we needed `grip`
about 4.2x larger, i.e. `l_60` about 4.2x larger.

**That would have been the wrong fix.** Closing the loop (`slip -> ld4 -> l_60 ->
grip -> k -> slip`) showed a fixed point of ~0.05-0.09 for ANY input slip, including
0.40 — the loop self-limits, so no amount of extra `l_60` reaches 0.25. `le4` is also
pinned at its 1024 cap above speed 1000, leaving no headroom there either. The
negative result is what redirected the search to "something outside this loop applies
the bleed too often", which is exactly what the doubled A4 call was.

## Still open

- Acceleration is 1.3-2x high across the bands; slip is 1.2x short. Both are now the
  same order as the original rather than a different one.
- Six `_ftol2` sites still stubbed to 0 (boost timer x3, two per-wheel, steer analog
  `+0xb24` — the last is nonzero on 1430/1441 original frames).
- `fTot` (summed per-wheel force) measured 2-3.5x short in the seventh follow-up;
  worth re-measuring now that the cadence is right, since that measurement was taken
  under the doubled-A4 regime and may have moved.

---

# Eleventh follow-up — fTot/accum re-measured: the "flat accum" defect was an ARTIFACT

The seventh follow-up's headline finding — "the friction accumulator is nearly FLAT
where the original's rises 2.5x, and is ~1% of the drive force" — was measured under
the doubled-A4 regime. **It does not survive the cadence fix and is superseded.**

Re-measured with A4 running once per frame (`friction_C_a4x.log`, 1097 samples = one
per frame, which independently confirms the cadence — it was 3 per frame before):

| band | fTot ORIG | fTot PORT | x | accum ORIG | accum PORT | x |
|---|---:|---:|---:|---:|---:|---:|
| 1-500 | 65683 | 33071 | 1.99 | 60920 | 31008 | 1.96 |
| 500-1000 | 100263 | 57048 | 1.76 | 65666 | 36596 | 1.79 |
| 1000-1500 | 156872 | 98690 | 1.59 | 90503 | 53909 | 1.68 |
| 1500-2000 | 198655 | 125396 | 1.58 | 128166 | 74250 | 1.73 |
| 2000-2600 | 220371 | 158905 | **1.39** | 152506 | 106736 | **1.43** |

Previous shortfalls, for comparison:
`fTot` 2.0 / 2.3 / 3.0 / 3.5 / 2.3 and `accum` 2.0 / 2.1 / 3.0 / 3.9 / 4.6.

**The shape defect is gone.** Our `|accum|` now RISES with speed —
31008 -> 36596 -> 53909 -> 74250 -> 106736, a 3.4x rise against the original's 2.5x —
where it was flat at 30641 -> 33370. The blend fraction also tracks now
(0.829 / 0.398 / 0.374 / 0.443 / 0.555 against 0.826 / 0.205 / 0.287 / 0.475 / 0.556;
the first and last bands are near-exact).

What remains is a roughly uniform **1.4-2.0x** shortfall in `fTot`, tightening as
speed rises. That is an ordinary magnitude gap in the per-wheel force, not the
order-of-magnitude wrong-shape defect the seventh follow-up described.

## Method note

This is the second time in this session that a defect "found" under a broken regime
dissolved once the regime was fixed — the first being the acceleration falloff itself.
Both were measured carefully and reported honestly at the time; what made them
misleading was the surrounding state, not the measurement. **Re-measure prior findings
after any structural change, before building on them.** The stale figures have been
struck through in `Integrate2.cpp`'s source comment rather than deleted, so the
retraction is visible at the call site.

---

# Twelfth follow-up — the fTot gap localizes to ONE field: p[0x1b] is 26-40x short

## Every other input to the block-#4 force matches EXACTLY

`Integrate2.cpp:284/299/308` builds the per-wheel suspension force from
`p[0x15] * p[0x1b] * g_suspScale * le4 * 0.0009766` (and the `p[0x16] * p[0x1b] *
g_suspScale` inner arm when `p[-1] == 0`). Measured, port vs original, speed > 1000:

| field | PORT | ORIGINAL | ratio |
|---|---:|---:|---:|
| `g_suspScale` | 21575.7 | 21580 | **1.000** |
| `p[0x15]` (+0x54) | 0.15 | 0.15 | **1.000** |
| `p[0x16]` (+0x58) | 0.0125 | 0.0125 | **1.000** |
| `p[-1]` flags (-0x04) | 0 | 0 | match |
| `p[0x1b]` w0 (+0x210) | 41.22 | 1091 | **0.038** |
| `p[0x1b]` w1 (+0x2d4) | 37.25 | 1081 | **0.034** |
| `p[0x1b]` w2 (+0x398) | 32.95 | 1086 | **0.030** |
| `p[0x1b]` w3 (+0x45c) | 13.24 | 535.9 | **0.025** |

(`g_suspScale` is not in the record; the original's is derived as
`3000 / (50 * 0.0027809) = 21580` and ours is measured — they agree to 0.02%, which
also confirms the corrected `frameMs = 50` reaches this global.)

**`p[0x1b]` — the per-wheel suspension load — is the only discrepancy, and it is
26-40x.** Note the FRONT/REAR PATTERN IS PRESERVED: the original's rear wheel 3 is
~0.49 of wheel 0 (535.9 / 1091); ours is ~0.32 (13.24 / 41.22). Same shape, uniform
scale error, so this is a missing factor rather than a broken computation.

It is dynamic on both sides (the original shows 1036-1051 distinct values over 1061
frames), so something writes it every frame.

## Why the fTot gap is only 1.4-2.0x despite a 26-40x input error

Not established. `fTot` is the sum of `p[0x1c..0x1e]`, which A5 (`FUN_0046ddb0`) also
writes, so the block-#4 suspension term is evidently not what dominates it. Do NOT
assume fixing `p[0x1b]` scales `fTot` by 30x — it plainly would not, or `fTot` would
already be 30x short. The relationship has to be measured, not inferred.

## Next step

Find what writes `p[0x1b]` (record `+0x210` / `+0x2d4` / `+0x398` / `+0x45c`) in the
original and compare against our writer. **Find it by reading the field at runtime and
by Ghidra, NOT by grepping the offset** — an index-grep across
`mashedmod/src/mashed_re/` finds only READS of `0x1b` (Integrate2, PhysicsChainHooks,
WheelContactSolver) and no writer, which is exactly the false-negative shape this
project has hit before: the wheel block is addressed as `rec+0x170` with stride
`0x31` floats in `VehicleInit`, so `p[0x1b]` there is `w[0x28]`, a completely
different literal.

---

# Thirteenth follow-up — all seven `_ftol2` sites ported

`FUN_004a2c48` is `_ftol2`, so every site is an `(int)` cast of a float expression and
the stub's `return 0` was zeroing seven real values. The gearbox timer was fixed in the
fifth follow-up; the remaining six (seven call sites — the child found one more `+0xbf4`
call than expected) are now ported verbatim from
`re/analysis/data/A8_ftol_gearbox_timer_20260825.md` Q4:

| site | receives | expression |
|---|---|---|
| `0x00467cfe` | `+0xbf4` boost timer | `(int)((float)+0xbf4 - dt)`, clamp `[0,3000]` |
| `0x00467e7e` | `p[0]` per-wheel spin | `(int)( dot(p[0x1f..0x21], vel) / (p[-0xa] * 1.745329) )` |
| `0x00467f93` | `p[-1]` brake branch | `(int)( (float)input[5] + 256.0 )` |
| `0x00470741` | `+0xb24` steer hold | `(int)((float)+0xb24 + dt)`, reset to 0 when `input[0]==0` |
| `0x00470763` | `+0xb28` steer hold | `(int)((float)+0xb28 + dt)`, reset to 0 when `input[1]==0` |

New constants: `_DAT_005cea24 = 0x3fdf6715 = 1.745329`,
`_DAT_005cea20 = 0x43800000 = 256.0`.

**The two steer sites matter most.** `+0xb24/+0xb28` are HOLD-DURATION counters, not
"filtered analog inputs" as the old stub name implied. They feed
`force = (f + 6000) * force * kGripMul` (clamped at `_DAT_005ceaa4 = 6000`), so with
the stub returning 0 that term was pinned at its floor and the wheel steer angle
never ramped with hold time. Corroborated against the original: `+0xb24` is nonzero on
1430 of 1441 driving frames (0..254, 128 distinct), and `+0xb28` is 0 throughout that
capture because it holds RIGHT lock only — which is exactly the reset rule.

Two `+0xbf4` branches (`bf8==1` / `bf8==2`, calls `0x00467dc6` / `0x00467e25`) remain
untranscribed in our port — they were already marked "shape only" before this session
and are unchanged. The original shows `+0xbf4` nonzero on only 9 of 1441 frames, so
this is low-impact, but it is still an open gap.

## Result (two runs)

| metric | before (a4x) | ftol_A | ftol_B | ORIGINAL |
|---|---:|---:|---:|---:|
| gate (a) driving-median | 1936.50 / 2044.31 | **1830.70** | **1806.16** | **1760.29** |
| gate (a) all-median | 1081.96 / 1110.02 | 928.53 | 861.80 | 687.91 |
| gate (a) max | 4142.73 | 4101.42 | 4029.17 | (stock 4275/4070/4344) |
| yaw 1000-1500 | 2.170 | 2.072 | 2.073 | 2.312 |
| yaw 1500-2000 | 2.527 | 2.550 | 2.550 | 2.662 |
| slip 2000-2600 | 0.2084 | 0.1948 | 0.1942 | 0.2498 |

**Gate (a) driving-median goes from 10-16% high to 3-4% high** — the clearest single
improvement to that metric this session. Gate (b) holds. Slip is unchanged within
noise at the top bands.

**One regression, stated:** slip in the 1000-1500 band drops 0.0599 -> 0.0314 against
the original's 0.0815. The other bands are unaffected. Not investigated; flagged.

All five ported expressions are verbatim from the instruction stream with no fitted
constants, so this is a fidelity correction whose net effect happens to be positive
rather than a tuning change.

---

# Fourteenth follow-up — the 1000-1500 slip regression, attributed

Bisected by disabling ONLY the two steer hold-duration counters and keeping the other
three `_ftol2` fixes:

| run | slip 1000-1500 | slip 1500-2000 | slip 2000-2600 | gate (a) driving-median |
|---|---|---|---|---:|
| ORIGINAL | 0.0815 | 0.1925 | 0.2498 | 1760.29 |
| a4x_A — no `_ftol2` fixes | 0.0599 (n=108, gear 2) | 0.1344 | 0.2084 | 1936.50 |
| bisect — 3 fixes, steer counters OFF | 0.0568 (n=105, gear 2) | 0.1274 | 0.2075 | 1897.74 |
| ftol_A — all 5 fixes | **0.0314** (n=158, gear 1) | 0.1316 | 0.1948 | **1830.70** |

**The steer hold counters own both effects.** The other three fixes (boost timer,
per-wheel spin, per-wheel brake) move gate (a) only 1936 -> 1898 and leave slip
essentially unchanged. Turning the counters on moves gate (a) 1898 -> 1831 (toward the
original's 1760) and drops the 1000-1500 slip 0.057 -> 0.031.

Also eliminated as a cause: the `p[-1]` brake-branch site. Logged at runtime, our
`p[-1]` is **0 on all four wheels** — exactly matching the original — so that branch
never executes in this scenario and the change is inert here.

## Verdict: KEEP the counters

They are a verbatim port of real behaviour, and the original's own record confirms
they are live (`+0xb24` nonzero on 1430 of 1441 driving frames, 128 distinct values).
Un-porting correct behaviour to protect one band's number would be exactly backwards.

Note also that the affected band's GEAR changes with the counters (median 2 -> 1),
i.e. part of the "regression" is the car occupying that speed band at a different
point in its shift sequence — a regime shift inside the band, not purely a physics
delta. The composition is otherwise identical (100% full throttle, 0% brake, 100%
all-grounded in every run).

So this is a **trade, not a defect introduced**: a correct port improved the
speed metric and exposed that slip in the mid band is under-produced. That under-
production is the same open defect the twelfth follow-up localized to `p[0x1b]`, and
it should be fixed there rather than compensated here.

---

# Fifteenth follow-up — p[0x1b] traced to `_DAT_0088e610`, and a CIRCULAR-VERIFICATION retraction

## RETRACTION: "g_suspScale matches the original to 0.02%" was CIRCULAR

The twelfth follow-up listed `g_suspScale` as PORT 21575.7 vs ORIGINAL 21580, ratio
1.000. **That was not a comparison against the original.** `_DAT_0088e610` /
`_DAT_0088e5f0` are GLOBALS, not record fields, so they are not in the capture — the
"original" column was **my own recomputation of the same formula our port uses**
(`3000 / (50 * 0.0027809)`). Comparing our output against my recomputation of our
input proves nothing. Retracted.

## The chain, from the child's decode plus measurement

`p[0x1b] = p[0x1a] * p[0x18]`, and `p[0x18] = 1.0` (`0x3f800000`, set once at
`FUN_0046b540` `0x0046bb30`, +0xc4 per wheel). Confirmed independently from the
capture: the original's `p[0x1b]/p[0x1a]` ratio is **0.9995 on all four wheels**.

`p[0x1a] = fVar4 +/- fVar4/3` (`_DAT_005ccac8` = 1/3), where
`fVar4 = (record+0x50 / grounded_count) * _DAT_0088e610`.

Measured inputs, port vs original:

| input | PORT | ORIGINAL | match? |
|---|---|---|---|
| `+0x50` (mass) | 1000.0 | 1000.0 | **yes** |
| `grounded_count` | 4 | 4 | **yes** |
| `p[0x18]` | 1.0 | 1.0 | **yes** |
| `p[0x1a]` w0 | 39.67 | 1091.56 | **27.5x short** |

Since every other input matches, the whole discrepancy is `_DAT_0088e610`:

| | suspDtTerm | suspScale = 3000/it |
|---|---:|---:|
| ours (`50 * 0.0027809`) | 0.139045 | 21575.7 |
| implied by the original's `p[0x1a]` | **3.274678** | **916.1** |
| ratio | **23.55x** | 1/23.55 |

## This ANSWERS the twelfth follow-up's open question

It asked why `fTot` is only 1.4-2.0x short when `p[0x1b]` is 26-40x short. Because
the block-#4 force is

```
p[0x15] * p[0x1b] * g_suspScale * le4 * 0.0009766
```

`p[0x1b]` is PROPORTIONAL to `suspDtTerm` while `g_suspScale` is `3000/suspDtTerm`.
**The two errors are reciprocal and cancel in that product.** So a 23.5x error in
`_DAT_0088e610` is invisible in this force and shows up only where `p[0x1b]` or
`g_suspScale` is used WITHOUT its partner.

That also means fixing `p[0x1b]` alone would BREAK the force by 23.5x. The two must
move together, i.e. the fix is to `_DAT_0088e610` itself, not to `p[0x1b]`.

## Status: INFERRED, not measured — confirm before acting

`_DAT_0088e610` is a global and is not in the capture. The 3.2747 above is derived
from the child's formula + measured `p[0x1a]` + measured `+0x50` + grounded_count 4.
Every ingredient except the formula is measured, but the formula is single-pass and
the result must be confirmed in Ghidra: **what does the ORIGINAL actually multiply to
build `_DAT_0088e610`?** Our port uses `frameMs * _DAT_005cea80` on the authority of a
2026-06-17 note, and 3.2747 / 0.0027809 = 1177.6, which is not the budget of 50 — so
either the multiplicand or the multiplier is wrong in that note.

Do NOT change `_DAT_0088e610` on the strength of this inference alone.

---

# Sixteenth follow-up — the writer is A5; the child's refutation rests on MY circular claim

## What the decode establishes (independent of my premises)

`re/analysis/data/A8_p1b_writer_20260826.md`:

- **Q1 — one writer**: A5 `FUN_0046ddb0`, `FST [ESI+0x1c]` at `0x0046e872`, i.e.
  `p[0x1b] = p[0x1a] * p[0x18]`, clamped to 0 if the product is negative
  (`0x0046e882`). Per-wheel, stride `ADD ESI,0xc4`, 4x (loop `0x0046e761`/`0x0046e981`).
  There is no literal-offset writer for wheels 1-3 — exactly the false-negative shape
  I warned about.
- **Q2 — live once per frame per wheel**, order `A4 -> A5 (writes) -> A6a (reads)`.
  The substep loop never calls it. Consistent with the tenth follow-up.
- **Q3 — fuller formula than I had**:
  `p[0x1a] = (record+0x50 / grounded_count) * _DAT_0088e610 +/- clamp(vel-proj, +/-1/3)`
  (`0x0046e3f6`-`0x0046e473`). The `+/-` term is a CLAMPED VELOCITY PROJECTION, not a
  flat 1/3 — my fifteenth-follow-up arithmetic used the flat form, so the 3.2747
  figure is order-of-magnitude only.
- **Q5**: `p[0x1b]` is read ONLY in A6a, 3 sites. **No second consumer**, so there is
  no independent way to pin its units.

## The Q4 refutation does NOT stand, and that is my fault

Q4 refutes `_DAT_0088e610` on the grounds that "its initializer ties it to the
already-matching `_DAT_0088e5f0` by `x * recip = 3000.0`, so matching one guarantees
the other." That is sound reasoning from a false premise: **I put "the global
suspension scale [matches] to 0.02%" into the child's prompt as established fact**,
and the fifteenth follow-up had already retracted exactly that claim as circular.

So the child's argument reduces to "suspScale matches (per Mariano), therefore
suspDtTerm matches" — which is my own circular claim laundered through a subagent.
**This is the THIRD time today I have handed a child one of my own unverified
inferences phrased as fact.** The other two were the `+0x928` boundedness and the
`+0xb14` cadence. It is now clearly a systematic error on my part, not bad luck.

## And Q4's proposed candidates are refuted BY MEASUREMENT

Q4 names `record+0x50` as the prime candidate ("unverified") and `record+0x9e0` as
secondary. Both are record fields and both ARE in the capture, and I measured them:

| candidate | PORT | ORIGINAL | verdict |
|---|---|---|---|
| `record+0x50` | 1000.0 | 1000.0 (distinct=1) | **matches — refuted** |
| `record+0x9e0` grounded count | 4.0 | 4.0 (all frames) | **matches — refuted** |
| `p[0x18]` | 1.0 | 1.0 | matches — refuted (both agree) |

## Where that actually leaves it

Every factor in `p[0x1a]` that CAN be measured from the record matches. The only
unmeasured one is `_DAT_0088e610`, and the sole argument against it is circular.
So it remains the candidate by elimination — but **nobody has measured it on either
side**, and I will not assert it again on inference.

**The next step is a different technique, not another static pass:** read the globals
`0x0088e610` / `0x0088e5f0` out of a LIVE original with Frida during a driving
scenario. They are not in the record capture, which is why this has stayed open. Until
that number exists, `p[0x1b]` stays open and nothing should be changed.

Also worth re-deriving once that lands: the fifteenth follow-up's 3.2747 assumed the
flat `+/- 1/3` form, and Q3 shows the real term is a clamped velocity projection.

---

# Seventeenth follow-up — the suspension globals MEASURED, and the p[0x1b] gap is INERT

## First non-circular measurement

`_DAT_0088e610` / `_DAT_0088e5f0` are globals, not record fields, so the statediff
capture could never see them — every prior check compared our computed value against a
recomputation of our own formula. Read them out of a **live original** instead, by
extending `scenario_launch.py`'s existing 10 Hz telemetry (additive; the addition is
kept as a permanent harness improvement). 80 driving samples:

| | ORIGINAL (measured) | PORT (computed) | ratio |
|---|---:|---:|---:|
| `suspDtTerm` `_DAT_0088e610` | **4.33337** | 0.139045 | **31.17x** |
| `suspScale` `_DAT_0088e5f0` | **692.302** | 21575.7 | **1/31.17** |
| product | **3000.0000** | 3000 (by construction) | 1.000 |

Both are CONSTANT over the run (distinct=1), so `suspDtTerm` is not a per-frame
wall-time product on the original either. Cross-checks in the same run confirm the
regime matches the archived capture: `wheel0Load` 1086.83 (capture: 1091.56) and
`mass` 1000.0.

So the p[0x1b] trail ends where the fifteenth follow-up predicted — `_DAT_0088e610`,
31.17x — and the sixteenth follow-up's refutation of that candidate is now formally
dead, as expected once its circular premise was removed.

## But the discrepancy is BEHAVIOURALLY INERT

`p[0x1b]` is proportional to `suspDtTerm`, and `suspScale` is `3000 / suspDtTerm`.
The child established `p[0x1b]` is read at exactly three sites, all in A6a, and all
three multiply it by `g_suspScale`:

```
Integrate2.cpp:284  p[0x15] * p[0x1b] * g_suspScale * le4 * 0.0009766
Integrate2.cpp:299  p[0x16] * p[0x1b] * g_suspScale
Integrate2.cpp:308  p[0x1b] * 3 * g_suspScale * (int)l94 * 0.0019531 * 1.1
```

The two errors are exact reciprocals and **cancel in every consumer** — measured
product 3000.0007 (original) vs 2999.9932 (port). So the 26-40x `p[0x1b]` gap that the
twelfth follow-up opened has **no effect on the force**, and fixing it alone would have
broken that force by 31x.

**This closes the p[0x1b] line as a non-defect for behaviour**, while recording that
our record CONTENTS differ from the original's by 31x on that field — which matters
for any future record-level diff, and for any consumer outside A6a.

## What is NOT established

The FORMULA. `4.33337 / 0.0027809 = 1558.3`, which is not the 50-unit budget, so the
2026-06-17 note (`suspDtTerm = frameMs * _DAT_005cea80`) does not produce the
original's value and at least one of its two factors is wrong. I am NOT hardcoding
4.33337: it is a measured value of a derived global, and without the formula it would
be a fitted constant that silently breaks at any other frame rate.

Next: decode what the original multiplies to build `_DAT_0088e610` (its writer, and
where the 1558.3-ish multiplicand comes from). Low priority given the cancellation —
this is now a fidelity/record-contents issue, not a behavioural one.

---

# Eighteenth follow-up — formula decoded, applied, and the cancellation CONFIRMED by experiment

## Two bugs, both cited

**1. A wrong decimal gloss.** `_DAT_005cea80` was recorded in our source as
`0x3b360bc0 = 0.0027809`. The hex is right; the decimal is not. Read straight out of
the PE's `.rdata`, `0x3b360bc0 = 0.00277780` (= 1/360), and `0.0027809f` compiles to
`0x3b363fc3` — **different bits**. This is the documented "plate hex gloss" trap.
`kSuspDtK` corrected to `0.0027778f`. Effect: 0.11%, within run-to-run noise, as
expected.

**2. The wrong parameter.** Decoded (`re/analysis/data/A8_suspdt_formula_20260826.md`):

```
FUN_00470c70:  FLD [EAX] ; FMUL [0x005cea80] ; FSTP [0x0088e610]   @0x00470f28
               FLD 3000.0 ; FDIV [0x0088e610] ; FSTP [0x0088e5f0]  @0x00470f3a
```

`[EAX]` is **`*param_2`** = `DAT_00803324`, a hardcoded float literal `0x44C30000` =
**1560.0** written at `0x0040d3d2` in the course-load path `FUN_0040d270` (sole
writer). Our port was passing **`param_1`, the 50-unit frame budget** — two different
dispatcher parameters, conflated.

`1560 * (1/360) = 13/3`, float32 `0x408aaaf3` = **4.33336782**, an exact bit match to
the value measured live. `1560/50 = 31.20` = the measured 31.17x.

## Result: record contents fixed, behaviour untouched

| | before | after (A / B) | LIVE ORIGINAL |
|---|---:|---:|---:|
| `p[0x1a]` (+0x20c) wheel 0 | 39.67 | **1088.31 / 1088.31** | **1086.83** |

**0.14% from the original**, closing a 27x fidelity error.

And the behavioural prediction held:

| run | slip 1000-1500 | 1500-2000 | 2000-2600 | gate (a) |
|---|---|---|---|---:|
| before (gloss_A / ftol_A) | 0.0325 / 0.0314 | 0.1290 / 0.1316 | 0.2013 / 0.1948 | 1769.26 / 1830.70 |
| after (susp_A / susp_B) | 0.0309 / 0.0302 | 0.1308 / 0.1307 | 0.1933 / 0.1939 | 1735.67 / 1776.79 |
| ORIGINAL | 0.0815 | 0.1925 | 0.2498 | 1760.29 |

Every number is inside the run-to-run spread. **A 27x correction to the record produced
zero behavioural change** — which is exactly what the seventeenth follow-up's
reciprocal-cancellation analysis predicted, and is now confirmed by experiment rather
than by argument.

Gate (a) across the last four runs: 1830.70 / 1769.26 / 1735.67 / 1776.79, mean ~1778
against the original's 1760.29 — **within 1%**, straddling it.

## Method note

This is the first time this session that a prediction was made BEFORE the measurement
and then held. The prediction was written into the source comment as a falsifiable
claim ("if a gate moves, the cancellation analysis is wrong") before the run. Worth
repeating: it converts a plausible-sounding argument into something the run can kill.

---

# Nineteenth follow-up — re-measured; my equilibrium model is DEAD; gap restated

## Re-measuring first was right: every prior number was stale

The grip figures the ninth/twelfth follow-ups reasoned from predate the A4-cadence fix,
the seven `_ftol2` ports and the `suspDtTerm` correction. Re-measured on the current
build:

| band | l_60 (was -> now) | ld4avg (was -> now) | grip (was -> now) |
|---|---|---|---|
| 1000-1500 | 392.9 -> **1214** | 0.096 -> **0.302** | 5.28e5 -> **1.35e6** |
| 1500-2000 | 440.5 -> **1497** | 0.108 -> **0.366** | 8.35e5 -> **2.44e6** |
| 2000-2600 | 880.2 -> **2096** | 0.215 -> **0.512** | 1.96e6 -> **4.47e6** |

Roughly 3x across the board. `le4` is still pinned at its 1024 cap in every band.

## RETRACTION: the equilibrium-slip model no longer fits

`slip = (w*dt)/k` fit our port at ratio 1.03 / 0.97 / 0.86 earlier today. On the
current build:

| band | slip predicted | slip measured | ratio |
|---|---:|---:|---:|
| 1000-1500 | 0.0396 | 0.0295 | 1.35 |
| 1500-2000 | 0.0562 | 0.1309 | **0.43** |
| 2000-2600 | 0.0768 | 0.1933 | **0.40** |
| 2600-5000 | 0.0704 | 0.2541 | **0.28** |

It now UNDER-predicts by 2.5-3.6x. **So the "grip needed / 3.9x short" column in the
previous step is void and must not be chased.** I am retracting that line of analysis
rather than building on a model the data has stopped supporting.

The direction of the failure is informative though: we now have MORE slip than a pure
"body rotates, clamp bleeds" balance allows, which means there is a genuine lateral
slip SOURCE the model omits — i.e. real lateral tyre force. That is what should be
there, and it appeared as the port got more faithful. The model was only ever valid
while the port was too kinematic for it to matter.

## The gap, restated honestly

At MATCHED speed, port vs original:

| band | port | ORIGINAL | short by |
|---|---:|---:|---:|
| 1000-1500 | 0.0295 | 0.0815 | 2.8x |
| 1500-2000 | 0.1309 | 0.1925 | 1.5x |
| 2000-2600 | 0.1933 | 0.2498 | **1.3x** |

Tightening as speed rises. Note also that our 2600-5000 slip is **0.2541**, essentially
the original's top-band value — the original's capture never reaches that band, so part
of the apparent deficit is a speed-DISTRIBUTION difference rather than a slip-law error.
Only the matched-speed column above is sound.

## Next step (a technique, not another model)

`le4` is saturated at its 1024 cap in every one of our bands. Whether the ORIGINAL's is
also saturated is unknown and would change what headroom exists in `l_60`. `le4` is a
local, so the record capture cannot show it — but the live-Frida global/local read that
settled `_DAT_0088e610` is the right instrument: hook `FUN_00467650` and sample `le4`
and `ld4` per wheel on the original. That is the one measurement that would tell us
whether `l_60` is even capable of the values this port would need.

Do NOT resume the equilibrium-model line without first re-validating it on the build
of the day.

---

# Twentieth follow-up — grip chain and per-wheel force BOTH match; the slip gap is unexplained

## No hook was needed

`le4` and `ld4` are locals, but every one of their inputs is a record field (both
branches of the block-#4 orientation check read only `+0x9bc..c4`, `p[-9..-7]`,
`+0x9e4`, `+0x9e8`, `+0x9b0..b8`, `+0x9d4..dc`, `p[0x1f..0x21]`). So they can be
RECONSTRUCTED from the existing capture — no Frida hook on a per-frame function, and no
new original-side run.

## Elimination 1: the grip chain MATCHES

| band | ORIG le4 raw | le4 capped | ORIG ld4avg | ORIG l_60 | PORT l_60 |
|---|---:|---:|---:|---:|---:|
| 1000-1500 | 1278.6 | 1024.0 | 0.3250 | 1330.4 | 1214 |
| 1500-2000 | 1784.3 | 1024.0 | 0.4333 | 1774.1 | 1497 |
| 2000-2600 | 2233.6 | 1024.0 | 0.4909 | 2009.5 | **2096** |

**The original's `le4` saturates at the same 1024 cap ours does** — that was the open
question and the answer is "yes, both". And `l_60` agrees within 10-19%, with ours
slightly HIGHER at the top band. So `grip`, and therefore the clamp's `k`, is not the
defect. The line the previous follow-up proposed is closed as a negative.

## Elimination 2: the per-wheel force now MATCHES too

`fTot` re-measured on the current build. It improved again on its own, purely from the
`_ftol2` and `suspDtTerm` fixes:

| band | ORIG | PORT now | x | x (previous measure) | slip x |
|---|---:|---:|---:|---:|---:|
| 1-500 | 65683 | 32821 | 2.00 | 1.99 | -- |
| 500-1000 | 100263 | 86499 | **1.16** | 1.76 | -- |
| 1000-1500 | 156872 | 132936 | **1.18** | 1.59 | 2.76 |
| 1500-2000 | 198655 | 162780 | **1.22** | 1.58 | 1.47 |
| 2000-2600 | 220371 | 211672 | **1.04** | 1.39 | 1.29 |

Above speed 500 the per-wheel force is now within **4-22%** of the original. That is the
third finding this session that dissolved once the surrounding regime was corrected.

## So the slip gap is UNEXPLAINED

At 2000-2600 the force matches to 4% and the grip chain matches to within 10%, yet the
original has **1.29x** more slip. At 1000-1500 the force matches to 18% and slip is
**2.76x** short. The shortfalls do not track: neither measured input explains the
output.

Everything measured on both sides now agrees except slip itself. That is a genuinely
different situation from where this started, and it is the honest handoff state — I am
not going to invent a mechanism to close it.

Candidates NOT yet examined (listed so the next session does not re-tread ground):
`le0/ldc/ld8`'s own inputs `p[-9..-7]` (wheel mount offsets, reconstructible from the
capture the same way), and the lateral component of the per-wheel force vector as
opposed to its magnitude — every comparison in this note used `|fTot|`, and a matching
magnitude with a different DIRECTION would produce exactly this signature.

**That direction check is the obvious next measurement, and it needs no new run:**
compare `dot(fTot_hat, velocity_hat)` between the original's reconstruction and our
diag.

---

# Twenty-first follow-up — force DIRECTION also matches; constant audit; final state

## The direction hypothesis is REFUTED

The previous follow-up proposed that a matching force MAGNITUDE with a different
DIRECTION would explain the slip gap. Measured (`fTot` summed from `p[0x1c..0x1e]`,
which are record fields on both sides — no hook needed):

| band | side | cos to body fwd | cos to velocity | lateral fraction |
|---|---|---:|---:|---:|
| 1000-1500 | ORIGINAL | -0.4632 | -0.5690 | 0.8863 |
| | PORT | -0.4437 | -0.4709 | **0.8962** |
| 1500-2000 | ORIGINAL | -0.3155 | -0.4956 | 0.9489 |
| | PORT | -0.3558 | -0.4663 | **0.9346** |
| 2000-2600 | ORIGINAL | -0.2269 | -0.4386 | 0.9739 |
| | PORT | -0.3060 | -0.4963 | **0.9520** |

Lateral fraction agrees within **2%** in every band. The direction is not the defect
either. Hypothesis dead.

## Constant audit — 5 of 14 plain literals had the WRONG BITS

Prompted by the `_DAT_005cea80` gloss bug, I audited every plain decimal literal on
this path against the PE's `.rdata`. `Integrate2.cpp` uses the bit-exact `Cf()` idiom
and was clean; the exposed ones were in `BodyOrientationIntegrate.cpp` (the law ported
today) and my own `kPosDtK`:

| constant | should be | we had | error |
|---|---|---|---|
| `kGripPerSpeed` | `0x3a2ec33e` = 0.00066666666 | 0.00066667 | 0.0005% |
| `kDtK` | `0x39aec33e` = 0.00033333333 | 0.0003334 | 0.0200% |
| `kAccumK` | `0x3b03126f` = 0.00200000009 | 0.0020001 | 0.0050% |
| `kSpinTerm` | `0x3a03126f` = 0.000500000024 | 0.000500029 | 0.0058% |
| `kPosDtK` | `0x39aec33e` = 0.00033333333 | 0.00033332 | 0.0040% |

All five converted to the bit-exact `Cf()` form. Re-measured on a clean 1100-sample
run: slip 0.0323 / 0.1321 / 0.1925 and gate (a) 1724.34, against 0.0309 / 0.1308 /
0.1933 and 1735.67 / 1776.79 before — **no movement**, as expected for sub-0.02%
changes. A bit-exactness fix, not a behavioural one.

(Harness note: two runs in a row stalled in the frontend nav and never reached the
race. Not caused by the change — a third identical run was fine. Worth knowing that
this recipe occasionally needs a retry; check the sample count before comparing.)

## Final state of A8

Everything measurable now agrees with the original EXCEPT slip:

| quantity | port vs original |
|---|---|
| gate (a) driving-median | ~1778 vs 1760 (within 1%) |
| gate (b) yaw rate | 2.07 / 2.55 / 2.55 vs 2.31 / 2.66 / 2.65 |
| per-wheel force `fTot` magnitude | within 4-22% above speed 500 |
| per-wheel force DIRECTION | lateral fraction within 2% |
| grip chain `l_60` | within 10-19%; `le4` saturates at 1024 on both |
| `p[0x1a]` record fidelity | 1088.31 vs 1086.83 (0.14%) |
| **slip** | **0.193 vs 0.250 (1.29x) at 2000-2600; 2.76x at 1000-1500** |

Four candidate explanations have now been eliminated by measurement: the grip/clamp
chain, the force magnitude, the force direction, and the constants. The slip deficit is
not explained by any input this session has been able to measure.

That is the handoff state. It is a much narrower and better-instrumented question than
the one this session opened with, and it should NOT be closed by inventing a mechanism.
