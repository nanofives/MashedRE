# SCRIBE_QUEUE fragment — batch_an session 5 (an_s5)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep + re-classify) writes hooks.csv / trackers and commits.

## Queued

2026-06-03  an_s5  bucket=re/analysis/bucket_gameplay_00471430_0047b6b0  confidence=C1->C2  rvas=00471430,00471450,00471490,00471530,00471560,00471780,004721b0,004722e0,00472500,00472520,00472550,00472560,004725f0,004726b0,004726f0,00472740,004728f0,004729b0,00472ad0,00472b10,00473220,004733b0,004744a0,0047b480,0047b5b0,0047b6b0  note=see-Notes-below

## Notes for the sweep

- **Count**: 26 RVAs, 26 plates authored in the bucket dir
  (`re/analysis/bucket_gameplay_00471430_0047b6b0/`, files `0x<rva>.md`). **None
  drift-skipped** — all 26 were their own first-field `gameplay,C1` rows in
  hooks.csv at session start (verified; each currently maps to a batch_y C1 plate
  under `re/analysis/bucket_00466100/`). The sweep should retarget the hooks.csv
  `file` column to the new bucket.

- **Pool slot ACTUAL = Mashed_pool11** (NOT the pre-assigned Mashed_pool8). The
  read-only open of pool8 hit a `LockException` (`Mashed_pool8.lock` @02:17:12,
  same minute as my open) — the known leaked-in-JVM-lock poisoning
  (memory `feedback_mcp_leaked_project_lock`). Fell back to verified-free pool11
  (no `.lock`), opened read-only, all 26 decompiled, `program_close`d cleanly.
  Recorded in `.pool_slot_an_s5`. NB pool8 may stay poisoned for the shared
  server's lifetime — do NOT re-assign it without re-verification.

- **Subsystem reclassifications — 6 RVAs gameplay -> render** (record
  `subsystem_observed=render`; the YAML frontmatter of each plate also carries it):
  - **00472b10** — RwIm2D 4-vertex tri-strip quad (DrawQuad; non-blended). Caller
    FUN_0042c010 (frontend render). Pure render-state + RenderPrimitive, no
    game-state.
  - **00473220** — resolution-scaled alpha-blended quad with side-color slot
    a4c/a84. Callers FUN_00428d30 (frontend) + FUN_00473ee0 (animated logo).
  - **004733b0** — near-duplicate of 00473220, side-color slot a30/a68. Caller
    FUN_00473ee0 (animated logo). (Memory `project_standalone_exe_phase_h` notes
    FUN_004733b0/FUN_00473220 are the logo's quad primitives.)
  - **004744a0** — 2×3×7 sine-warped alpha-blended quad grid (procedural ribbon,
    time global _DAT_007f1010). Caller FUN_00428d30 (frontend); inner-quad sibling
    of logo renderer FUN_00473ee0.
  - **004729b0** — parametric-curve gradient 3D-line draw via FUN_004cd070/430/140
    (RW 3D-line API). Caller FUN_00472ad0.
  - **00472ad0** — fixed-color (black->white) wrapper over 004729b0.
  The remaining **20 stay gameplay**: per-record accumulation table (00471430,
  00471450, 00471490 + scenery-name table 00471530/00471560), event-impulse ticker
  00471780, callback/handler installers 004721b0/004722e0, 0x691500 player-color
  record cluster (00472500/520/550/560/5f0), math leaves
  (004726b0 Vec2DotClamp[-1,1], 004726f0 Vec3DotClamp[-1,1], 004728f0
  ParamCurveArcLength, 00472740 4-byte RNG fill), and the script-bytecode op
  handlers 0047b480/5b0/6b0.

- **No library_skip, no needs_function_create** — all 26 had real function objects;
  the library-band screen is clean (none decode as CRT/RW/qhull/D3DX9 primitives).

- **C3 leads** (for a later promote-c3 batch, not this pass): 004726b0 and 004726f0
  are deterministic scalar-float dot-clamp leaves (same A/B shape as the existing
  C3 Vec3Magnitude 0x004c3ac0); 004728f0 builds on that C3 hook.

- **Stale-name flag (00471780)**: a 2026-05-13 master comment labels param_1
  "camera-anim entry base pointer", but the body is mechanically an animator-driven
  rigid-body impulse applier (force pipe FUN_0055b650). Recorded as [UNCERTAIN] in
  the plate; central re-classify should reconcile the name (do NOT carry the
  camera-anim label forward).

- **Corrected C1 finding (0047b5b0)**: the batch_y C1 plate described a 3-byte read
  storing "byte2"; the decompilation shows only **two** operand reads and the stored
  value is the second byte. Corrected in the new plate (0047b480 reads 3 / appends;
  0047b5b0 reads 2 / appends; 0047b6b0 reads 3 / direct-writes +0 and +2).

- **U-/S-IDs**: NOT minted (author-only). New holes are bare `[UNCERTAIN]` in the
  plates; central re-classify mints from the reserved batch_an range
  (U-8300..U-8599, S-6600..S-6799). Grounded float constants this session (read
  from pool11 memory): _DAT_005cc320=1.0f, _DAT_005cc33c=-1.0f, _DAT_005cc558=0.001f,
  _DAT_005cd5a8=1/640, _DAT_005cc560=1/480, _DAT_005cd0ec=0.005f.
