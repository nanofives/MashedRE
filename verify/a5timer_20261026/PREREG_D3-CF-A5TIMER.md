# PREREG D3-CF-A5TIMER: the A5 race timer runs through the pre-GO ticks (U-9790, L-17 (a), 2026-10-26)

Branch `orch/D3-CF-A5TIMER` from `orch/integration` `d55d89c48`. Committed alone, before any arm that sets the new knob.

## 1. What is already measured (before this file)

- U-9775 (`verify/u9775_20261025/RESULT_D3-CF-U9775.md`, merged at `e49198357`): in the original's solo D2 captures
  `DAT_007f0ff8` = 50 * (a0 - 1) + 50 * k at the entry of the call that produces record k + 1, a0 = the player's first moving
  call (877..896 over 13 captures). A5 `FUN_0046ddb0` reads it (`FILD [0x007f0ff8]` at `0x0046e0a9`) for its grip ramp
  (`_DAT_005cea7c` 60000, `_DAT_005cea74` -1/30000, `_DAT_005cea78` -30000). The port's timer read 0 at its first moving call.
  `MASHED_A5_RACE_TIMER_ADD=44200` (added to the copy A5 reads) put the port's onset on the trunk (a0 887): D2 records
  bit-exact vs orig_solo3 / orig_solo4 / o_w2 on the faithful stack CLSF4, and on the default `D2_DEFA_1..3` scored
  0.1874 / 0.2488 / 1928.43, 1928.43, 1930.04.
- Writers of `DAT_007f0ff8` in the original, from the plates and the port's citations: round 1's phase-3 entry zeroes it
  (`FUN_004102f0` first frame -> `FUN_0040e590`, store `0x0040e5a2`); `FUN_0040fc00` adds the tick budget at the end of every
  tick except race sub-state 7 (`0x0040fe46`, add `0x0040fe64`); `FUN_00418560` zeroes it when `FUN_0040e350()` (=
  `DAT_0063ba8c`, plate `re/analysis/ai_update/0x0040e350.md`) is 5 (`0x00418593`, store `0x00418598`), once per AI car per
  state-5 tick. So an AI race reaches GO at one tick's budget and a solo race (no AI car) carries the whole pre-move count.
- The exe's default countdown is a 180-tick stand-in (`countdown_ = 3.0f`, 1/60 s fixed step, GO on the 181st tick) with no
  phase 3, and nothing advanced `DAT_007f0ff8` on it (only the race-frame add, `Ai_AdvanceClockLate`).

## 2. The knob

`MASHED_A5_RACE_TIMER_RUN=1` (default OFF). With it set:

1. `StartRound` on round 1 seeds `DAT_007f0ff8` = `kA5RtPhase3Seed` = 44200 - 180 * 50 = 35200 (the `0x0040e5a2` zero plus the
   phase-3 ticks the stand-in does not run, calibrated on the trunk a0 887). Rounds 2+ are not touched (the original's round 2
   skips phase 3 at `0x0041026d`).
2. Every stand-in tick that returns before GO adds the budget (`0x0040fe64`), after zeroing first when AI cars exist and the
   tick is in the stand-in's state-5 window (`countdown_ <= 113 / 60`, the HUD state convention) (`0x00418598`).
3. Under `MASHED_SUBSTATE` the add is made where `Ai_AdvanceClock` did not make it (late clock ON or no AI spline).

`MASHED_A5_RACE_TIMER_ADD` is deleted in the same change (L-17). Expected in the D2 arm (solo): the timer reads
35200 + 180 * 50 = 44200 on the GO frame, so A5 sees the same values as u9775's ADD=44200 arm.

## 3. Arms (exe = this branch, `MASHED_NO_DEPLOY=1`, root `D:\MashedWT\D3-CF-A5TIMER_root`, D2 arms alone under the solo lock)

| arm | stack | gate |
|---|---|---|
| G-NOOP | `H_DEF`, `B_DEF_K0 / K1 / K26`, knob unset | equal to `merge_train.json` refs (G-CTRL `5943c5f1 / b493262b / 09551e6a`, B `3a03670c / 2516d903 / 53778664`) |
| `D2_DEF_1..3` | default, knob unset | D2 3/3 in the d2rebase band (`score_d2_v2.py port`), default unchanged |
| 13 tracks | default, knob unset | identical to `verify/rpct_20261022/tracks_flip_score.txt` |
| `D2_RNT_1..3` | knob + player record dump + `MASHED_A5_RACE_TIMER_LOG` | **G-ON-TIMER**: the log reads 44200 / 74100 / 104100 at slot-0 steps 0 / 600 / 1200 (the knob-off 0 / 29900 / 59900 + 44200) |
| `D2_RNT_1` dump | | **G-ON-REC**: record-for-record equal to u9775's `D2_DEFA_1.rec.msd` over the common length |
| `D2_RNT_1..3` | | **G-ON-D2**: D2 band score, reported (expected near `D2_DEFA`: 0.1874 / 0.2488 / ~1928) |
| `D2_CLSF4RN_1` | CLSF4 + knob + dump | reported: equal to u9775's `D2_CLSF4A_1.rec.msd` over the common length, D2 score |
| `H_RN`, `B_RN_K0 / K1 / K26` | knob | reported: the knob-ON G-CTRL / B hashes for the flip decision (AI races: GO reads 50, not 0) |
| 13 tracks | knob | reported |

## 4. Reading

- Any G-NOOP mismatch, D2 default fail or 13-track change with the knob unset: the change is not inert. Stop and report DECISION.
- G-ON-TIMER and G-ON-REC pass: the race flow reproduces the u9775 calibration without the diagnostic add. U-9790 is resolved
  as "the port's timer runs through the pre-GO ticks, behind MASHED_A5_RACE_TIMER_RUN". The default flip is a decision row with
  the knob-ON hashes, not taken here.
- G-ON-TIMER passes and G-ON-REC fails: a reader of `DAT_007f0ff8` other than A5 moves on the solo arm. Report the first
  differing record and its words.
- G-ON-TIMER fails: report the logged values.
