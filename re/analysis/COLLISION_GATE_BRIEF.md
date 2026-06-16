# Collision / RW-Physics gate decision brief (WS-B B1, 2026-06-16)

The ROADMAP WS-B stop-and-ask gate: how should the standalone source the
per-frame **car↔world** and **car↔car** contacts that feed the WS-A vehicle
physics (`FUN_0046ddb0`)? The phrasing in the workstream is **"vendor real
qhull-2002.1 vs port the used contact subset."** This brief establishes, with
live Ghidra evidence, *where qhull is actually used* — which reframes the
decision — then states options + a recommendation. The decision is the user's.

Anchored to MASHED.exe SHA-256
BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
(Ghidra pool3, read-only, session 2026-06-16). Every RVA below was verified by
MCP this session. Reframes [[qhull-rwphysics-island]] (which established the
island bounds + provenance) by tracing the island's *single external bridge*.

## Headline finding

**MASHED runs two distinct collision systems, and qhull is in neither one's
per-frame path:**

1. **First-party per-wheel / contact-solver system** (0x0046–0x0047 band) — the
   contacts WS-A consumes. Hand-rolled SAT / half-plane tests on triangle-soup
   terrain + tiny fixed 4-vertex vehicle hulls. **Calls only RW math leaves —
   zero qhull, zero RW-Physics 0x55-band.**
2. **RenderWare Physics 3.7 rigid-body world** (0x55 band + the qhull island
   0x57c5b0+) — a separate physics world (`DAT_006ce274`) with 4 collision
   bodies (`DAT_006c9a78[0..3]`). qhull builds those 4 bodies **once at
   scene-setup**, gated by a rebuild flag; the per-tick world step advances them
   through the 0x55 band, **never re-entering qhull**.

**The contact subset WS-B must port (B2 car↔world, B3 car↔car) requires no
qhull. qhull is reachable only through the scene-setup convex-hull build.**

## Evidence — the per-frame contact path is qhull-free

The runtime contact chain (mapped in `physics_collision_d4_breadth/REPORT.md`,
re-verified this session). Callee sets, none of which enter the qhull island
(0x57c5b0..0x5a5820) or the RW-Physics 0x55 band:

- `FUN_004709a0` (substep broad-phase) → `00467300`, `00469aa0`, `00469df0`,
  `0046e9e0`, `0046ef70`, `0046f6c0`, `004a2c48`, `0040e340/350`. All first-party.
- `FUN_0046f6c0` (wheel contact solver) → `0046c5f0`, `0046cc40`, `004a2c48`,
  `004a3384`, `004c39b0` (normalize), `004c3d90`, `004c4d20` (matrix), `004c52f0`,
  `00538c80`. RW **math/core** leaves only — no 0x57c5b0+.
- `FUN_00469aa0` (contact history) → `00468d80` (terrain), `004694e0` (object),
  `004c3d90`, `0040e350`. First-party.
- `FUN_00469df0` (car↔car) — reads the vehicle's own **4-vertex hull** at struct
  +0xA28..0xA54 and runs a 4-half-plane SAT by hand; callees are only
  `004c39b0/004c3ac0/004c3df0/004c4dc0` (RW math) + `0040e350`
  (`vehicle_dynamics_d2/00469df0.md`).

The per-car contact arrays the WS-A force integrator reads
(`DAT_008815a4` and siblings): all **15** references come from the
0x0046xxxx–0x0047xxxx first-party band (none from qhull or the 0x55 band). The
terrain-geometry list the solvers iterate (`DAT_0088e60c`) is **written** by
first-party `0x0046f7fb` / `0046db9c` / `00468d73` and read only by 0x46-band
code — i.e. the collision triangles come from the track collision world
(`COLLI*.BSP`, already parsed in R3), **not from qhull**.

## Evidence — qhull's single use-site is the load-time hull build

Tracing the only function that consumes the `qh_new_qhull` option string
(`"qhull s Pp"` @ `0x0062406c`):

```
FUN_00480340                         (physics-scene init)
  └ FUN_0047f840                     creates RW-Physics world DAT_006ce274 (WRITE @0x0047f88b)
                                     + sets rebuild flag DAT_0086caa0 = 0x7b (WRITE @0x0047f92f)
FUN_00470c70 (per tick)
  └ FUN_0047eb30  (RWP37 world step) reads DAT_0086caa0 @0x0047eb65:
        if (DAT_0086caa0 == 0x7b) {              ← one-shot gate
            FUN_0047d3c0(DAT_006ce274, ...);     ← THE qhull build
            DAT_0086caa0 = 0;                    ← clears flag (WRITE @0x0047eba7)
        }
        ... per-tick: FUN_0055dff0 / 0055ac00 / 0055deb0 (RW-Physics 0x55 step) — NO qhull ...
     └ FUN_0047d3c0   loop ×4: build 4 collision bodies into DAT_006c9a78[0..3]
        └ FUN_004826d0  "creates a collision body + collision mesh from a physics object"
           ├ FUN_00481e00  cast ~120 rays (FUN_0055c000) from a point, hull the hit cloud
           │   └ FUN_0057ca30   ← qhull entry (qh_new_qhull, options @0x0062406c)
           ├ FUN_00482140  build collision mesh from the hull
           └ FUN_00563810  free the hull
```

Key facts (all verified this session):
- `FUN_0057ca30` (the qhull option-string consumer) has **exactly one** external
  caller: `FUN_00481e00`. One bridge.
