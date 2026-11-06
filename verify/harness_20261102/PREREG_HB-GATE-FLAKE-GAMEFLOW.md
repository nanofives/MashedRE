# PREREG HB-GATE-FLAKE-GAMEFLOW (2026-11-02)

Committed alone, before any arm of this step ran.

## Finding that motivates it (read from existing raw data, no arm run yet)

MATCH-PORTSTATE-GAMEFLOW run g1, arm H_PSGO (exe `fd0211e9`), moved all three G-CTRL hashes to
3cc38c6b / 75d81021 / 8c8ba1f1. The rerun g1b on the same exe gave the refs 8284be46 / ba407a16 / 31fe043e.

- `arms_g1/H_PSGO.log` lines 235-246: `R7IN byte0xa slot=0 val=0xff r7frame=4308 test=0 det_frame=5406 type=2`,
  then `R7X EXIT det_frame=5406 round 1 -> 2`, then nine more `R7IN` lines (det_frame 5407..5415, `test=0`).
  `arms_g1b/H_PSGO.log` and every other H arm of g1 have no `R7IN` line.
- `test=0` means the byte came from `g_keys[0x1c]` (DIK_RETURN), not from `MASHED_R7_INPUT_PRESS`
  (`exe_main.cpp` CookCtrlByteAFromKeyboard). The keyboard is opened `DISCL_BACKGROUND | DISCL_NONEXCLUSIVE`,
  so an Enter pressed in any window on the machine reaches the arm. The R7 cook and the powerup fire read
  (`DIK_SPACE`) have no focus or `g_det_clock` gate.
- Round 1 ends at det_frame 1098 on every H arm. From then to frame 14400 the arm sits on the round-result
  screen, about 13,300 frames, so any Enter on the machine in that window starts round 2.

## Change

- `exe_main.cpp` `DetAmbientKeys()` (called at the end of `PollKeyboard`, only when `MASHED_DETERMINISTIC`):
  - a `DET-AMBIENT-KEY` log line per frame on which the device reports a key down (cap 64). Log only.
  - `MASHED_DET_NO_AMBIENT_KEYS=1` (default OFF): zero `g_keys` after the device read.
  - `MASHED_DET_TEST_KEY=<dik>@<det_frame>[+n]` (test only): the device reports `<dik>` down for n frames.
- `run_arms.py` `base_env` sets `MASHED_DET_NO_AMBIENT_KEYS=1` for every H / B / W / LR / CR / R arm.
  New stacks `R7P`, `PSGOR7P`, `TK`, `TKD`, `NAKOFF`, `PSGOTK`.

## Predictions

| id | arm(s) | prediction |
|---|---|---|
| Q1 | fd02: H_PSGOR7P (`MASHED_R7_INPUT_PRESS=4308`, one frame) | round 1 -> 2 at det_frame 5406; step file equal to g1 H_PSGO's for its first 4,969 lines. Exact g1 hashes only if the 9 extra held frames do not matter (not predicted either way) |
| Q2 | fd02: H_R7P | same as Q1 (GAMEFLOW knob neutral on Training) |
| Q3 | fd02: H_PSGO-r1, H_PSGO-r2, H_DEF-r1 | refs 8284be46 / ba407a16 / 31fe043e, no `R7IN` line (absent an ambient key) |
| Q4 | new: H_DEF, B_DEF_K0/K1/K26 (drop knob ON via base_env) | refs 8284be46 / ba407a16 / 31fe043e, B 1d5bf4f9 x3 |
| Q5 | new: H_NAKOFF (drop knob OFF, no key) | refs |
| Q6 | new: H_TK (test Enter det_frame 5406..5415, knob OFF) | g1's hashes 3cc38c6b / 75d81021 / 8c8ba1f1 exactly, `R7X EXIT det_frame=5406` |
| Q7 | new: H_TKD (same Enter, knob ON) | refs, 10 `DET-AMBIENT-KEY ... dropped` lines, no `R7IN` |

Q6 + Q7 are the acceptance: the press alone reproduces the outlier, and the knob removes it.
