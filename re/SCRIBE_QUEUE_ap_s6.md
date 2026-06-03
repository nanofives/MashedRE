# SCRIBE_QUEUE fragment — batch_ap session 6 (ap_s6)

Author-only promote-c2 pass (gameplay campaign 5/5 — FINAL, sixth/last slice
0x00575c60..0x0057ae30, the narrow-phase tail of the RW-Physics module, ending
0x178 bytes below the qhull island boundary at 0x0057c5b0). Bucket plates are the
C2 deliverable; central finalize (ghidra-sweep + re-classify) writes hooks.csv /
trackers, mints U-/S- IDs from the reserved ranges (U-8900..U-9199 /
S-7000..S-7199), and commits.

## Queued

2026-06-03  ap_s6  bucket=re/analysis/bucket_gameplay_00575c60_0057ae30  confidence=C1->C2  rvas=NONE  subsystem_observed=third-party-library[RenderWare-Physics-3.7] (ALL 28)  library_skip=00575c60:RenderWare-Physics-3.7,00575fe0:RenderWare-Physics-3.7,00576640:RenderWare-Physics-3.7,00576880:RenderWare-Physics-3.7,00577be0:RenderWare-Physics-3.7,00577cb0:RenderWare-Physics-3.7,00577ec0:RenderWare-Physics-3.7,005784a0:RenderWare-Physics-3.7,00578610:RenderWare-Physics-3.7,00578b20:RenderWare-Physics-3.7,00578bd0:RenderWare-Physics-3.7,00578cb0:RenderWare-Physics-3.7,00578d90:RenderWare-Physics-3.7,00578e50:RenderWare-Physics-3.7,00578ec0:RenderWare-Physics-3.7,00578f20:RenderWare-Physics-3.7,00578ff0:RenderWare-Physics-3.7,00579b50:RenderWare-Physics-3.7,00579c00:RenderWare-Physics-3.7,00579d50:RenderWare-Physics-3.7,00579e50:RenderWare-Physics-3.7,00579ee0:RenderWare-Physics-3.7,0057a250:RenderWare-Physics-3.7,0057a660:RenderWare-Physics-3.7,0057a9a0:RenderWare-Physics-3.7,0057adb0:RenderWare-Physics-3.7,0057ae20:RenderWare-Physics-3.7,0057ae30:RenderWare-Physics-3.7  needs_function_create=NONE  note=ENTIRE slice is the vendored RW-Physics narrow-phase collision pipeline (broadphase-pair emit + polygon contact clipping + SAT axis solver + manifold reduction + GJK distance + conservative-advancement TOI). 0 plates authored; all 28 reclass-OUT to third-party-library[RenderWare-Physics-3.7], kept C1. qhull watch NEGATIVE: none decode qh_*-shaped. Matches batch_ap header prediction; gameplay campaign s6 slice CLOSED. actual pool slot=Mashed_pool15 (pre-assigned; opened read-only cleanly, NO on-disk lock, program_close'd at end).

## Notes for the central re-classify

- **Count**: 28 candidates, **0 plates** (none qualify as game/engine-glue). All
  28 are their OWN first-field `gameplay,C1` rows in hooks.csv at session start
  (verified with anchored `^<rva>,` grep — no fuzzy-match drift, no rows >= C2;
  all carry the batch-z-s2 "physics-engine narrow-phase pipeline" note). All 28
  returned non-null `function_at` and decompiled cleanly on Mashed_pool15; none
  are thunks; **no needs_function_create**. The sweep has NOTHING to plate from
  this fragment (sweep_build_manifest plated-count 0 == queued rvas=NONE is
  expected, not an error — ao_s4/s5/s6 precedent).

- **Existing C1 plates**: every RVA already has a batch-z-s2 C1 plate at
  re/analysis/bucket_00565cd0/0x<rva>.md ("physics-engine narrow-phase pipeline:
  GJK distance + EPA penetration + SAT axis solver + polygon contact clipping +
  manifold reduction + conservative-advancement TOI"). Today's decomp on pool15
  confirms each plate's mechanical content — those plates stay valid as C1
  library-residue documentation; the hooks.csv `file` column can keep pointing
  at them.

- **qhull watch (the s6-specific mission item): NEGATIVE.** None of the 28 decode
  qh_*-shaped — no facet/ridge/vertex set machinery, no qh global struct, no
  convex-hull construction. The slice is GJK/SAT/contact-manifold runtime
  machinery (the CONSUMER side of hull data), tagged RenderWare-Physics-3.7, not
  qhull-2002.1. The qhull island proper still starts at 0x0057c5b0 as calibrated.

- **LIBRARY-CONFIRM EVIDENCE (why all 28 are RW-Physics-3.7, not game glue):**
  1. *RW memory system*: FUN_00578ec0 (refcounted convex-piece-set destroy) frees
     via `(**(code **)(DAT_007d3ff8 + 0x10c))(self)` (cited at 0x00578f0b) —
     DAT_007d3ff8 is the RwEngineInstance pointer, +0x10c its free slot — and
     calls FUN_00564190, the RW-Physics arena-destroy already attested by ao_s3
     (an ap_s1 candidate this batch), plus per-piece FUN_0055bad0 over a
     0x60-stride piece array.
  2. *Callee closure*: every depth-1 out-of-bucket callee lies in the RW band:
     FUN_004c4600 (RW-core 3x4 transform compose), FUN_0055bad0, FUN_0055bd70
     (shape AABB), FUN_0055c000 (support-point virtual), FUN_0055c2d0 (support
     interval), FUN_00564190 (arena destroy), FUN_00566200 (AABB transform),
     FUN_00566830 (perpendicular pick), FUN_005667c0 (vec3 normalize),
     FUN_005757d0 (manifold store, s5 slice). No callee enters application pages.
  3. *Caller closure*: the two pipeline entry points FUN_00578e50 (SAT entry) and
     FUN_0057adb0 (GJK/TOI entry) each have exactly ONE caller — 0x00575912 and
     0x005758fb respectively, both inside the in-band 0x00575120..0x00575b60
     dispatch family (ap_s5 slice). The whole slice is internal to the module.
  4. *Zero game-state access*: across all 28 decompilations the only globals
     touched are DAT_007d3ff8 (RwEngineInstance fn-table, in FUN_00578ec0),
     PTR_DAT_00623fe8 (static Johnson-subalgorithm subset-enumeration table
     indexed by simplex bitmask, read-only, in FUN_0057a250), DAT_005e5db0 (a
     static vtable blob stamped into synthesized manifold-shape records at
     +0x5c, in FUN_00575fe0), and .rdata float constants (0x005cc320/0x005cc328/
     0x005cc32c/0x005cc33c/0x005cc558/0x005cc56c/0x005cc9a0/0x005cc9b4/
     0x005cc9dc/0x005ccabc/0x005ccac8/0x005cd03c/0x005ce1d4/0x005ce54c/
     0x005cea1c/0x005ceabc/0x005ceac0/0x005e4564/0x005e4568/0x005e5050/
     0x005d757c). No 0x006xxxxx/0x007xxxxx mutable game globals, no entity
     pools, no application-page reads or writes.
  5. *Library idioms*: 0xffff pair-ID sentinels + bitmask filter matrices in
     FUN_00576640 (the exact RW-Physics idiom flagged in the batch header);
     shape-record vtable dispatch at +0x10 (world AABB), +0x1c (face table),
     +0x44/+0x48 (spatial-map queries) throughout; GJK 0x32-iteration cap with
     duplicate-support cycling detection in FUN_0057a250 — textbook
     Gilbert-Johnson-Keerthi with Johnson distance subalgorithm.
  6. *Coherent middleware surface*: a complete, self-contained convex narrow-phase
     SDK (below) with no per-title customization anywhere in it.

- **Per-RVA cluster map** (for the re-classify notes column):
  - *Broadphase-pair emit (3)*: 00575c60 pair-overlap walker — copies/builds
    64-byte transform slots + 32-byte AABB slots, emits 0x14-byte pair records,
    overflow flag world+0x78; 00575fe0 recursive aggregate descent — leaf emit
    vs recurse on shape flag bit 1, spatial-map fast path via vtable +0x44/+0x48,
    synthesizes triangle manifold-shapes (vtable DAT_005e5db0) from query hits;
    00576640 pair-list filter — skip-self, 0xffff sentinels, two bitmask filter
    matrices (group + per-shape 0x5a IDs), AABB tolerance test.
  - *Contact clipping (5)*: 00576880 polygon-polygon closest-feature/contact
    generator (vertex/edge/face case ladder + edge-pair march + plane projection
    emit); 00577be0 point clamp onto segment (t in [min,max]); 00577cb0 polygon
    edge-support selector vs probe direction; 00577ec0 segment-segment closest
    params (parallel + clamped branches); 005784a0 contact-point dedup-append
    (rejects points within _DAT_005ce54c of first/last, projects onto plane).
  - *SAT (7)*: 00578610 separating-axis dispatcher — center-delta fast path,
    single-face-record paths, full face-A/face-B/edgexedge sweep via per-shape
    face tables fetched through vtable +0x1c, subtracts both shapes' margins
    (+0x4c); 00578b20 axis interval-overlap depth (two FUN_0055c2d0 projections,
    flips axis); 00578bd0/00578cb0 face-record interval test (A-face / B-face
    variants); 00578d90 fallback axis from position delta orthogonalized against
    reference normal (degenerate -> FUN_00566830 perpendicular); 00578e50 SAT
    pipeline entry — runs 00578610, depth-gates, stores via FUN_005757d0;
    00578ec0 convex-piece-set refcounted destroy (see evidence #1).
  - *Support/aggregate (1)*: 00578f20 support point over a 0x60-stride convex
    piece array (per-piece FUN_0055c000 + margin +0x4c, max-dot select,
    threshold _DAT_005e5050).
  - *Manifold reduction (3)*: 00578ff0 manifold merge/reduce — 16-bucket hash
    chains on pair IDs, dominance merge via 00579b50, weighted-centroid +
    4-extremal-point reduction (>=5 points) building a reduced 0x38-dword
    manifold; 00579b50 merge predicate (plane-distance vs depth-scaled
    tolerance); 00579c00 barycentric combination over a feature bitmask
    (degeneracy ratio gate _DAT_005cea1c).
  - *GJK (6)*: 00579d50 support-pair evaluation (two FUN_0055c000 calls, CSO
    vertex + optional translation offset); 00579e50 per-vertex dot-table
    incremental update; 00579ee0 Johnson determinant/cofactor table update (sets
    the 4 tetrahedron cofactors when simplex mask == 0xf); 0057a250 GJK
    distance core — Johnson subset enumeration via PTR_DAT_00623fe8, 0x32 iter
    cap, duplicate-support cycle detection, separation/touching/penetration
    3-way return; 0057a660 initial axis selection (special-case shape type 4
    direction via 0057ae20, else AABB-center delta; probes +/- cardinal axes);
    0057a9a0 conservative-advancement distance driver — plane/slab fast paths
    for shape type 1, else 4-round bisection translating B along the axis by
    local_200, refining via depth/slope (TOI-style).
  - *Misc (3)*: 0057adb0 GJK/TOI pipeline entry (runs 0057a9a0, depth-gates,
    stores via FUN_005757d0 — mirror of 00578e50); 0057ae20 trivial accessor
    `return p + 0x30`; 0057ae30 triangle centroid + bounding-sphere radius
    (max vertex distance via branchless `0x21312300 >> (cmp-bits << 2) & 3`
    select, adds margin +0x4c).
  - The two 0x38-dword manifold-record layouts in 00578ff0 and the 0x14-byte
    pair records in 00575c60/00575fe0/00576640 are the same structures the s5
    slice (0x00575120..0x00575b60) dispatches over — one link unit.

- **Closest call**: 00575c60/00575fe0/00576640 (broadphase-pair emit) read a
  "world" workspace record with array bases/cursors/capacities at +0x0c..+0x24 —
  the most context-coupled cluster. Three indicators keep it library: the
  workspace is parameter-passed (never a global), the synthesized-shape vtable
  DAT_005e5db0 and the spatial-map vtable slots are RW-Physics statics, and the
  only callers are the in-band s5 dispatch family. No [UNCERTAIN] strong enough
  to force a C1->C2 plate.

- **POOL SLOT**: pre-assigned Mashed_pool15 opened read-only first try (no
  on-disk .lock present, no LockException, image_base 0x00400000 so RVAs map
  directly). `program_close` completed at end of session. Marker file
  `.pool_slot_ap_s6` records the slot.

- **Campaign effect**: gameplay C1 drains by 28 via reclass-OUT (kept C1 under
  third-party-library[RenderWare-Physics-3.7]); C2-promotion count from this
  session is 0, as the batch_ap header predicted for this band. With all six
  ap fragments drained, gameplay C1 -> 0 and the gameplay campaign CLOSES.
