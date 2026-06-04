# SCRIBE_QUEUE fragment — batch_ap session 4 (ap_s4)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep + central re-classify) writes hooks.csv / trackers.

## Queued

2026-06-03  ap_s4  bucket=re/analysis/bucket_gameplay_0056a250_0056efc0  confidence=C1->C2  rvas=NONE  library_skip=0056a250:RenderWare-Physics-3.7,0056a450:RenderWare-Physics-3.7,0056a7a0:RenderWare-Physics-3.7,0056aae0:RenderWare-Physics-3.7,0056ac40:RenderWare-Physics-3.7,0056adb0:RenderWare-Physics-3.7,0056b7a0:RenderWare-Physics-3.7,0056b9d0:RenderWare-Physics-3.7,0056ba30:RenderWare-Physics-3.7,0056bb30:RenderWare-Physics-3.7,0056bb80:RenderWare-Physics-3.7,0056bce0:RenderWare-Physics-3.7,0056bdf0:RenderWare-Physics-3.7,0056be80:RenderWare-Physics-3.7,0056c0a0:RenderWare-Physics-3.7,0056c310:RenderWare-Physics-3.7,0056c580:RenderWare-Physics-3.7,0056c8e0:RenderWare-Physics-3.7,0056caa0:RenderWare-Physics-3.7,0056cf90:RenderWare-Physics-3.7,0056d070:RenderWare-Physics-3.7,0056d350:RenderWare-Physics-3.7,0056d3f0:RenderWare-Physics-3.7,0056dd40:RenderWare-Physics-3.7,0056e680:RenderWare-Physics-3.7,0056ed60:RenderWare-Physics-3.7,0056ef30:RenderWare-Physics-3.7,0056efc0:RenderWare-Physics-3.7  note=ALL 28 candidates are vendored RW-Physics rigid-body solver internals (subsystem_observed=third-party-library[RenderWare-Physics-3.7] for every RVA); 0 plated; needs_function_create=NONE (all 28 had function objects and decompiled cleanly); actual pool slot=Mashed_pool10 (pre-assigned; opened read-only cleanly at the top-level mashed_pool location — an initial LockException was caused by mistakenly opening the NESTED clone mashed_pool/Mashed_pool10/Mashed_pool10.gpr, NOT a poisoned slot; program_close issued)

## Notes for the sweep

- **Count**: 28 candidates → **0 plated**, **28 library_skip** (kept C1, central
  re-classify reclasses to third-party-library[RenderWare-Physics-3.7]).
  **`rvas=NONE` is deliberate** (manifest builder skips the row — ao_s4/s5/s6
  precedent; the library_skip list is the deliverable). No bucket dir was
  created (nothing to plate — ao_s4 precedent).

- **Slice identity**: this is the rigid-body **constraint-solver core** of the
  RW-Physics module — SAT/GJK axis search, LDLt factorization, PGS solver
  setup/iteration (scalar + SSE 4-wide twins), quaternion integration,
  world-frame inertia-tensor rotation with gyroscopic torque, broadphase pair
  filtering. batch_y s5's per-plate `subsystem_observed: physics/*` labels
  (re/analysis/bucket_00565d50/0x0056*.md) match the live decomp 1:1.

