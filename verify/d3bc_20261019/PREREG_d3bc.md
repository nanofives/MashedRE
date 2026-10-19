# PREREG d3bc: D3 (b) regime C, FUN_00415e20's acos (2026-10-19)

Branch `work/d3bc` from `race/first-frame-parity` `de7551b9c`. Model claude-opus-5-5, account2. Exe `MASHED_NO_DEPLOY=1`,
private root `D:\MashedWT\d3bc_root`. Written before any knob arm. Scorer `vsum.py` (`ai_ctrl_window.check_regime` on
`re/tools/b_envelopes.json`, C per car, 196 calls). Original C references: `verify/d3_regimes_20261013/o_reg_r19` / `o_reg_r20`.

## 0. Measured before this PREREG

1. **Baseline reproduced.** `B_DEF_K0/K1/K26` `6efb49ed / 3e0724ac / 395fb8b7` (= orchestrator's). C phases K26 / K54 / K57,
   two runs each: K26 `395fb8b7` x2, K54 = K57 `961f1ea4` x2. All score C 0/3 with the same items: car 1 `c0# 14 [20]`,
   `c1# 73 [18]`, `steer# 86 [40]`, `|steer|~ 15 [8]`, `accel# 1 [3]`, `brake# 1 [2]`; car 2 `c0# 30 [32]`, `c1# 38 [41]`,
   `steer# 67 [73]`, `|steer|~ 8 [9]`; car 3 `c0# 20 [27]`, `c1# 57 [79]`, `steer# 76 [105]`, `c1~ 0 [23.5]`, `|steer|~ 15 [28]`.
2. **Regime C equals regime A in the original until row 149.** `regcmp.py` on o_reg_r1 (A) vs o_reg_r19 (C), every aistep
   column except clocks and `flag_a368`: first difference car 1 row 149, car 2 row 151, car 3 row 158. o_reg_r19 vs o_reg_r20
   (both C): cars 1 / 2 identical over all 196 rows. So C's 196-call window is A's commands for 149 rows plus 47 C rows, scored
   against a width-0 per-car envelope: passing C needs the port's command stream near-exact for ~200 calls.
3. **First command divergence per car (default build, o_reg_r19):** car 1 row 4 (`c0`), car 2 row 4, car 3 row 11. On the
   u9767 exactness stack (`EX` = `XW0RLOLCPF` without force-split): car 1 row 71, car 2 row 4, car 3 row 39, the same rows in
   A (vs o_reg_r1). Command-shape events the port misses on `EX`: car 1 row 100 `ai_mode 7` (`FUN_00414c30` returns 2, call
   `0x004163c3`; port `MASHED_MODE7` off), car 3 row 137 `ai_mode 3` (`FUN_00414a70` returns 1). Car 2's row-23 mode 3 + the
   contact timer are reproduced on `EX` (timer 1390 vs 1440, the known one-tick order).
4. **The steer error differs from row 1 with equal inputs.** The stored error `hist_dc / hist_d8` (`0x008032dc / 0x008032d8`)
   differs on every row on every car, by about 1e-4 deg, while `own_x / own_z / look_x / look_z` are bit-equal (B, `EX`,
   car 1 vs o_wobj2, physics exact through call 104).
5. **Offline emulation (`emu15e20.py`) on the original's inputs** (aistep own / look, o_wobj2 record call k-1 `+0x948 /
   +0x950` = the `at` row FUN_0046d510 returns) reproduces the original's stored error on **103 / 103** rows with: the RW LUT
   normalize `FUN_004c39b0` (`0x004c39b0..0x004c3a83`, sum `(x*x + y*y) + z*z`), a double acos (`FSTP qword` `0x00415eb2` /
   `0x00415f94` into the CRT acos `FUN_004a3384`), the product with the double at `0x005cc970` (bits `0x404ca5dc20000000`,
   `FMUL qword` `0x00415eba` / `0x00415f9c`) rounded to float (PC_24). Exact sqrt instead of the LUT: 11 / 103. The same form
   at 53-bit: 35 / 103.
6. **The port's form differs only in the acos.** `MASHED_AI_LUT=7` (bit 2 = this LUT normalize, `AiStandalone.cpp:248`) on
   `EX`, B car 1: the emulator on the port's own inputs (port record call k-1) matches the port's stored error on 13 / 102 rows,
   misses by 1 to 20 float ulps. `SteerAngleErrorFwd` calls `std::acos(d)` with a float `d`, which is acosf.

## 1. Hypothesis, knob, arms, gates

H1: `FUN_00415e20` = LUT normalize + double acos + exact `0x005cc970` at float rounding. Knob `MASHED_STEER_ACOS64=1` (default
OFF): `std::acos(double)` times `57.295780181884765625` in `SteerAngleErrorFwd`. Bit 2 of `MASHED_AI_LUT` is the second half.

Arms (`run_arms.py`): `B_DEF_K0/K1/K26` on the new exe (G-NOOP); `B_EXL7A_K1` with car 1 records (G-ERR); (b) on `L7A`
(`MASHED_AI_LUT=7` + knob, on the default) and `EXL7A` (on `EX`) at K0 / K1 / K26 / K54; D2, 13 tracks, G-CTRL only for a
default proposal.

| gate | rule |
|---|---|
| G-NOOP | knob unset: `B_DEF_K0/K1/K26` `6efb49ed / 3e0724ac / 395fb8b7` |
| G-ERR | `EXL7A` B car 1: stored error = o_wobj2 on every row while the record is exact (`firstdiff.py`), and = the emulator on the port's own inputs on every row |
| G-B | (b) on `L7A` and `EXL7A`: per car against the default (A 2/3, B 3/3, C 0/3) and against the first command row |
| G-DEF | default proposal only if no car passing on the default fails in A / B / C, D2 3/3 (`score_d2_v2.py`), 13 tracks 0 RED, G-CTRL knob-off unchanged. Flip only on Mariano's decision |

Decision: G-ERR PASS = H1 holds and the knob is the faithful form, kept default OFF unless G-DEF passes. G-ERR FAIL = report
the residual rows, no further knob on this function in this lane.
