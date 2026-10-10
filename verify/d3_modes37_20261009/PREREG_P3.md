# PREREG: D-11072 leg P3, `FUN_00414a70` + `FUN_00414300` on the live path progress

Date 2026-10-10. Committed before any P3 code. Scope: `PREREG_PATHPROG.md` §2 (leg P3).

## 1. Inputs check (static, done before code)

`FUN_00414a70(spline, &look, &cand, v)` (`0x00414a70..0x00414c2e`, 447 B) and `FUN_00414300`
(`0x00414300..0x0041448d`, 398 B) read:

| callee | reads | exe status with `MASHED_PATHPROG` |
|---|---|---|
| `FUN_00408a50` | `0x008a96e8 + v*0x30c` | LIVE (P2, byte-identical replay) |
| `FUN_00426bb0` | `DAT_0066d6d8` | LIVE (P1, 30 on Training) |
| `FUN_0040e370` / `FUN_0040e470` | `*(0x005f2770) + 0x34 + v*4` | LIVE (seeded; gates `p5f2770 = 0x005f2728`) |
| `FUN_0046d4a0` | record `+0x928 + t*0x40`, then `+0x30/+0x38` | LIVE (`Vehicle/VehicleRecordPtr.cpp`, exe-rebound, already used by branch 2) |
| `FUN_0046d6d0` | `0x00881f84` = record `+0x9e4` | LIVE, see below |
| `FUN_0046d510` | `(0,0,1)` (`0x00614708`, file bytes `0,0,1.0`) by the `+0x928` matrix, into record `+0x9d4` | substrate LIVE (matrix); body has no exe binding |
| `FUN_004c3ac0` / `FUN_004c3b30` | RW sqrt LUT `[*0x007d3ff8 + *0x007d3ffc]` | `Math/RwLutTables.cpp` (as P2) |

Speed (the task's "`+0xbc` write"): `0x00881f84` is record `+0x9e4` in the array-base convention
(`VehicleRecordBase.h`). It has TWO writers in the original, both `|record+0x9b0|`:
- `0x0046766e call 0x004c3ac0` / `0x00467673 fstp [esi+0x9e4]`, the head of `FUN_00467650`
  (Integrate2). Ported: `Vehicle/Integrate2.cpp:129`. Live in the exe.
- `0x004710c3` / `0x004710c8 fstp [esi+0x9d4]` with `esi = 0x008815b0 + k*0xd04` (= record `+0x10`),
  so record `+0x9e4` again, in `FUN_00470c70`'s loop `0x004710b0..0x00471104` over 16 records,
  gated `[rec+4] != 0 || [rec+0x10] == 1`, after the per-car step loop (`0x00470f80..0x00471099`)
  and before the `FUN_004709a0` substeps (`0x00471137`). **Not ported.** The port runs car-car inside
  `VehiclePhysics_StepCar`, so the original's insertion point has no exact equivalent.
Decision: the tail is NOT ported in this leg (it would also change `ControlStep`'s existing speed
read and confound the score). Its effect on `FUN_00414a70` is measured instead (G-P3-SPEEDTAIL).

`FUN_0046d510` side effect: it writes the transformed vector into record `+0x9d4`. The exe shim
returns the matrix `at` row (exact for `(0,0,1)` up to the sign of zero) and does NOT write the
record; G-P3-INPUTS counts how often the `at` row differs from the record's `+0x9d4`.

## 2. Port shape

Verbatim naked x87 via `re/tools/gen_naked_asm.py`, new spec `re/tools/naked_specs/target_p3.json`:
`FUN_00414a70`, `FUN_00414300`, leaves `FUN_00408a50`, `FUN_00426bb0`, `FUN_0040e370`,
`FUN_0040e470`, `FUN_004c3ac0`, `FUN_004c3b30` verbatim (LUT base patched like P2). C++ shims with the
original's stack contract: `FUN_0046d4a0`, `FUN_0046d6d0`, `FUN_0046d510`. Runs under
`_controlfp_s(_PC_24)`. Knob `MASHED_TGT3` (default OFF), active only with `MASHED_PATHPROG`.
Wiring in `Ai/AiStandalone.cpp` `ControlStep` as `0x004162f7..0x004163e6`: `FUN_00414570` /
`FUN_00415880` stay stubbed (the original returns 0 on 6696 / 6696 calls in `o_wobj2`, so
`FUN_00414a70` is reached every call, as in the port). `ret == 1` and LOS -> `look = cand`, mode 3;
`ret == 1` and no LOS -> the `FUN_00414c30` arm (stubbed, 0) -> `FUN_004150e0` (stubbed);
`ret == 2` -> `ctrl[4] = 0`, `ctrl[5] = 0xff`, return (`0x004163f6..0x00416405`).

## 3. Gates and predictions

- **G-P3-OFF.** Knob OFF: `ae67b13a / c8317ce2 / 31db9384` (run_h4.ps1 `P3_CTRL`). By construction.
- **G-P3-REPLAY (transcription gate, before any score is read).** New original-side probe
  (`scenario_launch.py --tgt-probe`, entry + leave on `0x00414a70`) records every input in §1 for the
  4 cars plus the return value and `cand` (`param_3[0..1]`) at exit. The port replays each call
  through the naked body. PASS = return value AND `cand` bits identical on 100% of calls (`cand`
  compared on `ret == 1` calls; on the others the port must leave `cand` untouched). PC_24 is the
  gate; PC_53 reported as a diagnostic. Prediction: PASS at PC_24 (no cross-call state, 2 sqrt
  calls through the verified LUT).
- **G-P3-INPUTS (report).** Knob ON, per call: progress in [0, 30], node count 30, speed > 0 on moving
  cars, own `at` row vs `+0x9d4` bit mismatches counted.
- **G-P3-SPEEDTAIL (report).** On every knob-ON call, the `1500.0` predicate (`0x00414bab` /
  `0x00414bbc`, `0x005cd0ac`) evaluated with `+0x9e4` and with `|+0x9b0|` read at the call; count
  flips. 0 flips in the window => the unported tail is measured-harmless on this scenario.
- **G-P3-COUNT (the score).** Per car, `FUN_00414a70 == 1` and `== 2` counts in the (b) window
  (first 220 calls from the first `c4 != 0`, `ai_ctrl_window.py`'s definition), joined to the step
  dump by `(frame, v)`. Original (regime A, `o_wobj1` = FINDINGS §1): car 1 16 / 0, car 2 35 / 10,
  car 3 12 / 26. Rule per car per kind: PASS if `|port - orig| <= max(5, 0.5 * orig)`.
- **Prediction for G-P3-COUNT.** Open-loop model `model_14a70.py` on `P2_LIVE` (knob ON, no P3):
  car 1 27 / 0, car 2 27 / 95, car 3 0 / 104. Closed loop the brake arm (`ret == 2`) slows the car,
  which opens the gap, so predicted closed-loop `== 2` counts for cars 2 and 3 are BELOW the open-loop
  95 / 104. Predicted verdict: car 1 `==2` PASS, car 2 `==1` PASS, at least one `==2` count FAIL (the
  port's window starts at frame 0 on the grid with no countdown, so the cars are bunched).
- **(b) per regime** with `ai_ctrl_window.py --check-regime re/tools/b_envelopes.json` (report, and
  regime-A car 2's `|steer|` median: predicted to move off 0 only if car 2 gets `ret == 1` calls
  with LOS inside its window).
- **Collateral** `collateral.py` with the D2 watch list (memory `d2-parked-watch-triggers`).

No default flip in this leg. Knob stays OFF regardless of the score.
