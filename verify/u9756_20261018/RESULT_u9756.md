# RESULT u9756: the spawn solver pass y (2026-10-18)

Branch `work/u9756` from `race/first-frame-parity` `01f36a4c3` (default G-CTRL `b7534619 / fb690fb1 / d3a20f4c`). Model
claude-opus-5-5, account2. PREREG `PREREG_u9756.md` with the knobs (`f10c75605`), amendments 1 (`25fcb74ef`), 2 and 3, each
committed before the arms it governs. Exe `c35c47d0` (`MASHED_NO_DEPLOY=1`), private root `D:\MashedWT\u9756_root`. Original
references: o_wobj2, u9754's o_seed1, new `o_sp1` / `o_sp2` (write watches at spawn).

## 0. Verdict

1. **U-9756 is resolved: a mistranscribed constant.** `_DAT_005cea60` is 0.2778f (`0x3e8e3bcd`); the port's `kWTorque`
   (`Collision/WheelContactSolver.cpp`), `kProbeScale` (`Vehicle/VehiclePhysicsRun.cpp`) and the unused `kWhl_TorqueScale`
   held 0.277779f (`0x3e8e390d`). It scales the suspension mounts into the wheel contact points, so on a slope the depths and
   the y correction drift. With **`MASHED_FIX_5CEA60`** (default OFF):
   - the spawn solver pass equals the original's: car 1 wheel-0 contact x -1.26823068 (o_sp2 -1.268230676651001), car 0
     0.33392337 (0.3339233696460724), wheel-0 depth 0.00939599983 (o_sp1 0.00939599983394146), y 0.468762934
     (0.46876293420791626);
   - car 1's y at window call 0 equals o_wobj2's 0.468502462 (the U-9755 / U-9756 gap closes).
2. **The order of the original's countdown** (o_seed1 / o_sp1 watches): `FUN_004704c0` twice at race start (place +
   `FUN_0046f6c0(0)`, which corrects the `+0x968` half, + `FUN_0046baa0`), then on every hold tick `FUN_0046baa0` and the
   physics frame `FUN_00470c70` (integrate `0x0046ea6c`, solver correction `0x0046fbf5`), which reach a fixed point. The port's
   single GO settle reaches the same y once the constant is right.
3. **Car 1's drift is unchanged** (call 4 2.4e-7, call 21 1.2e-4). The next first-differing fields are the `+0x968` half:
   the original keeps it one substep behind (`+0x99c` 0.468632549 against the port's 0.468502462 at call 0, `+0x970` 2.17e-5 /
   4.33e-5 at call 1), and the solver reads that half (`+0x9ac` = 1). This is U-9152's single-pose deviation.
4. **A second constant, `_DAT_005cea44`**, is 0.27777779f (`0x3e8e38e4`); `kTer_VelScale2` (car-vs-world contacts) held
   0.277779f. `MASHED_FIX_5CEA44` (default OFF) keeps (b) but moves the player's D2 out of band (slip 2000-2600 0.2549,
   driving-median 1880): new U-9757.
5. **Default-ON candidate: `MASHED_FIX_5CEA60` alone.** (b) dumps unchanged, D2 3/3, 13 tracks 0 RED; G-CTRL moves to
   `b40b47bc / fb690fb1 / d3a20f4c` (AI step rows only). Recommendation: default ON (user decision).

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | `B_DEF_K0/K1/K26` `c7b40cf2 / 5b9905d8 / f680a529`; the spawn diagnostic leaves dumps unchanged | PASS |
| G-WCP | wheel-0 x car 1 / car 0 = o_sp2 | PASS |
| G-PASS | wheel-0 depth and y out = o_sp1 | PASS |
| G-Y0 | `SPLSBKNF60R1` `+0x95c` at call 0 = o_wobj2 | PASS |
| G-DRIFT | car 1 1.2e-4 at call 21 | FAIL (next: the `+0x968` half, U-9152) |
| G-B | `F60` dumps = default; `F44` / `F6044` A 2/3 B 2/3 C 0/3 | PASS |
| G-D2 | `F60` 3/3 (0.1939-0.1944 / 0.2545 / 1960.31-1963.42, H1 398/400); `F6044` and `F44` OUT | `F60` PASS |
| G-13 | `F60`: 39 frames, 869 rows, 0 RED, instrument gates PASS | PASS |
| G-CTRL | `H_F60` `b40b47bc / fb690fb1 / d3a20f4c`; `H_F44` `2e547429 / d85e2213 / dd428bdb`; `H_F6044` `8cf76141 / 7c0a3e9e / b35f90ee` | reported |

## 2. Code

- `Collision/WheelContactSolver.cpp` `kWTorque`, `Vehicle/VehiclePhysicsRun.cpp` `kProbeScale`: `MASHED_FIX_5CEA60`.
- `Collision/CarWorldContacts.cpp`: `MASHED_FIX_5CEA44` at the `kTer_VelScale2` use.
- `Vehicle/VehiclePhysicsRun.cpp`: `MASHED_SPAWN_SOLVER_DIAG` (the original's countdown tail on a copy of the spawned record).

## 3. Tracker transaction (re-classify, no C-level change; `apply_trackers.py`)

U-9756 RESOLVED; U-9152 note (the `+0x968` half is the next drift driver); new U-9757 (`_DAT_005cea44` and D2); hooks.csv note on
`0046f6c0`; one CHANGELOG line.

## 4. User decision needed

`MASHED_FIX_5CEA60` default ON (opt-out at the flip)?
