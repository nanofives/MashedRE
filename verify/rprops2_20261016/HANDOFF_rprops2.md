# HANDOFF rprops2 (stopped 2026-10-17, user decision: do not resume)

Branch `work/rprops2`, worktree `.worktrees/rprops2`, base `04dee0ed` (178 commits behind the tip
`18ddb6ea2`). Nothing here is scored. No tracker was edited on this branch. Commits:
`0fdae99e` (PREREG, harness copy, base4 baseline) and `a56315b7c` (WIP code, default OFF).

## 1. Baseline measured at 04dee0ed (STALE, do not reuse)

Arm `base4`, d1r1 matched-pose recorder + `drawlist_diff.py` 3D mode (`scores_base4.txt`):

| track | matched draws | rows | RED |
|---|---|---|---|
| TRAINING 884 / 924 / 959 | 198 / 195 / 195 | 77 / 76 / 76 | 0 |
| Arctic 1051 / 1091 / 1126 | 145 / 144 / 105 | 78 / 77 / 77 | 0 |
| City 761 / 801 / 836 | 474 / 462 / 415 | 126 / 124 / 122 | 0 |

Rows equal `verify/rfx_20261016/RESULT_rfx.md` section 9 "rb3, default" exactly. G-CTRL on that build
`716cb2fd / 2a2afda1 / ba90ed88` (18,664 / 56,153 / 28,076 lines). Both are superseded by the tip
(rstates, rfx2, d2bsp, rres merged; the orchestrator gives about 870 rows in all now).

## 2. U-9699 camera-man yaw

Direct witness: the original draw record's `w` matrix, yaw = atan2(w[2], w[0]) (`camman_yaw.py`).

| frame | original yaw | base4 (table yaw) | i1 (step every frame) |
|---|---|---|---|
| TRAINING 884 | 4.398 | 0.000 (-4.398) | 4.972 (+0.574) |
| TRAINING 924 | 4.398 | 0.000 (-4.398) | 6.552 (+2.155) |
| TRAINING 959 | 4.398 | 0.000 (-4.398) | 9.143 (+4.746) |
| Arctic 1051 | -177.085 | -178.820 (-1.735) | -177.047 (+0.038) |

Arctic 1091 / 1126 record no camera-man. Translation exact everywhere (d_pos 0). The original holds
4.398 deg constant over three state-6 rows, so it does not step type-1 entries while racing; the i1 arm
stepped every frame and drifted. `camman_yaw_i1.txt`.

Code in `a56315b7c` (built in the i1 form; the i2 gate below was NEVER built or run):
- `Race/CameraEntries.cpp` `CamEntryType1Step441d40` (0x00441d40, 1730 B, instruction-order x87
  transcription from `disasm_range.py 0x00441d40 0x00442402`) and `CamEntryType1StepWith` (target =
  midpoint of two given car positions). Constants read from the image: 0x005cc948 = 1/3000, 0x005cc9fc =
  1000, 0x005ccacc = 45, 0x005cc9b8 = 7, 0x005cc9e0 = 25, 0x005cc750 = 16, 0x005cd9fc = 7.5, 0x005cc35c = 4,
  imm 100.0 at 0x004421d8, head offset (0, 0.1741, 0.2713) at 0x004423b7 / 0x004423bf. `CamCarMidpoint441c80`
  tail refactored into `MidArith` (same arithmetic). New `CamEntry::Row`, `CamEntryHost::path_progress`.
