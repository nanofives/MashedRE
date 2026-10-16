# RESULT carlod: per-part car visibility (FUN_00420870), the glass pass (FUN_00420050) and the env gloss pass (U-9079, U-9693)

**Verdict.** Both fixes ship default ON and pass every pre-registered rule on the 9 matched-pose frames, re-measured on
the rebased tree (`race/first-frame-parity` `0020d521`, which carries `librwrest`'s poses and per-slot liveries), with
one instrument input stated below. The 27 C02-CARLOD rows are gone, the 14 C17-CAR-GLOSS rows are gone, no row appears
that is not an allowed replacement, matched draws and triangles never drop, and G-CTRL is unchanged.

| fix | what | default | rows gone (9 frames) | new rows | pixels vs original |
|---|---|---|---:|---:|---|
| F-VIS | per-part RENDER bits by part code, then the glass atomics through their own render callback | **ON** (`MASHED_CAR_VIS=0` off) | 27 C02 + 8 C21 Glass ORDER | 0 | down on 9/9 (-0.19 to -0.02) |
| F-ENV | car env gloss (rtex) with the previous row's camera for the coefficient | **ON** with F-VIS (`MASHED_CAR_ENV=0` off) | 14 C17 (7 frames x 2) | 7 STATE CarGloss, rtex amendment 2's replacements of the vanished MISSING rows | vs F-VIS arm +0.16 at most; vs base +0.04 at most |
| follow-up 3 | Atmos ZERO/ONE duals, Storm dual-UV-transform | not ported | -- | -- | no cited frame contains either model |

Default build rows, base `0020d521` -> final: TRAINING 77/76/76 -> 72/71/71, Arctic 76/76/76 -> 71/71/72, City
153/151/149 -> 148/146/146; 0 RED on 9/9 before and after; matched draws 196/193/193 -> 204/201/199, 138/138/101 ->
146/144/101, 351/339/292 -> 359/343/292; matched triangles up or equal on 9/9. G-CTRL `716cb2fd / 2a2afda1 / ba90ed88`
(the new default) on the rebased base exe and on the final default exe (`gctrl.txt`). Pre-registration `PREREG_carlod.md`
(before the first measuring run) with amendments 1-7, each labelled with when it was written. Scores: `scores_rebased.txt`
(and `scores.txt` for the arms measured on the old base `b26d9f3c`), per-arm summaries `evidence/summary_*.txt`, row
deltas `evidence/rowdelta_*.txt`, final cited diffs `evidence/final_diffs_rebased/`.

**The instrument input.** The standalone's default AI cars produce no controller brake byte, and flag `0x800` (which
lights the 4-triangle brake plate, part 0x11) is that byte. The base build matched those plates by accident because it
drew every atomic of every car. The arms therefore take `0x800` from `MASHED_CAR_BRAKE_ROWS` (measurement only, default
unset), derived from the original's own draw records (`brake_from_draws.py`: Arctic 1091 slot 3, 1126 slot 1, City 836
slots 1 and 2, none on the other six frames, every plate 0.51 m from its car). That derivation is circular for part 0x11,
so the arm is not evidence for that gate. The gate rests on the disassembly and on the live witness (section 1). Without
the input the same build fails rules 2 and 3 on exactly those three frames (pre-rebase arm `vis`, section 3). **In play
the default build never lights a brake plate**; before this branch it lit every plate on every car permanently.

## 1. What the original does (all RVAs in `MASHED.exe`; extracts in `evidence/`)

- **Visibility.** `FUN_00420870` @ `0x00420870` runs per car per frame. For each part 0..0x7f whose record dword
  (`0x0063d9e0 + slot*0x2ac + part*4`) is non-null, a switch (`0x00420a33`, handlers `0x00420a3a..0x00420c10`) turns the
  record's state `S = rec+0x298` and flags `F = rec+0x294` into a bool and `FUN_004b5240` sets or clears the atomic's
  RENDER bit (`or cl,4` `0x004b524f`, `and cl,0xfb` `0x004b5256`). The result is cleared at `0x00420a19`, so a part code
  the switch does not list is never visible. All cases but 0x17 and 0x32 AND it with `NOT (F & 0x400)`
  (`0x00420b02..0x00420b14`). The table is in `PREREG_carlod.md` section 1 A and in `LibRw/RwCarFx.cpp`
  `CarPartVisible`, one cited handler per case. **Wheel set B (parts 0x21..0x24) is visible exactly at state 1, set A
  (0..3) at state 0, neither at state 2**, which is what the original's frames show (orchestrator finding on the 9 frames).
- **State S.** Zeroed at car init (`0x00420482`), written by `FUN_0041ef60` from `FUN_00446520`: `S = 1` when the squared
  distance from part 0x13 to the camera is below 144.0 (`fcomp [0x005ce1d0]`, `0x00446958`), else 2 (`0x00446965`,
  `0x00446975`), forced to 0 for slot 0 in some camera modes (`0x0044698d..0x004469ae`).
