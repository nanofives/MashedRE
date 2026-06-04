# SCRIBE_QUEUE fragment — batch_ar session 4 (ar_s4)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep + central re-classify) writes hooks.csv / trackers.

## Queued

2026-06-03  ar_s4  bucket=re/analysis/bucket_render_00540260_00549620  confidence=C1->C2  rvas=NONE  note=ALL-SKIP 50/50 vendored RenderWare (MatFX D3D9 pipeline + RpUserData 0x11f + plugin-0x105 + RpSpline 0x102 + RpPatch 0x123); library_skip=00540260:renderware,005402d0:renderware,00540340:renderware,00540510:renderware,005405c0:renderware,005412d0:renderware,00541b50:renderware,00541d40:renderware,005422c0:renderware,00543710:renderware,00543a40:renderware,00543b10:renderware,00543b20:renderware,00543b30:renderware,00543b90:renderware,00543bb0:renderware,00543d40:renderware,00543d70:renderware,00543da0:renderware,00543dc0:renderware,00543df0:renderware,00543e00:renderware,00543e10:renderware,00543e30:renderware,00543e50:renderware,005449f0:renderware,00544a70:renderware,00544ad0:renderware,00544bf0:renderware,00544d20:renderware,00545260:renderware,005459c0:renderware,00545d30:renderware,00546320:renderware,00546380:renderware,00546530:renderware,00546890:renderware,00546bf0:renderware,00546c50:renderware,00546cb0:renderware,00546d10:renderware,00547230:renderware,005492a0:renderware,005493d0:renderware,005493e0:renderware,00549580:renderware,005495a0:renderware,005495b0:renderware,00549610:renderware,00549620:renderware; actual pool slot=Mashed_pool13 (pre-assigned; opened read-only cleanly first try with program_name="MASHED.exe")

## Notes for the sweep / central re-classify

- **Count**: 50 candidates → 0 plated, 50 library_skip (kept C1, reclass-OUT to
  third-party-library[renderware]). 0 drift-skips (all 50 confirmed `render,C1` via
  anchored `^<rva>,` grep at session start). 0 needs_function_create (every RVA has a
  function object; all 50 decompiled cleanly on Mashed_pool13).
