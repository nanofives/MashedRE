# B5b — RenderWare Physics 3.7 identification + qhull-2002.1 vendor (2026-07-14)

Lane B5b (ROADMAP §WS-B, D1 Option A). Inherits B5a (`B5a_SYSTEM2_PLATING_2026-07-14.md`,
commit e56ee005). This note records the **decisive static discovery** made while vendoring qhull:
*"system 2" is RenderWare Physics 3.7 in its entirety* — and pins the qhull version to stock
**2002.1 (2002/8/20)**.

Anchor: `original/MASHED.exe(.unpatched)` SHA-256
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`. NO-GUESSING: every claim
below is a literal string embedded in that binary; RVA-level function↔file mappings are marked
`[B5a-confirmed]` (proven by a B5a plate) vs `[name-inferred]` (RCS filename only, not yet
tied to an RVA).

## 1. "System 2" == RenderWare Physics 3.7 (Criterion middleware)

MASHED.exe embeds 45 RCS `$Id:` strings from Criterion's RenderWare Physics Perforce depot,
branch **`//Physics/Rwp37Active/`** (one straggler from `//Physics/dev/`). This is RenderWare
**Physics** 3.7 — a separate Criterion product from RenderWare **Graphics** 3.x (the render
engine already anchored in CLAUDE.md). The whole "qhull/RW-Physics island + 0x55-band integrator"
of B5a is this module. Complete file inventory (verbatim, sorted):

| RWP source file | RCS rev | Subsystem role | Known RVA anchor |
|---|---|---|---|
| `qhull/src/RwpQHullWrapper.c` | #1 | **the qhull bridge** = `FUN_0057ca30` | `0x0057ca30` [B5a-confirmed] |
| `core/volume/src/RwpConvexHull.c` | #1 | cone-cast convex-hull build | `0x00481e00`/`0x004826d0` chain [B5a-confirmed via plate] |
| `core/volume/src/RwpConvexHullMassProps.c` | #1 | hull inertia/mass properties | [name-inferred] |
| `core/volume/src/RwpConvex.c` | #1 | convex volume type | [name-inferred] |
| `core/volume/src/RwpGjk.c` | #2 | **GJK support/extreme-point** = `FUN_0055c000` | `0x0055c000` [B5a-confirmed] |
| `core/volume/src/RwpVolume.c` | #1 | volume base | [name-inferred] |
| `core/volume/src/RwpVolumeUtils.c` | #1 | volume helpers | [name-inferred] |
| `core/volume/src/RwpBox.c` | #1 | box primitive | [name-inferred] |
| `core/volume/src/RwpSphere.c` | #1 | sphere primitive | [name-inferred] |
| `core/volume/src/RwpCylinder.c` | #1 | cylinder primitive | [name-inferred] |
| `core/volume/src/RwpCapsule.c` | #1 | capsule primitive | [name-inferred] |
| `core/volume/src/RwpPlane.c` | #1 | plane primitive | [name-inferred] |
| `core/volume/src/RwpTriangle.c` | #1 | triangle primitive | [name-inferred] |
| `core/volume/src/RwpTrilist.c` | #2 | triangle-list (track soup) volume | [name-inferred] |
| `core/volume/src/RwpGeneralPrimitive.c` | #1 | primitive dispatch | [name-inferred] |
| `core/volume/src/RwpFixedVolume.c` | #1 | fixed/static volume | [name-inferred] |
| `core/volume/src/RwpAggregate.c` | #1 | aggregate volume | [name-inferred] |
| `core/volume/src/RwpAggChildren.c` | #1 | aggregate children | [name-inferred] |
| `core/volume/src/RwpNull.c` | #1 | null volume | [name-inferred] |
| `core/volume/src/RwpGrid.c` | #3 | spatial grid broadphase | [name-inferred] |
| `core/volume/src/RwpBatch.c` | #1 (+`//Physics/dev/…#6`) | contact batch | [name-inferred] |
| `core/context/src/RwpMgr.c` | #1 | physics **world/context mgr** (`DAT_006ce274`) | [name-inferred] |
| `core/context/src/RwpContext.c` | #1 | context | [name-inferred] |
| `core/context/src/RwpDyn.c` | #1 | dynamics context | [name-inferred] |
| `core/context/src/RwpObj.c` | #1 | physics object | [name-inferred] |
| `core/context/src/RwpPipeline.c` | #2 | solver pipeline | [name-inferred] |
| `core/context/src/RwpJoint.c` | #1 | joint | [name-inferred] |
| `core/context/src/RwpRefPair.c` | #1 | reference/contact pair | [name-inferred] |
| `core/solverutils/src/RwpIntegrate_vanilla.c` | #1 | **per-tick integrator** (B5c target) | 0x55-band [B5a-confirmed set] |
| `core/solverutils/src/RwpConstraints.c` | #1 | constraint solve | [name-inferred] |
| `core/solverutils/src/RwpPartition.c` | #1 | island partition | [name-inferred] |
| `core/solverutils/src/RwpMatrixCap.c` | #2 | matrix-cap solver util | [name-inferred] |
| `core/farfield/src/RwpFarfield.c` | #1 | farfield/broadphase | [name-inferred] |
| `core/utils/src/RwpMath.c` | #1 | RWP math | [name-inferred] |
| `body/src/RwpBodyObj.c` | #2 | **rigid body object** (the 4 proxy bodies) | `DAT_006c9a78[0..3]` [B5a-confirmed] |
| `body/src/RwpBodyDef.c` | #1 | body definition | [name-inferred] |
| `body/src/RwpBodyStd.c` | #1 | standard body | [name-inferred] |
| `ragdoll/src/RwpRagdollObj.c` | #2 | ragdoll object | [name-inferred; likely unused by Mashed] |
| `ragdoll/src/RwpRagdollDef.c` | #1 | ragdoll definition | [name-inferred] |
| `ragdoll/src/RwpRagdollStd.c` | #1 | standard ragdoll | [name-inferred] |

