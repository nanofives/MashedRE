# PREREG: D3-SUBSTATE-ROW315 (L-57, run 22)

Date 2026-10-29. Branch `orch/D3-SUBSTATE-ROW315` from `orch/D3-SUBSTATE-SETTLE-ORDER` @ 7bfbc06c9. Committed alone,
before any arm. Standalone only, no MASHED.exe launch, no Ghidra MCP, no code change planned (a fix only if the
measurement shows a port defect, and then only under MASHED_SUBSTATE behind its own amendment).

Exe: `D:/MashedWT/D3-SUBSTATE-ROW315_data/mashed_re_58c21e05.exe`, a byte copy of the base step's final exe
(sha256 58c21e05da3b..., built from 440c1eac9; this branch's HEAD adds only the base RESULT). Its default gates were
measured by the base step (G-CTRL 5943c5f1 / 364350ae / 52052890, B 3a03670c / 9fcdbd3a / 53778664 = the base's
merge_train.json refs). Root `D:/MashedWT/D3-SUBSTATE-ROW315_root`, raw data `D:/MashedWT/D3-SUBSTATE-ROW315_data/arms`.

## 1. What is already measured (no run, from the base raw data)

- Row split. `D2_DEF_1..3` (launchrev lines removed) are identical to each other. `D2_SUB_1..3` and `D2_SUBH_1..3` all
  differ from `D2_DEF_1` first at line 315 (byte 220861), never earlier. Line 315's printed fields that differ:
  ftot z (-5273.65 / -5273.72), horiz, av y, wf[0..3], vel x. The difference grows on lines 316..318 (wf[0]: 0.2, 0.8,
  1.4, 2.4). ftot z crosses zero between lines 315 and 316, so line 315 is the first line where a sub-print difference
  of the large terms (|ftot x| ~ 2.17e5) shows in 6 significant digits.
- H_SUB dump length. Base `H_SUB.step.csv` (3676 rows) is byte-identical to the first 3676 rows of the PREGO step's
  `H_SUB.step.csv` (13962 rows). PREGO's rows 3677..13962 are sub-state 7, v3 only, and only the `rtick` column
  varies across them (10726 distinct values, every other column constant). `H_SUB.ptrace.log` (6470 lines) is a byte
  prefix of PREGO's (27042). `H_SUB.gates.csv` differs from PREGO's prefix on 4 rows only, frame 0: `rec_x_rec`,
  `rec_z_rec`, `ptr_x`, `ptr_z` (0 / 0 before, 0.500936747 / -1.998945 now) and `exp_dist` on 3 rows. Last step frame
  3234 (PREGO 13520), so the run stopped about 10286 frames before MASHED_DET_FRAMES=14400. Same 222-line game log.

## 2. Hypotheses

- H1 (row split). The split is A5's grip ramp onset. A5 (FUN_0046ddb0) reads the race timer DAT_007f0ff8 (FILD at
  0x0046e0a9, ForceIntegrator.cpp:275, copied into g_raceTimer at VehiclePhysicsRun.cpp:1184): fVar5 = 60000
  (_DAT_005cea7c) - timer, ramp only when fVar5 < 0. The stand-in seeds the timer to kA5RtPhase3Seed = 44200 - 180 x 50
  (AiStandalone.cpp:2233) and adds 50 per stand-in tick, so it reads 44200 at GO. Under MASHED_SUBSTATE, phase 3's
  first frame zeroes it (TrackRenderer.cpp, the 0x0040e5a2 store) and FUN_0040fc00's add runs on every sub-state
  frame (776 phase-3 frames + state 4 + 112 state-5 calls), so it reads a different value at GO and the ramp starts
  on a different row.
- H2 (dump length). The H_SUB process ended before the frame cap for a reason that is not a function of the frame index.
  Candidates: RenderFrame's device Reset failure (exe_main.cpp:3613-3615, quiet `break`, exit 0) or WM_DESTROY.

## 3. Arms (all D2 arms on the registered D2 recipe, `run_arms.py`, `mashed_lock.py run --solo`, inside test-slot)

| arm | stack | wall |
|---|---|---|
| D2_RTR_1 | DEF + `MASHED_A5_RACE_TIMER_LOG=1` + `MASHED_RECDUMP` (slot 0) | 90 s |
| D2_SUBHRTR_1 | SUBH + the same two | 110 s |
| D2_NRTR_1 | DEF + `MASHED_NO_A5_RACE_TIMER=1` + `MASHED_RECDUMP` | 90 s |
| D2_SUBHNRTR_1 | SUBH + `MASHED_NO_A5_RACE_TIMER=1` + `MASHED_RECDUMP` | 110 s |
| H_SUB | the base step's H_SUB, unchanged | limit 900 s |

The added variables only write files (the race-timer log every 600 slot-0 steps, the MSD1 record after each race
frame's physics). They read no state the simulation reads.

## 4. Predictions

- P1 (instrument check). D2_RTR_1's motion rows (launchrev lines removed) equal D2_DEF_1's on every common line, and
  D2_SUBHRTR_1's equal D2_SUBH_1's on every common line.
- P2 (timer at GO). The `[d2ref] A5 race timer step 0` value differs between D2_RTR_1 and D2_SUBHRTR_1. Step 600 and
  1200 values = step 0 + 50 x step on both (one slot-0 step per frame). With T(line) from that fit, the first motion
  line where exactly one side has T > 60000 is line 315 (allowed: 314..316, the log does not say on which side of the
  step the copy is taken relative to the motion line).
- P3 (cause). D2_NRTR_1 and D2_SUBHNRTR_1 motion rows (launchrev lines removed) are identical on every common line
  (>= 900 lines). If P3 holds, the race timer is the only input that separates the two paths in this recipe.
- P4 (record). The MSD1 dumps of D2_RTR_1 and D2_SUBHRTR_1, aligned at the first record whose velocity +0x9b0..+0x9b8
  is nonzero, are byte-identical up to the frame of the P2 onset and first differ on that frame or the next. Report
  only: the first differing offsets.
- P5 (H_SUB). The re-run self-exits at the frame cap with `H_SUB.step.csv` = 13962 rows, sha8 8284be46 (the PREGO
  value, since every row is equal). If it again stops at 3676 rows, H2 is refuted and the exit is code-driven.

## 5. Classification rule (decided now)

- P3 PASS and P2 PASS: the split is a consequence of the race timer at GO. Then compare SUB's timer at GO with the
  original's measured range (a0 877..896 over 13 captures, AiStandalone.cpp:2222-2229: the timer reaches the player's
  first moving call at 50 x (a0 - 1)). Inside that range: faithful consequence of the substate path (the stand-in's
  44200 is a constant fitted to one capture). Outside it: port defect in the SUB pre-GO tick count, fix in scope.
- P3 FAIL: H1 refuted, the split has a second input. Report RED with the P4 offsets and the next measurement.
- P5 PASS: the short dump is a harness artifact (the run ended early; the missing rows are a frozen sub-state-7 tail).
  P5 FAIL: report the exit frame and leave the cause open.
