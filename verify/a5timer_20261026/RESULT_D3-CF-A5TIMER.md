# RESULT D3-CF-A5TIMER: the A5 race timer runs through the pre-GO ticks (U-9790, L-17 (a), 2026-10-26)

Branch `orch/D3-CF-A5TIMER` from `orch/integration` `d55d89c48`. PREREG `PREREG_D3-CF-A5TIMER.md` (`c447814e5`, committed
alone before any arm), amendment 1 (`cdc197de0`, committed alone after the first gate run and before any D2 arm). Knob
`8602c36ad` + `8984a6789`. Exe `bd5843f6` (`MASHED_NO_DEPLOY=1`), root `D:\MashedWT\D3-CF-A5TIMER_root`. No default changed.

## 0. Verdict

GREEN, knob OFF. U-9790 resolved behind `MASHED_A5_RACE_TIMER_RUN` (default OFF). The default flip is decision L-23, row
`D3-CF-A5TIMER-FLIP`.

1. With the knob ON, the race flow carries `DAT_007f0ff8` through the pre-GO ticks: round-1 seed 35200 (the `0x0040e5a2` zero
   plus the phase-3 ticks the 180-tick stand-in does not run), the `FUN_0040fc00` add (`0x0040fe64`) on every stand-in tick,
   then the `FUN_00418560` zero (`0x00418598`, `FUN_0040e350()` = `DAT_0063ba8c` == 5 at `0x00418593`) on the state-5 window
   of an AI race.
2. Solo D2 arm (no AI car, so no state-5 zero): the timer reaches 44200 at GO, the value u9775's diagnostic add used. The
   player record equals u9775's `D2_DEFA_1` record for record, and the faithful stack CLSF4 + knob equals `D2_CLSF4A_1`
   (= orig_solo3 / orig_solo4 / o_w2 per RESULT_D3-CF-U9775.md). `MASHED_A5_RACE_TIMER_ADD` is deleted.
3. AI races are unchanged with the knob ON: G-CTRL, B and 13 tracks equal the refs. Only the solo D2 arm moves.

## 1. Gates, knob unset (exe `bd5843f6`)

| gate | measured | ref | verdict |
|---|---|---|---|
| G-CTRL `H_DEF` | `5943c5f1 / b493262b / 09551e6a` | merge_train.json | PASS |
| B `B_DEF_K0 / K1 / K26` | `3a03670c / 2516d903 / 53778664` | merge_train.json | PASS |
| D2 `D2_DEF_1..3` | 0.1900 x3 / 0.2550 x3 / 1964.25, 1965.35, 1965.35, H1 398/400, 1629..1630 lines | band | PASS 3/3 |
| 13 tracks | 39 rows identical to `verify/rpct_20261022/tracks_flip_score.txt`, INSTRUMENT GATES PASS | | PASS |

The first knob build (exe `cb255d94`, `8602c36ad`) gave the same four knob-off hashes.

## 2. Knob ON

| arm | result | PREREG gate |
|---|---|---|
| `D2_RNT_1..3` timer log | 44200 / 74100 / 104100 at slot-0 steps 0 / 600 / 1200, 3/3 | G-ON-TIMER PASS |
| `D2_RNT_1..3` record vs u9775 `D2_DEFA_1` | first difference none over 1626 / 1625 / 1625 records | G-ON-REC PASS |
| `D2_RNT_1..3` D2 | 0.1874 x3 / 0.2488 x3 / 1928.56, 1930.04, 1930.04, H1 398/400, 1628..1629 lines (u9775 DEFA: 1928.43, 1928.43, 1930.04) | G-ON-D2 in band 3/3 (reported) |
| `D2_CLSF4RN_1` record vs u9775 `D2_CLSF4A_1` | first difference none over 1625 records; D2 0.1942 / 0.2492 / 1896.74 | reported |
| G-CTRL `H_RN` | `5943c5f1 / b493262b / 09551e6a` | reported (= ref) |
| B `B_RN_K0 / K1 / K26` | `3a03670c / 2516d903 / 53778664` | reported (= ref) |
| 13 tracks | 39 rows identical to the reference, INSTRUMENT GATES PASS | reported (= ref) |

