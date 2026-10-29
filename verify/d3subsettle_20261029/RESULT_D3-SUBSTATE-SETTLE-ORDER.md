# RESULT: D3-SUBSTATE-SETTLE-ORDER (L-55, run 22)

Date 2026-10-29. Branch `orch/D3-SUBSTATE-SETTLE-ORDER` from `orch/D3-SUBSTATE-PREGO-ALIVE` @ 37f5a49cb. PREREG
`PREREG_D3-SUBSTATE-SETTLE-ORDER.md` committed alone at 97539653c, before any code. Amendment 1 committed alone at
ae442e7c6, after D2_SUB_1 on the first exe and before its own arms. Standalone only, no MASHED.exe launched, no Ghidra
MCP (decomp_pc.py headless), no .ps1 edited, no tracker touched, no default flipped.

## 0. Verdict first

> **DECISION. The settle order is fixed and it was the cause of the early-row shift: with MASHED_SUBSTATE the player's
> rows now equal the opt-out's byte for byte on rows 1..314 (before: different from row 1). D2_SUB is still OUT on the
> registered 30 s recipe, 0.2032 in / 0.2480 in / 1829.86 OUT x3. The remaining gap is the scoring window, not the physics.**
> The D2 demo leaves the race 30000 ms after race entry (exe_main.cpp `s_drive_hold`). Under SUBSTATE, phase 3 holds
> 763-776 frames inside that window, so a SUB arm logs ~918 rows where the opt-out logs ~1630. The opt-out cut to 924
> rows scores 0.2033 / 0.2489 / 1832.63 OUT, the same as SUB. With the window restored (`MASHED_DRIVE_HOLD_MS=42000`,
> amendment 1), D2_SUBH is **in band 3 of 3**: 0.1934 / 0.2515 / 1919.62 (1637-1639 rows). Default gates on the branch
> exe equal the refs.

| gate (final exe 58c21e05 = 440c1eac9, default, no knob) | result | ref |
|---|---|---|
| G-CTRL `H_DEF` | 5943c5f1 / 364350ae / 52052890 | this branch's base (PREGO). Integration after L-56 is 5943c5f1 / **306186d4** / 52052890; this branch does not contain L-56 |
| B `B_DEF_K0/K1/K26` | 3a03670c / 9fcdbd3a / 53778664 | same |
| D2 `D2_DEF_1..3` | 0.1942 / 0.2492 / 1896.74 in, 1629 lines x3 | same values |
| 13 tracks | not run. The change is SUBSTATE-only code plus a harness knob that defaults to the old value. The merge train's 13-track check covers it | merge_train.json |

## 1. The original's order (decomp_pc.py, excerpt `decomp_order_excerpt.txt`, raw decomp.txt sha in section 7)

- FUN_004111c0 case 5: `FUN_004103a0()` (`0x004112bb`), then `FUN_0040fc00()`. No vehicle step on a state-5 tick.
  Case 6: `FUN_00411170()` (`0x004112cd`), then `FUN_0040fc00()`.
- FUN_004103a0 exit call (`_DAT_005ccdf4` 1.86f `<=` t, `0x0041045c..0x0041046e`): loop 1 `FUN_0046baa0` (`0x0041043a`)
  + `FUN_0046d7f0` (`0x00410441`) per alive car, then loop 2 `FUN_0046d780` (`0x0041049b`) per alive car, then
  `DAT_0063ba8c = 6` (`0x004104e3`).

The port's spawn-settle has no RVA. It stands in for the pre-release hold ticks (d2engage). It now runs between
loop 1 and loop 2 of the exit call (before `0x00410470`), which is where the stand-in path runs it relative to its release.

## 2. The change (exe arm only, MASHED_SUBSTATE only)

| file | content |
|---|---|
| Race/SubStateMachine.h / .cpp | `SubStateHost::player_settle`, called once in the `t >= 1.86f` branch before the release loop |
| D3d9Render/TrackRenderer.cpp | binds it to `VehiclePhysics_SpawnSettle(0, dt, io)` with the stand-in GO frame's io and gate (`PosModelOrder() && VehiclePhysics_Enabled()`). `MASHED_NO_SUBSTATE_SETTLE_FIRST=1` opts out. The stand-in path is not edited |
| exe_main.cpp | `MASHED_DRIVE_HOLD_MS` (harness only, amendment 1): replaces the demo's 30000 ms drive hold when set > 0. Default unchanged |
| verify/savelink_20261023/run_arms.py | stacks `SUBNS` (SUB + opt-out), `SUBH` (SUB + `MASHED_DRIVE_HOLD_MS=42000`) |

Commits: 97539653c PREREG, bba24d7bf change (exe 298e7e4f), ae442e7c6 amendment 1, 440c1eac9 harness knob (final exe
58c21e05), plus this RESULT's commit.

## 3. Predictions

