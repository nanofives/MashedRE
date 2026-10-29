# RESULT: D3-SUBSTATE-ROW315 (L-57, run 22)

Date 2026-10-29. Branch `orch/D3-SUBSTATE-ROW315` from `orch/D3-SUBSTATE-SETTLE-ORDER` @ 7bfbc06c9. PREREG
`PREREG_D3-SUBSTATE-ROW315.md` committed alone at d34f3ca6f, before any arm. Standalone only, no MASHED.exe launched,
no Ghidra MCP, no .ps1 edited, no tracker touched, no source file changed, no default flipped.

## 0. Verdict first

> **GREEN. Both [UNCERTAIN] items from the base RESULT are explained, and neither needs a fix.**
> 1. **Row 315 is A5's grip-ramp onset, a consequence of the substate path that matches the original's own behaviour.**
>    A5 (FUN_0046ddb0) reads the race timer DAT_007f0ff8 (FILD at `0x0046e0a9`). Under MASHED_SUBSTATE the timer
>    counts the port's real pre-GO ticks, so it runs 250 units (5 ticks) ahead of the stand-in's fitted seed and passes
>    60000 five steps earlier. The slot-0 record is byte-identical on both paths through aligned record 310 and first
>    differs on record 311 = motion line 315. With A5's timer read opted out, the two paths are byte-identical on all
>    1626 common records. SUB's onset equals the original's rule k = 1203 - a0 at a0 = 892, inside the original's
>    measured 877..896. The stand-in's 44200 pins a0 = 887.
> 2. **The short H_SUB dump is a harness artifact.** The same exe, re-run, reaches the frame cap with `H_SUB.step.csv`
>    13962 rows, sha8 8284be46, byte-identical to PREGO's. The 3676-row file is a byte prefix of it. The rows it lacks
>    are a frozen sub-state-7 tail where only `rtick` changes. The display refresh changed from 60 Hz to 120 Hz between
>    that run's start (14:04) and the next run (14:06).

Default gates: not re-run. No source changed, and every arm ran the base step's final exe byte for byte (58c21e05,
section 6). The base RESULT measured that exe at G-CTRL 5943c5f1 / 364350ae / 52052890 and B 3a03670c / 9fcdbd3a /
53778664, which equal this branch's `merge_train.json` refs (this branch predates L-56).

## 1. Predictions (PREREG section 4)

| id | result | verdict |
|---|---|---|
| P1 instruments | D2_RTR_1 = D2_DEF_1 on 1628 common lines, D2_SUBHRTR_1 = D2_SUBH_1 on 1600 (launchrev lines removed) | PASS |
| P2 timer at GO | step 0 / 600 / 1200: DEF 44200 / 74100 / 104100, SUBH 44400 / 74350 / 104350 (`racetimer_log.txt`). From step 600 on SUBH = DEF + 250, 50 per step on both. SUBH passes 60000 at line 315, DEF at line 320 (below) | PASS (315, window 314..316) |
| P3 cause | D2_NRTR_2 = D2_SUBHNRTR_1 on all 1629 common lines (records: 1626, `recdiff_NRTR2_vs_SUBHNRTR.txt`). D2_NRTR_1 was 565 lines (wall-clock-bound arm, 30 s exit timer at low fps) and was re-run as D2_NRTR_2 (1630 lines). The 564 common lines are identical on all three | PASS |
| P4 record | RTR vs SUBHRTR: records identical through aligned k = 310, first differ at k = 311 (frame_idx 312) = motion line 315, 79 dwords (`recdiff_RTR_vs_SUBHRTR.txt`) | PASS |
| P5 H_SUB | self-exit at the cap, exit 0, 511 s. step 13962 rows 8284be46 = PREGO byte for byte, ptrace 27042 lines 31fe043e = PREGO, gates 54085 rows 2b19135b | PASS |

## 2. Row 315, mechanically

**Which field diverges first.** At full precision (MSD1, `MASHED_RECDUMP`, slot 0), the first record that differs is
aligned k = 311. The record before it is identical, so nothing diverges below print precision on lines 1..314. On
k = 311 the differences include +0x194 (0.00808951259 vs 0.00808950514), the wheel force words +0x214 / +0x21c / +0x2d8
/ +0x2e0 / +0x39c / +0x3a4 / +0x460 / +0x468 (the line's `wf=`, for example +0x214 -82669.5 vs -82669.2578), the
+0x928 matrix rows (+0x930, +0x948, +0x958..+0x960) and velocity +0x9b0 (746.744324 vs 746.742371). The line shows it
first in `ftot` z (-5273.65 vs -5273.72) because ftot z is a near-zero sum of terms of about 2.2e5 on that line.

**Which code writes the input that differs.** A5's grip ramp, ForceIntegrator.cpp:275-282:
`fVar5 = 60000 (_DAT_005cea7c) - (float)timer`. While fVar5 >= 0, local_6c = 1. Below 0, it is clamped at -30000
(_DAT_005cea78) and local_6c = 1 - fVar5 * (-1/30000) (_DAT_005cea74). local_6c then scales local_70 (:301-302), the
grip factor. The timer comes from DAT_007f0ff8 (the FILD at `0x0046e0a9`), copied into g_raceTimer before each car's
step (VehiclePhysicsRun.cpp:1184, default ON since the d2ref flip). The two paths write the timer differently:

| | writer | value at slot-0 step 0 |
|---|---|---|
| stand-in (knob off) | `Ai_RaceTimerRoundStart` (TrackRenderer.cpp:7694 -> AiStandalone.cpp:2245) seeds kA5RtPhase3Seed = 44200 - 180 x 50, then the `0x0040fe64` add (AiStandalone.cpp:2251) on each of the 180 stand-in ticks (TrackRenderer.cpp:5502) | 44200 |
| MASHED_SUBSTATE | phase 3's first frame stores 0 (TrackRenderer.cpp:5307, FUN_0040e590's `0x0040e5a2`), then the same `0x0040fe64` add on every sub-state frame (TrackRenderer.cpp:5357): 776 phase-3 frames, state 4, the state-5 calls | 44400 |

