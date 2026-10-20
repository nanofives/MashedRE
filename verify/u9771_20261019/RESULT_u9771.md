# RESULT u9771: car 3's call 7 is A6a's gear pre-factor association (2026-10-19)

Branch `work/u9771` from `race/first-frame-parity` `05736d993`. PREREG `PREREG_u9771.md` (section 1, amendments 1 and 2, each
committed before its arms). Exe `MASHED_NO_DEPLOY=1`, private root `D:\MashedWT\d3bc_root`. Model claude-opus-5-5, account2.
Stack `FULLRL` = u9770's full stack on the ccinv default.

## 0. Verdict

1. **Car 3's record call 7 is the gear pre-factor's association in A6a.** The original computes
   `fVar4 = (c498 * 0.5) * (fVar5 * k)` and `fVar5' = (fVar5 * k) * (c49c * 0.5)`, `k = _DAT_005ce1e8`
   (`0x0046768b..0x00467739`); the port had `((c498 * 0.5) * fVar5) * k`. In frame 7 that moved car 3's `local_cc` by 1 ulp
   (`8586.67383` vs `8586.6748`), hence the drive accumulators `+0xb14 / +0xb1c` and the A6a tail velocity. Offline, the
   original's order at float precision gives `8586.67383`.
2. **`MASHED_A6A_GEAR_ORDER=1` (default OFF)** on `FULLRL`: car 3's physics equals o_c3 through record call 54 (was 6), its
   stored steer error through row 57 (was 38). Car 1 still passes C (196/196 rows). Car 2 unchanged (call 136).
3. **Alone on the default (`GO`) it keeps every (b) verdict:** A 1/3 (car 3), B 3/3, C 0/3, the same cars as the default.
   Default hashes move (`b6d01d6d / a329528a / fa85dfeb`). D2 fails with it (section 1), so it stays default OFF.
4. **Two hypotheses refuted on the way, both left in as default-OFF knobs with no effect on this recipe:**
   `MASHED_A5_DRAFT_LUT` (A5's drafting test; it never fires here) and `MASHED_A6A_DRIVE_NORM_LUT` (the drive arm's surface
   test on `+0x1f0`; it does not run for car 3 here). Both are faithful to the decomp (FUN_004c3ac0 / FUN_004c39b0 LUT bodies).
5. **Next splits (new U-9772):** car 3 at record call 55 (`+0x9b0` -518.006104 vs -515.64032, `+0x9b8` -1768.26038 vs
   -1760.0874, position), car 2 at call 136 (`+0x9b0` -391.180573 vs -390.790558). Both are large single-call jumps, not ulp drift.

## 1. Gates for `MASHED_A6A_GEAR_ORDER` alone

| gate | measured | verdict |
|---|---|---|
| G-NOOP | knob unset, every exe of the lane (`678a52cf` .. `9103956c`): `d6d87a6c / b903bddb / 0d23fb1f`; `H_DEF` `66d51720 / ca4284be / 1193bbd1` | PASS |
| G-B | `GO` K0 / K1 / K26 `b6d01d6d / a329528a / fa85dfeb`: A 1/3 (car 3), B 3/3, C 0/3, the default's verdicts car for car | PASS |
| G-CTRL | `H_GO` = `H_GO-2` = `30e047aa / b6228d27 / 08b0e3d6` | reported |
| G-D2 | `D2_GO_1..3`: slip1500 `0.1912 / 0.1913 / 0.1912` in, **slip2000 `0.2574` OUT** (band 0.24555..0.25557), driving median `1952.77 / 1951.80 / 1952.77` in, H1 398/400 | **0/3 FAIL** |
| G-13 | 39 frames, 869 rows, all cited, 0 RED; against ccinv's default (868): Roundabout 1091 33 -> 34, cited | PASS |

**Not a default candidate**: D2 fails on slip2000 on all three runs. The same pattern as u9764..u9767: a faithful fix moves D2
out of the band, so an offsetting error remains on the D2 path.

## 2. How it was found (tools added, all diagnostic and default OFF)

- `--a6a-probe-wheel` against `MASHED_FORCE_SPLIT`, both now carrying the record's velocity, angular velocity, `+0x9e4`, `+0x9e8`
  (`fscmp.py`): every A6a block-#4 term equal through the frame before the split.
- `--a5-probe` (FUN_0046ddb0 entry / leave): A6a probe row k is the frame of record call k; A5's output equal.
- `--watch-write 9b8` (car partly frozen, sites only): A6a writes the velocity twice, the tail `0x004686a4` and the lateral
  `0x0046885a`.
- `--a6t-probe` (0x004c3ac0 from `0x004686a4`, A6a's tail stack) against `MASHED_A6A_TAILLOG`, `MASHED_A6A_VZLOG`,
  `MASHED_A6A_DRIVELOG`: the first differing tail input is `+0xb14 / +0xb1c`, then `local_cc`.
- `MASHED_A5GDIAG` now prints the car index.

Evidence in `D:/MashedWT/u9771_data` (`MOVED_OFF_GIT.txt`).
