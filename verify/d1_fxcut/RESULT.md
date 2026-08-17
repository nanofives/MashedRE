# D1 — the renderer divergence is CLOSED, and there was never a sky problem (2026-08-16)

Authoritative result for the D1 D3D9-vs-librw divergence. Supersedes
`verify/d1_nopart/RESULT.md` and `verify/d1_fxbloom/RESULT.md`, both of which are corrected
in place rather than deleted.

## Result

Renderer A/B over all 16 shots, both sides on the same build, differing only in
`MASHED_RENDER_LIBRW=1`. `re/tools/imgdiff.py`, pixels over threshold 16.

| shot | before | now |
|---|---:|---:|
| `race1/01_inrace_track` | 71.61% | **0.38%** |
| `r6/round3_result` | 69.15% | **0.25%** |
| `r6/round2_result` | 68.94% | **0.18%** |
| `race1/01_action` | 21.69% | **0.01%** |
| `r5/car_5_chase` | 0.92% | 0.21% |
| `r5/car_3_weave` | 0.64% | 0.64% |
| the other ten | ≤0.03% | ≤0.03% |

**16 of 16 at or below 0.64%**, and the worst remaining shot (`car_3_weave`) is unchanged
from before — it is pre-existing, not a residue of this work. The accumulation pattern that
blocked the inversion (round1 0.01% → round2 68.94% → round3 69.15%) is gone: 0.01 / 0.18 /
0.25.

## Cause, in one line

A single defect on the **D3D9** side, present in **both** runs: the scaffold FX particle
class (kind==2). It is now cut from the default build.

## The correction that matters

`d1_nopart` Finding 2 claimed a second, independent sky-colour divergence: librw's sky
orange, D3D9's grey cloud. **That was wrong**, and the way it was reached is worth recording
because it is the fourth wrong diagnosis on this one problem.

Every "residual vs librw" number in `d1_nopart` and `d1_fxbloom` was measured against
`verify/allmode/librw/`, **a capture that still contained the FX bloom**. Removing FX from
only the D3D9 side left the librw side's bloom standing. librw submits the static world
*after* the whole D3D9 pass (`exe_main.cpp:2794`), so it overpaints the bloom wherever world
geometry covers the screen — and nowhere else. What survives is exactly the region with no
world geometry in front of it: the sky band. That is why the residual looked sky-shaped, and
why `01_action` (a camera pointed at mostly sky) looked worst at 77.72%.

The lesson is narrow and mechanical: **when you remove a defect from one arm of an A/B, you
have to re-run the other arm.** I compared a fixed build against a stale baseline and read
the stale baseline's defect as a new finding.

Fog was tested and is not involved: `MASHED_NO_FOG=1` moves these frames 0.00-0.14% on
either path.

## What was changed

- `ParticleSystem.cpp` — FX class (kind==2) **cut from the default build**; draw-time
  suppression via a kind mask defaulting to 3 (ambient + car-spray). `MASHED_PARTS_KINDS=7`
  restores it. Draw-time and not spawn-time so the pool trajectory is unchanged and captures
  stay comparable across the cut. Re-pickup condition: the ported `Particle/` system lands.
- `ParticleSystem.cpp` / `.h` — screen-coverage guard on kind==2 plus `SetFovY`, fed from
  `TrackRenderer.cpp:4025` so the FOV has one source of truth. Retained but currently
  dormant: it only runs when FX are re-enabled. It was a real improvement (71.61% → 41.31%)
  and an insufficient one, because it bounds billboard *size* and the residue was stacked
  *opacity* — 36 quads at alpha 0xFF saturate wherever they overlap.
- `TrackRenderer.cpp:4601` — `MASHED_NO_PICKUPS`, the isolation gate that cleared the
  additive pickup orbs (`PickupField.cpp:231`) of suspicion at 0.00-0.04%.

## Evidence chain

| dir | what it establishes |
|---|---|
| `verify/allmode/` | the divergence, before any of this |
| `verify/d1_nopart/` | the pass is responsible (particles + orbs together) |
| `verify/d1_fxbloom/` | the **class** is responsible — kind==2 alone, 87-99%; orbs 0.00-0.04% |
| `verify/d1_iso_ctl,nofx,nopick/` | the isolation runs behind that table |
| `verify/d1_fxcut/` | default build == the no-FX isolation run, 0.00% on all 16 |
| `verify/d1_sky_librw{,_nofog}/`, `verify/d1_sky_d3d9_nofog/` | the A/B above, and fog ruled out |

Determinism held throughout: the post-change control reproduced the pre-change baseline at
0.00% on all 16 shots, so every number here is signal.

## What this does NOT establish

**Which renderer is faithful.** Both were compared to each other, never to the original —
the limitation recorded on 2026-08-15 is untouched by this work. The two paths now agree
with each other; that is a precondition for inverting `MASHED_RENDER_LIBRW`, not proof the
result matches Mashed. An original-side capture at a matched pose is still owed, and
`re/analysis/DIVERGENCE_LEDGER_3D.md:17-19` still carries the open sky item (the original's
sky has cloud layers and UV scroll a static clump will not animate).

Note for whoever picks that up: **librw draws no sky at all** (`RwRaceSubmit.h:24-28`) — the
sky comes from the D3D9 path in both runs, so "which renderer's sky is faithful" is a
D3D9-vs-original question, not a D3D9-vs-librw one. The original's `SkyDomeRender`
(`0x004492b0`) sets a camera-follow clear colour of R=0x50 G=0x58 B=0x60, which is a cheap
first check against an original capture.

---

**Capture-set note:** some `verify/d1_*` directories cited above were pruned on
2026-08-16 to reclaim disk. They are regenerable from the recipe in this file; the exact list
of what was deleted and what was deliberately kept is in `verify/d1_evidence/README.md`.
