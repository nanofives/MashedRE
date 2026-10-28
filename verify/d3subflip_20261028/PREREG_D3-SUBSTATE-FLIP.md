# PREREG: D3-SUBSTATE-FLIP amendment 1 (the pre-GO AI clock under MASHED_SUBSTATE)

Date 2026-10-28. Branch `orch/D3-SUBSTATE-FLIP` from `orch/integration` 327fd83c6. Committed alone, before any arm of the
change below. Decision: L-45 / D-11073 (flip MASHED_SUBSTATE default ON as one decided flip, gates re-scored).

## 0. What the first flip build showed (exe b55f9419, flip only: SUBSTATE + P3_RESET_PAIR seeded in Core/FaithDefaults.cpp)

- Opt-out `MASHED_NO_SUBSTATE=1 MASHED_NO_P3_RESET_PAIR=1` (stack NOSUB) reproduces every ref: G-CTRL
  5943c5f1/364350ae/52052890, B 3a03670c/9fcdbd3a/53778664.
- Flip ON: G-CTRL 72433c0f/1e5a1c92/d1cb07bd, B 68d6a0b8 on K0, K1 and K26 (964 lines, 321 dump frames vs 1018).
  (b) 2/3 on each arm (car 1 A FAIL: c0_distinct 10, steer_distinct 32, abs_steer_median 0). (e) launch
  1425.7/2052.0/2054.5 vs the original's 1425.7/2052.5/2055.0 (o_reg_r1, r13, r19 all give the latter).
- Launch rows, car 2, first 5 racing ticks: original o_reg_r1 c0 = 0/11/23/35/46, rec_9e4 at tick 3 = 861.1806.
  NOSUB gives exactly that. Flip ON gives c0 = 0/0/0/0/48 and 861.3292.
- clk_0ff4 (`DAT_007f0ff4`) at the first racing row: original 42800 (r1) / 43100 (r13), NOSUB 9050, flip ON 0.

## 1. Cause (static)

Under MASHED_SUBSTATE the race frame returns from the sub-state block (TrackRenderer.cpp `if (SubState_Enabled() &&
countdown_ > 0.f)`) before the stand-in's pre-GO clock add (`Ai_AdvanceCountdownClock`, the MASHED_AI_COUNTDOWN_CLOCK
port of `0x0040fe5e`). Inside the block, `Ai_AdvanceClock` stores only `DAT_007f1008` while MASHED_AI_CLOCK_LATE is
default ON, and the late add (`Ai_AdvanceClockLate`) runs only on race frames. So `DAT_007f0ff4` reaches GO at 0. The
original adds the tick budget at the end of every FUN_0040fc00 except sub-state 7 (`0x0040fe46` / `0x0040fe5e`), so it runs
through phase 3 and states 4 / 5. FUN_00416250's fresh-steer arm needs `DAT_007f0ff4 - [0x0089a4f0 + v*0x74] >= 0xc8`
(`0x00416608..0x00416621`), so with the clock at 0 it takes the decay arm and writes c0 = 0: the same symptom
MASHED_AI_COUNTDOWN_CLOCK fixed for the stand-in (AiStandalone.cpp comment at `Ai_CountdownClockEnabled`).

## 2. Change

In the sub-state block, on every frame that runs the FUN_0040fc00 partials (the `subClock` branch, states 3..6), add
`units` to `DAT_007f0ff4` when `Ai_ClockLateEnabled()` and `Ai_CountdownClockEnabled()` (with CLOCK_LATE off,
`Ai_AdvanceClock` already adds it). `DAT_007f0ff8` is unchanged (`Ai_RaceTimerCountdownTick` already covers it).
Only reachable with MASHED_SUBSTATE set. `MASHED_NO_AI_COUNTDOWN_CLOCK=1` removes it (and then gives the b55f9419 flip-ON
behaviour exactly).

## 3. Predictions (new exe)

| id | arm | prediction |
|---|---|---|
| P1 | `B_DEF_K0` (flip ON) | car 1..3 first 5 racing ticks: c0, rec_9e4, own_x, own_z, rec_b0c equal o_reg_r1's printed values; (e) launch 1425.7/2052.5/2055.0, ft_median_m0 2551.6/2052.5/2278.1 |
| P2 | `B_DEF_K0` | clk_0ff4 at the first racing row > 0 |
| P3 | `H_NOSUB`, `B_NOSUB_K0/K1/K26` | = the refs (5943c5f1/364350ae/52052890, 3a03670c/9fcdbd3a/53778664) |
| P4 | `B_NCC_K0` (flip ON + MASHED_NO_AI_COUNTDOWN_CLOCK=1) | step = 68d6a0b8 (the b55f9419 flip-ON hash) |
| P5 | `H_DEF`, `B_DEF_K*` run twice | identical hashes both runs (MASHED_DETERMINISTIC; phase 3's length is set by the port's dt-fed camera ticks, not QPC) |

Reported, not predicted: (b) at the 1200-frame B recipe and at `MASHED_DET_FRAMES=1897` (stack L697, the 697-frame
pre-race growth measured in section 0), D2 x3 (score_d2_v2.py, 8-run band), the 13-track table vs
`verify/hbfullcheck_20261028/tracks_score.txt`.

If P1 fails, or P3 / P4 fail, the flip is not committed and the step reports DECISION.
