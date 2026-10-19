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

## 2. Amendment 1 (written before any MASHED_AI_FWD_ATROW arm)

Measured after section 1's arms: G-NOOP PASS (`6efb49ed / 3e0724ac / 395fb8b7` on exe `3c47a766`). G-ERR FAIL: on `EXL7A` the
emulator on the port's inputs still matches the port's stored error on 13 / 102 rows, the same misses as without the knob. The
port's FUN_0046d510 stand-in is `aib_own_fwd_xz` (`TrackRenderer.cpp:153`), which returns the snapshot's
`(cos(yaw), sin(yaw))` (`TrackRenderer.cpp:6001`), with `yaw = atan2(at.z, at.x)` (`BodyOrient_Heading`,
`BodyOrientationIntegrate.cpp:454`). The original's FUN_00415e20 normalizes the raw `at` row (y zeroed, z negated). Its other
consumer is the mode-2 dot at `AiStandalone.cpp:1243`, also FUN_0046d510 in the original.

H2: with `aib_own_fwd_xz` returning record `+0x948 / +0x950`, `EXL7A` + this knob (`EXL7AF`) gives the original's stored
error on every row while the record is exact. Knob `MASHED_AI_FWD_ATROW=1` (default OFF; an all-zero row keeps the snapshot).

Arms: `B_DEF_K0/K1/K26` (G-NOOP), `B_EXL7AFR1_K1` (G-ERR), (b) on `L7AF` (default + the three) and `EXL7AF` at K0 / K1 / K26 /
K54, and the knob alone (`F2`). Gates as section 1. Decision unchanged.

## 3. Amendment 2 (written before any MASHED_STEER_SCALE_RDATA arm)

Measured after amendment 1's arms: G-NOOP PASS (exe `3ca4369f`). **G-ERR PASS**: `EXL7AF` B car 1's stored steer error equals
o_wobj2 on every row 1..104 (first miss row 105, where the record already splits), and the emulator on the port's inputs
matches the port on 102 / 102. (b) unchanged in count (`EXL7AF` A 1/3, C 0/3; `L7AF` A 2/3, B 2/3 car 2 `c1# 48 [17,46]`,
C 0/3; `F2` alone A 2/3, B 3/3, C 0/3).

New original captures `arms/o_c2` / `o_c3` (regime-A argv with `--statediff-car 2` / `3`; regime A, equal to o_reg_r19 through
row 148). On `EXL7AF` car 2's record equals the original through call 3 apart from the fields that also differ on the exact
car 1 (`+0x44 / +0x48 / +0x13c / +0x140 / +0xaf4..+0xb40`); at row 4 err and speed (`+0x9e4`, record call 3) are bit-equal
and the fresh-steer byte is 46 (original) vs 47 (port), m = 47.0222 with the port's `kSteerMagScale = 0.0030034f`.
The .rdata dword `0x005cd0e8` is `0x3b449ba6` = 0.003f. Over the fresh-steer rows (mode 0, curv <= 20, c0 xor c1 > 0) with
the original's own err and record speed, `trunc(err * speed * 0.003f)` gives the original's byte on 54/54 (car 2), 76/76
(car 3), 48/48 (car 1); 0.0030034f misses 6. An audit of every `_DAT_`-annotated float/double constant in
`AiStandalone.cpp` against the binary finds only this one and `kSteerScale` (handled by `MASHED_STEER_ACOS64`).

H3: FUN_00416250's fresh-steer products use 0.003f. Knob `MASHED_STEER_SCALE_RDATA=1` (default OFF), the two products
`0x00416648..0x00416656` / `0x0041675c..0x00416770` only (the mode-4/9/8 variants are not measured here).

Arms: `B_DEF_K0/K1/K26` (G-NOOP); `B_EXL7AFSR2_K26` / `B_EXL7AFSR3_K26` / `B_EXL7AFSR1_K1` (G-REC2: first record call where
cars 2 / 3 split from `o_c2` / `o_c3`, car 1 from o_wobj2, against amendment 1's call 4 / 7 / 105); (b) on `EXL7AFS`,
`L7AFS` and `SC` alone at K0 / K1 / K26 / K54. Gates and decision as section 1.

## 4. Amendment 3 (written before the arm; existing knobs only, no new code)

Measured after amendment 2's arms: G-NOOP PASS (exe `65de976f`). On `EXL7AFS` the first command row moves to car 1 row 100
(was 71), car 2 row 54 (was 4), car 3 row 39; the first record split outside the fields that differ on the exact car 1 is
car 2 call 11 (`+0x214`, 1 ulp), car 3 call 7 (`+0x9b8`, 1.2e-4). (b): `EXL7AFS` A 1/3, B 0/3, C 0/3; `L7AFS` A 2/3, B 1/3
(cars 2 / 3 `c1# 50 [17,46]`), C 0/3; `SC` alone A 2/3, B 1/3, C 0/3. No default candidate.

Report arm: car 1's row-100 event is FUN_00414c30 returning 2 (mode 7). `EXL7AFSM7` = `EXL7AFS` + the M4 set
(`MASHED_REGISTRY`, `MASHED_PICKUPS_ORIG`, `MASHED_REG_CARS`, `MASHED_REG_CALLBACKS`, `MASHED_MODE7`) at K0 / K26, with
car 1 records. Question only: does car 1 take mode 7 at row 100 and how far do its commands then match. No gate, no default.

## 5. Amendment 4 (written before any arm of the new knob body)

Measured after amendment 3's arm (`EXL7AFSM7`, exe `65de976f`): car 1 takes mode 7 at row 100 and its command stream equals
the original's through row 148 in C (o_reg_r19) and A (o_reg_r1); its record equals o_aq0 (u9766's regime-A car-1 capture,
aistep identical to o_reg_r1) through call 148 apart from a one-call `+0x39c` difference at call 117. (b) car 1 C: `c0# 19
[20]`, `steer# 39 [40]` only. Row 149 splits on a 1-ulp stored-error difference at row 148 (`1.25598145` vs `1.25597382`).

The double-acos model misses that row on the original's own inputs. FUN_004a3384's CW helper FUN_004a70a5 loads
`(cw & 0x300) | 0x7f` (`0x004a70a9..0x004a70b7`): the precision bits are kept, so under PC_24 `1+x`, `1-x`, the product and
FSQRT (`0x004a33a8..0x004a33b2`) round to float, FPATAN (`0x004a33b6`) is extended, and the FMUL by `0x005cc970` rounds once.
On the original's own inputs (`emucrt.py`) that form matches the stored error on 188/188 (o_aq0 car 1), 205/205 (o_c2),
344/344 (o_c3), 317/317 (o_wobj2) rows; the double acos misses 2 / 12 / 13 / 19.

H1': FUN_00415e20's angle = this CRT form. The knob is renamed `MASHED_STEER_ACOS_CRT` (default OFF) and its body replaced; the
`A64` tag in `run_arms.py` now sets it. Arms: G-NOOP; `EXL7AFSM7` with records of cars 1 / 2 / 3 at K26 and K0; (b) on
`EXL7AFS`, `EXL7AFSM7`, `L7AFS` at K0 / K1 / K26. Gates and decision as section 1 (G-ERR re-scored on the new body).
