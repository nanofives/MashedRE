# PREREG: savelink2 (a), the knob's progression side effect on the cup screens

Written before any arm runs. Exe `35a50e1f` (work/savelink2 at 449254089, the race-branch merge of work/savelink).
Merged exe gates, knob OFF (`D:/MashedWT/savelink2_data/arms_merge.txt`): G-CTRL bacfc747 / 8dd1c004 / 6f9932b6,
B f9deb9b9 / d3601fcc / 9da9a26a. These match 1ed749b6 (savelink) and the race-branch reference.

## Question

With `MASHED_SAVE_ORIGINAL=1` and the shipped save (sha256 bd18788182b2343e, magic 0xDEADBEEF, counter 0, col 4 = 2 on all
13 span rows), `Campaign_LoadProgress` (GameFlow.cpp:337) runs `ApplyProgressFromSpan` (GameFlow.cpp:111) and sets
`g_progUnlock[0..12] = 1`. Does that change anything the standalone draws on the cup screens?

## What the code says before measuring (static, cited)

- Boot restore, knob independent: exe_main.cpp:9445-9488 (`kBootRestoreSaveSpan = true`) loads `original/gamesave.bin`
  (or `MASHED_SAVE`) into the live table 0x007f0a40 via `Nav_GameStateLoadSave`. Both arms therefore have col 4 = 2 live.
- `Campaign_CurrentCup` (GameFlow.cpp:316) sets `.unlocked = live col 4 != 0 || g_progUnlock[i]`. The live term is true on
  rows 0..3 in both arms.
- Screen 6 row icons read live col 3, the movers read the mode column, and the launch gate reads `tbl[mode col + row*0xc]`.
  None of them read `g_progUnlock` (worker survey 2026-10-23, `ROW_MOVERS_RESULT.md` section 2).
- The original loads the same values. arm_C (`D:/MashedWT/savelink_data/arm_C.log`) dumped 0x007f0a40 after the menu-path
  load. All 13 rows are equal to the file span, with col 4 = 2 everywhere.

## Arms (private root D:/MashedWT/savelink_root, sandbox disabled, explicit PIDs)

Common env: `MASHED_GOTO=6`, `MASHED_SETTINGS_DEFAULTS=1`, `MASHED_DBG_DRAWSTREAM=<window>`, `MASHED_NO_DEPLOY=1`.
Root save = shipped unless stated. No `mashed_re_gamesave.bin` exists in the root.

| Arm | Knob | Root `original/gamesave.bin` | `MASHED_SAVE` (boot restore) | Nav | Window |
|---|---|---|---|---|---|
| S0  | OFF | shipped | unset | none | 200:203 |
| S0r | OFF | shipped | unset | none | 200:203 (noise floor) |
| S1  | ON  | shipped | unset | none | 200:203 |
| S1e | ON  | edited: col 4 = 0 on rows 1..12 | shipped copy | none | 200:203 |
| S0d | OFF | shipped | unset | `MASHED_NAV_KEYS=d,d,d` | late window after the last tap |
| S1d | ON  | shipped | unset | `MASHED_NAV_KEYS=d,d,d` | same window as S0d |

S1e separates the two sources. The knob reads the edited file, so `g_progUnlock` = {1,0,...}, while the boot restore
still puts the shipped table live.

## Predictions

- **P1 (log).** `progress loaded:` reads S0 `1/13 (save counter 0)`, S1 `13/13 (save counter 0)`, S1e `1/13 (save counter 0)`.
  S0d and S1d match S0 and S1.
- **P2 (draw).** `log/drawstream_re.json` is byte-identical for S0 == S0r == S1 == S1e, and for S0d == S1d. If S0 != S0r,
  the comparison falls back to `drawlist_diff.py` with GREEN required between each pair, with the noise floor stated.
- **P3 (hygiene).** The root save sha256 is unchanged after every arm (no arm writes: no race and no manual save). The
  edited save is restored to shipped after S1e.

## Decision rule

- P1 and P2 hold: the knob's all-unlocked progression has no visible effect on screens 6/7 with the shipped save. That
  state is also what the live table (and the original's) already says. The load side does not block a default flip.
  What remains are the write target (the diffing reference) and the col 5 trophy semantics `[UNCERTAIN]`.
- P1 holds and P2 fails: the knob changes the cup screen. I report the first differing draw rows with retaddrs and stop.
  No fix in this lane without Mariano.
- P1 fails: the load path is not what the code reading says. I report and stop.
