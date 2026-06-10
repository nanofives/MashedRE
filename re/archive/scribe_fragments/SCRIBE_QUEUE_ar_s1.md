# SCRIBE_QUEUE fragment — batch_ar session 1 (ar_s1)

Author-only promote-c2 pass. ALL-SKIP session: every candidate decoded as
vendored library code under the library-confirm rule — 36 RenderWare 3.x core
(RpGeometry/RpWorld stream+plugin tail and the RW D3D9 driver instance path)
plus 15 D3DX9 static-link residue past the calibrated PSGP band end (0x4fc9e0).
Zero plates authored. Central re-classify reclasses the library_skip list to
third-party-library[renderware] / third-party-library[d3dx9], kept C1.

## Queued

2026-06-03  ar_s1  bucket=re/analysis/bucket_render_004e8030_0050c897  confidence=C1->C2  rvas=NONE  library_skip=004e8030:renderware,004e8090:renderware,004e80c0:renderware,004e80f0:renderware,004e8120:renderware,004e8400:renderware,004e8440:renderware,004e8640:renderware,004e8750:renderware,004e8910:renderware,004e8940:renderware,004e89a0:renderware,004e89e0:renderware,004e8a10:renderware,004e8c70:renderware,004e8f50:renderware,004e8f80:renderware,004e8fb0:renderware,004e8fd0:renderware,004e9010:renderware,004e9080:renderware,004e9330:renderware,004e9850:renderware,004e98d0:renderware,004e9910:renderware,004e9950:renderware,004e99b0:renderware,004e9e40:renderware,004ea220:renderware,004ea9a0:renderware,004eb3c0:renderware,004eb9d0:renderware,004eb9e0:renderware,004eba40:renderware,004ebb00:renderware,004ebc30:renderware,004fcac3:d3dx9,00500c48:d3dx9,005010f1:d3dx9,00501ce4:d3dx9,0050272a:d3dx9,00502e9e:d3dx9,005033f4:d3dx9,00503a3a:d3dx9,00504008:d3dx9,0050570a:d3dx9,0050706f:d3dx9,00508772:d3dx9,0050a41e:d3dx9,0050b6ff:d3dx9,0050c897:d3dx9  note=subsystem_observed=third-party-library[renderware] x36 + third-party-library[d3dx9] x15; actual pool slot Mashed_pool1 (pre-assigned, opened clean read-only); 0 plates; 0 needs_function_create (all 51 have function objects, every decomp completed clean)

## Notes for the sweep

- **Count**: 51 candidates, 51 library_skip (36 renderware + 15 d3dx9),
  0 plates. All 51 were C1 in hooks.csv at session start (anchored `^<rva>,`
  grep — no drift, none already >= C2).
