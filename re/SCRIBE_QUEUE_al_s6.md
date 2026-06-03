# SCRIBE_QUEUE fragment — batch_al session 6 (al_s6)

Author-only promote-c2 pass (gameplay campaign 1/~5, sixth/last slice, closes the
lowest-156 region 0x00405400..0x0041d910). Bucket plates are the C2 deliverable;
central finalize (ghidra-sweep + re-classify) writes hooks.csv / trackers and commits.

## Queued

2026-06-02  al_s6  bucket=re/analysis/bucket_gameplay_0041a980_0041d910  confidence=C1->C2  rvas=0041a980,0041a9d0,0041aac0,0041ac60,0041ad60,0041adb0,0041ae20,0041ae60,0041af00,0041af50,0041b440,0041b520,0041b690,0041b720,0041b770,0041b7a0,0041beb0,0041c320,0041c380,0041c410,0041cb00,0041cd20,0041cdb0,0041ce00,0041d6d0,0041d910  note=subsystem_observed=render(particle-emitter/effect) for ALL 26 (recommend reclass gameplay->render); 4 ParticleEmitter classes + ghost-vehicle visual tint; needs_function_create=NONE; library_skip=NONE; actual pool slot=Mashed_pool10 (pre-assigned pool6 POISONED)

## Notes for the sweep

- **Count**: 26 RVAs, 26 plates authored in the bucket dir. **None drift-skipped** —
  all 26 were `gameplay/C1/mapped` per hooks.csv at session start (existing C1 plates
  in `re/analysis/bucket_00412130/`); none already C2+. All 26 returned non-null
  `function_at`. Every plate uses the `## Mechanical description` heading.

- **POOL SLOT**: pre-assigned **Mashed_pool6 was POISONED** — `project_program_open_existing`
  failed with `ghidra.framework.store.LockException` (a stale top-level
  `mashed_pool/Mashed_pool6.lock~` from 2026-06-02 19:11; the shared MCP reported 0
  open sessions and pinged fresh). Per [[feedback_mcp_leaked_project_lock]] I fell
  back to a verified-free clone **Mashed_pool10** (no top-level `.lock~`; pool8 had
  just been claimed 22:46 by a sibling so I avoided it; pool4 SKIPPED per batch).
  Opened read-only, `program_close`d cleanly. Recorded in `.pool_slot_al_s6`.

- **SUBSYSTEM — the "gameplay" hypothesis is WRONG for all 26; recommend reclass to
  `render` (particle-emitter / effect rendering)**. These are NOT game-logic; they
  are RenderWare clump/atomic/material manipulation for per-player on-screen effect
  emitters + ghost-vehicle visual tint. Their callers are all already-C2 **hud**
  (leaderboard_d3) and **boot** (panel-init) functions:
  - **4 ParticleEmitter classes** (ctor / config|packmask / per-frame resolver / dtor),
    each keyed by a per-record `+type` index into the shared `&DAT_007f1a1c`
    bit-position byte table and a per-type vec3 table:
    - **Class A — 17-atomic** (record stride 0x74 @ DAT_0063c8d0, 4 records):
      ctor 0041ad60, config 0041adb0, vec3-reset 0041ae20, low-bit apply 0041ae60,
      high-bit apply 0041af00, per-frame 0041af50, dtor 0041b440, array-iter 0041b520.
    - **Class B — 29-atomic** (record stride 0x16c @ DAT_0063cab8, 2 records):
      ctor 0041b690, packmask 0041b720, vec3-copy 0041b770, per-frame resolver
      0041b7a0, dtor 0041beb0.
    - **Class C — 24-atomic** (record stride 0x114 @ DAT_0063ce20):
      ctor 0041c320, config 0041c380, per-frame 0041c410, dtor 0041cb00.
    - **Class D — 34-atomic** (record stride 0x160 @ DAT_0063d298, 2 records):
      ctor 0041cd20, packmask 0041cdb0, per-frame resolver 0041ce00, dtor 0041d6d0,
      counter-reset 0041d910.
  - **Ghost-vehicle visual tint (4)**: teardown 0041a980 (caller FUN_0040cf80),
    tint-scale 0041a9d0, secondary-tint phase accumulator 0041aac0, distance-alpha
    0041ac60 — last three driven per-frame by FUN_0041ad00. 2-slot anchor table @
    DAT_0063c82c.
  - **Caller→subsystem map** (all callers already C2): per-frame drivers ←
    FUN_0041b540/0041c090/0041cc50/0041d830/0041ad00 (**hud** leaderboard_d3 + ghost
    update); ctors ← FUN_0041b450/0041bec0/0041cb10/0041d6e0 (**boot** panel-init);
    config/iter ← FUN_0041bf20/0041c010/0041cb80/0040dbd0/0041d730.

