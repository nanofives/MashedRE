# RESULT fflip: canonical recipe F, MASHED_MODE_LIVE + MASHED_SLOT_TYPE_LIVE default ON - FLIPPED (2026-10-21)

Branch `work/fflip` from `4623046c6` (work/brecipe merged). PREREG `PREREG_fflip.md` (`2677f3f79`, with the seed and the runner
swap), committed before any arm. Mariano's decision (2026-10-21). Exe `492e9d9d` (`MASHED_NO_DEPLOY=1`, copy in
`D:\MashedWT\fflip_data`), private roots `D:\MashedWT\fflip_root` / `fflip_root13`. Evidence in `D:\MashedWT\fflip_data`
(`MOVED_OFF_GIT.txt`, `arms\RUNLOG.txt`, copied here as `RUNLOG.txt`).

## Gate table

| gate | arm | measured | verdict |
|---|---|---|---|
| G-CTRL x2 | `H_DEF`, `H_DEF-2` | `ce087436 / b0a2973b / c4f7a000`, both equal (= brecipe F) | as predicted |
| G-OPT-F | `H_NOK` (recipe F + both opt-outs) | `2e01286e / c634bbac / 26a6e4ad` (= brecipe F0M) | as predicted |
| G-OLD | `H_OLDNOK` (rule 4, no game mode, both opt-outs) | `15594ec8 / 7285d708 / 26a6e4ad` (= the rbflip default) | PASS |
| G-NOFAITH | `H_NF` (recipe F + `MASHED_NO_FAITH=1`) | `997f4117 / e6288905 / a63bdcfa` | gates moved as predicted; step / ptrace were not predicted, both moved (below) |
| G-OLDNF | `H_OLDNF` (rule 4, no game mode, `MASHED_NO_FAITH=1`) | `66d51720 / ca4284be / 1193bbd1` | as predicted |
| G-B | `B_DEF_K0/K1/K26` | `7ab70c5f / 15262e4a / d1b4bc64`; flag_a368 0 / 0,1 / 0,2 | as predicted |
| G-B-IL | `B_IL_K0/K1/K26` | `9bd86ffe / 433abcd2 / 1a1ff727` | as predicted |
| (b) | `vsum.py` | plain A 3/3, B 2/3 (car 2 FAIL, same values as rbflip), C 3/3; interleave 9/9. No car PASS -> FAIL against rbflip | PASS |
| G-D2 | `D2_DEF_1..3`, alone | slip1500 0.1900 x3 / slip2000 0.2550, 0.2550, 0.2549 / drvmed 1964.25, 1965.67, 1966.66, H1 398/400, 1629..1635 lines; all in band | PASS 3/3 |
| G-13 | `run13.py run` on recipe F | 39 frames, 873 rows, all cited, 0 RED, instrument gates PASS; `tracks_def_score.txt` identical to brecipe's `tracks_F_score.txt` | PASS |

Every measured value equals its PREREG prediction. The one unpredicted pair (NO_FAITH step / ptrace on rule 0) is explained below.
Every arm ran once on the first attempt, no crash, no re-run, no orphan process.

## The NO_FAITH arm

`RULE-EVAL` lines (`arms\H_NF.log` / `arms\H_OLDNF.log`): the two arms agree through round 1 (scores `4,7,8,5`). After round 2
(scores `3,5,10,6`) rule 4 returns `r=3`: car 2's metric reached 3.0, `UpdateFinishOrder` put it in finish slot 0, and
`EvaluateResult` case 4 makes the first finisher + 1 the winner, so the match ends at 80.1 s. Rule 0 takes the `default` case,
no score passes 11, and a third round runs (scores `4,4,12,4`, match over at 89.7 s by FUN_00410510's score > 11). This is the
rule-gated consumer firing on the NO_FAITH trajectory. No gate scorer reads it.

## The rule-gated consumer (PREREG section 2)

`UpdateFinishOrder` (0x004177b0 fragment, `Race/RuleEngine.cpp:22`) records finishers on rules 4/7/8/9 only. Its only reader is
`EvaluateResult` (0x00410510, cases 4/8/9), which decides the match and round ends. No gate or score reads either one: (b)
(`ai_ctrl_window.py` + `b_envelopes.json`), D2 (`score_d2_v2.py`, frontend route, no rule), the 13-track scorer, and the G-CTRL
trio is a hash identity. On the new default rule 0 bypasses it. The NO_FAITH arm above is the only arm where it changed a result.

## D2 route

On the frontend route (`MASHED_GOTO=6`) `MATCH-SEED` logs `rule=0 mode_e9fc=3 fd0=0 host_mode=3 host_fd0=0`: the seeded knobs read
the launch mode 3 and fd0 0, the values the stand-ins returned, so the D2 recipe reads the same AI-host values as before.

## Runner edits (commit `2677f3f79`)

`MASHED_ROUND_RULE=4` -> `MASHED_ROUND_RULE=0 MASHED_GAME_MODE=10` in `verify/d1r2_20261016/run_gctrl.py:40`, `run_replay.py:35`,
`verify/d3_gatefire_20261008/run_h4.ps1:127`, `verify/fe_residuals_20261016/run_h4_wt.ps1:125`, `run_fx.py:12`,
`verify/rsky_20261016/run13.py:79`, and this lane's `run_arms.py` `base_env`. 118 dated lane files keep rule 4 as provenance
(list in PREREG section 1).

## New default

FAITH stack (38 knobs) + u9774's five + `WCS_TRINORMAL` + `WCS_COMBINE_PC24` + `A5_RUBBER_LIVE` + `MODE_LIVE` + `SLOT_TYPE_LIVE`,
on the canonical recipe F (`MASHED_ROUND_RULE=0 MASHED_GAME_MODE=10`). G-CTRL `ce087436 / b0a2973b / c4f7a000`. Opt-outs
`MASHED_NO_MODE_LIVE=1 MASHED_NO_SLOT_TYPE_LIVE=1` on F give `2e01286e / c634bbac / 26a6e4ad`; the old recipe with both opt-outs
gives the rbflip default `15594ec8 / 7285d708 / 26a6e4ad`. `MASHED_NO_FAITH=1` on F gives `997f4117 / e6288905 / a63bdcfa`, on the
old recipe `66d51720 / ca4284be / 1193bbd1`. B dumps unchanged `7ab70c5f / 15262e4a / d1b4bc64`. Failing car: B car 2 (passes
under `MASHED_SUBSTEP_INTERLEAVE`).
