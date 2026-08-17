# D1 — mirror fix applied to both renderer paths, and librw turns out to have been right all along (2026-08-16)

Applies the reflection found in `verify/d1_basis/RESULT.md` to the standalone's view
construction, so the port renders in the original's handedness rather than its mirror.

## Change 1 — D3D9 view construction

`TrackRenderer.cpp`: the right axis is negated in both `MatLookAtLH` and `MatViewFromBasis`.
Applied to the *axis*, not to the projection's `_11`, deliberately — librw builds its own
projection from the published fov/aspect/near/far, so negating there would have flipped only
the D3D9 path and silently broken the renderer A/B.

Verification: the transplant of the original's verbatim RwCamera basis now reproduces
**33.79%** — bit-for-bit the number obtained by pre-negating the basis by hand before the fix.
The negation is in the right place and the transplant no longer needs special handling.

## Change 2 — the A/B blew up, which was the useful part

Re-baselining after change 1 sent the renderer A/B from ≤1.01% to **23-57%**. librw had not
followed.

Cause, at `RwRaceSubmit.cpp:103-120`: **[D-S3-4]**, which negated `right` when building
librw's camera frame in order to cancel librw's view-space X negation
(`beginUpdate` builds view as inverse(LTM) with each basis row's X component negated,
`d3ddevice.cpp:1229-1240`). D-S3-4's own measurement was sound — cancelling did move librw
toward the D3D9 path, mean-abs 25.37 -> 15.41 — but **the conclusion drawn from it was
wrong**: the D3D9 path was itself mirrored relative to the original, so D-S3-4 tuned librw to
agree with a bad reference.

librw is a RenderWare implementation. Its built-in X negation *is* the original's convention.
It was correct out of the box and was compensated into being wrong to match us.

**D-S3-4 reverted**: librw is handed the plain basis and its own negation stands.

## Result

Renderer A/B, both paths sharing the original's handedness:

| shot | before mirror work | after |
|---|---:|---:|
| `race1/01_inrace_track` | 0.48% | **0.48%** |
| `r6/round3_result` | 0.10% | **0.10%** |
| `r5/car_3_weave` | 1.01% | **1.01%** |
| all 16 | max 1.01% | **max 1.01%** |

Parity is restored **exactly** — every shot matches the pre-fix baseline to the decimal. That
is the expected outcome and a real check: the two paths were mirrored *together* before and
are un-mirrored *together* now, so a shared reflection cancels out of a D3D9-vs-librw
comparison and cannot be seen from inside it. It took an original-side reference to find,
which is precisely why it survived a clean A/B for months.

The D3D9 side is 0.00% against the pre-revert capture on all 16, confirming change 2 touched
only librw.

Against the original at a transplanted pose: **89.68% -> 33.79%.**

## What this cost, and what it did not

Nothing regressed. The standalone's own presentation is now mirrored relative to every
capture taken before today, so **all pre-2026-08-16 `verify/` stills are horizontally flipped
with respect to current output** — they are not wrong about anything except handedness, but a
naive imgdiff against them will read ~30-45% and mean nothing.

Not re-checked, and it should be: any scaffold hand-tuned against the old mirrored view. The
HUD is unaffected (`RwIm2DBridge` / `QuadRenderer` have their own 2D path and do not use this
view matrix), and the two Frontend shots confirm it — 0.00% across the change.

## Residual

The 33.79% against the original is a different sim moment (cars in different places),
lighting and texture differences, and whatever is left of the pose read not being synchronised
to the capture. Structure, handedness, lens and framing now agree; content does not yet.
