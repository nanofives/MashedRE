# RESULT wheelcite: the 3 TRINORMAL RED rows are front-wheel steer, cited by C27b (2026-10-20)

Branch `work/wheelcite` from `2e24a7bd2`. PREREG `PREREG_wheelcite.md` (`c8fec69ed`), committed before the rescore. Instrument
change only (no exe, no knob, no default change). Rescore outputs sit next to each run's own scoring, prefix `wc_`:
`D:/MashedWT/wcsflip_data/tracks_def` and `D:/MashedWT/f5flip_data/tracks_def` (`wheelcite_rescore.txt`).

## Correction to the wcsflip record

`RESULT_wcsflip.md` and the U-9776 note call the 3 RED rows "car-body draws whose pose moved". They are **wheel** draws (96-triangle
`Advantage` mesh). Every wheel's world translation equals the original's on all 3 frames (max |dt| 4.4e-7, both builds). Only their
steer angle differs (`wheelred.py`, nearest-origin pairing):

| frame | TRINORMAL (wcsflip `24a608c8`) | default (f5flip) |
|---|---|---|
| Training 959 | -18.05 deg (front pair, AABB 0.0257) | -1.87 deg on that pair, -9.50 deg on another (AABB 0.0139, C27) |
| Roundabout 1091 | +15.54 / +10.79 deg (0.0212 / 0.0153) | +6.15 / +4.38 deg (0.0080) |
| Roundabout 1131 | +14.09 deg (0.0205) | +2.81 deg (0.0043) |

Under `MASHED_CAR_REPLAY` the clump root and body take the trace's matrices, but the wheel frames have no trace columns and keep the
standalone's own steer / spin (`TrackRenderer.cpp`, the `s_car_rep` block). Frames 959..1131 are ~1000 frames into a 4-car race, past
every call where an AI car is still equal to the original (D3 (b) reaches calls 150..330). So the rows measure the AI's steer output,
not rendering. TRINORMAL moves the AI's state, so it moves the steer.

## Change

- `re/tools/drawlist3d.py`: a MOVED row carries `dpos` (largest world-translation difference over its pairs) and `tris`; new cite
  predicates `max_dpos` and `tri`.
- `verify/rres_20261016/cites_rres.json`: `C27b-CAR-WHEEL-ROT` (MOVED, `^Advantage$`, `tri` [96], `max_dpos` 0.001), before C27.

## Predictions

| | measured | verdict |
|---|---|---|
| P1 wcsflip tracks | 869 rows, 0 RED, instrument gates PASS; the 3 RED rows -> C27b, no other row changes | PASS |
| P2 f5flip default tracks | 870 rows, 0 RED, gates PASS; 4 rows C27 -> C27b (Arctic 1091, Highway 853, Training 924 / 959), no other change | PASS |
| P3 C27b rows | 7 rows, all `tris` [96], `dpos` <= 4.4e-7 | PASS |

`citecmp.py` prints the per-row comparison. On these two runs no row is left under C27: every C27 row was a wheel at the original's
origin. C27 stays in the cite file for the other captures (U-9718 (c), Dump 735 / SuperG 857 of the re-capture).

## What this means for the TRINORMAL flip

With C27b the wcsflip run's 13-track gate reads 0 RED. Its other gates already passed (G-CTRL x2 `91cc1c0b / 2d77a5d7 / ab6c9329`,
opt-out, B dumps, D2 3/3). The flip still costs A car 2 (`c0_distinct 36 [13,35]`) for C car 3. It stays Mariano's decision, and the
cite is a gate change made after the gate failed, which he has to accept as well.

## Not done here (rest of U-9776)

Car 3's 1-2 ulp past call 157 and A car 2's loss with the knob.
