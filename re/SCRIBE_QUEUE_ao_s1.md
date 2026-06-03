# SCRIBE_QUEUE fragment — batch_ao session 1 (ao_s1)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep) writes hooks.csv / trackers and commits.

## Queued

2026-06-03  ao_s1  bucket=re/analysis/bucket_gameplay_0047f450_004e4440  confidence=C1->C2  rvas=0047f450,0047f480,0047f6d0,0047fad0,004893a0,004893b0,00489450,00489480,004894a0,004894f0,00489500,00489890,004898d0,00489910

## Notes for the sweep

- **Count**: 26 candidates examined, **14 plated** (rvas= above), **12
  library_skip** (kept C1, no plates — see below). None drift-skipped (all 26
  were C1 in hooks.csv at session start). needs_function_create=none (all 26
  had function objects in pool1).
- **Pool slot**: pre-assigned `Mashed_pool1`, opened read-only first try
  (session 5a5e83a0), `program_close`d at end. `opened_in_slot: Mashed_pool1`
  in every plate frontmatter; recorded in `.pool_slot_ao_s1`.
- **subsystem_observed**: all 14 plated RVAs CONFIRMED `gameplay` (no
  reclassifications among plated rows). Two families:
  - 0x0047f450/0x0047f480/0x0047f6d0/0x0047fad0 — world teardown + scenery-actor
    spawn + contact-sample ring buffer (game globals DAT_006ce274, DAT_006c6eb0
    pool arrays, debug mode DAT_00773920; literal "DESTROY CALLED" string anchor
    in 0x0047f480). Deepens the batch-y-s2 plates in bucket_00466100.
  - 0x004893a0/0x004893b0/0x00489450/0x00489480/0x004894a0/0x004894f0/
    0x00489500/0x00489890/0x004898d0/0x00489910 — the keyframe-track family:
    0x30-byte record setters + marker + cubic evaluator + arm/tick/disarm/reset.
    Deepens the batch-aa-s1 plates in bucket_00489450. New cross-links
    established this pass: FUN_004893b0 writes the +0x1c segment-break marker
    FUN_00489500 scans for; FUN_00489480's +0x18 field feeds the bit-0x10
    channel (FUN_00476a30); FUN_004894a0's +0x20..0x2f span feeds the bit-0x20
    channel (FUN_00476a40).
- **library_skip (12, all kept C1, reclass-OUT to third-party-library)**:
  library_skip=004c0790:renderware,004c0870:renderware,004c0910:renderware,
  004c0a60:renderware,004c0d70:renderware,004c0de0:renderware,
  004c1210:renderware,004c15c0:renderware,004c4220:renderware,
  004d8bd0:renderware,004e43b0:renderware,004e4440:renderware
  Evidence (per-RVA decomp in pool1, 2026-06-03):
  - 004c0790/004c0870/004c0910/004c0a60/004c0d70/004c0de0/004c1210/004c15c0 all
    operate on a struct whose layout is byte-exact RW 3.x RwFrame: +3
    privateFlags byte, +4 parent, +8/+0xc dirty-list link pair, 16-dword matrix
    at +0x10 with flags word +0x1c (identity flag OR 0x20003 in 004c15c0),
    list sentinel +0x90/+0x94, first-child +0x98, next-sibling +0x9c, root
    +0xa0 — driven through the engine global DAT_007d3ff8 (dirty-list head
    +0xbc, alloc fn-ptr +0x118 w/ hint 0x3000e, free fn-ptr +0x11c, type-pool
    offset DAT_007d3e70, frame registry list &DAT_00617f78). Matches the
    vendored gta-reversed RenderWare RwFrame/RwLinkList headers
    (mashedmod/deps). Shapes: dirty-unlink (0790), clone+mark-dirty (0870),
    recursive hierarchy clone (0910), root propagation (0a60), recursive
    destroy (0d70), hierarchy destroy (0de0), remove-child+re-root (1210),
    set-identity (15c0).
  - 004c4220 — pure 3x3 determinant over the padded RwMatrix layout (reads
    [0..2],[4..6],[8..10]; skips pad/flag columns [3],[7],[11]); sole caller
    FUN_005c47e0 sits in the RW/RW-Physics library band. NOTE: prior plate
    exists at re/analysis/bucket_00489450/0x004c4220.md (batch-aa-s1) calling
    it util-math; this session reclasses it OUT as RW-matrix library code.
  - 004d8bd0 — node destructor using the same DAT_007d3ff8 fn-table (+0x10c
    single-arg free), metrics counter at *(DAT_00911ad8+4+DAT_007d3ff8), and
    the 0x004d8xxx page's registry helpers (FUN_004d8000/8060/8090 are the
    frame-clone registry callees); freelist free via FUN_004e2ff0.
  - 004e43b0/004e4440 — module-globals-offset pattern: param used as byte
    offset into table base DAT_007d7170 (read in 4440, cleared in 43b0), plus
    +0x11c free dispatch with first arg from DAT_007d7164+DAT_007d3ff8 — RW
    engine-instance plugin-globals accessor shape.
  Per the library-confirm rule these were NOT renamed; existing plates in
  powerups_d3/powerups_d4/bucket_00489450 remain as historical C1 notes.
- **Uncertainties**: bare [UNCERTAIN] markers in plates only — NONE minted to
  U-IDs (author-only). All are data-semantic (field/channel/global meaning,
  unread .rdata float values, one unresolved indirect dispatch at
  0x004898fc) — NON-BLOCKING for C2.
- **Stubs**: noted per-plate, NONE minted to S-IDs. Recurring unmapped callees:
  emit family FUN_004769a0/9d0/9f0/00476a30/00476a40/00476d00/00476df0/
  004768c0/004770a0/00476cb0; world/builder band FUN_0055dec0/0055afc0/
  0055dd60/0055c810/0055c4f0/0055c4a0/0055bab0/0055b940/0057c500/0057c300/
  0057c220/0057c550/00562560/00561ea0/00559c40; app band FUN_00482820/
  FUN_00426060; 0x004exxx handle helpers FUN_004e69a0/004e7e30/004e4380;
  FUN_004c0b30.
- **Size corrections vs prior C1 plates**: 004893b0 is 31 bytes (prior plate
  said 16); 00489450 is 35 bytes (prior said 0x22=34); body ranges from pool1
  function objects cited in each plate.
- Files committed atomically (bucket dir + this fragment); no re-classify, no
  build, no Frida — author-only.
