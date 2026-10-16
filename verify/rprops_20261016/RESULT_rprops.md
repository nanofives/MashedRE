# RESULT rprops: per-track prop placement (U-9695) and the Arctic sky pass (U-9696)

**Verdict: both fixes pass their pre-registered gates on the nine d1r1 frames and ship default ON, with
opt-outs `MASHED_NO_PROPPLACE=1` and `MASHED_NO_SKYFIX=1`.** Matched draws rise on 9 of 9 frames and fall on
none, 0 RED rows, R-NOOP reproduces the base draw record and BMP byte for byte on 9 of 9, and the pixel
score rises by at most 0.18 anywhere. G-CTRL is unchanged. Branch `work/rprops`, measured on the rebased base `b26d9f3c` (rtex merged) after a first pass on `ec3301f3` that gave the same matched counts. Pre-registration `PREREG_rprops.md` (committed `3c0483bf`, amendment 1 `421dbd3a` after the rebase) before the first post-fix run.

## 1. Gates (PREREG section 2)

| gate | result |
|---|---|
| R-RULE1 (claimed rows match on every frame) | PASS for the rows the fixes claim: City `bark` / `Grass` 26 + 26 draws and the six `Standard.dff` building textures x 6 copies match on 3/3 City frames; TRAINING `CamManHot` and Arctic `CamManCold` match on 3/3 and 3/3 (pos exact, AABB within 0.01); Arctic `Vehicles` freighter / boat match on the frames where they are on screen; Arctic `sky` (both copies) and City `sKYLINE` match on 3/3 each. Rows left are in section 4 |
| R-RULE2 (nothing that matched stops matching) | PASS: matched draws and matched triangles rise on 9/9 frames and fall on none (`scores_v4.txt`). The key-based NEW-row list (28 rows) is all cited: extra off-screen copies (C01), STATE rows for draws that are now matched (C19), and the sky STATE row re-keyed because the sky moved from the D3D9 to the librw route (C20, one gone per one new) |
| R-RULE3 (no RED) | PASS: 0 RED rows on 9/9 frames, verdict CITED |
| G-CTRL | PASS: default build `c2e4d60d / b50bd39b / ba90ed88`, 18,664 / 56,153 / 28,076 lines, identical to the unchanged `b26d9f3c` build (`gctrl_base2.txt`, `gctrl_v4.txt`; rtex did not move the trio either) |
| R-NOOP | PASS 18/18: with `MASHED_NO_PROPPLACE=1 MASHED_NO_SKYFIX=1` the nine draw records and the nine BMPs are byte-identical to the base arm (`rnoop.txt`) |
| R-PIX | PASS: mean absolute pixel difference against the original, new base -> default: TRAINING +0.03 / +0.03 / 0.00, Arctic -1.11 / +0.18 / +0.13, City -4.25 / -5.22 / -4.78 (`pix_v4.txt`). Largest rise 0.18, bound 0.5 |

## 2. Numbers (re/tools/drawlist_diff.py 3D mode, `cites_rprops.json`)

| frame | orig draws | matched base -> now | matched tri % base -> now | rows base -> now |
|---|---:|---|---|---|
| training 884 | 242 | 194 -> 195 | 90.4 -> 91.4 | 80 -> 81 |
| training 924 | 257 | 191 -> 192 | 89.1 -> 90.1 | 79 -> 80 |
| training 959 | 233 | 191 -> 192 | 90.5 -> 91.4 | 79 -> 80 |
| arctic 1051 | 180 | 138 -> 143 | 90.8 -> 93.0 | 80 -> 82 |
| arctic 1091 | 177 | 138 -> 142 | 91.2 -> 92.9 | 80 -> 81 |
| arctic 1126 | 131 | 101 -> 103 | 91.5 -> 92.1 | 80 -> 81 |
| city 761 | 512 | 351 -> 472 | 81.2 -> 91.0 | 156 -> 131 |
| city 801 | 496 | 339 -> 460 | 81.0 -> 91.0 | 154 -> 129 |
| city 836 | 444 | 292 -> 413 | 77.7 -> 89.8 | 152 -> 127 |

"Base" is the unchanged build at `b26d9f3c` (`scores_base2.txt`), whose row counts (80/79/79, 80, 156/154/152)
match the orchestrator's figures after rtex.

Rows by citation, summed over each track's three frames, base -> now: C12 (City placement) 84 -> 6,
C13 (props at the origin) TRAINING 3 -> 0 and Arctic 3 -> 0, C14 (animation) City 9 -> 6, C15 (sky)
Arctic 12 -> 6. Rows added: C01 (extra copies off screen, TRAINING +3 and Arctic +3) and C20 / C19
state rows on draws that now match. Row counts rise on TRAINING and Arctic because a camera-man or a
boat now exists where one stood at the origin; the extra copies the original culls and librw does not
are counted off-screen in their row.

## 3. What was found, each to an address

All decoded from `original/MASHED.exe.unpatched` (decomp via `decomp_pc.py --create`, tables read by
`dump_hooktable.py`, `dump_camrows.py`, `dump_ship_tables.py`, call sites by `find_calls.py`, helper
bytes by `disasm_helpers.py`, `find_camman_arrays.py`).

