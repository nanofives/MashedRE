# RESULT: regime C under MASHED_WIRE_B2 (U-9204 step 1)

Date 2026-10-09. Pre-registration: `PREREG_B2C.md` (`770081e5`, before the arms). Arms: the
`run_bscore.ps1` recipe + the full `$B2` set, K = 0, 1, 26 (`B2C_RUN.txt`, all exit 0).

## 0. Verdict first

> - **W1 holds:** labels unchanged with the B2 knobs (K0 A, K1 B, K26 C). The knobs add no ring draw
>   before the roll.
> - **The B2 knobs change nothing inside the (b) window.** The B2 K0 window is identical to the default
>   build's K0 window, and B2 regime C is still identical to regime A. W3: A 2/3, C 0/3, unchanged.
> - **W2 (logic check) is consistent, but on a different witness than registered.** The per-call
>   `lt_ran/lt_ret/lt_fire` columns read 0 on every row of every arm. They also read 0 in the positive
>   control `NR_on` (57,592 rows), where RESULT_WIRE2 counted 103 firings by another route. **They are
>   not a live instrument in this recipe**, so "branch 2 not reached" is NOT claimed from them. The
>   witness used instead is window identity, plus RESULT_WIRE2's location of the port's firings at
>   **frames 558-660**, outside the window (frames 0-219).
> - **Main finding: the original fires branch 2 INSIDE the window; the port does not.** In
>   `verify/d3_elim_20261003/o_t3.msd.aistep.csv`, `ret148b0` is non-zero on 31 / 4 / 29 calls for
>   cars 1 / 2 / 3, first at window-relative calls **149 / 216 / 180**, and all of them fall within
>   the 220-call window. `FUN_004148b0` returns 0 when `DAT_0089a368 == 2` (`0x004148b0`;
>   `AiLeaderTimer.cpp:170`), so in the original the flag suppresses branch-2 behaviour that sits in
>   the last ~70 calls of the window. That matches original run 2 (regime C) differing from A. The
>   port fires branch 2 only after its window closes, so its regimes A and C come out identical.

## 1. What this changes for U-9204

The mechanism candidate narrows from "the flag-2 readers are unwired" to **"the port's branch-2
trigger fires about 400 frames later than the original's"**. The flag-2 test itself is ported, and
wiring it changes nothing in the window.

Caveats:
- `o_t3` is an earlier capture (2026-10-03) whose regime is unlabelled, since it records no
  `0x0089a368`.
- Run 2's car 1/2 drop-out is still unseparated.

Next concrete step: find what gates branch 2's first firing (the `FUN_004148b0` entry conditions at
the wired call site) on both sides, with a capture that records the flag.

## 2. Raw

`PREREG_B2C.md`, `run_b2c.ps1`, `B2C_RUN.txt`, `B2K{00,01,26}.{step,gates}.csv`, `B2C_SCORE.txt`.
