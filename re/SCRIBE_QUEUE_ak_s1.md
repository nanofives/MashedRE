# SCRIBE_QUEUE fragment — batch_ak session 1 (ak_s1)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep) writes hooks.csv / trackers and commits.

## Queued

2026-06-02  ak_s1  bucket=re/analysis/bucket_vehicle_00453f30_00482030  confidence=C1->C2  rvas=00453f30,004548a0,004548e0,00456760,004587a0,00459290,0045ba10,0046b1c0,0046b540,00474d60,00476c10,00476cb0,004770c0,004781b0,0047a020,0047e9c0,0047ea60,00480b70,00480d60,00481750,00481780,00481810,004818d0,00481a30,00481d90,00481e00,00482030

## Notes for the sweep

- **Count**: 27 RVAs, 27 plates authored in the bucket dir. None drift-skipped (all 27 were
  `vehicle/C1` per hooks.csv at session start; none already C2+). `0045ba10.md` pre-existed
  from an earlier interrupted ak_s1 attempt (already C2-quality, opened_in_slot Mashed_pool0)
  and was kept verbatim; the other 26 are this session.

- **POOL SLOT**: pre-assigned **Mashed_pool0 was poisoned** (a live process held
  `mashed_pool/Mashed_pool0.lock~`; the shared MCP reported 0 open sessions, and the stale
  `.lock` was removed but `.lock~` stayed locked). Per [[feedback_mcp_leaked_project_lock]] I fell
  back to a verified-free clone and opened **Mashed_pool8** read-only; `program_close`d cleanly.
  Recorded in `.pool_slot_ak_s1`. (pool3/pool4 also locked by sibling sessions at the time.)

- **Subsystem findings** — the "vehicle" hypothesis splits: **12 CONFIRMED vehicle**, **15 with a
  non-vehicle subsystem_observed** (recommend the sweep reclassify out of `vehicle`; I did NOT
  edit hooks.csv — author-only):
  - **CONFIRMED vehicle (12)**: 0046b1c0 (AABB→24-float per-slot physics geom),
    0046b540 (suspension/spring/mass init), 0047ea60 (spawn-pose placement),
    00480b70 (wheel-contact impulse), 00480d60 (9-case impulse dispatcher),
    00481750 (impulse wrapper), 00481780 (velocity accumulator),
    00481810 (collision contact query), 004818d0 (velocity cap/over-speed),
    00481a30 (per-frame proximity/zone loop), 00481d90 (body activation),
    00481e00 (AI multi-ray cone probe). All key the per-vehicle 0xd04 / 0x341 blocks
    (0x00881630…) and the DAT_006c9a78 / DAT_006c71d8 physics-body arrays.
  - **powerups (7)** — powerup weapon-type init cluster, all dispatched from FUN_0045bae0
    (PTR_FUN_005f99ac table): 00453f30 (100×36 table invalidate), 004548a0 (DepthCharge entry
    activate), 004548e0 (DepthCharge type init), 00456760 (GatlingGun type init),
    004587a0 (PowerUpIcons display init), 00459290 (Laser/lazer type init),
    0045ba10 (per-slot effect-struct reset).
  - **particle/effect (3)**: 004770c0 (core particle-system init, 18 callers across engine),
    00476c10 (effect-object 4-float vector setter), 00476cb0 (effect-object 2-field setter).
  - **render / RW helper (3)**: 00474d60 (RpClumpForAllAtomics wrapper), 004781b0
    (RpClumpStreamRead-from-file), 00482030 (quadrant UV/texcoord generator — pure math).
  - **physics / collision (2)**: 0047a020 (course/BSP "CourseDescInit" + COURSE.LUA; single
    caller = track loader FUN_00426e10), 0047e9c0 (12-call RW-Physics scene init;
    caller FUN_0047ea40). NB project memory flags 0047b9b0 (called by 0047a020) as a kept-label
    physics-vs-script boundary — keep 0047a020's reclass conservative.

