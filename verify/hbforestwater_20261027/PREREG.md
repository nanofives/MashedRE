# HB-FOREST-WATER PREREG (2026-10-27)

Committed alone, before any arm runs. Not to be amended after the first arm.

## 1. Hypothesis (static, from the bytes)

The original never draws Forest's water with the texture `WATER.DFF` ships (`wet_watercol00`).
Forest's hook record (table `0x005f33f8`, Course_Id 34) does a texture flip:

- slot 0 `0x0044dea0`: `push 0x5ce2d0` ("water"), `push [esi+0x105d0]` (course TXD), `call 0x475090`,
  store `0x0044dec5` -> `DAT_00895948`. `FUN_00475090` formats `"%s%.2d"` (`0x005cead8`), looks each
  name up with `FUN_004c5c00` (case-insensitive), stops at the first miss. Forest's `TEXTURES.TXD`
  holds `Water00`, `Water01`, `water02`..`water31` (32 textures).
- slot 1 `0x0044ded0`: `FUN_004751d0(DAT_00895948, units)` (acc += units, `cmp 0x12c` / `jl`
  advances one texture and zeroes acc), then `FUN_00475060(course clump 0, tex)`:
  `RpClumpForAllAtomics` -> `RpGeometryForAllMaterials` (`0x004e89a0`) -> `RpMaterialSetTexture`
  (`0x004e8090`).

Numbers that make it the prime candidate for the 48.1 vs 21.5 row (HB-ARCTIC section 2):

| item | mean RGB | luma |
|---|---|---|
| `wet_watercol00` (what the port binds) | 126.1 / 135.1 / 137.1, alpha 2..246 | 132.6 |
| `water00`..`water31` | about 16 / 23 / 19, alpha 255 | 19.9 .. 21.4 |
| original water region, forest_s8 (HB-ARCTIC mask) | 17.5 / 24.3 / 17.7 | 21.5 |
| port default, same region | 46.1 / 49.8 / 45.1 | 48.1 |

`WATER.DFF` prelit mean (91.6, 88.0, 83.8). Port = prelit x `wet_watercol00` (45.3 / 46.6 / 45.0).

The same mechanism runs on Sands (`0x00450cb9`, clump 1, units x2), Training (`0x004512ad`, clump 9)
and Warzone (`0x00452a9e`, clump 0, units x2). Those three are wired but NOT scored here (no
committed original water reference).

## 2. Change under test

Knob `MASHED_TEXANIM=1` (default OFF). With it on, the standalone builds the ring at load and runs
slot 1's tick + apply in `TrackRenderer::CopterTick` (the `FUN_0040fc00` slot-1 site).

## 3. Arms

Exe `mashedmod/build/mashed_re.exe` from `orch/HB-FOREST-WATER`, private root
`D:/MashedWT/HB-FOREST-WATER_root`, `verify/hbarctic_20261027/run_render.py` recipe (race demo,
deterministic clock, pinned 12-float `MASHED_CAM_POSE` from `verify/water_refs/forest_s8`),
capture `race1/01_grid.bmp`.

- `forest_s8_OFF`, `forest_s8_OFF2`: default.
- `forest_s8_ANIM`: `MASHED_TEXANIM=1`.
- Smoke (not scored): `sands`, `training`, `warzone` grid boot with `MASHED_TEXANIM=1`, the TEXANIM
  log line and a clean exit only.

## 4. Metric

Luma 0.299R + 0.587G + 0.114B. Reference `verify/water_refs/forest_s8/orig.bmp`.

- Mask W1 = the HB-ARCTIC mask, `|L(SEA) - L(OFF)| > 6` from
  `D:/MashedWT/HB-ARCTIC_data/render/forest_s8_{SEA,OFF}/race1/01_grid.bmp` (5.23% of the frame).
- Mask W2 = `|L(ANIM) - L(OFF)| > 6` (this knob's footprint).

## 5. Rules (all must hold for GREEN)

| rule | test |
|---|---|
| R0 | OFF vs OFF2 bit-identical |
| R1 | on W1: `|L(ANIM) - L(orig)| <= 8` and `< |L(OFF) - L(orig)|` |
| R2 | on W1: each RGB channel mean of ANIM within 8 of the original |
| R3 | on W1: mean local gradient (|dx|+|dy| of luma) of ANIM within a factor 2 of the original (texture-detail check, so a flat dark fill cannot pass R1) |
| R4 | outside W1 dilated by 3 px: pixels where ANIM differs from OFF <= 0.5% of the frame |
| R5 | `imgdiff.py orig.bmp ANIM` global mean abs diff not above `imgdiff.py orig.bmp OFF` |
| R6 | default gates on this exe (knob OFF) equal `merge_train.json` refs: `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` |

W2 numbers are reported alongside W1 and are not a gate.

Verdict: all R0..R6 hold = GREEN (knob ships default OFF, the flip is a decision row). Any of R1..R5
fails = RED with the measured row. R6 differs = DECISION.
