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

## 2. Amendment 1 (written before any MASHED_A6A_DRIVE_NORM_LUT arm)

Section 1's H1 is refuted: `MASHED_A5_DRAFT_LUT` moves no hash (`FS30DL`, `FULLRLDL`, `DL` = their knob-off hashes); the
drafting branch does not fire on this recipe. Measured since (exes `ca8f69c6` .. `d0d07acf`, G-NOOP `d6d87a6c` each time):
- New `--a5-probe` (FUN_0046ddb0 entry / leave, record in EDI at `0x00470923`), o_a5p3: the A5 call that takes the end-of-call-6
  velocity outputs `-1919.0802`, the next takes `-1964.98584` (end of call 7). So A6a probe row k is the frame of record call k,
  and the split is made in frame 7 after A5.
- `--watch-write 9b8` (o_w9b8, car partly frozen, sites only): per frame A5 writes at `0x0046e2dc` / `0x0046e360`, A6a at
  `0x004686a4` (tail, `-1964.928955` in frame 7) and `0x0046885a` (lateral, `-1964.98584`).
- New `--a6t-probe` (0x004c3ac0 from `0x004686a4`, A6a's tail stack) against the port's `MASHED_A6A_TAILLOG`, frame 7: the
  blend, `d8`, `l_64`, `linTerm`, `l_b8` equal; `+0xb14` `-131902.188` vs `-131902.203` and `+0xb1c` `-2744782` vs
  `-2744782.5` differ. They are the drive accumulate `w * (e4 * cc) + acc`.
- On the tracks `+0x1f0 == -0x69e1a6 / -0xe17f4c` the drive `e4` is scaled by `fwd^8`, `fwd` = forward . the normalised velocity;
  the original normalises with FUN_004c39b0 (LUT, decomp of 0x00467650 at the `iVar8 == -0x69e1a6` drive arm), the port with
  `Vec3Norm3` (sqrtf, `Vehicle/Integrate2.cpp` the `trackId` arm).

H2: with that normalisation on the LUT body, car 3's `+0xb14 / +0xb1c` and record equal the original's at call 7. Knob
`MASHED_A6A_DRIVE_NORM_LUT=1` (default OFF). Arms: G-NOOP; `TL3DN` (= `TL3` + the knob) K0: tail terms at frame 7 and car 3's record
split; (b) on `FULLRL` + the knob and the knob alone (`DN`). Gates as section 1 (G-VEL now: car 3's end-of-call-7 `+0x9b8` =
`-1964.98584`).
