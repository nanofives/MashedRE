# PREREG carlod: per-part car visibility (FUN_00420870) and the env gloss pass (U-9079, U-9693)

Committed before the first measuring run. Base: `race/first-frame-parity` `b26d9f3c` (rtex merged).
Branch `work/carlod`. Instrument: d1r1's matched-pose per-draw recorder and `re/tools/drawlist_diff.py`
3D mode (`verify/d1r1_20261016/RESULT_d1r1.md` sections 1 and 7) on its 9 frames: TRAINING 884/924/959,
Arctic 1051/1091/1126, City 761/801/836. The original side is d1r1's committed records
(`verify/d1r1_20261016/evidence/*_orig_row_*.draws.json`) and traces (`log/d1r1/orig_*/trace.csv`),
reused unchanged, so every row delta is a standalone-side change. Harness: copies of rtex's
`run_arm.py` / `run_replay3d.py` / `rowdelta.py` / `run_gctrl.py` in this directory, outputs under
`log/carlod/`. Base exe: `log/carlod/exe_base/mashed_re.exe`, the clean build of `b26d9f3c`.

Cited rows this lane owns (from `verify/rtex_20261016/evidence/final_diffs/`): **C02-CARLOD**, 3 rows per
frame, 27 in all (EXTRA Advantage 91-134 draws, EXTRA AdvantageSmall 12-16, EXTRA Glass 8-16) and
**C17-CAR-GLOSS**, 2 rows per frame on 7 frames (TEX1 `Advantage ... CarGloss`, MISSING `CarGloss`).

## 1. What the original does (static, then live)

All RVAs in `MASHED.exe` (anchor in CLAUDE.md). Extracts in `evidence/`.

**A. The per-part loop.** `FUN_00420870` @ `0x00420870`, called per car per frame from `FUN_0040da50`
(`FUN_00420870(slot, param_2, view matrix)`). The car record is `0x0063d9e0 + slot*0x2ac`; its dword
`part*4` (part 0..0x7f) is the part's RpAtomic, null entries skipped (`0x00420b20`). For each one a
switch (`0x00420a33`..`0x00420b16`) computes a bool from the record's **state** `S = *(rec+0x298)`
(`0x0063dc78`, values 0/1/2) and **flags** `F = *(rec+0x294)` (`0x0063dc74`) and passes it to
`FUN_004b5240` (`0x00420b16`), which sets (`or cl,4` `0x004b524f`) or clears (`and cl,0xfb`
`0x004b5256`) bit 4 of the atomic's flags byte (`mov byte [eax+2],cl`), rpATOMICRENDER. Every case then
ANDs the result with `NOT (F & 0x400)`, except parts 0x17 and 0x32 (`0x00420b98`).

| parts | visible when |
|---|---|
| 0 1 2 3 8 9 0x10 0x12 0x14 | S = 0 |
| 4 6 / 5 7 | S = 0 and F&0x10 clear / set |
| 0xa 0xc / 0xb 0xd | S = 0 and F&0x20 clear / set |
| 0x46 | S = 0 |
| 0x17 | S = 0 and F&0x400 (skips the 0x400 mask) |
| 0x21 0x22 0x23 0x24 0x29 0x2a 0x31 0x47 | S = 1 |
| 0x25 0x27 / 0x26 0x28 | S = 1 and F&0x10 clear / set |
| 0x2b 0x2d / 0x2c 0x2e | S = 1 and F&0x20 clear / set |
| 0x32 | S = 1 and F&0x400 (skips the 0x400 mask) |
| 0x64 | S = 2 and F&0x30 clear |
| 0x65 / 0x66 / 0x67 | S = 2 and (F&0x10, F&0x20) = (set, clear) / (clear, set) / (set, set) |
| 0x11 | F&0x800 set and F&0x20 clear (any S) |
| 0x15 | F&0x40 set and F&0x10 clear (any S) |
| 0x18 | F&0x80 set and F&0x20 clear (any S) |
| every other part code | never (`bVar11` stays false, `0x00420a2e`) |