- **Flags F.** `0x8` active, `0x40|0x80` when the course id is 0, 0x1a, 0x21 or 0x24 (`FUN_0040da50`, `FUN_0041eeb0`),
  `0x800` the brake byte, `0x10/0x20/0x400` damage and wreck.
- **Glass.** The atomics 0xe 0xf 0x2f 0x30 have no RENDER bit, yet the original draws them: after the clump render
  (`call 0x4e6680`, `0x00420162`) `FUN_00420050` calls each atomic's own render callback (`call [eax+0x48]`,
  `0x0042019d`, `0x004201d1`): 0xe then 0xf at state 0, 0x2f then 0x30 at state 1, nothing at state 2, gated by F bits
  `0x400`, `0x10/0x1000`, `0x20/0x2000`.
- **Live witness** (`vis_check.py`, `evidence/vis_model_check.txt`): rtex's 112 original car records (three tracks, all game
  phases). The table with the right S reproduces every part's render bit on 111, the distance rule picks that S on the same
  111 (the 112th is a one-frame flag read lag on the brake bit). Independent of that, the original's draw lists of the 9
  frames follow the table: for example Arctic 1126 has one state-1 car (13 `Advantage` parts, 2 `Glass`), three state-2
  cars (one `AdvantageSmall` plus the two light plates each) and one brake plate, 20 `Advantage` draws
  (`evidence/car_textures.txt`). The state rule predicts all 36 car-frames from the original's draw lists with 0 errors
  (`evidence/state_lag_test.txt`).

## 2. What changed

- `LibRw/RwCarFx.{h,cpp}`: `CarPartVisible`, `CarGlassPart`, `CarLodState`, `CarVisFlags`, `CarVisEnabled`; the env pass
  follows the visibility by default.
- `LibRw/RwRaceSubmit.{h,cpp}`: a car copy (plain, env or `librwrest`'s posed one) carries `CarVis {state, flags}`; before
  drawing it every atomic's RENDER bit is set from the part code (the model's build-time bits for copies without a state),
  `librwrest`'s pose and draw-order block then runs unchanged, and the glass atomics are rendered through
  `Atomic::render()` after it, inside the same light and render-state scope and only for a copy that was not culled. A part
  table of the Advantage car is logged at registration (`evidence/car_parts.txt`).
- `D3d9Render/TrackRenderer.cpp`, `Race/RaceSceneState.h`: state from part 0x13's distance (`part19_d2`, FUN_0041f120's operand
  order), flags per course; in `librwrest`'s `submit` lambda the env coefficient's distance against the **previous**
  frame's eye (`eye_prev_`); `MASHED_CAR_BRAKE_ROWS`, `MASHED_CAR_VISLOG` (diagnostics, default unset).
