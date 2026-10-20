# PREREG cpcflip: MASHED_WCS_COMBINE_PC24 default ON (2026-10-20)

Branch `work/cpcflip` from `race/first-frame-parity` `4fe71f50f` (u9776 merged). Mariano's decision (2026-10-20, this session):
flip `MASHED_WCS_COMBINE_PC24` default ON, accepting (b) A car 3's loss; overrides the "no passing car may fail" rule for this
flip only. Model claude-opus-5-5, account2. Exe `MASHED_NO_DEPLOY=1`, root `D:\MashedWT\u9756_root`, 13 tracks on
`D:\MashedWT\u9749_root13`. Written before any arm.

Flip: `WCS_COMBINE_PC24` joins the FAITH seed list (`Core/FaithDefaults.cpp`, value 1); opt-out `MASHED_NO_WCS_COMBINE_PC24=1`.

| gate | rule |
|---|---|
| G-CTRL x2 | `H_DEF` = `H_DEF-2`, new triple reported |
| G-OPT | `MASHED_NO_WCS_COMBINE_PC24=1`: `H_NC` = trinflip `91cc1c0b / 2d77a5d7 / ab6c9329`; `B_NC_K0/K1/K26` = `28a76cf2 / 76a0b4cd / 040ce640` |
| G-B | `B_DEF_K0/K1/K26` = u9776 `B_TC` `c4c2ae26 / b0f27763 / 6c35e33b`; (b) A 2/3 (cars 1, 2), B 2/3 (cars 1, 3), C 3/3 |
| G-D2 | three runs alone, `score_d2_v2.py port`, 8-run band: PASS 3/3 |
| G-13 | 13 tracks with `cites_rres.json` (C27b included): 0 RED, row changes against the trinflip run cited |

If D2 or 13 tracks fail, do not flip.
