# PREREG u9760: car 1's call-1 force words (2026-10-19)

Branch `work/u9760` from `race/first-frame-parity` `0bb215a3b` (default G-CTRL `b40b47bc / fb690fb1 / d3a20f4c`, default B
dumps `c7b40cf2 / 5b9905d8 / f680a529`). Model claude-opus-5-5, account2. Exe `acbea31f` (`MASHED_NO_DEPLOY=1`), private root
`D:\MashedWT\u9756_root`. Written before any knob arm.

## 0. Measured before this PREREG

- Write watch `o_f214` (car 1, `+0x214 / +0x21c`): the writers are A5 `FUN_0046ddb0` (`pc 0x0046e8af / 0x0046e8c3 /
  0x0046e91a / 0x0046e94f`) and A6a `FUN_00467650` (`pc 0x0046832a / 0x0046833a`), both under A4 (`0x0047107b`).
- A5 all-grounded branch (`+0x9e0 == 4.0`, `0x0046e885`), ESI = `rec + 0x1f4 + w*0xc4`: `fc = (DAT_0088e5f0 * [esi+0x18]) *
  0.5`, `F = (fc*n.x, (n.y - 1)*fc, fc*n.z)` into `[esi+0x20..+0x28]`; `len = FUN_004c3ac0(F)`; if `len != 0`: `inv = 1/len`,
  `a = inv*F` (stored floats), `t = ((a.y*[esi+0x30] + a.x*[esi+0x2c]) + a.z*[esi+0x34]) * len`, `F = t * axis`
  (`0x0046e8d8..0x0046e925`). `FUN_004c3ac0` = `LutSqrt(float((x*x + y*y) + z*z))` (`0x004c3ac4..0x004c3b1a`). The race
  thread runs x87 at PC_24, so every step rounds to float.
- Port (`Vehicle/ForceIntegrator.cpp` Phase 6): `Vec3Mag3` = `std::sqrt` (`ForceIntegrator.h:81-82`), `inv`, `a`, the dot
  and `t` in `double`, dot order `(a.z*az + a.x*ax) + a.y*ay`. The constants (`_DAT_005ccd08` 3000, `_DAT_005cea80`
  0x3b360bc0, `_DAT_005cc32c` 0.5) match the binary.
- Record at call 1 (u9759 `UTBQ1SSE`): `+0x214` -2.8865118 / -2.88652229, `+0x21c` -2385.21582 / -2385.22363; inputs at call
  0 (`+0x200..+0x20c`, `+0x220..+0x228`) equal.

## 1. Hypothesis

H1: with the original's arithmetic, car 1's `+0x214 / +0x21c` at call 1 equal the original's (A6a also writes them, so a
remaining gap there points at A6a). H2 (prediction): the call-1 velocity gap and the call-21 drift shrink.

## 2. Knob and arms

- `MASHED_A5_SUSP_LUT=1` (default OFF): Phase 6's all-grounded block in the original's arithmetic.
Arms: `B_DEF_K0/K1/K26`, `H_DEF` (G-NOOP); `B_XR1_K1` / `B_XA5LR1_K1` (the u9756 drift stack + `MASHED_UP_TRINORMAL`,
`MASHED_RW_SSE_POINTS`, `MASHED_RING_DBUF`, `MASHED_FWD_AT`, car-1 record; without / with the knob); `B_A5L_K0/K1/K26`;
`H_A5L`.

## 3. Gates

| gate | rule |
|---|---|
| G-NOOP | knob off: B dumps and `H_DEF` as above |
| G-F214 | `XA5LR1` car 1 `+0x214 / +0x21c` at call 1 = o_wobj2 |
| G-DRIFT | car 1 x/z gap at call 21 < 1e-5 (now 1.202e-4); else the next first-differing field |
| G-B | (b) on `A5L`: no car that passes on the default turns to FAIL |
| G-D2 / G-13 | for a default proposal: D2 3/3, 13 tracks 0 RED, G-CTRL reported |