From step 600 on the gap is 250, so the timer passes 60000 five steps earlier under SUB. Measured onsets (timer read
on vs opted out, same path): DEF first differs from DEF-without-timer at record k = 316 / line 320
(`recdiff_RTR_vs_NRTR2.txt`). SUBH-without-timer is identical to DEF-without-timer, so SUBH's own onset is the k = 311
split.

**Port defect, faithful consequence, or harness artifact: a faithful consequence of the substate path.**
`verify/u9775_20261025/RESULT_D3-CF-U9775.md` measured the original: the first moving call a0 varies over 877..896
across 13 captures, the ramp starts at k = 1203 - a0, and n 44200 reproduces a0 887 (orig_solo3 / orig_solo4 / o_w2).
The stand-in's onset k = 316 = 1203 - 887 agrees with that. SUB's k = 311 is a0 = 892, inside the original's range.
SUB derives the timer from the port's own phase-3 length (776 frames), while the stand-in pins it to one capture.
No fix: the SUB path runs the original's writers (`0x0040e5a2`, `0x0040fe64`) and nothing reads them wrong. The
D2 effect (drvmed 1896.74 at a0 887 vs 1919.62 for SUBH) is the onset sensitivity U9775 already recorded (other onsets
1924.76 / 1928.03, U-9790).

## 3. The short H_SUB dump, mechanically

- Every row of the 3676-row base dump equals the same row of the 13962-row dump (PREGO's, and now this re-run's).
  Rows 3677..13962 are sub-state 7, v3 only, and only `rtick` changes across them (10726 distinct values, every other
  column constant). The game log and the `R6 round 1 OVER t=18.3s winner=car3` line are the same.
- The base run stopped at step frame 3234, about 10286 frames short of MASHED_DET_FRAMES=14400, still exit 0. In
  deterministic mode the frame cap is a pure function of the frame index (exe_main.cpp:10125), and the re-run on the
  same exe reached it. The early stop does not depend on the frame index.
- The adapter line in each game log: the base step's H_DEF / B_* / H_SUB runs (13:52..14:06) report `refresh=60`, its
  D2 runs from 14:06 on report `refresh=120`, as do all of this step's runs. The display mode changed during the base
  H_SUB run. **[UNCERTAIN]** which exit path ended it. RenderFrame's quiet `break` on a failed device Reset
  (exe_main.cpp:3613-3615) and WM_DESTROY both exit 0 without a log line. Evidence missing: an exit-reason log line,
  which the exe does not have.
- Also measured: the H_SUB gates differ from PREGO's only on 4 frame-0 rows (`rec_x_rec` / `rec_z_rec` / `ptr_x` /
  `ptr_z` = the settled slot-0 position 0.500936747 / -1.998945 instead of 0 / 0, `exp_dist` on 3 rows). That is the
  base step's settle-before-release, as intended. Refreshed H_SUB values on this exe for D3-SUBSTATE-FLIP:
  **8284be46 / 2b19135b / 31fe043e** (step / gates / ptrace).

## 4. Next

- D3-SUBSTATE-FLIP can use the refs in section 3. The D2 recipe question for SUB is the base step's DECISION
  (L-57 / `MASHED_DRIVE_HOLD_MS`). This step adds that SUBH's in-band score already carries the a0 892 onset. A SUB D2
  check that wants the stand-in's a0 887 has no knob for it. One would be a measurement knob only, never a default.
- Harness: an exit-reason line in exe_main.cpp at the loop's `break` / WM_DESTROY would close the section 3 [UNCERTAIN]
  in one run. Not done here (a source change for a diagnostic, and code layout moves some tracks' hashes).

## 5. Branch content

- `verify/savelink_20261023/run_arms.py`: stacks `RTR`, `SUBHRTR`, `NRTR`, `SUBHNRTR` (file-writing diagnostics only,
  commit 2e14a1d6f)
- `verify/d3subrow315_20261029/`: PREREG, this RESULT, `recdiff.py`, the three `recdiff_*.txt`, `racetimer_log.txt`,
  `RUNLOG.txt`, `raw_sha256.txt`

## 6. Raw (D:/MashedWT/D3-SUBSTATE-ROW315_data, sha256)

Full 64-hex list: `raw_sha256.txt` (19 files: the exe, every arm's motion_diag / rec.msd, the timer logs, H_SUB's four
files). Exe `mashed_re_58c21e05.exe` = 58c21e05da3ba8ed42c136be5f7c390dcb52f9f5a55b5a6dd1267a7d6a8dda73 = the base
step's `mashed_re_final.exe`. Base-step and PREGO raw files cited in sections 1 and 3 are listed in
`verify/d3subsettle_20261029/raw_sha256.txt` and under `D:/MashedWT/D3-SUBSTATE-PREGO-ALIVE_data/post/`.

## 7. Process

Every run inside `test-slot` and `mashed_lock.py run --label D3-SUBSTATE-ROW315` (D2 arms `--solo`), private root
`D:/MashedWT/D3-SUBSTATE-ROW315_root`. Every PID was spawned by this step's runner. The D2 arms end at their wall time
when the runner kills its own PID (the D2 recipe). No .asi deploy, `original\` untouched.
