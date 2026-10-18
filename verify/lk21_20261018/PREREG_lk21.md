# PREREG lk21: car 1's lookahead flip at window call 21 (2026-10-18)

Branch `work/lk21` from `race/first-frame-parity` `d41e51312` (default G-CTRL `b1f49e7a / b1be0054 / d3a20f4c`). Model
claude-opus-5-5, account2. Worktree build `MASHED_NO_DEPLOY=1`, exe `263d9380`, private root `D:\MashedWT\lk21_root`.
Written before any knob arm.

## 0. Measured before this PREREG (no new knob)

- G-NOOP: `B_DEF_K0/K1/K26` `e332af0c / 01ad5975 / 772f9fb7` (the u9747 flip default).
- The flip is not the lookahead's: the replay of the original's own rows through the port's `SplineLookahead`
  (`LR_M5`, the default LUT form) gives the original's target on every mode-0 car-1 call of the window in o_reg_r1 /
  r13 / r19 (0 misses; `lrwin.py`). The replay's only inputs are (spline, v, own_x, own_z) and the index word
  (`AiStandalone.cpp` `LookReplayOnce`), so the live flip is a position difference.
- On the plain default car 1 is off from call 0 (z -9.2e-5, speed +0.64 from call 1, look differs from call 1);
  the call-21 flip is the cev `S` stack's (`SPL`): there car 1's x/z is exact to 1e-9 through call 3, the z gap is
  1.2e-7 at call 4 and grows about 1.5x per call to 1.2e-4 at call 21 (`posdrift.py`).
- Record diff, car 1, `SPL` + `MASHED_RECDUMP_SLOT=1` (`B_SPLR1_K1`, step dump = `B_SPL_K1` `a8f11851`) against
  `o_wobj2` (regime B, car-1 record per frame; its car-1 aistep equals o_reg_r13 bit for bit), aligned on the
  position bits (`recdiff1.py`): at call 0 the basis differs, right.z `+0x930` 8.74227766e-08 / 4.37113883e-08 and
  at.x `+0x948` -8.74227766e-08 / -4.37113883e-08, and the gap carries (`+0x930` at call 1 4.33134774e-05 /
  4.32702436e-05). Also at call 0: y `+0x95c` 0.468502462 / 0.468502671.
- Cause of the basis bits: `FUN_004704c0` seeds both matrices with `FUN_004c4d20(m, &DAT_006146fc, deg, 1)`
  (`0x0047061c` / `0x0047062e`): r = deg * `_DAT_005cd7a8` as a float, x87 `fsin` / `fcos`, then `FUN_004c4a50(m, axis,
  1 - cos, sin, 1)`, which for axis (0, 1, 0) writes right = (1 - omc, 0, -s), up = (0, 1, 0), at = (s, 0, 1 - omc)
  (`decomp_4c4d20.txt`, `decomp_4c4a50.txt`). For car 1, r = float(pi), s = -8.74227766e-08. The port computes the
  same r, s, c (`GridDir_PortYaw`, `Race/RoundGrid.cpp`), turns them into one float yaw with `atan2`, and
  `BodyOrient_Init` rebuilds the basis from cos / sin of that rounded yaw: cos(float(-pi/2)) = -4.37113883e-08.

## 1. Hypothesis

H1: seeding the spawn basis with the original's elements (at.x = s, at.z = 1 - omc, right = (1 - omc, 0, -s)) removes
the call-0 basis difference, and with it the z drift that flips car 1's lookahead at call 21 under `SPL`.

## 2. Knob and arms

`MASHED_SPAWN_BASIS_RW=1` (default OFF): `GridDir_PortYaw` records (yaw bits -> s, 1 - omc); `BodyOrient_Init` uses
them when the yaw it is given is one of those values. Arms (1200 frames, K0 / K1 / K26): `SB` (the knob on the
default), `SPLSB` (`SPL` + the knob), `SPLSBR1` (+ car-1 record dump).

## 3. Gates

| gate | rule |
|---|---|
| G-NOOP2 | knob off on the new exe: `B_DEF_K0/K1/K26` unchanged, `B_SPL_K1` `a8f11851`, `H_DEF` `b1f49e7a / b1be0054 / d3a20f4c` |
| G-BASIS | `SPLSBR1` K1: car 1 `+0x928..+0x954` at call 0 equal to o_wobj2 bit for bit |
| G-DRIFT | `SPLSB` K1 car 1: own x/z gap at call 21 < 1e-5 (now 1.2e-4); first look difference later than call 21 |
| G-B | (b) `SB` on the default: no car that passes on the default (A cars 2, 3; B cars 1, 2) turns to FAIL |
| G-D2 / G-13 | only for a default proposal: D2 3/3 (the player's basis also comes from the grid law), 13 tracks 0 RED |

A G-BASIS pass with G-DRIFT failing is reported with the next differing field (the y at `+0x95c` is the named
candidate), no default proposal.
