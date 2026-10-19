# RESULT fwdsse: MASHED_FWD_AT + MASHED_RW_SSE_POINTS default ON (2026-10-19)

User decision 2026-10-19. Branch `work/fwdsse` from `b0ad0796e`, PREREG `PREREG_fwdsse.md` with the flip (`d339583af`). Exe
`3e66e2b6` (`MASHED_NO_DEPLOY=1`), private root `D:\MashedWT\f60flip_root`. Model claude-opus-5-5, account2.

## 0. Verdict

**All gates pass; both knobs are default ON.** Opt-outs `MASHED_NO_FWD_AT=1`, `MASHED_NO_RW_SSE_POINTS=1`. New default
G-CTRL `726b8fab / b048d6f0 / ecc20282`.

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-CTRL | `H_DEF` = `H_DEF-2` = `726b8fab / b048d6f0 / ecc20282`; `H_NOBOTH` `b40b47bc / fb690fb1 / d3a20f4c`; `H_NOFWD` `54106a23 / 209fa8b1 / ab8bc1b5`; `H_NOSSE` `ded760d5 / fa7a6390 / 27a97e09` (= u9758 `H_FA`) | PASS |
| G-BOPT | `B_NOBOTH_K0/K1/K26` `c7b40cf2 / 5b9905d8 / f680a529` | PASS |
| G-B | `B_DEF_K0/K1/K26` `85561724 / 76ef3eab / a000895f`: A 2/3 (cars 2, 3), B 2/3 (cars 1, 2; car 3 c1_distinct 49 > 46), C 0/3 | PASS |
| G-D2 | `D2_DEF_1..3`: slip 0.1942 / 0.2522, driving-median 1916.54 / 1916.54 / 1916.54 (in [1904.70, 1982.44]), H1 398/400, 1627 / 1628 / 1628 motion_diag lines | 3/3 PASS |
| G-13 | 39 frames, 867 rows, 867 cited, 0 RED, instrument gates PASS (`D:/MashedWT/fwdsse_data/tracks_def`). Against the last default run (u9756 `tracks_f60`, 869 rows): Egypt 740 20 -> 21, Dump 735 20 -> 19, Training 884 21 -> 20, Training 924 21 -> 20, every row cited by `verify/rres_20261016/cites_rres.json` | PASS |

## 2. Code

`Vehicle/VehiclePhysicsRun.cpp` `FwdAt()` reads `MASHED_NO_FWD_AT`; `Math/RwV3dTransformPointsCPU.cpp` `RwSsePoints()` reads
`MASHED_NO_RW_SSE_POINTS`.

## 3. Trackers (re-classify, no C-level change; `apply_trackers.py`)

U-9758 and U-9759 flip notes, one CHANGELOG line.
