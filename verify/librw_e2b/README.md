# E2'b — first geometry rendered through librw

`world_probe_arctic.png` — Arctic's `GRAPH.BSP` parsed by our `Track::World`
loader, constructed into `rw::Geometry`/`rw::Atomic` by `LibRw/RwSceneBuild.cpp`,
and drawn by librw's D3D9 backend. Produced by `RenderWorldProbe()` under
`MASHED_RENDER_LIBRW=1`; the camera is a deterministic overview derived from the
world bbox (centre `(-14.96, 4.45, -7.93)`, radius `130.18`).

## What this shows

The landmass, harbour structures, cranes and tower are recognisably the same
track that appears in the `verify/librw_ref` D3D9 shots, and the surfaces are
textured (the teal rock / water-edge tones match). Together with the structural
self-test — 16229 verts and 16480 tris built, exactly the parser's totals, with
zero out-of-range vertex indices or matIds — that is the E2'b gate: the geometry
and texture bridges produce a correct scene.

## What this is NOT

**Not an E3' parity shot, and it must not be imgdiff'd against
`verify/librw_ref`.** This draws the STATIC WORLD ONLY — no car, props, copters,
particles, pickups, HUD or menu — while those baselines are full-game frames.
Comparing them would be apples-to-oranges. Real parity waits until the librw
submit path covers everything the D3D9 path draws.

The subject fills ~4% of the frame because the overview camera sits at roughly
1.5x the bbox extent. That is framing, not a defect; tighten the camera if a more
legible reference is wanted.
