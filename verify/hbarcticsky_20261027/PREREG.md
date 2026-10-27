# HB-ARCTIC-SKY PREREG (2026-10-27), written before any arm ran

Step: the Arctic sky band (rows 0..79) is about 24 luma brighter on the default build than on the
original on 3 of the 4 HB-ARCTIC-OPENSEA poses (`verify/hbarcticopensea_20261027/RESULT_HB-ARCTIC-OPENSEA.md`
section 4: e1 46.8 vs 70.9, w1 45.0 vs 69.8, n1 42.9 vs 68.7, w2 31.3 vs 35.4).

## What was read before writing this (no arm, no build)

1. Original pixels, `D:/MashedWT/HB-ARCTIC-OPENSEA_data/orig/<pose>/a.bmp`, 10-row means with the
   countdown box (x 200..440, rows 0..59) cut: on e1, w1 and n1 rows 20..69 are RGB (40,44,48) to
   (42,47,50). (40,44,48) is Arctic's `Setup_Fog(0.1,70,40,44,48)` colour (`COURSE.LUA` line 14).
   The default build shows 65..79 luma there, sky texture visible.
2. `SKY.DFF` (Arctic.piz): one geometry, flags `0x1000f` (prelit, no lighting), 52 vertices, white
   prelit, alpha 0 on the lowest ring (y -0.094, r 1.79..1.86), 127 on the next (y 0.031), 255 on
   the top rings (y 0.229..0.36). Copy B (upright) puts its alpha-0 ring about 3 degrees below the
   eye; copy A (`diag(-1,-1,1)`) puts its alpha-255 rings below the eye. The pinned poses look 25
   or 35 degrees down with a 48.46 degree vertical FOV (`orig_lens.json`), so the band is below the
   eye and only copy A covers it.
3. `FUN_004492b0` (Arctic record slot 4, decomp_pc.py on pool 10): the copy A draw at `0x0044937d`,
   `SkySecondaryDispatch` `0x00489240` and the `course+0x10130` clump (`windows.dff`) at `0x004493a4`
   all run between `FUN_004c1a00(FUN_004840d0(0))` (camera `+0x18` begin-update) and
   `FUN_004c19f0(FUN_004840d0(0))` (camera `+0x1c` end-update), after
   `FUN_004c1bb0(cam, &0xff605850, 1)` (`0x004492f0..0x00449300`, through `[DAT_007d3ff8 + 0x9c]`).
4. `FUN_004840d0(0)` returns `DAT_006ce81c[0]`, written only by `FUN_00484000` (`0x0048404b`),
   called once from Arctic slot 0 at `0x00448979` as `FUN_00484000(0, 0x80, 0x80, 0x600)`.
   `FUN_00483ed0` builds it: `FUN_004c77c0(w, h, 0, flags | 5)` stored at camera `+0x60`,
   `FUN_004c77c0(w, h, 0, 1)` at `+0x64`, `FUN_004c1a40(cam, 0.1)` (writes `+0x80`),
   `FUN_004c1b10(cam, 300.0)` (writes `+0x84`), `FUN_004c5a00(raster)` stored to `DAT_006ce82c[0]`.
   So copy A, the big lens batch and `windows.dff` are drawn through a second camera whose frame
   buffer is a 128x128 raster created with type bits 5, not through the race camera.
   [UNCERTAIN] which texture consumer reads `DAT_006ce82c[0]`: `FUN_0049a0a0` (`0x00448986`) is a
   3-way dispatch on `DAT_00773968` to `FUN_0045b350` / `FUN_0049a740` / `FUN_0049a2e0`, not decoded.
5. The port: `TrackRenderer.cpp` queues copy A (`course_id_ == 0`) and copy B into the main
   frame (`RaceSubmit_AddSky`), and `RwRaceSubmit.cpp` draws the big lens batch right after the
   first sky copy when `RaceFx_LensAfterSky()`.

## Hypothesis

H1: the band excess is copy A drawn into the race frame. In the original it goes to the 128x128
camera only, and the race frame shows the clear colour there.

## Knob (default OFF)

`MASHED_ARCTIC_SKY_RTT`:
- unset: default, unchanged code path.
- `1`: copy A and the after-sky big lens batch are not drawn into the race frame.
- `a`: copy A only is dropped (the lens batch then follows copy B).
- `lens`: the after-sky lens batch only is dropped.

The 128x128 camera and its consumer are NOT built here.

## Arms

Exe from this branch, `MASHED_NO_DEPLOY=1`. Standalone recipe of `verify/hbarcticopensea_20261027/run_render.py`
(MASHED_RACE_DEMO=1 MASHED_GOTO=6, deterministic, Arctic, `MASHED_CAM_POSE` = the original's read-back
basis). Poses e1, w1, w2, n1. Arms per pose: DEF, K1 (`=1`), KA (`=a`), KL (`=lens`). Control: DEF2 on e1.
Original frames: the existing HB-ARCTIC-OPENSEA captures (no new original run).

## Metrics

- Band = rows 0..79 minus the countdown box (x 200..440, rows 0..59), mean luma (0.299, 0.587, 0.114).
  d = |arm - original|.
- Sub-band = rows 20..69 with the same cut.
- Rest = rows 80..479, mean abs luma difference arm vs DEF (what else the knob moves).

## Predictions

- P1: K1 band d <= 6 on e1, w1, n1 (DEF 24.0 / 24.8 / 25.8).
- P2: KA alone gives >= 80% of K1's band reduction on e1, w1, n1.
- P3: K1 rest vs DEF <= 1.0 on all four poses.
- P4: G-CTRL and B on the knob-unset exe equal `re/orchestrator/merge_train.json` refs.
- P5: DEF vs DEF2 byte-identical.

Pass = P1 and P4 (and P5 as the control). Otherwise RED with the numbers. A pass does not flip the
default: that is a decision row.
