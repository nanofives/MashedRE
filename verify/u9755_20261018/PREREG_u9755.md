# PREREG u9755: the spawn y via FUN_0046d400 (2026-10-18)

Branch `work/u9755` from `race/first-frame-parity` `79c7e1bf1` (default G-CTRL `b1f49e7a / b1be0054 / d3a20f4c`). Model
claude-opus-5-5, account2. Private root `D:\MashedWT\u9755_root`. Written before any knob arm.

## 0. Measured before this PREREG

- The original's spawn y for car 1 is 0.4717099666595459: the first write to record `+0x95c` (`o_seed1.msd.watch.csv`
  of u9754, pc `0x004c52cd`, the translation write inside `FUN_004704c0`). The port's, read off the countdown record
  (`MASHED_PATHPROG_GATE_LOG`, now `%.9g`, `B_TPPG_K1`): 0.471710175; x and z equal the original's (-1.10121727 /
  -0.902031064). 0.471710175 - 0.4717099666595459 = 2.08e-7, the same gap U-9755 records at window call 0.
- The original: `FUN_0040e590` calls `FUN_004704c0(car, local_34, local_30, local_2c, deg, &DAT_00803324)` with the grid
  point from `FUN_00408b00` (`verify/u9747_20261018/decomp_4704c0_40e590.txt`); `FUN_004704c0` writes y =
  `(float)(param_3 - FUN_0046d400(param_2, param_3, param_4, param_6))`. `FUN_0046d400` (`decomp_46d400.txt` of lk21):
  line start (x, y + 3, z), end (x, y - 3, z) (`_DAT_005cc31c` = 3.0), isect type 1, best = 6.0, `FUN_00538c80` with
  callback `FUN_0046d3a0`; returns best - 3 if best != 6.0 (`_DAT_005cd0a0` = 6.0) else 0 - 3. The callback keeps
  `dist * 6.0` when smaller and the triangle's material colour is neither 0xff0000ff nor 0xff010101 (`decomp_46d3a0_538c80.txt`).
  `FUN_00538c80` case 1 is `FUN_00538d60` (2961 bytes; calls `FUN_004c3b90` and the callback only).
- The port: `place()` (`TrackRenderer.cpp`) sets y = `GroundHeight(x, z) + car_ground_off_`; `FUN_00408b00`'s own y is not
  computed (`RoundGrid_AnchorAt` keeps x / z of the lateral and direction vectors). `FUN_00408b00`'s slot block
  (`verify/grid_orig_ts/FUN_00408b00_decomp.txt`, param_2 < 4): y = A.y -/+ fn.y*0.8 -/+ (fn.y*0.4)*0.8 - d.y*{0, 1.1, 2.2, 3.3}
  in the decomp's order (`_DAT_005ccac0` 0.4, `_DAT_005cc9bc` 0.8, `_DAT_005ccabc/b8/b4` 1.1 / 2.2 / 3.3).
- `BW_538d60` generated verbatim (`re/tools/naked_specs/collworld_line_u9755.json`, `CollisionWorldLine_naked.inc`, 819 lines;
  new constants `0x005ce54c` 1e-6, `0x005cea1c` -1e-5, `0x005e4574` 1e-8).

## 1. Hypothesis

H1: with `FUN_00408b00`'s y as `param_3` and `FUN_0046d400` over the collision replica, the port's spawn y equals the
original's bit for bit, and car 1's call-0 y gap (2.09e-7) and the drift it feeds (U-9755) shrink.

## 2. Knob and arms

`MASHED_SPAWN_Y_LINE=1` (default OFF): `place()` uses y = param_3 - (FUN_0046d400 - ...) as above for every car; the
caller convention adds `car_ground_off_`, and the stored value is the float whose difference with `car_ground_off_`
reproduces that y exactly. Type 1 is dispatched to `BW_538d60` under PC_24 like type 3. Arms (1200 frames, K0 / K1 /
K26): `SY` (default + knob), `SPLSBKNSY` (lk21 / u9754's `SPL` + `MASHED_SPAWN_BASIS_RW` + `MASHED_REASSERT_KEEP_NORMAL`
+ this knob), `SPLSBKNSYR1` (+ car-1 record), `TPPGSY` (spawn y log).

