# PRE-REGISTRATION: U-9201 (the phase-3 camera target), step 1 = measurement, no game-code change

Date 2026-10-09. Follows `RESULT_LEG2.md` §2. Committed BEFORE either capture below runs.

## 0. What the captures already settled (`o_leg1b.csv`, `L2.trace.csv`)

- **Refuted: "the original runs the camera through states 0-2".** The original's `cam+4..+0xc` is
  (0,0,0) on every state 0-2 sample, and `cam[7]` (`0x00897ffc`) is 0 throughout. Both sides enter
  phase 3 from a zero target.
- **Refuted for the release: `FUN_00442a20`.** Its decompilation (`u9201_decomp.txt`) copies entry 0's
  `+0xc0` to `cam+0x58` and entry 0's `+0x4c` matrix into the RW camera frame (`[[cam+0x84]+4]+0x10`),
  then calls `FUN_00441700`. So during phase 3 the RENDERED view is entry 0. It never writes
  `cam+0x40`, which is what entry 0 chases. It is render-only.
- **Car positions agree to about 0.04** (car 0 orig (0.5009, 0.4345, -1.9989) vs port (0.5009, 0.4748,
  -2.0025)). That cannot move the camera 0.3.
- **The camera diverges on its first update.** The original's first phase-3 target is (-0.057, 3.146,
  4.986); the port's is (0.030, 3.304, 4.838). The original then sways (x -0.14 -> +0.31, z 5.26 -> 4.44),
  while the port drifts smoothly.
- **The original's own phase-3 length varies per run** (651 / 652 / 730; `o_leg1b` about 735), with
  identical car positions.

## 1. Hypothesis H1 (decision, not diagnosis)

The main block's time input `DAT_007f1030` (`FUN_0040fc00` L170-L172: `_DAT_007f1010 +=
units/3000` when `DAT_007f1000 > 0x3c`, then `DAT_007f1030 = _ftol2(...)`; read `/ 60000` at L1039) is
a game-lifetime accumulator. So the camera's sway phase at phase-3 entry depends on how long the game
ran before the race. The port feeds `cam_ticks_`, which starts at 0 per race.

**Decision registered now:** if H1 holds (predictions P1-P2), the fix is to feed `RaceCamera` the
original's time base, and nothing else, then re-predict with `leg2_model.py` and re-run the L2 gates.
If H1 fails, no code is written in this step. The first diverging RaceCamera field (§2) is reported
and becomes the next step.

## 2. Captures

Two original runs, `orig_rampwatch.py --cam --u9201 --hz 60`, scenario `--track 0 --mode 10 --cars 4
--car 0 --poke-ctrl-slots --hold 60 --statediff-out <scratch>` (PREREG_CADENCE's run 2 recipe:
no presses). Outputs: `o_u9201_r1.csv`, `o_u9201_r2.csv`.

| # | prediction (H1) | refutes H1 if |
|---|---|---|
| P1 | `t1030` at the first phase-3 sample DIFFERS between r1 and r2, and is non-zero | equal in both runs, or 0 |
| P2 | the first phase-3 `cpx/cpy/cpz` differ between r1 and r2 by more than 0.01 | they agree within 0.01 |
| P3 | (report) each run's phase-3 length; their difference should follow P2's | n/a |
| P4 | (report) the first phase-3 sample of `+0x964..+0x9a0` per run, laid next to the port's same fields | n/a |

The port's twin fields (`RaceCamera` members citing these offsets, and `cam_ticks_`) are added to
`MASHED_LEG2_TRACE` in step 2, after these captures. They are diagnostic only and change nothing the
gates read.
