# PREREG D1-SHADOWS: the TRAINING road shadows missing from `sbs_row_00924.jpg`

Committed alone, before any arm. Branch `orch/D1-SHADOWS` off `orch/integration` `364c43d69`.

## The observation
`verify/d1r2_stills_20261024/sbs_row_00924.jpg` (TRAINING in-race, original left, standalone right): long pole and
gantry shadows across the road are present in the original half and absent in the standalone half.

## What the original draws (static, cited)
- **Assets** (`training.piz`, parsed this step): `ROAD.DFF` has 62 materials (CliffEdge / Dirt / RoadTarmac / Wall)
  carrying a MatFX extension (chunk `0x120`) of type 4 (DUAL), srcBlend 5, dstBlend 6, dual texture `newShadow`
  (44) or `newShadow_Small` (18). Every ROAD.DFF geometry has format `0x2008b` (two UV sets). `GRAPH.BSP` has one
  DUAL material (`newShadow_Small`). Matches `verify/rtex_20261016/evidence/matfx_census.txt:20,22`.
- **Render path** (Ghidra, `Mashed_pool10`, read-only, this step): `FUN_00543500` reads the MatFX data word at
  `+0x30` (`puVar3[0xc]`) per mesh and calls `FUN_005405c0(.., *puVar3)` on case 4 and `FUN_005405c0(.., puVar3[6])`
  on case 6, `FUN_005412d0` on 2, `FUN_00541d40` on 1/3, `FUN_00540340` otherwise. The DUAL stage program inside
  `FUN_005405c0` is cited in `verify/rtex_20261016/RESULT_rtex.md:22-26` (`0x005408f9..0x005409af`).
- So the shadows are the second texture stage of the road materials, not a separate shadow pass.

## What the port has
rtex F-DUAL (`21cd1eeeb`, 2026-10-16 05:01) renders MatFX DUAL on world sectors and prop DFFs, **default ON**,
opt-out `MASHED_LIBRW_DUAL=0` (`mashedmod/src/mashed_re/LibRw/RwSceneBuild.cpp:40-63`).

## Hypothesis H1 (the gap is a stale capture)
The standalone half of `sbs_row_00924.jpg` comes from `verify/d1r2_20261016/`, captured on exe `b71e597a`
(2026-10-16 02:22), before F-DUAL landed. The current default build already draws the shadows.

## Arms (standalone only, TRAINING, the d1r2 row-indexed camera replay)
Exe: this branch built with `MASHED_NO_DEPLOY=1` (no source change at PREREG time). Private root
`D:\MashedWT\D1-SHADOWS_root`. Trace `verify/d1r2_20261016/orig2_trace_v3.csv`, shots 884, 924, 959, the
`run_replay.py` recipe with `MASHED_ROOT` pointed at the private root. Original frames: the d1r2 BMPs
`log/d1r2/orig2/orig_row_{00884,00924,00959}.bmp` (the same pixels as the left half of the stills).
- **DEF**: default env.
- **DUAL0**: `MASHED_LIBRW_DUAL=0` (contrast, the pre-F-DUAL single-stage draw).

## Measures and pass rules (fixed now)
1. **ROI** = the road band right of the cars in the 640x480 frame, x 290..459, y 150..299 (holds the long vertical
   shadow and the crossbar shadow in the original at row 924, no car in it). Grayscale (ITU-R 601).
   - M1: Pearson correlation of orig vs standalone ROI pixels, per row.
   - M2: standalone ROI mean luminance, per row.
2. **G-FRAME**: `score_d1r2.py frame` mean-abs orig@K vs standalone@K, per row and arm.

H1 holds (D1-SHADOWS needs no port) when, at row 924: M1(DEF) > M1(DUAL0) **and** M2(DEF) < M2(DUAL0) **and**
G-FRAME(DEF) < G-FRAME(DUAL0), and the shadow streaks are visible in the DEF side-by-side. Rows 884 and 959 are
reported, not gated.

If H1 fails: locate why the default build does not draw the DUAL stage on these materials (texture resolve of
`newShadow`, the second UV set, or the stage program), and port the fix behind a new default-OFF knob with its own
PREREG addendum committed before its arms.

## Gates
No source change under H1, so the exe is `orch/integration` source: G-CTRL / B are not re-run for a measurement-only
step. Any source change re-opens them: G-CTRL `bacfc747/8dd1c004/6f9932b6`, B `f9deb9b9/d3601fcc/9da9a26a`
(`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`) must be unchanged.

## Boot discipline
Boot only while `roadmap.ps1 fleet` shows no D3-CF-U9779 row claimed or working, every boot wrapped in
`mashed_lock.py run --label D1-SHADOWS`. Spawned PIDs only.