**B. State S.** Zeroed at car init (`FUN_00420420` `0x00420482`); written by `FUN_0041ef60` (store at
`0x0041ef6e`) from the race camera update `FUN_00446520`: `0` for slots 0..3 at its start (`0x00446547`);
in the chase branch, per slot, `FUN_0041f120(slot, 0, &d2)` (3 stack args, `add esp,0xc` `0x0044694f`) gives
`d2` = |`rec+0x258` (part 0x13's LTM position, cached by `FUN_00420870`) - camera object frame position|^2,
then `fld [ebp-0x1ac]; fcomp [0x005ce1d0]; test ah,5; jp` (`0x00446952`..`0x00446963`): `S = 1` when
`d2 < 144.0` (`[0x005ce1d0]` = 144.0, read from the image), else `S = 2` (`0x00446965`, `0x00446975`);
then `S = 0` for slot 0 when `FUN_0042f6a0() == 2` or `DAT_007f0fd0` is 5, 9 or 10 (`0x0044698d`..`0x004469ae`),
and `0` for all slots at `0x004486d9` (`FUN_00448220`).

**C. Flags F.** Bit 3 (`0x8`, slot active, `FUN_0041eda0`). `FUN_0041eeb0` (`0x0041eeb0`, called per car per
frame at the end of `FUN_0040da50`, `FUN_0041eeb0(slot, bc44, bc44, brake)`): `0x40` and `0x80` follow
`DAT_0063bc44[slot]`, which `FUN_0040da50` sets to 1 when `DAT_00644158` (the course id) is 0, 0x1a, 0x21
or 0x24; `0x800` follows the controller's byte `*(0x007f103c + ctrl*0x4c + 1)` (the brake byte, per
`Ai/AiController.cpp` `[5] brake`); then `F&0x400` clears `0x8c0`, `F&0x10` clears `0x40`, `F&0x20` clears
`0x880`. `0x10`/`0x20` are written by `FUN_0041ede0`, `0x400` by `FUN_0041ef80`.

**D. Live witness (original, 112 car records)**, `vis_check.py` on rtex's `--matfx` captures
(`log/rtex/orig_matfx2_*`, every 100 trace rows, all game phases), `evidence/vis_model_check.txt`:
- the table in A with the right S reproduces **every** part's render bit on 111 of 112 records (71 parts
  each); the 112th is City row 900 car 2, whose flag word was read one frame after the bit it produced
  (`0x8c8` -> `0xc8`, part 0x11 still on);
- the distance rule in B picks that S on the same 111 (`d2` from `pos258` and `p29c_pos`);
- flags seen: TRAINING `0x008`, Arctic and City `0x0c8` (course ids 30 / 0 / 26; 0 and 0x1a are in C's set,
  30 is not), `0x800` on two City cars at row 900. `0x10`, `0x20`, `0x400` never set.
- visible parts at S = 1, F = `0x008`: `0x21 0x22 0x23 0x24 0x25 0x27 0x29 0x2a 0x2b 0x2d 0x31` (11 per car,
  the 44 Advantage draws of d1r1 over four cars); with F = `0x0c8` plus `0x15 0x18`. At S = 2: `0x64` only
  (plus 0x11 / 0x15 / 0x18 by flags).

**E. The standalone today** draws every non-hull atomic of the car model, so it draws the S = 0 / 1 / 2
sets together (92-134 extra Advantage draws, 12-16 AdvantageSmall, 8-16 Glass per frame, C02). With the
env pass on (`MASHED_CAR_ENV=1`) the extra Glass atomics also get the second pass (rtex rule 2 / 6 fail).

## 2. Fixes (each with an env knob; default decided by section 4)

