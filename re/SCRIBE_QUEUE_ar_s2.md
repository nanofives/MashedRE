# SCRIBE_QUEUE fragment — batch_ar session 2 (ar_s2)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep + central re-classify) writes hooks.csv / trackers.

## Queued

2026-06-03  ar_s2  bucket=re/analysis/bucket_render_0050d089_00538c80  confidence=C1->C2  rvas=NONE  note=ALL-SKIP 51/51 vendored (6 d3dx9 static-link residue past the PSGP calibrated band + 45 RenderWare: RtAnim interpolator core, plugin-0x116 MatFX-family, RpSkin D3D9 CSL pipeline incl. LITERAL nodeD3D9SkinAtomicAllInOne_csl, PTank plugin-0x12f, plugin-0x12a world-sector BVH/collision family); library_skip=0050d089:d3dx9,0050e1dd:d3dx9,0050e8b7:d3dx9,0050ef6d:d3dx9,0050eff8:d3dx9,005112c9:d3dx9,0052d8e0:renderware,0052df70:renderware,0052e0e0:renderware,0052e2c0:renderware,0052e310:renderware,0052e650:renderware,0052ea70:renderware,0052eb40:renderware,0052eeb0:renderware,0052fd60:renderware,0052fdb0:renderware,0052fe90:renderware,0052ff00:renderware,0052ffd0:renderware,00530000:renderware,00530010:renderware,00530050:renderware,00530090:renderware,005300d0:renderware,00530160:renderware,005302c0:renderware,00530650:renderware,00530c00:renderware,00532000:renderware,005320b0:renderware,005327d0:renderware,00532a60:renderware,00532b80:renderware,005333a0:renderware,005336d0:renderware,00533d00:renderware,00533ec0:renderware,00534870:renderware,00534a80:renderware,00535330:renderware,005356f0:renderware,00535700:renderware,00535910:renderware,00538310:renderware,00538600:renderware,00538a80:renderware,00538b70:renderware,00538ba0:renderware,00538bd0:renderware,00538c80:renderware; actual pool slot=Mashed_pool9 (pre-assigned; opened read-only cleanly first try, program_name="MASHED.exe" at FLAT mashed_pool location)

## Notes for the sweep / central re-classify

- **Count**: 51 candidates → 0 plated, 51 library_skip (kept C1, reclass-OUT to
  third-party-library[d3dx9] x6 + third-party-library[renderware] x45). 0
  drift-skips (all 51 confirmed `render,C1` via anchored `^<rva>,` grep at
  session start). 0 needs_function_create (every RVA has a function object; all
  51 decompiled cleanly on Mashed_pool9; 005356f0 is a Ghidra thunk to
  FUN_004e6920).
