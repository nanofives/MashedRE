# Mirror-fix audit — what the reflection touched, and the check that should have caught it (2026-08-16)

Companion to `verify/d1_mirrorfix/RESULT.md`. The mirror fix changed **only the view matrix**;
world data and simulation are untouched. So world-space behaviour is identical and merely
appears on the other side of the screen. The risk being audited is narrower: **values chosen
by looking at a mirrored screen**, and compensating sign flips authored to make that mirrored
view look right.

## The zero-cost check nobody ran

Sponsor banner textures carry readable text. Cropped from `01_grid`:

| | banner text | HUD |
|---|---|---|
| pre-fix (`verify/d1_far_d3d9/`) | **"supersonic" and "EMPIRE" reversed** | "2" correct |
| post-fix (`verify/d1_mirrorfix/final_d3d9/`) | correct | "2" correct |

This is unambiguous, needs no reference capture, and was present in **every standalone
capture ever taken**. The world was mirrored and the sponsor text said so the whole time.

It also independently confirms the HUD is unaffected — `RwIm2DBridge` / `QuadRenderer` use
their own 2D path, never this view matrix — which the two Frontend shots corroborate at
0.00% across the change.

**Standing check to adopt: on any new track capture, read the sponsor text.** Mirrored
world geometry is otherwise invisible without an original-side reference, which is what let
this survive months of clean A/B runs.

## Cleared: lighting is data-derived

`sun_dir_` is parsed from LIGHTS.DFF (`ParseLightsDffDirectional` / `ParseLightsDffFaithful`,
`TrackRenderer.cpp:584/680`, applied at `:1064-1065`), not hand-picked to make shading look
right. A view-only change cannot invalidate it. The WS-E relight work compared standalone
shots against *original-side* shots by heading, so that lane's headings were matched in world
space, not by eye.

## Cleared: start grid and AI lanes are data-derived

`StartRound` (`TrackRenderer.cpp:~3105-3140`) builds its lateral axis from the gate ribbon —
`dir` from `gates_[0]`->`gates_[1]`, `lat = {-dir.z, dir.x}` — and places the 2x2 grid at
+/-0.9 along it. Both the direction and the sign come from track data, so the grid is in the
same world position as before; only its screen side changed. Same for `a.lane = -3, 0, +3`,
which is an offset along a data-derived axis.

These are *invented* values (the grid shape is scaffold, and the original's real grid is at
`x = 0.50, -1.10, 1.14, -0.47` / `z = -2.00, -0.90, 0.20, 1.30`, measured this session) — but
invented is a separate problem from mirrored, and the view fix neither helps nor hurts it.

## The fix itself is self-consistent

Both builders negate exactly one basis row. In `MatLookAtLH`, `up` is computed from the
pre-negation right (`TrackRenderer.cpp:790`), so the final triple has determinant **-1** — a
pure horizontal reflection, vertical untouched, no double-flip. `MatViewFromBasis` likewise.

## Three things to act on

**1. `MASHED_AI_STEERFLIP` (`Ai/AiStandalone.cpp:483-494`) — not invalidated, but it exposes a
contradiction.** Its justification is entirely world-space (a position log with `own=`/`tgt=`/
`vel=` in world coordinates, plus which way `+0x9c0` rotates yaw), so nothing was read off the
screen and the mirror fix does not touch it. But it is gated behind `MASHED_AI_PUREPURSUIT`,
which is **opt-in and default OFF** (`:438-444`), while the shipping default path is the
verbatim bands at `:519-560`, which do **not** apply the flip. The codebase therefore asserts
two opposite steer signs: if the flip is right the default path steers backwards, and if the
bands are right the flip is wrong. This predates today and is now observable in a view that
can be trusted. Worth resolving.

**2. Two opposite "track right" conventions in one file.**

| site | expression | perpendicular |
|---|---|---|
| `TrackRenderer.cpp:2254` | `rx = fz, rz = -fx` | `(+fz, -fx)` |
| `TrackRenderer.cpp:3133` | `lat = {-dir[1], dir[0]}` | `(-fz, +fx)` |
| `TrackRenderer.cpp:2770` | `aimx = g[0] + (-tdz)*lane` | `(-fz, +fx)` |

