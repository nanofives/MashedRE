# RESULT u9772: car 3's call 55 and car 2's call 136 are A5's drafting term (2026-10-20)

Branch `work/u9772` from `0ddf1d4d6`. PREREG `PREREG_u9772.md` (committed `0cd2f4a2e` before any knob arm). Exe `9a2b0e46`,
`MASHED_NO_DEPLOY=1`. Arm outputs in `D:\MashedWT\u9772_data` (`MOVED_OFF_GIT.txt`).

## Verdict

1. **Cause (U-9772 RESOLVED):** A5 (FUN_0046ddb0) resets its drafting factor `local_70` to 1 when `DAT_007f0fd0` is 4 or 9 (0.75
   when it is 8 and the car is not 0). FUN_00442ce0 returns 1 and FUN_00442c80 returns 0 on the same values. The original holds
   `DAT_007f0fd0 = 0` on this recipe (`--peek 007f0fd0:i`). The port's A5 read `g_playerCount`
   (`Vehicle/ForceIntegratorStubs.cpp`, annotated `DAT_007f0fd0`), which `VehiclePhysics_Init` sets to the car count, 4. So in
   every 4-car race the port cancelled the draft and the catch-up grip. In the original, car 3 drafts behind car 1 from call 55
   (gap 1.4011, alignment 0.99101 > 0.99 `_DAT_005cc9b4`, `r = 0.4251`, `local_70 = 0.85 * r = 0.361`).
2. **Knob `MASHED_A5_FD0_LIVE=1` (default OFF):** the three tests read the live `0x007f0fd0` word, as the exe's AI already does.
   On `FULLGO` (`TLG3F` K0), car 3's frame-55 A5 gives `l70 0.361369282`, `sigma 0.996683002`, the original's out / in ratio.
   The call-55 jump (`dvx +2.366`, `dvz +8.173`) becomes ulp drift (`-1.2e-4 / -2.4e-4`). Car 2's call-136 jump (`+0.390 / +3.220`)
   becomes `+1.8e-4 / +1.7e-3` from call 137.
3. **The remaining ulps are the drafting arithmetic:** adding u9771's `MASHED_A5_DRAFT_LUT=1` (float / LUT drafting test, inert
   until now because the draft never fired) makes car 3's record bit-equal to the original through call 148 and car 2's
   through call 172 (K0). The pair is stack `FLD`.
4. **Next split (new U-9773):** car 3 at call 149 and car 2 at call 150 (K26) / 173 (K0). Position differs first with velocity
   bit-equal (car 3: `+0x958` `-3.289e-05`, `+0x960` `-2.880e-04`). It is not the original's A/C split: o_c3, o_a3w0, o_a3w1,
   o_a3w2 and o_b3w0 agree with each other there, and the port differs from all of them at 149 in both K0 and K26 (o_a3w3
   rolled regime B and splits at 61).
5. **Not a default candidate:** `FLD` alone on the default gives (b) A 0/3, B 1/3, C 0/3, so default car 3 (A) and cars 1 and 3
   (B) fail. By the PREREG, D2 / 13 tracks / G-CTRL were not run.

## Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | knob unset, exe `9a2b0e46`: `d6d87a6c / b903bddb / 0d23fb1f` | PASS |
| G-A5 | `TLG3F` K0: `l70 0.361369282 = 0.85 * 0.42514`, record equal through call 148 with `MASHED_A5_DRAFT_LUT` | PASS |
| G-B | below | reported, no default |

## (b) table (vsum.py)

| stack | K0 (A) | K1 (B) | K26 (C) |
|---|---|---|---|
| DEF | 1/3 (car 3) | 3/3 | 0/3 |
| FULLGO (`c309ee1f / bb1d3d4c / bd0248cc`) | 1/3 (car 1) | 1/3 (car 1) | 1/3 (car 1) |
| FULLGO + FLD (`c49393b6 / 3047d554 / 97ac342a`) | 2/3 (cars 2, 3) | 2/3 (cars 1, 3) | 1/3 (car 1) |
| FL alone (`9a1940f7 / ae3cfd46 / d6f9b165`) | 0/3 | 1/3 (car 2) | 0/3 |
| FLD alone (`461e000c / e26ac05b / 1212adc4`) | 0/3 | 1/3 (car 2) | 0/3 |

On FULLGO + FLD, regime C misses by one distinct value on two cars: car 2 `c1_distinct 40` vs `[41,41]`, car 3 `80` vs
`[79,79]` (plus `steer_distinct`). Car 1 A fails at `c1_distinct 20` vs `[21,70]`.

## Files

- `mashedmod/src/mashed_re/Vehicle/ForceIntegrator.cpp`: `Fd0Word()` and the three tests (A5 phase 3, `RubberBandGrip`,
  `RubberBandGate`).
- `verify/u9772_20261020/run_arms.py`: stacks `FL`, `FLD`, `FULLGOF`, `FULLGOFD`, `TLG2F/3F`, `TLG2FD/3FD`.