1. **City trees and buildings are not the per-track hook table (U-9695's guess), they are
   `Clump_Mts_Pair`.** COURSE.LUA City: `Mts_Filename(0,"TreeTrunks.mts")`, `Mts_Filename(4,"Standard.mts")`,
   `Clump_Mts_Pair(12,0)` (Trunk.dff) and `(10,4)` (Standard.dff). `Course::CreateFromDescription`
   `0x00479330` loads each Mts to `course + idx*0x1000` (`FUN_00478110`, count at `course + 0x10000 +
   idx*4`), then for each pair takes the clump's first atomic, clones it per record (`FUN_004e69a0`,
   `FUN_004c0b30`, `FUN_004e7e30`, `FUN_004e6f80`, `FUN_004c1040`) and sets each frame to the record
   (`FUN_004c1480(frame, record, 0)`), after zeroing the clump frame translation (`FUN_004c1340`). All
   10 paired clumps in the game are single-atomic. TreeTrunks.mts has 27 records, Standard.mts 6
   (460 and 2056 bytes at 76 each). City's hook-table record (`0x005f3758`) is something else: slot 0
   `0x0044b3c0` builds oak-leaf sprite pools, slot 2 `0x0044b8f0` renders clumps 6..13 explicitly.
2. **A baked parse keeps a root-frame offset the original discards.** `Standard.dff` frame 0 is at
   (0.0217, -0.957, 0.0350) and rouabout `Frame_H.dff` at (-3.32, 0, 0). The standalone's `DffModel`
   bakes the parent chain, so the six buildings sat 0.957 low (the "MOVED 0.957" rows). `DffModel::Parse`
   now takes `bake_frames`, false for paired clumps.
