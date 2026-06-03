# SCRIBE_QUEUE fragment — batch_an session 4 (an_s4)

Author-only promote-c2 pass (gameplay campaign batch 3/~5, slice 0x0045dff0 + the sparse
walk across 0x00461-0x0046d). Bucket plates are the C2 deliverable; central finalize
(ghidra-sweep + re-classify) writes hooks.csv / trackers and commits.

## Queued

2026-06-03  an_s4  bucket=re/analysis/bucket_gameplay_0045dff0_0046dd90  confidence=C1->C2  rvas=0045dff0,00461e90,00462760,004656e0,00465f40,00466000,00466100,004670a0,004671d0,004672d0,0046bce0,0046bd20,0046bd60,0046bda0,0046be10,0046be50,0046bf50,0046bfc0,0046d2e0,0046d320,0046d360,0046d660,0046d740,0046d880,0046dd80,0046dd90

## Notes for the sweep

- **Count**: 26 RVAs, 26 plates authored in `bucket_gameplay_0045dff0_0046dd90/`. None
  drift-skipped. All read end-to-end against live decomp (read-only). Plate set == queued rva
  set (verified, exact 26/26 via diff).

- **POOL SLOT — IMPORTANT, NOT the pre-assigned one.** Pre-assigned Mashed_pool7 was POISONED:
  the first `project_program_open_existing` call failed (server required `program_name`), which
  leaked an in-JVM lock on pool7 (matches memory `feedback_mcp_leaked_project_lock`). The
  on-disk `Mashed_pool7.lock` was (re)stamped at the exact attempt time (2026-06-03 02:17 ART)
  and the retry returned `LockException: Unable to lock project`. Per the documented remedy I
  fell back to a **verified-free clone (not `acquire`)**: opened **Mashed_pool14** read-only
  (full 80M idata, no .lock), session `f071768c55dc4d66bc3b4b155efb73c3`, image_base
  0x00400000. `program_close` issued at end. Recorded in `.pool_slot_an_s4`. Mashed_pool14 is
  outside the batch_an pool set {1,2,3,7,8,10} so it does not collide with sibling sessions.

- **needs_function_create = NONE.** All 26 returned a non-null `function_at` and decompiled
  cleanly.

- **library_skip = NONE.** Zero vendored-library hits (no CRT/D3DX9/qhull/RW-primitive named
  families). All 26 are genuine game/engine glue.

- **Subsystem findings — the "gameplay" hypothesis reclassifies for 8 of 26.** I did NOT edit
  hooks.csv (author-only). subsystem_observed recorded per plate; recommend central re-classify
  reconcile as below. Evidence is in-body callee bands (0x005a6xxx/0x005a8xxx = RWS audio;
  0x004c0xxx-0x004c7xxx = RenderWare core).

  - **audio (5)** — per-entity 3D positional-sound slot management:
    - **00462760** — walks the `&DAT_0069045c` channel-handle table via FUN_005a89a0/89b0
      (audio-channel state queries); returns 1 if any of 4 slots idle.
    - **004656e0** — starts an audio channel: FUN_005a86a0 (issue) + FUN_005a8960 (set state 2)
      on `(&DAT_0069045c)[player]`; sets per-player flags `&DAT_0088f680[+0xc]`, `&DAT_008aa2e0`.
    - **00465f40** — 3D emitter position+volume setter: writes record `&DAT_0068f640` +0x58/5c/60
      position, pushes to live channel via FUN_005a6d60, volume via FUN_0045e0f0.
    - **00466000** — extended position+volume+second-param setter (FUN_005a6d60 + FUN_005a6dc0
      opcode 4 + FUN_0045e0f0).
    - **00466100** — volume-only setter for the same `&DAT_0068f640` record family (FUN_0045e0f0).

  - **render (3)** — alt-camera / render-object triple (caller FUN_00467110 already render in
    hooks.csv as "alt-camera-triple init DAT_006905b0/b4/b8"):
    - **004670a0** — constructs the global render object `DAT_006905b0` (FUN_004c1d30 create +
      FUN_004c0740 frame attach + 2× FUN_004c77c0 sub-resources at +0x60/+0x64).
    - **004671d0** — getter returning `DAT_006905b0->frame(+4) + 0x40` (or per-player via
      FUN_0042f510); gated by FUN_0042b930.
    - **004672d0** — sibling getter returning the frame pointer `DAT_006905b0->(+4)`.

  - **gameplay — KEEP (18)** — vehicle-physics record accessors + collision/trigger:
    - 0045dff0 (float open-interval predicate, subsystem-neutral helper),
    - 00461e90 (pure switch (selector,key)→(code,code); key constants decode as 0xAARRGGBB-style
      colors — possible surface/material classifier, [UNCERTAIN]),
    - 0046bce0/0046bd20 (per-vehicle vec3 getters, record +0x94/+0xa0),
    - 0046bd60/0046bda0 (per-wheel scalar getter / 16-float telemetry sum, base 0x881744/0x881750),
    - 0046be10 (indexed paired-field setter into the per-vehicle record),
    - 0046be50 (6-case per-vehicle paired setter, clamp ≤100, record +0x5c0/+0x5d8),
    - 0046bf50 (accumulate force/velocity vec3 into record +0xf50, or delegate to FUN_00481780),
    - 0046bfc0 (full vehicle collision/impulse solver: AABB contact test + velocity integration
      using FUN_004c3ac0 Vec3Magnitude; **0 callers found — indirect/vtable dispatch, [UNCERTAIN]**),
    - 0046d2e0/0046d320 (duplicate per-wheel getter, base 0x881790; the field audio FUN_00463f40
      labels "surface material code" — but the getter itself is pure vehicle state),
    - 0046d360 (per-wheel getter, base 0x881738),
    - 0046d660 (per-vehicle velocity vec3 getter, record +0xf50; read companion of 0046bf50),
    - 0046d740 (per-vehicle vec3 setter, record +0x6e4),
    - 0046d880 (geometric trigger/zone containment test over wedge volumes, marks per-slot flags),
    - 0046dd80 (global float getter DAT_0061313c, subsystem-neutral),
    - 0046dd90 (per-vehicle field setter record +0x6f4, no bounds check).

- **Shared per-vehicle record**: stride **0xd04 bytes (0x341 dwords)**, base family 0x008815xx-
  0x008822xx, ≤16 vehicles; per-wheel sub-blocks stride **0xc4 bytes**, 0x11 (17) per vehicle,
  ≤4 wheels used. The velocity vec3 lives at record +0xf50 (write 0046bf50/0046bfc0, read 0046d660).

- **U-mint**: every plate carries `## Uncertainties`; recommend minting U-rows for the
  data-semantic [UNCERTAIN] markers (per-vehicle field identities, the 0046bfc0 indirect-caller
  gap, the 00461e90 color-constant classifier hypothesis, 0046dd90 missing bound check).

> DRAINED by sweep-20260603-1259 — 26 plates + 26 bookmarks to master Ghidra, 0 renames, 0 errors. C1->C2 finalize via re-classify follows.
