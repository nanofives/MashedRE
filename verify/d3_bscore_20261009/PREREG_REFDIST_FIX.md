# PRE-REGISTRATION: the reference-distance bridge picks the wrong car (U-9205 corrected), committed before the fix

Date 2026-10-09. Follows `RESULT_B2GATE.md`. **Correction first:** U-9205's premise ("the port's
recipe drives the player") is WRONG. The port's player gets no input (`human_drive_` true, `di.accel`
0 under `MASHED_DETERMINISTIC`). Both players are pushed by AI car 2 at frame ~22 (original d = 0.63,
port 0.61; `PUSH_ORIG.txt`), and both trail by frame 200 (z -10.91 original, -10.67 port). The player
input matches; the bridge does not.

**Finding (from the bytes).** `FUN_00442a60` (`0x00442b3a..0x00442b97`): with a, b the most-separated
pair (`FUN_0040e180`), pa/pb their wrapped `race_pct` and sa/sb their `FUN_0046cbb0` states:
- if pa > pb: ref = a if sa == 0, else b if sb == 0, else return (no store);
- otherwise: ref = b if sb == 0, else a if sa == 0, else return.

So **the reference is the pair's LEADING car** (higher race_pct), unless that car's state is non-zero.
The port's bridge (`Race/SpectatorDistances.cpp`, behind `MASHED_REFDIST`, default OFF) takes
`if (pb < pa) ref = b`, the REARMOST car, then guards. That is why the port's start-of-race distances
appear on reversed car indices, and why the trailing player reads 0.

**Fix:** transcribe the branch above exactly, including the no-store exits. Default build untouched
(knob OFF).

| # | prediction | refuted if |
|---|---|---|
| F1 | G-CTRL: knob OFF byte-identical (`c741a4c5` / `642af0ac` / `05984e30`) | differs |
| F2 | B2 arm (K = 0, full `$B2` set): at GO the reference is the front-most car (player distance 0), as the original's o1 (0 / 1.55 / 1.83 / 2.75 for v0..v3) | player distance != 0 at the first race frame |
| F3 | branch 2 first fires (exit-114 condition on the arm's own distances) at **frame 117 (car 1), 120 (car 2), 296 (car 3)**, +-15. Offline: `predict_refdist_fix.py` on `B2K00`'s trajectories. Caveat: other readers of 0x008989b0 can move trajectories before the first firing | outside +-15 (report the actual; the original is 149 / 216 / 180) |
| F4 | (report) (b) regime A under B2 + fix vs the default build's 2/3 | n/a |