| id | result | verdict |
|---|---|---|
| P1 | D2_SUB_1..3 and D2_SUBH_1 (final exe): `settle slot=0 order=1 settleOnly=1`, then `bf4=3000 bf8=0`, `2800/2`, `2600/2 sp=0.64`. The opt-out's sequence | PASS |
| P2 | D2_SUB_1..3 (final): 0.2032 in / 0.2479-0.2480 in / 1828.41-1829.86 OUT. First exe: 0.2032 / 0.2472-0.2488 / 1827.21-1833.06 OUT | **FAIL** (drvmed) |
| P3 | default gates = base refs (table above) on both exes | PASS |
| P4 | D2_SUBNS_1 (final): 0.2016 / 0.2591 OUT / 1791.85 OUT, motion_diag sha8 23f1dbf4 = the PREGO post bytes. The first exe's D2_SUBNS_1 never reached GO (render 5.6 fps, wall-clock bound, 0 motion rows) and was re-run on the final exe | PASS |
| P5 (amendment 1) | D2_SUBH_1..3: 0.1934 / 0.2515 / 1919.62 in, 0.1934 / 0.2515 / 1919.62 in, 0.1933 / 0.2515 / 1917.10 in, 1637 / 1637 / 1639 rows | PASS |

Band (verify/d2rebase_20261019/band.json): slip1500 [0.18396, 0.20586], slip2000 [0.24555, 0.25557], drvmed
[1893.63, 1997.46]. Full table: `SCORE_D2.txt`.

Report only:
- Row identity against the opt-out (`D2_DEF_1` on the same exe, `launchrev release` lines removed): D2_SUB_1, D2_SUBH_1
  and D2_SUBH_3 are identical on rows 1..314. Row 315 differs in low digits (first exe: ftot z -5273.72 against
  -5273.65). **[UNCERTAIN]** the cause of the row-315 split. Evidence missing: a per-term trace of ftot on that row
  under both paths. It moves drvmed from 1896.74 (opt-out) to 1919.62 (SUBH), both in band.
- `B_SUB_K0` 1d5bf4f9 = the PREGO value (a 1200-frame B arm ends near GO under SUB).
- `H_SUB` changed: 3eddc272 / 8256f9d8 / 92cfa365, 3676 / 12941 / 6470 lines (PREGO: 8284be46 / c33e1c11 / 31fe043e,
  13962 step rows). Same 222-line game log on both, the same `R6 round 1 OVER t=18.3s winner=car3`, and both exited on
  their own (exit 0). This run exited after 135 s, PREGO's after 248 s. **[UNCERTAIN]** why the step dump is shorter.
  Evidence missing: a frame-by-frame diff of the two H_SUB step dumps. The FLIP's expected flip refs no longer apply to
  a SUB build that includes this change.

## 4. Why D2_SUB is still OUT

The driving sample is cut short, as measured: `[leg2] phase 3 released after 776 frames` (D2_SUB_1, first exe), and the
demo leaves the race at `el >= 30000` from race entry. The stand-in's countdown is shorter, so it gets ~1630 rows in the
same 30 s. Cutting the opt-out to SUB's 924 rows gives SUB's score (section 0). Restoring the window gives in-band
values. No other code change was made between P2 and P5 (same exe, only `MASHED_DRIVE_HOLD_MS` and a 110 s wall).

## 5. DECISION

Question: how is D2 measured for a MASHED_SUBSTATE build, given that the registered recipe's 30 s exit timer includes
phase 3?

- **(a) recommended**: accept P5 as the D2 evidence for SUB. The SUB D2 recipe carries `MASHED_DRIVE_HOLD_MS=42000`
  (or a later GO-anchored hold), and the default recipe stays as is. Merge this branch (default unchanged), then re-run
  D3-SUBSTATE-FLIP with refreshed SUB refs (H_SUB changed here). Caveat: P5 is an amendment registered after the first
  measurement, and the 42000 value is calibrated to ~776 phase-3 frames. A GO-anchored hold would remove that coupling.
- (b) keep the 30 s recipe as the D2 test for SUB. D2 then fails under SUB by construction (phase 3 eats ~13 s), and the
  flip stays parked.
- (c) score SUB arms with a fixed post-GO row count (scorer change, needs its own band check).

Evidence: this file, `SCORE_D2.txt`, the PREREG and its amendment.

## 6. Branch content (no tracker touched)

- the four source files and run_arms.py (section 2)
- `verify/d3subsettle_20261029/`: PREREG (+ amendment 1), this RESULT, SCORE_D2.txt, decomp_order_excerpt.txt,
  raw_sha256.txt, RUNLOG_first.txt (exe 298e7e4f), RUNLOG_final.txt (exe 58c21e05)

## 7. Raw (D:/MashedWT/D3-SUBSTATE-SETTLE-ORDER_data, sha256)

Full 64-hex list: `raw_sha256.txt` (25 files: both exes, decomp.txt, every cited gate / D2 / H_SUB file, the 924-row
control cut). Arm folders: `arms/` (first exe), `final/` (final exe). Private root `D:/MashedWT/D3-SUBSTATE-SETTLE-ORDER_root`.

## 8. Process

Every run inside `test-slot` and `mashed_lock.py run --label D3-SUBSTATE-SETTLE-ORDER` (D2 `--solo`). It queued behind
the merge train's solo window once. Every mashed_re PID was spawned by this step's runner. The D2 arms are ended at
their wall time by the runner killing its own PID (the D2 recipe). Nothing else was killed. No .asi deploy. `original\`
untouched.
