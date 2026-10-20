# RESULT ccinv: MASHED_CC_MATRIX_INVERT default ON (2026-10-19)

User decision 2026-10-19 (relayed by the orchestrator), accepting (b) A 2/3 -> 1/3. Branch `work/ccinv` from `3ca5a4756`,
PREREG `PREREG_ccinv.md` with the flip (`ccb44927d`). Exe `4b4edf93` (`MASHED_NO_DEPLOY=1`), private root
`D:\MashedWT\d3bc_root`. Model claude-opus-5-5, account2.

## 0. Verdict

**All gates pass; the flip stands.** `Rw_MatrixDerive` (FUN_004c4dc0) calls `RwMatrixInvertEntry` by default, so the car<->car
spin tail of VehicleCarCarContact (0x00469df0) no longer reads an uninitialised stack local. Opt-out
`MASHED_NO_CC_MATRIX_INVERT=1` restores the stub (= the pre-flip default, bit-identical). `MASHED_CC_MATRIX_INVERT` is a no-op.

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-CTRL | `H_DEF` = `H_DEF-2` = `66d51720 / ca4284be / 1193bbd1` (new default); `H_NOCI` = `3b3690f6 / 9a657924 / a386f43b` | PASS |
| G-BOPT | `B_NOCI_K0/K1/K26` `6efb49ed / 3e0724ac / 395fb8b7` | PASS |
| G-B | `B_DEF_K0/K1/K26` `d6d87a6c / b903bddb / 0d23fb1f`: **A 1/3, B 3/3, C 0/3** as expected. Changed verdict: car 2 A PASS -> FAIL `c0# 37 [13,35]`. Changed values, same verdicts: car 1 A `\|steer\|~` 0 -> 6.5 (still out of [7,23]); car 2 C `c0#` 30 -> 33, `c1#` 38 -> 39, `steer#` 67 -> 71, `\|steer\|~` 8 -> 9 (now in), cars 1 / 3 C unchanged. B unchanged 3/3 | PASS (per the decision) |
| G-D2 | `score_d2_v2.py port`, whole log, 8-run band: slip1500 0.1859, slip2000 0.2534, driving median 1989.96 / 1988.09 / 1987.69 (spread 2.27 < h 17.305), H1 398/400, 1630 / 1634 / 1635 lines; all three in band | 3/3 PASS |
| G-13 | 39 frames, 868 rows, all cited, 0 RED, instrument gates PASS (`D:/MashedWT/ccinv_data/tracks_def`). Against a6flip's `tracks_def` (870): Dump 735 20 -> 19, Training 959 21 -> 20, both cited (the two frames that moved the other way in a6flip) | PASS |

D2 is a one-car solo recipe, so no car<->car call runs there; its motion_diag hashes match a6flip's runs (`762a2e73`,
`eb11e8d0`).

## 2. Code / trackers

`Collision/ContactStubs.cpp` `Rw_MatrixDerive`. Trackers via `apply_trackers.py`: flip notes on U-9769 and S-3442, one
CHANGELOG line. Evidence in `D:/MashedWT/ccinv_data` (`MOVED_OFF_GIT.txt`).