- **Corrections to prior C1 plates** (sweep should prefer these C2 values):
  1. **00474d60**: callee is the named RW API **`RpClumpForAllAtomics`** (resolved at 0x004e66d0),
     not raw `FUN_004e66d0`; callback = `LAB_00474d30` (label precedes the function body).
  2. **004781b0**: read callee is **`RpClumpStreamRead`** (resolved at 0x004e7420), not raw
     `FUN_004e7420`; opens via FUN_004cc230(type 2,mode 1), seeks chunk 0x10 (rwID_CLUMP).
  3. **0046b540**: the decompiler token `s_Afff_SandSpWheel_005cc753._1_4_` is **not a string** —
     it is the float **3.6f** (0x40666666 at 0x005cc754, immediately preceding the "SandSpWheel"
     string). Resolves the prior [UNCERTAIN] about that multiplier.
  4. **0047ea60**: spawn-position scale `_DAT_005cc33c` = **-1.0f** (0xbf800000, read live) — a
     negation; prior plate left it unvalued.

- **Live constants read (Mashed_pool8)** — decoded IEEE-754 LE, now in the plates:
  0046b1c0: _DAT_005cc32c=0.5f, _DAT_005ce034=0.33333f. 0046b540: _DAT_005cc558≈0.001f,
  _DAT_005cea54≈1.05f, _DAT_005cc320=1.0f, 3.6f (above). 0047ea60: _DAT_005cc33c=-1.0f.
  00480d60: _DAT_005cf218=-6.5, _DAT_005cf21c/20=∓0.2222, _DAT_005cf224/28=∓0.1111,
  _DAT_005cf22c≈-6.66e-4. 00481780: _DAT_005cf230≈0.002777 (≈1/360).
  004818d0: _DAT_005cc990≈1.0e-5 (motion flag), _DAT_005cd0ec≈0.005 (vel damp).
  00482030: _DAT_005cc35c=4.0f.

- **U-ID drift to reconcile**: 00480d60 — the prior plate's frontmatter cites **U-3708** while
  its body text cites **U-3704** for the same "switch case 0..8 → game-event mapping" hole.
  Sweep should collapse to one ID.

- **Uncertainties carried** (all data-semantic / decompiler-artifact, NON-BLOCKING for C2 of these
  control-flow-complete reads; NOT minted — sweep owns numbering): U-0469/0470/0471 (0045ba10
  ESI struct + FUN_004b6520), U-0474..U-0481 (powerups array/field semantics), U-0643/U-0644
  (0047a020 struct-A init + Lua runner protocol), U-1449 (004781b0 struct-vs-filename arg),
  U-3704/U-3708 (00480d60 case→event map), D-10354-fill/D-10355-fill (0046b1c0/0046b540 field
  semantics), S-3724..S-3729 set (0047e9c0 callees). **needs-U-ID** (new, NOT minted): 00481810
  + 00481d90 have **no direct callers** (indirect/dispatch entry not located); 0047ea60's
  `_DAT_005cc33c=-1.0` negation rationale; 00481a30 zone-state codes 1/2 + counter-range callback
  selection.

- **Stubs noted** (NOT minted — sweep owns numbering). Recurring shared physics/RW helpers across
  the vehicle plates: FUN_0057c210 (physics-body resolve from handle), FUN_0055b650 (apply impulse
  at world point), FUN_004c3df0 (transform point by matrix), FUN_004c51a0 (write RW matrix/frame),
  FUN_0046d4a0 (vehicle-state getter), FUN_0055ac00/0055ac50/0055ad30 (RW-Physics scene
  insert/activate triad). Named RW imports observed: RpClumpForAllAtomics, RpClumpStreamRead,
  RpClumpDestroy, RpAtomicGetWorldBoundingSphere. Particle helpers: FUN_00534b60 (particle create),
  FUN_004e8090 (bind texture), FUN_00474db0 (per-channel alloc). Prior IDs to reuse where they
  exist: S-1441/S-1441B/S-1441C/S-1441D, S-1443, S-1444, S-1448..S-1452, S-0460.

- **needs_function_create = NONE.** All 27 RVAs returned non-null `function_at`.

- **No new U-IDs / S-IDs / arg_types minted. No Frida, no build, no re-classify, no hooks.csv
  write.** Per author-only mission: only the bucket dir, this fragment, and `.pool_slot_ak_s1`
  were created/modified.
