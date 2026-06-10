# Renderer-gate decision brief (2026-06-10)

The ROADMAP's R4 stop-and-ask gate: how should the standalone render RW data
long-term? The spike (D3d9Render/TrackRenderer) has produced evidence; the
decision is the user's.

## Option A — vendor librw (the re3 path)

- MIT-licensed reimplementation of the RW 3.x API; clump/geometry/texture
  streaming already proven against our DFFs (we cross-referenced its source).
- BUT: its world/BSP sector streaming is NOT implemented (we checked —
  world.cpp is minimal), so our world parser stays ours either way.
- Pros: real RW lighting/material/fog pipeline for free; future-proof for
  matfx/skin; closest to "what the engine actually did" without RE'ing every
  render function. Cons: a large dependency; its API shape would absorb the
  codebase; still not Mashed's exact pipeline (Criterion built per-game).

## Option B — RW-subset verbatim port (most faithful, most work)

- RE Mashed's actual render path (FUN_004c5xxx RW core + the 0x0042e/0x00473
  draw layers we already partially know) and port verbatim per the project
  DoD. The only option that makes renderer behavior *diff-checkable*.
- Cons: weeks of RE; the RW core is the vendored-library band we deliberately
  kept at C1.

## Option C — keep the custom D3D9 spike (current)

- Already renders 13/13 worlds + props + cars with fog/sky/prelight; tiny,
  dependency-free, fully understood.
- Cons: every fidelity gap (lighting, matfx, UV anim, render states) is
  hand-reimplemented forever; permanent divergence risk; none of it is
  diff-checkable against the original.

## Evidence from the spike

- The DATA layer is renderer-agnostic (parsers validated independently) —
  no option invalidates it.
- The look gap was dominated by MISSING FEATURES (fog/sky), not by D3D9
  mechanics: closing fog alone transformed the dump capture.
- Vehicle lighting and UV/material effects are the next gap class — exactly
  what librw's pipeline or an RW-subset port would provide and what the spike
  must hand-build.

## Recommendation

Hybrid: keep the spike as the DEV VIEWER (it is excellent for validating
data cracks), and adopt **Option B incrementally** for the shipping renderer —
RE Mashed's render functions demand-driven (lighting/material first), exactly
like every other subsystem, falling back to **Option A (librw)** only if the
RW core band proves impractical to port.

## Decision — RATIFIED 2026-06-10

User ratified the recommendation (Option B incremental, spike stays the dev
viewer, librw is the fallback). First Option-B port target per the divergence
ledger: vehicle lighting (`Vehicle_Shininess_Range` consumer, ledger #9).
