# SCRIBE_QUEUE fragment — batch_ap session 5 (ap_s5)

Author-only promote-c2 pass (gameplay campaign FINAL batch 5/5, fifth slice
0x0056f020-0x00575b60, RenderWare-Physics-3.7 band approaching the qhull
island). Outcome: **ALL-SKIP — zero C1->C2 plates authored.** Every one of the
28 candidates decodes as a vendored RenderWare-Physics-3.7 primitive
(library-confirm rule, applied aggressively per the batch_ap header). No
qh_*-shaped convex-hull code in this slice (no facet/ridge/vertex-set
structures) — it is rigid-body dynamics, not qhull-2002.1. No bucket dir
content (nothing to plate). Central re-classify reclasses all 28 OUT to
third-party-library[RenderWare-Physics-3.7], kept C1.

## Queued

2026-06-03  ap_s5  bucket=re/analysis/bucket_gameplay_0056f020_00575b60  confidence=C1->C2  rvas=NONE  library_skip=0056f020:RenderWare-Physics-3.7,0056f0a0:RenderWare-Physics-3.7,0056f1f0:RenderWare-Physics-3.7,0056f350:RenderWare-Physics-3.7,0056fad0:RenderWare-Physics-3.7,0056fb90:RenderWare-Physics-3.7,0056fea0:RenderWare-Physics-3.7,00570090:RenderWare-Physics-3.7,005729a0:RenderWare-Physics-3.7,005735f0:RenderWare-Physics-3.7,00573670:RenderWare-Physics-3.7,00573890:RenderWare-Physics-3.7,005739d0:RenderWare-Physics-3.7,00574230:RenderWare-Physics-3.7,005742c0:RenderWare-Physics-3.7,005748a0:RenderWare-Physics-3.7,00574920:RenderWare-Physics-3.7,005749b0:RenderWare-Physics-3.7,00574a20:RenderWare-Physics-3.7,00574ac0:RenderWare-Physics-3.7,00574ad0:RenderWare-Physics-3.7,00575120:RenderWare-Physics-3.7,005751f0:RenderWare-Physics-3.7,005752b0:RenderWare-Physics-3.7,00575560:RenderWare-Physics-3.7,005757d0:RenderWare-Physics-3.7,00575880:RenderWare-Physics-3.7,00575b60:RenderWare-Physics-3.7  note=ALL 28 candidates are vendored RW-Physics solver/CCD/narrow-phase/mass-property core (subsystem_observed=third-party-library[RenderWare-Physics-3.7] for every RVA); 0 plated; needs_function_create=NONE (all 28 had function objects and decompiled cleanly); actual pool slot=Mashed_pool12 (pre-assigned; opened read-only cleanly, no LockException; program_close issued)

## Notes for the sweep

- **Count**: 28 RVAs verified against live decomp + a per-RVA reference scan
  (read-only, Mashed_pool12, session cebbb5645f024b51b5ef8abfa1b957fc,
  image_base 0x00400000). **0 plates authored — `rvas=NONE` is deliberate**
  (manifest builder skips the row; nothing for the sweep to plate). The
  deliverable of this session is the `library_skip` list for central
  re-classify.

- **POOL SLOT**: pre-assigned **Mashed_pool12** opened read-only on the FIRST
  attempt (program_open, read_only=true; no LockException). `program_close`
  issued at end. Recorded in `.pool_slot_ap_s5`.

- **needs_function_create = NONE.** All 28 returned a function object.

- **Drift check**: all 28 are their OWN first-field `gameplay,C1` rows in
  hooks.csv at session start (anchored `^<rva>,` grep). All point at existing
  C1 plates — batch-y-s5 (`re/analysis/bucket_00565d50/`, 10 RVAs
  0056f020..005735f0) and batch-z-s2 (`re/analysis/bucket_00565cd0/`, 18 RVAs
  00573670..00575b60) — which were read FIRST and deepened; the library
  verdict below is the deepening.

