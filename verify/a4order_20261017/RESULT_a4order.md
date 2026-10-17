# RESULT a4order: countdown clock default ON (part 1) + U-9746, every car's A4 before any substep (part 2), 2026-10-17

Branch `work/a4order` from `race/first-frame-parity` `3e23c80b1`. Model claude-opus-5-5, account2. Every exe built with
`MASHED_NO_DEPLOY=1` and snapshotted on `D:\MashedWT\a4order_data\exe`: part 1 `fa36a37e`, part 2 `71db1824` (= commit
`087757284`'s source). Private roots `D:\MashedWT\a4order_root` / `_root2` / `_root13`. No original-side capture was taken; the
original references are d3bB2's `o_c2r1` / `o_pw1` (`.aistep.csv` in git, `.msd` on `D:\MashedWT\d3bB2_data\verify`).

## 0. Verdict

- **Part 1 done** (`7d3d0bef9`, merged by the orchestrator as `d9b8bfb45`): `MASHED_AI_COUNTDOWN_CLOCK` default ON, opt-out
  `MASHED_NO_AI_COUNTDOWN_CLOCK=1`. G-CTRL default `490c4b2d / 1fdd0ea6 / 46088ac4` x2, opt-out `4c81db89 / aa03f7ae / 7d3af534`
  (`P1_RESULT.md`).
- **Part 2: `MASHED_A4_ORDER` ported, default OFF, recommendation: keep OFF.** It is faithful where it was meant to act: regime B
  car 2's post-contact coast is now 1050.255 against the original's 1050.409 (the `S` control reads 1038.302). But the
  second coast at call 61 still does not happen (U-9749), and in regime A the knob trades car 3 for car 2 (car 3 passes on the
  default and fails under the knob), which the default-ON rule forbids.

## 1. Code (`087757284`)

`FUN_00425a40` runs the AI tick (`0x00425a73`) before `FUN_00470c70` (`0x00425a8d`). `FUN_00470c70` runs the order refresh
(`0x00470e2e..0x00470f16`), then for EVERY car the `+0xaec` gate (`0x0047101b..0x00471048`) and A4 `FUN_00470670`
(`0x00471076`), then the refresh loop (`0x004710a1..0x004710da`), then the substeps (`FUN_004709a0`, `0x00471137`), where the
car<->car contact runs (`0x00470bcd`). The exe stepped one car at a time (gate + A4 + substeps), player first.

Under `MASHED_A4_ORDER=1` (`VehiclePhysicsRun.cpp`, `TrackRenderer.cpp`):

- before the player's `StepCar`: `VehiclePhysics_A4PreBegin`, `VehiclePhysics_A4Pre(slot)` for each AI car the opponent loop
  will step (same skips and io), `VehiclePhysics_A4PreEnd`. `A4Pre` runs `Fi_UpdateBoostOrder` once, then the car's `StepCar`
  up to the end of the A4 block;
- the car's own `StepCar` skips the gate, A4 and refresh, keeps the record velocity (re-seeds only if the caller changed it
  after the pre-phase) and runs the substeps; `ReassertContacts` (port-only) stays next to the substeps;
- the ring phase each AI car's A4 takes is the one the per-car order gives it (checked in the real step, mismatch logged);
- a car's first step, a budget over `0x32` and `MASHED_A8_A4_FIRST=0` keep the per-car order;
- **the knob implies `MASHED_AI_TICK_FIRST`**: the A4 loop reads this frame's AI commands; in the exe's default order they
  do not exist before the player's substeps. So "the knob alone" = T + the A4 order, and T is its control.
- trace `MASHED_A4_ORDER_TRACE`.

## 2. Gates (PREREG `PREREG_a4order.md`, committed in `087757284` before any part-2 arm)

| gate | measured | verdict |
|---|---|---|
| G-NOOP | knob off on `71db1824`: `H_DEF-p2` `490c4b2d / 1fdd0ea6 / 46088ac4`; `B_DEF_K1` `5257c915` (= d3bB2 `B_CC_K1`) | PASS |
| G-TOOK | `done=1` on every pre-phase line except each car's first step (3 per run); ring-phase mismatch lines: `B_A_K1` 1, `B_SA_K0` 2, `B_SA_K1` 2, `B_SA_K26` 1, `H_A` 0 | FAIL on the registered "0 mismatch" item (see 2.1) |
| G-SPEED | `B_SA_K1` car 2 `rec_9e4` on calls 24..32: 1050.255 x9 (original 1050.409, max diff 0.154; `S` 1038.302) | PASS |
| G-COAST2 | no constant run at k = 60..62; car 2 runs on at 1954 / 1968 / 1981 (original 1341.140 on 61..69) | FAIL (U-9749) |
| determinism | `B_SA_K1` x2 `75255391`; `B_A_K1` x2 `af6cd307`; `B_SAP_K1` (+ player trace) = `B_SA_K1` | PASS |

