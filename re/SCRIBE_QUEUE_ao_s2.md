# SCRIBE_QUEUE fragment — batch_ao session 2 (ao_s2)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep + central re-classify) writes hooks.csv / trackers.

## Queued

2026-06-03  ao_s2  bucket=re/analysis/bucket_gameplay_004e4800_00558030  confidence=C1->C2  rvas=00554010,00554150,00554200,00554390,00555830,00556780,00556e40,00557110,005572b0,005572c0  note=subsystem_observed=hud(font-vector 2D text/path module) for all 10 plated; library_skip=004e4800:renderware,004e4d90:renderware,004e6710:renderware,004e68a0:renderware,004e6920:renderware,004e6d00:renderware,004e6d80:renderware,004e6f80:renderware,004e6fe0:renderware,004e8e90:renderware,004e8ea0:renderware,00534b60:renderware,00534d00:renderware,00557ec0:renderware,00557fb0:renderware,00558030:renderware; actual pool slot=Mashed_pool3 (pre-assigned Mashed_pool2 threw a real LockException — likely in-JVM poisoned; record per header fallback rule)

## Notes for the sweep / central re-classify

- **Count**: 26 candidates → 10 plated C1->C2, 16 library_skip (kept C1, reclass-OUT to
  third-party-library[renderware]). 0 drift-skips (all 26 confirmed `gameplay,C1` via
  anchored `^<rva>,` grep at session start). 0 needs_function_create among candidates.
- **library_skip evidence (per-RVA, library-confirm rule)**:
  - 004e4800/004e4d90/004e6710/004e68a0/004e6920/004e6d00/004e6d80/004e6f80/004e6fe0/
    004e8e90/004e8ea0 — RenderWare RpClump/RwFrame/geometry CORE page (batch_an already
    confirmed FUN_004e45b0=RwFrameRemoveChild, FUN_004e6e00=RpClumpDestroy on this page).
    All are thin engine primitives with NO game-state access: plugin-offset globals
    (DAT_007d7174/007d722c/007d7230), engine-global alloc/free table slots
    (DAT_007d3ff8+0x108/0x10c/0x118/0x11c), module object lists (DAT_0061862c/00618664/
    0061867c/006186d0), freelist alloc memhint 0x30010 (chunk-tag 0x10) with 3 circular
    sentinel lists + default callback FUN_004d7ff0 (clump-create shape), refcounted
    set-object-at-+0x18 with 4-dword bounding-sphere copy (atomic-set-geometry shape),
    short-refcount AddRef/Release-destroy pair at +0xe (geometry refcount shape).
  - 00534b60/00534d00 — RenderWare PTank particle-plugin create pair: 00534d00 calls the
    Ghidra-named RpAtomicCreate, allocates a 0x138 descriptor (memhint 0x3012f), installs
    a render-callback override at atomic+0x48 (LAB_00535690), uses plugin offset
    DAT_007dc57c, and packs per-flag vertex streams with the distinctive stride table
    flag1->0xc, 8->0x40, 0x10->0xc, 4->8, 2->4, 0x40->0x10, 0x20->4, 0x80->0x10,
    0x100->0x20 with mutual-exclusion sanitization in 00534b60 (which passes static
    callback table &DAT_00623c78). No game-state reads/writes.
  - 00557ec0/00557fb0/00558030 — LOD-atomic plugin family: lazy per-object extension
    record via global table DAT_00913274, freelist DAT_0091327c memhint 0x30112, exactly
    10 refcounted geometry slots (AddRef FUN_004e8e90 / Release FUN_004e8ea0 /
    set-geometry FUN_004e68a0 — the same RW-core trio skipped above), render-callback
    chain save/override at object+0x48 (LAB_00557b70, restore in 00557fb0), range select
    storing (range, _DAT_005cc55c/range) in 00558030. No game-state reads/writes.
    NB: hooks.csv notes for 00557fb0/00558030 carried a batch_w "physics core + 2D text
    glue" MIXED label — refuted by these bodies (geometry-slot LOD machinery).
- **Plated 10 (hud / font-vector)**: 00554010 (path-node handle resolver), 00554150
  (path-node destructor), 00554200 (binary path sub-list stream loader, header magic 1,
  0x18-byte elements, alloc tag 0x301a1), 00554390 (top-level binary vector-font loader —
  the FGDC20.RWF spec function, single application-page caller FUN_00427880), 00555830
  (font destructor), 00556780 (text-format PostScript-operator font loader:
  begin/end/moveto/lineto/curveto/closepath + full UTF-8 ladder), 00556e40 (style-block
  refcount release), 00557110 (style 2D-position/uv tween init, leaf), 005572b0 (5-byte
  thunk), 005572c0 (stroke renderer emitting 9-float vertices, flush threshold 0xfd, via
  FUN_004cd070/4cd170/4cd140 submit triplet). Old hooks.csv batch-y note "collision-bvh-
  octree + physics-math tail" does NOT describe these 10 — the 2026-05-19 plates in
  re/analysis/bucket_00554010/ already said font-vector; honor subsystem_observed=hud.
- **Recurring [UNCERTAIN] (one per plate, do not multi-mint)**: the font/path module
  (0x00553xxx-0x00557xxx) may itself be a vendored RenderWare-family 2D toolkit
  (PostScript operator strings, rwID-band alloc hints 0x30190/0x301a1, engine-global
  table idioms). No Ghidra Library tag, no FidDB name, no citable documented symbol —
  per the in-doubt rule these are PLATED, not skipped. If a later calibration confirms a
  vendored Rt2d-family band here, reclass the 10 plates out in one transaction.
- **Other plate-local [UNCERTAIN]s**: see each plate's Uncertainties section (element
  record semantics 0x18, FUN_004cc5e0 0x1a1 find-vs-seek, version-byte domain, render-
  state IDs 1/9/2, undumped float constants DAT_005d757c/_DAT_005cc320,
  LAB_00554940 form-0 dispatch has NO function object — pre-existing known
  function_create candidate, not a candidate row this batch).
- **Pool note**: Mashed_pool2 (pre-assigned) threw ghidra.framework.store.LockException
  on open; an earlier malformed open (missing program_name) may have poisoned it in-JVM
  the same way batch_an s4 poisoned pool7 — treat pool2 as suspect for future batches.
  Session ran read-only on Mashed_pool3 (stale-lock-tolerant per header); program_close
  called at end.
- No hooks.csv / STUBS.md / UNCERTAINTIES.md / DEFERRED.md / re/SCRIBE_QUEUE.md writes.
  No master-project mutation. No U-/S- IDs minted (reserved U-8600..U-8899 / S-6800..
  S-6999 are for central re-classify).