## Amendment 1 (2026-10-19, branch `work/u9760b` from `b0ad0796e`, before these knob arms)

Measured with the new diagnostic `MASHED_FORCE_SPLIT` (inert: `B_XFS_K1` = `XR1` step dump `25d9c921`, `B_XA5LFS_K1` =
`69e1844c`): at call 1 the port's wheel-0 force before A6a's addend is `-13.9359436 / -11923.7607`, equal to the original's A5
output (`o_f214`: `-13.935943603515625 / -11923.7607421875`). **A5 is exact; the gap is A6a's addend**: port `11.0494213 /
9538.53711`, original `11.0494318 / 9538.54492` (its before/after stores). The addend is `lat * f5` with `f5 = (ld4*spd)*0.02*
lbc` (`spd*ld4` < 50), `lbc` proportional to `le4`. The port takes A6a's lengths with exact sqrt (`Mag3` / `Vec3Mag3`); the
original calls the LUT length `FUN_004c3ac0` nine times in order (`0x0046766e +0x9e4`, `0x00467680 +0x9e8`, `0x004680f6 le4`,
`0x0046820a ld4`, `0x0046833e |F|`, `0x004684bb m15`, `0x004684d7 m16`, `0x004685b7 m78`, `0x004686a4 speed`), matching the
port's nine sites. New knob `MASHED_A6A_LUT` (default OFF), exe `eadbd7fc`. Arms: `B_DEF_K0/K1/K26` (G-NOOP), `B_XA6FS_K1`
(XR1 + knob + split log), `B_XA5A6FS_K1` (+ `MASHED_A5_SUSP_LUT`), `B_A6L_K0/K1/K26`, `H_DEF`, `H_A6L`. Gates as in section 3,
with G-F214 now: the addend equals the original's.

## Amendment 2 (2026-10-19, after the amendment-1 arms, before these)

Measured: G-NOOP PASS (`B_DEF_K0/K26` `c7b40cf2 / f680a529`). G-F214 PASS with `MASHED_A6A_LUT`: the addend `11.0494318 /
9538.54492` and the call-1 force words `-2.8865118 / -2385.21582` equal the original's; the call-1 velocity `+0x9b0` too.
G-DRIFT FAIL but smaller early (call 4 3e-9, call 12 4.05e-6, call 21 1.164e-4). Next first-differing field: omega `+0x9c0` at
call 1. G-B PASS (`A6L`: A 2/3, B 2/3, C 0/3). Default candidate `A6L`: D2 `D2_A6L_1..3`, 13 tracks with `MASHED_A6A_LUT=1`
(cites `verify/rres_20261016/cites_rres.json`), `H_DEF`, `H_A6L`.

## Amendment 3 (2026-10-19, rebased onto `c43ebeba1`, before these)

Amendment-2 gates were measured on `b0ad0796e` (exe `eadbd7fc`): `A6L` (b) A 2/3 B 2/3 C 0/3, D2 3/3 (slip 0.1904 / 0.2547,
driving-median 1980.40-1980.65, H1 398/400), 13 tracks 867 rows 0 RED, `H_A6L` `3df00fdb / 7ed9989c / d42c9d28`; evidence in
`D:/MashedWT/u9760_data/verify/split_b0ad0796e`. The fwdsse flip merged (`MASHED_FWD_AT` + `MASHED_RW_SSE_POINTS` default ON;
default G-CTRL `726b8fab / b048d6f0 / ecc20282`, B dumps `85561724 / 76ef3eab / a000895f`). `work/u9760b` rebased onto
`c43ebeba1`. All gates re-run on the rebased exe: G-NOOP (`B_DEF_*`, `H_DEF` = the values above), G-F214 (`B_XA6FS_K1`), G-B
(`B_A6L_*`), D2 `D2_A6L_1..3` (each alone), 13 tracks with `MASHED_A6A_LUT=1`, `H_A6L`. `MASHED_A6A_LUT` stays default OFF.
