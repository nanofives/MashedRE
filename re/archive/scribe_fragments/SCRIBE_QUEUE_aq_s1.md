# SCRIBE_QUEUE fragment — batch_aq session 1 (aq_s1)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep + central re-classify) writes hooks.csv / trackers.

## Queued

2026-06-03  aq_s1  bucket=re/analysis/bucket_render_00401690_004dc5b0  confidence=C1->C2  rvas=00401690,004492b0,00490500  note=subsystem_observed=render (all 3 plated CONFIRMED render, application pages); needs_function_create=004492b0; library_skip=004c1940:renderware,004c1980:renderware,004c3e20:renderware,004c3e90:renderware,004c5e00:renderware,004c5f60:renderware,004c78a0:renderware,004c78e0:renderware,004cbca0:renderware,004cbd00:renderware,004cd810:renderware,004cd850:renderware,004cd900:renderware,004cdb60:renderware,004d9000:renderware,004d9030:renderware,004d9040:renderware,004dc300:renderware,004dc370:renderware,004dc410:renderware,004dc4a0:renderware,004dc530:renderware,004dc5b0:renderware; actual pool slot=Mashed_pool1

## Notes for the sweep / central re-classify

- **Count**: 26 candidates → 3 plated C1->C2, 23 library_skip (kept C1,
  reclass-OUT to third-party-library[renderware]). 0 drift-skips (all 26
  confirmed C1 via anchored `^<rva>,` grep at session start).
- **needs_function_create=004492b0** (SkyDomeRender): instruction stream
  present 0x004492b0..RET 0x0044942a but NO function object in pool1
  (pre-existing S-1760). Plated from the full raw listing dump this
  session; the sweep's function_create should use body 0x004492b0..0x0044942a.
- **library_skip evidence (per-RVA, library-confirm rule, decomp/listing
  read in pool1 2026-06-03)**:
  - 17 rwID-module open/close pairs WITHOUT function objects (LAB_ labels
    only; listing dumps read this session). All share the RW 3.x module
    open/close idiom: engine-global base `[0x007d3ff8]` + per-module
    globals-offset slot (0x007d3ec0 camera, 0x007d3ffc vector, 0x007d4054
    texture, 0x007d40a8 raster, 0x007d45c4 stream, 0x00911d00/0x007d4600
    immediate, 0x007d4628 image, 0x007d6c64 chunkgroup, 0x007d6c94 color),
    freelist create/destroy via 0x004cc9e0/0x004cc9f0 with rwID-band
    memhints (0x40005 camera, 0x40401 vector, 0x40006 texture, 0x40407
    raster, 0x40404 stream, 0x40018 image), fn-table alloc/free at
    +0x108/+0x10c, module instance counters inc/dec (e.g. 0x007d3ec4,
    0x007d40ac, 0x007d45c8, 0x00911d04, 0x007d6c68, 0x007d6c94). Matches
    librw_plugin_compat/REPORT.md plugin table rows (ids 0x401, 0x404,
    0x405, 0x406, 0x407, 0x408, 0x40a, 0x40d, 0x412). The two
    Im2D-adjacent RVAs flagged by the mission (004cd810/004cd850) decode
    as the IMMEDIATEMODULE close/open pair (0x1d-dword globals block,
    helper calls 0x004e1990/0x004e1960) — module lifecycle, NOT game Im2D
    glue.
  - 004dc300/004dc370/004dc410/004dc4a0/004dc530 — V3d x matrix transform
    family on the padded RwMatrix layout (reads rows [0..2],[4..6],[8..10],
    skips pad/flag columns) with the 0xf pointer-alignment dual path
    (PSGP-style aligned/unaligned dispatch); zero direct callers (vector
    fn-table targets); no game globals. Batch_y plates in
    re/analysis/bucket_004d7ac0/ (rw-d3d9-matrix-*) remain as historical
    C1 notes.
  - 004dc5b0 — dynamic vertex-buffer suballocator keyed on stride; D3D9
    device global DAT_007d4110 vtable +0x68 create call (usage=8, pool=1),
    engine fn-table +0x118 alloc, arenas DAT_007d70b0..bc; ALL 5 callers
    sit in the RW d3d9 driver band (FUN_004ea9a0, FUN_004f13d0,
    FUN_004f1b00, FUN_00530c00 x2 — the PTank band ao_s2 confirmed
    vendored). Batch_y plate (rw-d3d9-vb-pool) remains as historical C1
    note.
  Per the library-confirm rule none were renamed.
- **Plated 3 (all render, application pages — the mission's three
  most-likely-genuine RVAs)**:
  - 00401690 — mesh AABB + center/extent + reciprocal-magnitude writer
    (object in ESI, fields [1..13]); calls FUN_004b3f90 resolver +
    Vec3Magnitude 0x004c3ac0 (C3 hook); caller FUN_004025f0 (0x00402xxx
    boot/asset chain).
  - 004492b0 — SkyDomeRender: camera begin/end-update bracket, 12
    render-state calls via engine fn-table [0x007d3ff8+0x20], renders
    clumps [arg+0x100f0] (null-checked) and [arg+0x10130] (NOT null-
    checked) via FUN_004e6680; dispatched only via fn-ptr slot 0x005f34ac
    (U-1768 stands).
  - 00490500 — per-frame effect-ring update (3 pools 0x00766xxx-0x00771xxx,
    6-pool flush via FUN_00476df0); deepens the 2026-05-02
    effects_particle plate, which re-verified exactly; size corrected
    0x943 → 0x944.
- **Uncertainties**: bare [UNCERTAIN] markers in plates only — NONE minted
  (author-only). Pre-existing IDs referenced, not duplicated: U-1768,
  U-0587..U-0589, S-1760, S-0580..S-0594. New bare markers are
  data-semantic (undumped 0x005cxxxx float globals, struct-chain
  semantics, render-state id meanings, ESP+0x14 arg index) —
  NON-BLOCKING for C2.
- **Stubs**: noted per-plate, NONE minted. New unminted callee of note:
  FUN_004671a0/FUN_004840d0/FUN_00451060/FUN_00489240 (application
  callees of SkyDomeRender, several already tracked from SESSION_MMMM).
- **Pool slot**: pre-assigned Mashed_pool1 opened read-only first try
  (session 2e1ff3d2), program_close called. Recorded in .pool_slot_aq_s1.
- No hooks.csv / STUBS.md / UNCERTAINTIES.md / DEFERRED.md /
  re/SCRIBE_QUEUE.md writes. No master-project mutation. No U-/S- IDs
  minted (reserved U-8900..U-9199 carried over per batch plan are for
  central re-classify).

## Drained

drained-by=sweep-20260603-2220; 3 plates, 3 bookmarks, 0 renames, 1 master function_create (FUN_004492b0 body 0x004492b0..0x0044942a, resolves the S-1760 no-fn-object hold)