**Amendment 1 (first knob build, exe `cb255d94`).** Zeroing before the add left 50 at an AI race's first racing tick: `H_RN`
was unchanged, but `B_RN_K1 / K26` read `c36c19d4 / 489f045e`, first different at step-dump frame 60 in `flag_a368`
(`0x0089a368`). The `FUN_004177b0` band-0 roll (`DAT_007f0ff8 * (1/3000) > 1.0`) fired one call early. u9745ov matched the
original's roll at window call 61 with 0 on the first racing tick, so the knob now zeroes after the add. Which original tick
carries the last state-5 zero, relative to the stand-in's GO frame, is not measured: new U-9791.

## 3. What the knob changes, and where

- `Ai/AiStandalone.cpp` `Ai_RaceTimerRunEnabled` / `Ai_RaceTimerRoundStart` / `Ai_RaceTimerCountdownTick`, with
  `kA5RtPhase3Seed = 44200 - 180 * 50`. 44200 is the add that put the port's onset on the trunk capture (a0 887). 180 is the
  number of stand-in ticks before GO (`countdown_ = 3.0f`, 1/60 s fixed step, GO on the 181st).
- `D3d9Render/TrackRenderer.cpp`: the stand-in tick before its `return`, `StartRound` (round 1 only, since the original's
  round 2 skips phase 3 at `0x0041026d`), and the `MASHED_SUBSTATE` block (the add where `Ai_AdvanceClock` did not make it).
- `Vehicle/VehiclePhysicsRun.cpp`: `MASHED_A5_RACE_TIMER_ADD` deleted.
- After the arms, one comment-only edit: the marker in `Ai_RaceTimerCountdownTick` now reads `[UNCERTAIN U-9791]`. Same
  line count, no code change.

## 4. Process notes

- Solo window: at the start of the D2 arms two rows were `working`. D4-FX-GATES's note read "waiting out A5TIMER solo". That
  row was waiting on this one and this one on it. D4-WSJ-REF was a tooling survey, and the machine lock had been free for 70
  min. The arms ran under `mashed_lock.py run --solo`, which closes the machine to every other run, and no build ran during
  them. A first launch was stopped after its first process started (a 10 min tool cap) and was re-run whole. That process is
  not counted and it left no RUNLOG line.
- Trackers (via re-classify): U-9790 resolved in place, U-9791 filed, CHANGELOG entry prepended. No C-level change.

## 5. Raw data (off git, `D:\MashedWT\D3-CF-A5TIMER_data`)

SHA-256 of every cited raw file in `raw_sha256.txt`: `arms\` (gate step / gates / ptrace, D2 motion_diag, record dumps, logs,
RUNLOG), `arms_v1\` (the pre-amendment knob-ON B dumps and RUNLOG), and the 13-track score files (copies here as
`tracks_score_off.txt` / `tracks_score_rn.txt`). The u9775 comparison records are `D:\MashedWT\D3-CF-U9775_data\arms\
D2_DEFA_1.rec.msd` / `D2_CLSF4A_1.rec.msd` (SHA-256 in `verify/u9775_20261025/raw_sha256.txt`). Tools: `run_arms.py` (u9775's
runner with the ADD stacks removed and RN / RNT / CLSF4RN added) and `msdeq.py` (record equality over the common length).

## 6. Next

- Decision L-23 (row `D3-CF-A5TIMER-FLIP`): default ON. Expected hashes after the flip: G-CTRL and B unchanged
  (`5943c5f1 / b493262b / 09551e6a`, `3a03670c / 2516d903 / 53778664`), 13 tracks unchanged. The D2 default moves to
  0.1874 / 0.2488 / ~1929, in band.
- D3-CF-COLLLUT (L-18) can re-measure `MASHED_COLL_LUT` on top of this knob. The expected faithful-path D2 is 0.1942 / 0.2492 /
  1896.74 (`D2_CLSF4RN_1`).
