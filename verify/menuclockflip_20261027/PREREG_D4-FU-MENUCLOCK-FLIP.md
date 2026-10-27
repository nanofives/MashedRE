# PREREG D4-FU-MENUCLOCK-FLIP: `MASHED_MENU_CLOCK_FAITHFUL` default ON

Decision L-36 = 1 (Mariano 2026-10-27). Base `orch/integration` @ `725474f9c`. Evidence base
`verify/menuspeed_20261027/RESULT_D4-FU-MENUSPEED.md` (section 3, arms E0-E2).

## Change

`exe_main.cpp`: the exe frame-clock input is 0x32 every frame by default (`FUN_00493390` stores 0x32 into
`DAT_007f1000` at `0x004933d5`, `FUN_00493480` reads it at `0x00493485`). `MASHED_NO_MENU_CLOCK_FAITHFUL=1` opts out
(elapsed ms clamped to 50, the pre-flip default). `MASHED_MENU_CLOCK_FAITHFUL` is no longer read.

## Arms and predictions

| Arm | Build / env | Prediction |
|---|---|---|
| G | flipped exe, `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` | no prediction on direction. Unchanged (`5943c5f1/364350ae/52052890`, `3a03670c/e85ea6fb/53778664`) = normal merge. Moved = decided flip, new hashes recorded |
| F0 | flipped exe, default, `MASHED_GOTO=8` | = E1: dt nonzero on every call, items settle at call 16, back row at call 40, all frozen at call 90 |
| F1 | flipped exe, `MASHED_NO_MENU_CLOCK_FAITHFUL=1` | = E0: about 1 call in 3 with nonzero dt, items at 16 ticks, back row at 40 ticks, frozen at 90 ticks, call indices near 51 / 123 / 273 |

Wall ms are reported, not gated (frame pacing, RESULT_D4-FU-MENUSPEED section 3).