- **library_skip evidence (per-RVA, library-confirm rule)** — the slice decomposes
  into five RW toolkit/plugin families, each confirmed by the canonical RW idioms
  (engine fn-table DAT_007d3ff8 +0x108/+0x10c/+0xf4/+0xcc/+0x118/+0x11c with
  0x3xxxx memhints, plugin-offset globals, RW error idiom FUN_004d7ff0+FUN_004d8480,
  plugin registrar FUN_004c2d90/FUN_004e8f50/FUN_004e8f80):
  - **RW MatFX D3D9 material pipeline** (0x00540260..0x00543b20, 11 RVAs): every fn
    addresses material plugin data via `*(DAT_007dc5dc + obj)` (MatFX plugin-offset
    global) and drives the D3D device DAT_007d4110 vtable (+0x144 DrawPrimitive,
    +0x148 DrawIndexedPrimitive, +0x170 SetVertexShader/FVF, +0x1ac SetPixelShader,
    +0x1b4 SetPixelShaderConstantF, +0x164 SetTransform-adjacent) through the rwd3d9
    state cache (DAT_006181c8/cc/d0/d4 — the &DAT_00618xxx RW module band) and TSS
    runs via FUN_004d54f0. 00540260/005402d0 scan the per-material 0x18-stride
    effect-record table for effect types 4/5. 00540340/00540510 single/dual-pass
    draw with env-map frame matrix staged at DAT_00623d38 → FUN_004cb330(0x10,..)
    (SetTransform). 005405c0 (0xd0c bytes) is the multi-pass dual-texture dispatcher
    keyed on effect-type pairs (9/1, 5/6, 9/3, 2/2, 1/5) under caps flags
    DAT_007dc614/618/61c/620/624. 005412d0 env-map+alpha dual-pass renderer (calls
    00541b50, 00540340; pixel-shader handles DAT_007dc600/604). 00541b50 env-map
    texcoord-gen setup: TSS 0xb (TEXCOORDINDEX) = 0x30000 CAMERASPACEREFLECTIONVECTOR
    or 0x10000 CAMERASPACENORMAL, TSS 0x18 (TEXTURETRANSFORMFLAGS) 3/2, camera LTM
    via FUN_004c0ed0(*(*DAT_007d3ff8+4)) negated into the stage transform — the
    rwd3d9 MatFX env-map idiom verbatim. 00541d40 bump-pass renderer (dynamic-VB
    Lock/Unlock FUN_004dcbb0/FUN_004dcd30 — the aq_s2-confirmed renderware
    suballocator — plus FVF 0x252 via vtable+0x164). 005422c0 (0x123a bytes)
    bump-map vertex-buffer builder: locks source VB/IB via COM vtable +0x2c/+0x30,
    vertex-element decode through FUN_004f7dc0/FUN_004f81b0, normalize via
    FUN_004c3b90 (RwInvSqrt), engine malloc/free DAT_007d3ff8[0x42]/[0x43].
    00543710 module init: pipeline create ladder FUN_004d4170/FUN_004d4dd0/
    FUN_004d4f90/FUN_004d4380 (+0x2c=0x120 pipeline id), im3d pipes from
    FUN_004eb3c0/FUN_004eb9d0, caps probe FUN_004cbb60 (+0x94 MaxSimultaneousTextures
    → DAT_007dc624; +0x90 caps bits 0x1000000/0x10 → DAT_007dc618/61c; +0xcc
    PixelShaderVersion > 0x100 gates FUN_004cbb20 pixel-shader creation from
    bytecode blobs DAT_005e4580/45e0/4630/4670/46f8/47a8 → handles
    DAT_007dc5fc..610). 00543a40 symmetric teardown (FUN_004d41e0 pipeline destroy +
    FUN_004cbb50 shader release). 00543b10/00543b20 attach pipelines DAT_007dc5f4/f8
    to object +0x6c/+0x7c. All globals are the MatFX module block DAT_007dc5dc..634
    + rwd3d9 state cache 0x00618xxx/0x00623dxx — no game state.
  - **RpUserData toolkit, plugin ID 0x11f** (0x00543b30..0x00543e50, 12 RVAs):
    00543e50 is the registrar already C1-noted by rw_engine_init_cont1 (D-0245) —
    FUN_004c2d90(0,0x11f,..) engine-plugin + 14 per-object-type registrations
    (FUN_004e8f50/8f80 geometry, FUN_004f0910/0940, FUN_004c1710 frame,
    FUN_005c4640/4780, FUN_004c1cc0, FUN_004e4bd0/4c00, FUN_004e80c0/80f0,
    FUN_004c5da0, FUN_004cf430) filling handle block DAT_007dc634..668. 00543bb0
    slot-table allocator (memhints 0x3011f/0x30002, strlen/strcpy via fn-table
    +0xf4/+0xcc); 00543b90/00543da0 zero-shim thunks onto it; 00543d40 count
    non-empty slots at DAT_007dc634+off; 00543d70/00543dc0 bounds-checked slot
    accessors (DAT_007dc634/DAT_007dc63c); 00543df0/00543e00/00543e10/00543e30
    4-byte element get/get-float/set/set on slot data +0xc; 00543b30 two-object
    merge guard (FUN_005c4ab0 validity check ×2 → FUN_0053e830). Pure toolkit, no
    game globals.
  - **RW plugin 0x105 family** (0x005449f0..0x00544d20, 5 RVAs): 00544d20 registrar
    — decomp literally shows the FidDB-named `RpAtomicRegisterPlugin(0xc,0x105,..)`
    plus FUN_004c2d90(0,0x105) and geometry registrations FUN_004e8f50/8f80 →
    plugin-offset handles DAT_007dc66c (atomic) / DAT_007dc670 (geometry). 005449f0
    per-geometry record-array (re)alloc (0x14-stride, memhint 0x30105); 00544a70
    record writer (rate field + _DAT_005cc320/rate reciprocal, ring-links last entry
    to base); 00544ad0/00544bf0 bind/seek a record onto the atomic-side block
    (+0x50/+0x52/+0x54/+0x58/+0x5c writes with RwFrame dirty-propagate FUN_004c0e50
    on +0x4c flag word; RW error idiom code 0x105 on missing geometry). All access
    via plugin-offset globals; no game state.
  - **RpSpline toolkit, plugin/error code 0x102** (0x00545260..0x00547230, 12
    RVAs): 00546380 spline ctor (type 1 open → alloc N*0xc+0x50, type 2 closed →
    N*0xc+0x5c, memhint 0x3000c, object tag *p=0xc, init via FUN_00545340, control
    points copied to +0x14; RW error idiom 0x102) — the track_loader_d4 D-5755 note
    cites it as a game callee, but the fn itself is the vendored spline ctor.
    00545260 stream deserializer (0x28 header via FUN_004cbd30 + FUN_004cc790 read,
    memhint 0x10102, builds via 00546380, memcpy fn-table +0xd0). 00546320 dtor
    (frees +0x14/+0x10/self via fn-table +0x10c). 00546530 registrar
    FUN_004c2d90(0,0x102,..) (plugin registrar ID 0x102, rw_engine_init_cont1
    D-0251). 00546890 N×N float basis-matrix builder (memhint 0x30102, recurrence
    piVar[i+2]=piVar[i+1]*4-piVar[i]). 005459c0/00545d30 parametric curve evaluators
    over interpolated basis LUTs DAT_007dc6a8..6c4 (+6c8..6d4 second-derivative set)
    with normalize via FUN_004c3910/FUN_004c39b0 and modes 10/0xb (error idiom 0x102
    on bad mode) — 00545d30 additionally builds the full position+tangent/normal/
    binormal frame matrix (FUN_004c4530 identity init). 00546bf0/00546c50/00546cb0
    the Shoemake matrix→quaternion X/Y/Z-largest branch triplet (FUN_004c3b30 sqrt,
    consts _DAT_005cc320=1.0/_DAT_005cc32c=0.5 by use); 00546d10 batched
    quaternion-rotate over a vec3 array; 00547230 Möller-Trumbore ray-triangle
    intersect (epsilon _DAT_005e4574, reciprocal _DAT_005cea1c). Pure math/toolkit;
    zero globals outside RW static const pages 0x005cxxxx/0x005e4xxx.
  - **RpPatch toolkit, error code 0x123** (0x005492a0..0x00549620, 8 RVAs):
    005493e0 patch-mesh ctor — flag/count-driven sizing ((bits 0/1/2 of *p →
    pos/normal/prepos arrays ×3, +texcoord sets ×2 via p[4]) ×p[1] + p[3]*0x11 +
    p[2]*0xb dwords + 0x60 header, memhint 0x30028, RW error idiom 0x123, lock-state
    init FUN_004f3bc0, refcount p[0x11]=1, flags +0x40=0xffff). 005492a0 element
    accessor (+0x48 array). 005493d0 getter off plugin-offset global DAT_007dc724
    (+8). 00549580 lazy-instance getter (+0x54, builds via FUN_00549aa0). 005495a0
    refcount++ (+0x44); 005495b0 refcount-- with instance release via FUN_004e8ea0
    (the ao_s2-reclassed renderware refcount trio) and final free (FUN_004f3b60
    lock destroy + fn-table +0x10c). 00549610 flags|=v (+0x40); 00549620 rebuild-if-
    dirty (low-16 of +0x40 → FUN_00549b20) then clear. batch_x s4 already mapped
    this page as "RpPatch + RW-plugin glue"; the refcount/lazy-instance/dirty-flag
    shapes match the RpPatch atomic/mesh API. No game globals.
