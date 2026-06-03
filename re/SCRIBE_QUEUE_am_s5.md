# SCRIBE_QUEUE fragment — batch_am session 5 (am_s5)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep) writes hooks.csv / trackers and commits.

## Queued

2026-06-03  am_s5  bucket=re/analysis/bucket_gameplay_0044e190_00453eb0  confidence=C1->C2  rvas=0044e190,0044e240,0044e2d0,0044e310,0044e680,0044ecc0,0044f830,00450300,00451060,00451730,00451820,00451990,00451ad0,00451cc0,00451ce0,00452140,004523f0,00452ec0,00452f00,00452f30,00453100,00453210,004532f0,00453730,004538b0,00453eb0

## Notes for the sweep

- **Count**: 26 RVAs, 26 plates authored in the bucket dir. **None drift-skipped**
  — all 26 are their OWN first-field `gameplay,C1` rows in hooks.csv at session
  start (verified). NB a naive `grep RVA hooks.csv` fuzzy-matches two C2 rows that
  merely *mention* these RVAs in their notes column (00485460 `smplfzx` cites
  `FUN_00451730`; 0045c350 `Bezier::Interpolate` cites `FUN_00452140`) — those are
  different functions, not drift. The real rows are hooks.csv lines 4091
  (00451730) and 4097 (00452140), both `gameplay,C1`.
- **Pool slot ACTUAL**: pre-assigned **Mashed_pool8** was free (no `.lock`),
  opened read-only, all 26 decompiled, `program_close`d cleanly. Recorded in
  `.pool_slot_am_s5` (untracked session marker). No lock poisoning this session.

- **Subsystem confirmation**: all 26 CONFIRMED `gameplay` (no reclassifications).
  The slice is a cohesive **projectile/depth-charge-effect + portal-trigger +
  scenery-sub-object cluster**, not a flat "early game-state" range. Sub-groups:
  - **Scenery sub-object builders** (0044e190, 0044e240, 0044e2d0, 0044e310):
    fastcall/register-passed (`unaff_EBX`/`in_EAX`/`unaff_ESI`) helpers that build
    child RW frames for a parent record using a count table `DAT_008952b0` and an
    RwV3d position table `DAT_00895560`; parent frame slots at +0xa0/+0xa4, child
    array at +0xc (6-dword stride). 0044e240 = 10× `FUN_00557ec0` + finalizer.
  - **Scenery grid init** (0044e680): fills the `0x00890080` record array (stride
    0xf8) from an 8×10 float column-X grid (10..240) + texture table `DAT_00894e00`
    + 3-dword table `DAT_00894da0`; per-row base Z `_DAT_005ce2e0/_e4`.
  - **3×10 effect-array init/reset** (0044f830 init, 0044ecc0 reset): array near
    `DAT_0089560c` (outer 0x46 dwords, inner 0x1c), 800-dword handle→slot back-index
    at `DAT_00894e20`, per-rank seed bases 2000/0x834/0x898.
  - **Waypoint/path globals init** (00450300): six Y=4.930f RwV3d waypoints at
    `0x00684bbc`/`0x00684c1c` + two (-1,-1,0) dir vecs + 4 channel bindings.
  - **Sky/camera frame set** (00451060): GetCamera(0)→FUN_00467210→FUN_004c1480
    (frame matrix-set; the pre-existing "SkyClearColorSet" name conflicts with the
    body — see plate).
  - **Portal/AABB trigger volume** (00451730, 00451820, 00451990, 00451ad0,
    00451cc0, 00451ce0, 00452140, 004523f0): per-entity AABB-inside test in the
    portal's local frame (bounds at +0x14/+0x1c/+0x20/+0x28, matrix at +0x94,
    occupied flag at +0x48), movement-vector computation (00451ce0), exit 4-corner
    sub-sample with `printf("movement is (%f, %f, %f)\n")` debug (00451ad0),
    deactivate (00451cc0), singleton wrapper over `DAT_00684d18` (00452140), and a
    clump-list destructor (004523f0).
  - **Two-pool reset** (00452ec0, 00452f00, 00452f30): pools at `0x00684e38`
    (stride 0x1c, 5 slots) and `0x00684ea8` (stride 0x110, 32 slots); per-slot
    reset detaches an RW frame.
  - **Depth-charge/missile effects** (00453100 trail-strip, 00453210 ground-ring,
    004532f0 particle-dispatch, 00453730 ray-impact flash, 004538b0 per-frame
    homing+bob integrator, 00453eb0 generic effect-ring registrar).

