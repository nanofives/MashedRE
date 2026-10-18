# PREREG u9756: the spawn solver pass y (2026-10-18)

Branch `work/u9756` from `race/first-frame-parity` `01f36a4c3` (default G-CTRL `b7534619 / fb690fb1 / d3a20f4c`). Model
claude-opus-5-5, account2. Private root `D:\MashedWT\u9756_root`. Written before any knob arm.

## 0. Measured before this PREREG

- Original order (u9754 `o_seed1.msd.watch.csv`, write watch on car 1 `+0x204` / `+0x95c`): `FUN_004704c0` runs twice at race
  start (from `0x0041016c` and `0x00410311`): place (y 0.4717099667, `0x004c52cd`), classifier normal (`0x0046cff8`, return
  chain ending `0x0046f827`), `FUN_0046baa0` (`0x0046bb36`). Then every hold tick (return `0x00410343`): `FUN_0046baa0`, the
  physics frame `FUN_00470c70`: integrate `FUN_0046e9e0` writes y 0.46876293420791626 (`0x0046ea6c`, return `0x00470948`),
  solver correction writes 0.4687626361846924 (`0x0046fbf5`). Later hold ticks reproduce the same y.
- Spawn solver pass (new `o_sp1`, watch on car 1 `+0x99c` / `+0x194`): its correction lands in the `+0x968` half
  (`0x0046fbf5`, 0.4717099667 -> 0.46876293420791626); wheel-0 depth written by the classifier (`0x0046d008`)
  0.00939599983394146.
- Port, same pass (new `MASHED_SPAWN_SOLVER_DIAG`: `FUN_0046baa0` + `SolveWheelContacts(..., 0)` on a copy of the spawned
  record under `MASHED_POS_MODEL`): y 0.471709967 -> 0.468763143, wheel-0 depth 0.00939529203. The step dumps are unchanged
  with the diagnostic on (`B_SPLSBKNSSD_K1` = `022d760c`).
- Wheel-0 contact point (new `o_sp2`, watch on `DAT_0088e620` / `24`): original car 1 x -1.268230676651001, car 0
  0.3339233696460724; port -1.26821804 / 0.333935976 (1.26e-5 off), y equal.
- Cause: the suspension mounts are scaled by `_DAT_005cea60` = 0.2778 (`0x3e8e3bcd`, read from the anchored binary); the port's
  `kWTorque` (`Collision/WheelContactSolver.cpp:51`, used at `:105` / `:111`), `kProbeScale` (`Vehicle/VehiclePhysicsRun.cpp:357`)
  and `kWhl_TorqueScale` (`ContactConstants.h:59`, unused) hold 0.277779f (`0x3e8e390d`). Separately `_DAT_005cea44` is
  0.2777778 (`0x3e8e38e4`) and the port's `kTer_VelScale2` (`ContactConstants.h:43`, used in `CarWorldContacts.cpp:275`)
  also holds 0.277779f.

## 1. Hypothesis

H1: with `_DAT_005cea60`'s value, the spawn pass's contact points, depths and y equal the original's, and car 1's call-0 y
and its drift close. H2 (separate): `_DAT_005cea44`'s value moves the car-vs-world contact response only.

## 2. Knobs and arms

- `MASHED_FIX_5CEA60=1` (default OFF): `kWTorque` and `kProbeScale` = 0.2778f.
- `MASHED_FIX_5CEA44=1` (default OFF): `kTer_VelScale2` = 0.27777779f.
Arms (1200 frames, K0 / K1 / K26): `F60` (default + 60), `F44` (default + 44), `F6044`, `SPLSBKNF60` (`SPL` +
`MASHED_SPAWN_BASIS_RW` + `MASHED_REASSERT_KEEP_NORMAL` + 60), `SPLSBKNF60SSD` (+ the spawn diagnostic), `SPLSBKNF60R1`
(+ car-1 record).

## 3. Gates

| gate | rule |
|---|---|
| G-NOOP | knobs off: `B_DEF_K0/K1/K26` `c7b40cf2 / 5b9905d8 / f680a529`, `H_DEF` `b7534619 / fb690fb1 / d3a20f4c` |
| G-WCP | `SPLSBKNF60SSD` car 1 / car 0 wheel-0 contact x = -1.268230676651001 / 0.3339233696460724 |
| G-PASS | same arm, car 1 wheel-0 depth 0.00939599983394146 and y out 0.46876293420791626 |
| G-Y0 | `SPLSBKNF60R1` car 1 `+0x95c` at window call 0 = o_wobj2's 0.468502462 |
| G-DRIFT | `SPLSBKNF60` K1 car 1 x/z gap at call 21 < 1e-5 (now 1.2e-4); else the next first-differing field |
| G-B | (b) on `F60`, `F44`, `F6044`: no car that passes on the default turns to FAIL |
| G-D2 / G-13 | for any default proposal: D2 3/3, 13 tracks 0 RED, G-CTRL reported |

## Amendment 1 (2026-10-18, after the section-2 arms, before any D2 / 13-track arm)

Results on `c35c47d0`: G-NOOP PASS. G-WCP PASS (car 1 / car 0 wheel-0 x -1.26823068 / 0.33392337). G-PASS PASS (depth
0.00939599983, y out 0.468762934). G-Y0 PASS (`+0x95c` at call 0 equal to o_wobj2). G-DRIFT FAIL (call 4 2.4e-7, call 21
1.2e-4); next first-differing fields: the `+0x968` half (`+0x99c` 0.468632549 / 0.468502462 at call 0, `+0x970` 2.17e-5 /
4.33e-5 at call 1), which the original keeps one substep behind and the port sets to the current pose (U-9152 deviation);
the solver reads that half (`+0x9ac` = 1). G-B: `F60` B dumps equal the default's; `F44` A 2/3 B 2/3 C 0/3 (the default's set).
`H_F60` `b40b47bc / fb690fb1 / d3a20f4c`. Default candidate: `F6044` (both constants): D2 `D2_F6044_1..3`, 13 tracks with both
knobs, G-CTRL `H_F6044`.

## Amendment 2 (2026-10-18, after D2_F6044, before these)

`D2_F6044_1..3`: slip 0.1960 in, slip 2000-2600 0.2549 OUT (> 0.25487), driving-median 1880.50 / 1882.46 / 1880.50 OUT
(< 1904.70); H1 398/400. `H_F6044` `8cf76141 / 7c0a3e9e / b35f90ee` (the player trace moves). `F6044` is not a default
candidate. Attribution only: `D2_F60_1`, `D2_F44_1`, `H_F44`.

## Amendment 3 (2026-10-18, after the attribution arms, before these)

`D2_F60_1` in band (0.1939 / 0.2545 / 1960.31, H1 398/400); `D2_F44_1` OUT (0.2549 / 1880.00); `H_F44` moves the player
trace (`dd428bdb`). `_DAT_005cea44` is the D2 mover and stays a default-OFF knob. Default candidate: `F60` alone: `D2_F60_2..3`,
13 tracks with `MASHED_FIX_5CEA60=1`.
