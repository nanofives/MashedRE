# D1 — ROOT CAUSE: the standalone renders the world MIRRORED relative to the original (2026-08-16)

The full-basis transplant landed, and it exposed the thing that has been defeating every
original-vs-standalone comparison in this project.

## The basis transplant

`MASHED_CAM_POSE` now accepts 12 floats — `pos, right, up, at`, the camera's full orthonormal
basis — alongside the legacy 6-float eye/at form. The 6-float form assumes up is world +Y and
therefore cannot express roll, and the original's race camera measurably has ~26 deg of it.
The view matrix is built by the new `MatViewFromBasis` (`TrackRenderer.cpp`), which sets the
rows from the basis directly instead of re-deriving axes from a target point.

`race_draw_burst.py` now emits it as `orig_cambasis.txt`, read straight off the RwCamera
frame LTM. Sanity check on the measured basis: `cross(up, at)` reproduces the measured
`right` to 5 decimals, which is the D3D left-handed convention — so the basis and our matrix
builder agree, and the transform below is not a convention error on our side.

## What it showed

With pos, basis, FOV, near and far ALL matched to the original, the standalone renders the
same landmarks **on the opposite side of the screen**:

| landmark | original | standalone |
|---|---|---|
| rock formation + chimneys | left | right |
| large buildings | right | left |
| cable / suspension rig | overhead, leaning right | overhead, leaning left |

Flipping the standalone image horizontally makes them line up — road, banners, rock stack,
buildings, and the camera roll all in the same places.

## Measurement

```
orig vs basis            89.68%
orig vs basis MIRRORED   44.38%
```

**A horizontal flip halves the difference.** This is the one place in this session where a
pixel number is admissible as evidence: it is not a continuous parameter that could trend
spuriously (the failure mode that invalidated the FOV sweep), it is a discrete structural
transform, and a 45-point drop from a single mirror is not something a wrong hypothesis
produces.

The residual 44.38% is the expected mix of a different sim moment (the cars are not in the
same places), lighting and texture differences, and the ~8.7 deg read-vs-capture timing
offset from `verify/d1_carproj/RESULT.md`.

## Why this hid for so long

A mirrored world is **self-consistent**. The standalone derives its own chase camera in its
own space, so gameplay looks entirely normal — you cannot see the mirror by playing, only by
transplanting a pose measured in the original's space. Every previous attempt to do that was
also fighting a mis-sourced pose (`ctrl+0x40` instead of the camera frame) and a wrong lens
(60 deg instead of 48.46), so the mirror never had a clean chance to show itself. It took
fixing both, plus the basis form, before the remaining error was a pure reflection.

This is the actual answer to "same track, same pose, different scenery" from
`verify/d1_frame/RESULT.md` — and it supersedes that note's "the worlds are not in the same
coordinate frame" as the correct statement of the problem. They are related by a reflection.

## LOCATED: the sign of the camera RIGHT axis

Neither of the two candidates below survived inspection, so the note continues past them.

- **Loader**: no axis negation anywhere in the world/DFF read. Vertices go in verbatim.
- **Matrices**: `MatLookAtLH` and `MatPerspectiveFovLH` are both textbook D3D left-handed,
  with no sign error.

The disagreement is in the **convention between RenderWare's camera basis and ours**, on the
right axis specifically. Negating it in the transplanted basis and re-rendering:

```
orig vs basis (verbatim)       89.68%
orig vs basis, image MIRRORED  44.38%
orig vs NEGATED RIGHT AXIS     33.79%
```

The negated-axis render beats the post-hoc image mirror by 10.6 points, which is the expected
ordering and a useful check: flipping pixels also flips the HUD and mis-handles occlusion,
while negating the axis renders the scene correctly to begin with. Landmarks line up natively
— rock stack and chimneys left, buildings right, cable rig overhead, matching roll.

Independent corroboration from the earlier car projection: `verify/d1_carproj/RESULT.md`
recorded the frame-basis prediction as ~80-100 px to the *right* of the actual cars and
guessed at read-vs-capture timing. Mirroring that prediction about the screen centre
(344-396 -> 244-296) lands it on the measured car cluster at 245-320. **That residual was
this reflection, not timing.** The timing hypothesis in that note is withdrawn.

## What this means for the standalone

Our world data is verbatim from the same files the original reads, and our camera derivation
is standard D3D LH (`right = cross(worldUp, forward)`). Given the same eye and forward, the
original places world-left on the opposite side of the screen from us. It follows that
**the standalone has been rendering the world mirrored relative to the original all along** —
self-consistently, which is why gameplay looks fine and nobody caught it.

I am stating that as a consequence of the measurement, and flagging that it rests on the two
inspections above (verbatim loader, textbook matrices) being complete. Neither was exhaustive.

## The fix is a decision, not an edit

Two ways to close it, with very different blast radii:

1. **Negate the right axis in the standalone's view construction.** Makes the standalone
   match the original. Mirrors the entire presentation — every capture, every baseline, the
   HUD side, the chase camera's sense of left and right, and any hand-tuned scaffold that was
   authored against the current (mirrored) view.
2. **Negate only on transplant.** Keeps the standalone as it is and makes same-view
   comparison correct. Leaves the shipping renderer mirrored with respect to the original,
   which for a source port is a faithfulness defect, not a cosmetic one.

Option 1 is the correct end state and option 2 is the safe interim. Not applied either way —
this is a call about what the port is supposed to be.

## What was NOT determined

**Which axis, and where the flip lives.** Two candidates, and I have not discriminated them:

1. **The loader** — our BSP/DFF vertex read negates (or fails to negate) an axis relative to
   the original. RenderWare's file convention vs D3D's left-handed space is the usual source.
2. **The view/projection convention** — a sign in how we build the view matrix or the
   projection, applied consistently so it never showed up in standalone-only work.

Both are self-consistent under normal gameplay, so appearance cannot discriminate them. A
coordinate can: the original's four cars sit at world
`x = 0.50, -1.10, 1.14, -0.47` / `z = -2.00, -0.90, 0.20, 1.30` (a staggered start grid, read
from the render hierarchy this session). Dump the standalone's spawn grid and compare signs —
if the x values are negated, the reflection is in the data path; if they match, it is in the
view path. One capture, no new RE.

## Scope of the damage

This invalidates the *geometry* of every original-vs-standalone visual comparison in the
project, including this session's sky verdict framing. It does **not** touch anything
standalone-only (the FX particle isolation, the renderer A/B, the re-baselines) or anything
read directly out of the original's memory (the lens, the far plane, the car positions).
