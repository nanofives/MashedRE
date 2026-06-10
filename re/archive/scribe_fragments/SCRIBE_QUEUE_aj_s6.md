# SCRIBE_QUEUE fragment — batch_aj session 6 (aj_s6)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep) writes hooks.csv / trackers and commits.

## Queued

2026-06-02  aj_s6  bucket=re/analysis/bucket_sky_worldobj_dbg_video_hud_00484280_00555910  confidence=C1->C2  rvas=00484280,004842b0,00484310,004844a0,00484580,004847d0,00484a50,00484ac0,00484cf0,00484de0,00484f70,00487e40,00487e70,004880a0,00488320,00488390,004883b0,00489240,004942b0,00494320,00494480,0049dd60,00554940,00555910  needs_function_create=00555910,00554940

## Notes for the sweep

- **Count**: 24 RVAs, 24 plates authored in the bucket dir. **None drift-skipped**
  — all 24 were `C1` in hooks.csv at session start (verified per-row); none already
  ≥C2.
- **Pool slot**: PRE-ASSIGNED `Mashed_pool5` opened cleanly **read-only** (no `.lock`
  present; verified-free). `opened_in_slot: Mashed_pool5` in every plate. No master
  writes; `program_close` called at end. (`.pool_slot_aj_s6` records this.)

- **Subsystem confirmation — all 5 hypotheses HOLD; NO reclassifications**:
  - **debug-overlay (5)**: 0x00484280/2b0/310/4a0/580 — an input-driven debug-page
    overlay system. Queue at `DAT_006cf078` (head `DAT_006cf880`), guard flags
    `DAT_006cfc98/9c`, installed render callback `DAT_006cf070` (via the 0x004e5xxx
    debug-print band), entry array `DAT_006cf480` (count `DAT_006cf478`), input keys
    3/0x14..0x1e cycled by the dispatcher [[00484580]] (single caller FUN_00478cd0).
    Draws via the D3D9/RW device vtable `DAT_007d3ff8+0x20` and the debug-line fn
    `FUN_004b61c0`. CONFIRMED debug-overlay.
  - **world-objects (6)**: 0x004847d0/a50/ac0/cf0/de0/f70 — the world-object registry +
    spatial-collision system. 12×12 grid init ([[004847d0]], `DAT_006cfe68`, anchor
    `DAT_006dccb0`, done-flag `DAT_006e70d4`); cell insertion ([[00484a50]], max 100/cell,
    bidirectional link, heterogeneity tracker +0x1ac/+0x1b0/+0x1b4); multi-cell AABB
    registration ([[00484ac0]]); public registrar ([[00484cf0]], array `DAT_006dccb8`
    stride 0x8C, max 300, 13 callers across ai/vehicle/track); grid-accelerated pair
    collision ([[00484de0]], skip-bitmap +0x68, callbacks at field [7]) and brute-force
    O(N²) fallback ([[00484f70]], callbacks at field [6]). CONFIRMED world-objects.
  - **sky (7)**: 0x00487e40/e70/880a0/8320/8390/83b0/89240 — the sky/cloud element
    system. 128-slot array `DAT_0086ae20` (stride 0xe dwords); remove-by-index
    ([[00487e40]]), 13-case type registrar ([[00487e70]]), RW mesh create
    ([[004880a0]], `DAT_007030ac` = `FUN_00534b60(0x2000,0x10000087,0)`), fsin parallax
    anim tick ([[00488320]], types 4/6/0xc), phase-increment setter ([[00488390]]),
    2745-byte per-frame vertex-buffer build ([[004883b0]], 128 slots × 16 vertices,
    4 RW streams locked via FUN_00535700 / committed via FUN_00535910), and the 10-byte
    secondary-dispatch thunk SkySecondaryDispatch ([[00489240]], `*(DAT_007030ac+0x48)`).
    CONFIRMED sky.
  - **video (4)**: 0x004942b0/494320/494480/49dd60 — COM-style video playback graph.
    Per-frame tick ([[004942b0]], vtable m8/m12 on `DAT_00771a28`), COM-Release teardown
    ([[00494320]], 8-pointer graph `DAT_00771a10..a34`, Stop m9 + Release m2), sub-step
    glue ([[00494480]], selector `DAT_006147d8` into array `DAT_00771a10`), and a threaded
    streaming-object ctor ([[0049dd60]], 3 CRITICAL_SECTIONs + SetEvent, vtables
    0x5d0dc0/0x5d0d84/0x5cf9d0, SEH frame). CONFIRMED video. (The muddy cluster_0049
    batch_t note "input/DInput-adjacent" is a batch-level tag, NOT a per-RVA finding —
    per-RVA decomp is unambiguously video for all four.)
  - **hud (2)**: 0x00554940/555910 — the font/glyph rendering pair. CONFIRMED hud.