- **RW band 0x004e8030..0x004ebc30 (36 RVAs)** — evidence per cluster (all
  addresses from pool1 clone, session 2026-06-03):
  - `004e8030 / 004e8090 / 004e8120 / 004e8400 / 004e8440` — RpGeometry
    morph-target destroy / set / stream-read / stream-size / stream-write
    family on module list `&DAT_006186a4`: refcount short at obj+0x18
    (`(short)param_1[6]`), engine fn-table `DAT_007d3ff8` +0x118 alloc with
    memhint 0x30007 / +0x11c free via plugin offset `DAT_007d7260`, RW
    stream version window 0x35000..0x37002 with error idiom
    `FUN_004d7ff0(0x80000004)` + `FUN_004d8480`, stream helpers
    `FUN_004cc5e0/004cbd30/004cc580/004cbe80`, plugin-callback codec
    `FUN_004e1b60/004e1c90/004e1ce0/004e1d20`. No game globals.
  - `004e80c0 / 004e80f0` — thin wrappers passing `&DAT_006186a4` to
    `FUN_004d7de0` (callback set) / `FUN_004e1ac0` (plugin register).
  - `004e8640` — morph-target bounding-sphere calc: `FUN_004c1740` bbox over
    verts at target+0x14 count, half-extents via `_DAT_005cc32c` (0.5),
    `FUN_004c3b30` sqrt when > `DAT_005d757c`, scale `_DAT_005d8df0`. Pure
    RW data, .rdata consts only.
  - `004e8750` — RpGeometry morph-target-array grow: stride 0x1c records at
    geom+0x5c, count geom+0x18, native flag geom+8 & 0x1000000, engine
    fn-table +0x108 alloc (0x3000f) / +0x110 realloc, RW error 0x80000013.
  - `004e8910 / 004e8940 / 004e89a0 / 004e89e0` — triangle vertex-index
    setter (3 u16), triangle material setter via matlist
    `FUN_004f3cb0/004f3be0` at geom+0x20, ForAllMaterials iterator over
    geom+0x20/+0x24, geometry-unlock flag-or at geom+0xc with mesh rebuild
    free `FUN_004f0c10` of geom+0x54.
  - `004e8a10` — RpGeometry mesh-list (RpMeshHeader) build: dedupe arrays
    alloc'd via engine fn-table +0x108 with memhints 0x10006/0x10503, mesh
    add `FUN_004f0c50`, finalize `FUN_004f3b00`, free +0x10c.
  - `004e8c70 / 004e8f50 / 004e8f80 / 004e8fb0` — RpGeometryCreate-shape
    (numVerts cap 0xffff -> RW error 6; flags 0xff0000 texcoord-count
    nibble; module list `&DAT_006186d0`; matlist init `FUN_004f3bc0`;
    `FUN_004e8750(geom,1)` single morph target) + this module's
    callback-set / plugin-register / stream-callback-register thin wrappers
    (`FUN_004d7de0` / `FUN_004e1ac0` / `FUN_004e1b00` on `&DAT_006186d0`).
  - `004e8fd0 / 004e9010 / 004e9080 / 004e9330 / 004e9850` — RpGeometry
    stream-size / body-size / stream-write / stream-read / destroy family
    on `&DAT_006186d0`: chunk ids 0xf/1/8 with version 0x37002, matlist
    stream `FUN_004f3ce0/004f3d40/004f3e90`, byte-swap loops on 8-byte
    tri records, refcount short at geom+0xe, full-destroy frees +0x5c
    morph block and matlist `FUN_004f3b60` then engine free +0x10c.
  - `004e98d0 / 004e9910 / 004e9950` — stream callbacks reading/writing 4-byte
    plugin words `&DAT_007d7278/7274` and `&DAT_007d727c/7270`, and a
    2x4-byte reader into `*(obj+0x7c)+0x2c/+0x30` via `FUN_004cc770`.
  - `004e99b0 / 004e9e40 / 004ea220` — RpWorld stream-read family: version
    window checks per chunk (1/8/9/10), arena carve-out via `DAT_0061864c`
    (world module list) + `DAT_006189e4` (sector size), calls Ghidra-named
    `RpWorldDestroy` / `RpWorldInstance` / `RwFrameDestroy` /
    `RwFrameDestroyHierarchy`, plane-sector recursive reader (004ea220
    self-recurses + mutual-recurses with 004e9e40 atomic-sector reader),
    pre-0x36002 16-bit index byte-swap fixup loop. No game globals.
  - `004ea9a0 / 004ebc30` — RW D3D9 driver instance path: batch_ar plan
    already cites aq_s1 confirming FUN_004ea9a0 as RW d3d9 driver.
    Vertex-declaration build over D3DDECLUSAGE-style element records,
    vertex-buffer create/free via `FUN_004dc5b0/004dcaa0/004dcb80/004dc750`,
    morph-range interpolation `FUN_004f6870/004f57e0/004f71f0/004f79f0`,
    qsort `FUN_005c24e0`. Globals touched are 0x0091xxxx D3D9 driver caps
    words (`DAT_00911fbc` & 0x10000, `DAT_0091208c` & 0x80,
    `DAT_00912074` & 1) — RW d3d9 raster/caps module data, NOT game state.
  - `004eb3c0 / 004eb9d0` — pure leaves returning CSL pipeline-node
    descriptors literally named `PTR_s_nodeD3D9AtomicAllInOne_csl_00618708`
    / `PTR_s_nodeD3D9WorldSectorAllInOne_csl_00618768` — definitive RW
    d3d9 pipeline nodes.
  - `004eb9e0 / 004eba40 / 004ebb00` — index-range min/max scan (pure leaf),
    tri-list sort wrapper over `FUN_005c24e0` stride 6, tristrip->trilist
    degenerate-skipping converter with base-offset rebias (pure leaf, no
    globals at all).
