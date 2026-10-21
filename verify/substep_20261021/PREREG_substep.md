# PREREG substep: FUN_004709a0's substep-outer / car-inner order as explicit phase functions (2026-10-21)

Branch `work/substep` from `92de86464` (fflip default). Mariano's decision (2026-10-21): replace u9778's fiber interleave
(`MASHED_SUBSTEP_INTERLEAVE`, `Vehicle/SubstepInterleave.cpp`) with a restructure of `VehiclePhysics_StepCar` into phase
functions driven by a substep-outer / car-inner loop like FUN_004709a0. New knob `MASHED_SUBSTEP_OUTER=1`, default OFF.
The fiber code stays as the reference. Written before any knob arm. Evidence goes to `D:/MashedWT/substep_data`.

## 1. The original's frame (MASHED.exe.unpatched, capstone, `verify/u9778_20261021/cslist.py 004709a0 00471600`)

Caller FUN_00425a40 (0x00425a72..0x00425a99): FUN_00418860, FUN_00424eb0(param, 0x803324) (car 0's FUN_0046c7d0 heading assist),
**FUN_00470c70(param, 0x803324) at 0x00425a8d**, FUN_00422ba0, FUN_004252c0. FUN_0040fc00 calls FUN_00425a40 at 0x0040fc6c, and
FUN_004111c0's sub-state-6 case calls at 0x004112d5, after the round tail FUN_00411170 at 0x004112cd (u9779).

FUN_00470c70 (param_1 = budget 0x32, param_2 = &DAT_00803324):
- Frame pre-part, once: DAT_007f0f64 / 68 / 6c (0x00470c73..0x00470cc1); the DAT_007f0fd0 == 1 / == 2 event branches
  (0x00470cc7..0x00470da1); DAT_007f1018 (0x00470da4..0x00470e03); FUN_00467350 (0x00470e0d), FUN_0047eb30 (0x00470e15); the boost
  order DAT_0088e660..66c (0x00470e1a..0x00470f16, port `Fi_UpdateBoostOrder`); suspension globals 0x0088e610 / 0x0088e5f0
  (0x00470f1c..0x00470f3a); param_1 == 0 skips the chunk loop (0x00470f40).
- Chunk loop 0x00470f50..0x00471151: `chunk = min(rem, 0x32)` (0x00470f50..0x00470f61), then
  - A4 loop over all 16 cars 0x00470f80..0x00471099: rec+0 = slot, rec+0xd00 flag (FUN_0040e470 / FUN_00443080), input
    descriptor choice, the +0x14 / +0x4 / +0x10 gate (0x00470ffe..0x00471019), +0xc = 0 (0x00471023), the +0xaec gate
    (0x0047101b..0x00471048), FUN_00470670(rec, (float)chunk, desc, param_2) at 0x00471076;
  - refresh loop over all 16 cars 0x004710a1..0x004710fc: +0x9e4 = FUN_004c3ac0(+0x9b0) (0x004710c3 / 0x004710c8), +0x9e8 =
    FUN_004c3ac0(+0x9bc) (0x004710d5 / 0x004710da), FUN_0046dc20(slot) when (float)+0xc passes _DAT_005ceaac (0x004710f4);
  - substep loop 0x00471106..0x00471141: `sub = min(rem, 0x19)` (0x00471110 / 0x00471117), FUN_004709a0((float)sub, param_2) at
    0x00471137, no car argument, `rem -= sub` (0x0047113f);
  - `rem -= chunk` (0x00471143..0x00471151).
- Frame post-part, two loops over all 16 cars, same +0x14 / +0x4 / +0x10 gate:
  - 0x00471157..0x00471314: ring frame end (0x00471195..0x004711af: if +0x9a8 != 0 swap the selectors and copy +0x928 over
    +0x968), +0xaf4..+0xafc = _DAT_005cd058 x the sum of 8 vec3 (0x004711b1..0x00471244), the +0xaf0 timer and the 0x007e9de0
    event push (0x0047124a..0x004712fa);
  - 0x0047131a..0x004713e9: +0x18 timer (0x00471341..0x0047134f), +0x14 != 0: FUN_0046da80 -> +0x9e0, FUN_0046f6c0(0) at 0x00471388,
    FUN_00480b70 (0x00471391); +0x18 == 0: FUN_00480720 (0x004713b2), FUN_004809e0 (0x004713d1).
  - DAT_0080333c / 338 / 334 / 340 = 0 (0x004713f2..0x00471410).

FUN_004709a0 (param_1 = (float)sub, param_2):
- Once per call: mode = FUN_0040e350 (0x004709a3), count = FUN_0040e340 (0x004709ac), DAT_007f0fdc -= param_1 clamped at 0
  (0x004709b5..0x004709d6).
- Car loop i = 0..15 (0x004709e5, 0x00470c47..0x00470c53), per car: descriptor choice (0x004709f0..0x00470a4f); the gate
  (0x00470a53..0x00470a69), +0x9f0 == 0xff skip (0x00470a9b); src = [+0x9a8] half, dst = [+0x9ac] half (0x00470a6f..0x00470a8f);
  the retry body (ebp < 2 at 0x00470ab0): FUN_0046e9e0 (0x00470ad3), FUN_0046f6c0(1) (0x00470ae0), FUN_00469aa0 (0x00470ae8),
  FUN_0046ef70 if +0x9ec != 0 (0x00470afe), retry if the scan reported (0x00470b06 / 0x00470b0a); the car-car loop j = i+1..15
  (0x00470b0d..0x00470c1f, proximity 0x00470b44..0x00470ba9, mode 6 / 7 / 0xa / 0xb 0x00470baf..0x00470bc1, FUN_00469df0 at
  0x00470bcd, ebp += 1 per contact 0x00470bde, FUN_00467300 0x00470c0a), retry if any contact (0x00470c25 / 0x00470c2b); the
  selector swap (0x00470c31..0x00470c41) only on the committed path.

So every car's A4 runs before any car's substep 1, every car's substep 1 runs before any car's substep 2, and the frame end
runs for every car after the last substep.

## 2. The port today

`VehiclePhysics_StepCar` (`Vehicle/VehiclePhysicsRun.cpp:1051..2461`) runs one car's whole frame:
- pre-part (to :1697): the A5 race timer copy, the spawn settle (port-only, a nested StepCar on the car's first step), the
  record adapter IN, the start-boost arm (port-only seed), `Fi_UpdateBoostOrder` at slot 0, the suspension globals, the body
  basis, then per chunk the +0xaec gate (slot 0 gates every slot), A4 (`VehicleControlIntegrate`), the post-A4
  `ReassertContacts` (port-only, off under the seeded POS_MODEL), the +0x9e4 / +0x9e8 refresh;
- substep body (:1791..:2071): A9 position / orientation, `SolveWheelContacts`, the car-world scan and fixup, the car-car
  loop j > slot, the retry, the ring selector swap (RING_DBUF);
- chunk tail (:2075..:2104): the `MASHED_A8_A4_FIRST=0` A4 (opt-out only), `chunkRem -= chunkL`;
- frame tail (:2105..:2461): `RingFrameEnd`, the safety clamp, `drive_delta`, diagnostics, the adapter OUT.

`TrackRenderer::UpdateCar`: the AI tick (`ai_tick_phase`, AI_TICK_FIRST); the A4 pre-phase (`MASHED_A4_ORDER`, FAITH seed:
`VehiclePhysics_A4Pre` runs each AI car's StepCar up to the end of its A4 / refresh); the player's heading assist; `StepPlayer`
(:5333); `UpdateRace` (:5568, unless RACE_TAIL_LATE); the opponent loop (:5624) with its port-only skips (dead :5629, spun
:5633) and the port-only stuck relocation (:5648..:5668, a stand-in for FUN_00470c70's unported respawn branches), then each AI
car's StepCar (:5712).

The fiber form (u9778) runs the player and the pre-phase's AI cars in fibers that yield after each committed substep, before
`UpdateRace`, and the opponent loop takes each AI car's result. A car the loop skips or relocates, or whose io differs, is
restored from a snapshot (`VehiclePhysics_IlSnapshot` / `IlRestore`) and stepped per car. Fiber order per round: car 0
pre-part + substep 1, car 1 pre-part + substep 1, ... then substep 2 of each car, then each tail.

## 3. The restructure

- `StepCtx` (per slot): every local of today's StepCar that lives across a phase boundary (io, record, flags, frameMs,
  basis, posDelta, input bytes, chunk / substep counters, a4Skip, the settle-diag flag) plus a pointer to the car's
  `ContactScratch` (today's file-static `g_cOnMesh / g_cNormal / g_cAvgN / g_cGrounded`).
- Phase functions: `StepBegin` (pre-part to the chunk loop), `ChunkDue`, `StepChunkHead` (gate, A4, refresh),
  `SubstepDue` / `StepSubstep` (one substep, car-car loop and swap included), `StepChunkTail`, `StepEnd` (frame tail).
- Knob OFF: `VehiclePhysics_StepCar` = the same phases in the same order, with the shared scratch, so the default build is
  byte-identical. The fiber form keeps working (its yield sits between substeps, as today).
- Knob ON (`MASHED_SUBSTEP_OUTER=1`): `VehiclePhysics_StepCarsOuter(n, slots, dt, ios)` drives the cars like FUN_00470c70:
  every car's `StepBegin`, then per chunk every car's `StepChunkHead`, then substep rounds over the cars in slot order, then
  every car's `StepChunkTail`, then every car's `StepEnd`. Each car has its own `ContactScratch`. The call site is the fiber's:
  same cars, same snapshot / restore for skipped, relocated or changed-io cars.
- Mechanism controls (diagnostic): `MASHED_SUBSTEP_OUTER_ORDER=car` runs the phases one car at a time (per-slot scratch),
  `MASHED_SUBSTEP_OUTER_ORDER=fiber` runs them in the fiber's yield order (pre-part fused with substep 1).
- Port-only parts keep today's observable behaviour: the spawn settle stays inside the car's pre-part (as the fiber
  suspends yields there), the stuck relocation and the skips stay in the opponent loop with the fiber's snapshot / restore.

## 4. Predictions

| id | arm | prediction |
|---|---|---|
| P0 | knob off, after every refactor step (`B_DEF_K0`; G-CTRL `H_DEF` at the end) | B K0/K1/K26 `7ab70c5f / 15262e4a / d1b4bc64`, G-CTRL `ce087436 / b0a2973b / c4f7a000` (a moved hash is a refactor bug) |
| P1 | `MASHED_SUBSTEP_OUTER_ORDER=car` (`SXC`) | = knob off: B `7ab70c5f / 15262e4a / d1b4bc64`, G-CTRL = `H_DEF` |
| P2 | `MASHED_SUBSTEP_OUTER_ORDER=fiber` (`SXF`) | = the fiber form: B `9bd86ffe / 433abcd2 / 1a1ff727`, G-CTRL = `H_IL` |
| P3 | `MASHED_SUBSTEP_OUTER=1` (`SX`) | = the fiber form on B and G-CTRL. Basis: the two orders differ only in where cars 1.. run their pre-part. On a steady frame that pre-part (A4 already done by the A4 pre-phase) writes only the car's own +0x9d4..+0x9dc, +0x4 (already 1) and, with an unchanged io, no velocity. FUN_00469df0's port reads none of +0x9d4..+0x9dc (grep `Collision/CarCarContacts.cpp`). On the first physics frame the settle, the A4 and the start-boost arm of cars 1.. move ahead of car 0's substep 1, which matters only if a car-car contact happens in that frame's substep 1 |
| P4 | `SX` (b) on B K0/K1/K26 | A 3/3, B 3/3, C 3/3 (= the fiber form) |
| P5 | `SX` io mismatch lines (`[u9778] interleave:` in `mashed_re.log`) | 0 on every B arm and on `H_SX` |
| P6 | `SX` D2 x3, alone | PASS 3/3, values = the default's (D2 runs one car, so the driver falls back to the plain step, as the fiber form does) |
| P7 | `SX` 13 tracks (`run13.py`, recipe F) | 0 RED, every row cited, instrument gates PASS |
| P8 | G-CTRL knob off, final build | `ce087436 / b0a2973b / c4f7a000` |

If P3 fails, the lane finds the first diverging operation with the u9778 tools on K0 (`ccdiff.py` / `ccring.py` against
`o_cc3_1`, `velcmp.py`) and states which of the two orders the original's captures support. Neither knob is flipped.

Not measured before this PREREG: G-CTRL with the fiber form on recipe F (`H_IL`). It is measured in this lane and is P2 / P3's
reference.
