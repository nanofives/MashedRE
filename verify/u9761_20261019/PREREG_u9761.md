# PREREG u9761: the A6a torque chain at float precision, and the A6L / D2 slip1500 conflict (2026-10-19)

Branch `work/u9761` from `race/first-frame-parity` `5ca4815fd` (default G-CTRL `726b8fab / b048d6f0 / ecc20282`, B dumps
`85561724 / 76ef3eab / a000895f`). Model claude-opus-5-5, account2. Exe `effef35d` (`MASHED_NO_DEPLOY=1`), private root
`D:\MashedWT\u9756_root`. Written before any knob arm.

## 0. Known before this PREREG

- U-9760 (`verify/u9760_20261019/RESULT_u9760.md` section 5): with `MASHED_A6A_LUT` car 1's call-1 force words and velocity
  are exact; the first differing field is omega `+0x9c0` at call 1 (-8.73517183e-06 / -8.73517274e-06). On the current default
  `A6L` gives (b) A 2/3 B 3/3 C 0/3 but D2 slip1500 0.1877 OUT (band 0.18855-0.19635).
- The port keeps A6a's `l_60`, `l_d0`, `m15`, `m16`, `r` and the blend factor in `double` ([U-A6A-FLOAT10]); the race thread
  runs x87 at PC_24 (memory `track-load-runs-at-pc24`), so the original rounds each of those steps to float.

## 1. Hypothesis

H1: with the chain at float precision (and `MASHED_A6A_LUT`), car 1's omega `+0x9c0` at call 1 equals o_wobj2. H2 (open
question, no prediction of direction): whether the float chain moves D2 slip1500 back into the band with `MASHED_A6A_LUT`.

## 2. Knob and arms

- `MASHED_A6A_F32=1` (default OFF, `Vehicle/Integrate2.cpp`): `l_60`, `l_d0`, `m15`, `m16`, `r`, the blend `(l_d0 - m78) / l_d0`
  and its products, `lb0f`, and `grip = l_60 / +0x18c` in float.
Arms: `B_DEF_K0/K1/K26`, `H_DEF` (G-NOOP); `B_XA6F32FS_K1` (the u9760 stack + both knobs, car-1 record + split log);
`B_F32_K0/K1/K26`, `B_A6LF32_K0/K1/K26`; `D2_F32_1..3`, `D2_A6LF32_1..3` (each alone); then 13 tracks / `H_*` only for a
default proposal.

## 3. Gates

| gate | rule |
|---|---|
| G-NOOP | knob off: B dumps and `H_DEF` as above |
| G-OMEGA | `XA6F32FS` car 1 `+0x9c0` at call 1 = o_wobj2 |
| G-DRIFT | car 1 x/z gap at call 21 (now 1.164e-4 with `MASHED_A6A_LUT`): report |
| G-B | (b) on `F32` and `A6LF32`: no car that passes on the default turns to FAIL |
| G-D2 | `F32` and `A6LF32`: slip1500 / slip2000 / driving-median in the d81a8df6 bands, H1 >= 398/400 |
| G-13 | for a default proposal only: 0 RED, changes cited |

## Amendment 1 (2026-10-19, after the B and D2 arms, before these)

Measured: G-NOOP PASS (`85561724 / 76ef3eab / a000895f`). G-OMEGA PASS (`XA6F32FS`: `+0x9c0` at call 1 = o_wobj2; next physics
field `+0x460` at call 2). G-DRIFT unchanged (call 21 1.164e-4). G-B PASS: `F32` A 2/3 B 2/3 C 0/3; `A6LF32` A 2/3 **B 3/3** C
0/3. G-D2: `F32` alone slip1500 0.1863 OUT x3; `A6LF32` 3/3 in (slip1500 0.1887, slip2000 0.2537-0.2539, driving-median
1959.04-1965.90, H1 398/400). Default candidate: the pair `MASHED_A6A_LUT` + `MASHED_A6A_F32`: 13 tracks with both knobs
(cites `verify/rres_20261016/cites_rres.json`), `H_DEF`, `H_A6LF32`.