- **RW resolutions since the 2026-05-19 C1 pass** (master has named these callees;
  recorded in the new plates — sweep may confirm/propagate the names):
  - **004523f0**: C1 `FUN_004e6e00` (STUB) → now **`RpClumpDestroy`**.
  - **00452ec0** and **00452f00**: C1 `FUN_004e45b0` (STUB) → now
    **`RwFrameRemoveChild`** (with `FUN_004e4800` as its frame-lookup-by-id
    predecessor — the "lookup-and-release" idiom is concretely a frame detach).

- **C1 correction (004538b0 / 00453eb0 family)**:
  - **00453eb0** ring-entry field order: C1 plate said "+1=param_4, +2=param_3";
    the current decomp writes `+0x04=param_3`, `+0x08=param_4`. Corrected in the
    new plate. Also: the `if (&DAT_006870a8 + iVar1*9 != NULL)` guard is
    always-true (address-of-static) — vestigial.

- **C3 citation**: **004538b0** calls the mapped C3 `Vec3Magnitude`
  (`FUN_004c3ac0`) for homing-vector length, paired with its normalize companion
  `FUN_004c39b0` (still C1/unmapped, RwV3dNormalize candidate).

- **Uncertainties**: filed as **bare `[UNCERTAIN]`** in the plates (NOT minted to
  U-IDs) per author-only mission. Recurring open items for central minting:
  - register-only calling conventions on the scenery builders (0044e190/2d0/310,
    00453730/004532f0/004538b0 use `in_EAX`/`unaff_ESI`/`unaff_EDI` — signatures
    not recovered).
  - effect-record dispatcher / hook cluster: `FUN_00484cf0`, `FUN_00465940`,
    `FUN_00487020`, `FUN_00474de0`/`FUN_00474d60`/`FUN_00476860`,
    `FUN_00477760`/`FUN_00486610`.
  - collision/raycast triple: `FUN_0045bfe0` + `FUN_004b4cd0` + `FUN_0045c350`
    (and `FUN_004b4650`/`FUN_004b5080`).
  - portal helpers: `FUN_0040e340` (entity count), `FUN_0046d4a0` (entity matrix),
    `FUN_004c4dc0`/`FUN_004c52f0` (matrix copy / mode-2 inverse transform),
    `FUN_0046be10` (event emit), `FUN_0046bf50`/`FUN_0041f1e0`/`FUN_00481750`/
    `FUN_00480720`, `FUN_00550bc0` (handle→frame).
  - scenery/frame APIs: `FUN_004e69a0`, `FUN_004c0b30`, `FUN_004e7e30`,
    `FUN_004c1040`, `FUN_004c1340`, `FUN_004c1480`, `FUN_004c51a0`, `FUN_004e5fc0`,
    `FUN_004b6520`, `FUN_004b3f90`, `FUN_0047fad0`, `FUN_0047d130`/`FUN_0047d180`,
    `FUN_0057c210`, `FUN_0055ae70`, `FUN_004840d0`, `FUN_00467210`, `FUN_004c1740`,
    `FUN_004a2c48` (RNG), `FUN_00472520`/`FUN_00472560`. All are callees;
    non-blocking for C2 of the bit-identical leaves.

- **needs_function_create**: none (all 26 have function objects; `function_at` +
  decomp succeeded for all).
- **library_skip**: none — every bucket member is a game function. The only
  CRT/RW-named callees touched are `RpClumpDestroy`, `RwFrameRemoveChild`, and the
  `FID_conflict__wprintf` debug print in 00451ad0.
- **Stubs**: none minted this session (all unmapped callees recorded as bare
  `[STUB]` in the plates).
- Files: bucket dir + this fragment committed atomically (path-scoped).
  `.pool_slot_am_s5` left untracked (session marker).