- **D3DX9 cluster 0x004fcac3..0x0050c897 (15 RVAs, odd-aligned, past PSGP
  band end 0x4fc9e0)** — the batch_ar d3dx9 watch CONFIRMED for all 15:
  - `004fcac3` — 4x4*4x4 float matrix multiply with aliasing-safe local temp
    (D3DXMatrixMultiply shape; full 16-float matrices, x87, no globals).
    NOTE: batch_aa s3 hooks.csv note calls it "RwMatrixMultiply" — the
    operand is a full 4x4 (16 floats), not an RW 4x3+flags matrix, and the
    fn touches zero RW module lists; d3dx9 tag is the better fit. Naming
    question only; library verdict identical either way.
  - `00500c48` — vertex-decl semantic parser: literal "POSITION",
    "BLENDWEIGHT", "BLENDINDICES", ... "SAMPLE" table mapping to
    D3DDECLUSAGE 0..0xd, HRESULT 0x80004005 (E_FAIL), CRT
    `_isalpha/_isdigit/_atol/_strncmp`.
  - `005010f1 / 00501ce4 / 00502e9e / 005033f4 / 00503a3a / 00504008` —
    shader-assembler family: dest-modifier parser (table PTR_DAT_005db9f0,
    0x44-stride, 0x54 opcodes), instruction token encoder (literal error
    strings "source modifiers are not allowed on destination parameters",
    "...vs_1_1" etc.), register-name parser (table PTR_DAT_005dd0d0, 21
    entries), YACC action dispatcher + yyparse (callees exclusively in this
    family: FUN_005000c2 error sink, FUN_00501ce4, FUN_00502e9e,
    ShaderCompiler::AST FUN_00511774, operator_new), and the
    D3DXAssembleShader entry: `GetModuleHandleA/LoadLibraryA("d3d9.dll")` +
    `GetProcAddress("Direct3DShaderValidatorCreate9")`, shader-version
    switch on 0xfffe0101..0xffff03ff, "vs_1_0 is no longer supported"
    diagnostics. 0050272a additionally cites
    `PTR_s_D3DX9_Shader_Assembler_0061bca8` and GUBD magic 0x47554244.
  - `0050570a / 0050706f` — PSGP vertex-skinning kernels: bone stride 0x40,
    rcpps Newton-Raphson w-divide, 4-vertex unrolled SIMD transform with
    scalar-tail fallback `FUN_00508252`; no RW or game globals.
  - `00508772 / 0050a41e / 0050b6ff / 0050c897` — PSGP quaternion/matrix
    kernels: rsqrtss NR normalise with lazy-init guard words
    `_DAT_007db0f0/0e0/0d0` (0.5 / 3.0), and three 3DNow! packed-float
    kernels (PackedFloatingADD/MUL/Accumulate/CompareGT + pfrcp/pfrsqrt
    iteration helpers + `FastExitMediaState` femms) for squad/catmull-rom,
    quaternion SRT 4x4 build, Euler->matrix — the SSE/SSE2/3DNow!
    dual-path PSGP dispatch shape per memory project_d3dx9_psgp_band.
- **No game-state reads/writes** (0x006xxxxx-0x009xxxxx game globals) in any
  of the 51 — the only 0x009xxxxx touches are the RW d3d9 driver's own caps
  words listed above.
- **No U-IDs / S-IDs minted** (author-only). One bare [UNCERTAIN] recorded:
  [UNCERTAIN] 004fcac3 master-name "RwMatrixMultiply" (batch_aa s3 note)
  vs D3DX 4x4 multiply shape — naming question only, NON-BLOCKING, RVA is
  library_skip either way; rename decision left to sweep/re-classify.
- No hooks.csv / STUBS / UNCERTAINTIES / DEFERRED / shared SCRIBE_QUEUE
  writes. Master Ghidra untouched (read-only pool1 clone, program_close
  done).

## Drained

drained-by=sweep-20260604-0020; 0 plates, 0 bookmarks, 0 renames (ALL-SKIP: library_skip, rvas=NONE)
