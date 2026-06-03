# SCRIBE_QUEUE fragment — batch_ap session 3 (ap_s3)

Author-only promote-c2 pass (gameplay campaign batch 5/5 — FINAL, third slice
0x00566ca0-0x00569e90, RenderWare-Physics-3.7 region). Outcome: **ALL-SKIP —
zero C1->C2 plates authored.** Every one of the 28 candidates decodes as a
vendored RenderWare-Physics-3.7 primitive (library-confirm rule, applied
aggressively per the batch_ap header). No bucket dir was created (nothing to
plate — ao_s4 precedent). Central re-classify reclasses all 28 OUT to
third-party-library[RenderWare-Physics-3.7], kept C1.

## Queued

2026-06-03  ap_s3  bucket=re/analysis/bucket_gameplay_00566ca0_00569e90  confidence=C1->C2  rvas=NONE  library_skip=00566ca0:RenderWare-Physics-3.7,00566d50:RenderWare-Physics-3.7,00566ea0:RenderWare-Physics-3.7,005670a0:RenderWare-Physics-3.7,005672c0:RenderWare-Physics-3.7,00567350:RenderWare-Physics-3.7,005675d0:RenderWare-Physics-3.7,00567630:RenderWare-Physics-3.7,005676f0:RenderWare-Physics-3.7,00567c00:RenderWare-Physics-3.7,00567c40:RenderWare-Physics-3.7,00567c60:RenderWare-Physics-3.7,00567f00:RenderWare-Physics-3.7,005684c0:RenderWare-Physics-3.7,00568560:RenderWare-Physics-3.7,005685f0:RenderWare-Physics-3.7,005687a0:RenderWare-Physics-3.7,005687d0:RenderWare-Physics-3.7,00568860:RenderWare-Physics-3.7,00568990:RenderWare-Physics-3.7,00568c40:RenderWare-Physics-3.7,00568c80:RenderWare-Physics-3.7,00568cf0:RenderWare-Physics-3.7,00568d20:RenderWare-Physics-3.7,00568dd0:RenderWare-Physics-3.7,00568fd0:RenderWare-Physics-3.7,00569690:RenderWare-Physics-3.7,00569e90:RenderWare-Physics-3.7  note=ALL 28 candidates are vendored RW-Physics collision/island/solver core (subsystem_observed=third-party-library[RenderWare-Physics-3.7] for every RVA); 0 plated; needs_function_create=NONE (all 28 had function objects and decompiled cleanly); actual pool slot=Mashed_pool9 (pre-assigned; opened read-only cleanly, no LockException; program_close issued)

## Notes for the sweep

- **Count**: 28 RVAs read end-to-end against live decomp (read-only,
  Mashed_pool9, session 4e03384a0445406e81fbfc40d37c876a, image_base
  0x00400000). **0 plates authored — `rvas=NONE` is deliberate** (the manifest
  builder's RVAS_RE will not parse it, so the row is skipped at manifest-build
  time; that is correct — there is nothing for the sweep to plate from this
  session). The deliverable of this session is the `library_skip` list for
  central re-classify.

