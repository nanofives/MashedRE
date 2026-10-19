# RESULT a6flip: MASHED_A6A_LUT + MASHED_A6A_F32 + MASHED_A5_SUSP_LUT default ON (2026-10-19)

User decision 2026-10-19. Branch `work/a6flip` from `67666a33b`, PREREG `PREREG_a6flip.md` with the flip (`d787452da`). Exe
`997e76a9` (`MASHED_NO_DEPLOY=1`), private root `D:\MashedWT\u9756_root`, host LIVING-PC. Model claude-opus-5-5, account2.

## 0. Verdict

**All gates pass; the three knobs are default ON** (opt-outs `MASHED_NO_A6A_LUT=1`, `MASHED_NO_A6A_F32=1`,
`MASHED_NO_A5_SUSP_LUT=1`). New default G-CTRL `3b3690f6 / 9a657924 / a386f43b` (= u9762's `H_TRI`). Car 1 is bit-exact against
o_wobj2 through window call 4 on the new default (u9762), and (b) regime B is 3/3 for the first time on the default.

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-CTRL | `H_DEF` = `H_DEF-2` = `3b3690f6 / 9a657924 / a386f43b`; `H_NOALL` `726b8fab / b048d6f0 / ecc20282`; `H_NOLUT` `7baf3fd4 / cfcfc67e / 70af054f`; `H_NOF32` `d8bf6ec6 / 0fd1daf6 / 08731207`; `H_NOSUSP` `bbe1bfd5 / a054388e / 9f599ae9` (= u9761's `H_A6LF32`) | PASS |
| G-BOPT | `B_NOALL_K0/K1/K26` `85561724 / 76ef3eab / a000895f` | PASS |
| G-B | `B_DEF_K0/K1/K26` `6efb49ed / 3e0724ac / 395fb8b7`: A 2/3 (cars 2, 3), **B 3/3**, C 0/3; no car that passed fails | PASS |
| G-D2 | new gate (`score_d2_v2.py port`, whole log, 8-run band): slip1500 0.1859, slip2000 0.2534, driving-median 1989.05 / 1988.09 / 1989.96, all in; H1 398/400; 1630-1634 lines; spread 1.87 < h 17.305 | 3/3 PASS |
| G-13 | 39 frames, 870 rows, 870 cited, 0 RED, instrument gates PASS (`D:/MashedWT/a6flip_data/tracks_def`); against the last default (fwdsse, 867): Egypt 740 21 -> 20, Storm 928 21 -> 22, Dump 735 19 -> 20, Training 884 20 -> 21, Training 959 20 -> 21, all cited | PASS |

## 2. Code

`Vehicle/Integrate2.cpp` `A6aLut()` / `A6aF32()` and `Vehicle/ForceIntegrator.cpp` `A5SuspLut()` read `MASHED_NO_*`.

## 3. Trackers (re-classify, no C-level change; `apply_trackers.py`)

U-9760 / U-9761 / U-9762 flip notes, one CHANGELOG line.
