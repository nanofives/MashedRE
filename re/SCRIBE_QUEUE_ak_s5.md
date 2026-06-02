# SCRIBE_QUEUE fragment — batch_ak session 5 (ak_s5)

Author-only promote-c2 pass (ai mid cluster). Bucket plates are the C2
deliverable; central finalize (ghidra-sweep + re-classify) writes hooks.csv /
trackers, mints any new IDs, and commits.

## Queued

2026-06-02  ak_s5  bucket=re/analysis/bucket_ai_00415d00_00452ea0  confidence=C1->C2  rvas=00415d00,00415e20,00416060,004161e0,00416250,00416a30,00422ba0,004252c0,00426c90,00429840,00429860,00442cc0,00443080,00443300,00443440,00443d10,00443dc0,00452160,00452ea0  note=ai mid cluster, all 19 CONFIRMED ai (no reclassifications, zero library-band hits); no needs_function_create (function_at resolved all 19); 4 drift-corrections + carried uncertainties detailed below

## Notes for the sweep

- **Count**: 19 RVAs, 19 plates authored in the bucket dir. None drift-skipped —
  all 19 were `ai, C1` in hooks.csv at session start (verified anchored on the
  address column), none already C2+.
- **Subsystem confirmation**: all 19 CONFIRMED `ai`. No reclassifications. Zero
  RenderWare / CRT / library-band hits — consistent with the batch header screen
  ("ai (57): ZERO hits. Clean application code."). The cluster is the AI
  control-step core (FUN_00416250 / FUN_00416a30) plus its spline-geometry
  helpers (FUN_00443300 Catmull-Rom, FUN_00443440 progress+curvature, FUN_00443dc0
  lookahead finder, FUN_00443d10 tile lookup), the collision event-queue
  reader/writer pair (FUN_00422ba0 / FUN_004252c0), and several getters/setters.
- **POOL-SLOT FALLBACK (record the ACTUAL slot used = Mashed_pool7)**: the
  pre-assigned **Mashed_pool5** could not be opened — `project_program_open_existing`
  returned `LockException: Unable to lock project`, and the on-disk
  `Mashed_pool5.lock~` was "Device or resource busy" (held by a live channel lock
  in the shared MCP JVM) even though `program_list_open` reported 0 open programs.
  This is the leaked-in-JVM-lock poisoning described in memory
  `feedback_mcp_leaked_project_lock`; on-disk deletion does not clear it and the
  slot is poisoned for the shared server's lifetime. Per the batch header's
  POOL-SLOT HYGIENE clause I fell back to a **verified-free, unassigned** clone
  **Mashed_pool7** (NOT `acquire`, NOT pool4) and `program_close`d at end. The
  stale `Mashed_pool5.lock` was removed; `Mashed_pool5.lock~` is still held and
  was left in place. `.pool_slot_ak_s5` records pool7.
- **No new U-IDs / S-IDs minted** (reserved range U-7500..U-7699 / S-6200..S-6399
  left to the central re-classify). New holes are marked bare `[UNCERTAIN]` in
  each plate's `## Uncertainties` section. Pre-existing IDs referenced (not
  re-minted): U-3587 (00429840/860 DAT_008991bc), U-3588 (00426c90 arg-passing),
  U-1428 (00415e20 ST0 return), U-1432 + merged U-0398 (00443080 DAT_00897ffc),
  U-1887/U-1888 (00443dc0 slot-pin / FUN_00416230 commit), U-3590/U-3591
  (00422ba0 type-9/type-4), U-3592/U-3593 (004252c0 event codes / FUN_00415860),
  U-0413..U-0417 (00416250/00416a30 param_3 mapping, float meanings, mode names,
  split rationale), U-1427 (00416060 FUN_004a2c48 arg coupling).
- **DRIFT CORRECTIONS for the sweep to reconcile** (fresh pool7 decomp vs the
  prior C1 plates):
  1. **00416060** — prior plate frontmatter listed `FUN_00443d10` as a depth-1
     callee; the fresh decomp **inlines** the two-level tile lookup. Real depth-1
     callees are `FUN_004c3bf0` and `FUN_004a2c48` (×2 per loop). Corrected in the
     new plate's frontmatter + body.
  2. **00422ba0** — prior plate listed `FUN_00422b50` in the type-4 path; the
     fresh decomp does NOT call it. Type-4 callees are `FUN_004922e0`,
     `FUN_004c3ac0`, `FUN_0040e470`, `FUN_00417370`. Corrected.
  3. **00452ea0** — prior plate described `DAT_0088ff50` as a 1-byte-stride array;
     the fresh decomp returns `undefined4` for `(&DAT_0088ff50)[param_1]` (4-byte
     element / stride 4). Reported the decomp typing; flagged for element-width
     reconciliation against the listing.
  4. **00443080** — prior frontmatter `size_bytes: 6`; the decomp body is 5 bytes
     (0x00443080..0x00443084). Minor; noted in the plate.
- **FUN_00416250 vs FUN_00416a30 relationship** (re-confirmed in fresh decomp):
  00416a30 is the mode-4/9 variant of 00416250, byte-equivalent except it omits
  the mode-5 path (no FUN_00415020), the mode-10 path (no FUN_00426c00 /
  FUN_00414f00), the FUN_004148b0 mode-6 early-return, and the behavior-mode commit
  to DAT_0089a52c. Useful as a same-shape cross-check when these go C3.
- Files left UNTRACKED until the session commit: the bucket dir (19 plates), this
  fragment, and `.pool_slot_ak_s5`. No hooks.csv / STUBS / UNCERTAINTIES /
  DEFERRED writes, no re-classify, no master-Ghidra mutation, no build, no Frida —
  per the author-only mission.