**Implications for the D1 plan (Option A) — the plan is unchanged, only sharpened:**
- The D1 brief (`COLLISION_GATE_BRIEF_D1_2026-07.md:16,58`) already framed Option A as
  "vendor real qhull-2002.1 **+ reconstruct the RW-Physics call surface**". This note confirms
  and names that surface. No scope surprise.
- Only the `qhull/` subtree is public-domain and VENDORABLE. Every `Rwp*.c` file is proprietary
  Criterion code — we **reimplement it clean-room from our own decomp** (exactly the re3 /
  gta-reversed methodology already used for RenderWare Graphics), we do NOT vendor it.
- The RCS filenames are a **free naming key** for the whole 0x55/0x56/0x57 band: as B5c/B5d
  plate each band function, tag it to its `Rwp*.c` owner (e.g. the integrator quartet =
  `RwpIntegrate_vanilla.c`; the support map `FUN_0055c000` = `RwpGjk.c`).

## 2. qhull version pinned: stock 2002.1 (2002/8/20)

- Embedded banner string `"2002.1 2002/8/20"` (`s_2002_1_2002_8_20_00625e04`, referenced from
  the qhull error path `FUN_0058f6a0` / `FUN_0059adb0`; see `re/console/pc_features.csv:5064`).
  This is upstream qhull's own `qh_version`, and it is **unmodified** — RWP embedded stock qhull
  and only wrapped it (in `RwpQHullWrapper.c`), it did not fork the numeric core.
- The "2003, all rights reserved" string in the binary is the **Criterion/RWP** copyright, NOT
  qhull's — do not misread it as qhull 2003.1.
- qhull 2002.1 is **public domain** (`COPYING.txt`: Qhull is free, no copyleft; redistribution
  clean). Vendored source = the official `qhull/qhull` git tag `2002.1`
  (github.com/qhull/qhull, tag `2002.1`, tarball SHA verified to contain
  `README.txt` banner "qhull, rbox  2002.1  August 20, 2002").