- **Evidence (per-RVA confirm, not address-screen)**:
  - **Whole-slice structural scan (live, ghidra_eval, all 28)**: every callee and
    every caller stays inside the 0x0055xxxx–0x0057xxxx physics band, plus exactly
    two out-of-band callees: FUN_004c3b30 (FastSqrt, C3) and FUN_004c45f0 (RW core
    math page). The ONLY data references outside function bodies are .rdata float
    constants: 0x005cc320/0x005cc32c/0x005cc564/0x005cc568/0x005cc574 (scalar
    consts), 0x005cd03c (epsilon), 0x005ceabc (ptr-const), 0x005d757c (FLT_MIN-band
    seed), 0x005e5a20–0x005e5a7c (SSE lane consts + sign-mask tables). ZERO
    game-state globals read or written by any of the 28.
  - **Live decomp read this session (11)**: 0056a250 (4-axis SAT max-separation
    scan over 4-float-stride arrays), 0056a450 (leaf, pure float, high-summary:
    only global = DAT_005cc320), 0056ac40 (SSE movmskps lane-select axis search,
    sign-mask consts 0x005e5a70..7c, lane-increment 0x005e5a40..4c, rsqrt-band
    seed DAT_005d757c), 0056b7a0 (broadphase pair filter: AABB fetch via
    FUN_0055bd80, pair-exclusion bitmask, scene tolerance at scene+0xc014,
    emits 0x14-byte pair records with 0x8000 flag + 0xffff sentinel),
    0056b9d0 (pair-list reset wrapper: zeroes ctx+0x84, scene+0xc018 read,
    forwards to FUN_0056b7a0 + FUN_00573670), 0056bb30 (per-record flag-bit scan,
    0x28 stride, dispatches FUN_0056ba30), 0056bb80 (quaternion integrate by
    angular velocity: |w| via FastSqrt, fsin/fcos half-angle, Hamilton product;
    epsilon _DAT_005cd03c, 1.0=_DAT_005cc320, 0.5=_DAT_005cc32c), 0056bdf0
    (indexed saxpy over 0x20-stride 6-float velocity records), 0056ed60 (3x3
    similarity transform R*I*R^T, 0x10-stride rows), 0056ef30 (27-field
    solver-context zero-init, offsets +0x04..+0x108), 0056efc0 (4-array
    indexed-slot solver write, ctx offsets +0xc4..+0xec).
  - **batch_y C1 plates re-read (all 28)**: re/analysis/bucket_00565d50/
    0x0056*.md mechanical descriptions confirm the same shapes for the 17 not
    re-decompiled live (a7a0/aae0/adb0 GJK-iteration + LDLt-substitution family;
    ba30/bce0/be80/c0a0/c310 rigid-body-step + broadphase; c580/e680 inertia
    rotation; c8e0/caa0/cf90/d070/d350/d3f0/dd40 jacobian + PGS setup/iteration
    incl. the scalar/SSE twin pair d3f0/dd40; ed60 above). An anchored grep over
    all 28 plates finds ZERO DAT_006xxxxx/DAT_007xxxxx/DAT_0091xxxx game-state
    globals — pure parameter-passed records throughout.
  - **Module anchor**: FUN_0056b7a0 calls FUN_0055abb0 and FUN_0055ae50 — both
    already confirmed library_skip RenderWare-Physics-3.7 by ao_s4's per-RVA
    pass (re/SCRIBE_QUEUE_ao_s4.md), and FUN_0055bd80 sits in the same confirmed
    page. The in-band drivers that call this slice (FUN_00560260, FUN_00561390,
    FUN_0055ff70, FUN_00567c00, FUN_005729a0, FUN_00573670) are all inside the
    same physics band (queued for later slices / already-tagged family).
  - **Scalar/SSE twin**: FUN_0056d3f0 (scalar) and FUN_0056dd40 (SSE, rsqrtps +
    one Newton step via consts _DAT_005e5a20/30, lane masks _DAT_005e5a60..68,
    sentinel XOR masks _DAT_005e5a70..7c) implement the SAME constraint
    diagonal-normalization — the CPU-dispatch twin idiom of a shipped
    middleware library, not game code.

- **No FidDB names / no Ghidra Library tags** on any of the 28 (all FUN_) — the
  skip rests on the "obvious thin engine primitive (solver/.../matrix-vector math
  on parameter-passed records) with NO game-state read/write" arm of the
  library-confirm rule, applied per-RVA as instructed (aggressively), with the
  ao_s4 module anchor as cross-evidence.

- **[UNCERTAIN]** (for central re-classify to note, no U-mint here): whether this
  solver core is RenderWare-Physics-3.7 proper or a statically-linked sub-library
  it vendors (e.g. the MathEngine/Karma-derived solver RW Physics shipped) — the
  3.7 tag follows the batch_ao convention for this band; no version string is
  visible in this slice.

- **POOL SLOT**: pre-assigned **Mashed_pool10**, opened read-only via
  `project_program_open_existing(project_location=...\mashed_pool,
  project_name=Mashed_pool10, program_name=MASHED.exe)`, session
  16b1552415e24e0594edfa12770df801, image_base 0x00400000. NOTE for future
  sessions: `mashed_pool\Mashed_pool10\` (a NESTED directory) also contains a
  `Mashed_pool10.gpr` — opening THAT one throws LockException (live lock file
  dated 2026-06-03 20:47). The real slot is the TOP-LEVEL
  `mashed_pool\Mashed_pool10.gpr`, which opened cleanly on first try (ao_s4
  precedent held). `program_close` issued at end. Recorded in `.pool_slot_ap_s4`.

- **Drift check**: all 28 confirmed `gameplay,C1,mapped` in hooks.csv via
  anchored `^<rva>,` grep at session start (no 0x prefix in CSV col 1). 0
  drift-skips, 0 already->=C2.

## Drained

drained-by=sweep-20260603-2132; 0 plates, 0 bookmarks, 0 renames (ALL-SKIP: 28 library_skip, rvas=NONE)