3. **Camera-men are table rows.** Each of the 12 tracks with a camera-man has a slot-1 helper (one caller
   each, all inside its own record's slot 1) that loads identity, `RwFrameRotate(frame, (0,1,0),
   FUN_00442420(i), REPLACE)` and `RwFrameTranslate(frame, FUN_00442410(i), POSTCONCAT)` for entries
   1..N. `0x00442410(i)` = `0x008964fc + i*0xd8` = entry + 0x3c = the camera-table row's xyz,
   `0x00442420(i)` = entry + 0xb0. Recorded: TRAINING CamManHot (2, 0.625, -26.125) = row 1 of
   `0x005f8d50`; Arctic CamManCold (-27.513, 1.004, 43.407) = row 1 of `0x005f82a8`. N per track and the
   source clump (`course + 0x10110 + idx*4`) are in `kCamMan`, 12 of 12 read from the bytes.
4. **Arctic freighters and boats.** `0x00448770` (clump 3, one clone, 2 ships) and `0x00448880`
   (clump 6, three clones, 4 boats) place them from `0x005f95d0` / `0x005f95f0` tables; the per-frame
   `0x00448cb0` / `0x00448e20` re-pose each with yaw, roll `cos(acc)*5` and pitch `sin(acc)*5` degrees about
   the matrix's own `at` and `right` rows, and y = `sin(acc)*amp - off` (0.5 / 4.5, 0.25 / 4.0), the float
   accumulators `0x00683258 / 0x0068325c / 0x00683260` stepping 0.02 / 0.01 / 0.04. Boats use the advanced
   values (they run after the advance at `0x00448d78`).
5. **Tick phase is measured, not tuned.** The recorded freighter y at rows 1044 / 1051 / 1091 gives an
   accumulator count of K-1 (1043.0, 1050.0, 1090.0). The loop ticking once per `Render` measured K+1,
   so the first two ticks are skipped (U-9699, the original's gate is not decoded). After that the
   freighter and boat AABBs match to 0.01 on every frame where they are on screen.
6. **Sky.** `FUN_004491e0` rebuilds each sky clump frame per frame: identity, rotate 180 deg about X, 180
   about Y, translate to the camera frame position. The Arctic frame function draws the sky clump
   (`0x0044937d`) after it, and the generic path `0x00426d20` / `0x00426d90` draws the same four sky
   slots again with `Ry(0x0066d720 = 0)` + camera position (`0x00426da2`): Arctic's recorded frame
   has two 86-triangle sky draws 0.265 apart in y, TRAINING and City one. The sky DFF root frame
   (Arctic y -0.155, City `SKYDOME.DFF` -0.070) is dropped by `LoadIdentity`, which is the "0.11 higher".
   `0x0066d720` has one reference (the read), so the angle is 0.
7. **Two corrections to the d1r1 rows.** The 22-triangle `World` mesh in U-9696 is not a sky-pass draw:
   its backtrace is `0x0049aa7a` (world sector loop), Z test and write on. And U-9695's "City record of the
   hook table" is the wrong place for the trees (item 1).

## 3b. Predictions (PREREG section 3), scored

- **Pr-1** City C12 rows 84 -> 0 apart from off-screen draws: **partly refuted**. 84 -> 6. The six left are
  the `oaktreesprite` leaf sprites (216 triangles, never ported, which the prediction did not account for) and two
  off-screen `buildconc2` copies per frame. Trunks: every original `bark` / `Grass` draw is matched (27 records); buildings 6 records.
- **Pr-2** camera-man translation exact, AABB "moved by <= 0.1" and not GREEN: **translation confirmed, rest
  refuted**. Position equals the table row; the draw then MATCHES at 0.01 (not "moved"), because a 0.15 x 0.32 x 0.47
  mesh is insensitive to a 4 deg yaw error (U-9698 stays open for the yaw itself).
- **Pr-3** ships shrink to the bob amplitude: **exceeded**. The phase fit (K-1) puts every on-screen freighter and boat
  within 0.01 on all frames.
- **Pr-4** sky A matches the lower sky draw and B stays MISSING: **partly refuted, in the good direction**. Both
  copies match, because the second caller (`0x00426d20` / `0x00426d90`) was decoded in this run.
- **Pr-5** G-CTRL unchanged: **confirmed**.

## 4. What did not close, and why

- **City `oaktreesprite`** (216 triangles, MISSING): the leaf sprites, City slot 0 `0x0044b3c0` ->
  `FUN_0047c330` / `FUN_0047c560`. Not ported (U-9700).
- **Arctic `World` 22-triangle pair** (MISSING + EXTRA, same AABB): the standalone binds an unnamed
  256x256 A8R8G8B8 texture where the original binds `World` 256x256 R5G6B5 on that world-sector mesh
  (U-9696, narrowed).
- **Camera-man yaw** is run-time state (`0x00441d40`, not ported): recorded 4.398 / 182.915 deg against
  table 0 / 178.82. The rows still match at 0.01 because the mesh is 0.15 x 0.32 x 0.47 (U-9698).
- **Ten tracks have no matched-pose capture.** Camera-men on 11 tracks and the other 6 pair uses
  (Highway, rouabout x4, sands, Storm, Warzone) are placed from the decoded code and only smoke-run
  (section 5, U-9701).
- The remaining C01, C19, C20, C21 rows are d1r1's, unchanged in kind.

## 5. Smoke run on the other tracks

`smoke_tracks.py` (300 deterministic frames each, private game root, `smoke_v4.txt`): all 10 tracks without a
matched-pose capture exit 0 and log the decoded counts. Camera-men: rouabout 4, Egypt 6, sands 6, SuperG 6,
Warzone 9, Forest 6, Storm 6, dump 13, Highway 9, Neustein 5 (the helpers' `mov ebp, N`). `Clump_Mts_Pair`
copies: rouabout Shrub01 31, FatTree 9, Pylon_H 6, Frame_H 9; Highway Lampost 14; sands OakTrunk 51; Storm Bush 18;
Warzone Pylon_H 10. A rouabout frame shows the shrub tufts and distant pylons where they belong. This checks
loading and counts only. There is no original-side capture on these tracks, so no draw-list or pixel claim is made
for them (U-9701).

## 6. What changed in the default build

Yes, render only. `TrackRenderer.cpp`: `Clump_Mts_Pair` / `Mts_Filename` parse (one `read_mts` for
RWP_Object and the pairs), camera-men, Arctic ships with `UpdatePropAnims`, the sky copies (A on Arctic,
B everywhere) and their route. `DffModel::Parse(bake_frames)`. `Race/CameraEntries.{h,cpp}`
`CamEntry::TableRow`. `LibRw/RwRaceSubmit.{h,cpp}` `RaceSubmit_AddSky` and the sky pass before the world.
Knobs: `MASHED_NO_PROPPLACE=1` (pairs, camera-men, ships), `MASHED_NO_SKYFIX=1` (sky matrices, root offset
and route), `MASHED_SKY_D3D9=1` (route only, keeps the corrected matrices on the legacy draw).
The sky now draws through librw, which is the d1r3 hand-off: the legacy D3D9 sky block can go once
`MASHED_SKY_D3D9` is not needed.

## 7. Reproduce

```
cmd /c "set MASHED_NO_DEPLOY=1 && mashedmod\build.bat"
py -3.12 verify\rprops_20261016\run_arms.py mashedmod\build\mashed_re.exe v4
py -3.12 verify\rprops_20261016\run_arms.py mashedmod\build\mashed_re.exe v4noop MASHED_NO_PROPPLACE=1 MASHED_NO_SKYFIX=1
py -3.12 verify\rprops_20261016\score_rprops.py v4 --vs base2 --cites verify\rprops_20261016\cites_rprops.json
py -3.12 verify\rprops_20261016\score_pix.py base2 v4
py -3.12 verify\rprops_20261016\run_gctrl.py mashedmod\build\mashed_re.exe v4_default
```
Original-side captures queued: none taken (`original\MASHED.exe` does not boot on this PC while the audio endpoint is Unknown; all comparisons use the existing d1r1 original-side records). The ten-track capture list is U-9701.

The runners use a private copy of `original/` (`RPROPS_ROOT`, default under the session scratchpad) and
the original-side records and traces of d1r1 (`verify/d1r1_20261016/evidence`, `log/d1r1/orig_*/trace.csv`
in the main checkout, read only).
