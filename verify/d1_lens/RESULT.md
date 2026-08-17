# D1 lens — the original's FOV is MEASURED; it is not what makes the transplant inaccurate (2026-08-16)

## The measurement

Read live out of `MASHED.exe` in a Quick Battle on TRAINING, via
`re/frida/race_draw_burst.py` (extended this session to dump the lens alongside the pose):

```
viewWindow=(0.6000,0.4500)  fovy=48.46deg  fovx=61.93deg
near=0.1000  far=360.00  projType=1 (perspective)
ctrl 0x58=0.6  0x24=0.75  0x6c=1  0x70=1
recip_ok=True  setupfov_ok=True
```

RenderWare stores no angle — the lens is `RwCamera::viewWindow`, the half-extents at unit
distance — so `fovy = 2*atan(0.45) = 48.46 deg` is our arithmetic on the measured halves,
and `orig_lens.json` keeps the raw fields so it can be redone.

The `RwCamera*` is `*(DAT_00897fe0 + 0x84)`, dereferenced that way by three independently
reversed functions (`0x00441700` `Camera::SetupFOV`, `0x00441760` `Camera::Apply`,
`0x00442a20` `Camera::InitWithMatrix`). Field offsets are the RW 3.x layout, confirmed
against our own reversals: `0x004c1c80` (`RwCameraSetViewWindow`) writes `+0x68/+0x6c` and
the reciprocals at `+0x70/+0x74`; `0x004c1a70` reads projType `+0x14` and clips `+0x80/+0x84`.

Two cross-checks, both passing, so this is not a hopeful pointer read:

1. `recipViewWindow == 1/viewWindow` — confirms the offsets are right.
2. The static derivation from `Camera::SetupFOV` reproduces the read exactly:
   `vw.x = ((ctrl[0x6c] * ctrl[0x58]) / ctrl[0x24]) * 0.75 = ((1 * 0.6) / 0.75) * 0.75 = 0.6`
   `vw.y = 0.75 * ctrl[0x70] * ctrl[0x58] = 0.75 * 1 * 0.6 = 0.45`
   The two undocumented multipliers `ctrl[0x6c]`/`ctrl[0x70]` are both **1.0**, which is the
   fact the static analysis could not supply. Corroborated independently by
   `hooks.csv:559` `MinimapCameraOrthoSetup`, recorded as "ortho 0.6 x 0.45".

`0.6/0.45 = 4:3`, so the standalone's `last_aspect_ = 800/600` is consistent.

## What the standalone currently has

`TrackRenderer.cpp:4021` — **60 deg, self-declared invented**, never justified by evidence.
Also `near = 0.05` (measured: 0.1) and `far = radius_ * 8` (= 2122 on Training; measured:
360). The comment block there records that an earlier branch derived ~48 deg from view
window 0.6 and that branch was deleted. **The deleted value was the correct one.**

`MASHED_CAM_FOV=<deg>` was added this session so the constant can be overridden without a
rebuild.

## The part that did NOT work

Feeding the measured 48.46 deg into the pose transplant made the pixel diff **worse**:

| | vs original |
|---|---:|
| `MASHED_CAM_FOV=48.46` (measured) | 90.48% |
| `MASHED_CAM_FOV=60` (invented) | 87.87% |

**Do not read that as evidence against 48.46 deg.** A 35→80 deg sweep at the previous pose
is monotonically decreasing (61.58 → 49.38 mean abs diff) with no minimum anywhere in range.
A metric that improves as the lens widens without bound is not measuring alignment; it is
measuring how much dark near-camera content gets pushed out of frame. **Whole-frame imgdiff
cannot validate a pose transplant**, and every "FOV is a suspect" claim built on that metric
— including mine in `verify/d1_sky_orig/RESULT.md` — was resting on it.

## Where that leaves the transplant

FOV is **eliminated** as the cause of the residual, by direct measurement rather than by
fit. Of the two suspects named in `verify/sametrack3/SAME_TRACK_RESULT.md`, one is closed
and **sim moment** remains.

Visibly, at the same pose numbers, the original looks down the dirt road from an elevated
position while the standalone sits at road level with a horizontal barrier crossing the
frame — the same road and the same banners, different vantage. That is a larger discrepancy
than a lens can explain, and it is consistent with the near-camera dark band already filed
in `verify/d1_sky_orig/RESULT.md`. The next question is whether the two frames are at the
same simulation moment and whether the standalone's world is at the original's scale, and
it needs a landmark-correspondence check, not a pixel metric.

## ADOPTED AND RE-BASELINED (same day)

Applied: `last_fov_ = 2*atan(0.45)` (48.46 deg) from a named `kViewWindowY = 0.45f` so the
measured half-extent is what appears in the source and the angle is visibly derived from it;
`last_near_ = 0.1f`. `MASHED_CAM_FOV` still overrides. `far` deliberately unchanged (below).

