# SCRIBE_QUEUE fragment — batch_al session 4 (al_s4)

Author-only promote-c2 pass (gameplay campaign batch 1/~5, slice 0x00412100-0x00418e50).
Bucket plates are the C2 deliverable; central finalize (ghidra-sweep + re-classify)
writes hooks.csv / trackers and commits.

## Queued

2026-06-02  al_s4  bucket=re/analysis/bucket_gameplay_00412100_00418e50  confidence=C1->C2  rvas=00412100,00412130,00412190,00412620,00412880,004128f0,00412e30,00413b80,00413bb0,00413bc0,00413cb0,00413f50,00413fa0,00416230,00417370,004173a0,004173b0,004173e0,00417750,00418a00,00418a70,00418aa0,00418bd0,00418db0,00418e30,00418e50

## Notes for the sweep

- **Count**: 26 RVAs, 26 plates authored in `bucket_gameplay_00412100_00418e50/`. None
  drift-skipped (all 26 were `gameplay/C1` per hooks.csv at session start; none already
  C2+). All read end-to-end against live decomp in Mashed_pool3.

- **POOL SLOT**: pre-assigned **Mashed_pool3** opened read-only (verified unlocked; no
  `.lock`/`.lock~`). `program_close` clean (session 0f9a1c157bf64df9a66bf1b74e523969).
  Recorded in `.pool_slot_al_s4`. No leak.

- **needs_function_create = NONE.** All 26 returned non-null `function_at` and
  decompiled cleanly.

- **library_skip = NONE.** Zero vendored-library hits (no CRT/D3DX9/qhull/RW primitive
  decoded as a named family); the whole slice is genuine game/engine glue.

- **Subsystem findings** — the "gameplay" hypothesis splits three ways. This slice is the
  HUD/effects "VehicleIcons + wheel-trail-decal + drift-mark" cluster (matches the existing
  hooks.csv batch_v note). I did NOT edit hooks.csv (author-only); recommend the central
  re-classify reconcile as below:
  - **CONFIRMED gameplay (7)** — per-player stat/state table (0x0089a4fc/0x0089a500,
    stride 0x74) + aggregate-stat block (0x0089a3xx/0x0089a420): 00412130 (mode/slot-count
    helper), 00413fa0 (mode*3+bias bucket index), 00416230 (player-state +0x00 setter),
    00417370 (player rand ±180 init), 004173a0 (aggregate float getter _DAT_0089a360),
    004173b0 (player rand ±120 init), 00417750 (per-event stat accumulator).
  - **hud (6)** — VehicleIcons sprite-batch object `&DAT_0063bd50` + icon vertex buffer
    `&DAT_00828320`: 004128f0 (icon quad builder), 00413b80 (batch init "VehicleIcons" 5×6),
    00413bb0 (batch teardown), 00413bc0 (atlas UV-rect, cell 0.25), 00413cb0 (animated
    sparkle-grid draw), 00413f50 (render-state-bracketed batch flush).
  - **render / effects (13)** — wheel-trail-decal array `&DAT_0063bb38` (6 slots, stride
    0x2c) + drift-mark array `&DAT_0063bda0` (4 recs, stride 0x64) + particle pool:
    00412100 (trail tick), 00412190 (trail slot builder), 00412620 (trail per-frame
    update), 00412880 (thunk→00412190), 00412e30 (particle spawn lerp, 128-pool),
    004173e0 (world decal-grid lookup — render/collision, see caveat), 00418a00
    (drift-mark ctor), 00418a70 (per-record conditional draw, +0x34==2), 00418aa0
    (emitter RW-atomic transform init), 00418bd0 (raycast→matrix builder), 00418db0
    (drift-mark destroy pass, 4 recs), 00418e30 (drift-mark per-frame tick, 4 recs),
    00418e50 (reset 64-float array to 1.75f).
  - **Caveat (genuinely ambiguous)**: 004173e0 is a two-level world-grid byte lookup
    (`&DAT_007f1a9c` short[] → `&DAT_007f9a9c` 8×8 byte tiles); could be render-decal OR
    gameplay-collision. Marked render but flagged for the sweep to decide.