- **F-VIS**: `Vehicle/CarPartVisible.{h,cpp}` (new, both targets, one body) transcribes A. `LibRw/RwRaceSubmit`
  gives each queued car instance a state and flag word and sets each atomic's RENDER flag from
  `CarPartVisible(S, F, part)` before drawing it (parts from `g_model_parts`; the model's build-time
  flags restored for instances without one). `TrackRenderer` supplies S from B (the part-0x13 world position
  `car_part19_` x the instance matrix against `last_eye_`, FUN_0041f120's operand order, unclamped `d2 < 144.0`)
  and F = `0x8` plus `0xc0` when `course_id_` is 0, 0x1a, 0x21 or 0x24. Knob `MASHED_CAR_VIS=0` disables.
  Stated gaps, `[UNCERTAIN]`, U-row filed: F bits `0x10`, `0x20`, `0x400` stay 0 (the standalone has no
  damage or wreck state to feed them), `0x800` stays 0 (no brake-byte binding in the standalone;
  `Vehicle::g_launchAccelByte` is the accel byte only), the slot-0 `S = 0` modes of B are not modelled,
  and S uses the current frame's matrix where the original reads last frame's cache (differs only for a
  car within one frame of the 12 m threshold).
- **F-ENV** (re-run, not new code): `MASHED_CAR_ENV=1` on top of F-VIS, judged by the same rules.
- **Follow-up 3** (48 ZERO/ONE dual materials on the Atmos car, the dual-UV-transform material on Storm):
  only if cited. All four cars on all 9 frames draw texture `Advantage` (d1r1 Pr-1), so no cited row
  involves either model. Filed, not ported, unless a cited row appears.

## 3. Rules (per fix, on all 9 frames)

1. **Targets.** F-VIS: the three C02-CARLOD rows disappear on all 9 frames (27 rows). F-ENV, measured on top
   of F-VIS: the C17-CAR-GLOSS rows (TEX1 `Advantage ... CarGloss`, MISSING `CarGloss`) disappear on every
   frame that has them (TRAINING 3, Arctic 1051 / 1091, City 761 / 801).
2. **No new rows.** No (kind, tex, detail-class) row appears that the comparison arm lacks
   (`rowdelta.py`, rtex's key), on any of the 9 frames, and every remaining row stays cited (0 RED). F-ENV
   keeps rtex amendment 2: a NEW STATE row whose differing fields are a subset of {AREF, AF, CULL} on a
   texture whose MISSING row disappeared in the same frame is that MISSING row's replacement (cited C19), not
   a failure. Any new EXTRA / MISSING / ORDER / MOVED row fails.
3. **Matched draws and matched triangles do not drop** per frame.
4. **No-op control.** `MASHED_CAR_VIS=0` reproduces the `base` arm's rows on 9/9 frames.
5. **G-CTRL** (`run_gctrl.py`, TRAINING default arm) gives `c2e4d60d / b50bd39b / ba90ed88` before (base exe)
   and after (final default exe).
6. **Pixels (report).** Mean abs RGB difference against the original BMP per frame, base arm vs each arm.
   A rise above 0.5 on any frame blocks default ON (recommendation instead).

A fix passing 1-5 ships default ON with its opt-out knob, rule 6 permitting. Failing any keeps it default OFF.

## 4. Arms

`base` (exe_base, default knobs), `vis` (new exe, defaults), `vis_off` (`MASHED_CAR_VIS=0`, rule 4),
`vis_env` (`MASHED_CAR_ENV=1`), and if F-ENV passes the final exe with its new defaults. Rule 2 for F-VIS
compares `vis` against `base`, for F-ENV `vis_env` against `vis`.

## 5. Predictions

- Pr-1: with F-VIS the standalone draws 11 Advantage-family parts per car on TRAINING and 13 on Arctic and
  City (0x15, 0x18), and every C02 row goes; no other row moves.
- Pr-2: the C03 depth-off per-car mesh row (`0x004ec3e1`, `FUN_0041faa0`) is unaffected: it is not one of
  the 11 parts.
- Pr-3: with F-ENV the env second pass runs only on the visible env parts of A's set (parts 0x25, 0x29,
  0x2b at S = 1) and the Arctic 1051 pixel rise of rtex (+0.60) is gone.
- Pr-4: `MASHED_CAR_VIS=0` is bit-identical in rows to base, so the no-op control passes.

## 6. Amendments

(none yet)
