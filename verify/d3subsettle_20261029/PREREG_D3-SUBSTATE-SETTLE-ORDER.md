# PREREG: D3-SUBSTATE-SETTLE-ORDER (L-55, run 22)

Date 2026-10-29. Branch `orch/D3-SUBSTATE-SETTLE-ORDER` from `orch/D3-SUBSTATE-PREGO-ALIVE` @ 37f5a49cb (that branch
merges as is under L-55). Committed alone, before any code change and any arm. Background:
`verify/d3subprego_20261029/RESULT_D3-SUBSTATE-PREGO-ALIVE.md` section 4.

## 1. The gap

With `MASHED_SUBSTATE` the player's state-5 release runs at `0x0041049b` (SubStateMachine.cpp, exit call of
FUN_004103a0), and the port's spawn-settle step then runs inside the first race StepCar (`settleOnly=0`, `bf8=2` on its
row). The stand-in path places the settle BEFORE `LaunchRev_Release()` on the GO frame (TrackRenderer.cpp, d2engage fix
B part 4, `MASHED_POS_ORDER`): with the opt-out the settle runs at `bf8 0`, the release follows, then the race step.
Measured first motion_diag rows (PREGO RESULT section 4): opt-out `3000/0` then `2800/2`, SUB `2900/2` then `2700/2`.

## 2. The original's order (decomp_pc.py, no Ghidra MCP; excerpt `decomp_order_excerpt.txt`)

FUN_004111c0 dispatcher on DAT_0063ba8c:
- case 5: `FUN_004103a0()` (`0x004112bb`), then `FUN_0040fc00()`. No vehicle step on a state-5 tick.
- case 6: `FUN_00411170()` (`0x004112cd`, return `0x004112d2`), then `FUN_0040fc00()`.

FUN_004103a0, exit call (the one where `_DAT_005ccdf4` (1.86f) `<= t`, `0x0041045c..0x0041046e`):
1. loop 1 per alive car: `FUN_0046baa0` (`0x0041043a`, hold reset), `FUN_0046d7f0` (`0x00410441`, charge);
2. loop 2 per alive car: `FUN_0046d780` (`0x0041049b`, release), stores into `0x007f0a04 + v*12`;
3. `DAT_0063ba8c = 6` (`0x004104e3`).

So every hold tick (loop 1) precedes the release on the same call, and the first race step follows on a later tick.
The port's spawn-settle has no RVA of its own: it is the exe's stand-in for the pre-release hold ticks (d2engage). Its
faithful place under SUBSTATE is therefore between loop 1 and loop 2 of the exit call, which is where this step puts it.

## 3. The change

Exe arm only, reachable only with `MASHED_SUBSTATE` set:
- `SubStateHost` gains `player_settle`. SubStateHeldCountdown5_C calls it once inside the `t >= 1.86f` branch, before
  the release loop (`0x00410470`).
- TrackRenderer binds it to the same body the stand-in runs on its GO frame (`VehiclePhysics_SpawnSettle(0, dt, io)`
  with the stand-in's io, gated on `PosModelOrder() && VehiclePhysics_Enabled()` as there).
- `MASHED_NO_SUBSTATE_SETTLE_FIRST=1` removes it (same exe control arm).
- The stand-in path is not edited. No default flipped. `MASHED_SUBSTATE` stays default OFF.

## 4. Arms

Runner `verify/savelink_20261023/run_arms.py`, stacks `SUB` (exists) and a new `SUBNS` = `SUB` +
`MASHED_NO_SUBSTATE_SETTLE_FIRST=1`. One exe (this branch's commit). Private root
`D:/MashedWT/D3-SUBSTATE-SETTLE-ORDER_root`, raw data `D:/MashedWT/D3-SUBSTATE-SETTLE-ORDER_data`, every run inside
`test-slot` and `mashed_lock.py run --label D3-SUBSTATE-SETTLE-ORDER` (D2 arms `--solo`). Standalone only.

Arms: `D2_SUB_1..3`, `D2_SUBNS_1`, `D2_DEF_1..3`, `H_DEF`, `B_DEF_K0/K1/K26`; report-only `B_SUB_K0`, `H_SUB`.

## 5. Predictions

| id | arm | prediction |
|---|---|---|
| P1 | `D2_SUB_1..3` | motion_diag starts `settle slot=0 order=1 settleOnly=1`, its first data row has `bf4=3000 bf8=0`, the next `bf4=2800 bf8=2` (the opt-out's charge sequence) |
| P2 | `D2_SUB_1..3` | `score_d2_v2.py port`: slip1500, slip2000 and drvmed all `in` the band (verify/d2rebase_20261019/band.json) on 3 of 3 |
| P3 | default, knob off | `H_DEF` 5943c5f1 / 364350ae / 52052890 (this branch's base; the integration ref after L-56 is 5943c5f1 / 306186d4 / 52052890, which this branch does not contain), `B_DEF_K0/K1/K26` 3a03670c / 9fcdbd3a / 53778664, `D2_DEF_1..3` 0.1942 / 0.2492 / 1896.74 in band |
| P4 | `D2_SUBNS_1` (control, same exe) | 0.2016 / 0.2591 OUT / 1791.85 OUT, motion_diag sha8 23f1dbf4 or 56dcfd8d (the PREGO post bytes) |

Verdicts: P2 PASS and P3 PASS = GREEN. P3 FAIL = RED. P2 FAIL with P1/P3 PASS = DECISION (the order is not the whole
of D2's shift). Report only: whether P2's values equal the opt-out's 0.1942 / 0.2492 / 1896.74 exactly; line count
against `d2_min_lines` 1600; `B_SUB_K0` / `H_SUB` against the PREGO SUB hashes (1d5bf4f9; 8284be46 / c33e1c11 / 31fe043e).

## Amendment 1 (committed after D2_SUB_1 on exe 298e7e4f / bba24d7bf, before any arm below)

D2_SUB_1 measured: P1 PASS (first rows `3000/0`, `2800/2`, `2600/2`, the opt-out's charge sequence). Score 0.2032 in /
0.2488 in / 1833.06 OUT at 924 lines. Its rows equal the opt-out `D2_DEF_1` (PREGO post, `launchrev release` lines
removed) on rows 1..314; row 315 differs in the last digits (ftot z -5273.72 against -5273.65). The opt-out log cut to
its first 924 lines scores 0.2033 / 0.2489 / 1832.63 OUT: the same as D2_SUB_1. The game log shows why the log is short:
`[leg2] phase 3 released after 776 frames`, and the demo leaves the race at a fixed 30000 ms after race entry
(exe_main.cpp `s_drive_hold ... el >= 30000`), so phase 3 takes ~13 s out of the driving window.

Added, harness only: `MASHED_DRIVE_HOLD_MS` (exe_main.cpp, default 30000 = unchanged; values <= 0 or unset keep 30000).
New stack `SUBH` = `SUB` + `MASHED_DRIVE_HOLD_MS=42000` (30000 + ~776/60 s), D2 wall 110 s (`A4_D2_WALL`).

| id | arm | prediction |
|---|---|---|
| P5 | `D2_SUBH_1..3` | motion_diag >= 1600 lines and slip1500, slip2000, drvmed all `in` the band on 3 of 3 |
| P3' | default, knob off, final exe | as P3 (H_DEF, B_DEF_K0/K1/K26, D2_DEF_1..3) |

P2 is still scored as registered (the 30000 ms window). The verdict rule is unchanged: P2 FAIL = DECISION, whatever P5
shows. P5 is the evidence for the decision's options.