- **Corrections to prior C1 plates** (sweep should prefer these C2 values):
  1. **00418e50**: fill constant `0x3fe00000` = **1.75f**, NOT 0.875f (the C1 plate's
     IEEE-754 derivation was wrong; 0.875 = 0x3f600000, 1.75 = 0x3fe00000).
  2. **00412620**: `_DAT_005cc568` (100.0) and `_DAT_005cd038` (-100.0) are **ray
     segment-extension scalars** (build a long swept segment for the FUN_00474e70
     visibility test), NOT blend alphas as the C1 plate guessed.
  3. **00413bc0**: `_DAT_005cd060` = **0.25** confirmed by memory read (0x3e800000),
     resolving the C1 plate's "likely 0.25 but unverified".
  4. **00418db0**: record count is exactly **4** (span 0x190 / stride 0x64); cleans up the
     C1 plate's "6…4 records" arithmetic confusion. `&DAT_0063bde0` = `&DAT_0063bda0`+0x40
     (same 4-record array, walking the embedded object ptr at record+0x40).

- **Live constants read (Mashed_pool3)** — decoded IEEE-754 LE, now in the plates:
  0x005cc320=1.0, 0x005cc32c=0.5, 0x005cc354=12.0, 0x005cc358=5.0, 0x005cc35c=4.0,
  0x005cc558=0.001, 0x005cc55c=10.0, 0x005cc564=0.25, 0x005cc568=100.0, 0x005cc574=2.0,
  0x005cc99c=0.3, 0x005cc9a0=0.05, 0x005cc9a4=0.025, 0x005cc9bc=0.8, 0x005cd02c=0.13,
  0x005cd030=0.0, 0x005cd038=-100.0, 0x005cd03c=0.0001, 0x005cd060=0.25, 0x005cd074=1.25,
  0x005cd078≈π/4, 0x005cd07c=π/4, 0x005cd080=0.62832, 0x005cd084=0.06, 0x005cd100=57.2958
  (180/π), 0x005cd104=-12.0, 0x005d757c=0.0 (sentinel). 0x007f100c/0x007f101c are runtime
  BSS accumulators (0.0 at static rest).

- **Uncertainties carried** (all data-semantic / decompiler-artifact, NON-BLOCKING for C2 of
  these control-flow-complete reads; marked bare `[UNCERTAIN]` in the plates, NOT minted —
  central re-classify owns U-numbering from the reserved U-7700..U-7999 range): vtable-+0x48
  method bodies (00412100/00418db0); DAT_007f0fd0 mode codes + FUN_0042f6a0 codes
  (00412130/00413fa0); 00412190 slot-record field layout + uninitialized `fStack_8` term;
  FUN_0047ce00 state code 8 (00412620); particle out-record offsets 5/6/7/8 (00412e30);
  (0x37,0x14) units (00413b80); icon color-byte channel order (004128f0); sparkle 3-dword
  cell fields + *(cam+0x68) (00413cb0); RW render-state ids 6/8 (00413f50); FUN_00430790
  semantic (00413fa0); player-record +0x00 field (00416230); FUN_004a2c48 return range
  (00417370/004173b0); decal-grid byte payload semantics (004173e0); aggregate stat meaning
  (00417750/004173a0); FUN_00465e80 id-18/20 registration roles (00418aa0); 0xff010101 cell
  sentinel (00418bd0); FUN_00418de0 body (00418e30). **needs-attention** (no direct caller):
  004173b0 (function_callers = 0; indirect/vtable entry not located).

- **Stubs noted** (NOT minted — sweep owns S-numbering from reserved S-6200..S-6399).
  Recurring shared helpers across this cluster: FUN_004a2c48 (RNG byte), FUN_0042f6a0
  (game-mode getter), FUN_004b6520 (zero-fill). RW/transform helpers: FUN_004c1340/
  FUN_004c13e0/FUN_004c1480/FUN_004c39b0 (Vec3Normalize)/FUN_004c45f0/FUN_004c51a0/
  FUN_004b42c0/FUN_004b4430/FUN_004c3d90; raycast pair FUN_0045bfe0/FUN_004b4cd0 +
  FUN_004b4650/FUN_004b5080; emitter handle FUN_0047d080/FUN_0047d100/FUN_0047d180;
  sprite-batch FUN_004770c0/FUN_004770a0/FUN_00476cb0/FUN_004768c0/FUN_004769a0/
  FUN_004769d0/FUN_004769f0/FUN_00476a30/FUN_00476a40/FUN_00476d00/FUN_00476df0;
  camera FUN_004671a0/FUN_00467210/FUN_0046d4a0; FUN_00465e80 (subsystem register);
  FUN_005592f0 (draw). Named/known: Vec3Magnitude@0x004c3ac0 = C3; FUN_0040bb30 (texture
  by name). FUN_00418de0 (drift-mark per-record handler; outside this slice).

- **No new U-IDs / S-IDs / arg_types minted. No Frida, no build, no re-classify, no hooks.csv
  write.** Per author-only mission: only the bucket dir, this fragment, and
  `.pool_slot_al_s4` were created/modified.
