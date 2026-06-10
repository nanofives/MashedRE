# SCRIBE_QUEUE fragment — batch_an session 6 (an_s6)

Author-only promote-c2 pass (gameplay campaign 3/~5, sixth/last slice of batch_an,
closes the next-156 region at its top end 0x0047ba20..0x0047f380). Bucket plates are
the C2 deliverable; central finalize (ghidra-sweep + re-classify) writes hooks.csv /
trackers, mints U-/S- IDs from the reserved ranges, and commits.

## Queued

2026-06-03  an_s6  bucket=re/analysis/bucket_gameplay_0047ba20_0047f380  confidence=C1->C2  rvas=0047ba20,0047bb10,0047bc90,0047c1f0,0047c230,0047c270,0047c2d0,0047ccb0,0047cd50,0047cdc0,0047cde0,0047ce00,0047ce20,0047ce70,0047ce80,0047cf20,0047d130,0047d150,0047d180,0047d330,0047d640,0047def0,0047f1e0,0047f230,0047f290,0047f380  note=subsystem_observed MIXED — ~19 stay gameplay (convex-hull collision narrow-phase + scenery-actor pool mgmt/destructors), 4 recommend reclass to camera (camera-path node containment), 2 to hud (on-screen event-marker overlay builders), 1 to render (squared-distance LOD setter). Per-RVA below. needs_function_create=NONE (all 26 had function objects and decompiled). library_skip=NONE (no candidate decodes as a library primitive; library-band screen was already ZERO at generation). actual pool slot=Mashed_pool10 (pre-assigned; opened read-only despite a STALE on-disk .lock dated Tue May 26 — see POOL SLOT note).

## Notes for the sweep

- **Count**: 26 RVAs, 26 plates authored in the bucket dir. **None drift-skipped** —
  all 26 are their OWN first-field `gameplay,C1` rows in hooks.csv at session start
  (verified with anchored `^<rva>,` grep). NB a naive `grep <rva> hooks.csv` fuzzy-
  matches SIX of them to unrelated C2 rows that merely *cite* them in the notes column
  (the am_s5 gotcha): `0047bb10`↔00c160 camera, `0047bc90`↔0047bcc0 track,
  `0047cdc0`↔00412050 render, `0047d150`↔00407600 Player::GetPositionPtr,
  `0047d180`↔004073b0 util, `0047f1e0`↔0046cb30 Player::GetOffset3D. Those are different
  functions, not drift — the real rows are all `gameplay,C1` (file column
  re/analysis/bucket_00466100/0x<rva>.md, batch-y-s2).

- Every plate uses the `## Mechanical description` heading; none use `## Purpose`
  (verified). Bare `[UNCERTAIN]` markers only — **no U-IDs / S-IDs minted** (per the
  batch_an author-only protocol; central re-classify mints from U-8300..U-8599 /
  S-6600..S-6799). All 26 returned non-null `function_at`; none are thunks.

- **POOL SLOT (stale-lock handling)**: pre-assigned **Mashed_pool10** carried a STALE
  on-disk `.lock`/`.lock~` dated **Tue May 26 01:53** (hostname MARIANO-PC / maria) —
  predates today's work. Per memory feedback_mcp_leaked_project_lock I did NOT delete
  it; instead I test-opened read-only via `project_program_open_existing` — it SUCCEEDED
  (image_base 0x00400000, RVAs map directly), so the lock was honored/stale-tolerant, not
  a poisoned in-JVM leak. No fallback to pool9/11/12/13 was needed. `program_close`d
  cleanly at end. Recorded in `.pool_slot_an_s6`. (For the central sweep: pool10 is fine;
  the stale .lock can be cleared with `ghidra_pool.ps1 cleanup` if it bothers a later GUI
  session.)