- **LIBRARY-CONFIRM VERDICT — all 28 are the RenderWare-Physics-3.7
  constraint-solver / CCD / narrow-phase / mass-property CORE.** Evidence
  (per-RVA reference scan over every instruction, one ghidra_eval pass):

  1. **Zero game-state data references.** Across all 28 bodies, every data
     reference in 0x005c0000..0x009fffff lands in the module's .rdata float
     constants (0x005cc31c..0x005e58b0 — e.g. 0x005cc320=1.0-family,
     0x005cc33c=-1.0, 0x005e5738=default-constraint template) with exactly
     three exceptions, all library-internal:
     `005742c0` → `DAT_007d3ff8` (the RW allocator fn-table, +0x108 temp-buffer
     alloc — the explicit library indicator from the batch header);
     `00574a20`/`00574ac0` → `DAT_007dc8d4` (the module's PRIVATE SSE-available
     bool: written only by the 00574a20 initializer, read only by the 00574ac0
     getter, both called only from in-band FUN_00567c40). No entity pools, no
     0x006xxxxx/0x007xxxxx game globals.
  2. **Every caller is in-band** (0x0055xxxx..0x0057xxxx): FUN_00560260 and
     FUN_00561390 are ALREADY reclassed third-party-library[RenderWare-Physics-3.7]
     (batch_ao); the remaining callers (00563b00, 00567c40, 00568990, 0056b9d0,
     00576880-family, 00578e50, 00578ff0, 0057adb0) are pending batch_ap
     sibling-session candidates in the same band. ZERO application-page
     (0x0040xxxx-0x004xxxxx game code) callers. 00574230 has no static caller
     (vtable-dispatched; reads only 0x005d757c).
  3. **Callee set is exclusively vendored-band or generic math.** All callees
     are 0x0055xxxx RW-Physics rows already reclassed-OUT (0055abb0, 0055bae0,
     0055bbf0, 0055bd80, 0055c230, 0055c2d0, 00561280...) or in-band siblings;
     the two out-of-band callees are FUN_004a3a90 (= _strncmp, calibrated CRT
     band) and FUN_00546b10 (RwMatrix→Quaternion Shepperd-method converter,
     C2 — generic engine math, not game state).
  4. **Shape: a single coherent rigid-body-dynamics module**, matching the
     batch-y-s5/batch-z-s2 C1 plates read first:
     - Solver row machinery: 0056f020 (commit row, 5 cursors), 0056f0a0
       (SSE 4-row alignment pad via 0x005e5738 template), 0056f1f0 (append
       jacobian row, 0x40-stride blocks), 0056fad0 (J_A/J_B normal jacobian),
       0056f350 (contact-manifold → normal+2-friction rows), 00570090
       (10.5KB joint-constraint setup: linear/angular axes, limits, motors,
       springs), 0056fb90 (body-local→world quaternion transform),
       0056fea0 (quaternion-G-matrix builder).
     - CCD/TOI: 005729a0 (5-substep conservative-advancement TOI), 005735f0
       (velocity-at-point leaf), 00573670 (edge-batch CCD driver; ctx+0x70
       manager chain with +0xc014/+0xc018 offsets = the cross-bucket
       physics-context object; static bodies materialized via 0055bd80).
     - Mass properties: 005739d0 (Mirtich-style polyhedral integrator),
       00573890 (centroid+inertia from moments), 005742c0 (multi-part
       compound inertia via DAT_007d3ff8 temp buffers), 005748a0 (9-float
       AXPY leaf), 00574230 (vtable-dispatched cost accumulator).
     - SSE dispatch init: 00574920 (CPUID leaf-0 vendor string), 005749b0
       (SEH-guarded OS SSE probe), 00574a20 (GenuineIntel/AuthenticAMD
       strncmp + CPUID leaf-1 EDX bit-25 → DAT_007dc8d4), 00574ac0
       (1-instruction getter).
     - Narrow-phase/contact: 00574ad0 (dual-plane clip → contact emit),
       00575120 (manifold append w/ dedup, 0x28-stride records), 005751f0
       (triangle-area edge-score leaf), 005752b0 (per-pair narrow-phase via
       SAT 00576880), 00575560 (pair-loop driver, 0x120-stride), 005757d0
       (per-shape dispatch via 0055c230), 00575880 (broad→narrow glue w/
       pool budgets), 00575b60 (body-B feature normal via 0055c2d0).
  5. **qhull watch (negative)**: none of the 28 decode qh_*-shaped — no
     facet/ridge/vertex sets, no qh global struct. The island boundary
     (0x0057c5b0) is above this slice; tag is RenderWare-Physics-3.7 for all.

## SUBSYSTEM RECLASSIFICATIONS (subsystem_observed vs hooks.csv)

All 28: hooks.csv says `gameplay`; observed = **third-party-library
[RenderWare-Physics-3.7]** (kept C1 per the library-confirm rule). No other
subsystem reclassifications.

## Uncertainties (bare, for central re-classify to mint if wanted)

- [UNCERTAIN] FUN_00546b10 (callee of 0056fb90) is classified `vehicle,C2` in
  hooks.csv but is a generic RwMatrix→Quaternion converter also consumed by
  this vendored module; whether its subsystem label should move is a tracker
  question, not resolved here (evidence: call site in FUN_0056fb90 body;
  hooks.csv row 00546b10).
- [UNCERTAIN] 00574230 has zero static call references (reached via vtable
  slot per batch-z-s2 plate); the owning vtable address was not chased in
  this session.

## Drained

drained-by=sweep-20260603-2132; 0 plates, 0 bookmarks, 0 renames (ALL-SKIP: 28 library_skip, rvas=NONE)
