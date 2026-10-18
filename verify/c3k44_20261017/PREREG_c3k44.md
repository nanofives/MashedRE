# PREREG c3k44: regime-B car 3 (U-9753) (2026-10-17)

Branch `work/c3k44` from `race/first-frame-parity` `438566c88` (default G-CTRL `1fbc77ae / 29a08810 / d3a20f4c`; (b) A 2/3,
B 2/3 = cars 1, 2, C 0/3; regime-B car 3 fails `c1#` 52 [17, 46]). Model claude-opus-5-5, account2. Exe `b_262d7ac2`, no code
change at writing. Written and committed before the arms below.

## 1. Measured so far

- U-9753's premise does not hold: from window call 44 the original's probe records no lookahead target for car 3 (empty
  `look_x` / `look_z`), so the replay's "flips" compare the port's target against an empty field.
- On o_reg_r13, car 3's `FUN_00414a70` returns 1 on calls 39..41 (mode 3, steer 178 / 227 / 255, accel 0, brake 255) and 2 on
  calls 44..57 (accel 0, brake 255, no target recorded; speed 2710 -> 1734). The port's default never calls `FUN_00414a70` (its
  `ret14a70` column is empty) and car 3 keeps accelerating (2830 -> 3049).
- The port has `FUN_00414a70` behind the U-9684 chain (`MASHED_PATHPROG` + `MASHED_TGT3`, optional `MASHED_ROUND_GRID` +
  `_ORDER`), default OFF.

## 2. Arms (`run_arms.py`, K0 / K1 / K26 = regimes A / B / C)

| tag | knobs |
|---|---|
| DEF | none |
| TGT | `MASHED_PATHPROG`, `MASHED_TGT3` |
| TGTRG | TGT + `MASHED_ROUND_GRID`, `MASHED_ROUND_GRID_ORDER` |
| TGTT / TGTRGT (K1) | the same + `MASHED_TGT3_TRACE` / `MASHED_PATHPROG_TRACE` |

## 3. Gates and expectations

| gate | rule | expectation |
|---|---|---|
| G-RET | car 3 window `ret14a70` on K1: 1 on 39..41, 2 on 44..57 (o_reg_r13) | not predicted |
| D3 (b) | default-ON candidate only if no car that passes on DEF turns to FAIL (A cars 2, 3; B cars 1, 2) | not predicted |
| D2 | only for a candidate: 3 runs alone, the d2close bounds | the D2 recipe is solo; AI knobs should not reach it |
| 13 tracks / G-CTRL | only for a candidate | 0 RED |