- **SUBSYSTEM MAP — three subsystems in one slice.** The "gameplay" hypothesis is mostly
  right; the slice is a **convex-hull collision narrow-phase** + a large **scenery-actor
  pool manager** + a small **camera-path containment island** + a **HUD event-overlay**
  pair. Recommend the central re-classify split as follows.

  **KEEP gameplay (19):**
  - *Convex-hull collision (3):* `0047ba20` point-vs-face-edge straddle test (← 0047bb10;
    calls Vec3Normalize FUN_004c39b0); `0047bb10` per-hull face-contact accumulator
    (builds 0x10-byte contacts via FUN_00477e60 + centroid via FUN_00477f00; ← 0047c160
    [camera]); `0047bc90` __fastcall edge shared-endpoint equality test (← 0047bcc0
    [track]).
  - *Scenery-actor pool accessors/managers (16):* `0047ccb0` resource+handle-table
    destructor (DAT_006c3010/DAT_0086ecc4); `0047cd50` reset-scale+flag callback (type
    gate {0x10,0x11,0x19}; DAT_006c9438|=1); `0047cde0`/`0047ce00` setter/getter for the
    200-entry flag array DAT_006c9438; `0047ce20` getter for the "kind" array DAT_006c6b90
    (note: lower bound `idx>0`, NOT ≥0); `0047ce70` live-count getter (DAT_006c6eb0);
    `0047ce80` getter for the 200-entry DAT_006c9758; `0047cf20` scenery-actor
    visibility toggle (clump+0x60 bitset; FUN_0055ade0/ac50/ad30); `0047d130` handle
    getter (DAT_006c71d8); `0047d150` render-object getter; `0047d180` reposition/
    re-transform; `0047d330` 4-special-actor teardown (← 0047f380); `0047f290`
    single-slot destructor (maintains DAT_006c6eb0); `0047f380` full 200-slot pool
    destructor (← 0047f450); `0047f1e0`/`0047f230` special-actor current-vertex vec3
    getters (+0 and +0x10 of the 0x20-byte vertex; DAT_006c9a78 indexed 0..15).

  **→ camera (4):** `0047c1f0`, `0047c230`, `0047c270`, `0047c2d0` — an isolated
  4-function island testing the **camera-path node arrays** (`DAT_006c2fe8` count,
  `0x006c2fa8` per-node sub-counts, `0x006c27a8` per-node records stride 0x80) that are
  managed by `FUN_0047c160` (already classified **camera**, C2). c270/c2d0 are the outer
  any-active-node tests (no direct callers → top-level/indirect); c1f0/c230 are their
  inner all-sub-items predicates (==0 / ==2). Register setup (EAX=record base,
  EBX=sub-count) confirmed from the raw listing.

  **→ hud (2):** `0047d640` (type-6 records) and `0047def0` (type-8/9/10 records) —
  per-frame **on-screen event-marker overlay builders** that walk an event/link DB and
  push RGBA+geometry draw records into the **256-entry display ring** at `DAT_007e9de0`
  (payload arrays DAT_007e9de4/ea1e4/ea5e4/ea9e4/ec9e4; wraps at 0x100). Both gated by
  FUN_0040e350 mode ∉ {1..5} and driven by the single caller `FUN_0047e9c0`. (render-
  overlay is the alternative bucket if hud is reserved for 2D-panel UI only.)

  **→ render (1):** `0047cdc0` — `DAT_006c6870[idx] = dist²` squared-distance setter;
  3 callers incl. `FUN_00412050` (render C2, "fills stride-0xb table … FUN_0047cdc0×2
  per iter"). Weakly held (1-line generic helper) — keep-gameplay is acceptable if the
  sweep prefers not to move a util-shaped leaf.

  Net: gameplay 19 + camera 4 + hud 2 + render 1 = 26.

- **Shared resolver / cross-bucket callees** (noted in plates, depth-1, not minted):
  `FUN_0057c210` (handle→entity resolver) is the spine of the whole scenery-actor cluster
  (ccb0/cd50/cf20/d130/d150/d180/d330/f1e0/f230/f290/f380). The destroy chain
  FUN_00559ee0 / FUN_0057c370 / FUN_004e4440 / FUN_004e43b0 / FUN_004e7e30 / FUN_004c0c20
  / FUN_004e6920 recurs across d330/f290/f380. `FUN_004c39b0` (Vec3Normalize, math leaf)
  is touched by 0047ba20 and 0047def0.

- **Uncertainties carried** (all data-semantic / decompiler-artifact, NON-BLOCKING for C2
  of these control-flow-complete reads; bare `[UNCERTAIN]` in plates, central mints):
  - Hull struct layout beyond cited offsets (+0 normals / +0x90 verts / +0xf0 face table /
    +0x138 center / +0x148 face count) — 0047ba20 / 0047bb10.
  - 0x80-byte camera-path node-record layout (c270/c2d0) and the FUN_00478030/FUN_004780c0
    sub-item predicates (c1f0/c230).
  - DAT_006c9438 / DAT_006c9758 / DAT_006c6b90 element semantics (the scenery-actor
    per-slot state/kind/index arrays); the `idx>0` vs `idx>=0` bound asymmetry on ce20.
  - The whole event-DB node layout (+0xac/+0xb4/+0xb8/+0xbc/+0xc0/+0xdc + 0x28-stride
    sub-entries), the FUN_0040e350 mode gate {1..5}, DAT_007f1030 mode tag, and the
    negative-magic sentinels (-0xfefeff/-0xffff01/-0xff80) + RGBA color→class mapping —
    0047d640 / 0047def0.
  - Per-actor tables DAT_0086cbc0 (0x110) / DAT_0086cba4 (0x44) field meaning (def0);
    DAT_006c9a78 array width (≥16 per f1e0/def0 vs 4-slot teardown in d330).
  - Numeric values of the `&DAT_00881f84`-family floats in def0 (cited by address).
  - **Resolved constants** (read from memory, NOT uncertain): DAT_005d757c = **0.0f**
    (sign-test threshold), _DAT_005cc320 = **1.0f** (averaging/clamp); DAT_007f1008 /
    DAT_007f100c = 0.0 static (runtime-written globals).

- **needs_function_create = NONE.** **library_skip = NONE.** No new U-IDs / S-IDs /
  arg_types minted. No Frida, no build, no re-classify, no hooks.csv write. Per the
  author-only mission only the bucket dir, this fragment, and `.pool_slot_an_s6` were
  created/modified.

> DRAINED by sweep-20260603-1259 — 26 plates + 26 bookmarks to master Ghidra, 0 renames, 0 errors. C1->C2 finalize via re-classify follows.
