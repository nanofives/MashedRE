# RESULT lk21: car 1's lookahead flip at window call 21 (2026-10-18)

Branch `work/lk21` from `race/first-frame-parity` `d41e51312` (default G-CTRL `b1f49e7a / b1be0054 / d3a20f4c`). Model
claude-opus-5-5, account2. PREREG `PREREG_lk21.md` (`6f2bd180e`), knob 1 (`20e667495`), amendment 1 + knob 2
(`e93da56bb`), each committed before the arms it governs. Exes (`MASHED_NO_DEPLOY=1`): base `263d9380`, `220b8193`, final
`71600f2f`. Private root `D:\MashedWT\lk21_root`. Original references: o_reg_r1 / r13 / r19, `o_wobj2`
(`verify/d3_modes37_20261009`, regime B, car-1 record every frame; its car-1 aistep equals o_reg_r13 bit for bit).

## 0. Verdict

1. **The lookahead is not the defect.** The replay of the original's own rows through the port's `SplineLookahead`
   (the default LUT form) gives the original's target on every mode-0 car-1 call of the window in all three regimes
   (0 misses, `lrwin.py` on `LR_M5`). Its only inputs are (spline, v, own_x, own_z) and the index word, so the live flip
   is car 1's position.
2. **Car 1's position drifts from a call-0 physics state.** On the plain default it is off from call 0 (z -9.2e-5,
   speed +0.64 from call 1, look differs from call 1); the call-21 flip is the `S` stack's (`SPL`), where x/z is exact
   to 1e-9 through call 3, z is one float step off at call 4 (1.195e-7) and the gap grows about 1.5x per call to
   1.2e-4 at call 21 (`posdrift.py`).
3. **The drift enters through the per-wheel force at call 1** (`newdiff.py`, record diff against o_wobj2 aligned on
   the position bits): the first new differences are the wheel force words `+0x210 / +0x214 / +0x21c` (1102.24048 /
   1102.29553, -2385.21582 / -2385.3418, ratio 1.00005) and the body velocity `+0x9b0`. The ratio is the one of the
   per-wheel ground normal held from spawn: `+0x200..+0x208` = (0, 0.999462068, -0.0312347524) in the original through
   about call 13, (0, 0.999512017, -0.0312363114) in the port; from about call 18 both carry the walk's
   (0, 0.999512076, -0.0312347524) and agree (`nrm.py`). The original's seed differs from the walk's in y only
   (5.0e-5). `FUN_004704c0` runs `FUN_0046f6c0(0)` at spawn; the writer of that seed inside it is not identified.
4. **Two other call-0 differences are real but do not move the drift**, each ported behind a default-OFF knob:
   - `MASHED_SPAWN_BASIS_RW`: the spawn basis. `FUN_004704c0` uses `FUN_004c4d20(m, (0,1,0), deg, 1)` (`0x0047061c` /
     `0x0047062e`), whose `FUN_004c4a50` writes right = (1 - omc, 0, -s), at = (s, 0, 1 - omc); the port turned the grid
     law's (s, c) into one float yaw (`atan2`) and rebuilt cos / sin from it (car 1 at.x -4.37e-8 against -8.74e-8).
     With the knob the rows `+0x928..+0x950` equal the original's at call 0 (G-BASIS PASS).
   - `MASHED_UP_TRINORMAL`: record `+0x9c8` is `FUN_0046c5f0`'s LUT normal of three transformed suspension points
     (`0x0046f79b..0x0046f7b0`, ESI = record + 0x9c8, wheels 0 / 3 / 2), which the port had marked "no state effect" and
     replaced with the basis up row (U-9156). With the knob `+0x9c8` / `+0x9d0` match, `+0x9cc` is 0.999992728 against
     0.999988973 (G-UP not exact).
   With both, car 1's z gap is unchanged through call 21 (G-DRIFT FAIL), as expected from 3.
5. No default change is proposed. Both knobs are faithful forms of the original's code and stay default OFF.

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP / 2 / 3 | `B_DEF_K0/K1/K26` `e332af0c / 01ad5975 / 772f9fb7`, `B_SPL_K1` `a8f11851` on all three exes; `H_DEF` `b1f49e7a / b1be0054 / d3a20f4c` on `71600f2f` | PASS |
| G-BASIS | `SPLSBR1` car 1 `+0x928..+0x950` at call 0 = o_wobj2 (pad words `+0x954` / `+0x964` and y `+0x95c` differ) | PASS |
| G-DRIFT | `SPLSB` z gap at call 21 1.2e-4 (unchanged) | FAIL |
| G-UP | `SPLSBUTR1` `+0x9c8` / `+0x9d0` equal, `+0x9cc` 0.999992728 / 0.999988973 | FAIL (3.8e-6) |
| G-DRIFT2 | `SPLSBUT` z gap at call 4 1.195e-7, call 21 1.2e-4 (unchanged) | FAIL |
| G-B | `SB` A 2/3 B 2/3 C 0/3 (the default's pass set); `SPLSB`, `SPLUT`, `SPLSBUT` 0/3 x3 | no regression on the default; no gain |

## 2. Code

- `Vehicle/BodyOrientationIntegrate.cpp`: `BodyOrient_NoteExactBasis`, `BodyOrient_Init` reads it under
  `MASHED_SPAWN_BASIS_RW`; `Race/RoundGrid.cpp` `GridDir_PortYaw` notes (yaw bits -> sin r, 1 - omc).
- `Collision/WheelContactSolver.cpp`: `MASHED_UP_TRINORMAL`, the `0x0046f7b0` call (`Math::LutSqrt`); the comment that
  called `FUN_0046c5f0` "no state effect" is corrected.

## 3. Tools

`lrwin.py` (replay misses by window call), `posdrift.py` (one car's x/z / speed gap at full precision), `recdiff1.py`
(record diff for any car, aligned on the position bits), `newdiff.py` (fields that first differ per call), `nrm.py`
(per-wheel normal both sides). Decomp / disassembly: `decomp_4c4d20.txt`, `decomp_4c4a50.txt`, `decomp_46d400.txt`,
`decomp_46c5f0.txt`, `disasm_46f6c0.txt`, `disasm_46da80.txt`, `disasm_46ddb0.txt`.

## 4. Tracker transaction (re-classify, no C-level change; `apply_trackers.py`)

New U-9754 (the spawn per-wheel ground normal seed); U-9156 note (`+0x9c8` is `FUN_0046c5f0`'s LUT normal); hooks.csv
notes on `0046c5f0`, `004704c0`, `0046f6c0`; one CHANGELOG line.

## 5. Follow-ups

- U-9754: find the writer of the spawn wheel normal inside `FUN_004704c0` -> `FUN_0046f6c0(0)` (`0x00470638`). The y-only
  difference from the walk's normal (z identical) is the lead. This is D2-class physics work and affects every car.
- The other `FUN_0046c5f0` calls (`0x0046fd57`, stack local, three-grounded branch; `FUN_0046da80` `0x0046db60`, record +
  0x9c8) and the spawn y (`FUN_0046d400`, an RW line query, 7 ulp) are unported.
- The port already has `Math/RwMatrixRotate.cpp` (`0x004c4d20`); `MASHED_SPAWN_BASIS_RW` could call it directly.