- **POOL SLOT**: pre-assigned **Mashed_pool9** opened read-only on the first
  attempt via `project_program_open_existing` (project_location =
  `mashed_pool` dir root — the pool .gpr files live flat in `mashed_pool\`,
  NOT in per-slot subdirs; first attempt with a `Mashed_pool9\` subdir path
  failed NotFoundException, retried correctly). `program_close` issued at end.
  Marker `.pool_slot_ap_s3`.

- **needs_function_create = NONE.** All 28 returned a function object and
  decompiled cleanly.

- **Drift check**: all 28 are their OWN first-field `gameplay,C1` rows in
  hooks.csv at session start (verified with anchored `^<rva>,` grep; none
  already >=C2; none drift-skipped). All 28 point at existing batch_y-s5 C1
  plates in `re/analysis/bucket_00565d50/`, which were read first and deepened
  — the library verdict below is the deepening.

- **LIBRARY-CONFIRM VERDICT — all 28 are one coherent RenderWare-Physics-3.7
  collision/island/solver core family.** Four independent lines of evidence:

  1. **Zero game-state access.** No candidate reads or writes any Mashed game
     global (no 0x006xxxxx-0x009xxxxx DAT anywhere in the 28 decomps). The
     only data refs in the whole slice are the library's own .rdata constant
     pool: DAT_005d757c (zero, cited at 0x00566cd2 et al.), _DAT_005cc320
     (1.0, 0x00567027), _DAT_005cc33c (-1.0, 0x0056705b), _DAT_005cc35c and
     _DAT_005cca00 (torus-quartic coefficients, 0x00567120/0x005671c2) — the
     exact pool ao_s4 identified as the RW-Physics constant block.
  2. **Zero application-page callers.** function_callers on all 28: every
     caller lands in the vendored band — FUN_0055f520 / FUN_0055f670
     (sizer-callers), FUN_0055f800 (layout-caller), FUN_0055fea0,
     FUN_0055ff90, FUN_00560260, FUN_00561e60, FUN_00561e80 (step/world
     functions 0x0055f-0x00561), FUN_0056ba30 (same band, s4's slice),
     FUN_0057bf30 (just below the qhull island), FUN_005c5780 (adjacent to
     the library .rdata pool), plus in-slice callers 00567c60->00567f00->
     005684c0/00568560/005685f0 and 00569690->00569e90. 005670a0 and 00569690
     have 0 direct callers (00569690 is reached via the solver's dispatch —
     [UNCERTAIN] which indirect site; non-blocking for a library verdict
     given body+callee evidence).
  3. **Callee set is exclusively vendored-band.** FUN_005665a0 (polynomial
     root solver, s2's slice), FUN_0056ef30 (zero-init, s4's slice),
     FUN_00574a20/FUN_00574ac0 (s5's slice), FUN_0055abb0 (scene shape query
     — CONFIRMED RW-Physics by ao_s4's library_skip row), FUN_0055bd80,
     FUN_00575c60/FUN_00576640/FUN_00575880/FUN_00575560 (s6's slice,
     narrow-phase feature/clip family), FUN_00564100 (memset-shaped, s1's
     slice), FUN_00569e90 (in-slice). Zero calls into application pages.
  4. **Shared struct family + constants with the confirmed RW-Physics rows.**
     body+0x20 ushort slot index, body+0x24 owner-scene ptr, scene+0x60
     active-slot bitmap tested as `1 << (idx & 0x1f)` over `(idx >> 5) * 4`
     word table (00567c60 at 0x00567e3c, 00567f00 at 0x00567f3a/0x00567f76,
     00568dd0 at 0x00568f76), 0xffff/-1 short sentinels for static-world
     bodies (00567f00, 00568990 at 0x005689fc, 00568dd0, 00568fd0) — the
     identical record family as ao_s4's 26 all-skip RVAs and batch_ak's
     00559c40 PhysicsSceneBodyRegister. 00568990 additionally reads the
     0x14-byte broad-phase pair records produced by confirmed-library
     0055a9a0 (pair generator) and dispatches via FUN_0055abb0.

- **Per-RVA mechanical verdicts** (deepening the batch_y-s5 C1 plates in
  `re/analysis/bucket_00565d50/`; all read from live decomp this session):

  | RVA | size | mechanical role (RW-Physics) |
  |---|---|---|
  | 00566ca0 | 167b | ray-vs-plane TOI test (dot(p2,p4)-p5; writes (t,slope) pair; 0xffffffff degenerate / 0 separated / 1 hit) |
  | 00566d50 | 334b | ray-vs-sphere TOI (quadratic: b^2-a*len2 discriminant, t = b-SQRT(disc)) |
  | 00566ea0 | 500b | ray-vs-infinite-cylinder TOI (Schur complement vs axis; param_7 selects entry/exit scale 1.0/-1.0 via _DAT_005cc320/_DAT_005cc33c) |
  | 005670a0 | 532b | ray-vs-torus TOI via degree-4 quartic; coefficients use _DAT_005cc35c/_DAT_005cca00; roots via FUN_005665a0(&out,0,0x3f800000,4,coeffs); 0 callers (pure leaf) |
  | 005672c0 | 133b | buffer-size calculator: (n&0x3fffffff)*0x110 + ((n<<2)/3)*0x40 + aligned per-count arrays from +0x10/+0x14/+0x28 |
  | 00567350 | 640b | packed-allocator slot layout: bumps arena cursor at p1+8, carves 13 budget-checked sub-regions into param_3[1..0x27]/param_2[0,3] |
  | 005675d0 | 91b | solver-counter init: zeros 9 fields in p2 (+0x20..+0x68), writes (p3+round)>>2 derived capacities at p2+0x7c..+0x98 and p1+4/+0x10 |
  | 00567630 | 178b | buffer-size calculator companion: strides 0x80/0x50/0x40/0x100 over counts +0x10/+0x14/+0x18/+0x20/+0x28 |
  | 005676f0 | 1289b | large packed-allocator layout: ~30 budget-checked sub-regions into param_2[0..0x1a]/param_3[0..0x48]; strides 0xa0/0x40/0x30/0x20 |
  | 00567c00 | 49b | paired-context reset: zeros p1+0x11c/+0x110, calls FUN_0056ef30(p1), zeros 5 fields in p2 |
  | 00567c40 | 18b | probe: FUN_00574a20() then FUN_00574ac0(); returns (ret!=0)+1 |
  | 00567c60 | 670b | island finalization: zero bookmark array, FUN_00567f00 builds chains, flattens linked-list islands (0x28-stride) into bulk body/contact/joint arrays; loose-body singleton pass gated on scene+0x60 bitmap |
  | 00567f00 | 1460b | island construction: incremental union-find with path compression over contact pairs (+0x50/+0x58 bodies, -1 short sentinels) and joint records (0xe0 stride at +0xb4); dispatches FUN_005684c0/FUN_00568560/FUN_005685f0 |
  | 005684c0 | 149b | create island (0x28-stride record at p1+8): 1-or-2-body init + optional contact/joint list heads |
  | 00568560 | 143b | append body/contact/joint to existing island (count/head/tail triplets at +1/+4..+6/+7..+9) |
  | 005685f0 | 431b | union-by-rank island merge: rank rule on *p, linked-list concat, count merge, stale-slot compaction via last-island memmove (10 dwords) + back-pointer fix |
  | 005687a0 | 36b | buffer-size calculator: (p+0x10)*0x28 aligned + 0x30 + (p+8)*0x10 |
  | 005687d0 | 129b | buffer-size calculator: strides 0x60/0x4c (+0x38), 0xb0/0x14 (+0x30), 0x140 (+0x34) + 0x120 |
  | 00568860 | 292b | second-stage solver-buffer layout: 9 arena carves keyed off counts at p4+0x30/+0x34/+0x38; strides 0x14/0x20/0x8/0x40/0x120/0x10/0x4c/0x60; sets init flag p2[0x1f]=1 |
  | 00568990 | 675b | narrow-phase contact dispatcher: per broad-phase pair (0x14-stride, -1 static sentinel) resolve transforms via FUN_0055abb0/FUN_0055bd80, generate features 2x FUN_00575c60, intersect FUN_00576640, emit rows FUN_00575880, group-dispatch FUN_00575560 (0xe0-stride out); reads scene record at **(p1+0x70) incl. +0xc014/+0xc018 farfield-arena fields |
  | 00568c40 | 64b | buffer-size calculator: 4 regions, 0x4c-stride contact batch (+0x8), 0x10-stride (+0x14) |
  | 00568c80 | 109b | 4-slot allocator layout matching FUN_00568c40's regions (0x4c/4/0x10/4 strides) |
  | 00568cf0 | 47b | hash-table size: next-pow2(n)*4 aligned + n*0x28 aligned + 0x10 header |
  | 00568d20 | 175b | hash-table allocator: 4-uint header + 0x28-record pool + pow2 bucket table; zeroing via FUN_00564100; matches FUN_00568cf0's size |
  | 00568dd0 | 505b | contact spatial-hash rebuild: per body-chain contact (+0xdc chain, 0x37-dword recs) hash (bodyA_idx*0x20+slotA)*0x3ffd+bodyB_idx*0x20+slotB masked by buckets-1; inserts 10-float warmstart records; normal-projection with active-bitmap gate |
  | 00568fd0 | 354b | persistent-contact lookup: same 0x3ffd hash, symmetric pair match, best dot(normal,prev_normal) (seed 0xff7fffff = -FLT_MAX) selects warmstart impulse into pfVar3[0x34] |
  | 00569690 | 342b | PGS impulse row: clamp(target-dc, lo, hi) from 3 stack floats, write impulse matrix cell at param_12, clamp-side flags 0/0x3f800000, copy 34-dword arg block, call FUN_00569e90, accumulate delta |
  | 00569e90 | 951b | block J^T*impulse apply: two body indices from table at in_stack_74 (+8 stride, -1 sentinel), 6-component scaled rows from 0x60-stride jacobian blocks, accumulates into 0x10-stride delta-velocity buffers for both bodies' constraint rows |

- **Mission question answered (game-side collision GLUE watch)**: none found.
  Every candidate is interior library machinery — no entity-pool access, no
  game globals, no application-page caller anywhere in the slice. The genuine
  glue boundary sits BELOW this band (the 0x0055f-0x00561 world/step callers,
  themselves vendored) and the game-facing API surface already catalogued in
  batch_ak/ao.

- **C2-grade corrections vs the batch_y C1 plates** (recorded here since no
  plates are authored; informational for any future deepening):
  1. 00568fd0: C1 plate omitted the best-candidate seed value — the running
     max is initialized to 0xff7fffff (-FLT_MAX as raw hex; signed decimal
     -3.4028235e38) at 0x00569012, and pfVar3[0x34] is zeroed per contact
     BEFORE the bucket walk (so a no-match contact warmstarts at 0).
  2. 00568990: C1 plate's "resolves two body transforms" — live decomp also
     shows the scene-record reads at **(p1+0x70) +0xc014/+0xc018 cached into
     p1+0x3ec/+0x3f0 before the pair loop (0x005689a4..0x005689c9); 0xc11c
     arena overhead from ao_s3's 00559560 row places these in the farfield
     arena tail.
  3. 00567350: C1 plate listed slot indices [28],[25],[34],[37] in decimal;
     live decomp confirms param_3[0x1c]/[0x19]/[0x22]/[0x25] (same values) —
     no drift, just unit confirmation.

- **Uncertainties**: one bare [UNCERTAIN] noted inline above (00569690's
  indirect call site — 0 static callers; reached via solver dispatch). NOT
  minted (author-only; reserved range U-8900..U-9199 untouched, central
  re-classify mints if it wants a row — per the data-semantic-nonblocking
  rule this does not block the library verdict).

- **For central re-classify**: reclass-OUT all 28 to
  `third-party-library[RenderWare-Physics-3.7]`, kept C1 (library residue,
  not reimplementation targets — same treatment as ao_s4's 26 all-skip rows).
  Do NOT rename on master (none were renamed; read-only session). No U-/S-
  IDs minted. gameplay C1 still drains by 28 via the reclass-OUT.
