# PREREG f5flip: u9774's five knobs default ON (2026-10-20)

Branch `work/f5flip` from `race/first-frame-parity` `059746f9e` (u9774 merged). Mariano's decision (2026-10-20, relayed by the
orchestrator). Model claude-opus-5-5, account2. Exe `MASHED_NO_DEPLOY=1`, root `D:\MashedWT\u9756_root`, 13 tracks on
`D:\MashedWT\u9749_root13`. Written before any arm.

## 1. Flip

The five u9774 knobs join the FAITH seed list in `Core/FaithDefaults.cpp` with value `1`: `MASHED_A6A_SPIN_ORDER`,
`MASHED_TER_SPIN_F32`, `MASHED_FIXUP_F32`, `MASHED_FIXUP_SUM_ORDER`, `MASHED_FIXUP_SLIDE_ORDER`. Each keeps its own reader; the seed
skips a knob when `MASHED_NO_<name>=1` is set or the variable is already set, and seeds nothing under `MASHED_NO_FAITH=1`.
`MASHED_COLL_LUT` and `MASHED_A5_RING_ORDER` stay OFF.

## 2. Gates (any failure means no flip; report)

| gate | rule |
|---|---|
| G-CTRL x2 | `H_DEF`, `H_DEF-2` = `df5ee50e / a90bbb16 / b3cd0e5f` (u9774 `H_F5`) |
| G-OPT5 | the five opt-outs together (`N5`): `H_N5` = `826277f2 / 250c1569 / 2c8164b2`; `B_N5_K0/K1/K26` = `2c24bca3 / 9be204c1 / 97cac7c8` |
| G-OPTALL | `MASHED_NO_FAITH=1`: `H_NF` = `66d51720 / ca4284be / 1193bbd1` |
| G-B | `B_DEF_K0/K1/K26` = u9774 F5 `c165d2e5 / c01e164a / 2e937af7`; (b) A 2/3 (cars 2, 3), B 2/3 (cars 1, 3), C 1/3 (car 1) |
| G-D2 | three runs alone, `score_d2_v2.py port`, 8-run band: PASS 3/3 |
| G-13 | 13 tracks: 0 RED, row changes against the faithflip default cited |
