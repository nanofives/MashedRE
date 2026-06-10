# SCRIBE_QUEUE fragment — batch_am session 6 (am_s6)

Author-only promote-c2 pass (gameplay campaign 2/~5, sixth/last slice of batch_am,
closes the next-156 region 0x0041e140..0x00455fe0 at its top end 0x00454130..0x00455fe0).
Bucket plates are the C2 deliverable; central finalize (ghidra-sweep + re-classify)
writes hooks.csv / trackers and commits.

## Queued

2026-06-03  am_s6  bucket=re/analysis/bucket_gameplay_00454130_00455fe0  confidence=C1->C2  rvas=00454130,00454170,004541e0,00454350,00454790,004547c0,00454820,00454a20,00454a30,00454bd0,00454c00,00454c10,00454c60,00454cd0,00454f80,00454fa0,00454ff0,00455030,00455060,00455100,00455150,00455610,004556f0,00455910,00455b50,00455fe0  note=subsystem_observed MIXED — this whole slice is ONE powerup-weapon subsystem (depth-charge/mine + homing-missile); ~12 stay gameplay (weapon entity logic), ~14 recommend reclass to render/hud/effects (visual-frame mgmt + overlay draw). Per-RVA below. needs_function_create=NONE (all 26 had function objects; 00454a20 is a thunk→FUN_004548a0). library_skip=NONE (no candidate decodes as a library primitive; only named RW *imports* RpAtomicGetWorldBoundingSphere/RwFrameRemoveChild/RwFrameAddChild and CRT fsin/FUN_004a3384 appear as call targets). actual pool slot=Mashed_pool10 (pre-assigned, verified-free).

## Notes for the sweep

- **Count**: 26 RVAs, 26 plates authored in the bucket dir. **None drift-skipped** —
  all were `gameplay/C1` per the working set at session start. All 26 returned non-null
  `function_at`. Every plate uses the `## Mechanical description` heading; none use
  `## Purpose`. `00454a20` is `thunk:true` (→ FUN_004548a0); `00454548a0` itself is
  already C2 in master (its thunk is the candidate).

- **POOL SLOT**: pre-assigned **Mashed_pool10** had no top-level `.lock`/`.lock~`;
  opened read-only via `project_program_open_existing`, `program_close`d cleanly.
  Recorded in `.pool_slot_am_s6`. (pool0/4/5/6 carried active locks per the batch
  header — not touched.)

- **SUBSYSTEM MAP — one weapon subsystem, two entity systems.** The working "gameplay"
  hypothesis is HALF right: the weapon *logic* is gameplay; the *visual* plumbing is
  render/hud/effects. Recommend the central re-classify split as follows.

  **System 1 — Depth-charge / mine** (Pool A `0x00688250` 4×0x2c markers; Pool B
  `0x00688020` 8×0x44 entities; marker array `0x006882f8` 4×0x18):
  - `00454820` master per-frame tick (← FUN_0045bba0); `00454350` per-entity state
    machine (drop→flight→collision→detonate→cleanup); `004541e0` spawn-from-stack;
    `00454170` entity reset; `00454790` reset-all sweep (← FUN_0045bed0); `004547c0`
    marker reset; `00454a20` thunk→teardown (← FUN_0045be90/FUN_0045bf30); `00454a30`
    marker-array accessor (← FUN_004177b0). → **KEEP gameplay** (weapon entity logic).
  - `00454130` Pool-A render dispatch (vtable+0x48, ← FUN_0045b990) → **render**
    (per-record draw dispatch, no game logic).
  - `00454cd0` 2D screen-flash overlay (FUN_00476xxx sprite API) → **render**
    (2D-overlay), the explosion white-flash.
  - `00454bd0`/`00454c00`/`00454c10`/`00454c60` trail/marker-record managers
    (register/free/advance + matrix-apply on the 0x18 marker array) → **render/effects**
    (borderline; visual trail bookkeeping, indirectly dispatched).

  **System 2 — Homing missile** (Pool E `0x006883b0` 5×0x6c entities; Pool F
  `0x006885d0` ×0x2c owner/visual-attach; Pool G `0x006886b0` 4×0x48):
  - `00455150` missile spawner/launcher (mode 2; uses **C3 hook FUN_004c3ac0
    Vec3Magnitude**); `00455610` non-homing flight integrator; `004556f0` homing flight
    integrator (uses **FUN_004c3ac0**); `00455910` detonation handler. → **KEEP
    gameplay** (weapon spawn/flight/AI/detonation).
  - `00454f80` Pool-E rec0 frame accessor (← FUN_00407800); `00454fa0`/`00454ff0`
    Pool-E sub-block resets; `00455030` Pool-E reset sweep; `00455060` Pool-F scene-
    graph registrar (attach+show); `00455100` Pool-E UV-anim tick; `00455fe0` Pool-G
    reset (← FUN_00456c70). → **render** (RwFrame/atomic scene-graph + visibility mgmt).
  - `00455b50` homing target lock-on indicator (billboard reticle, FUN_00459620 draw) →
    **hud / render** (overlay indicator, not game logic).

  Net recommendation (central re-classify to honor with the above evidence):
  **KEEP gameplay (12)**: 00454170, 004541e0, 00454350, 00454790, 004547c0, 00454820,
  00454a20, 00454a30, 00455150, 00455610, 004556f0, 00455910.
  **→ render (13)**: 00454130, 00454cd0, 00454bd0, 00454c00, 00454c10, 00454c60,
  00454f80, 00454fa0, 00454ff0, 00455030, 00455060, 00455100, 00455fe0.
  **→ hud (1)**: 00455b50 (lock-on indicator).
  (Exact gameplay/render/hud bucket counts: gameplay 12 + render 13 + hud 1 = 26. The
  four trail-record managers 00454bd0/c00/c10/c60 are the softest "render/effects"
  calls — if the project keeps a separate `effects` subsystem they belong there;
  otherwise render.)

