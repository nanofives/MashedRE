# PREREG c2k93: car 2 from window call 93 against o_reg_r13 (2026-10-17)

Branch `work/c2k93` from `work/u9745ov` `bef7c8882`. Model claude-opus-5-5, account2. Exe `c1_e1b6a390` (no new code). Written
and committed before the scoring arms below; the exploration arms already run are listed in section 1.

## 1. Measured so far (exploration, `arms/RUNLOG.txt`)

- On the SACL stack car 2 equals o_reg_r13 on calls 57..92; at 93 the lookahead target differs (original (-1.002, -11.643),
  port (-1.056, -12.016)) and from there both cycle over three consecutive spline points, the port's set one point on.
- The lookahead replay (`LR_` / `LR_M5`, the original's rows' (spline, v, own_x, own_z) through the port's `SplineLookahead`):
  at k = 93..99 the LUT form gives the original's points to 3 decimals, the exact form gives the port's. On mode-0 rows of the
  (b) window (`LOOK_REPLAY_FLIPS_MODE0.txt`), choice flips (> 0.01) for car 2: r1 LUT 0 / exact 4, r13 LUT 4 / exact 24, r19 LUT 6 /
  exact 9; car 1 LUT 1..3 / exact 38..48; car 3 LUT 15..53 / exact 44..74 (a contiguous run from call 44 under both forms).
- Live, the LUT form alone (`B_SACL5_K1`) still flips at 93: car 2's position is 2.2e-3 off by then, and the replay shows the
  decision sits within the LUT/exact difference. The original's speed refresh (`0x004710a1..0x004710da` with `0x004c3ac0`'s
  arithmetic, the port's `MASHED_D2LAT_REFRESH`) cuts that drift to 0.45e-3 at k = 92 (`B_SACLR_K1`); refresh + LUT (`B_SACLR5_K1`)
  takes car 2's command mismatches against r13 from 116 to 96 of 220.

## 2. Scoring arms (on the current default + `MASHED_AI_CLOCK_LATE`)

Tags: `CL` (u9745ov), `CLR` = CL + `MASHED_D2LAT_REFRESH`, `CL5` = CL + `MASHED_AI_LUT=5`, `CLR5` = CL + both.

| gate | measure | pass rule | expectation |
|---|---|---|---|
| D3 (b) | `B_{CLR,CL5,CLR5}_K{0,1,26}` against `B_CL_*` | default-ON candidate only if no car that passes on CL turns to FAIL | not predicted (dflip / d3bB2 saw LUT=5 trade A car 2 / car 3) |
| D2 | `D2_CLR_1..3` and `D2_CLR5_1..3`, each alone | the d2close bounds, H1 398/400, >= 1600 lines | not predicted: d2settle found the exact-form refresh moved D2 out of bounds; the LUT form has not been scored on the current default |
| 13 tracks | only if a stack passes both rows above | 0 RED | not predicted |