- The build (`FUN_0047d3c0`) runs **only** when `DAT_0086caa0 == 0x7b`, which is
  set by the physics-world *init* (`FUN_0047f840`) and cleared after a single
  build. So qhull executes at **scene/physics-world setup**, not per-frame.
- The per-tick portion of `FUN_0047eb30` advances the RW-Physics bodies through
  the 0x55 band (`FUN_0055dff0/0055ac00/0055deb0`) and reads back collision-body
  transforms (`FUN_0057c210`) — it does **not** re-enter qhull.
- `FUN_00562460` (also a one-shot, in the same gate) → `RpWorldForAllWorldSectors`
  (`0x004e5c70`): the world step's static collision is the **RW world BSP**, not
  qhull.

[UNCERTAIN] I traced the single qhull entry that references the option string and
confirmed its one external caller. I did **not** exhaustively enumerate every
island function's external callers, so "qhull is reached *only* via
`FUN_00481e00`" is asserted for the convex-hull-build path (the documented
`RwpQHullWrapper.c` path) but not proven for all ~165 KB of the island. Path to
resolution: a range-caller sweep over 0x57c5b0..0x5a5820 if the decision below
ever depends on it (it does not for B2/B3).

## What WS-B is replacing (the scaffold)

The standalone today drives the car on a **`COLLI*.BSP` triangle-soup ground
raycast** (R5 opener, `D3d9Render/TrackRenderer` + `Track` collision-world load)
— a single down-ray per car. B2/B3 replace that with the real per-wheel +
car↔car contact solvers (system 1 above), whose geometry source
(`COLLI*.BSP`) we already parse.

## Options

### Option A — Vendor real qhull-2002.1
Compile Brad Barber's public-domain qhull-2002.1 (the exact version anchored at
`s_2002_1...` `0x00625e04`) and wrap the single `FUN_0057ca30` bridge.
- **Needed only if** the standalone must reproduce **system 2** (the RW-Physics
  rigid-body world + its 4 qhull-built collision bodies) faithfully.
- Pros: bit-faithful hull construction for free; public domain; the build is
  **load-time, not perf-critical**; Quickhull is notoriously hard to reimplement
  correctly (degenerate/coplanar handling), so vendoring beats hand-porting.
- Cons: pulls in the whole RW-Physics 0x55-band call surface as a dependency
  (the bodies are useless without the 0x55 integrator that steps them); large;
  only justified if system 2 proves load-bearing for handling fidelity.

### Option B — Port the used contact subset (no qhull)
Port B2 (`FUN_00468d80` terrain, `FUN_004694e0` object, `FUN_0046f6c0` +
`FUN_0046cc40`/`FUN_0046c5f0` wheel) and B3 (`FUN_00469df0` car↔car) verbatim.
- Dependencies: RW math leaves (`004c3df0/004c39b0/004c3ac0/004c4d20/004c4dc0` —
  mostly already ported per WS-A2) + the `COLLI*.BSP` triangles (already parsed).
- **Requires zero qhull, zero 0x55-band.** This is the WS-A contact source.
- Cons: does not, by itself, reproduce system 2; if diff-original later shows the
  rendered car motion genuinely depends on the RW-Physics body world, Option A
  comes back for that piece.

These are **not mutually exclusive for the same purpose** — they serve different
collision systems. B2/B3 are Option B regardless.

## Recommendation

**Take Option B now; defer the qhull/Option-A question to WS-A.**

1. Port the used contact subset (B2/B3) verbatim — it needs no qhull and is the
   direct contact source for `FUN_0046ddb0`. This unblocks the WS-A critical path.
2. During WS-A8 (`diff-original` of per-frame velocity/pos vs the original on
   matched inputs), determine empirically whether the car transform the renderer
   draws is driven by the first-party solvers (system 1) or genuinely needs the
   RW-Physics body world (system 2).
3. **Only if** WS-A telemetry shows system 2 is load-bearing for fidelity, revisit
   Option A — and at that point **vendor real qhull-2002.1** (build-and-wrap at
   the single `FUN_0057ca30` bridge) rather than reimplement Quickhull, since the
   build is load-time and Quickhull is high-risk to hand-port.

This keeps the expensive qhull/RW-Physics-middleware dependency out of the
critical path until evidence proves it necessary — consistent with the
demand-driven ROADMAP v2 model.

## Decision — RATIFIED 2026-06-16

User ratified **Option B**: port the used contact subset now (B2/B3) as
first-party code — it requires no qhull — and **defer** the qhull / RW-Physics
rigid-body-world question to WS-A. qhull-2002.1 stays library-tagged and
un-ported. If WS-A8 `diff-original` telemetry later proves the RW-Physics body
world (system 2) is load-bearing for handling fidelity, revisit by **vendoring
real qhull-2002.1** (build-and-wrap at the single `FUN_0057ca30` bridge), not by
reimplementing Quickhull.

**Unblocks:** B2 (port `FUN_00468d80` terrain + `FUN_004694e0` object +
`FUN_0046f6c0`/`FUN_0046cc40`/`FUN_0046c5f0` wheel) and B3 (`FUN_00469df0`
car↔car), each its own session ending in a `diff-original`. Dependencies are RW
math (WS-A2, mostly ported) + the already-parsed `COLLI*.BSP` triangles. No
qhull, no 0x55-band.
