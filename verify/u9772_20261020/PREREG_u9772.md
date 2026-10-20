# PREREG u9772: U-9772, car 3's call 55 and car 2's call 136 (2026-10-20)

Branch `work/u9772` from `race/first-frame-parity` `0ddf1d4d6`. Model claude-opus-5-5, account2. Exe `MASHED_NO_DEPLOY=1`,
private root `D:\MashedWT\d3bc_root`. Written before any knob arm. Stack `FULLGO` = u9771's `FULLRL` + `MASHED_A6A_GEAR_ORDER`.

## 0. Measured before this PREREG

1. G-NOOP (exe `c88c7886`): `d6d87a6c / b903bddb / 0d23fb1f`. Traced arms `TLG3 / TLG2` = `FULLGO` K0 (`c309ee1f`).
2. Car 3, A6a tail (o_a6t3b vs `MASHED_A6A_TAILLOG`): equal at frames 52 and 54 (frame 53 has 1-ulp friction terms that
   do not reach the record); at frame 55 the speed at A6a entry already differs (`+0x9e4` 1891.25146 vs 1882.56848).
3. Car 3, A5 (o_a5p3 `--a5-probe` vs `MASHED_A5GDIAG`): frame 55's A5 input is equal (`|v|` 1897.463, `+0x9e4` 1897.468).
   The original's output is a pure rescale, out / in = 0.996683 on x and z; the port's `sigma` is 0.992198
   (`local_70` 0.85, base 0.009179), so the original's `local_70` is about 0.361.
4. `DAT_00803334..3c / DAT_00803340` (A5's add vector, `0x0046e30a / 0x0046e304`) have one writer, a zero-store in FUN_00470c70
   (`0x00471406 / 0x00471410`); `--peek` reads 0 throughout. So the difference is `local_70`.
5. The drafting test of A5 (decomp of 0x0046ddb0) on the original's end-of-call-54 records: car 1 is 1.4011 ahead of car 3,
   alignment 0.99101 > 0.99 (`_DAT_005cc9b4`), so `r = 1 - (6 - 1.4011) * 0.125 = 0.4251`, and `0.4251 * 0.85 = 0.361`. At
   call 53 the alignment is 0.98810 (no draft). Records of cars 1 and 3 are equal in the port at call 54.
6. A5 then tests `DAT_007f0fd0` (`== 4 / 9 -> local_70 = 1`, `== 8 && car -> 0.75`), as do FUN_00442ce0 and FUN_00442c80. The
   original holds `DAT_007f0fd0 = 0` on this recipe (`--peek 007f0fd0:i`). The standalone's A5 reads `g_playerCount`
   (`Vehicle/ForceIntegratorStubs.cpp`, annotated `DAT_007f0fd0`), which `VehiclePhysics_Init` sets to the car count
   (`Vehicle/VehiclePhysicsRun.cpp`, `g_playerCount = carCount`) = 4, so the draft is reset to 1 and the catch-up multiplier
   FUN_00442ce0 returns 1 and FUN_00442c80 returns 0 in every 4-car race. The exe's own AI reads `0x007f0fd0` live (0 here).

## 1. Hypothesis, knob, arms, gates

H1: with A5, FUN_00442ce0 and FUN_00442c80 reading the live `DAT_007f0fd0` word (0 here), car 3's A5 output at frame 55 and its
record at call 55 equal the original's. Knob `MASHED_A5_FD0_LIVE=1` (default OFF). Arms: G-NOOP; `TLG3F` (= `TLG3` + the knob)
K0: car 3's A5 `l70` / `sigma` at frame 55 and its record split; car 2's split (`TLG2F`); (b) on `FULLGO` + the knob and on the
knob alone (`FL`); D2 / 13 tracks / G-CTRL for the knob alone if its (b) keeps every passing car.

| gate | rule |
|---|---|
| G-NOOP | knob unset: `d6d87a6c / b903bddb / 0d23fb1f` |
| G-A5 | `TLG3F` K0: car 3's `l70` at frame 55 = 0.85 * 0.4251.. and its record equal past call 55 |
| G-B | (b) report; default only on Mariano's decision with D2 3/3, 13 tracks 0 RED, G-CTRL reported |