- **Zero game-state reads/writes across all 50**: every global cited is the RW
  engine fn-table (DAT_007d3ff8), the D3D device slot (DAT_007d4110), plugin-offset
  /module blocks (DAT_007dc5dc..724), the rwd3d9 render-state cache + staging
  matrix (DAT_006181c8..d4, DAT_00623d30..3c — the &DAT_00618xxx RW module band per
  the library-confirm rule), or RW static const/shader-blob pages
  (0x005cxxxx/0x005d7xxx/0x005e4xxx). No 0x006xxxxx..0x009xxxxx game globals, no
  entity pools, no application-page data flow observed in any decomp body.
- **Watches from the batch header resolved**: the 0x00546xxx "math-library page"
  is the RpSpline evaluator + Shoemake/Möller-Trumbore math cluster as predicted
  (00546bf0/00546c50/00546cb0/00546d10 confirmed vendored-band callees, tag
  renderware). No d3dx9-shaped dispatch (no SSE/PSGP dual paths) anywhere in the
  slice.
- **Bucket dir** re/analysis/bucket_render_00540260_00549620/ intentionally NOT
  shipped — 0 plates (aq_s2/ap ALL-SKIP precedent: fragment only).
- **Pool note**: Mashed_pool13 (pre-assigned) opened read-only cleanly on the FIRST
  attempt (FLAT mashed_pool\ location, program_name="MASHED.exe" passed) despite
  the expected stray on-disk .lock from aq_s4's failed attempt. program_close called
  at end. POOL_SLOT_FILE=.pool_slot_ar_s4 written (untracked marker).
- No hooks.csv / STUBS.md / UNCERTAINTIES.md / DEFERRED.md / re/SCRIBE_QUEUE.md
  writes. No master-project mutation. No U-/S- IDs minted (reserved U-9200..U-9499 /
  S-7200..S-7399 are for central re-classify; nothing needed — no plates).
