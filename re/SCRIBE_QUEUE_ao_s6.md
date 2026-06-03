# SCRIBE_QUEUE fragment — batch_ao session 6 (ao_s6)

Author-only promote-c2 pass (gameplay campaign 4/~5, sixth/last slice of batch_ao,
top of the RenderWare-Physics page 0x0055e440..0x00562380, directly below the qhull
island at 0x0057c5b0+). Bucket plates are the C2 deliverable; central finalize
(ghidra-sweep + re-classify) writes hooks.csv / trackers, mints U-/S- IDs from the
reserved ranges (U-8600..U-8899 / S-6800..S-6999), and commits.

## Queued

2026-06-03  ao_s6  bucket=re/analysis/bucket_gameplay_0055e440_00562380  confidence=C1->C2  rvas=NONE  subsystem_observed=third-party-library[RenderWare-Physics-3.7] (ALL 26)  library_skip=0055e440:RenderWare-Physics-3.7,0055f450:RenderWare-Physics-3.7,0055f480:RenderWare-Physics-3.7,0055f520:RenderWare-Physics-3.7,0055f670:RenderWare-Physics-3.7,0055f800:RenderWare-Physics-3.7,0055fdd0:RenderWare-Physics-3.7,0055fe10:RenderWare-Physics-3.7,0055fe30:RenderWare-Physics-3.7,0055fe40:RenderWare-Physics-3.7,0055fe50:RenderWare-Physics-3.7,0055fea0:RenderWare-Physics-3.7,0055ff70:RenderWare-Physics-3.7,0055ff90:RenderWare-Physics-3.7,005601f0:RenderWare-Physics-3.7,00560260:RenderWare-Physics-3.7,00561040:RenderWare-Physics-3.7,00561280:RenderWare-Physics-3.7,00561390:RenderWare-Physics-3.7,00561c50:RenderWare-Physics-3.7,00561e60:RenderWare-Physics-3.7,00561e80:RenderWare-Physics-3.7,00561ea0:RenderWare-Physics-3.7,00561ec0:RenderWare-Physics-3.7,00562010:RenderWare-Physics-3.7,00562380:RenderWare-Physics-3.7  needs_function_create=NONE  note=ENTIRE slice is the vendored RW-Physics rigid-body solver (ctor/dtor/setter API + island constraint-solver step + CCD + sleep + RpWorld sector-import bridge). 0 plates authored; all 26 reclass-OUT to third-party-library[RenderWare-Physics-3.7], kept C1. Matches batch_ao header prediction. actual pool slot=Mashed_pool15 (pre-assigned; opened read-only cleanly, NO on-disk lock, program_close'd at end).

## Notes for the central re-classify

- **Count**: 26 candidates, **0 plates** (none qualify as game/engine-glue). All 26
  are their OWN first-field `gameplay,C1` rows in hooks.csv at session start
  (verified with anchored `^<rva>,` grep — no fuzzy-match drift, no rows >= C2).
  All 26 returned non-null `function_at` and decompiled cleanly on Mashed_pool15;
  none are thunks; **no needs_function_create**. The sweep has NOTHING to plate
  from this fragment (sweep_build_manifest plated-count 0 == queued rvas=NONE is
  expected, not an error).

- **Existing C1 plates**: every RVA already has a batch-y-s4 C1 plate at
  re/analysis/bucket_00554010/0x<rva>.md ("collision-bvh-octree + physics-math
  tail"). Today's decomp on pool15 confirms each plate's mechanical content
  verbatim — those plates stay valid as C1 library-residue documentation; the
  hooks.csv `file` column can keep pointing at them.

- **LIBRARY-CONFIRM EVIDENCE (why all 26 are RW-Physics-3.7, not game glue):**
  1. *RW memory system*: FUN_0055f450 allocates the 0x424-byte solver context via
     `(**(code **)(DAT_007d3ff8 + 0x108))(0x424, 0x30900)` (cited at 0x0055f450);
     FUN_0055fdd0 frees via the same table's `+0x10c` (cited at 0x0055fdd0).
     DAT_007d3ff8 is the RwEngineInstance pointer; +0x108/+0x10c are its
     memory-function slots; 0x30900 is an RW allocation-hint tag.
  2. *RW plugin-offset idiom*: FUN_00561ea0 returns `*(DAT_00623fac + obj)` with
     sentinel `-1 -> 0`; FUN_00561ec0 identical on DAT_00623fb0 — the canonical
     RWPLUGINOFFSET accessor pattern. FUN_00562010 consumes
     `**(DAT_00623fac + sector)` on an RpWorldSector.
  3. *RpWorldSector layout*: FUN_00562010 reads sector+0x84 (numTriangles, u16),
     sector+4 (triangles, stride 8 = RpTriangle), sector+8 (vertices, stride 0xc
     = RwV3d) — exact RW 3.x world-sector geometry walk.
  4. *Neighbour attestation*: batch_aj reclassed 0055dc70 AND 00562520 ->
     RW-Physics (00562520 cluster contains FUN_00562500, the recorded caller of
     FUN_00562010); batch_ak reclassed 00559c40/0055ae70/0055b940/0055bab0/
     0055c4a0. This slice is bracketed on BOTH ends by attested RW-Physics rows.
  5. *Callee closure*: every depth-1 callee of the 26 lies in the 0x0055a..0x00573
     RW-Physics band, plus FUN_004c3ac0 (Vec3Magnitude, RW core) and FUN_004e55d0
     (RW sector helper) and FUN_00538c80 (spatial-grid register). No callee
     re-enters application pages.
  6. *Zero game-state access*: across all 26 decompilations the only globals
     touched are DAT_007d3ff8 (RwEngineInstance), DAT_00623fac/DAT_00623fb0 (RW
     plugin offsets), .rdata float constants (0x005cc32x/0x005cc56c/0x005cc9a0/
     0x005cd110/0x005d757c/0x005e5258/0x005ceabc), the vtable blob DAT_005e51c8,
     and DAT_00913284 (a .bss phase flag written ONLY inside FUN_00560260 between
     the two callback invocations — solver-module-internal, not game state).
  7. *Coherent middleware surface*: ctor (0055f450/0055f800) + dtor (0055fdd0) +
     parameter setters (0055fe10/fe30/fe40) + per-frame step pipeline (0055fe50/
     fea0/ff70/ff90/00561040/00560260) + CCD (00561390) + sleep management
     (00561c50) + begin/end wrappers (00561e60/e80) + RpWorld geometry import
     (00561ea0/ec0/00562010/00562380) — a complete, self-contained rigid-body
     solver SDK, not per-title glue.

- **Per-RVA cluster map** (for the re-classify notes column):
  - *Solver-context API (15)*: 0055e440 vtable-init ctor (base-init FUN_0055c380 +
    fields +0x40/+0x44); 0055f450 alloc-0x424 ctor wrapper; 0055f480 7-sub-array
    16-aligned slab layout; 0055f520 max-scratch-size estimator; 0055f670
    total-size estimator; 0055f800 top-level context initializer (installs
    step/finalize fn-ptrs LAB_00569140/LAB_005697f0 or FUN_0056a450/FUN_0056adb0
    by mode at +0x58); 0055fdd0 dtor; 0055fe10 vec3 setter +0x4c; 0055fe30 setter
    +0x08; 0055fe40 setter +0x04; 0055fe50 10-arg forwarder -> FUN_0055a1f0;
    0055fea0 self-constraint collector (stride-0x14 entries, tag 0x8000); 0055ff70
    guarded dispatch -> FUN_0056bb30; 0055ff90 broadphase pair-activation filter
    (bitmap tests, stride-0x28 pair records); 00561040 island-driver outer loop
    (batches <= ctx+0x48 constraints per FUN_00560260 call).
  - *Solver math/step (5)*: 005601f0 sparse 4-dword row compaction; 00560260
    3537-byte islanded constraint-solver step (inertia-tensor projection, two
    callback phases around DAT_00913284=0/1); 00561280 contact->world-space
    projection + path-length accumulator; 00561390 CCD swept-impact + contact
    builder (calls Vec3Magnitude FUN_004c3ac0 x3, builds reflection matrix);
    00561c50 4-threshold velocity-clamp sleep-eligibility (-> FUN_0055ac00).
  - *Begin/end wrappers (2)*: 00561e60 -> FUN_00568fd0; 00561e80 -> FUN_00568dd0
    (both over ctx+0xc0/+0xc4).
  - *RpWorld import bridge (4)*: 00561ea0 plugin-offset getter DAT_00623fac;
    00561ec0 plugin-offset getter DAT_00623fb0; 00562010 per-sector triangle
    indexer (registers each RpTriangle into spatial grid via FUN_00538c80,
    per-triangle callback LAB_005620c0); 00562380 triangle-normal builder +
    bound expansion (primitive type tag = 3 at param_2+0x18).

- **Closest call**: the RpWorld import bridge (00562010/00562380) is the most
  glue-like cluster, but three independent indicators keep it library: the
  plugin-offset read (RW-Physics registers the sector plugin, not game code),
  its caller FUN_00562500 already attested RW-Physics (batch_aj 00562520), and
  its tuning constants living in the same RW-Physics .rdata band
  (_DAT_005cc56c/_DAT_005cd110/_DAT_005cc9a0 shared with the solver core).
  No [UNCERTAIN] strong enough to force a C1->C2 plate.

- **POOL SLOT**: pre-assigned Mashed_pool15 opened read-only first try (no
  on-disk .lock present, no LockException, image_base 0x00400000 so RVAs map
  directly). `program_close` completed at end of session. Marker file
  `.pool_slot_ao_s6` records the slot.

- **Campaign effect**: gameplay C1 drains by 26 via reclass-OUT (kept C1 under
  third-party-library[RenderWare-Physics-3.7]); C2-promotion count from this
  session is 0, as the batch_ao header predicted for the RW-Physics page.