- `TrackRenderer` `CamManStep` / `UpdateCamMen`, private entries built by `CamEntryBuild441b30`;
  stepping gated to pre-race frames (`countdown_ > 0`, or the replay row's DAT_0063ba8c in 3..5) = i2.
- Opt-in knob `MASHED_CAMMANYAW=1`. Default build: camera-men keep the rprops table yaw.
- All 12 camera-man tracks' rows 1..N are type 1 (`dump_camtypes.py`).
- `Util/CameraEntryDispatch.cpp`: comment only (type 1 still not dispatched there, deliberately).

## 3. U-9700 ship tick gate (decoded, no code)

`0x0041e9e0` = `return *(DAT_0063d7e4 + 0x18)`. `DAT_0063d7e4` has two writers, both in `FUN_0041e870`
(`0x0041e878` clear, `0x0041e895` store): it scans the hook table `0x005f33f8` (stride 0x48, count
`0x005f37a0`) for the record whose `+0x10` equals its argument. `+0x14` = slot 0, `+0x18` = slot 1.
`0x0041e8c0` = `jmp [DAT_0063d7e4 + 0x18]`; `0x00426ab0` calls it when the pointer is non-null.
`0x0041e9b0(id)` returns `*(DAT_0063d7e4 + 0x10) == id` and the result is discarded. **No frame counter,
no two-update gate in the exe.** `0x00448cb0` poses BEFORE it advances, so call K poses with K-1 advances.
The port's "skip the first two Render ticks" (`UpdatePropAnims`) is an offset between the standalone's
Render count and the replay row index, not exe behaviour.

## 4. U-9701 sprite passes (decoded, no code)

- `SkySecondaryDispatch` `0x00489240` = `(*(DAT_007030ac + 0x48))(DAT_007030ac)`. `DAT_007030ac` has one
  writer, `0x004880e2` in `FUN_004880a0`: a mesh from `FUN_00534b60(0x2000, 0x10000087, 0)` (-> `FUN_00534d00`),
  src blend 5 / dst blend 2, `+4 = 0x200`. `+0x48` is the RpAtomic render callback set by `FUN_00534d00`
  (`FUN_00535690` -> stream callbacks -> `FUN_00535cb0` DrawIndexedPrimitive TRIANGLELIST, 4n verts / 2n prims).
  `FUN_004883b0` refills it every frame from the 0x38-stride pool at `0x0086ae4c` (16 verts per live entry,
  streams 1 / 4 / 2 / 0x80). `FUN_004862d0` initialises the pool family (`exp_cloud2`, `scorch`).
  Callers of 0x00489240: `0x00449393` (Arctic slot 4 `0x004492b0`) and `0x0044b90a` (City slot 2 `0x0044b8f0`).
- City oak leaves: `0x0044b3c0` (slot 0; it is inside a containing function, see decomp) calls
  `FUN_0047c330` (mesh `DAT_006c301c = FUN_00534b60(0x800, 0x87, 0)`, texture "oaktreesprite" via
  `FUN_004c5c00`, size 0.1 / 0.1) then `FUN_0047c560` per OakLeaf.mts record (0x18-byte sprite records into
  the table `DAT_0086ecc4`). Drawn by `FUN_0047cd30` (`(*(DAT_006c301c + 0x48))`, guarded by `_DAT_006c3014`),
  vertices written by `FUN_0047c670` (called from `0x0047cc40`). Callers of 0x0047cd30: 0x0044af70,
  0x0044b9a9, 0x00450a46, 0x00452de9. Original records draw it TRIANGLELIST 432 verts / 216 prims, twice
  per City frame, world = identity, texture 128x128.
- Not decoded: `0x0047cc40`'s caller chain and the per-vertex colour/UV law of `FUN_0047c670` beyond the
  decomp in the session (fixed colour 0xC0C0C0, alpha 0xFF; UV from the record +0x10/+0x14).

## 5. U-9696 remainder

Not started (the Arctic 22-triangle `World` sector mesh's unnamed 256x256 A8R8G8B8 texture).

## 6. Tracker notes to apply on the tip (orchestrator applies; NOT edited here)

- **U-9699** append: "rprops2 (WIP, unscored, `work/rprops2` a56315b7c): 0x00441d40 ported as
  `CamEntryType1Step441d40` behind opt-in `MASHED_CAMMANYAW=1`. Witness (draw `w` yaw): TRAINING 4.398 deg
  constant over rows 884/924/959 (state 6), Arctic 1051 -177.085. Stepping every frame drifts TRAINING
  +0.57/+2.16/+4.75 deg while Arctic 1051 lands at +0.038; the original does not step type-1 entries in
  state 6. Next: score the pre-race-only gate (written, never built). HANDOFF_rprops2.md section 2."
- **U-9700** append / resolve the semantic question: "0x0041e9e0 = *(DAT_0063d7e4+0x18), the hook record's
  slot 1; DAT_0063d7e4 set only by FUN_0041e870 (0x0041e878/0x0041e895, Course_Id scan of 0x005f33f8);
  0x0041e9b0's result is discarded. No frame gate exists; 0x00448cb0 poses before advancing (K-1 advances
  at call K). The port's 2-tick skip is a Render-count vs replay-row offset, to be replaced by ticking at
  the course-update analogue. HANDOFF_rprops2.md section 3."
- **U-9701** append: "[DAT_007030ac+0x48] = RpAtomic render callback of an additive sprite mesh
  (FUN_004880a0 @0x004880e2 creates it via FUN_00534b60/FUN_00534d00; FUN_004883b0 refills it per frame
  from pool 0x0086ae4c; init FUN_004862d0). City oak leaves: same mesh class, FUN_0047c330 (create,
  'oaktreesprite') / FUN_0047c560 (per-OakLeaf.mts record, 0x18 B) / FUN_0047cd30 (draw) / FUN_0047c670
  (vertex build, from 0x0047cc40). HANDOFF_rprops2.md section 4."

## 7. Next steps for a fresh lane from 18ddb6ea2

1. Re-measure the base on the tip first (nine frames + G-CTRL), never reuse section 1.
2. Cherry-pick or re-apply `a56315b7c`'s CameraEntries.cpp port (no conflict expected there; TrackRenderer
   hunks will need hand-merging), build, score the pre-race gate against the section 2 witness.
3. U-9700: move the ship tick to the course-update analogue and delete the skip constant; score the ships.
4. U-9701: port the leaf-sprite mesh (FUN_0047c330/560/670/cd30) as a librw pass; the City oaktreesprite
   rows are the claim. Then the 0x0086ae4c pool pass.
5. U-9696: dump the material of the 22 Arctic triangles (bb [-31.00, 0.56, 34.19, -27.63, 0.62, 36.06]).

Harness: `verify/rprops2_20261016/*.py` (private game root under this session's scratchpad, which may be
gone; re-copy original\ per memory `temp-scratchpad-files-can-vanish`).
