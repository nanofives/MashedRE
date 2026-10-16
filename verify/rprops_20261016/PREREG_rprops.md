# PREREG rprops: per-track prop placement (U-9695) and the Arctic sky pass (U-9696)

Committed before the first measuring run. Base: `race/first-frame-parity` `ec3301f3`.
Branch `work/rprops`, worktree `.worktrees/rprops`. Evidence dir `verify/rprops_20261016/`,
bulky runs under `log/rprops/` (gitignored). Assets come from a private copy of `original/`
(memory `mashed-root-main-shares-append-logs`), never the shared main root.

## 0. What exists at the base (surveyed, no new run)

Instrument: d1r1's matched-pose per-draw recorder and `re/tools/drawlist_diff.py` 3D mode
(`verify/d1r1_20261016/RESULT_d1r1.md`), nine frames: TRAINING 884/924/959, Arctic 1051/1091/1126,
City 761/801/836. Citations `verify/d1r1_20261016/cites_d1r1.json`. Original-side records and
traces are reused as they are (`verify/d1r1_20261016/evidence/*_orig_row_*.draws.json`, traces at
`log/d1r1/orig_*/trace.csv` in the main checkout, read only). Only the standalone side is re-run.

Decoded before this file, from `MASHED.exe` (`original/MASHED.exe.unpatched`), by
`decomp_pc.py --create` plus the raw tables read by `verify/rprops_20261016/dump_hooktable.py`,
`dump_camrows.py`, `find_calls.py`:

1. **City tree and building copies are `Clump_Mts_Pair`, not the per-track hook table.**
   COURSE.LUA City: `Mts_Filename(0,"TreeTrunks.mts")`, `Mts_Filename(4,"Standard.mts")`,
   `Clump_Mts_Pair(12,0)` (Trunk.dff), `Clump_Mts_Pair(10,4)` (Standard.dff). The standalone ignores
   both commands. `Course::CreateFromDescription` `0x00479330` loads each Mts at `course + idx*0x1000`
   (`FUN_00478110`, count at `course + 0x10000 + idx*4`, records stride 0x40), then for each pair takes
   the clump's first atomic (`FUN_004b3f90`), and for record i >= 1 clones it (`FUN_004e69a0`), gives the
   clone a new frame (`FUN_004c0b30`), adds it to the clump (`FUN_004e6f80`) and the clump frame
   (`FUN_004c1040`), then `FUN_004c1480(frame, record, 0)` replaces each atomic frame matrix with the
   record. All ten paired clumps in the game (City 2, Highway 1, rouabout 4, sands 1, Storm 1,
   Warzone 1) are single-atomic (`dff_dump.py`), so a pair replaces the clump's one identity instance
   with one instance per record. TreeTrunks.mts has 27 records, Standard.mts 6 (file sizes
   2056 / 460 bytes at 76 bytes per record).
2. **Camera-men are placed from the camera-entry table, not from the DFF.** Every track with
   `CamMan.dff` has a slot-0 helper that clones the clump N-1 times and a per-frame helper that, for
   entries i = 1..N, does `RwFrameRotate(frame, (0,1,0), FUN_00442420(i), REPLACE)` then
   `RwFrameTranslate(frame, FUN_00442410(i), POSTCONCAT)` (Arctic `0x00448dc0`, N=5; TRAINING
   `0x00451220`, N=3; 12 call sites of `0x00442410` in all, found by raw E8 scan). `0x00442410(i)` =
   entry `0x008964c0 + i*0xd8` + 0x3c = the table row's xyz, `0x00442420(i)` = entry +0xb0 (a float the
   type-1 step `0x00441d40` rewrites at run time). Recorded original: TRAINING CamManHot world
   translation (2, 0.625, -26.125) = table row 1 of `0x005f8d50`; Arctic CamManCold
   (-27.513, 1.004, 43.407) = row 1 of the default table `0x005f82a8`. Recorded yaw is 4.398 deg
   (TRAINING) and 182.915 deg (Arctic) against table values 0 and 178.82, so the yaw is run-time state.
3. **Arctic freighters and boats** (`Vehicles` texture, 331 and 117 triangle draws) are Arctic hook
   record slot-0 helpers: `0x00448770` clones the freighter clump (`course+0x1011c`, clump 3) once
   (2 ships), `0x00448880` clones the fishing boat clump (`course+0x10128`, clump 6) three times
   (4 boats), both placed from float tables at `0x005f95d0 / 0x005f95e8` and `0x005f95f0 / 0x005f9620`;
   the per-frame helpers `0x00448cb0` / `0x00448e20` re-pose them with sine/cosine accumulators
   (`0x00683258 / 0x0068325c / 0x00683260`). Recorded original y of the 331-triangle draw drifts
   (-4.043, -4.076, -4.395) across rows 1044/1051/1091.
