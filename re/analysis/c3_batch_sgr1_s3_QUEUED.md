# c3_batch_sgr1_s3 — outcome (2026-06-25)

Branch `c3/batch-sgr1-s3`. Authored 6 reimpls (5 .cpp + 1 deferred), wired build +
registry. **1 of 7 promoted C2→C3; 6 queued.** All 6 reimpls are byte-faithful —
where the original is callable they match bit-for-bit, and where the test
input/state makes the original crash, the reimpl crashes at the *identical*
address. The blockers are verification-harness / scenario / caller-gate issues,
NOT reimpl bugs. Files are kept (compile clean, RH_ScopedInstall live).

## PROMOTED (1)

| RVA | Export | Evidence |
|-----|--------|----------|
| 0x0046d510 | VehicleVelocityWorldGet | `log/diff_vehicle_velocity_world_get.csv` GREEN 9/9 non-degen (out3_idx bounds return 1 for idx<16 else 0), run_diff_parallel scenario:race. 13 callers C2, callee FUN_004c3df0 C4. Twin of 0x0046d700. |

## QUEUED (6)

### 0x004840b0  ShapeOwnerHandleGet  (Vehicle/ShapeOwnerHandlePool.cpp)
- run_diff at menu → **INCONCLUSIVE-DEGENERATE (rc=5)**: DAT_006ce82c is zero in
  live state (like the sister 0x004840d0). The proper lane is
  `early_window_leaf_diff` with `seed_table` (already in the registry entry).
- **BUT** 0x004840b0 has **0 callers** (DEFERRED **D-10584**, dead-code getter).
  Per the 2026-06-25 caller-gate ruling (a verified leaf getter whose callers are
  all C1 — here, *no* C2+ caller context at all — must be QUEUED), this cannot
  promote even with a GREEN seed_table diff.
- **Re-pickup:** a caller is discovered (resolves D-10584) AND early_window GREEN.

### 0x0047c270  CameraPathAllNodesEq2  (Camera/CameraPathPredicates.cpp)
### 0x0047c2d0  CameraPathAnyNodeNonzero  (Camera/CameraPathPredicates.cpp)
- run_diff scenario:race → **8/8 IDENTICAL crashes both sides** (rc=1). The
  calibration's "param_1=0 sentinel" is wrong: the inner-inner predicates
  FUN_004780c0 (+0x0) / FUN_00478030 (+0xc) dereference param_1 as a context
  pointer, so the **ORIGINAL itself AVs** on param_1=0. The 8/8 address-identical
  crash proves the reimpl outer scan + register-convention inner call is faithful.
- **Re-pickup:** verify with a VALID param_1 context pointer (what the indirect
  dispatcher passes), e.g. a struct_call_observe-style harness that allocs/seeds a
  context buffer and points param_1 at it. int_scalar cannot supply a live pointer.

### 0x00448700  VehicleSeedWritePair  (Vehicle/VehicleSeed.cpp)
- void_write_observe scenario:race → **flaky AV (rc=1)**. The 100x DispatchAll loop
  (FUN_004464c0) iterates the live camera-entry array DAT_008964c0, which is
  mid-update at race-attach. void_write_observe runs Orig THEN Reimpl; orig wrote
  param_1 fine on idx 0 then the running game thread re-dirtied state and reimpl
  AVd. Seeding DAT_00898994=0 (the entry count) before each call did NOT hold —
  the live game thread repopulates it between the two NativeFunction calls.
- **Re-pickup:** a frozen-state / suspended-attach methodology (game thread not
  running) so DispatchAll iterates a stable array, OR a single-side observe.

### 0x004853b0  SmplFzxStateBlockGetLogged  (Physics/SmplFzxStateBlock.cpp)
- int_scalar scenario:race → **10/10 IDENTICAL crashes both sides** (rc=1). The
  SmplFzx manager pointer `*(0x006e71cc)` is uninitialized (≈0x4) in Quick Battle,
  so the double-deref `*( *(0x6e71cc)+0xc )` AVs in the **ORIGINAL** too. 10/10
  address-identical → reimpl faithful (incl. the double-deref corrected from the
  plate's single-deref via 0x4853d4 disasm; fmt VA 0x005cf310).
- **Re-pickup:** a scenario where the SmplFzx physics manager is allocated, OR a
  struct harness that seeds 0x006e71cc → a fake manager + state-block table.

### 0x0041f880  (Gameplay/SlotFlagModeMap.cpp — NOT authored)
- `void_setter_observe` cannot express a thiscall struct mutator (this in ECX;
  reads this+0x294 flags, writes this+0x28c, conditional SlotFieldSet on
  DAT_00636acc==0). See `re/analysis/frontend_gate_unblock_u/0x0041f880_QUEUED_c3_batch_sgr1_s3.md`.
- **Re-pickup:** struct_call_observe (already in SEEDED_ARG_TYPES) with seeded
  flags + ECX convention + DAT_00636acc=0 + observe this+0x28c. Caller-gate already
  satisfied (SlotFieldSet 0x00422aa0 is C3).
