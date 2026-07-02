# Visual-polish pass — state (2026-06-19, branch ws-visual-polish)

Worktree-isolated from the actively-churning main checkout (concurrent session on
WS-G4 + frontend parity). Spike = `mashed_re.exe` (TrackRenderer 3D race render).

## 1. Collision FX (skids / impacts) — IMPLEMENTED (commit 2102325d)
Was unimplemented (zero skid/impact/spark code). Added `TrackRenderer::EmitCarFx`,
called per car (player + 3 AI) each frame before the particle pool advances:
- **Skid smoke:** velocity decomposed into forward + lateral; on hard lateral slip
  while grounded + moving, emits dark alpha-blended smoke trails behind the rear axle
  (two wheel tracks), rate ∝ slip.
- **Impact sparks:** a sudden one-frame speed drop (= collision) emits a warm
  `SpawnBurst` spark fan, count ∝ decel.
Reuses the existing `ParticleSystem` billboard pool (alpha blend). Thresholds scale to
track radius. [SCAFFOLD] presentation like the ambient emitters; data path real.

**Calibrated + verified FIRING (MASHED_FX_DEBUG instrumentation).** First pass barely
fired (0 skids / 1 spark): the lateral-slip skid trigger stays ~0 because the kinematic/AI
cars keep velocity aligned with heading, and they encode motion as `(speed, yaw)` — `vel[]`
is zero in the scaffold path, so `vf` (and the slip/corner derived from it) was always 0.
Fix: drive the **skid trigger off yaw-rate × passed `speed`** (cornering), not `vf`.
Re-measured on a moving AI car: `spd=13.74, yawRate=1.61 → corner=22.0 > kCorner=6.65 →
skidI=3.32`; skid counter climbed **0→43→96→207→251 in one race**, + impact spark on a
stop. So skid smoke now actively emits on AI cornering and sparks on impacts. (Counter is
the definitive proof; the small spike window makes the dark smoke hard to isolate in a
single low-res screenshot.) Builds; FX is additive — can't break existing render.

## 2. Vehicle lighting + HUD — present & working (verified in-frame)
- Lighting: `ApplyCarLighting` (face-normal directional diffuse + 0.45 ambient, two-sided)
  folds shading into car vertex colours. Screenshot `verify/visual_polish/vp_race_f0.png` (start grid)
  shows the 4 cars clearly shaded with form (not flat-white). Reasonable; **not changed
  blind** — a fidelity rewrite (e.g. smooth/Gouraud normals) needs a visual side-by-side
  vs the original to avoid degrading existing appearance (verify-don't-guess).
- HUD: present (color panel top-left). Renders.

## 3. Parity sweep — run; needs fresh co-captures for a valid verdict
`drawlist_diff.py` on the existing logs = RED (174/60/2) but **invalid**: stale
(Jun 12-13) and screen-mismatched (A = original frontend loop, B = TrackRenderer::LoadCar).
A valid sweep needs fresh co-captures (`menu_draw_burst.py` original side + standalone
`MASHED_DBG_DRAWSTREAM`), which contends MASHED with the active WS-G4 session.

## Visual verification (captured + inspected)
- `verify/visual_polish/vp_race_f0.png` (~4 s): start grid — 4 shaded cars, city/night track, HUD.
- `verify/visual_polish/vp_t6500.png` (~6.5 s): cars racing in motion on the track, FX/lighting active.
(Relocated from verify/ root at salvage-merge 2026-07-02 per verify/ subfolder convention.)
Spike renders a credible, moving race. (The race-demo is a fast auto-exiting verification
driver — `PostQuitMessage` on done, <8 s — so a crisp mid-slide FX frame is a timing
gamble; the FX system is in and running.)

## Remaining for "looks/feels finished" (user-judgment + clean-machine dependent)
- Your visual sign-off on a viewable run (the subjective bar).
- A valid parity sweep on a clean machine (fresh co-captures).
- Optional lighting/HUD fidelity tuning — only with an original side-by-side.
Merge `ws-visual-polish` (collision FX) when satisfied.
