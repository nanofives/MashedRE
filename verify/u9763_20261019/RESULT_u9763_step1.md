# RESULT u9763 step 1: the D2 reference re-measured on the original (2026-10-19)

Measurement only: no band, no default and no code changed. Branch `work/u9763` from `a7ba31fe6`. Model claude-opus-5-5,
account2, host LIVING-PC.

## 1. How the current band was derived

`d81a8df6` (2026-09-29, `re/analysis/D2_REOPEN_2026-09-29.md`, pre-registered before any fix): four original solo
captures, reduced with `a8_slip_axis.py` (`slip vs fwd`, bands 1500-2000 and 2000-2600) and `a8_momentum.py
--orig-steer-min 33.0` (`driving-only horiz median`). Bound per metric = `mean_orig ± max(3h, 2% of mean_orig)`, `h` = the
four runs' half-range:

| metric | the four (steerR, elim, solo3, solo4) | mean | 3h | 2% | band |
|---|---|---:|---:|---:|---|
| slip1500 | 0.1913, 0.1911, 0.1937, 0.1937 | 0.19245 | 0.00390 | 0.00385 | 0.18855 .. 0.19635 (3h) |
| slip2000 | 0.2498, 0.2501, 0.2498, 0.2498 | 0.249875 | 0.00045 | 0.00500 | 0.24488 .. 0.25487 (2%) |
| driving-median | 1940.59, 1957.90, 1937.89, 1937.89 | 1943.57 | 30.02 | 38.87 | 1904.70 .. 1982.44 (2%) |

Original argv (solo3's provenance): `scenario_launch.py --statediff-out <msd> --statediff-drive --statediff-drive-late
--statediff-steer 1 --hold 38 --poke-ctrl-slots` (`--cars` defaults to 1). Captured on MARIANO-PC. The original-side
reduction is the whole capture (n=854 slip samples on solo3, with or without `--max-lines`).

The port side has drifted from the pre-registration since: the pre-registration used `a8_run_port.py` (`TRACK_SEL=0`,
`STEER_HOLD_AFTER=4`) with `--max-lines 1080`; the current scorer (`verify/d2settle_20261016/score_d2.py`, from attempt 20 /
d2settle) uses `MASHED_TRACK_SEL=12 MASHED_STEER_HOLD_AFTER=0` and no `--max-lines`. On the triple's port log the two
reductions differ (slip1500 0.1930 with `--max-lines 1080`, 0.1859 without; driving-median 1855.86 / 1990.80).

## 2. The original re-measured (same argv, LIVING-PC, own PIDs, no crash)

| capture | frames | slip1500 | slip2000 | driving-median (n) | H1 (post-trough >= 100 / 400, median) |
|---|---:|---:|---:|---:|---|
| `orig_solo_r1` (`dcb97cd2`) | 2335 | 0.1979 | 0.2515 | 1937.06 (1180) | 398, 1333.9 |
| `orig_solo_r2` (`a4b436d8`) | 2336 | 0.1979 | 0.2515 | 1937.06 (1181) | 398, 1333.9 |
| `orig_solo_r3` (`27512ce2`) | 2335 | 0.1953 | 0.2503 | 1971.67 (1185) | 398, 1333.9 |
| `orig_solo_r4` (`aca58d62`) | 2335 | 0.1984 | 0.2517 | 1944.28 (1177) | 398, 1333.9 |
| solo3 / solo4 (MARIANO-PC, re-reduced) | | 0.1937 | 0.2498 | 1937.89 (1154) | 398, 1333.9 |

- **The original does not fit its own band on this host.** r1, r2 and r4 have slip1500 0.1979 / 0.1979 / 0.1984, above the
  band top 0.19635; slip2000 and driving-median stay inside.
- The original is not one value: r1 = r2 bit for bit, r3 and r4 differ (the late drive injection lands on different frames),
  and the LIVING-PC runs sit higher in slip than the MARIANO-PC ones.
- All eight runs: slip1500 0.1911 .. 0.1984, slip2000 0.2498 .. 0.2517, driving-median 1937.06 .. 1971.67, H1 398/400.

## 3. The knob arms against the original's own spread (current scorer, all eight original runs)

| arm | slip1500 | slip2000 | driving-median | inside 0.1911..0.1984 / 0.2498..0.2517 / 1937.06..1971.67 |
|---|---:|---:|---:|---|
| current default (fwdsse) | 0.1942 | 0.2522 | 1916.54 | slip1500 yes; slip2000 no (+0.0005); driving-median no (-20.5) |
| pair LUT + F32 (u9761) | 0.1887 | 0.2537-0.2539 | 1959.04-1965.90 | slip1500 no (-0.0024); slip2000 no (+0.0020); driving-median yes |
| **triple LUT + F32 + SUSP_LUT (u9762)** | **0.1859** | **0.2534** | **1989.05-1990.80** | **none: slip1500 -0.0052 below, slip2000 +0.0017 above, driving-median +17.4 above** |

The three-knob arm sits outside the original's own spread on all three metrics. Its slip1500 is the furthest of the
three arms from every original run. The comparison mixes a port recipe (`TRACK_SEL=12`, `STEER_HOLD_AFTER=0`, whole log)
with the original's capture recipe, as every D2 gate since d2settle has.

## 4. Not done here (user decisions)

Whether to rebuild the band from more original runs or from both hosts, and whether the port's scoring recipe should go
back to the pre-registered one. No band or default was changed.