- **C3 hook touched (not a candidate)**: `FUN_004c3ac0` (Vec3Magnitude, a landed C3
  hook) is called by the two missile launchers/integrators `00455150` and `004556f0`
  to derive launch/closing speed. Noted in those plates; depth-1, not minted.

- **Pool-E record-count correction**: the prior C1 plate for `00455030` said "8
  entries"; the literal loop bounds (base 0x006883b0, end 0x006885cc, stride 0x6c) give
  `0x21c/0x6c = 5` records. Plates report **5** (NO-GUESSING; cited from the loop
  arithmetic). Pool A/B/F/G counts confirmed from their iterators (4/8/?/4).

- **Indirect dispatch**: 13 of the 26 (the per-record update/integrator/spawner/handler
  methods: 00454130's targets, 004541e0, 00454c00, 00454c10, 00454c60, 00454cd0,
  00455060, 00455100, 00455150, 00455610, 004556f0, 00455910, 00455b50) have **0 direct
  callers** — they are invoked via function-pointer/vtable tables (the `+0x48` slot seen
  in 00454130). `callers_noted: []` in those plates reflects this; it is not a missing-
  data gap. The direct-caller drivers resolved are FUN_0045b990 / FUN_0045bba0 /
  FUN_0045bed0 / FUN_0045be90 / FUN_0045bf30 / FUN_00407800 / FUN_004177b0 / FUN_00456c70.

- **Uncertainties carried** (all data-semantic / decompiler-artifact, NON-BLOCKING for
  C2 of these control-flow-complete reads; bare `[UNCERTAIN]` in plates, NOT minted —
  central re-classify mints from U-8000..U-8299):
  - Numeric values of the many `_DAT_005c*` / `_DAT_007f100c` tuning floats not dumped
    (cited by address only across 00454350 / 004541e0 / 00454cd0 / 00455100 / 00455150
    / 00455610 / 004556f0).
  - vtable+0x48 method semantics (00454130); FUN_004726f0 curve-lookup + target-resolve
    chain FUN_0045a0f0/FUN_0045a0d0/FUN_00407640/FUN_00407620/FUN_0046d4a0/FUN_0046d6d0
    semantics (00455150 / 004556f0 / 00455b50).
  - 0x18-stride marker-array (0x006882f8) vs 0x2c marker-pool (0x00688250) field
    reconciliation (00454a30 / 00454c00 / 00454c10).
  - `RwFrameRemoveChild(uVar2)` single-arg decompiler artifact in 00455fe0 (listing has
    both operands).
  - thunk_FUN_00426060 (scene-root getter) identity (00455060 / 00455150 / 00455fe0).

- **needs_function_create = NONE.** **library_skip = NONE.** No new U-IDs / S-IDs /
  arg_types minted. No Frida, no build, no re-classify, no hooks.csv write. Per
  author-only mission only the bucket dir, this fragment, and `.pool_slot_am_s6` were
  created/modified.

> DRAINED by sweep-20260603-0427 — 26 plates + 26 bookmarks to master Ghidra, 0 renames, 0 errors. C1->C2 finalize via re-classify follows.
