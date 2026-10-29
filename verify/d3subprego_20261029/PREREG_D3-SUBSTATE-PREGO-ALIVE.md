# PREREG: D3-SUBSTATE-PREGO-ALIVE (L-53 (a), Mariano 2026-10-29)

Date 2026-10-29. Branch `orch/D3-SUBSTATE-PREGO-ALIVE` from `orch/integration` 34235b24c. Committed alone, before
any arm of the change. Background: `verify/d3subflip_20261028/RESULT_D3-SUBSTATE-FLIP.md` on `orch/D3-SUBSTATE-FLIP`
@ 6c5fb3389 (evidence only, not merged), section 5.

## 1. The gap

Under `MASHED_SUBSTATE` the exe arm of SubStateMachine.cpp routes the original's `FUN_0046c7b0` calls to
`SubStateHost::car_alive`, which TrackRenderer binds to `aib_alive(v)` = `g_aib.alive[v]`. `AiBridgeSnapshot` fills that
array inside an AI tick only. A D2 arm is `MASHED_MEASURE_SOLO` (no AI car), so the host's `ai_tick` returns before the
snapshot and `g_aib.alive[0]` stays 0 through the countdown. The alive tests then skip the player:

| site | original (capstone, MASHED.exe.unpatched) | skipped on a 0 |
|---|---|---|
| FUN_004102f0 phase 3 | `0x00410330 call 0x46c7b0`, `0x00410338 cmp eax,1` | `0x0041033e call 0x46baa0` (hold) |
| FUN_004103a0 state 5 loop 1 | `0x0041042c call 0x46c7b0`, `0x00410434 cmp eax,1` | `0x0041043a call 0x46baa0`, `0x00410441 call 0x46d7f0` (rev charge) |
| FUN_004103a0 state 5 loop 2 | `0x0041048d call 0x46c7b0`, `0x00410495 cmp eax,1` | `0x0041049b call 0x46d780` (release) + the two stores into `0x007f0a04 + v*12` |

`FUN_0046c7b0` reads the record's `+0x4` (`(&DAT_008815a4)[v*0x341]`). The port has it as `VehicleSlotGetter`
(Vehicle/VehicleState.cpp, record-rebound). Memory ai-host-car-alive-is-racing-frames-only: the record word is 1 on
countdown frames (u9747 gate log, 2026-10-18).

## 2. The change

Exe arm only (the .asi already calls `0x0046c7b0` at its RVA): the three sites above call `::VehicleSlotGetter(v)`
directly. `SubStateHost::car_alive` and its `aib_alive` binding go (no other reader). Reachable only with
`MASHED_SUBSTATE` set, so the default build's behaviour does not change. No default flipped.

Also ported onto this branch, from `orch/D3-SUBSTATE-FLIP` 6c5fb3389, behind the same knob: amendment 1 (TrackRenderer.cpp,
`Ai_AdvanceCountdownClock(units)` on sub-state frames when `subClock && Ai_ClockLateEnabled() && Ai_CountdownClockEnabled()`).
The FLIP's D2 numbers were measured with it in, so the control (P4) needs it to reproduce them. The FLIP's
`MASHED_SUBFLIP_DIAG` is not ported.

A one-line `[substate] state5 alive=` log on the first state-5 call (the four `VehicleSlotGetter` values), exe arm only.

## 3. Arms

Runner `verify/savelink_20261023/run_arms.py` with a new stack `SUB` = `MASHED_SUBSTATE=1`, `MASHED_P3_RESET_PAIR=1`
(the FLIP's seeded knob set). Two exes from this branch:

- `pre`: amendment 1 + the `SUB` stack, NO alive change (the control build).
- `post`: pre + the alive change (the candidate, the commit that is reported).

Private root `D:/MashedWT/D3-SUBSTATE-PREGO-ALIVE_root`, raw data `D:/MashedWT/D3-SUBSTATE-PREGO-ALIVE_data`, every run
inside `test-slot` and `mashed_lock.py run --label D3-SUBSTATE-PREGO-ALIVE` (D2 and the 13 tracks `--solo`).

## 4. Predictions

| id | arm | prediction |
|---|---|---|
| P1 | `D2_SUB_1..3` on post | the log has `[substate] state5 alive=` with car 0 = 1, and a `launchrev release car=0` line (absent on the FLIP's arms) |
| P2 | `D2_SUB_1..3` on post | score_d2_v2.py: slip1500, slip2000 and drvmed all `in` the band (verify/d2rebase_20261019/band.json) on 3 of 3 |
| P3 | default gates on post | `H_DEF` 5943c5f1 / 364350ae / 52052890, `B_DEF_K0/K1/K26` 3a03670c / 9fcdbd3a / 53778664, `D2_DEF_1..3` 0.1942 / 0.2492 / 1896.74 in band, 13 tracks = verify/hbfullcheck_20261028/tracks_score.txt (merge_train.json refs win if they differ) |
| P4 | `D2_SUB_1` on pre (control) | 0.2016 / 0.2591 OUT / 1791.85 OUT, the FLIP's numbers, and no `launchrev release car=0` line |

Report only (no prediction): whether P2's values equal the opt-out's 0.1942 / 0.2492 / 1896.74 exactly, the
motion_diag line count of the `D2_SUB` arms against `d2_min_lines` 1600 (the FLIP's ~920 lines held at 102 s too, so it is
reported, and one 102 s arm is run if it is under 1600), and `B_SUB_K0/K1/K26` / `H_SUB` hashes against the FLIP's
expected flip refs (B 1d5bf4f9, G-CTRL 8284be46 / c33e1c11 / 31fe043e).

## 5. Verdict rule

GREEN = P3 holds AND P2 holds on 3 of 3. P2 fails on any arm = DECISION with the numbers (L0 files a new L-n). P3 fails =
RED. P4 failing means the knob set does not reproduce the FLIP, reported, and P2 is then read against P4's own numbers.
