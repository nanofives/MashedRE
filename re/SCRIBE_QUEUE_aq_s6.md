# SCRIBE_QUEUE fragment — batch_aq session 6 (aq_s6)

Author-only promote-c2 pass (render campaign 1/~3, sixth slice
0x004e5f30..0x004e7fa0 — the RpAtomic/RpClump lifecycle + stream + plugin-thunk
tail of the RW-core band). Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep + re-classify) writes hooks.csv / trackers, mints
U-/S- IDs from the reserved ranges (U-8900..U-9199 / S-7000..S-7199), and
commits.

## Queued

2026-06-03  aq_s6  bucket=re/analysis/bucket_render_004e5f30_004e7fa0  confidence=C1->C2  rvas=NONE  subsystem_observed=third-party-library[renderware] (ALL 26)  library_skip=004e5f30:renderware,004e5f90:renderware,004e5fc0:renderware,004e6100:renderware,004e62f0:renderware,004e6470:renderware,004e65c0:renderware,004e6650:renderware,004e6760:renderware,004e67b0:renderware,004e6880:renderware,004e6e00:renderware,004e6fb0:renderware,004e7010:renderware,004e7060:renderware,004e7420:renderware,004e7cc0:renderware,004e7d40:renderware,004e7d70:renderware,004e7da0:renderware,004e7dd0:renderware,004e7df0:renderware,004e7e10:renderware,004e7e50:renderware,004e7e90:renderware,004e7fa0:renderware  needs_function_create=NONE  note=ENTIRE slice is the vendored RenderWare 3.x RpAtomic/RpClump module (create/destroy/interpolator/bounding-sphere/stream-read/plugin-registry thunks + RpLight create). 6 of 26 carry Ghidra-CONFIRMED RW names already (RpAtomicCreate/RpAtomicDestroy/RpAtomicGetWorldBoundingSphere/RpAtomicStreamRead/RpAtomicRegisterPlugin/RpClumpDestroy/RpClumpStreamRead/RpClumpForAllLights/RpClumpRegisterPlugin — batch_t s3 + batch_an renames). 0 plates authored; all 26 reclass-OUT to third-party-library[renderware], kept C1. Matches batch_aq header prediction. actual pool slot=Mashed_pool15 (pre-assigned; opened read-only cleanly first try, program_close'd at end).

## Notes for the central re-classify

- **Count**: 26 candidates, **0 plates** (none qualify as game/engine-glue). All
  26 are their OWN first-field `render,C1` rows in hooks.csv at session start
  (verified with anchored `^<rva>,` grep — no fuzzy-match drift, no rows >= C2).
  All 26 returned a function object and decompiled cleanly on Mashed_pool15;
  **no needs_function_create**. The sweep has NOTHING to plate from this
  fragment (sweep_build_manifest plated-count 0 == queued rvas=NONE is expected
  — ap precedent).

- **Existing-row pointers**: 24 rows carry the batch-t-s3 note "RW scenegraph
  CORE (RpWorld/RpClump/RpAtomic/RpLight/RwFrame); RW 3.7.0.2 version anchor"
  with plates under re/analysis/bucket_004e1ce0/. 004e7d40 has an
  rw_engine_init_d3 plate (cites S-0201). 004e5fc0 has an effects_particle_d4
  plate citing S-1960 + U-3578/U-3579/U-3580 — today's decomp confirms that
  plate's mechanical content (morph-keyframe lerp); the existing U-rows are
  data-semantic and stay with the kept-C1 library row.

- **LIBRARY-CONFIRM EVIDENCE (why all 26 are renderware, not game glue):**
  1. *RW engine-instance fn-table throughout*: alloc `(**(DAT_007d3ff8+0x118))`
     with memhints 0x30014 (atomic, at 0x004e67b0), 0x3000f (clump frame-list,
     inside 0x004e7420), 0x30007 (light, at 0x004e7fa0); free slots +0x11c
     (freelist) and +0x10c (heap) in every destroy path (0x004e6470,
     0x004e62f0, 0x004e6e00, 0x004e7cc0, 0x004e7010).
  2. *RW module object lists*: &DAT_00618664 (RpAtomic), &DAT_0061867c
     (RpClump), &DAT_006186a4 (RpLight) — the exact &DAT_00618xxx idiom in the
     batch header; FUN_004d8000/FUN_004d8060 add/remove on every create/destroy.
  3. *Plugin-offset globals*: DAT_007d7200 (atomic/clump freelist pair),
     DAT_007d722c / DAT_007d7230 (camera/light-in-clump link offsets, WRITTEN
     by the plugin-attach registrar 0x004e65c0 and READ by 0x004e6650,
     0x004e6760, 0x004e62f0, 0x004e6e00, 0x004e7420), DAT_007d7234 /
     DAT_007d725c (stream-read targets in 0x004e7e50), DAT_007d71fc /
     DAT_007d71d4 (stream plugin-callback scratch in 0x004e7060).
  4. *RW stream chunk machinery*: chunk-find FUN_004cc5e0 with the RW 3.7
     version window `0x35000 <= v <= 0x37002` checked verbatim in 0x004e7060
     and 0x004e7420; error path = FUN_004d7ff0(0x8000001a|0x80000004|
     0x80000013) + FUN_004d8480 (RwError set) — pure library error plumbing.
  5. *Struct layouts byte-exact vs vendored RW headers*: RpAtomic +0x10 sync
     callback / +0x14 geometry-ref / +0x18 morph-target / +0x1c..+0x28
     interpolated sphere / +0x2c..+0x38 world sphere / +0x40/+0x44 in-clump
     links / +0x48 render callback / +0x4c interpolator flags / +0x50/+0x52
     start/end morph target (int16) / +0x54/+0x58 time/recip-time floats
     (init 0x3f800000) / +0x5c position; RpClump +0x08 atomic list / +0x10
     light list / +0x18 camera list. RpAtomicCreate stamps defaults
     (flags 5, interpType 1) and installs in-module callbacks FUN_004e6880
     (sync) and FUN_004e5f90 (render default via engine-relative pipeline
     `*(DAT_00911ae0 + 0x3c + DAT_007d3ff8)`).
  6. *Zero game-state access*: across all 26 decompilations the only globals
     touched are the engine instance DAT_007d3ff8, the plugin offsets and
     module lists above, the RpLight default-color statics DAT_006186bc/c0/c4,
     and the engine-relative offset DAT_00911ae0. No 0x006xxxxx..0x009xxxxx
     mutable GAME globals, no entity pools, no application-page callees.
  7. *Named anchors interleaved*: RpClumpDestroy (0x004e6e00) is
     Ghidra-CONFIRMED (batch_an); RpAtomicCreate/Destroy/StreamRead/
     GetWorldBoundingSphere, RpClumpStreamRead/ForAllLights and both
     RegisterPlugin thunks carry batch-t-s3 RW renames. The unnamed 17 are
     their direct callees/callbacks/siblings on the same pages calling the
     same statics — one link unit.

- **Per-RVA cluster map** (for the re-classify notes column):
  - *Atomic lifecycle (4)*: 004e67b0 RpAtomicCreate (alloc +0x118 hint
    0x30014; installs 004e6880 + 004e5f90; adds to &DAT_00618664); 004e6470
    RpAtomicDestroy (geometry deref FUN_004d8bd0, frame detach FUN_004e8ea0 +
    conditional FUN_004c0e50 via FUN_004e4440, plugin-dtor FUN_004c0790,
    freelist free +0x11c); 004e7cc0 identical-body destroy returning param_1
    (the ForAllAtomics-callback variant used by RpClumpStreamRead error
    paths); 004e6880 default sync callback (resync interpolator if +0x4c bit1,
    set dirty bit0 at +3).
  - *Interpolator / bounds (2)*: 004e5fc0 morph-keyframe lerp — interpolates
    sphere center+radius between keyframes (0x1c-stride records at
    morphTarget+0x5c) by t = *(+0x5c) * *(+0x58), clears +0x4c bit1, sets +3
    bit0; 004e6100 RpAtomicGetWorldBoundingSphere — resyncs via 004e5fc0,
    transforms sphere by frame LTM (FUN_004c0ed0/FUN_004c3d60), scales radius
    by sqrt(max row-length²) via FastSqrt FUN_004c3b30, returns atomic+0x2c.
  - *Atomic render default (1)*: 004e5f90 — pipeline execute: uses atomic
    +0x6c pipeline else engine default `*(DAT_00911ae0 + 0x3c + DAT_007d3ff8)`,
    calls FUN_004d40d0(pipeline, atomic, 1).
  - *Clump lifecycle (3)*: 004e6e00 RpClumpDestroy (destroy all atomics in
    +0x08 list inline, unlink lights/cameras via DAT_007d7230/DAT_007d722c
    with per-node FUN_004e4d90/FUN_004c1cf0, frame FUN_004c0de0, freelist
    free); 004e62f0 byte-identical body returning void (destructor-callback
    variant); 004e6fb0 two-arg callback unlinking an atomic's +0x40/+0x44
    in-clump links and clearing +0x3c owner — ForAll-callback shape.
  - *Clump queries (2)*: 004e6650 walks clump+0x10 light list counting
    entries (stops if node == DAT_007d7230+4 — list-head sentinel quirk);
    004e6760 RpClumpForAllLights (walks +0x10 list, callback(node -
    DAT_007d7230 - 4, data), early-out on 0).
  - *Plugin/freelist registrar (1)*: 004e65c0 — registers two 0xc-byte
    0x10-aligned attach-plugins (light-in-clump via FUN_004c1cc0 ->
    DAT_007d722c, camera-in-clump via FUN_004e4bd0 -> DAT_007d7230), both
    with dtor FUN_004d7ff0-family callback args; returns success bool.
  - *Stream (5)*: 004e7060 RpAtomicStreamRead (chunk 1 struct + version
    window, frame-index fixup from caller-passed frame table param_2,
    geometry from in-stream chunk 0xf (FUN_004e9330) or caller geometry table
    param_3, plugin-data read FUN_004e1b60 + callback FUN_004e1c90);
    004e7420 RpClumpStreamRead (reads counts triple, FUN_004e6d80 clump
    create, frame hierarchy FUN_005c47e0, geometry list alloc hint 0x3000f,
    per-atomic RpAtomicStreamRead, RpLightStreamRead + camera FUN_005c4670
    attach via plugin offsets, plugin-data FUN_004e1b60; on every error path
    destroys via the 004e7cc0/FUN_004e6d00 callbacks); 004e5f30 stream-WRITE
    callback — two int32s from *(obj+0x6c)+0x2c/+0x30 via FUN_004cc770;
    004e7e90 same shape from *(obj+8)+0x2c/+0x30; 004e7e50 stream-READ of
    one-or-two int32s into the plugin globals &DAT_007d725c and (if size==8)
    &DAT_007d7234 via FUN_004cc790.
  - *Plugin-registry thunks (6)*: 004e7d40 RpAtomicRegisterPlugin ->
    FUN_004d7de0(&DAT_00618664,...); 004e7d70 RpClumpRegisterPlugin ->
    FUN_004d7de0(&DAT_0061867c,...); 004e7da0 -> FUN_004e1ac0(&DAT_00618664,
    4 args) [register-plugin-stream shape]; 004e7dd0 -> FUN_004e1b00
    (&DAT_00618664, 2 args); 004e7df0 -> FUN_004e1b30(&DAT_00618664, 2 args);
    004e7e10 -> FUN_004d7db0(&DAT_00618664, 1 arg) [get/validate-plugin-
    offset shape]. All six prepend an RW module-registry pointer and forward
    verbatim.
  - *Light create (1)*: 004e7fa0 — alloc +0x118 hint 0x30007 via plugin
    offset DAT_007d7260, stamps refcount/word +0x18=1, type 0, frame 0,
    flags 0xffffffff slot [1], color defaults from DAT_006186bc/c0/c4, adds
    to &DAT_006186a4 module list — RpLightCreate shape.
  - *Array teardown (1)*: 004e7010 — walks an (array,count) pair calling
    FUN_004e8ea0 (frame-detach/destroy) per entry, then frees the array via
    engine free +0x10c and nulls it — RpClumpStreamRead's geometry/frame
    list cleanup helper (called at 0x004e7060/0x004e7420 error paths).

- **Closest call**: NONE. Unlike batch_ao there is no context-coupled cluster
  here — every function is parameter-or-engine-static driven; no [UNCERTAIN]
  strong enough to force a C1->C2 plate. No new uncertainties filed; the
  pre-existing U-3578/U-3579/U-3580 on 004e5fc0 stay as-is on the kept-C1 row.

- **POOL SLOT**: pre-assigned Mashed_pool15 opened read-only first try (flat
  mashed_pool\ location per the .gpr gotcha; no LockException; image_base
  0x00400000 so RVAs map directly). `program_close` completed at end of
  session. Marker file `.pool_slot_aq_s6` records the slot.

- **Campaign effect**: render C1 drains by 26 via reclass-OUT (kept C1 under
  third-party-library[renderware]); C2-promotion count from this session is 0,
  as the batch_aq header predicted for the RW-core band.
