# RESULT cpcflip: MASHED_WCS_COMBINE_PC24 default ON - FLIPPED (2026-10-20)

Branch `work/cpcflip` from `4fe71f50f` (u9776 merged). PREREG `PREREG_cpcflip.md` (`f83c991ae`, with the seed), committed before
any arm. Mariano's decision (this session): flip, accepting (b) A car 3's loss. Exe `91ccc6bf` (`MASHED_NO_DEPLOY=1`). Evidence
in `D:\MashedWT\cpcflip_data` (`MOVED_OFF_GIT.txt`).

| gate | measured | verdict |
|---|---|---|
| G-CTRL x2 | `H_DEF` = `H_DEF-2` = `b9fb3f25 / 8740221b / b2e4b612` | reported |
| G-OPT | `MASHED_NO_WCS_COMBINE_PC24=1`: `H_NC` `91cc1c0b / 2d77a5d7 / ab6c9329`; `B_NC` `28a76cf2 / 76a0b4cd / 040ce640` (= trinflip) | PASS |
| G-B | `B_DEF` `c4c2ae26 / b0f27763 / 6c35e33b` (= u9776 `B_TC`); (b) A 2/3 (cars 1, 2), B 2/3 (cars 1, 3), C 3/3. Against trinflip: A car 1, A car 2, C car 2 FAIL -> PASS; A car 3 PASS -> FAIL (`c1_distinct 71 [21,70]`, `steer_distinct 97 [33,96]`) | as expected |
| G-D2 | 3 runs alone: 0.1900 x3 / 0.2550 x3 / 1964.25, 1964.25, 1963.95, H1 398/400 | PASS 3/3 |
| G-13 | 39 frames, 868 rows, all cited, 0 RED, instrument gates PASS (`cites_rres.json` with C27b) | PASS |

One K26 run (the first after the build, `B_DEF_K26`) wrote a 1784-line step dump that is a byte-identical prefix of the
1796-line one; the three repeats give `6c35e33b`. A dump-length artifact, not a trajectory difference.

## New default

FAITH stack (38 knobs) + u9774's five + `WCS_TRINORMAL` + `WCS_COMBINE_PC24`. G-CTRL `b9fb3f25 / 8740221b / b2e4b612`; opt-out
`MASHED_NO_WCS_COMBINE_PC24=1` gives the trinflip default; `MASHED_NO_FAITH=1` seeds nothing. B dumps `c4c2ae26 / b0f27763 /
6c35e33b`. Failing cars: A car 3, B car 2.