- **library_skip evidence (per-RVA, library-confirm rule)**:
  - **d3dx9 odd-aligned cluster (6)** — the batch-header d3dx9 watch CONFIRMS:
    all six sit past the PSGP calibrated band end (0x4fc9e0), are not
    0x10-aligned, and decode as the D3DX9 3DNow!/MMX packed-float path
    (`PackedFloatingMUL/ADD/SUB/SUBR/NegAccumulate/PosNegAccumulate`,
    `PackedSwapDWords`, `FloatingReciprocalAprox` + 2 Newton iterations,
    `FastExitMediaState` = femms), .rdata consts DAT_005db288/298/2a8/2ac only,
    ZERO RW or game globals: 0050d089 (quaternion+rotation-center+scaling →
    4x4 matrix, the D3DXMatrixTransformation shape; null-param identity paths),
    0050e1dd (matrix inverse, parallel-cofactor 3DNow! path w/ optional
    determinant out-param + CompareEQ-0 singularity gate), 0050e8b7 (matrix
    inverse, second cofactor variant returning NULL on singular), 0050ef6d
    (vec3 by 4x4 transform + W-divide = D3DXVec3TransformCoord shape), 0050eff8
    (4x4 x 4x4 multiply = D3DXMatrixMultiply shape), 005112c9 (in the
    project-named `ShaderCompiler::` namespace — StringPool FUN_00510e2f / AST
    FUN_0051105d/FUN_0051120b callees, `_tolower/_isdigit/_atol` CRT, D3DX
    shader-assembler error 0xb56 with LITERAL format string "Constant variable
    '%s' bound to register greater than 8191 (%d requested)").
  - **RtAnim-style interpolator core (3)** — 0052df70 (subtract-anim-time:
    keyframe linked-list rewind below current time + per-channel apply via
    callback table param_1[0x11], loop/done callbacks param_1[3]/param_1[6],
    .rdata float DAT_005d757c; NO game globals), 0052e0e0 (add-anim-time
    forward variant, duration wrap via *(anim+0xc), same callback scheme),
    0052e2c0 (set-current-time dispatcher: delta vs *(param_1+4) → calls df70
    on negative / e0e0 on positive). Generic toolkit state only.
  - **Plugin-0x116 family / MatFX-shaped (10)** — 0052e310 (registrar: ID 0x116
    via FUN_004c2d90 + already-named RpAtomicRegisterPlugin + stream-callback
    registrars FUN_004e7da0/dd0/df0 + FUN_004e8f50/80, plugin offsets →
    DAT_007dc500/504/508, RW error fn FUN_004d7ff0 as default), 0052d8e0
    (zero-size registrar ID 0x1b7, single FUN_004c2d90, strictly-positive
    guard), 0052e650 (stream-rights callback: plugin data at
    DAT_007dc508+offset, FUN_004c2dc0 plugin-offset lookups 0x120/0x12e,
    forwards to FUN_00530000), 0052ea70 (palette byte-dedup helper, pure
    pointer math, no globals), 0052eb40 (composite buffer alloc via engine
    fn-table DAT_007d3ff8+0x108 with memhint 0x30116 = malloc-hint|pluginID),
    0052eeb0 (plugin slot setter on DAT_007dc508 w/ FUN_004d7ff0 destroy +
    FUN_0052fdb0 re-sort), 0052fd60 (module shutdown: iterate pipeline handles
    &DAT_007dc524..0x7dc534, FUN_004d41e0 pipeline-destroy), 0052fdb0 (4-key
    sort network over plugin record at DAT_007dc508+off; FUN_005c24e0 callee),
    0052fe90 (stream WRITE: chunk header via FUN_004cc580 type 0xfffffff4 ver
    0x37002 + two 4-byte writes FUN_004cc770 of tag 9 + plugin dword),
    0052ff00 (stream READ counterpart: FUN_004cc5e0 header, RW 3.5–3.7 version
    window check `0x35000..0x37002`, RW error idiom FUN_004d7ff0(0x80000004) +
    FUN_004d8480 module-error struct {0x116, code}, freelist alloc via
    fn-table+0x118 on DAT_007dc518).
  - **RpSkin D3D9 CSL pipeline (12)** — 005327d0 (pipeline constructor
    registering LITERAL `nodeD3D9SkinAtomicAllInOne_csl`
    (PTR_s_..._00623c30); caps probe FUN_004cbb60: vs/ps version words +0xc4/
    +0xcc >= 0x101, max-VS-const +0xc8 → matrix budget (n-12)/3 →
    DAT_007dc540/544/548/54c), 0052ffd0 (module attach gate: flag bit0 →
    DAT_007dc524 = FUN_005327d0()), 00530000 (store default pipeline
    DAT_007dc524 into atomic+0x6c), 00530010/00530050 (HW-skin feature gates
    over DAT_007dc540/544/548 + plugin data at DAT_007dc508+off — inverse
    predicates of each other), 00530090 (instance-data free via
    fn-table+0x10c, zero +0x24..+0x38), 005300d0/00530160/005302c0 (skin chunk
    stream write / read (alloc memhint 0x30116, fn-table +0x108/+0x10c) / size
    accessor — table of [9]/[10]/[11] counts + payload), 00530650 (vertex
    stream rebuild: vertex-descriptor walk terminator 0x11, D3D9 VB Lock flags
    0x2800/0x800, repack helpers FUN_004f5030/57e0/71f0/77f0/79f0/8490/8660,
    weight clamp vs _DAT_005cc320 = 1.0f), 00530c00 (D3D9 vertex-declaration
    builder: element types 2/4/5..0xe, usage indices, caps byte +0xec, VB
    create FUN_004dc5b0/FUN_004dcaa0, FUN_004cb8a0 decl finalize), 00532000/
    005320b0 (batch matrix*vec3 skin transforms, direct + per-vertex
    byte-indexed 0x40-stride palette; pure math, no globals).
  - **Anim/skin math + stream leaves (5)** — 00532a60 (quaternion→4x4 matrix,
    flags dword 0x3 = 4.2039e-45, _DAT_005cc320=1.0f), 00532b80 (quaternion
    SLERP + lerped translation: dot, sign flip vs DAT_005d757c, minimax
    arcsin/sin polynomial ladders over .rdata consts _DAT_005d8dc0..005d8dec /
    _DAT_005cc8f4..005cc908, FUN_004c3b30 = FastSqrt), 005333a0 (stream-read
    of count*0x24-byte keyframe records via FUN_004cc790 + prev-pointer fixup
    /0x24*9), 00533d00 (interpolator alloc via fn-table+0x118 freelist
    DAT_0091246c memhint 0x3011e + 0x40-aligned matrix cache +0x108), 00533ec0
    (per-frame matrix-palette assembly: hierarchy flags &2/&0x1000/&0x2000/
    &0x4000, push/pop stack walk, FUN_004c4600 matrix-multiply, inlined
    FUN_00532a60 fast path, dirty-list link into DAT_007d3ff8+0xbc).
  - **Plugin-0x11e registrar (1)** — 005336d0 (FUN_004c2d90 ID 0x11e +
    FUN_004c1710 (8-byte plugin → DAT_00912468) + FUN_005c4780 stream
    callbacks; pure attach, no game state). NOTE: DAT_00912468 is in the
    0x009xxxxx page but is a registrar-minted RW plugin offset, not game data.
  - **PTank plugin-0x12f family (5)** — 00534a80 (registrar: FUN_004c2d90 ID
    0x12f → _DAT_007dc580 + RpAtomicRegisterPlugin → DAT_007dc57c — page-mate
    of ao_s2-confirmed PTank 00534b60/00534d00), 00534870 (ring-buffer
    random-stream read on plugin block DAT_007dc578+DAT_007d3ff8 with
    start/cur/step/end pointers, >>1 result), 00535330 (PTank vertex-format
    allocator: 9 channel flags bit0..bit8 → strides 0xc/0x40/0xc/8/4/0x10/4/
    0x10/0x20, alloc fn-table+0x108 memhint 0x3012f), 00535700 (per-channel
    buffer selector switch on bitmask 1..0x100 over plugin block at
    DAT_007dc57c+off, dirty-OR +0x3c on write-lock flag 0x40000000), 00535910
    (commit pending: move +0x3c mask into +0x40). 005356f0 = thunk to
    FUN_004e6920 (RW core destructor: module list &DAT_00618664, FUN_004d8060/
    FUN_004d8bd0/FUN_004e8ea0/FUN_004c0790, freelist free via fn-table+0x11c on
    DAT_007d7200 plugin offset) — body is RW core, tag renderware.
  - **Plugin-0x12a world-sector BVH/collision family (6)** — 00538600
    (registrar ID 0x12a: FUN_004e5cd0 0x2c-byte world-sector plugin →
    DAT_00912484 + FUN_004e5d00 stream cbs + FUN_004f0910 0x68-byte geometry
    plugin → DAT_00912488 + FUN_004f0940 stream cbs; zeroes DAT_00912480),
    00538310 (recursive 2-way BVH node split writing AABB pairs at plugin
    offset DAT_00912488; leaf extent magnitude via FUN_004c3ac0 =
    Vec3Magnitude), 00538a80 (set DAT_00912480 + read plugin slot at
    DAT_00912484+off), 00538b70/00538ba0 (freelist walkers freeing via
    fn-table+0x10c; next-ptr at +0x58 / +0xc), 00538bd0 (enumeration entry:
    already-named RpWorldForAllWorldSectors(world, LAB_00538c30, &acc); null
    world → RW error idiom FUN_004d7ff0(0x80000016) + FUN_004d8480 {0x12a,
    code}), 00538c80 (bounding-volume dispatcher switch on type word
    param_2[6]: 1→FUN_00538d60, 3→FUN_00539ec0, 4→FUN_00539900,
    5→RpAtomicGetWorldBoundingSphere + FUN_00539ec0; all callees are this
    plugin's page — type-5 case uses the already-named RW export). NOTE:
    DAT_00912480/84/88 are registrar-minted plugin offsets in .bss, same class
    as DAT_007dcxxx — no game-entity access anywhere in the family.
- **subsystem_observed**: third-party-library[d3dx9] (6) /
  third-party-library[renderware] (45) for all rows; none remain render-glue.
- **Stubs encountered**: none authored (no plates).
- **Uncertainties**: none minted (author-only; no bare [UNCERTAIN] markers
  needed — every skip decision is library-confirmed by named RW exports,
  literal strings, RW error/stream/fn-table idioms, or D3DX PSGP shape).

## Drained

drained-by=sweep-20260604-0020; 0 plates, 0 bookmarks, 0 renames (ALL-SKIP: library_skip, rvas=NONE)
