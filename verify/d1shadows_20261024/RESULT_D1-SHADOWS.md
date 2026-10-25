# RESULT D1-SHADOWS: the missing road shadows are already ported (stale capture), no port needed

**Verdict: GREEN, H1 holds** (`PREREG_D1-SHADOWS.md`, committed alone as `d7b2aef7d` before any arm). The pole and
gantry shadows across the TRAINING road are the second texture stage of the road materials (RenderWare MatFX DUAL,
texture `newShadow`). The port has drawn that stage by default since rtex F-DUAL (`21cd1eeeb`, 2026-10-16 05:01).
The standalone half of `verify/d1r2_stills_20261024/sbs_row_00924.jpg` was captured on exe `b71e597a`
(2026-10-16 02:22), before F-DUAL landed. On the current default build the shadows render and match the original.
No source change, no knob, no default touched.

## What the original draws (static)
- **Assets** (`matfx_training_road.txt`, scanner `matfx_dual_scan.py` / `dff_atomics.py` over `training.piz`):
  `ROAD.DFF` has 62 MatFX extensions (chunk `0x120`), all type 4 DUAL, srcBlend 5, dstBlend 6, dual texture
  `newShadow` (44) or `newShadow_Small` (18), on the CliffEdge / Dirt / RoadTarmac / Wall materials of all 21 road
  atomics. Every geometry has format `0x2008b` (two UV sets). `GRAPH.BSP` has one DUAL material (`newShadow_Small`).
  `TEXTURES.TXD` holds `NewShadow` and `NewShadow_Small`.
- **Render path** (Ghidra, `Mashed_pool10`, read-only, `decomp_00543500.txt`): `FUN_00543500` reads the dword at
  `+0x30` of the material's MatFX data (`puVar3[0xc]`) per mesh. Case 4 calls `FUN_005405c0(.., *puVar3)`, case 6
  `FUN_005405c0(.., puVar3[6])`, case 2 `FUN_005412d0`, cases 1/3 `FUN_00541d40`, default `FUN_00540340`.
  The DUAL stage program in `FUN_005405c0` (`0x005408f9..0x005409af`) is cited in
  `verify/rtex_20261016/RESULT_rtex.md:22-26`.
- No separate shadow pass draws these. The dynamic-object shadows (`RWObjShad`, `FUN_00481a30`) are a different
  effect, already ported default ON (U-9705 / U-9710).

## What the port runs
`LibRw/RwSceneBuild.cpp:40-63` (`DualEnabled`, `IsDualMaterial`), default ON, opt-out `MASHED_LIBRW_DUAL=0`.

## Arms (standalone only, private root `D:\MashedWT\D1-SHADOWS_root`, both under `mashed_lock.py`)
Exe: this branch at `364c43d69` source (no `mashedmod/` change, `git diff 364c43d69 -- mashedmod` empty), built with
`MASHED_NO_DEPLOY=1`, sha256 prefix `F49E53F0`. TRAINING row-indexed camera replay of
`verify/d1r2_20261016/orig2_trace_v3.csv`, shots 884 / 924 / 959 (`run_replay.py`, a copy of d1r2's with `--root`).
Original frames: `log/d1r2/orig2/orig_row_*.bmp` (the left half of the d1r2 stills). Logs: `arm_DEF.txt`,
`arm_DUAL0.txt` (both exit 0, 3000 dump lines, 3 shots).

| row | arm | M1 ROI r vs orig | M2 ROI mean (orig) | mean-abs full frame |
|---|---|---|---|---|
| 884 | DEF | 0.8742 | 88.37 (83.57) | 12.27 |
| 884 | DUAL0 | 0.4723 | 119.22 (83.57) | 27.57 |
| **924** | **DEF** | **0.9964** | **102.14 (101.73)** | **8.81** |
| 924 | DUAL0 | 0.4369 | 122.80 (101.73) | 21.79 |
| 959 | DEF | 0.9125 | 109.90 (109.06) | 7.34 |
| 959 | DUAL0 | 0.6144 | 123.99 (109.06) | 15.41 |

ROI x 290..459, y 150..299 (`roi_score.py`, `scores_roi.txt`). Full-frame mean-abs from `score_d1r2.py frame`
(`scores_gframe_*.txt`). That script prints `FAIL` because its d1r2 gate also needs K-60 / K+60 control shots,
which this step did not take. Only the mean-abs values are used here, as pre-registered.

**Pass rule at row 924, all three hold:** M1 0.9964 > 0.4369, M2 102.14 < 122.80, mean-abs 8.81 < 21.79. Rows 884
and 959 go the same way (reported, not gated). DUAL0 reproduces the old still: 21.79 here against 21.94 in
`verify/d1r2_stills_20261024/RESULT_d1r2_stills.md` for row 924, which is consistent with that capture having no
DUAL stage. Side-by-sides (original | DEF | DUAL0): `sbs_row_00884_DEF_DUAL0.jpg`, `sbs_row_00924_DEF_DUAL0.jpg`,
`sbs_row_00959_DEF_DUAL0.jpg`. In 924 the vertical pole shadow and the crossbar shadow are in the DEF panel at the
original's pixels and absent from DUAL0.

Remaining full-frame difference at 924 (8.81) is cars, HUD and the car-driven effects (the replay moves only the
camera, the standalone race runs underneath), as in d1r2. Not a shadow item.

## Gates
Not run: no source change on this branch (`mashedmod/` identical to `orch/integration` `364c43d69`), so the default
build is the integration build. Nothing to protect.

## Decision
None. No default to flip, no knob added.

## Next
- D1-SHADOWS can close. The stills in `verify/d1r2_stills_20261024/` are pre-F-DUAL for the standalone half. If
  Mariano wants a current set for the record, `sbs_row_*_DEF_DUAL0.jpg` here is one (middle panel = default build).
- Not done: original-side re-capture (the existing d1r2 original BMPs were reused, same camera rows).
