# PREREG u9771: U-9771, car 3's record call 7 and car 2's call 136 (2026-10-19)

Branch `work/u9771` from `race/first-frame-parity` `05736d993`. Model claude-opus-5-5, account2. Exe `MASHED_NO_DEPLOY=1`,
private root `D:\MashedWT\d3bc_root`. Written before any knob arm. Stack `FULLRL` = u9770's (d3bc full stack + M4 mode-7 set
+ `MASHED_CC_PC24` + `MASHED_A4_ORDER` + `MASHED_REG_LATE`) on the ccinv default.

## 0. Measured before this PREREG

1. G-NOOP (exes `678a52cf`, `b980b350`): `B_DEF_K0` `d6d87a6c`. The force-split arms do not move the run (`377171b5` = `FULLRL`).
2. Original `--a6a-probe-wheel 0..3` on car 3 (o_a3w0..3) against the port's `MASHED_FORCE_SPLIT` slot 3 (`fscmp.py`): every
   A6a block-#4 term equals on calls 0..6 on all four wheels; on call 7 `ld8` (`[esp+0x18]`) and the terms built from it are 1 ulp
   apart on all four wheels together.
3. Probe extended with the record at the probe point (o_b3w0; port `MASHED_FORCE_SPLIT` + the same fields): at A6a of call 7,
   car 3's `+0x9b8` is `-1949.10852` (original) vs `-1949.10864`; `+0x9b0`, angular velocity, `+0x9e4`, `+0x9e8` equal. At the
   end of call 6 the record is equal (`+0x9b8` `-1934.46875`). A6a writes the velocity at its tail; in FUN_00470670 the call
   before A6a (`0x00470a..`, decomp) is A5, FUN_0046ddb0, which scales the velocity (`fVar4 * vel`) and adds a vector.
4. A5's drafting loop in the original (decomp of 0x0046ddb0, the loop over the other records): the other car's speed and the
   gap length by FUN_004c3ac0 (LUT), both normalisations by FUN_004c39b0 (LUT), `r = 1 - (6 - |gap|) * 0.125` at PC_24.
   The standalone (`Vehicle/ForceIntegrator.cpp` Phase 2) uses `Vec3Mag3` / `Vec3Norm3` (sqrtf) and `r` in double. Constants
   `_DAT_005cd0a0` 6, `_DAT_005cd050` 0.125, `_DAT_005cc9b4` 0.99, `_DAT_005cd03c` 1e-4 match the port's.

## 1. Hypothesis, knob, arms, gates

H1: with the drafting loop on the LUT bodies and float `r`, car 3's velocity at A6a of call 7 equals the original's. Knob
`MASHED_A5_DRAFT_LUT=1` (default OFF). Arms: G-NOOP (`B_DEF_K0/K1/K26` = `d6d87a6c / b903bddb / 0d23fb1f`); `FS30DL` (=
`FS30` + the knob) K0: `+0x9b8` at the probe point and the record split call of car 3; (b) report on `FULLRL` + the knob and on
the knob alone.

| gate | rule |
|---|---|
| G-NOOP | knob unset: `d6d87a6c / b903bddb / 0d23fb1f` |
| G-VEL | `FS30DL` K0: car 3's `+0x9b8` at A6a of call 7 = `-1949.10852` |
| G-B | (b) report; default only on Mariano's decision with D2 / 13 tracks / G-CTRL |

Car 2's call 136 is written as an amendment before its arms.
