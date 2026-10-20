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

## Amendment 1 (measurement, no knob yet)

- `MASHED_WCS_POSLOG` now also writes `<path>.ent`: per solver call (FUN_0046f6c0), the four wheels' state / fv / key at
  ENTRY, the channel `scenario_launch.py --wheelstate-probe` samples on the original.
- Car 3, B recipe, original `o_wsp3_1` (`--wheelstate-probe`, equal to `o_c3` through call 399) vs port `B_RP3_K26-2`: the entry
  samples are bit-equal for every solver call through the second call of frame 1019 (the frame that produces record call 149).
  The next entry (= that call's output) differs on every wheel (orig w0..w3 fv -0.00449260 / -0.00449260 / -0.00959048 /
  -0.00749469, port -0.01714067 / -0.01714067 / -0.00409550 / +0.01018281), with all states 1 on both sides and the end-of-frame
  contact normals equal. So the split is made inside that one solver call, by the classifier's depth (FUN_0046cc40, the write at
  `0x0046d008` from the BSP walk `0x00538cd1`): its inputs are the transformed wheel points `DAT_0088e620` (from
  `Rw_VtableDispatch(g_wheelContactPos, g_suspScratch, 4, ring matrix)`) and the walked batch. The correction arithmetic
  (`0x0046fbc7..0x0046fc00`) and the wheel selection match the original's.
- The car-3 drive-torque rings `+0x1ac..+0x1e8 / +0x270..` hold only two stale entries in the port (the original fills all 16);
  their one known reader is FUN_0046bda0 (car-part steer animation, render only). Not on this path; noted for later.
- A hardware write watch misses alternate writes (the original integrates twice per frame, 25 ms each: the per-call x step /
  velocity = 4.63e-5 = 50 ms of `_DAT_005cea80 * kPosDtK`), so watch logs cannot count passes.
