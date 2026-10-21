# RESULT rbflip: MASHED_A5_RUBBER_LIVE default ON - FLIPPED (2026-10-21)

Branch `work/rbflip` from `b7e366526` (u9777 merged). PREREG `PREREG_rbflip.md` (`1ce7bee17`, with the seed), committed before
any arm. Mariano's decision (2026-10-21). Exe `595c774b` (`MASHED_NO_DEPLOY=1`), private roots `D:\MashedWT\rbflip_root` /
`rbflip_root13`. Evidence in `D:\MashedWT\rbflip_data` (`MOVED_OFF_GIT.txt`).

| gate | measured | verdict |
|---|---|---|
| G-CTRL x2 | `H_DEF` = `H_DEF-2` = `15594ec8 / 7285d708 / 26a6e4ad` (= u9777 knob on) | as predicted |
| G-OPT | `MASHED_NO_A5_RUBBER_LIVE=1`: `H_NR` `b9fb3f25 / 8740221b / b2e4b612`; `B_NR` `c4c2ae26 / b0f27763 / 6c35e33b` (= cpcflip default) | PASS |
| G-NOFAITH | `MASHED_NO_FAITH=1`: `H_NF` `66d51720 / ca4284be / 1193bbd1` (unchanged) | PASS |
| G-B | `B_DEF_K0/K1/K26` `7ab70c5f / 15262e4a / d1b4bc64`; `flag_a368` 0 / 0,1 / 0,2; (b) A 3/3, B 2/3 (car 2 FAIL, same values as cpcflip), C 3/3. Against cpcflip: A car 3 FAIL -> PASS, no car PASS -> FAIL | PASS |
| G-D2 | 3 runs alone: 0.1900 x3 / 0.2550 x3 / 1965.35, 1965.35, 1965.67, H1 398/400, 1630..1632 lines | PASS 3/3 |
| G-13 | 39 frames, 873 rows, all cited, 0 RED, instrument gates PASS; `score_out.txt` identical to u9777's knob-on run (`tracks_rl`) | PASS |

Every measured value equals the PREREG prediction.

One deviation in procedure, none in results: the first G-CTRL launch had a 10-minute tool limit, so I stopped it. Stopping the
shell left its Python runner alive, and that runner went on to `H_DEF-2` while the rerun ran `H_DEF` on the same root (shared
`player_trace.log`, 50078 lines). I killed that runner chain and its game by PID (4776, 40484, 68340, 53724) and the first game
(60132). `RUNLOG.txt` marks every H_ line before the NOTE line as discarded. All four H_ arms were rerun clean, one at a time,
and the gate values above come from those reruns. The B arms ran before this and are unaffected.

## New default

FAITH stack (38 knobs) + u9774's five + `WCS_TRINORMAL` + `WCS_COMBINE_PC24` + `A5_RUBBER_LIVE`. G-CTRL `15594ec8 / 7285d708 /
26a6e4ad`; opt-out `MASHED_NO_A5_RUBBER_LIVE=1` gives the cpcflip default; `MASHED_NO_FAITH=1` seeds nothing. B dumps `7ab70c5f
/ 15262e4a / d1b4bc64`. Failing car: B car 2.
