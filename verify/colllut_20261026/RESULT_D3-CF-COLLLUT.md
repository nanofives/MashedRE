# RESULT D3-CF-COLLLUT (L-18): MASHED_COLL_LUT re-measured on the A5 race-timer default (2026-10-26)

Branch `orch/D3-CF-COLLLUT` from `orch/integration` `992687ebc` (`4fd6191c5` is an ancestor, `git merge-base --is-ancestor`).
PREREG `PREREG_D3-CF-COLLLUT.md` committed alone (`403d96a6b`) before any arm. No source change. Exe `5e5198b2`
(`MASHED_NO_DEPLOY=1`, `original\mashed_re_dev.asi` not redeployed), root `D:\MashedWT\D3-CF-COLLLUT_root`, 13 tracks on
`D:\MashedWT\D3-CF-COLLLUT_root13`. No default changed.

## 0. Verdict

DECISION. With `MASHED_COLL_LUT=1` on the new default, D2 passes 3/3 at 0.1942 / 0.2492 / 1896.74, the value the PREREG
predicted from D3-CF-A5TIMER's `D2_CLSF4RN_1`. That stack is the one u9775 found bit-exact against the a0 887 originals
(orig_solo3, orig_solo4, o_w2) for the whole capture. G-CTRL does not move. B moves on K1 only. The 13-track table moves on 5
of 39 rows (4 better, 1 worse, 0 red). The flip is recommended. It changes the B ref, the 13-track reference and the D2 default,
so it has to merge alone with new refs.

## 1. Gates

| gate | default (no knob) | `MASHED_COLL_LUT=1` | ref (merge_train.json) |
|---|---|---|---|
| G-CTRL `H_*` step / gates / ptrace | `5943c5f1 / b493262b / 09551e6a` PASS | `5943c5f1 / b493262b / 09551e6a` (unchanged) | `5943c5f1 / b493262b / 09551e6a` |
| B `K0 / K1 / K26` | `3a03670c / 2516d903 / 53778664` PASS | `3a03670c / **e85ea6fb** / 53778664` (K1 moves) | `3a03670c / 2516d903 / 53778664` |

Line counts are the same in both columns (14659 / 56873 / 28436, B 1459 / 1825 / 1411).

## 2. D2 (solo window, `mashed_lock.py run --solo`, `score_d2_v2.py port`, floor 1893.63)

| arm | stack | motion_diag lines / sha8 | slip1500 | slip2000 | drvmed | H1 |
|---|---|---|---|---|---|---|
| `D2_CL_1` | COLL_LUT | 1636 / `aacdc3ce` | 0.1942 in | 0.2492 in | 1896.74 in | 398/400 |
| `D2_CL_2` | COLL_LUT | 1637 / `5b9627c8` | 0.1942 in | 0.2492 in | 1896.74 in | 398/400 |
| `D2_CL_3` | COLL_LUT | 1636 / `aacdc3ce` | 0.1942 in | 0.2492 in | 1896.74 in | 398/400 |
| `D2_CLNORN_1` | COLL_LUT + `MASHED_NO_A5_RACE_TIMER_RUN` (control) | 1636 / `06d83824` | 0.1939 in | 0.2533 in | **1893.37 OUT** | 398/400 |
| `D2_CLNORN_2` | control | 1636 / `06d83824` | 0.1939 in | 0.2533 in | **1893.37 OUT** | 398/400 |
| `D2_CLNORN_3` | control | 1637 / `dc1f894e` | 0.1939 in | 0.2533 in | 1893.76 in | 398/400 |
| `D2_DEF_1` | default | 1633 / `3e11df81` | 0.1874 in | 0.2488 in | 1928.09 in | 398/400 |
| `D2_DEF_2` | default | 1632 / `a6723862` | 0.1874 in | 0.2488 in | 1928.43 in | 398/400 |
| `D2_DEF_3` | default | 1631 / `e081f469` | 0.1874 in | 0.2488 in | 1928.43 in | 398/400 |
| `D2_NORN_1` | timer opt-out | 1633 / `b9d24cab` | 0.1900 in | 0.2550 in | 1965.67 in | 398/400 |