- **⚠ SPECIAL — `needs_function_create` for BOTH hud RVAs (not just one)**:
  - **0x00555910** (expected): NO function defined in the clone — re-confirmed from
    listing (`SUB ESP,0x188` prologue, `CALL FUN_005551d0`, store-fn `_DAT_00913100`).
    Matches hooks.csv U-5649 + the prior `font_ctx_promote_ae4` plate. Flagged
    `needs_function_create=00555910`. **Sweep MUST `function_create` at 0x00555910 on
    master before C2.**
  - **0x00554940** (DISCREPANCY — surface to user/sweep): the plan's SPECIAL flag asserts
    "function IS defined; plate normally", BUT in read-only clone `Mashed_pool5`
    (2026-05-17/18 snapshot) `function_at` AND `decomp_function` BOTH return
    `error: no function found at 00554940` — it is only `LAB_00554940` (consistent with
    the font_text C1 plate's U-1067 "unrecognized function, no decompile"). Plated from
    the raw listing (per-string glyph renderer: `SUB ESP,0xC8`; reads font-ctx flags
    +0x10 bit1 = 1/2-byte mode, glyph block +0x134, kerning +0x8; calls the text-width fn
    `FUN_005554d0`; RW Im2D state band 0x00552xxx + device vtable `DAT_007d3ff8`).
    Flagged `needs_function_create=00554940` **defensively**. **Sweep MUST verify on
    master**: if the function is present there (clone merely stale), skip the create and
    decompile+plate normally to C2; if absent (as in this clone), `function_create` is
    required before C2 — same handling as 0x00555910.

- **U-IDs / S-IDs**: per the batch_aj workflow (lines 54–56, 123–124) this author-only
  session **mints NONE** — every plate's `## Uncertainties` / `## Stubs encountered`
  sections say "U-ID/S-ID NEEDED (do NOT mint)" with the specific gap described. The
  sweep mints centrally from pre-assigned ranges. Pre-existing IDs **referenced** (not
  re-filed): U-2007 (0049dd60 RET-0x10), U-T3-009/010/014 (video cluster_0049),
  U-1067/1068/1070 + U-5649 (hud font gap), S-2000/S-2001 (0049dd60).

- **Out-of-bucket callee families referenced but NOT plated** (left for their own passes
  / sweep S-IDs): 0x004e5xxx debug-print band (FUN_004e5840/5820/5660), debug-line/render
  FUN_004b61c0/004b60c0/004b41b0, input/cvar helpers FUN_0055dec0/004292d0/004292c0,
  RW geometry/render band FUN_00534b60/004e7e30/004e8090/004c0b30/00535700/00535910/
  004c3d90/004c39b0, math float→int FUN_004a3090/004a2c48, camera/anchor getters
  FUN_00467210/00443090, video processors FUN_00493fc0/004938e0, threaded-ctor helpers
  FUN_004a0ef0/004a1160, font helpers FUN_005c4d30 + RW Im2D 0x00552xxx + the `.met`
  parser band. The collision/anim/render indirect callbacks (object field [6]/[7],
  `DAT_006cf070`, `*(DAT_007030ac+0x48)`, font-ctx `+0x138`) are data-dependent, not
  static CALL targets.

- **Strong C3 candidates** (clean fully-determined leaves, no external data semantics):
  [[004842b0]] (4-dword reset), [[00488390]] (single-field setter), [[00489240]]
  (10-byte indirect thunk), [[00487e40]] (bounded array clear). Noted for a future c3
  batch; not promoted here.

- Files left UNTRACKED for central finalize: the 24 bucket plates, this fragment, and
  `.pool_slot_aj_s6`. One commit only (no re-classify, no build, no Frida, no master
  writes) — per author-only mission.
