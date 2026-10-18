# PREREG u9747: AI path progress at GO under tick-first (2026-10-18)

Branch `work/u9747` from `race/first-frame-parity` `438566c88` (default G-CTRL `1fbc77ae / 29a08810 / d3a20f4c`). Model
claude-opus-5-5, account2. Written and committed before the knob arms.

## 1. Measured so far

- `B_TPPT_K1` / `B_TPPG_K1` (tick first + path progress, traced; base exe `a2670e87`, gate-log exe): countdown frames 0..180
  the player's progress row reads 29.507563 and the AI rows 0; GO frame 181 all three AI rows read 29.507563; from frame 182
  they read 29.7297 / 29.4578 / 29.1857 (the original at rest, o_tg1: 29.999977 / 29.73 / 29.457 / 29.18).
- Gate log: the alive word `FUN_0046c7b0` is 1 for all four cars through the countdown (the port's A3 writes `+0x4 = 1`,
  as `0x0046b6fc` does), so U-9747's stated cause is wrong. The gate that fails is `IsCarSlotActive` (`0x0040e370`, slot
  state): 0 for cars 1..3 until GO. And every record's pose (`+0x928` row) is (0,0,0) until the car's first step.
- From the bytes: the slot state 2 comes from `FUN_00418860`'s alive-poke (`0x004188ae..0x004188db`), which the original
  runs on its pre-GO ticks (749 probe calls before the window on o_reg_r13); the pose comes from `FUN_004704c0`
  (`0x004705fe` / `0x0047060a` translation, `0x0047061c` / `0x0047062e` rotation) at race start.

## 2. Knob `MASHED_COUNTDOWN_SPAWN=1` (default OFF)

On each countdown frame: `Ai_CountdownAlivePoke` (the alive-poke alone) and `VehiclePhysics_SeedSpawnPose` for the player
and each AI car (both record matrices from the grid pose).

## 3. Gates

| gate | measure | pass rule | expectation |
|---|---|---|---|
| G-NOOP | knob off on the new exe: `B_TPPG_K1`, `B_DEF_K1`, `H_DEF` | `b13cef3a`, `d78355d9`, `1fbc77ae / 29a08810 / d3a20f4c` | PASS |
| G-PROG | `B_TPPGC_K1` path-progress trace | countdown frames: AI rows 29.73 / 29.457 / 29.18 +-0.01, player 29.999977 +-0.01; the GO frame's AI rows the same | PASS |
| G-ORDER (U-9747) | `B_SCS_K1` car 2 `ret14a70` at window call 1 | 0 (original), against `S`'s 1 (d3bB2) | not predicted |
| D3 (b) | `CS`, `TCS`, `SCS` against `DEF`, `T`, `S` | default-ON candidate only if no car that passes on DEF turns to FAIL | not predicted (the knob alone changes the countdown only) |
| D2 / 13 tracks / G-CTRL | only for a candidate | the usual rules | |
