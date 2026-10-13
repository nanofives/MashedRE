# PREREG: the car-2 opening-pickup miss (D3, mode-7 carrier)

Date 2026-10-13. Branch `work/car2pickup`, base `d7f5a32b`. Committed before any new run (original or exe) and before
any code change. Carrier: RESULT_M3 finding 2, RESULT_U9219 §3.2.

## 0. Measured before this file, from captures already in the repo (no run)

Tools in this folder: `traj.py` / `allcars.py` (car positions, both sides), `speed.py` (AI step rows).

1. **The pickup, original side (G-PICK input).** `o_cb1..3.msd.cb.tsv` (RESULT_M3 §1): car 2 collects pool 4 (type 12)
   through `FUN_00458920`'s collect arm, witness `c010:2:12` (`FUN_0045c010(2, 12)`), slot `+0xa8` 0 -> `0x5f9a58`.
   Pool 4 sits at (1.88176906, 0.669412076, -3.09718895), radius 0.5 (cb row `entry_in`). The car slot at the collect
   (`slot_b`) is (0.946209, 0.409745, -2.774814), radius 0.533389, 3-D distance **1.023047** < sum 1.033389.
   `o_rd3.msd.rd.tsv` (the `FUN_00442a60` per-car inputs) puts car 2 at exactly that position on driving tick 42
   (clk 41900, first `ba8c == 6` row = tick 0 at clk 39800). Same pickup on the exe side: the exe pool slot 1 holds the
   same position bit for bit (`M3B_ON.registry.csv` frame 0).
2. **Exe car 2's closest pass** (`M3B_ON.registry.csv`): 1.098253 at registry frame 33, never inside 1.033389.
   Exe registry frame f matches original tick f-1 on the first 22 ticks (per-tick z deltas equal to 1e-4).
3. **Grid pose differs before anything moves.** Car positions at the first driving row, exe minus original:
   car 2 dx +0.0157, dy +0.0402, dz -0.0083; car 1 dx +0.0079; car 3 dx +0.0235; player x equal, dy +0.0403.
   Exe `U_CTRL.step.csv` row frame 0 (substate 6, speed 0) own (1.15241814, 0.194150925) vs original parked
   (1.13672280, 0.20244953). [UNCERTAIN: whether the exe registry/own position is the same quantity as the original's
   matrix translation at `+0x958`; the original's `reg.tsv` car slot equals its `rd.tsv` matrix row bit for bit, the
   exe's equality is asserted only by the `car_pos_eq` column.]
4. **The contact.** On both sides car 2 runs into the parked player car 0 at the same tick (original tick 22 /
   aistep frame 821, exe registry frame 23 / step frame 23). What follows differs:
   - original car 2: speed `+0x9e4` 2374.48 -> **1050.41**, then **exactly 1050.409424 for 9 ticks** (frames 822..830),
     position advancing by a constant (+0.002118, -0.001523, -0.048543) per tick; car 0 takes the rest of the momentum
     and also moves at constant velocity for the same 10 ticks. Car 2's AI command: `c1` (brake) 0 -> **255** on
     frames 821..836, `c0` 21 -> 15, 10, 5, 0. Momentum along z is conserved to 5% (car 2 + car 0 after = -0.1023,
     car 2 before = -0.1077 per tick).
   - exe car 2: speed 2375.12 -> 2399.86 -> 2424.26, no drop, `c1` stays 0. Car 0 is pushed
     (-0.0185, 0, -0.0499 per tick) and decelerates tick by tick (not constant).
   - after the coast the original's car 2 holds x ~1.02 and passes the pickup at 1.0153..1.0230; the exe's keeps
     curving to x 0.82 and passes at 1.0983.
5. **Bytes read so far** (`decomp_phys.txt`, `re/tools/findoffset.py`, `disasm_va.py` on `MASHED.exe.unpatched`):
   - `FUN_004709a0` loops over all 16 vehicles **inside each substep**: vehicle i runs `FUN_0046e9e0`, `FUN_0046f6c0`,
     `FUN_00469aa0` / `FUN_0046ef70`, then the `j = i+1..15` pair loop calling `FUN_00469df0` (`0x00470bcd`). So the
     impulse written to car j by car i's contact is in car j's record before car j integrates the same substep.
     `FUN_00470670` (A4, which calls A6a `FUN_00467650`) runs once per vehicle per frame, outside the substep loop.
   - The port runs the whole frame (A4 + both substeps + pair loop) per car in `VehiclePhysics_StepCar`
     (`VehiclePhysicsRun.cpp:900-1221`).
   - `FUN_00469df0` stores `0x1e0` (480) to `+0xaec` on both cars if it is 0 (`0x0046ac6c..0x0046ac88`).
     `+0xaec` at a plain displacement: reads only at those two tests; writes `0x0046bc7e` (0, record reset) and
     `0x0046c543` (480, another function). No decrement found at a plain displacement (memory
     findoffset-blind-to-computed-bases: a computed base would be missed). 480 / 50 clock units per tick = 9.6, i.e.
     positive on exactly 10 ticks, which matches the original's 10-tick coast. [UNCERTAIN: this is a numerical match
     only. The reader that makes the coast is not identified.]

