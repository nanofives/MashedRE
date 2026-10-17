# PREREG a4order part 2: U-9746, every car's A4 before any substep (2026-10-17)

Branch `work/a4order` from `race/first-frame-parity` `3e23c80b1`. Part 1 (its own commit) made `MASHED_AI_COUNTDOWN_CLOCK`
default ON (opt-out `MASHED_NO_AI_COUNTDOWN_CLOCK=1`). Every part-2 arm runs on top of that new default. Written and
committed before any part-2 arm runs. Model claude-opus-5-5, account2.

## 1. The original's order (from the bytes, `verify/d3bB2_20261017/disasm_470c70.txt`)

`FUN_00425a40`: AI tick `FUN_00418860` (`0x00425a73`), `FUN_00424eb0` (`0x00425a82`), then `FUN_00470c70` (`0x00425a8d`).
Inside `FUN_00470c70`, per chunk (`0x00470f50..0x00470f61`, `local_24 = min(remaining, 0x32)`):

1. the order refresh `0x00470e2e..0x00470f16` (once per call, before the chunk loop);
2. the per-vehicle loop `0x00470f80..0x00471099` (16 records, stride `0xd04`): the `+0xaec` gate
   (`0x0047101b..0x00471048`) and the A4 call `FUN_00470670` (`0x00471076`), for EVERY car;
3. the refresh loop `0x004710a1..0x004710da` (`+0x9e4 = |+0x9b0|`, `+0x9e8 = |+0x9bc|`), every car;
4. the substep loop `0x00471106..0x00471141`, `FUN_004709a0` at `0x00471137`, which steps the cars and runs the
   car<->car pair test (contact call `0x00470bcd`).

The exe steps one car at a time (gate + A4 + that car's substeps), player first, so an impulse the player's substeps
apply to an AI car (`0x00470bcd`) reaches that car BEFORE its A4 of the same frame.

## 2. The knob `MASHED_A4_ORDER=1` (default OFF)

- Before the player's `StepCar`, `VehiclePhysics_A4PreBegin`, then `VehiclePhysics_A4Pre(slot)` for every AI car the
  opponent loop will step (same skips: dead in round mode, spun out; same io, including the shock throttle), then
  `VehiclePhysics_A4PreEnd`. `A4Pre` runs `Fi_UpdateBoostOrder` (item 1) once, then the car's `StepCar` up to and
  including the A4 block (adapter IN, boost arm, gate, A4, refresh where the default makes it) and returns.
- The car's own `StepCar` later skips the gate, the A4 call and the refresh, keeps the record velocity the pre-phase's
  A4 and any later impulse left (it re-seeds only if the caller changed vel / speed after the pre-phase), and runs
  its substeps. `ReassertContacts` (port-only) stays next to the substeps.
- The torque ring phase each AI car's A4 uses is the one the per-car order gives it (player base + 1, cars + 2, + 3, + 4);
  the real step checks it and logs a mismatch to `mashed_re.log`.
- Not moved: a car's first step (the spawn settle hands io back), a frame budget over `0x32`, `MASHED_A8_A4_FIRST=0`.
  The player's A4 already runs before every substep (it steps first).
- **The knob implies `MASHED_AI_TICK_FIRST`.** The A4 loop reads the commands the AI tick wrote this frame
  (`0x00425a73` before `0x00425a8d`); in the exe's default order the AI tick runs after the player's step, so the AI
  cars' commands do not exist before the player's substeps. Consequence: "the knob alone" = T + the A4 order, and the
  arm "stacked with T" is the same arm.
- Trace `MASHED_A4_ORDER_TRACE=<path>` (one line per pre-phase car), measurement only.

## 3. Arms (`run_arms.py`, exe built with `MASHED_NO_DEPLOY=1`, snapshot on `D:\MashedWT\a4order_data\exe`)

| tag | knobs (on the new default) | purpose |
|---|---|---|
| DEF | none | G-NOOP and the baseline |
| T | `MASHED_AI_TICK_FIRST` | control: what the knob implies, without the A4 move |
| A | `MASHED_A4_ORDER` (+ trace) | the knob |
| AE | A + `MASHED_AI_CONTACT_EVENT` | the knob stacked with the event |
| TE | T + `MASHED_AI_CONTACT_EVENT` | control for AE |
| S | d3bB2's `S` stack (U-9684 chain, `MASHED_GRID_DIR`, event, tick first) | control: car 2's contact in the original's frame |
| SA | S + `MASHED_A4_ORDER` (+ trace) | the knob where the speed / coast comparison is defined |

D3 (b): `B_<tag>_K0` (A), `_K1` (B), `_K26` (C), scored with `vsum.py` / `re/tools/b_envelopes.json`.

## 4. Gates and expectations

| gate | measure | pass rule | expectation |
|---|---|---|---|
| G-NOOP | knob off on the part-2 exe: G-CTRL; `B_DEF_K1` dump | G-CTRL = `490c4b2d / 1fdd0ea6 / 46088ac4`; `B_DEF_K1` = d3bB2's `B_CC_K1` `5257c915` | PASS |
| G-TOOK | `A`, `SA` traces | `done=1` lines for slots 1..3 on racing frames; 0 ring-phase mismatch lines | PASS |
| G-SPEED | `SA_K1`, car 2 `rec_9e4` at window calls 24..32 (`sbs.py` against `o_c2r1`) | within 1.0 of 1050.409 on all 9 calls (S: 1038.302, 12.1 off) | PASS: the A4 force now precedes the impulse |
| G-COAST2 | `SA_K1`, car 2 `rec_9e4` at calls 60..70 | a constant run of >= 8 calls starting at k in [60, 62] (original 1341.140 on 61..69) | not predicted; 50/50 |
| D3 (b) | A, AE (and SA) per regime | default-ON recommendation only if no car that passes on DEF (A 1/3, B 2/3, C 0/3 expected) turns to FAIL, and (b) improves somewhere | not predicted |
| D2 | `D2_A_1..3`, each alone, 90 s, `verify/d2close_20261016/score_all.py d2` | slip 1500-2000 in [0.18855, 0.19635], 2000-2600 in [0.24488, 0.25487], driving-median in [1904.70, 1982.44], H1 398/400, >= 1600 `motion_diag` lines (re-run below) | PASS, unchanged: the D2 recipe runs the player solo (`MASHED_MEASURE_SOLO`), no AI car reaches the pre-phase |
| 13 tracks | `run13.py` (`cites_rres.json`), DEF and A | 0 RED; rows reported against the 867 of d3bB2's CC arm | DEF 867; A 0 RED, rows not predicted |
| G-CTRL | `H_A` | reported (not a gate) | moves |

A piz-collision AV (`MASHED.exe+0x1509b4` / `+0xe66db`) or a D2 run under 1600 lines is re-run, not scored.
