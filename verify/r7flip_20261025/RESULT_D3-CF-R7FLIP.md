# RESULT D3-CF-R7FLIP: `MASHED_R7_EXIT` + `MASHED_R7_INPUT` default ON together. Measured gates equal the expected post-flip refs, D2 PASS 3/3, 13 tracks 0 RED (2026-10-25)

Verdict: **GREEN.** Every PREREG prediction (P1 to P5) holds.

## Setup

Decision L-16 = 1 (Mariano, 2026-10-25). Branch `orch/D3-CF-R7FLIP` from `orch/integration` @ `3d0c006c8`, the current tree,
which holds the L-13 / L-14 / L-15 deletions (D4-DC-39b0-L13 merge `97891ea5f` is an ancestor of `3d0c006c8`).
- Flip `34438fd59`: `R7ExitEnabled()` (TrackRenderer.cpp) and `R7InputOn()` (exe_main.cpp) now read `MASHED_NO_R7_EXIT` /
  `MASHED_NO_R7_INPUT` and are ON unless the opt-out is set. This is the same form as the U9779 set (`MASHED_NO_ROUND_END7`).
  The comments at every site now say default ON. Test knobs `MASHED_R7_INPUT_PRESS` / `_PAD_PRESS` are unchanged.
- PREREG + runner `fa9a810da` (committed alone, before any arm). The runner is a copy of r7joypad's. Its only addition is `NOXI` =
  both opt-outs.
- Exe sha256 `0d5cb8ac3ed979c9ae67f919aa1c6364bfa88481a6997f0c74b74d8dfd4f2800`, built with `MASHED_NO_DEPLOY=1`. `original\mashed_re_dev.asi`
  was not redeployed. Private roots: `D:\MashedWT\D3-CF-R7FLIP_root` (arms, D2) and `..._root13` (13 tracks), both robocopies of
  `original\`. Every run was under `mashed_lock.py`, and the D2 arms ran under `--solo` with no other build or train during them.

## Results

| # | arm(s) | measured | prediction |
|---|---|---|---|
| P1 | `H_DEF` (G-CTRL) | `5943c5f1 / b493262b / 09551e6a` (14659 / 56873 / 28436 lines) | **holds** (= expected) |
| P2 | `B_DEF_K0 / K1 / K26` | `3a03670c / 2516d903 / 53778664` (1459 / 1825 / 1411 lines) | **holds** (= expected) |
| P3 | `B_NOXI_K0` (both opt-outs) | `c37d4c0c` | **holds** (= the pre-flip B K0 ref) |
| P4 | `D2_DEF_1 / 2 / 3`, `--solo`, `score_d2_v2.py port` | **PASS 3/3**: slip1500 0.1900 x3, slip2000 0.2550 x3, drvmed 1965.35 / 1965.35 / 1966.66, all in band, H1 398/400 x3. motion_diag 1630 / 1631 / 1634 lines (all >= 1600, none re-run), sha8 `1c1d5b1b / aacb6a3c / 7f0e420a` | **holds** |
| P5 | 13 tracks, `run13.py run` (no knob), scored vs `verify/rres_20261016/cites_rres.json` | 13/13 OK after 1 attempt, 0 RED on all 39 rows, INSTRUMENT GATES PASS. The score table is **identical** to `verify/r7joypad_20261025/data/tracks_XI_score.txt` (41 / 41 non-blank lines) | **holds** |

Notes:
- D2 values differ from R7JOYPAD's `D2_XI` (slip2000 0.2549 / 0.2548, drvmed 1970.58). The tree differs: the L-13/14/15
  deletions landed after R7JOYPAD was measured. All three values stay in band.
- Each D2 log has one line `R7IN byte0xa slot=2 val=0xff r7frame=0 test=0`, at det_frame 37 / 38 / 38. That line is D2's scripted Enter tap. R7JOYPAD saw
  it at 72 / 69 / 70 and R7INPUT at 18 / 12 / 19. [UNCERTAIN] The shift is not explained here. Row D3-CF-R7DETFRAME owns it.
  No `R7IN` / `R7X` line appears in any H or B log (`data/r7_log_lines.txt`).
- Pad cooperative level 6 vs the original's 5 is unchanged by this step (row D3-CF-PADCOOP).

## Merge note

Gate refs move with this merge: G-CTRL `ef5f2e10/57f7ff83/b4f4b55a` -> `5943c5f1/b493262b/09551e6a` and B
`c37d4c0c/0a8f9806/7013671d` -> `3a03670c/2516d903/53778664`. This step does not edit `merge_train.json`, because L0 sets the refs.

## Evidence (committed: summaries only)

`data/run_gates.txt`, `data/run_d2.txt`, `data/score_d2_DEF.txt`, `data/t13.out`, `data/tracks_DEF_score.txt`,
`data/r7_log_lines.txt`, `data/RUNLOG.txt`, `data/raw_sha256.txt`.

Raw data stays in `D:/MashedWT/D3-CF-R7FLIP_data` (`arms/`, `t13/`). Cited raw files:

| file | sha256 |
|---|---|
| `D:/MashedWT/D3-CF-R7FLIP_data/arms/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `D:/MashedWT/D3-CF-R7FLIP_data/arms/H_DEF.gates.csv` | `b493262bed8ea6ecad4399f0ebaa53b902965423e74bfa07359188559fff479f` |
| `D:/MashedWT/D3-CF-R7FLIP_data/arms/H_DEF.ptrace.log` | `09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1` |
| `D:/MashedWT/D3-CF-R7FLIP_data/arms/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `D:/MashedWT/D3-CF-R7FLIP_data/arms/B_DEF_K1.step.csv` | `2516d9034a1774893bf2f59edd368c3783ad6cfe0ed491dc68b3d84a0afec7f6` |
| `D:/MashedWT/D3-CF-R7FLIP_data/arms/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `D:/MashedWT/D3-CF-R7FLIP_data/arms/B_NOXI_K0.step.csv` | `c37d4c0cfbdd5843f946feec67c0d3559ac17613d2520cf86ac53f6f79935f84` |
| `D:/MashedWT/D3-CF-R7FLIP_data/arms/D2_DEF_1.motion_diag.log` | `1c1d5b1bd9faccbbd7bf35fa851a779896ac41b0f6213debd07edbb6f0b242b6` |
| `D:/MashedWT/D3-CF-R7FLIP_data/arms/D2_DEF_2.motion_diag.log` | `aacb6a3c1e1d0a308d9a0619547a9a6ebf28c6ab64f387f9c74a21d506786314` |
| `D:/MashedWT/D3-CF-R7FLIP_data/arms/D2_DEF_3.motion_diag.log` | `7f0e420aa83936dce1b6cd4fe3966355c3eb0356809245fc72c5a536e2355669` |
| every other file (arm logs, `t13/*`) | `data/raw_sha256.txt` |
