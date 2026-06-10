# SCRIBE_QUEUE fragment — batch_an session 1 (an_s1)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep) writes hooks.csv / trackers and commits.

## Queued

2026-06-03  an_s1  bucket=re/analysis/bucket_gameplay_00456040_004588c0  confidence=C1->C2  rvas=00456040,00456140,004568a0,004568d0,00456c30,00456c70,00456c90,00456ce0,00456d00,00456eb0,00456f70,004570a0,00457100,00457610,00457650,004576b0,004577f0,00457ba0,00457c10,00458020,00458080,00458360,004584e0,00458570,00458880,004588c0

## Notes for the sweep

- **Count**: 26 RVAs, 26 plates authored in the bucket dir. None drift-skipped
  (all were `gameplay C1` in hooks.csv at session start; none already C2+).
- **Pool slot (IMPORTANT)**: assigned `Mashed_pool1` was LOCKED by a live sibling
  session (lock ts 05:14:46 UTC, ~40 s after this session began), and `Mashed_pool2`
  was also locked (05:15:41 UTC). Per multi-session etiquette I did NOT delete an
  active lock; fell back to a verified-free, UNassigned clone **`Mashed_pool9`**
  (no `.lock`/`.lock~`), opened read-only, `program_close`d at end. `opened_in_slot:
  Mashed_pool9` in every plate frontmatter; recorded in `.pool_slot_an_s1`.
- **Subsystem confirmation**: all 26 CONFIRMED `gameplay` (no reclassifications).
  The whole bucket is the **weapon/effect entity-pool family** (depthcharge / mine /
  debris / decal / spark), specifically pools G/H/I/J:
  - Pool G: slot record base `DAT_006886b0` (stride 0x48, 4 slots) + RW-handle table
    `DAT_006886b8` + shared sprite list `DAT_006887d0` — alloc 0x00456040, teardown
    0x004568a0/0x00456c30/0x00456c70(+thunk 0x00456c90), per-frame lock/track
    indicator 0x004568d0.
  - Pool H: base `DAT_00688810` (stride 0x68, **64 entries** — (0x68a210-0x688810)/0x68;
    C1's "~50" was loose), bitmap `DAT_0068a300`, lists `DAT_0068a2c0`/`DAT_0068a210`
    — spawn 0x00456eb0 (+wrapper 0x004577f0), per-frame update/draw 0x00457100, cull
    0x00457650, list-release 0x00457610, active-predicate 0x004576b0.
  - Pool I/J: J entries `DAT_0068a310` (stride 0x6c, 32), pair-headers `DAT_0068b0d0`
    (stride 0x18, 4), pool-I `DAT_0068b198` (stride 0x50, 25), list `DAT_0068b090` —
    reset 0x00457ba0/0x00458020, state-machine 0x00458080, teardown 0x00458360/
    0x00458880, decal-raycast 0x00457c10, sub-burst 0x004588c0, dir-helpers
    0x004584e0/0x00458570.
  Draw-side helpers (screen-sprite family 0x004769xx/0x00476xxx, RW matrix helpers
  0x004c1xxx/0x004c5xxx) are render-ADJACENT but the bucket logic (spawn/lifetime/
  physics-raycast/state-machine/teardown) is gameplay — kept `gameplay`, no split.
- **C2 corrections vs the C1 plates** (worth the sweep's attention):
  1. `FUN_004e6e00` resolves to **`RpClumpDestroy`** (RenderWare) — C1 guessed
     "register-resource". Affects 0x004568a0, 0x00458880. Both are DESTROY/teardown
     passes; the 0x004568a0 C1 title "initialisation" was WRONG (it destroys clumps).
  2. `FUN_004e45b0` resolves to **`RwFrameRemoveChild`** — C1 "handle release pair".
     Affects 0x00457ba0, 0x00458020, 0x00458080 (paired with `FUN_004e4800` =
     frame-parent accessor). Clean RW scene-graph detach.
  3. 0x004568d0 colour mapping: `FUN_0045a0f0(slot)==-1` selects the **BLUE**
     (searching) quad, not red (C1 implied red for -1).
  4. 0x00457c10 decal record scale `[0x13] = 0x3e000000 = **0.125f**` (C1 mechanical
     text said 0.25f).
  5. 0x00458570 degenerate-vector return = **0.0**, NOT `DAT_005d757c` — this DIFFERS
     from the sibling 0x004584e0 (which returns the `DAT_005d757c` epsilon). C1 plate
     was wrong on 0x00458570.
  6. 0x00458080 state 3 (sustained) ALSO dispatches a **type-13** effect record via
     `FUN_00484cf0` each frame (callback `LAB_00457e10`), not just a re-draw.
- **library_skip**: none. No in-bucket RVA is a third-party library family. (RW API
  calls RpClumpDestroy / RwFrameAddChild / RwFrameRemoveChild appear as named callees
  only.)
- **needs_function_create**: none. All 26 had function objects; 0x00456c90 is a
  genuine `thunk:true` (verified via function_at) → FUN_00456c70.
- **Thunks / wrappers**: 0x00456c90 = thunk → 0x00456c70 (caller FUN_0045b9d0);
  0x004577f0 = thin forwarder → 0x00456eb0 (single forwarding call, not a true thunk
  object but behaviourally identical).
- **No-caller (indirect / fn-ptr / vtable) functions** (count=0 in graph, noted in
  plates, NON-BLOCKING): 0x00456040, 0x00456ce0, 0x00456d00, 0x00456f70, 0x00457100,
  0x00457650, 0x00457c10, 0x004577f0, 0x00458080, 0x00458360, 0x00458570, 0x004588c0.
- **Uncertainties**: bare `[UNCERTAIN]` markers in plates only — NONE minted to U-IDs
  (per author-only mission). Mostly effect-identity (which weapon) + a few flag-bit/
  render-state-id meanings; all data-semantic, NON-BLOCKING for C2 of these
  bit-identical leaves.
- **Stubs**: noted per-plate, NONE minted to S-IDs. Recurring unmapped callees across
  the bucket: screen-sprite family (FUN_004769a0/004769d0/004769f0/00476a10/00476a30/
  00476a80/00476d00/00476df0/004768c0/004770a0), RW matrix/frame helpers
  (FUN_00474d60/00474d80/00474de0/004c0ed0/004c1480/004c1520/004c3d90/004c4dc0/
  004c4d20/004c5010/004c51a0/004c5930), RNG (FUN_00472650/004a2c48), effect dispatch
  (FUN_00484cf0), physics/raycast (FUN_0045bfe0/004b4cd0/004b4650/004b5080/004b6520/
  0044d5e0), and adjacent gameplay (FUN_004595c0/00455fe0/0045a0f0/0045a110/00459620/
  0046d4a0/0046d660/0046d740/0046be50/00458630/00490020). `FUN_004c3ac0` = Vec3Magnitude
  (landed hook) — used by 0x00456f70, 0x004584e0, 0x00458570.
- Files left UNTRACKED until the atomic commit below: bucket dir + this fragment +
  .pool_slot_an_s1. No re-classify, no build, no Frida — author-only.

> DRAINED by sweep-20260603-1259 — 26 plates + 26 bookmarks to master Ghidra, 0 renames, 0 errors. C1->C2 finalize via re-classify follows.