- **Named RW import resolved**: the C1 plates' raw `FUN_004e6e00` is the master's
  **`RpClumpDestroy`** (used by the four dtors 0041b440/0041beb0/0041cb00/0041d6d0
  and the teardown 0041a980). Recorded in the plates.

- **Recovered from disassembly (not just decomp)**: **0041b520** — the degraded
  decomp elides the loop counter (`extraout_ECX`); listing shows `MOV ECX,0x63c8d0`
  / `CALL 0x0041ae20` / `ADD ECX,0x74` / `CMP ECX,0x63caa0` / `JL` → 4 records of
  stride 0x74 at DAT_0063c8d0. Confirms the Class A record array.

- **Live constants decoded (Mashed_pool10, IEEE-754 LE)** — now in the plates:
  - `_DAT_005cd188 = -90.0f` (0xc2b40000, ghost Z tint-scale, 0041a9d0).
  - `_DAT_005ccac4 = 360.0f` (0x43b40000, phase wrap modulus — accumulators are
    degrees, 0041aac0).
  - `_DAT_005cc56c = 0.1f`, `_DAT_005cc32c = 0.5f` (ghost distance-alpha scale +
    far-cap, 0041ac60). `DAT_005d757c = 0.0f` (shared zero sentinel).
  - `_DAT_005cc320 = 1.0f`, `_DAT_005cd11c = 0.015f`, `_DAT_005cd118 = 1.125f`
    (tint), `_DAT_005cc94c = 2^32 (4294967296.0f)` (negative-phase wrap addend),
    `_DAT_005cc9c0 = 0.2f` (osc freq), `_DAT_005cc9e0 = 25.0f`, `_DAT_005cd18c =
    0.04f` (fade rate) — Class A per-frame 0041af50.
  - `_DAT_005cc8f0 = 0.15f` (Class C/D pulse amplitude, 0041c410 / 0041ce00).

- **Shared callee glue (NOT minted — sweep owns S-IDs)**: FUN_004b3fc0 (clump
  build/load + atomic-handle enumerate), FUN_004b6520 (zero-fill/memset),
  FUN_004b5190 (handle→in-clump-index), FUN_004b5260 (set atomic/material color),
  FUN_004c13e0 (geometry tint/scale apply), FUN_004c51a0 (compose RwMatrix),
  FUN_004c1480 (apply matrix to frame), FUN_0040b6d0/FUN_0040b890 (aspect num/den +
  mode code), FUN_0040b420 (state classifier returning {−2..2}), FUN_0042f500
  (game-state predicate, 0041c380), FUN_0041f260 (reference-position getter,
  0041ac60), FUN_004c39b0 (vec3 length/normalize), FUN_004c3ac0 (Vec3Magnitude — a
  C3 hook). Named RW import: RpClumpDestroy.

- **Uncertainties carried** (all data-semantic / decompiler-artifact, NON-BLOCKING
  for C2 of these control-flow-complete reads; bare `[UNCERTAIN]` in plates, NOT
  minted — sweep mints from U-7700..U-7999):
  - packmask kind-index array base (+0x00) overlaps the per-frame visibility-result
    region (+0x10+i*4) — layout not fully reconciled vs handle table at +0xb0
    (0041b720, 0041cdb0).
  - `&DAT_007f1a1c` (kind→bit-position byte table) + per-type vec3 tables
    (`&DAT_005f3304/334c/337c/33c4`) contents not dumped — cited by indexing only.
  - FUN_0040b420 {−2,−1,0,1,2}→bit map semantics; FUN_0040b6d0/0040b890 aspect/mode
    codes (8/0xc); DAT_007f0fd0 game-mode enum (1/2/7); FUN_0042f500 predicate.
  - 29/34 declared atoms vs 40 visibility slots scanned in the resolvers (extra
    slots default-invisible; cases 0x11..0x15 distinguish Class D from Class B).
  - trailing vararg args on the last FUN_004c13e0 in 0041c410 / 0041ce00 (stack
    artifact). DAT_0063d584/d588 semantics (0041d910, adjacent to osc phase d558).

- **needs_function_create = NONE.** **library_skip = NONE** (slice is below CRT /
  D3DX9-PSGP / qhull bands; no RVA decoded as a named library primitive — only the
  named RW *import* RpClumpDestroy, which is a call target not a candidate).

- **No new U-IDs / S-IDs / arg_types minted. No Frida, no build, no re-classify, no
  hooks.csv write.** Per author-only mission: only the bucket dir, this fragment,
  and `.pool_slot_al_s6` were created/modified.

> DRAINED by sweep-20260603-0334 — 26 plates + 26 bookmarks to master Ghidra, 0 renames, 0 errors. C1->C2 finalize via re-classify follows.