## 3. Gates

| gate | rule |
|---|---|
| G-NOOP | knob off: `B_DEF_K0/K1/K26` `e332af0c / 01ad5975 / 772f9fb7`, `H_DEF` trio unchanged |
| G-SPAWNY | `TPPGSY` car 1 countdown y = 0.4717099666595459 bit for bit |
| G-Y0 | `SPLSBKNSYR1` car 1 `+0x95c` at window call 0 = o_wobj2's 0.468502462 |
| G-DRIFT | `SPLSBKNSY` K1 car 1 x/z gap at call 21 < 1e-5 (now 1.2e-4); else report the next first-differing field |
| G-B | (b) `SY`: no car that passes on the default turns to FAIL |
| G-D2 / G-13 | only for a default proposal: D2 3/3, 13 tracks 0 RED |

## Amendment 1 (2026-10-18, after a first SY build, before the scored arms)

First build (`320b6727`): car 1's countdown y 0.471709996, 1 ulp from the original. New original-side probe
`scenario_launch.py --spawn-drop-probe` (entry / exit of `FUN_0046d400`, entry of `FUN_0046d3a0`), capture `o_sdrop1`:
caller `0x00470517` for all four cars, param_3 = 1.17499995 (`0x3f966666`) for every car, one hit each, best
`0x406f3466 / 0x406d02b4 / 0x406acd3a / 0x40689b88`; car 2's x / z are `FUN_00408b00`'s (z 0.202357411, the port's place()
0.202357292). The port's log (`MASHED_SPAWN_Y_LOG`, `B_SYL_K1`) has the same param_3 and the same four best bits, and
CollWorld_SpawnY returns 0.471709967 for car 1: the lost ulp was the caller convention (y + car_ground_off_ cannot carry y
back out). Changes, all under the knob: the exact y goes to `VehiclePhysics_SeedSpawnPose(..., physY)`, which also seeds
the d2posmodel `g_physPos` / `g_outPos` pair; x / y / z of the grid point follow `FUN_00408b00`'s order
(`RoundGrid_GridPoint`). Result on exe `388fd4b2`: `B_TPPGSY_K1` countdown pose of cars 0..3 = the original's probe
values bit for bit (G-SPAWNY PASS). All section-3 gates are scored on this exe; the earlier `SY` / `SPLSBKNSY` arms are
superseded.

## Amendment 2 (2026-10-18, after the scored arms, before any D2 / 13-track / G-CTRL arm)

Scored on `388fd4b2`: G-NOOP B dumps unchanged; G-SPAWNY PASS (all four cars); G-Y0 FAIL (`+0x95c` at call 0 0.468502671,
unchanged); G-DRIFT FAIL (car 1 1.2e-4 at call 21, unchanged); G-B `SY` A 2/3 B 2/3 C 0/3 (the default's set).
Cause of the call-0 y (`MASHED_SETTLE_DIAG`, `B_SPLSBKNSYD_K1` vs `B_SPLSBKND_K1`): the spawn settle's output for car 1 is
0.506262839 (physics 0.468762839) with spawn y 0.471709967 and with 0.471710175 alike; the original's frame-2 y after
`FUN_004704c0`'s `FUN_0046f6c0(0)` is 0.468762636 (o_sdrop1), then held until the first racing step. So the call-0 gap is
made in that spawn solver pass, not by the spawn y. The default candidate is `SY`: D2 `D2_SY_1..3`, the 13 tracks
(`run13.py` with `MASHED_SPAWN_Y_LINE=1`) and G-CTRL `H_SY`.