### 2.1 The ring-phase mismatches

`B_A_K1.a4.txt` has 657 / 838 / 655 pre-phase lines for slots 1 / 2 / 3 (rule 4 rounds eliminate cars). A mismatch line means a
car had its pre-phase A4 and then did not run its own `StepCar` in that frame: the exe's race update between the player's step
and the opponent loop eliminated or spun it out. Its pre-phase A4 stays applied (the default never runs A4 for it on that
frame), and the later cars' ring phase is one ahead of the default's. Stated residual, not fixed.

## 3. Why the second coast is missing (U-9749)

Frames aligned on car 2's window (`PLAYER_SA_K1_vs_pw1.txt`, `SBS_SA_K1_car2.txt`):

- In the original the second coast is a player<->car 2 contact. On the aligned frame both records arm `+0xaec` to 480 (car 2
  `o_c2r1` frame 874, player `o_pw1` frame 905), and car 2's AI override reads 1440 at call 60 (type-4 event, U-9745). AI cars 1
  and 3 are not in contact there (car 3 is 1.85 from car 2).
- Under SA the port's player matches the original bit for bit through the first contact and coast (x 0.3035, `+0x9e4` 1237.92
  on k = 23..31). On the first frame its A4 runs again (k = 32) the original's speed drops 23.81 and the port's 6.56. From
  there the player drifts: x 0.0946 against 0.1402 at k = 59 with car 2 within 0.002 of the original, so the contact does not
  happen. Car 2's own first A4 frame after its coast (k = 33) shows the same class: +26.0 on the original, +34.5 on the port.
- So the remaining cause is the first A4 frame after a `+0xaec` skip, on both cars. Not chased further here.

## 4. D3 (b) per arm (`vsum.py`, `re/tools/b_envelopes.json`; `arms/p2/SCORE_B.txt`)

| arm | knobs (on the new default) | A | B | C | notes |
|---|---|---|---|---|---|
| DEF | none | 1/3 | 2/3 | 0/3 | A: car 3 passes, car 2 `c0#` 41; B car 2 `c0#` 39 `c1#` 49 `steer#` 87 |
| T | tick first (control for A) | 1/3 | 2/3 | 0/3 | same verdicts as DEF (dump differs) |
| **A** | `MASHED_A4_ORDER` (implies T) | 1/3 | 2/3 | 0/3 | **A: car 2 PASSES, car 3 turns to FAIL** (`abs_steer~` 0.0 [7, 23]); B car 2 40 / 47 / 86 |
| TE | T + event (control for AE) | 0/3 | 1/3 | 0/3 | B car 2 `c0#` 39 only |
| AE | A + event | 0/3 | 1/3 | 0/3 | B car 2 `c0#` 62 `steer#` 93; car 3 fails |
| S | d3bB2 `S` (control for SA) | 0/3 | 0/3 | 0/3 | B car 2 39 / 48 / 86 / `abs_steer~` 17.5 |
| SA | S + A | 0/3 | 0/3 | 0/3 | B car 2 `c1#` 50 `steer#` 87 (`c0#` now in bounds); B car 3 `c1#` 51 only |

Default-ON rule: a car that passes on DEF (A car 3, B cars 1 and 3) may not turn to FAIL. Under A, A car 3 does. **Not eligible.**

## 5. G-CTRL, 13 tracks, D2

| item | measured |
|---|---|
| G-CTRL knob off (`H_DEF-p2`) | `490c4b2d / 1fdd0ea6 / 46088ac4` (= the new default) |
| G-CTRL knob on (`H_A`) | `541e9656 / 02ea8d9c / 32dd6520`, 0 ring-phase mismatch lines |
| 13 tracks DEF (`run13.py`, `cites_rres.json`) | 39 frames, 867 rows, 0 RED, 39/39 CITED; byte-identical score file to d3bB2's CC |
| 13 tracks A | 867 rows, 0 RED, 39/39 CITED. Two frames move, both `C27-CAR-PART-OFFSET`: Roundabout 1091 33 -> 34 rows (316 -> 312 draws matched), Training 884 21 -> 20 (213 -> 215) |