## 3. Vendored under `mashedmod/deps/qhull-2002.1/`

- `src/*.c` (20) + `src/*.h` (10) from the tag, plus `COPYING.txt`/`README.txt`/`Announce.txt`/
  `REGISTER.txt` for provenance. Read-only reference; do NOT port these — they are the vendored
  library.
- **Build**: `build_qhull.bat` compiles only the 12-file **library subset**
  (`geom geom2 global io mem merge poly poly2 qhull qset stat user`) → `qhull_2002_1.lib`
  (484,548 bytes, verified 2026-07-14). The CLI drivers (`qconvex/qdelaun/qhalf/qvoronoi/rbox/
  unix`) and examples (`user_eg/user_eg2`) each carry their own `main()` and are EXCLUDED.
- **Bit-identity lever**: compiled `/arch:IA32 /fp:precise` (x87, NOT SSE2) to reproduce the
  original ~2004 MSVC x87 rounding. qhull is famously FP-rounding sensitive, so this is required
  before any bit-identical-hull claim (same convention as the RW-math leaves, memory
  `project-wsa2-rwmath-bitident`).

## 4. Public-API mapping (bridge `FUN_0057ca30` → qhull-2002.1), from B5a plate

| Decomp callee | qhull-2002.1 symbol | Signature (from `qhull.h`/`qset.h`) | Confidence |
|---|---|---|---|
| `FUN_0058f520(3,count,pts,0,opt,x,x)` | `qh_new_qhull` | `int qh_new_qhull(int dim,int numpoints,coordT*points,boolT ismalloc,char*qhull_cmd,FILE*outfile,FILE*errfile)` | strong (7-arg shape + dim=3 + "qhull s Pp" cmd) |
| `FUN_005834a0(set)` | `qh_setsize` | element count of a `setT*` | strong (used to sum `facet->vertices` sizes) |
| `FUN_00589dc0(0)` | `qh_freeqhull` | `qh_freeqhull(boolT allmem)`; arg `0`=`!qh_ALL` | strong (canonical teardown pair) |
| `FUN_0058f0a0(&a,&b)` | `qh_memfreeshort` | `qh_memfreeshort(int*curlong,int*totlong)` | strong (canonical `qh_freeqhull; qh_memfreeshort` seq) |

Game-side (NOT qhull — port clean-room): `FUN_0057c670` (build output hull table from the qhull
facet list `qh facet_list`=`DAT_0091459c`, walk `facet->next`, `facet->vertices`),
`FUN_00563840` (output buffer alloc — `[UNCERTAIN]` game allocator vs `qh_memalloc`; resolve at
port time), `FUN_00563810` (body activate).

## 5. Bit-identity acceptance — CAPTURED + DIFFED (DOCUMENTED DIVERGENCE)

Frida capture (`re/frida/capture_qhull_hull.py`) warped stock MASHED into a race
(track 0, mode 10, 1 car) and captured all **4** `FUN_0057ca30` calls at track load
(the 4-body build fires during the phase-2→3 load transition, gated `DAT_0086caa0==0x7b`):
- All 4 bodies: **n=117 input points**, output slab **14508 bytes**. The 4 captured
  input clouds are **byte-identical** (MD5 equal) — the 4 proxy bodies share one shape.
- Saved to `log/b5b_qhull_capture/body{0..3}.{cloud,slab}.bin` (reproducible; gitignored).

Offline replay (`Collision/b5b_qhull_selftest.cpp` — vendored qhull-2002.1 REALfloat=1 +
ported bridge, `deps/qhull-2002.1/build_b5b_selftest.bat`) fed each captured cloud back
through `QhullBuildHull` and diffed the output slab vs the captured original:

