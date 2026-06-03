# SCRIBE_QUEUE fragment — batch_al session 3 (al_s3)

Author-only promote-c2 pass (gameplay slice 3, 0x0040ad90..0x00412010). Bucket
plates are the C2 deliverable; central finalize (ghidra-sweep) writes hooks.csv /
trackers and commits. This session did NOT touch hooks.csv / STUBS /
UNCERTAINTIES / DEFERRED, did NOT mint U-/S-IDs, did NOT mutate the master Ghidra
project.

## Queued

2026-06-02  al_s3  bucket=re/analysis/bucket_gameplay_0040ad90_00412010  confidence=C1->C2  rvas=0040ad90,0040ae30,0040aef0,0040b420,0040b890,0040b8e0,0040b930,0040b970,0040b9a0,0040ba00,0040ba60,0040baa0,0040bae0,0040be50,0040c5b0,0040c690,0040ca00,0040cd90,0040cda0,0040cf40,0040d430,0040d8f0,0040dc70,0040dcb0,0040e500,00412010  note=subsystem_observed=gameplay (all 26 CONFIRMED, ZERO library-band hits, no reclassifications); library_skip=NONE; needs_function_create=NONE (all 26 are real Ghidra function objects, incl. the thunk 0040cd90); actual pool slot=Mashed_pool2

## Notes for the sweep

- **Count**: 26 RVAs, 26 plates authored in the bucket dir. None drift-skipped —
  all 26 were `gameplay` `C1` in hooks.csv at session start (all tagged
  `batch-t-s1 MULTI[...]`, prior C1 plates under `re/analysis/bucket_00405400/`).
  Each plate was re-verified against the LIVE pool2 decompilation (NO-GUESSING:
  the old C1 notes carried speculative language that was dropped/marked
  `[UNCERTAIN]`).
- **Slot**: opened the pre-assigned `Mashed_pool2` read-only (session
  c9f902cf…, project_name=Mashed_pool2 confirmed via open identity). No leak, no
  fallback needed. `program_close` ran clean at end. Marker `.pool_slot_al_s3`.
- **Subsystem confirmation**: all 26 CONFIRMED `gameplay`. The slice is a
  contiguous game-logic region: object-frame transforms (0040ad90/ae30/aef0),
  game-mode grid-count getters (0040b890/b8e0), a 4-player score-mask classifier
  family over the `0x008a94e0` int[4] table (0040b930/b970/b9a0/ba00/ba60),
  projectile/powerup per-slot effect dispatch + its ring driver
  (0040be50/0040c5b0), a packed-color effect emitter + per-player effect tick
  driver (0040c690/0040ca00), a per-player skid/trail-style slot manager
  (0040d8f0), a slide/hold ramp state machine (0040dc70/0040dcb0), a player-search
  loop (0040e500), and clump teardown (0040cf40/00412010). NO third-party-library
  reclass-OUTs.

- **library_skip = NONE (boundary detail for re-classify)**: three functions
  ORCHESTRATE named RenderWare clump APIs but are themselves application code,
  so they stay `gameplay` and are plated (not skipped):
  - `0040cda0` calls `RpClumpForAllAtomics` (`0x004e66d0`, named in project) over
    clump `param_1` with the `LAB_0040bb10` per-atomic callback ("shine" effect).
  - `00412010` calls `RpClumpDestroy` (`0x004e6e00`, named in project) — bulk
    teardown of 6 records' `+0x3c` clumps + a trailing singleton at `0x0063bc40`.
  - `0040bae0` calls the generic foreach `FUN_004e89a0` (NOT named; same
    `*ForAll*` shape) with `FUN_0040baa0` (sibling in this bucket) as the callback.
  Confirm at finalize that none of the 26 is itself a named `Rw*/Rp*/Rt*`
  primitive (they are not).

- **Boundary-flags (kept gameplay; flag only if central re-classify wants to
  re-home)**:
  - hud-adjacent: the score-mask family `0040b930/b970/b9a0/ba00/ba60` (writes
    per-player 0/1 masks consumed by callers in the `0x0041a***`/`0x0041c***`
    cluster), and the slide/hold ramp machine `0040dcb0` (reset `320.0`, `5.0`
    hold) — both operate on game-state, kept gameplay.
  - particle/render-adjacent: `0040be50/0040c690/0040ca00/0040d8f0` emit effects
    via `FUN_0048aa50` / `FUN_0048cf90` (out-of-bucket emitters) but are driven by
    gameplay state + input flags (`DAT_007f101c`), kept gameplay.

- **Calling-convention boundary flag**: `0040be50` takes its per-slot index in
  **EAX** (Ghidra renders it as `in_EAX`, no declared parameter); the sole caller
  `0040c5b0` loads EAX (= its loop index `iVar6`) before the `call`. Worth
  recording for any future hook signature on `0040be50`.

- **Thunk**: `0040cd90` is a real Ghidra thunk function object
  (`thunk_FUN_00425ef0`, 5-byte `JMP 0x00425ef0`). Plated through the thunk
  (target `FUN_00425ef0` is a pure-data leaf counting 8 records with both tested
  DWORDs non-zero). No `function_create` needed.

- **Named callees confirmed in-project** (cite in re-classify): `0x004e66d0` =
  `RpClumpForAllAtomics`, `0x004e6e00` = `RpClumpDestroy`. `0x004c3ac0`
  (referenced by `0040be50`/`0040ca00`) is the project's mapped `Vec3Magnitude`
  address but is still `FUN_004c3ac0` in the Ghidra master — cited by RVA, not
  renamed here.

- **Constants**: NOT read via `memory_read` this session — every `_DAT_005cc***`
  / `_DAT_005ccd**` threshold and `DAT_005d757c` is cited by ADDRESS only in the
  plates and remains data-semantic `[UNCERTAIN]` (non-blocking for C2 per the
  "data-semantic uncertainties don't gate a verified leaf" rule). `DAT_005d757c`
  recurs as the pervasive near-zero/zero sentinel (prior batches read it as
  `0.0f`); not re-verified this session.

- **No pre-existing U-/S-IDs in this bucket** (the prior C1 plates used bare
  `[UNCERTAIN]`, no minted IDs). New holes are marked as bare `[UNCERTAIN]`
  inline in each plate's `## Uncertainties` section (chiefly: the data-semantic
  float values; the unused `FUN_004672d0(0)` result in `0040cda0`; the EAX-arg
  convention in `0040be50`). Central re-classify mints IDs from the reserved
  range U-7700..U-7999 / S-6200..S-6399 as needed.

- **No new S-IDs**, no `function_create`, no master writes. Files left as the
  bucket dir + this fragment + `.pool_slot_al_s3` for the single end-of-session
  commit.
