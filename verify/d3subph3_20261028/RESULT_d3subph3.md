# RESULT: D3-SUBSTATE-PH3 (D-11073, phase 3 of the race sub-state machine)

Date 2026-10-28. Branch `orch/D3-SUBSTATE-PH3` from `orch/integration` 0d1285dc7. PREREG `PREREG_d3subph3.md`
(sections 0-5 at 11f5bf0dc, amendment 1 at 5026f2b6c, both committed alone and before any knob arm).

## 0. Verdict first

> **GREEN. Phase 3 now matches the original call for call. The frame-0 camera transient (U-9203) is one
> reset branch the port skipped. Behind default-OFF `MASHED_P3_RESET_PAIR`, port vs original over all 809
> phase-3 camera calls: x/z within 2.1e-5, y a constant +0.0405 (U-9202 grid height), phase-3 length 809 = 809.**
>
> - Scope correction: the step title ("phase 3 unported") was stale. Leg 2 (7ec59a21e, 2026-10-09) ported phase 3's hold,
>   release and camera chain behind `MASHED_SUBSTATE`. What was left was U-9203, which needed the original capture
>   this step made.
> - Default gates unchanged on this step's exe (dd4d2b69): G-CTRL `5943c5f1/364350ae/52052890`, B
>   `3a03670c/9fcdbd3a/53778664` (= `merge_train.json` refs).
> - G-KNOB-OFF PASS: arm `B_P3R_K0` (MASHED_SUBSTATE + replay, new knob off) gives byte-identical trace and step files
>   on the pre-change exe (9e72832a) and on this step's exe.
> - All five pre-registered predictions PASS (section 3).

## 1. Original capture (`o_p3c_r1`, read-only)

New probe `scenario_launch.py --p3cam-probe N` (entry + leave on `FUN_00446520`). TRAINING, 4 cars, statediff path,
private root, muted, under `mashed_lock`, audio endpoint checked (Philips FTV, OK). One run, no crash. 1142 calls over
2456 frames: 809 in sub-state 3 (all from `0x004483c7`), 1 in 4, 112 in 5, 220 in 6. Phase 3 lasted 808 frames on this run
(QPC-dependent, U-9201).

Frame 0 of phase 3 (sdframe 2) carries two calls, as in the port (`FUN_004111c0` case 3: `FUN_004102f0` -> `FUN_0040d470(1)`,
then `FUN_0040fc00` -> `FUN_0040d470(0)`):

| call | reset | `DAT_007f1008` | `_DAT_007f100c` | cam +4..+0xc after |
|---|---|---|---|---|
| 0 | 1 | 0 | 0.0 | (0.50889, 1.12211, -0.61099) |
| 1 | 0 | 50 | 0.0166667 | (0.13503, 3.24539, 4.80863), spring block 0 = a snap |
| 2 | 0 | 50 | 0.0166667 | (0.21588, 3.23199, 4.78866) |

H-A (an earlier non-reset call) is refuted, and H-B (an input differs) holds. H-C (the reset reproduces) is refuted, with
the cause in the bytes.

## 2. Cause, from the decomp of `0x00446520` (`decomp_446520.txt`, raw)