**Key config discovery — RWP built qhull with `REALepsilon = 1e-6` (not stock `FLT_EPSILON`).**
The first replay diverged (nFacets 226 vs 230). Reading the ORIGINAL's live `qh_qh` precision
globals at `qh_new_qhull` onLeave (`re/frida/capture_qhull_hull.py`, `FUN_0058f520` hook)
proved the original's `qh DISTround = 2.9622792681e-6` — **exactly 8.389× (= 1e-6/FLT_EPSILON)
larger** than the stock-`FLT_EPSILON` build's `3.531e-7`. `DISTround` is the coplanar-merge
threshold; the loosened epsilon merges 4 more near-coplanar facets. Setting
`#define REALepsilon 1e-6` in the vendored `user.h` (2nd required edit) makes the vendored
build compute `DISTround = 2.96227927e-6` — matching the original to full float precision.

Result **after** the `REALepsilon=1e-6` fix (all 4 bodies):

| field (slab offset) | original | vendored+ported | verdict |
|---|---|---|---|
| numpoints (input) | 117 | 117 (byte-identical cloud) | **MATCH** |
| nVerts (`+0x1c`) | 117 | 117 | **MATCH** |
| vertex array (`+0x8c`, 117×vec3) | — | — | **BIT-IDENTICAL** |
| nFacets (`+0x1e`) | 226 | 226 | **MATCH** |
| nEdges (`+0x20`) | 682 | 682 | **MATCH** |
| `qh DISTround` | 2.9622792681e-6 | 2.96227927e-6 | **MATCH** |
| facet normals (`+0x700`, 226×vec3) | — | — | **≤1 ULP** (maxAbsDiff = 1.19209e-7 = FLT_EPSILON; facet order identical) |

**Residual = the irreducible sub-ULP cross-compiler FP floor.** With the epsilon matched, the
hull is vertex-bit-identical, count-identical, and facet-order-identical; the only remaining
difference is the facet plane **normals**, which agree to **≤1 ULP** (`qh_setfacetplane`'s
Gaussian elimination rounds sub-ULP differently between the MSVC-2022 and the ~2003 build).
This is the same 1-ULP FP-codegen floor already accepted as C4 for the RW-math leaves
(memory `project-wsa2-rwmath-bitident`), not a triangulation/config divergence.

**Verdict:** the vendored qhull (REALfloat=1 **+ REALepsilon=1e-6**, x87) + ported bridge
reproduce the original's 4 body hulls to **bit-identical vertices + counts + facet order, with
normals matching to ≤1 ULP** — functionally bit-identical, residual at the irreducible FP
floor. The ported bridge/packaging are faithful (given identical qhull facets they produce an
identical slab). Not claiming byte-perfect C4 (the ≤1-ULP normal residual is real and cited);
this is the strongest reproduction achievable across compilers and far exceeds the
"documented divergence" fallback.

## 6. Status
- Deliverable #1 (vendor qhull-2002.1 + build, REALfloat=1 x87) **DONE + verified**.
- Deliverable #2/#3 (port bridge + 4-body build chain) **DONE + compile-verified**;
  qhull path runs end-to-end (self-test: valid hull from a point cloud).
- Acceptance **DONE — reproduced to the FP floor.** After discovering RWP's
  `REALepsilon=1e-6` (proven by the original's live `DISTround`), the vendored+ported hull is
  **vertex-bit-identical + count-identical + facet-order-identical**, normals ≤1 ULP. Two
  vendored-source edits total (`REALfloat=1`, `REALepsilon=1e-6`), both cited. Residual = the
  irreducible sub-ULP cross-compiler FP floor (not a divergence to close).
- Remaining for the FULL system-2 (B5c/B5d): port the RWP integrator
  (`RwpIntegrate_vanilla.c`, 0x55-band) + GJK support (`RwpGjk.c` `FUN_0055c000`) + the
  material/world helpers extern-declared in `CollisionBuildDeps.h`, then wire the build
  chain into build.bat and behaviorally verify (B5e).
