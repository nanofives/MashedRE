# RESULT u9747: AI path progress at GO under tick-first (2026-10-18)

Branch `work/u9747` from `race/first-frame-parity` `438566c88` (default G-CTRL `1fbc77ae / 29a08810 / d3a20f4c`). Model
claude-opus-5-5, account2. PREREG `PREREG_u9747.md` (`4b52aef9a`, before the knob arms). Exes (`MASHED_NO_DEPLOY=1`,
`D:\MashedWT\u9747_data\exe`): base `a2670e87`, final `d0528f6b`. Original references: o_tg1 (at-rest progress), o_reg_r13,
o_c2r1 (alive word).

## 0. Verdict

**U-9747 is resolved, but its stated cause was wrong.** The alive word (`FUN_0046c7b0`, record `+0x4`) is 1 for every car
through the countdown on both sides (the port's A3 writes it, as `0x0046b6fc` does; o_c2r1 shows 1 from the first phase-3
frame). Two other things were missing on the port's countdown frames:

1. **Slot state.** `IsCarSlotActive` (`0x0040e370`) read 0 for cars 1..3 until GO. The original sets it to 2 with
   `FUN_00418860`'s alive-poke (`0x004188ae..0x004188db`), which runs on its pre-GO AI ticks; the port's countdown stand-in
   runs no AI tick.
2. **Spawn pose.** Every record's pose was (0,0,0) until the car's first step, so countdown progress was computed at the
   origin (29.507563 for every row that ran). The original's `FUN_004704c0` writes both state matrices at race start
   (`0x004705fe` / `0x0047060a` translation, `0x0047061c` / `0x0047062e` rotation).

Ported behind **`MASHED_COUNTDOWN_SPAWN=1` (default OFF)**. It passes every registered check. **Recommendation: default ON
(user decision).**

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | knob off on the final exe: `B_TPPG_K1` `b13cef3a`, `B_DEF_K1` `d78355d9`, `H_DEF` `1fbc77ae / 29a08810 / d3a20f4c` | PASS |
| G-PROG | `B_TPPGC_K1`: AI rows 29.727715 / 29.454863 / 29.182793 on every countdown frame and at GO (original 29.73 / 29.457 / 29.18); player row 0.000000 (the original's 29.999977 across the line, 2.3e-5 apart) | PASS on the AI rows; the player row fails the literal "+-0.01 of 29.999977" only by the wrap |
| G-ORDER (U-9747) | `B_SCS_K1` car 2 `ret14a70` at window call 1: 0 (original 0; `S` gave 1 in d3bB2); commands equal on calls 0..7 except +-1 at call 4 | PASS |

Two build iterations: the first poke used the AI host's `car_alive` (`g_aib.alive`, filled only on racing frames) and never
fired; the final calls `FUN_0046c7b0` itself, as the original does.

## 2. D3 (b), D2, 13 tracks, G-CTRL (`arms/SCORE_B.txt`, `arms/d2`, `arms/h`)

| arm | A | B | C |
|---|---|---|---|
| DEF | 2/3 | 2/3 (cars 1, 2) | 0/3 |
| CS (the knob) | 2/3 | 2/3 (cars 1, 2) | 0/3 |
| T / TCS | 2/3 / 2/3 | 2/3 / 2/3 | 0/3 / 0/3 |
| S / SCS | 0/3 / 0/3 | 0/3 / 0/3 | 0/3 / 0/3 |

- D2 `D2_CS_1..3` (each alone): slip 0.1941 / 0.1936 / 0.1939, 0.2545 x3, driving-median 1962.62 / 1959.25 / 1960.31, H1 398/400,
  1630..1634 lines: 3/3.
- 13 tracks: 39 frames, 869 rows, 0 RED; the score file is identical to the default's.
- G-CTRL `H_CS` `b1f49e7a / b1be0054 / d3a20f4c` (the player trace is byte-identical to the default's; step and gates move).

## 3. Code (`4b52aef9a` + this commit)

- `TrackRenderer.cpp` countdown block: under the knob, `Ai::Ai_CountdownAlivePoke()` and `VehiclePhysics_SeedSpawnPose` for the
  player and each AI car on every countdown frame.
- `AiStandalone.cpp` `Ai_CountdownAlivePoke`: `FUN_00418860`'s AI-round test and `FUN_0040e480(v, 2)` for `FUN_0046c7b0(v) == 1`.
- `VehiclePhysicsRun.cpp` `VehiclePhysics_SeedSpawnPose`: both record matrices from the grid pose.
- Diagnostic `MASHED_PATHPROG_GATE_LOG` (both gates and the record position per car, first 400 calls).

## 4. Tracker transaction (re-classify, no C-level change; `apply_trackers.py`)

U-9747 RESOLVED; hooks.csv notes on `00418860`, `004704c0`; one CHANGELOG line.

## 5. User decisions needed

1. `MASHED_COUNTDOWN_SPAWN` default ON (opt-out at the flip)? Faithful pre-GO state; no gate moves except G-CTRL's AI rows.

## 6. Follow-ups

- With this in, `MASHED_AI_TICK_FIRST` no longer starts from unconverged progress; its own (b) is unchanged (T = TCS = DEF pass
  set). The `S` stack's remaining (b) failures are elsewhere (U-9745 / U-9185 / U-9186, U-9684).
