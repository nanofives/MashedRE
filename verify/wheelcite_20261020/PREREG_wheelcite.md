# PREREG wheelcite: a structural cite for car-wheel rotation rows (U-9776 part 1) (2026-10-20)

Branch `work/wheelcite` from `2e24a7bd2`. Instrument change only: no exe, no knob, no default change.

## Finding that motivates it (measured before this PREREG, `wheelred.py`)

The 3 RED rows of the wcsflip 13-track gate (Training 959, Roundabout 1091 / 1131, `MOVED Advantage`) are front-wheel
draws (96 triangles). Matching each standalone wheel draw to the original's by world translation, on all three frames and
on both the TRINORMAL build (wcsflip `24a608c8`) and the f5flip default:

- every wheel's world translation equals the original's (max |dt| 0.0000 over 16 wheels per frame);
- the RED pairs differ only in their steer angle about the body (-18.05 deg Training 959, +15.54 / +14.09 deg Roundabout
  1091 / 1131); on the default the same wheels differ by -1.87 / +6.15 / +2.81 deg, and the default's Training C27 rows
  (0.0136 / 0.0139) are a -9.50 deg steer difference.

Under `MASHED_CAR_REPLAY` the clump root and body take the original's trace matrices, but the wheel frames "have no trace
columns: they keep the standalone's own inputs" (`TrackRenderer.cpp` comment at the `s_car_rep` block). The steer angle is
the standalone's own AI output ~1000 frames into a 4-car race, where no AI car is equal to the original anyway (the D3 (b)
lane is at calls 150..330). So these rows measure non-replayed AI state, not rendering. C27's `moved by up to 0\.0[01]`
regex is a magnitude bound fitted to the default's observed values, not a mechanism.

## Change

1. `re/tools/drawlist3d.py`: a MOVED row carries `dpos`, the largest world-translation difference (`w[12..14]`) over its
   paired draws (absent when either side has no `w`). New cite predicate `"max_dpos": x` cites the row only if
   `dpos <= x`.
2. `verify/rres_20261016/cites_rres.json`: new cite `C27b-CAR-WHEEL-ROT`, kind MOVED, tex `^Advantage$`, `max_dpos`
   0.001, placed before C27.

## Predictions (rescore only, `run13.py score` on the existing outputs)

- P1 wcsflip tracks (`D:/MashedWT/wcsflip_data/tracks_def`): 0 RED; the 3 RED rows cite C27b.
- P2 f5flip default tracks (`D:/MashedWT/f5flip_data/tracks_def`): 0 RED, same row count 870; every row's verdict unchanged
  except C27 -> C27b relabels.
- P3 every row C27b cites has `dpos` 0.000 (measured) and is a 96-triangle wheel draw; no other row changes cite.

If P1 or P2 fails, the cite is not adopted. Adopting it does not flip TRINORMAL: that stays Mariano's decision.