D2 (`D2_A_*`, each alone, 90 s, `verify/d2close_20261016/score_all.py d2`; `arms/d2/SCORE_D2_a4order.txt`):

| run | lines | slip 1500-2000 | slip 2000-2600 | driving-median (n) | H1 |
|---|---|---|---|---|---|
| `D2_A_1` | 1286 | (0.1915) | (0.2486) | (1871.51 OUT, n 1076) | NOT SCORED: under the 1600-line floor, re-run as `D2_A_1r` |
| `D2_A_1r` | 1630 | 0.1890 | 0.2476 | 1912.04 (1420) | 398/400, 1362.7 |
| `D2_A_2` | 1631 | 0.1890 | 0.2480 | 1912.04 (1421) | 398/400 |
| `D2_A_3` | 1630 | 0.1890 | 0.2476 | 1912.04 (1420) | 398/400 |

3/3 in bounds. `D2_A_1r` and `D2_A_3` are byte-identical to d3bB2's `D2_CC_3` (`59461ead`): the D2 recipe runs the player solo,
so no AI car reaches the pre-phase and the player's order is unchanged.

## 6. Other findings

- **U-9748 (new):** the original increments the torque ring phase `DAT_007f101c` once per `FUN_004111c0` call (`0x004111c1`,
  unconditional at entry; other writers `0x0040ff17`, `0x00492784`, none in `FUN_00470c70` / `FUN_00470670`), so every car's A4
  reads one value per physics call (`0x00470776`). The port advances it once per car step: with 4 cars each car writes 4 of the 16
  slots of `+0x1ac` / `+0x270`. The port's only reader is the car-part steer visual (`FUN_0046bda0`, mean of 16). Solo runs are
  unaffected. `datarefs_7f101c.txt`.
- **d3bB2's CCET arms ran without `MASHED_AI_TICK_FIRST`.** `run_arms.py`'s `<tag>T` loop (`STACK[_t + 'T'] = ...`) replaced
  `STACK['CCET']` with CCE + the two trace knobs; every d3bB2 `CCET` RUNLOG line (H, B x2, D2 x3) carries no tick-first knob. So
  d3bB2's CCET rows (G-CTRL `33b5eab2 / 2b13c757 / f3e8e2f9`, (b) "identical to CCE", D2) are CCE with traces. Here TE (= what CCET
  meant) scores the same (b) verdicts as d3bB2's CCE. The loop in this copy no longer overwrites a defined tag.

## 7. Tracker transaction (re-classify, no C-level change; `apply_trackers_p2.py`)

New U-9748, U-9749 (U-9747 was the highest id on `3e23c80b1` and on `race/first-frame-parity` at `d9b8bfb45`); U-9746 note;
hooks.csv notes on `00470c70`, `00470670`, `004111c0`; one CHANGELOG line. Part 1's transaction: `apply_trackers_p1.py`.

## 8. User decisions needed

1. `MASHED_A4_ORDER` default: recommended OFF. It is the original's order and fixes car 2's coast speed, but regime A trades
   car 3 for car 2 and the second coast still needs U-9749.

## 9. Follow-ups

- U-9749 first: diff the player's record on the original (`o_pw1.msd`) and the port around k = 31..33 for the fields A4 / A5 /
  A6a read; with it the k = 61 coast is the next check.
- U-9748: one ring phase per race frame behind a knob, then re-read the C27 car-part rows.
- The pre-phase residual of 2.1 (a car eliminated or spun out after its pre-phase A4) if the knob is ever proposed for default.

## 10. Provenance

`arms/RUNLOG.txt` (part 1), `arms/p2/RUNLOG.txt`, `arms/h/RUNLOG.txt`, `arms/d2/RUNLOG.txt` (exe sha8, knobs, pid, exit, output
sha8). Scores: `arms/p2/SCORE_B.txt`, `arms/d2/SCORE_D2_a4order.txt`, `tracks_{def,a}_scores_diff.txt` (13-track outputs on
`D:\MashedWT\a4order_data\tracks\{def,a}`). Cited dumps in git: `B_SA_K1.step.csv`, `B_A_K1.step.csv`, the D2 `motion_diag` logs
(force-added). Everything else under `arms/` moved to `D:\MashedWT\a4order_data\verify` (`MOVED_OFF_GIT.txt`, sha8 + size).