Re-baseline: the standard 16-shot set re-captured on both renderer paths under the new lens.
**Renderer parity survives the change**, which was the thing at risk:

| | max delta | shots ≤0.5% |
|---|---:|---:|
| A/B at 60 deg (previous baseline) | 0.38% | 16/16 |
| A/B at 48.46 deg (new baseline) | **1.01%** | 15/16 |

The one shot above 0.5% is `r5/car_3_weave` at 1.01%, which was already the worst shot at
0.64% before the lens change and is the same pre-existing item — not new, and not a
regression introduced by the lens. `01_inrace_track` 0.38% → 0.48%, `round2_result` 0.18% →
0.06%, `round3_result` 0.25% → 0.10%.

The lens change itself moves in-race frames by **26-37%** and both Frontend shots by
**0.00%**, confirming it touches only the 3D projection and leaves the 2D menu path alone.

New baseline dirs: `verify/d1_lens_base_d3d9/`, `verify/d1_lens_base_librw/`. These supersede
`verify/d1_fxcut/` and `verify/d1_sky_librw/` as the current-default reference.

Against the original at the transplanted pose the number is 90.48% (vs 87.87% at the old
60 deg) — recorded for completeness and **not** treated as a regression, for the reason given
above: whole-frame imgdiff cannot validate a pose transplant, so it is not evidence either
way. The lens was adopted on the measurement, not on that metric.

## FAR PLANE — SETTLED, and without a second track

The far plane is **COURSE.LUA `Setup_Fog`'s second argument**. Not a formula on any radius:
the original computes no world radius at all. `radius_` is ours, invented at
`TrackRenderer.cpp:1657`, and `radius_ * 8` was a guess.

Chain, static:

- `Setup_Fog` handler `0x0047ab30` stores the pair into the course description at
  `+0x1ec` (near) and `+0x1f0` (far).
- Track loader `0x00426e10` reads that pair, guards on `near != far`, writes near to the
  camera's `fogPlane` (`+0x88`) and passes far to `0x004c1b10` — the
  `RwCameraSetFarClipPlane` equivalent, which writes `+0x84`.
- `0x00426810` performs the same `+0x88` / `+0x84` pairing per frame for tracks with a
  camera anim, independently confirming the pairing.
- Init-time default, where no `Setup_Fog` runs: **180.0f** at every camera-init site
  (`0x0042d560`, `0x0042f660`, `0x00467110` all write `0x43340000`).

**Confirmed by measurement, both arguments at once — which is why the second track turned
out to be unnecessary.** The live camera read on TRAINING returned `fogPlane = 20.0` and
`farPlane = 360.0`; our own COURSE.LUA asset survey (`TrackRenderer.cpp:1436-1440`) records
training as `Setup_Fog(20, 360)`. Exact match on the pair, so the operand binding — which of
the two globals lands on `+0x84` vs `+0x88`, the one thing the static analysis could not
confirm without re-disassembling the call site — is settled by the data.

Five independent race entries all read the same lens, so the reading is stable, not
per-run noise.

Applied: `last_far_ = fog_on_ ? fog_end_ : 180.f`. `fog_end_` was already parsed at
`:1460`; it had simply never been wired to the projection. Note `TrackRenderer.cpp:4166`
already asserted the relation in prose ("fog_end_ = far") while the code did something else.

Re-baselined again on both paths. Renderer parity holds: max **1.01%** (`car_3_weave`, the
same pre-existing shot), everything else ≤0.48%. The far change itself moves frames by only
0.01-0.08%, which is the expected result rather than a null one — on Arctic `fog_end_` is 70,
so geometry is now clipped exactly where the fog has already saturated it to the fog colour.
Far == fog end means the clip is invisible by construction. That is coherent with the
original's design and is why the original could afford a 360 far plane at all.

New baseline: `verify/d1_far_d3d9/`, `verify/d1_far_librw/`.

Residual gap, stated: nobody has re-disassembled `0x00426e10`'s call site to read the two
push operands, and `0x00426810` is C1 with the far-clip call still open as U-0563. The live
measurement makes the binding unambiguous, but the static confirmation is owed.

## Original recommendation, now applied

Replace the invented `1.0472f` with the measured `2*atan(0.45)`, and `near = 0.05` with
`0.1`. That is measured-beats-invented and is right independently of the confounded pixel
number. Held back because it changes the default build's framing and therefore every
standalone capture taken to date — a deliberate call, not a drive-by edit.

`far` is deliberately excluded: 360.0 was read on Training only, and there is no evidence yet
whether it is a constant or track-derived (`radius_ = 265.32` there, so it is neither 8x nor
an obvious multiple). One track is not enough to replace a formula.

---

**Capture-set note:** some `verify/d1_*` directories cited above were pruned on
2026-08-16 to reclaim disk. They are regenerable from the recipe in this file; the exact list
of what was deleted and what was deliberately kept is in `verify/d1_evidence/README.md`.
