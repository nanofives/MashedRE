# RESULT u9765: car 1's body matrix at call 4 (2026-10-19)

Branch `work/u9765` from `work/u9764` `a9169954e`. PREREG `PREREG_u9765.md` with the knob (`13a5fe246`). Exe
`MASHED_NO_DEPLOY=1`, private root `D:\MashedWT\u9756_root`. References: o_wobj2 (unprobed), o_w93c (write watch on `+0x93c /
+0x948`, matches o_wobj2 outside the torque-ring history). Model claude-opus-5-5, account2.

## 0. Verdict

1. **Cause found: the body-matrix re-orthonormalize.** `FUN_0046e9e0` integrates the matrix as `dst row = (omega x src row) +
   src row` (`0x0046ee90..0x0046ef5b`) and calls `0x004c4680(ebx, ebx)` at `0x0046ef5e`. `0x004c4680` scales each row by the LUT
   inverse sqrt `0x004c3b90` (`0x004c4702 / 0x004c474a / 0x004c4792 / 0x004c4948 / 0x004c49b3`), so its rows are not exactly
   unit: o_w93c shows car 1's `up.y` at `1`, `1.00000012` after the step, `1.00000024` after the ortho, `1.00000048`, and so on.
   The port's live path (`VehiclePhysicsRun.cpp:1799` -> `BodyOrientationIntegrate.cpp` `MatrixOrthoNormalize`) uses
   `1/std::sqrt` and keeps `up.y` at exactly 1. Neither u9764 knob reached it (it calls `std::sqrt`, not `FastInvSqrt`).
2. **`MASHED_ORTHO_LUT=1` (default OFF)** runs `0x004c4680` as disassembled: LUT inverse sqrt, the original's selection order
   (`0x004c4898..0x004c48ef`, which differs from the old port on the tie `|a.u| == |a.r| < |u.r|`), and the `<= 0.0` checks
   (`[0x005d757c]`). Car 1 against o_wobj2 (`recpos.py`, x / z): velocity equal through call 20 (was 4), position through
   call 33 (was 6). It gives the same result alone (`XW1OL`) and with `MASHED_ROTATE_LUT` (`XW1RLO`).
3. **Not a default candidate.** G-B FAIL: regime B car 3 fails (`c1_distinct 57 [17,46]`, `steer_distinct 85 [29,84]`), and it
   passes on the default. G-D2 FAIL 0/3: slip1500 `0.2008` in, slip2000 `0.2618` OUT, drvmed `1788.53` OUT, H1 398/400, logs
   1628-1630 lines (not wall-pinned: the car drives slower). A remaining infidelity elsewhere was offset by the exact
   normalize.
4. **Next gap (new U-9766):** on `RLO`, the earliest differing physics field is the torque `+0x9c0` at call 5
   (`-0.000898641185` original, `-0.000898640312` port), then `+0x9e8` at call 16, velocity x at call 21, the matrix at call
   28. On `OL` alone it is `+0x9c0` at call 4, the field `MASHED_ROTATE_LUT` fixes (U-9764).

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | `B_DEF_K0/K1/K26` `6efb49ed / 3e0724ac / 395fb8b7` | PASS |
| G-REC | `XW1RLO`: matrix (`+0x928 / +0x93c / +0x948`) equal through call 27 (was 3); first physics field `+0x9c0` at call 5 (`firstdiff.py`) | PASS |
| G-POS | `XW1RLO` and `XW1OL`: velocity from call 21, position from call 34 (was 5 / 7) | reported |
| G-B | `B_OL` `ac74503a / 9afa4bc2 / 6e07f31b`, `B_RLO` `a7719968 / 23592e5e / 301727b5`: A 2/3, **B 2/3 (car 3 FAIL)**, C 0/3 | FAIL |
| G-D2 | `D2_OL_1..3`: slip1500 `0.2008` in, slip2000 `0.2618` OUT, drvmed `1788.53` OUT | 0/3 FAIL |

13 tracks and G-CTRL were not run: G-B and D2 rule the knob out as a default.

## 2. Code / tools

- `Vehicle/BodyOrientationIntegrate.cpp`: `MASHED_ORTHO_LUT`, `MatrixOrthoNormalizeRw` (same transcription as
  `Track/WarzonePontoon.cpp` `OrthoNormalize`).
- `verify/u9765_20261019/recfirst.py` (per-call field diff outside read-time fields), `firstdiff.py` (first differing call per
  physics field). Evidence in `D:/MashedWT/u9765_data` (`MOVED_OFF_GIT.txt`).

## 3. Next

- U-9766: car 1's torque `+0x9c0` at call 5 on `RLO`; find its writer with `--watch-write 9c0` (check the run against o_wobj2
  first).
- Why regime B car 3 fails and D2 slows under `MASHED_ORTHO_LUT`: the knob is faithful at car 1, so these name a second
  infidelity it exposes.
