# D1 — landmark correspondence: the pose transplant is invalid, the worlds are not in the same frame (2026-08-16)

Chasing the vantage mismatch left open by `verify/d1_lens/RESULT.md`. Two hypotheses tested,
one refuted, and the actual cause located.

## Hypothesis 1 (mine): we transplant the wrong pose — REFUTED

`Camera::Apply` (`0x00441760`) rebuilds the camera's RW frame from the Euler angles at
`+0x34/+0x38/+0x3c` plus the `+0x40` position, and never reads `+0x4c`. So the `+0x4c` delta
we transplant is the director's *aim point*, not the camera basis. That predicted the
renderer might be using something else.

Read the ground truth — camera frame is `*(RwCamera + 4)`, RW 3.x `RwFrame` modelling at
`+0x10` / LTM at `+0x50`, `RwMatrix` = right/up/at/pos at `+0x00/+0x10/+0x20/+0x30`:

```
frame pos = (1.488, 3.805, 22.600)   vs ctrl eye (0.066, 3.316, 5.017)   dist 17.65
frame at  = (-0.000, -0.187, -0.982) vs 0x4c dir (-0.022, -0.397, -0.918) angle 12.69 deg
frame right = (-0.894, 0.440, -0.084)   euler elev=23.392 azim=181.349 roll=0
```

A 17.6-unit position gap looked like the answer. **It is not.** `frame.right.y = 0.440` is
~26 deg of roll, and the frame matrix therefore predicts a visibly tilted horizon. The
original capture's horizon is **level** — buildings vertical, road straight down the frame.
The prediction fails, so that matrix does not describe the camera that rendered this image
(modelling and LTM are identical, so it is not a stale-LTM artefact either).

The controller fields, by contrast, are internally consistent: `sin(23.392 deg) = 0.397`,
exactly the normalised `-dir.y` from `+0x4c`, with `roll = 0` matching the level horizon. So
**the controller pose we already transplant is the credible source**, and hypothesis 1 is
dead. Recorded because it was a well-motivated guess that a single landmark killed — which
is the point of using landmarks rather than a pixel metric.

The pixel numbers could not have decided this: frame pose 87.04% vs controller pose 90.82%,
i.e. the *refuted* hypothesis scores *better*. Third independent demonstration today that
whole-frame imgdiff cannot adjudicate a pose question.

## Hypothesis 2: the worlds are not in the same coordinate frame — SUPPORTED

Same track file on both sides (verified: the original opens exactly one `.piz`,
`TRAINING.PIZ`; the standalone logs `original/TOASTART/TRACKS/training.piz`). Same camera
pose. Same measured lens. The two frames show **different scenery**:

| | original | standalone |
|---|---|---|
| road surface | dirt / sand, no markings | asphalt with a painted white centre line |
| left side | buildings, scaffolding, cable rig | blue guard-rail sections, barrels |
| distance | town, helicopter | open water |
| camera height above road | ~2.8u, looking down | at road level |

These are different stretches of road, not the same view rendered differently. With eye,
look direction, FOV, near and far all matched, the only remaining variable is **where the
world sits**. The standalone's Training world is not in the original's coordinate frame —
some combination of origin, rotation and scale differs.

Ground height is *not* the discrepancy: the standalone's car y runs -2.3 to 0.3 while the
original's camera aims at y = 0.486, so the vertical scales are comparable. The offset is
therefore mostly horizontal and/or rotational, which is consistent with seeing a different
part of the same track.

## What this invalidates

**Every original-vs-standalone comparison that relies on `MASHED_CAM_POSE` is measuring two
different places.** That includes `verify/sametrack3/SAME_TRACK_RESULT.md` ("coherent
same-road view" — coherent, but not the same road), the 88-90% figures quoted all session,
and the coarse sky verdict in `verify/d1_sky_orig/RESULT.md`. The sky *colours* there were
sampled from sky pixels in both frames and survive as a rough check; the framing-dependent
parts do not.

It does **not** touch anything measured this session on the standalone alone: the FX particle
isolation, the renderer A/B, and the lens/far measurements are all either standalone-vs-
standalone or read directly out of the original's memory. None depend on the transplant.

## Gate correspondence attempted — BLOCKED, and the premise was wrong anyway

Two findings, in the order they matter.

**1. The standalone applies no world transform.** `center_` / `track_center_` /
`radius_` (`TrackRenderer.cpp:1652-1681`) are computed from the world bbox and the gate
centroid, and every use of them is camera framing — the orbit vantage at `:3940-3943`, the
overview at `:3999`, the pickup ring at `:2260`. **No vertex is displaced.** The world goes
into the batches in file coordinates.

So there is no standalone-side transform to invert. If the original also renders the BSP in
file coordinates — and nothing suggests otherwise — then the two worlds already share a
frame, and "solve the world transform" has no transform to solve. The hypothesis 2 framing
above is therefore too strong: what is established is that the two frames show different
scenery, not that the *world* is misplaced.

**2. Gate correspondence cannot be run today.** The standalone parses 30 gates for Training,
but there is no mapped gate/checkpoint array on the original side — `hooks.csv` has no
race-gate row (the "gate" hits are all prose, meaning conditional guards). Reading the
original's gate positions is itself an un-started RE task, so the correspondence has no
second column.

## Where the discrepancy actually has to be

By elimination, with the world in a shared frame and the lens measured, the remaining
candidate is the **camera space**: the controller's `+0x40` position may not be the world
eye. That is exactly what the frame read hinted at before its roll refuted it as a
substitute — `frame.pos` and `ctrl.eye` are 17.6 units apart, and both cannot be the world
camera position. One of the two is in another space, and the level horizon says the *frame*
is the odd one out, which leaves an unexplained inconsistency rather than an answer.

Stated plainly: **this is unresolved.** I can name what it is not (not the lens, not a
standalone world transform, not the `+0x4c`-vs-Euler distinction) and not what it is.

## Next step

The cheapest decisive test does not need gates at all — it needs **one** dynamic point whose
world coordinates and screen position are both known on the original side: the player car.

Read the original's car world position, project it with the pose and lens we transplant, and
compare against where the car actually appears in `orig.bmp`. If it lands on the car, the
pose and lens are in the world frame and the fault is in our world; if it lands elsewhere,
`ctrl+0x40` is in another space and the transplant is mis-sourced. Either outcome closes the
question, from a single capture, with no new correspondence machinery.

Prerequisite: the original's car-position global. The physics work has car struct offsets and
`re/prior_art/MashedTrainer` documents pointer chains, so this is a lookup rather than new RE.

Until that lands, treat `MASHED_CAM_POSE` as a debugging aid and not as a parity instrument,
and do not quote an original-vs-standalone pixel number.