## 1. Runs this leg will make

- Original (read-only, explicit PIDs, the `o_rd3` argv with `--statediff-car 2` instead of 1, plus `--cb-probe`):
  `o_p2a`, `o_p2b` (car 2's 0xd04 record per render tick), and `o_p0a` (`--statediff-car 0`, the player). The car
  recorded is an observation choice; no input is forced beyond the `o_rd3` recipe. A collision AV at
  `MASHED.exe+0x1509b4` / `+0xe66db` voids the run and it is re-run.
- Exe: the `run_h4.ps1` recipe inline, default build, with the default-OFF witness logs `MASHED_CARCAR_LOG`,
  `MASHED_REGISTRY_TRACE` (+ `MASHED_REGISTRY MASHED_PICKUPS_ORIG MASHED_REG_CARS MASHED_REG_CALLBACKS` for the pickup
  path). Arms named in RESULT.

## 2. Gates

| gate | pass rule | prediction |
|---|---|---|
| **G-PICK** | the original's car 2 collects pool 4 (type 12) at driving tick 42 +-1 in 2 / 2 new runs, `c010:2:12`, slot `+0xa8` -> `0x5f9a58`; the exe's car 2 does not collect pool 4 in the first 60 frames (closest pass reported) | PASS on both sides (already measured in `o_cb1..3` + `M3B_ON`) |
| **G-COAST** | in both new car-2 captures, record fields that change at the contact frame and change back 10 +-1 ticks later are listed; exactly one field holds a value that steps by the clock (50 per tick) from 480 down, OR none does | the field is `+0xaec` (480, 430, ..., 30, then <= 0) [prediction from the 480 / 50 match only] |
| **G-CAUSE** | the first car-2 quantity on the path to the pickup whose exe value differs from the original's by more than the port's own determinism floor (0) AND whose correction alone (replay or knob arm) brings the exe's closest pass to pool 4 inside 1.033389, named with its producer RVA by code site | the contact response on car 2 at the contact tick (speed drop + coast), producer `FUN_00469df0` impulse on the OTHER car and the coast gate [UNCERTAIN which]. The 1.6 cm grid offset (§0.3) is predicted NOT to be sufficient alone |
| **G-FIX** | after the fix: exe car 2 collects pool 4 (type 12) within +-3 ticks of the original's tick 42 on the M3B_ON / U_M4ON recipe; car 2's held word nonzero from that tick; car-2 mode-7 returns of 2 inside the original's window 20 -> 0 and over the whole run 52 -> fewer [count not predicted] | PASS only if G-CAUSE names a port defect |
| **G-CTRL** | a fix behind an opt-out: the opt-out arm reproduces `ae67b13a / c8317ce2 / 31db9384`. A fix behind a default-OFF knob: knob OFF reproduces them | PASS |
| collateral (report) | `collateral.py --mode banded` cross-side, floor = a same-arm repeat; D2 watch fields | no prediction |
| (b) per regime (report) | canonical scorer, merged version if it has landed, version named | no prediction |

## 3. Decisions, written before the outcome

1. G-CAUSE names **port-only scaffolding** (call order, an overwrite, a missing call that the original makes) and the
   correction is a transcription of the original's bytes: fix it in the default code path with a `MASHED_NO_*` opt-out
   (ROADMAP v3 rule), run G-FIX, G-CTRL (opt-out), collateral, (b). STOP for the user's decision anyway if the change
   moves any D2 watch field past its floor (D2 is closed; a D2-visible change is the user's call).
2. G-CAUSE names a **C2 body transcription error** with a hook: fix it, run its C2->C3 legs (run_diff + self-baseline +
   negctl + witness), hand the path2 keys to the orchestrator.
3. G-CAUSE names an **unported reader** (e.g. the coast gate is a function the exe never calls): port it from the bytes
   behind a default-OFF knob and STOP for the user's decision (new behaviour, D2-visible).
4. G-CAUSE is the grid pose (§0.3) or anything upstream of the contact: no fix in this leg; name the producer, STOP.
5. No candidate brings the closest pass inside 1.033389: G-CAUSE FAIL, report the chain as far as measured, no fix.
