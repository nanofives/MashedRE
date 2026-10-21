# PREREG rbflip: MASHED_A5_RUBBER_LIVE default ON (2026-10-21)

Branch `work/rbflip` from `race/first-frame-parity` `b7e366526` (u9777 merged). Mariano's decision (2026-10-21): flip
`MASHED_A5_RUBBER_LIVE` default ON. Model claude-opus-5-5, account2. Exe `MASHED_NO_DEPLOY=1`, root `D:\MashedWT\rbflip_root`,
13 tracks on `D:\MashedWT\rbflip_root13`, evidence in `D:\MashedWT\rbflip_data`. Written before any arm.

Flip: `A5_RUBBER_LIVE` joins the FAITH seed list (`Core/FaithDefaults.cpp`, value 1); opt-out `MASHED_NO_A5_RUBBER_LIVE=1`.

Predictions (from u9777's knob-on arms, `verify/u9777_20261020/RESULT_u9777.md`):

| gate | rule |
|---|---|
| G-CTRL x2 | `H_DEF` = `H_DEF-2` = `15594ec8 / 7285d708 / 26a6e4ad` |
| G-OPT | `MASHED_NO_A5_RUBBER_LIVE=1`: `H_NR` = cpcflip default `b9fb3f25 / 8740221b / b2e4b612` |
| G-NOFAITH | `MASHED_NO_FAITH=1`: `H_NF` = `66d51720 / ca4284be / 1193bbd1` (unchanged, the knob sits in the FAITH seed list) |
| G-B | `B_DEF_K0/K1/K26` = `7ab70c5f / 15262e4a / d1b4bc64`; `flag_a368` per K 0 / 0,1 / 0,2; (b) A 3/3, B 2/3 (car 2 FAIL), C 3/3 |
| G-D2 | three runs alone, `verify/d2rebase_20261019/score_d2_v2.py` whole log, 8-run band: PASS 3/3 |
| G-13 | 13 tracks (`verify/rsky_20261016/run13.py`, cites `verify/rres_20261016/cites_rres.json`): 873 rows, 0 RED |

Default-ON rule: no car passing on the cpcflip default (A cars 1, 2; B cars 1, 3; C cars 1, 2, 3) may fail, D2 3/3, 13 tracks
0 RED, G-OPT equal to the old default. If any gate fails, stop and report; the flip is not committed as a pass.