4. **Sky** (`FUN_004492b0`, Arctic record slot 4): `FUN_004491e0` first sets each non-null sky clump
   frame (`course + 0x100f0 + i*4`, 4 slots) to identity, translate (0,0,0), rotate 180 deg about X
   then 180 deg about Y, then translate by the camera frame position (`cam_frame + 0x40`)
   (`0x004491e0`). The recorded original draws the 86-triangle sky mesh twice (backtraces
   `0x0044937d` and `0x00426da2`), about 0.27 apart in y. The standalone draws it once, translate-only,
   on the legacy D3D9 path (`TrackRenderer.cpp` `sky_`), 0.11 above the original. The 22-triangle
   `World` draw in the C15 rows is an ordinary world-sector draw (backtrace `0x0049aa7a`) that the
   standalone draws untextured.

## 1. Fixes under test

- **F-PAIR** `Clump_Mts_Pair`: parse `Mts_Filename` and `Clump_Mts_Pair` in the COURSE.LUA scan; a paired
  clump's instance list becomes the Mts record matrices (3x3 + translation, the layout the RWP_Object
  path already reads).
- **F-CAM** camera-men: instances at camera-table rows 1..N for the track (N = 5 Arctic, 3 TRAINING,
  per-track counts from the 12 helpers), yaw from the table row.
- **F-SHIPS** Arctic freighters and boats from the `0x005f95d0` tables with the tick accumulators.
- **F-SKY** the sky frame of `0x004491e0` (and, when the legacy path is deleted, the sky pass in the librw
  path: depth test and write off, unfogged, drawn first).

Opt-out knobs: `MASHED_NO_PROPPLACE=1` (F-PAIR, F-CAM, F-SHIPS) and `MASHED_NO_SKYFIX=1` (F-SKY).

## 2. Gates

| gate | pass condition |
|---|---|
| **R-RULE1** | every row a fix claims to turn GREEN matches on every frame where it occurs |
| **R-RULE2** | no row that matched at the base stops matching on any of the nine frames (matched draws and matched triangles per frame do not fall) |
| **R-RULE3** | no new RED row: every unmatched row on the nine frames hits a citation |
| **G-CTRL** | default build fingerprint `c2e4d60d / b50bd39b / ba90ed88` (`run_gctrl.py`, step / gates / ptrace sha8) before and after, ptrace line count 27,356 |
| **R-NOOP** | the opt-out arm reproduces the base draw record byte for byte |
| **R-PIX** | `imgdiff` against the original BMPs on the nine frames does not rise by more than 0.5 on any frame |

A fix is default ON with the opt-out knob only if R-RULE1..3, G-CTRL, R-NOOP, R-PIX all pass.
Otherwise it stays default OFF with a recommendation in `RESULT_rprops.md`.

## 3. Predictions (falsifiable, fixed before the run)

- **Pr-1** City: with F-PAIR the C12 rows go from 84 to 0 apart from draws wholly outside the
  frustum (the original culls by sector, librw does not; those hit C01). Trunk draws: 27 records,
  26 or 27 on screen per frame. Building copies: 6 records.
- **Pr-2** Camera-man world translation equals the table row to 1e-4 on both tracks. The AABB still
  differs from the recorded one by the yaw error only (the table yaw is not the run-time yaw), so the
  CamMan rows move from "moved by 26 / 43" to "moved by <= 0.1", not to GREEN.
- **Pr-3** Ships: the `Vehicles` MOVED rows (89.3 units) shrink to the bob amplitude. The tick phase
  at a given trace row is not known from the trace; if the phase offset cannot be fitted from the
  recorded y values (rows 1044, 1051, 1091) the ships ship with the accumulators at zero and the
  residual is cited.
- **Pr-4** Sky: with the `0x004491e0` rotation the standalone's dome AABB matches the recorded lower
  sky draw (`0x0044937d`) to 0.01 on the three Arctic frames; the second sky draw (`0x00426da2`) stays
  MISSING until its caller is decoded.
- **Pr-5** F-PAIR, F-CAM and F-SHIPS change no value the simulation reads, so G-CTRL is unchanged.

## Amendment 1 (committed before the first post-fix run)

- The G-CTRL ptrace line count in section 2 (27,356) was copied from the memory note that predates
  d2settle's A3 LUT sqrt. The unchanged base build measures **28,076 lines**, step 18,664, gates 56,153,
  sha8 `c2e4d60d / b50bd39b / ba90ed88` (`log/rprops/base_gctrl.txt`), the registered trio. The gate is
  the trio plus those line counts.
- Baseline arm re-run on the private game root reproduces d1r1's nine frames exactly (matched draws
  194/191/191, 138/138/101, 351/339/292, rows 86/85/85, 81/81/81, 157/155/152; `scores_base.txt`). So
  the instrument is deterministic across sessions, which is I-N0 for this lane.
- Ship tick phase: from the recorded freighter y at rows 1044, 1051, 1091 (-4.043, -4.076, -4.395) and
  the formula y = sin(acc258)*0.5 - 4.5 with acc258 advancing 0.02 per tick, the tick count at row K is
  K - 2 to K - 3 (the sin branch x = 1.99, 2.14, 2.93 plus three periods gives n = 1041.6, 1048, 1089
  for rows 1044, 1051, 1091). The standalone ticks once per Render. The offset between the two counts
  is measured in the post-fix run and reported; it is not tuned.
