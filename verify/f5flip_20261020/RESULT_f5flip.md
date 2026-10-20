# RESULT f5flip: u9774's five knobs default ON (2026-10-20)

Branch `work/f5flip` from `race/first-frame-parity` `059746f9e`. PREREG `PREREG_f5flip.md` (`deda36c30`, with the seed change),
committed before any arm. Exe `47a7e6d7` (`MASHED_NO_DEPLOY=1`), root `D:\MashedWT\u9756_root`, 13 tracks on `D:\MashedWT\u9749_root13`.
Arm outputs in `D:\MashedWT\f5flip_data` (`MOVED_OFF_GIT.txt`). Model claude-opus-5-5, account2.

## 0. Verdict: FLIPPED (Mariano's decision, 2026-10-20)

`MASHED_A6A_SPIN_ORDER`, `MASHED_TER_SPIN_F32`, `MASHED_FIXUP_F32`, `MASHED_FIXUP_SUM_ORDER` and `MASHED_FIXUP_SLIDE_ORDER` are in the
FAITH seed list of `Core/FaithDefaults.cpp` (value 1). Opt-outs: `MASHED_NO_<name>=1` each; all five together give the faithflip
default; `MASHED_NO_FAITH=1` still gives the pre-FAITH default. `MASHED_COLL_LUT` and `MASHED_A5_RING_ORDER` stay OFF. Every gate
passes; the new default equals u9774's `F5` arm in G-CTRL, B dumps, D2 and 13 tracks.

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-CTRL x2 | `H_DEF` = `H_DEF-2` = `df5ee50e / a90bbb16 / b3cd0e5f` (= u9774 `H_F5`) | PASS |
| G-OPT5 | five opt-outs: `H_N5` `826277f2 / 250c1569 / 2c8164b2`; `B_N5_K0/K1/K26` `2c24bca3 / 9be204c1 / 97cac7c8` | PASS |
| G-OPTALL | `MASHED_NO_FAITH=1`: `H_NF` `66d51720 / ca4284be / 1193bbd1` | PASS |
| G-B | `B_DEF_K0/K1/K26` `c165d2e5 / c01e164a / 2e937af7` (= u9774 F5); (b) A 2/3 (cars 2, 3), B 2/3 (cars 1, 3), C 1/3 (car 1): every verdict unchanged | PASS |
| G-D2 | 3 runs alone: slip1500 0.1900 x3, slip2000 0.2550 x3, driving-median 1965.35 x3, H1 398/400 (faithflip default: 0.1914 / 0.2501 / 1907.69) | PASS 3/3 |
| G-13 | 39 frames, 870 rows, all cited, 0 RED, identical to u9774's F5 run; against the faithflip default: Arctic 1091 35 -> 36, Highway 853 17 -> 18, cited | PASS |