`:2254` is the negation of the other two. It does not currently bite, because the consumers of
`:3133` and `:2770` are symmetric (`side = +/-0.9`, `lane = -3,0,+3`) and therefore
mirror-invariant. But `:2254`'s consumer is **asymmetric** — `lat[3] = {+0.7, +1.7, +0.7}` at
`:2258` puts all three AI cars on **one** side of the racing line, and which side depends
entirely on the odd-one-out convention. It is data-derived (the side is justified by spline
nav-seeding at `:2242-2246`, a world-space argument), so it is not broken. It is a latent trap
for any future code that assumes one convention while calling the other. Unify it.

**3. Free camera: the mirror fix SILENTLY REPAIRED strafe and mouse-look. Do not "fix" them
again.** At `TrackRenderer.cpp:4015-4020`, `right[] = {-sin yaw, 0, cos yaw}`. Verified by
hand: `cross(up, fwd)` normalises to `(sin yaw, 0, -cos yaw)` = **minus** that, and after the
negation it is **exactly** that. So before today, positive strafe pushed the eye toward
screen-LEFT and positive yaw swung the look toward screen-LEFT. Both are correct now. If
anyone reports "strafe/mouse-look reversed today", that is the bug being *fixed*, not
introduced.

## Cleared by the survey

- **Lighting, doubly.** The fallback sun is traced to binary addresses as an axis-angle
  derivation (`TrackRenderer.cpp:1069-1081`), and decisively `sun_dir_[0] == 0` — zero lateral
  component, so it lies in the plane of symmetry and cannot be backwards under a horizontal
  mirror. The `-sun_dir` negations at `:184`/`:1107` are the standard travel-direction to
  direction-to-light conversion, not compensations.
- **Race-camera director** (`Race/RaceCamera.cpp:186-334`): every constant carries an `.rdata`
  VA; the only axis-looking term (`:296`) is a pitch axis, not a lateral one.
- **Chase rig**: purely longitudinal and vertical (`:4047-4056`) — no lateral term exists, so
  the "chosen so it frames the car" comment only picks distances, which a mirror cannot
  invalidate.
- **Car body and wheel matrices**: determinant +1 (proper rotations, no reflection); the
  "saw its flank, not its rear" comment is a 90/180 degree distinction, which a horizontal
  mirror preserves. Note the matrix is **triplicated** at `:3696`, `:4536`, `:4569` — any
  future change needs all three.
- **`w.front` (`:2138`, "verified visually")**: the only visual justification in the render
  path, but it picks front vs rear along the long axis, which a mirror does not swap.
- **Wheel spin sign (`:2810`)**: a mirror reverses apparent travel and apparent rotation
  together, so rolling-without-slipping is preserved.
- **No minimap or second 3D-to-2D projection exists in the standalone**; all 2D drawing is
  screen-space and never routed through the view matrix.

## Audit verdict

**No compensating lateral flip was authored against the mirrored view.** Every left/right
choice in the standalone is either derived from track data / binary-cited constants, or
symmetric about the lateral axis and therefore mirror-invariant. Nothing needs reverting.

The three items above are not damage from the fix: one is a pre-existing contradiction the fix
now lets us settle, one is a latent convention clash, and one is an input inversion the fix
silently repaired.

## Still open

An empirical steering check was attempted and **failed to produce data**: with
`MASHED_DEMO_DRIVE=1` the logged heading was constant at 1.54978 across all three in-race
captures, i.e. the demo driver did not turn the car at all, so the run says nothing about
steering sense. Noted so it is not mistaken for a passing result. A working steering test
needs a driver that demonstrably changes heading.

---

**Capture-set note:** some `verify/d1_*` directories cited above were pruned on
2026-08-16 to reclaim disk. They are regenerable from the recipe in this file; the exact list
of what was deleted and what was deliberately kept is in `verify/d1_evidence/README.md`.