- `if (param_2 != 0) local_d4 = 0;` (right after the two same-pair tests) sends a reset INTO the pair-switch branch:
  `[0x267] += _DAT_007f100c + _DAT_007f100c` (0 on call 0), and since `[0x267] < 1.0` (`_DAT_005cc320`) the midpoint
  and pair zoom blend toward the stored pair `[0x265]/[0x266]` with weight `max(2b - 1, 0)` = 0. On the first call those
  ids are 0/0 (cam `0x00897fe0` is in .data's zero tail), so mid = car 0 and pair zoom = 0. The reset block
  (`if (param_2 != 0 || local_d4 != 0)`, `local_d4` re-zeroed first, set by `|d| > 5.0` `_DAT_005cc358`) then stores
  that camera. Call 1 is not a reset, but its distance test fires (5.8 > 5), so it re-snaps with the real pair.
- `RaceCamera::Update` skipped the branch on `force_reset` (`if (!same_pair && !force_reset)`), so its reset call
  produced the grid shot directly and every later frame ran one spring step ahead: U-9203's "about one frame ahead,
  0.09 at f0, decays by f60".
- Second, smaller difference: `FUN_0040fc00` stores `DAT_007f1008` / `_DAT_007f100c` (0x0040fc57) at its top, before
  `FUN_0040d470(0)`. The port stored them in `Ai_AdvanceClock`, after the chain, so its frame-0 second call read 0, and
  entry 0 skipped its first step (z 34.5 vs the original's 34.49836).

## 3. Change and score

`MASHED_P3_RESET_PAIR` (default OFF, not in FaithDefaults, meant with `MASHED_SUBSTATE`):
- `Race/RaceCamera.cpp`: a reset takes the pair-switch branch, and never-written pair ids (-1) read as 0.
- `D3d9Render/TrackRenderer.cpp`: on sub-state 3..5 frames, `DAT_007f1008` = units and `_DAT_007f100c` are stored before
  the chain call. These are the same values `Ai_AdvanceClock` writes again later in the frame.

`SCORE_d3subph3.txt` (`score_p3cam.py`, original run's `DAT_007f1030` replayed per phase-3 frame):

| prediction | knob OFF (`B_P3R_K0`) | knob ON (`B_P3RK_K0`) | verdict |
|---|---|---|---|
| P1 d(0), pred (0, +0.040, 0) +-0.01 on / \|d\| > 4 off | (-0.374, +2.164, +5.420) | (0.00000, +0.03750, 0.00000) | PASS |
| P2 spring block 0 after row 1 | nonzero | all 0 | PASS |
| P3 max k=1..60 \|d(k) - d(120)\|, pred < 0.01 on / > 0.05 off | 0.08514 | 0.00001 | PASS |
| P4 sub-state-3 rows, pred 809 +-3 | 810 | 809 (original 809) | PASS |
| P5 entry 0 z after row 1, pred 34.4984 +-0.0005 | 34.5 | 34.4983597 (original 34.4983597) | PASS |
| report: max \|dx\|,\|dz\| over all 809 calls | 5.42 | 2.1e-5 | |
| report: dy over calls 1..808 | 0.0279..0.0413 | 0.04054..0.04055 | U-9202 |

## 4. Limits and named gaps

- The knob changes `RaceCamera` on every reset when set, including resets outside the sub-state chain (`B_K3_K0`, knob
  alone, step file = DEF `3a03670c`, but the step dump does not see the camera). That is the original's behaviour, but no
  sub-mode-6 reset was compared against the original here.
- Pair ids read as 0 only while never written. The original keeps the previous race's last pair in a second race of the
  same boot, while the port's `RaceCamera` re-inits. Only the first race after boot was measured.
- U-9202 (+0.0405 y on every car, D2-domain) is untouched and is now the only phase-3 camera residual.
- One original run. The match is call-exact under replay, so the QPC phase difference between runs (U-9201) is
  outside this comparison by construction.

## 5. Tracker actions owed (not applied)

- **U-9203 -> RESOLVED** (cause section 2, fixed behind `MASHED_P3_RESET_PAIR`, evidence this file). re-classify has no
  documented "resolve an UNCERTAINTIES row" operation (only promotions and the DEFERRED close/rewrite/add helpers), so the
  row was not edited. No function changes C-level.
- **D-11073 rewrite** (via `deferred_close.py rewrite`): phase 3 is ported (leg 2) and now call-exact
  (`MASHED_P3_RESET_PAIR`). The remaining item is the D3-SUBSTATE-FLIP decided flip, which should carry
  `MASHED_P3_RESET_PAIR` with `MASHED_SUBSTATE`.

## 6. Raw (D:/MashedWT/D3-SUBSTATE-PH3_data, sha256)

| file | sha256 |
|---|---|
| `o_p3c_r1.msd.p3cam.csv` (original, 1142 calls) | ac72f052cb96691a4f09ebe8c501ce374ae41067b5272436cbda36f279486126 |
| `o_p3c_r1.log` | 355b3eda2d4e5bc32c9762eb6663d02fc808519e7d4ed4b36cc197e58da5ab02 |
| `p3c_r1_replay.txt` (808 tick lines) | cb619b354dee1e97026f675b7f52551dc1cebcf6cbc235af8cd9fe937ec111d3 |
| `decomp_446520.txt` (0x00446520, 0x0040e370, 0x0046c7b0) | 6a038e631899e3c5e7114441d45cd3ddd67165d5da14692550f1e0e869ea7462 |
| `arms/B_P3R_K0.trace.csv` (= `arms_base/` copy) | b2d16ee71b6a4c1dd73be46a53e8218e76caaa0edb54f9873c62f28c82f9157f |
| `arms/B_P3RK_K0.trace.csv` | 6873b6456df6eb2d72b29788cd01117cdba14fd2fda4746aca8b2f747bb200ac |
| `arms/B_P3R_K0.step.csv` | f98e3d1c9355130c63e4e71f7197c80390a09db897321b88485b170ae3869bd3 |
| `arms/B_P3RK_K0.step.csv` | 221b5391af4c8a9c48baf6d8fad5472e8722e693ee136d2e21ea50f4a8425b85 |
| `mashed_re_base.exe` (pre-change) | 9e72832a1f9498c0dfced0d3f59d3e7a12894218982c2d9f8cecb2744d3d4f45 |
| `mashed_re_knob.exe` (this step) | dd4d2b699d65819d818b15091e96eb81941af4a817e7e71cbddcef7994139316 |

Committed extracts: `o_p3c_r1.p3cam.head131.csv`, `B_P3R{,K}_K0.trace.head131.csv`, `CENSUS_o_p3c_r1.txt`,
`ARMS_RUN1.txt`, `SCORE_d3subph3.txt`. Every MASHED / mashed_re PID was its own and exited on its own (original
19712's lock holder, arms 11628, 5392, 10684, 5632, 11036, 16536, 19328, 5256). Nothing was killed. No .asi deploy.