- **COLL_LUT on the new default: 0.1942 / 0.2492 / 1896.74 x3, PASS 3/3**, equal to the prediction. drvmed is 3.11 over the floor.
- **Control (COLL_LUT on the pre-flip timer): 0.1939 / 0.2533 / 1893.37, 1893.37, 1893.76, 1/3 in band.** The PREREG predicted
  1891.51 OUT from u9775's `D2_CLSF4RT_1` (a different exe). The control shows that COLL_LUT alone does not pass D2 without the
  race timer, which is why u9775 found it blocked only by the onset.
- **Current default without COLL_LUT: 0.1874 / 0.2488 / 1928.09, 1928.43, 1928.43**, the expected 0.1874 / 0.2488 / ~1929.
- Every arm is over 1600 lines, so no re-run was owed.
- Run note: my first solo run hit the tool's 10-minute background limit while `D2_DEF_1` was running. The runner process
  survived the kill. It finished `D2_DEF_1..3` and `D2_NORN_1` (`f955ff38` 1635 / `793799c2` 1637 / `a6723862` 1632 /
  `b7b66701` 1636, all in RUNLOG) and closed its solo window. My relaunch waited on that window (`d2arms_b.txt`: "WAITING for
  solo") and then re-ran the same 4 arms, so the runs never overlapped. The table above is the relaunch. `arms\` holds its
  files, which overwrote the first set. The first set's `D2_DEF_2` hash `a6723862` equals the relaunch's `D2_DEF_2`. No other
  worker ran during the window. No wait, no deadlock.

## 3. 13 tracks, `MASHED_COLL_LUT=1` (`cites_rres.json`, reference `verify/rpct_20261022/tracks_flip_score.txt`)

13/13 first attempt, 39 rows, INSTRUMENT GATES PASS, every row CITED with 0 red. 34 rows are identical to the reference. 5 move:

| row | matched (ref -> CL) | m.tri% (ref -> CL) | rows / cited (ref -> CL) |
|---|---|---|---|
| 01_Egypt 780 | 149 -> 151 | 96.0 -> 96.9 | 19 -> 18 |
| 07_Roundabout 1131 | 291 -> 293 | 90.7 -> 91.4 | 26 -> 25 |
| 13_Training 884 | 213 -> 215 | 95.4 -> 96.4 | 21 -> 20 |
| 13_Training 924 | 209 -> 211 | 94.0 -> 95.0 | 21 -> 20 |
| 13_Training 959 | 207 -> 205 | 95.3 -> 94.2 | 21 -> 21 |

Net +6 matched draws. Under the merge train these 5 rows read DIFFER, so the flip needs this table
(`verify/colllut_20261026/tracks_score.txt`) as the new `full.run13_reference`.

## 4. Decision row (L-18)

Flip `MASHED_COLL_LUT` default ON (`Collision/ContactDeps.h:40`, add a `MASHED_NO_COLL_LUT` opt-out on the d2ref pattern)?
- **Expected refs after the flip:** G-CTRL `5943c5f1 / b493262b / 09551e6a` (unchanged), B `3a03670c / e85ea6fb / 53778664`,
  13-track reference = `verify/colllut_20261026/tracks_score.txt`, D2 default 0.1942 / 0.2492 / 1896.74.
- **For:** this is the faithful collision path. With the timer onset matched it is bit-exact against the a0 887 originals over the
  whole capture (u9775). D2 3/3. Tracks net +6 matched, 0 red.
- **Against:** the D2 margin is 3.11 over the floor (1928.43 now). The timer opt-out stops being an in-band control after the
  flip. `MASHED_NO_A5_RACE_TIMER_RUN` alone gives 1893.37 OUT 2/3, so the D2 control would need both opt-outs set.
- **Recommendation: flip,** merged alone under the decided-flip protocol, with the B ref and `run13_reference` changed in the
  same merge.

## 5. Files

Committed here: PREREG, this RESULT, `run_arms.py` (a5flip's, paths changed, adds `CLNORN`), `gates.txt`, `d2arms.txt` (first
solo run, cut at `D2_DEF_1`, ends where the tool lost its stdout), `d2arms_b.txt` (relaunch), `score_d2.txt`, `tracks_run.txt`,
`tracks_score.txt`, `raw_sha256.txt`.

Raw data off git in `D:\MashedWT\D3-CF-COLLLUT_data` (`arms\` 71 MB: gate step / gates / ptrace, D2 motion_diag, logs, RUNLOG.
`tracks\` 118 MB: replay draws and diffs). SHA-256 of every cited raw file, including the 5 moved track diffs, is in
`raw_sha256.txt`.
