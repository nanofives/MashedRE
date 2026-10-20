# PREREG u9773: U-9773 and the failing (b) cars (2026-10-20)

Branch `work/u9773` from `work/f5flip` `4f4078955` (FAITH + u9774's five default ON). Model claude-opus-5-5, account2. Exe
`MASHED_NO_DEPLOY=1`, root `D:\MashedWT\u9756_root`. Written before any knob arm. Failing (b) cars on the default: A car 1, B car 2,
C cars 2 and 3.

## 0. Measured before this PREREG

1. On the new default (`B_R3_K26`, `B_R2_K26`, record dumps; `recpos.py` vs `o_c3` / `o_c2`): car 3 first differs at call 149,
   position first (`+0x958` -3.289e-05, `+0x960` -2.880e-04, velocity equal); car 2 at call 150 (`+0x9b0` -1.25e-3, `+0x9b8` +3.6e-2).
2. Original write watch on car 3's `+0x958 / +0x960` (`o_ww958c3_2`, the B recipe, `--watch-write 958,960`; equal to `o_c3`
   through call 159; the first try `o_ww958c3` rolled regime B at call 61): per frame the position is written by the substep
   integrate (`0x0046ea62 / 0x0046ea76`), a matrix call (`0x004c4a29 / 0x004c4a35`), and last by the wheel contact solver
   FUN_0046f6c0's correction `0x0046fbc7..0x0046fc00` (`pos -= (1/n) * sum`, n = grounded wheels after the all-four drop). For
   call 149 that correction is dx -1.17e-05, dz +3.224e-04 (from -0.213374496 / -18.4990807 to -0.213386163 / -18.4987583).
3. The port's correction block (`Collision/WheelContactSolver.cpp`) matches the original's arithmetic and wheel selection, so the
   difference is in its inputs. New diagnostic `MASHED_WCS_POSLOG=<path>` (default OFF): per call, slot, substep, n, wheel
   states, the three sums, inv and the corrected position.

## 1. Method

Find the first differing input of that correction for car 3 at call 149 (states, loads `+0x194/+0x258/+0x31c/+0x3e0`, the
contact normal terms), name its producer's RVA, port the fix behind a default-OFF knob (amendment with its measurement), and
re-check car 3's record past 149. Then car 2 (150), then A car 1 and B car 2. Gates for any default candidate: G-NOOP (default
`df5ee50e / a90bbb16 / b3cd0e5f`, B `c165d2e5 / c01e164a / 2e937af7`), (b) all regimes reported per car, D2 3/3, 13 tracks 0 RED.
