# RESULT D3-CF-A5TIMER-FLIP: MASHED_A5_RACE_TIMER_RUN default ON (decision L-23 = B, 2026-10-26)

Branch `orch/D3-CF-A5TIMER-FLIP` from `orch/integration` `86bdd5121` (`git merge-base --is-ancestor` checked). Exe `3ecb144b`
(`MASHED_NO_DEPLOY=1`, `original\mashed_re_dev.asi` not redeployed), private root `D:\MashedWT\D3-CF-A5TIMER-FLIP_root`
(robocopy of `original\`). Knob evidence: `verify/a5timer_20261026/RESULT_D3-CF-A5TIMER.md` (orch/D3-CF-A5TIMER).

## 0. Verdict

GREEN. `DAT_007f0ff8` now runs through the pre-GO ticks by default (round-1 seed 35200 at `0x0040e5a2`, the `0x0040fe64` add
on every stand-in tick, the `0x00418598` zero on an AI race's state-5 window). `MASHED_NO_A5_RACE_TIMER_RUN=1` opts out and
gives the pre-flip default (the D2 control). `MASHED_A5_RACE_TIMER_ADD` was already deleted by D3-CF-A5TIMER (`8602c36ad`).
No `getenv` of it remains in `mashedmod/src` (two comments name it as history).

G-CTRL, B and the 13 tracks equal the refs. The D2 default moves as predicted, in band 3/3.

## 1. Gates (default, no knob set)

| gate | measured | ref | verdict |
|---|---|---|---|
| G-CTRL `H_DEF` | `5943c5f1 / b493262b / 09551e6a` | merge_train.json `gctrl.ref` | PASS |
| B `B_DEF_K0 / K1 / K26` | `3a03670c / 2516d903 / 53778664` | merge_train.json `b.ref` | PASS |
| 13 tracks (`cites_rres.json`) | 39 rows identical to the reference (byte-equal scorer output), INSTRUMENT GATES PASS, 13/13 first attempt | `verify/rpct_20261022/tracks_flip_score.txt` | PASS |
| D2 `D2_DEF_1..3` | see section 2 | d2rebase band | PASS 3/3 |

## 2. D2 (solo window, `mashed_lock.py run --solo`)

| arm | motion_diag lines / sha8 | slip1500 | slip2000 | drvmed | H1 |
|---|---|---|---|---|---|
| `D2_DEF_1` | 1629 / `80dfb162` | 0.1874 in | 0.2488 in | 1928.56 in | 398/400 |
| `D2_DEF_2` | 1629 / `80dfb162` | 0.1874 in | 0.2488 in | 1928.56 in | 398/400 |
| `D2_DEF_3` | 1628 / `21aa4ea7` | 0.1874 in | 0.2488 in | 1930.04 in | 398/400 |
| `D2_NORN_1` (opt-out) | 1630 / `1c1d5b1b` | 0.1900 in | 0.2550 in | 1965.35 in | 398/400 |

**Measured D2 default: 0.1874 / 0.2488 / 1928.56, 1928.56, 1930.04.** Every arm is over 1600 lines, so no re-run.

- The three default arms are byte-identical to D3-CF-A5TIMER's knob-ON arms: `80dfb162` = `D2_RNT_1`, `21aa4ea7` =
  `D2_RNT_2 / 3` (`D:\MashedWT\D3-CF-A5TIMER_data\d2arms.txt`). The flip changed nothing but the default.
- The opt-out arm is byte-identical to the pre-flip default: `1c1d5b1b` = D3-CF-A5TIMER's `D2_DEF_2 / 3`, 0.1900 / 0.2550 /
  1965.35. The D2 control is intact.
- Solo window: the conductor ran no other build or boot during the D2 arms (step brief). `mashed_lock.py run --solo` drained
  and held the machine lock for the four arms. No wait, no deadlock.

## 3. The change

- `Ai/AiStandalone.cpp` `Ai_RaceTimerRunEnabled`: was `getenv("MASHED_A5_RACE_TIMER_RUN")` set and not `0`; now ON unless
  `MASHED_NO_A5_RACE_TIMER_RUN` is set and not `0` (the d2ref opt-out pattern). The `MASHED_A5_RACE_TIMER_RUN` variable no
  longer does anything.
- Comment-only: the knob header in `AiStandalone.cpp` / `AiStandalone.h` and two `[a5timer]` markers in
  `D3d9Render/TrackRenderer.cpp` now say default ON since L-23.
- No tracker change. U-9790 was resolved by D3-CF-A5TIMER. U-9791 (tick of the last state-5 zero) stays open: the AI-race
  zero placement it covers is unchanged and its gates (G-CTRL, B, tracks) equal the refs.

## 4. Files

Committed here: this RESULT, `run_arms.py` (a5timer's runner, paths changed, `RN` empty, `NORN` / `NORNT` added), `msdeq.py`
(copy, unused this run), `gates.txt` / `d2arms.txt` / `tracks_run.txt` (runner stdout), `score_d2.txt` (`score_d2_v2.py port`),
`tracks_score.txt` (13-track table), `raw_sha256.txt`.

Raw data off git in `D:\MashedWT\D3-CF-A5TIMER-FLIP_data` (`arms\`: gate step / gates / ptrace, D2 motion_diag, logs,
RUNLOG; `tracks\`: replay draws and diffs). SHA-256 of each cited raw file in `raw_sha256.txt`.

## 5. Next

Merge alone with the re-gate. merge_train.json refs stay the same (G-CTRL and B did not move). The D2 default to carry forward
is 0.1874 / 0.2488 / ~1929. D3-CF-COLLLUT (L-18) can re-measure `MASHED_COLL_LUT` on this default. The faithful-path value
D3-CF-A5TIMER expects there is 0.1942 / 0.2492 / 1896.74 (`D2_CLSF4RN_1`).
