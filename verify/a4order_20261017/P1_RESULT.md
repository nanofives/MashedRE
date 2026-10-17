# a4order part 1: MASHED_AI_COUNTDOWN_CLOCK default ON (2026-10-17)

User decision 2026-10-17: the AI countdown clock (`FUN_0040fc00` `0x0040fe4f..0x0040fe5e`, U-9744) is default ON, opt-out
`MASHED_NO_AI_COUNTDOWN_CLOCK=1`. The user accepted that regime A trades car 2 for car 3 (A stays 1/3).

Exe `fa36a37e` (built from `3e23c80b1` + this change, `MASHED_NO_DEPLOY=1`, snapshot `D:\MashedWT\a4order_data\exe\p1_fa36a37e.exe`),
private root `D:\MashedWT\a4order_root`, `run_arms.py` G-CTRL recipe (`arms/RUNLOG.txt`; outputs moved to `D:\MashedWT\a4order_data\p1`):

| run | step | gates | ptrace | expected |
|---|---|---|---|---|
| `H_DEF-1` (default) | `490c4b2d` | `1fdd0ea6` | `46088ac4` | d3bB2 arm CC `490c4b2d / 1fdd0ea6 / 46088ac4` |
| `H_DEF-2` (default) | `490c4b2d` | `1fdd0ea6` | `46088ac4` | same |
| `H_NOCC-1` (`MASHED_NO_AI_COUNTDOWN_CLOCK=1`) | `4c81db89` | `aa03f7ae` | `7d3af534` | the dflip default `4c81db89 / aa03f7ae / 7d3af534` |

The rest of the arm's evidence is d3bB2's CC arm (`verify/d3bB2_20261017/RESULT_d3bB2.md` sections 5-6): 13 tracks 867 rows
0 RED, D2 3/3 in bounds, D3 (b) A 1/3, B 2/3, C 0/3. Trackers: `apply_trackers_p1.py` (U-9744 note, hooks.csv note on
`0040fc00`, one CHANGELOG line), no C-level change.
