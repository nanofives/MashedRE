# PREREG u9770: U-9770, car 1's row 149 and car 3's record call 7 (2026-10-19)

Branch `work/u9770` from `work/ccinv` `f3e534629` (= `race/first-frame-parity` `38de9c963` content). Model claude-opus-5-5,
account2. Exe `MASHED_NO_DEPLOY=1`, private root `D:\MashedWT\d3bc_root`. Written before any knob arm. Stack `FULL` =
`EXL7AFSM7PCA` (d3bc full stack + the M4 mode-7 set + `MASHED_CC_PC24` + `MASHED_A4_ORDER`) on the ccinv default.

## 0. Measured before this PREREG

1. G-NOOP on the new exe (`00592c86`): `B_DEF_K0` `d6d87a6c` (the ccinv default).
2. Original `--c30-probe 3600` capture o_c30b (regime A, car 1 equal to o_reg_r1 over 220 rows): FUN_00414c30 returns 2 for
   car 1 on rows 100..148 with held word 0 and `490:1`; at row 149 the held word `(&DAT_0088fc88)[v*0x2d]` is `0x5f9b58`,
   FUN_00414490 returns 0 (`490:0`) and FUN_00414c30 returns 0.
3. Port, `FULL` + `MASHED_MODE7_TRACE`, K0: the same calls return 2 on rows 100..149; the held word is 0 at row 149 and non-zero
   (`0x13`) at row 150. The port collects one row later.
4. Original write watch on car 1's held word (`--watch-write da98` on car 1's record = `0x0088fd3c`): writer pc `0x0045bfb5`,
   return chain `0x00458a20` (FUN_00458920) <- `0x0048501e` <- `0x00485085` (FUN_00485070) <- `0x0040fd0c` (FUN_0040fc00).
5. FUN_0040fc00's call order (disassembly): FUN_00425a40 (AI tick + physics) at `0x0040fc6c`, FUN_0040da50 (cars register) at
   `0x0040fc75`, FUN_0040d470 at `0x0040fc7c`, FUN_0045bba0 (pickups) at `0x0040fcd0`, FUN_00485070 (pair pass) at `0x0040fd07`.
   The port runs `CarsReg_FrameTick` / `Pickups_FrameTick` / `Registry_FrameTick` in UpdateRace, before the AI tick
   (`TrackRenderer.cpp` the `[mode 7 M1]` block).

## 1. Hypothesis, knob, arms, gates

H1: with the three ticks after the cars' physics, in FUN_0040fc00's order (cars register before the `0x0040fc7c` refdist
producer site, pickups and the pair pass after it), car 1's held word turns non-zero at row 149 and its commands follow the
original past row 149. Knob `MASHED_REG_LATE=1` (default OFF). Arms: G-NOOP (`B_DEF_K0/K1/K26` = `d6d87a6c / b903bddb /
0d23fb1f`); `FULLRL` (= `FULL` + the knob, traced) K0 / K26: first car-1 command row against o_reg_r1 / o_reg_r19, held word row;
(b) report on `FULLRL` and on the knob alone (`RL`).

| gate | rule |
|---|---|
| G-NOOP | knob unset: `d6d87a6c / b903bddb / 0d23fb1f` |
| G-HELD | `FULLRL` K0: car 1's held word non-zero first at row 149 |
| G-B | (b) report; default only on Mariano's decision with D2 / 13 tracks / G-CTRL |

Car 3's record call 7 is a separate leg, written as an amendment before its arms.
