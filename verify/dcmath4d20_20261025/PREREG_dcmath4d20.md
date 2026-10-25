# PREREG dcmath4d20: one body for 0x004c4d20 (D4-DC-MATH-4d20, W0-9)

Committed alone, before any arm. Branch `orch/D4-DC-MATH-4d20`, code commit `8d6513d39` on base `aed0e1bc7`.
Exe sha256 `ff990ccf9537e254a53c3ab33ee71c3d0fdf8f1deae5a27e6b61a45cbf7c6709` (`MASHED_NO_DEPLOY=1`).

## Change under test
- `Race/RaceCamera.cpp`: `MASHED_CAM_ROTATE_RW=1` (new, default OFF) makes `RotateAboutAxis` call the shared
  `RwMatrixRotate(m, axis, deg, 0)` (`Math/RwMatrixRotate.cpp`, 0x004c4d20) and then
  `Math::RwV3dTransformVectorsCPU(v, v, 1, m)` (the slot +0x14 stand-in). The axis goes in unnormalized, as the original
  passes it. With the knob OFF the old fused std::cos / std::sin Rodrigues runs, now with no RVA header (port-only).
- `re/tools/dual_copy_allowlist.txt`: key `DUP-IN-TARGET:004c4d20` removed. rva-lint: 1 finding (`004177b0`), NEW=0.
- `verify/savelink_20261023/run_arms.py`: stack `CR` = `MASHED_CAM_ROTATE_RW`.

## Static facts recorded before the arms
- **S1, ROTATE_LUT and ROTATE_PC24 are already ON in the exe default.** The row says "default OFF". That is stale:
  `Core/FaithDefaults.cpp:30-31` seeds `ROTATE_LUT=1` and `ROTATE_PC24=1` (faithflip 2026-10-20, opt-outs
  `MASHED_NO_ROTATE_LUT` / `MASHED_NO_ROTATE_PC24`). So the shared body already runs the LUT inverse sqrt and the PC_24
  scope for every caller it has. The remaining split is the camera's second body, which this lane puts behind a knob.
- **S2, the original's four camera sites are one pair each**: `FUN_004c4d20(m, axis, deg, 0)` then
  `FUN_004c3df0(dst, src, 1, m)` at `0x00441892`/`0x004418ab`, `0x004418cc`/`0x004418da`, `0x00441968`/`0x00441976`
  (all in `FUN_00441820`) and `0x00447557`/`0x00447570` (capstone over `MASHED.exe.unpatched`). `FUN_004c3df0` dispatches
  device slot +0x14, the VECTORS method (`Math/RwV3dTransformPointsCPU.cpp` D3-CONTACT note), so
  `RwV3dTransformVectorsCPU` is the stand-in, with the live slot's sum order `(x*right + y*up) + z*at`.
- **S3, reachability in mashed_re.exe** (`dumpbin /disasm` + `mashed_re.map`): `RotateAboutAxis` (`0x000E87F0`) is
  called from `RaceCamera::NodeDir` (`0x000E874E`, `0x000E87CF`) and `RaceCamera::Update` (`0x000E92C2`), and it calls
  `_RwMatrixRotate` (`0x000E885B`) and `RwV3dTransformVectorsCPU` (`0x000E8868`). Both are camera code. 18 other call
  sites reach `_RwMatrixRotate` (render, D2 vehicle code, camera entries, heading assist); this lane does not touch them.
- **S4, where the cost lands: the camera, not D2.** `RaceCamera.cpp` is exe-only and its outputs are the camera
  pose. dualcopy4 changed the same file's `Vec3Norm` and G-CTRL stayed identical across all its images
  (`verify/dualcopy4_20261016/RESULT_dualcopy4.md`). The witness is d1r2's row-indexed camera replay.
- **S5, a headerless inline 0x004c4d20 form that does feed D2** (not touched, reported): `Race/RoundGrid.cpp`
  `GridDir_PortYaw` computes `sin r` and `1 - cos r` in double for `BodyOrient_NoteExactBasis`, used by
  `BodyOrient_Init` under `MASHED_SPAWN_BASIS_RW` (seeded ON, `FaithDefaults.cpp:28`). The original builds those with
  `FUN_004c4d20(m, (0,1,0), deg, 1)` at `0x0047061c` / `0x0047062e`. The scanner does not see it (no RVA header).
  `D3d9Render/TrackRenderer.cpp:1918` holds constants only (`(0, -sin60, cos60)` literals), not a body.

## Predictions and gates
- **G1 (default)**: `H_DEF` = G-CTRL `ef5f2e10/57f7ff83/b4f4b55a`, `B_DEF_K0/K1/K26` = `c37d4c0c/0a8f9806/7013671d`
  (refs from `re/orchestrator/merge_train.json`). Pass = exact. Any miss = RED.
- **G2 (knob ON, no pass/fail on the hash, it feeds the decision)**: `H_CR`, `B_CR_K0/K1/K26`. Prediction: equal to the
  refs (S4). If they differ, the measured hashes are the expected flip hashes and S4 is refuted for G-CTRL / B.
- **G3 (D2, solo)**: `D2_DEF_1..3` and `D2_CR_1..3`, scored with `verify/d2rebase_20261019/score_d2_v2.py` (whole-log
  window, band.json). Each run needs >= 1600 motion_diag lines (re-run up to 2 times otherwise). Pass = all 6 in band.
- **G-CAM (the draw witness)**: d1r2's replay (`verify/d1r2_20261016/run_replay.py` copied here with the private root,
  `--trace verify/d1r2_20261016/orig2_trace_v3.csv`, 3000 frames), arms `CAM_DEF` and `CAM_CR`, each scored with
  `verify/d1r2_20261016/score_d1r2.py pose`.
  - G-CAM-RUN: `CAM_CR`'s `cam_dump.csv` differs from `CAM_DEF`'s. Identical dumps mean the replay does not execute the
    rotate sites, so the witness is blind: that is BLOCKED for the camera half, not GREEN.
  - G-CAM-POSE: `CAM_DEF` and `CAM_CR` both G-POSE PASS (d1r2's thresholds). Report pos / at / right median, p95, max
    for both. Prediction: `CAM_CR` is no worse than `CAM_DEF` on pos p95 and at p95. A worse CR is not RED for this
    lane (the default is OFF) but makes the recommendation B (keep OFF).
- **G-REP (determinism control)**: `CAM_DEF2`, a second `CAM_DEF` run. Pass = its `cam_dump.csv` sha equals `CAM_DEF`'s,
  so a CR difference is the knob and not run-to-run noise.

## Decision this feeds
`MASHED_CAM_ROTATE_RW` default ON (add `CAM_ROTATE_RW` to the FAITH set), or delete the port-only stand-in? Expected
G-CTRL / B with it ON = the G2 measurement. Filed as one decision row, not done here.

## Raw data
`D:/MashedWT/D4-DC-MATH-4d20_data/arms` and `.../cam` (not committed). The RESULT lists cited raw files with SHA-256.
Runs under `mashed_lock.py run --solo --label D4-DC-MATH-4d20`, private root `D:/MashedWT/D4-DC-MATH-4d20_root`.