- `Util/PromoLoop_sessionB.cpp`: `Or4b5240` gained the clear arm it lacked.
- `Track/TrackWorld.cpp`: **not changed by this lane in the end.** `librwrest` fixed the same sector-layout bug
  independently (4-byte sector normals); `evidence/bsp_layout_check.txt` (all 490 sectors of the 13 track pizzes: the header
  flags' layout differs from the old size-only solver on exactly 6, Arctic COLLISIONS.BSP x5 and AI.BSP x1) agrees with it.

## 3. Rules (PREREG section 3), on the rebased tree

| rule | F-VIS (`visx` vs `base2`) | F-ENV (`final2` vs `visx`) |
|---|---|---|
| 1 targets gone | PASS 27/27 C02 (+8 C21 Glass ORDER) | PASS 14/14 C17 on the 7 frames that had them (TRAINING 3, Arctic 1051/1091, City 761/801) |
| 2 no new rows | PASS 0 on 9/9 | PASS: 7 STATE CarGloss rows with differing fields inside {AREF, AF, CULL}, each on a frame where its MISSING row vanished (amendment 2); nothing else new |
| 3 matched do not drop | PASS equal to base on 9/9 (draws and triangles) | PASS up: +8 +8 +6, +8 +6 0, +8 +4 0 |
| 4 no-op control | PASS 9/9 (`visoff` = `base2`, rows, matches and pixels) | PASS: `nolagx` (`MASHED_CAR_ENV_PREVCAM=0`) differs from `final2` only at City 801 (2 rows, matched 341 vs 343), the frame the previous-camera change was made for |
| 5 G-CTRL | PASS `716cb2fd / 2a2afda1 / ba90ed88` | PASS (final default exe, same trio) |
| 6 pixels | PASS: `base2` -> `visx` -0.12 -0.14 -0.06 / -0.12 -0.03 -0.02 / -0.19 -0.06 -0.05 | PASS: `visx` -> `final2` +0.05 +0.14 +0.05 / +0.16 0.00 0.00 / +0.08 +0.01 0.00 (max +0.16 < 0.5); against `base2` the largest is +0.04 |

Measured before the rebase and kept on record (arms on `b26d9f3c`, `scores.txt`): `vis1_noglass` (RENDER bits only) lost the
original's Glass draws (new MISSING Glass row, matched 194 -> 186); `vis` (with the glass pass, no brake input) fails rules
2 and 3 on Arctic 1091/1126 and City 836 (the brake plates, 1 + 1 + 2 draws); `visb_env` (rtex's coefficient) left City 801
with two cars unmatched and Arctic 1051 +0.57 against `visb`, which the previous-camera coefficient and Arctic's collision
colours (shade 1.0 against the original's 0.25) fixed.

## 4. Found on the way

1. **The env coefficient is computed against the previous row's camera.** rtex's 112 live coefficients are reproduced with
   residual 0.00000 by trace row K-1's camera and are off by up to 0.009 with row K's (`evidence/env_lag_live.txt`); the
   original draws env for 36/36 car-frames when predicted with row K-1 and 35/36 with row K (`evidence/env_lag_test.txt`).
   rtex's "about 1e-4 on stationary rows" was this lag. The state rule cannot discriminate (all 16 lag combinations give 0
   errors on 36 car-frames), so it keeps this frame's values.
2. **Arctic's collision BSP was read colourless** (size-only layout solver, 4-byte sector normals), so the env shade sampler
   returned 1.0 against the original's ~0.25 and the Arctic gloss was 0.87 against 0.61. Found here, fixed upstream by
   `librwrest` at the same time; the 6-sector check in `evidence/bsp_layout_check.txt` covers all 13 tracks.
3. **The existing `.asi` hook `Or4b5240` (FUN_004b5240) only set the render bit.** The binary also clears it
   (`dis_004b5240.txt`). Under the `.asi`, `FUN_00420870` could never hide an atomic. Fixed from the disassembly, row
   demoted C3 -> C2 until a flag=0 / bit-set vector is diffed.
4. Part 0x13 is a 1-triangle locator (`evidence/car_parts.txt`): the standalone's part-0x13 distance agrees with the car-origin
   distance to the 0.1 the origin estimate resolves (`evidence/env_cars.txt` against `evidence/vislog_cited_rows.txt`).
5. First launches of an arm sometimes write no record and exit 0 (old base TRAINING, `vis1_noglass` Arctic); a re-run of that
   track produced the expected numbers each time (PREREG amendment 2; `run_all.py` now re-runs such a track once). The
   shared `log/librw_race.txt` and `mashed_re.log` are overwritten by other sessions' runs, so diagnostics here write their
   own files.
6. Two builds must not run at once: a worktree created at a relative path ended up nested inside this one and its build wrote
   into this worktree's `build\obj`. Removed with `scripts/diag.py wt-remove`; the base was rebuilt alone.
7. `decomp_pc.py` still fails from a worktree (d1r1 note 5); the main checkout's copy was run for read-only decompilation.

## 5. Gaps (U-9703, filed)

(a) F bits `0x10/0x20/0x400` stay 0 (no damage or wreck model). (b) `0x800` has no source for AI cars or for the player
(`TrackRenderer.cpp` `io.input[5]` is not wired). (c) Slot 0's `S = 0` modes. (d) S timing against the camera. (e) Car
models with their own part-0x13 origin (per-slot liveries) use the player's `car_part19_`. U-9697 (c) stays open for the
flat-soup sampler; (g) Atmos / Storm stays unported (no cited row).

## 6. Reproduce

```
mashedmod\build.bat          (set MASHED_NO_DEPLOY=1; one build at a time)
py -3.12 verify\carlod_20261016\run_all.py <exe_base> <exe_new>      (the rebased campaign: base2, final2, visx, visoff, nolagx, G-CTRL)
py -3.12 verify\carlod_20261016\run_arm.py <exe> <arm> [--tracks training,arctic,city] [--no-brake] [KNOB=V ...]
py -3.12 verify\carlod_20261016\rowdelta.py <before arm> <after arm>
py -3.12 verify\carlod_20261016\vis_check.py ; env_lag_live.py ; env_lag_test.py ; state_lag_test.py ; bsp_layout_check.py
py -3.12 verify\carlod_20261016\brake_from_draws.py ; collect_evidence.py
```

Arms on `0020d521`: `base2`, `visx`, `visoff`, `nolagx`, `final2` (all defaults). Arms on `b26d9f3c`: `base`, `vis1_noglass`, `vis`,
`visb`, `visb_off`, `visb_env`, `venv`, `venv_nolag`, `visb2`, `venv2`, `final`. Raw runs, BMPs and the frozen exes are under
`log/carlod/` (not committed).
