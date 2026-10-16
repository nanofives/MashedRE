# PREREG d2posmodel: fix B, the exe's car position model made the original's

Branch `work/d2posmodel`, base `232f9f76` (race/first-frame-parity). Model claude-opus-5-5 (account2).
Committed BEFORE the first measuring run of this lane. Amendments are appended and committed before the
runs they govern. Brief: orchestrator 2026-10-16 (port "fix B" of `verify/d2settle_20261016/RESULT_d2settle.md`
sections 2, 3, 5). D2 is un-parked for this work (user decision 2026-10-16); every D2 watch-list hit is reported.

## 0. Facts read before any run

0.1 **Mechanism (d2settle, committed).** The original's first physics tick moves every car by
(0, -0.0029474, +0.0000921), x bit-identical, from `FUN_0046baa0`'s seeds (states 1, fv 0) followed by
`0x0046f6c0` latching all four wheels, dropping the most-loaded one and translating the ring `+0x30` by
-(1/3)·Σ fv·n. D0-e (port solver, real wheel points, seeds, original frame-1 position) gave a correction of
(0, -0.00295186, +0.0000922680) and a position x exact, z within 1 ulp, y 4.4e-6 from the original's frame 2.

0.2 **Spawn y convention, from the decomp (this lane, `re/tools/decomp_pc.py`, pool slot 10, read-only).**
`FUN_0040e590` (race-start setup) per car: `FUN_0046b1c0`, `FUN_0046b540` (A3), `FUN_0046baa0` (call at
`0x0040ed6e`), the grid `FUN_00408b00(...)` -> (x, y, z), then `FUN_004704c0(slot, x, y, z, orient, &DAT_00803324)`.
`FUN_004704c0`: `fVar6 = FUN_0046d400(x, y, z, world)`, ring translation y = `local_8 = (float)(y - fVar6)`, set into
both halves `&DAT_00881ec8 + slot*0x341` (= record `+0x928`) and `&DAT_00881f08 + slot*0x341` (`+0x968`), then
`FUN_0046f6c0(0)` (call at `0x00470638`), `FUN_0047ea60`, `FUN_0041eda0`, and `FUN_0046baa0` last.
`FUN_0046d400`: a segment from (x, y + 3, z) to (x, y - 3, z) (`_DAT_005cc31c` = `0x40400000` = 3.0) through the
BSP walk `FUN_00538c80` with callback `FUN_0046d3a0` and accumulator initialised to 6.0; the callback keeps
`t * 6.0` (`_DAT_005cd0a0` = `0x40c00000` = 6.0) for the smallest hit fraction t whose material word is not
`0xff0000ff` / `0xff010101`. Returns `acc - 3.0` on a hit, `DAT_005d757c (0.0) - 3.0` otherwise. On a hit the
spawn y is therefore y - ((y + 3 - y_hit) - 3) = y_hit, the first non-sentinel surface below y + 3 at the
car's centre (x, z). So the original's physics y at a spawn is the ground under the car centre.
The exe's spawn is `car_pos_[1] = GroundHeight(x, z) + car_ground_off_` (`TrackRenderer.cpp` grid `place`,
`car_ground_off_ = -model.bbox[1]`), so `car_pos_[1] - car_ground_off_` is the same convention up to the exe's
`GroundHeight` picking the same surface. No change to the exe's spawn code is needed for the convention; the
offset is removed and re-added at the physics adapter (section 1, part 3).
Original frame-1 y 0.43742681 (`0x3edff668`); exe frame-1 y 0.474926978 on the RG arm (`D_RGXS.settle.txt`),
whose difference is `car_ground_off_` up to the two `GroundHeight` evaluations.

0.3 **`FUN_0046baa0` (port `VehiclePhysicsStateReset`, `Race/SubStateCallees.cpp`, C3 GREEN 9/9) writes**, among
others: the slot index at `+0x0`; int 1 at `+0x198/+0x25c/+0x320/+0x3e4` (`0x0046bb23`); 0 at `+0x194/+0x258/+0x31c/
+0x3e0`; 1.0f at `+0x204/+0x2c8/+0x38c/+0x450`; 4.0f at `+0x9e0` (`0x0046bb57`); 0 at `+0x9b0..+0x9c4`, `+0x9e4`,
`+0x9e8`, `+0x9ec`, `+0x490`, `+0x494`, `+0xaec`, `+0xb0c`, `+0xb14..+0xb1c`, `+0x14..+0x4c`; -1 at the 18 contact
keys `+0x4ac + i*0x40`. It does NOT write `+0x4`, `+0x10`, `+0xb20`, `+0xbf4`, `+0xbf8`, `+0x9a8`, `+0x9ac`.

0.4 **Exe today.** No physics during the countdown (the countdown branch of `TrackRenderer::UpdateCar` returns
first). The first `StepCar` per slot runs a one-shot spawn-settle step whose output is discarded except the record.
`SolveWheelContacts` (`VehiclePhysicsRun.cpp`) seats every wheel at `groundY - 0.05` (stand-in), sets the ring
centre to (x, mean wheel ground y, z), and `SyncContactRingMatrix` overwrites the solver's translation with
`io.pos`. `TrackRenderer` sets `car_pos_ = car_pos_ + drive_delta` in x/z and `car_pos_[1] = GroundHeight + car_ground_off_`
every frame, for the player and the three AI cars. `io.pos[1]` is never integrated.

0.5 **D0-a settle step (d2settle `D_DEF.settle.txt`).** With the record as memset, the settle step's A4 leaves
vel (0, -216.668, 0) and `+0xb20 = 1`; the writer of that `+0xb20` with an all-zero input block is not identified
(`Integrate2.cpp` writes it at the drive and brake arms, `VehicleControl.cpp` at the steer arms). The original's
first ticks have vel 0 at A6a entry and at the substep entry (`o_fx1.msd`).

## 1. The change (all default OFF in this lane until section 5 says otherwise)

Knobs, header-only `Vehicle/PosModelKnobs.h` (value `1` = on): `MASHED_POS_SEED` (part 1), `MASHED_POS_WHEELS`
(part 2), `MASHED_POS_TRANS` (part 3), `MASHED_POS_MODEL` (all three).

- **Part 1, seeds.** `VehiclePhysicsStateReset(slot)` (the whole `FUN_0046baa0` port) immediately before the spawn-
  settle step, i.e. before the slot's first solver call. The exe has no pre-race physics ticks, so this is its one
  hold tick. STATED deviation: the original seeds and steps on every hold tick; the exe steps once. The exe's round
  restarts re-run no record reset with or without the knob (an existing gap, not touched).
- **Part 2, real suspension points.** The `MASHED_RW_WHEELPOS` arm of `WheelContactSolver.cpp`: the four
  `g_suspScratch` points through the ring matrix into `DAT_0088e620`, which the classifier reads. Before the solver,
  `SolveWheelContacts` copies the CURRENT body basis rows into the active ring (the port publishes them only after
  the solver). The ring translation stays whatever `SolveWheelContacts` seats (part 3 decides).
- **Part 3, the solver's translation is the car's position.** Inside `StepCar` `io.pos` is the physics position:
  adapter IN takes it from the slot's last physics position when the caller round-tripped the last OUT bit for
  bit, else (teleport: spawn, grid, off-mesh recovery) from the caller as (x, pos[1] - ground_off, z), with
  `ground_off = car_ground_off_` from `TrackRenderer`. The substep integrates y as well as x, z (A9 stores all three,
  `0x0046ea5f / 0x0046ea69 / 0x0046ea73`). The ring centre is seated at `io.pos` (not ground level), and after
  `0x0046f6c0` the corrected centre is copied back into `io.pos`; `SyncContactRingMatrix` then writes that same value.
  The settle step's position is kept (its velocity is still discarded). Adapter OUT returns
  (x, y + ground_off, z); `TrackRenderer` (player and AI) takes `io.pos` instead of `car_pos_ + drive_delta` with a
  snapped y. Off-mesh detection and recovery are unchanged.
- Diagnostics (default OFF, write nothing when unset): `MASHED_SETTLE_DIAG` gains a `SOLVER_IN` line (entry states,
  fv, `+0x9e0`, wheel 0/2 contact positions the classifier read) and `goff` / `physy0` on the `SETTLE` line.

## 2. Arms (`run_arms.py`, recipes identical to d2settle's; exe `MASHED_ROOT` = private copy of `original/`)

Stage per part, then together. Stacks: `DEF` (no knob), `P1`, `P2`, `P3`, `P23` (parts 2+3, no seed), `B` (all),
`BC` (all + fix C `MASHED_SPEED_REFRESH`), `RG` / `BRG` (PATHPROG + ROUND_GRID + ORDER, without / with B).

| arm | what | role |
|---|---|---|
| `D_DEF`, `D_P1`, `D_P2`, `D_P3`, `D_P23`, `D_B`, `D_BRG` | 300 frames, settle diag + ptrace | knob-took, G-SETTLE, G-SPAWNY, G-PROG |
| `H_DEF`, `H_DEF-2` | G-CTRL 14,400 frames, no knob | no-op control + determinism + collateral floor |
| `H_P1`, `H_P2`, `H_B`, `H_B-2`, `H_BC` | G-CTRL | per part / together; `H_B-2` = determinism + collateral floor |
| `D2_DEF_1`, `D2_P1_1`, `D2_P2_1` | D2 90 s | same-build control, per part (report) |
| `D2_B_1..3`, `D2_BC_1..3` | D2 90 s | gates (B), fix C re-test (BC) |
| `B_DEF_K{0,1,26}`, `B_B_K{0,1,26}` (+ `B_BC_K*` if BC passes D2) | D3 (b) 1200 frames | report |

`P3` and `P23` are diagnostic arms only (300 frames): neither is a candidate, see 4.3 / 4.4.

## 3. Gates

Scored by `g_settle.py` (G-SETTLE, G-SPAWNY, G-PROG), `verify/d2settle_20261016/score_d2.py` (D2),
`re/tools/statediff/a8_launch.py --orig verify/d2_reopen_20260929/orig_solo3.msd --port <log>` (recovery),
`verify/d3_regimes_20261013/score_regimes.py re/tools/b_envelopes.json verify/d3_regimes_20261013/ORIG_LABELS.txt`
(D3 (b)), `re/tools/statediff/collateral.py --mode banded` with `cut600.py` (collateral, recipe of
`RESULT_U9684.md` 3.3: A = `o_p2b`, floor A `o_p0a`, `--speed s.v2.rec_9e4 --anchor s.v2.substate:ge:6
--bands 1000-1500,1500-2000,2000-2600`).

- **G-SETTLE** (`D_B`): the player's move over its first `StepCar` (pre = the `SETTLE slot=0 in.pos`, post = ptrace
  `f=1 pos`) against the original's frame 1 -> 2 (0, -0.0029474455, +0.0000921488): x bit-identical (float32);
  |dz - 0.0000921488| <= 2.4e-7 (2 ulp at |z| = 2, d2settle's tolerance); |dy - (-0.0029474455)| <= 1.0e-5.
  The dy tolerance is set from D0-e's known port error (4.4e-6 in y), so this gate checks that the default path
  reproduces D0-e's mechanism. It is not a new precision claim.
- **G-SPAWNY** (report, `D_BRG`, where x / z are the original's grid): `physy0` against 0.4374268055.
- **G-PROG** (`D_BRG`): the player's (v=0) progress on the last row of the pathprog trace = 29.999977 +- 1e-5.
- **D2** (`D2_B_1..3`): slip 1500-2000 in 0.18855..0.19635, slip 2000-2600 in 0.24488..0.25487, driving-median in
  1904.70..1982.44 (`d81a8df6`, unchanged), on 3/3 runs. Original 0.1937 / 0.2498 / 1937.89; current default
  0.1927 / 0.2489 / 1860.35 (driving-median OUT on the base, U-9180).
- **Recovery H1** (`D2_B_1..3`): >= 50 % of the 400 post-trough frames >= 100 AND median >= 900, 3/3 runs.
- **D3 (b)** per regime: report against the base A 1/3, B 2/3, C 0/3.
- **Collateral**: paired fields past the floor for `H_B` (floor `H_B-2`) not more than 2 above the base `H_DEF`
  (floor `H_DEF-2`; expected 39 / 94).
- **G-CTRL**: sha8 of step / gates / ptrace. No-op control: `H_DEF` = `H_DEF-2` = `c2e4d60d / b50bd39b / ba90ed88`.
  Determinism: `H_B` = `H_B-2`.
- **Knob-took**: `D_P1` `SOLVER_IN` of the settle's first solver call reads st=(1,1,1,1) fv=(0,0,0,0) gc=4 (base:
  0,0,0,0 / gc 0). `D_P2` `SOLVER` fv differ from the stand-in's -0.0499756 on all four wheels. `D_B` `SOLVER`
  `centre_in` y equals `io.pos` y (base: the mean wheel ground y 0.437702239 on the default grid).

## 4. Predictions (registered now)

4.1 No-op: `H_DEF` = `H_DEF-2` = base hashes. PASS.
4.2 `D_P1`: knob took as in section 3. The settle step's postA4 velocity is (0, 0, 0) [UNCERTAIN: depends on A6a
    taking its parked full stop (`Integrate2.cpp` `+0xb20 == 0 && speed < 16`); the base settle's `+0xb20 = 1` writer
    is not identified (0.5)]. Position move 0 (part 3 off).
4.3 `D_P3` (part 3 alone, stand-in wheels): fv = -0.0499756 whatever the body height, so every latched call adds
    about +0.0499512·n; predicted: ptrace y rises by more than 0.05 over the first 3 frames. Not a candidate.
4.4 `D_P23` (no seed): the D0-c case on the default path. Predicted: the first correction is upward (D0-c measured
    +0.00994·n), i.e. G-SETTLE's dy has the wrong sign. Report.
4.5 `D_B`: G-SETTLE PASS, conditional on 4.2's velocity being 0. If the settle step keeps vy = -216.668, the
    y integral adds about -0.0050 per substep before the solver and dy fails; that outcome is reported, not tuned.
4.6 `D_BRG`: G-SPAWNY within 1e-6; G-PROG PASS (the +z move crosses the start line backwards, as the original's).
4.7 D2, recovery, D3 (b), collateral under B: no direction is predicted. The bounds decide.

## 5. Default rule (decided now)

Fix B is wired default ON on this branch, with opt-out `MASHED_POS_MODEL=0`, only if ALL of: G-SETTLE PASS,
G-PROG PASS, no-op PASS, determinism PASS, D2 three metrics in bounds on 3/3, recovery H1 3/3, collateral within
+2 of the base. Otherwise it stays default OFF and the result says which gate failed. If driving-median (U-9180)
moves into its bound under B it is reported first in the RESULT, whatever the other gates say.
Fix C: its opt-in stays as is unless `BC` passes the same gate set AND fix B goes default ON. A part that fails
in a way that needs an architecture-level choice not covered by the brief stops the lane and is reported.

## 6. D2 watch list (RE-PICKUP conditions of DEFERRED D-11071)

This lane changes the inputs of `0x0046f6c0` (wheel contact positions, ring rotation rows, ring translation) and
consumes its translation output; it changes player-car speed on Training through every physics path. It does not
change `+0x4a4`, `ProduceTerrainBatch` / `FUN_00538c80`, grip-clamp #6, the substep / chunk loop or
`ReassertContacts` code. Every measured hit is reported in the RESULT.

## Amendment 1 (2026-10-16, after the D arms, before any H / D2 / B run and before the G-PROG re-read)

Recorded as measured (`g_settle.py`, `D_*.settle.txt`, `D_*.ptrace.log`; the first batch of 7 D runs ran with a
`MASHED_ROOT` lacking `original\`, fell back to the main root and appended to its CWD logs, and is void; the
re-run with the fixed root reproduced every `settle.txt` sha8 exactly):
- **G-SETTLE (registered arm `D_B`, default grid): FAIL.** move (0, -0.00296074, +0.0000925063): dz off by
  +3.6e-7 (tolerance 2.4e-7), dy off by -1.33e-5 (tolerance 1e-5), x bit-identical. Not re-thresholded.
  Report: on `D_BRG` (the original's grid x / z / yaw) the same gate reads dz +1.2e-7, dy -4.6e-6, x bit-identical,
  i.e. it passes there. The two arms differ in spawn pose only: yaw -1.576028 (default grid) vs -1.568874, z
  -2.00247 vs -1.99904. The call-2 fv differ left/right by +-3.7e-5 (`D_B` 0.009368 / 0.009423 vs `D_BRG`
  0.009405 / 0.009385), which is what a yaw rotation on a sloped tri gives.
- **G-SPAWNY (report):** `D_BRG` physy0 0.437426984 vs 0.4374268055, +1.8e-7. Prediction 4.6 (1e-6) PASS.
- **Prediction 4.2:** `D_P1` settle postA4 vel (0,0,0), `+0xb20` 0, loads 1083.34192 x3 / 541.670959. PASS.
- **Knob-took for part 1, as registered: FAIL.** The settle's first solver call enters with st (0,0,0,0) fv 0 gc 0
  on every arm. Cause, from the code and the `SOLVER_IN` lines: the A4-first `ReassertContacts(r)` after A4 runs
  before this car's first `SolveWheelContacts` and uses the cached `g_cGrounded`, which is one global written by
  every car's last solver call (initial false for slot 0's first step), so it zeroes the four seeded states. The
  solver's call 1 then yields (0,0,2,2), `ReassertContacts` promotes to (1,1,2,2), and the 4 -> 3 drop translation
  happens on call 2. The end-of-frame position is still the D0-e one (`D_BRG` above). Not changed in this lane
  (`ReassertContacts` is a D2 watch-list item), reported.
- **Prediction 4.3** (`D_P3` climbs > 0.05 in 3 frames): PASS, +0.1398 in the first StepCar.
- **Prediction 4.4** (`D_P23`, no seed): the first nonzero correction is upward (+0.00706·n) as predicted, but
  the settle step keeps vy -216.668, so the y integral (-0.0100) dominates and the net move is (0, -0.00297, -0.00022):
  dy has the original's sign, dz the wrong one. Half MISSED.
- **G-PROG, as registered: FAIL**, and the reading point was mis-registered. The last pathprog row is frame 117,
  where the player has driven off (ptrace velocity nonzero from `f=24`, prog 1.04706). The brief's and d2settle's
  definition is the parked player's progress after the settle. On `D_BRG` the v=0 prog reads 29.999977 on every
  row from frame 0 while the player is parked; d2settle's base `D_RG` read 0.000000 there.

Amended reading (governs the re-scored `D_BRG` and a new base control `D_RG`, both run after this commit):
**G-PROG** = the v=0 prog on every pathprog row before the frame at which the player's ptrace velocity first
becomes nonzero, all equal to 29.999977 +- 1e-5; the control `D_RG` (no fix B) is predicted to read 0.000000 there.
The registered G-SETTLE result above stands as the gate result; `D_BRG` is the report.

## Amendment 2 (2026-10-16, after `D2_*` B / P1 / P2 / P23 / BX / DX and `H_DEF`, `H_DEF-2`, `H_B`; before the bisect)

Recorded (`score_d2.py`, all runs on exe `90a55dc7`):
- **D2 gate under B: FAIL 3/3.** `D2_B_1..3` identical: slip 1500-2000 and 2000-2600 UNSCORABLE (n=0),
  driving-median 1338.62 (n=54). Same-build control `D2_DEF_1`: 0.1927 / 0.2489 / 1860.35 (n 376 / 562 / 1366) =
  the recorded default.
- **Isolation (report):** `D2_P1_1` (seed alone) = 1338.62 n=54, the same collapse. `D2_P2_1` 0.1927 / 0.2489 /
  1860.27 (= default). `D2_P23_1` (wheels + translation, no seed) 0.1944 in / 0.2538 in / **1875.84** OUT (n 360 /
  566 / 1338), driving-median +15.5 against the default.
- **Diagnostic `D2_BX_1` vs `D2_DX_1`** (ptrace + world-contact + off-mesh logs): both cars hit the x = -2.04 wall
  near f 95 with the same first fixup (vel (-1696, 0, 680) -> (189, -78, 143)). The default slides off it (31
  fixups in the run, sp 1952 at f 300). Under B the car stays on it: 215 fixups, all slots s5 / s9, normal
  (1, 0, 0), arm x -1.80, sp 10-50 for the rest of the run. 0 off-mesh recoveries in either.
- **No-op control: PASS** (`H_DEF` = `H_DEF-2` = `c2e4d60d / b50bd39b / ba90ed88`). `H_B` `b1b6f319 / 4ea8366f / e92bdf83`.

Fix B as registered therefore stays default OFF (section 5: D2 fails). Two further measurements, registered now:

A2.1 **Bisect (DIAGNOSTIC, report only)** of which `FUN_0046baa0` write breaks the D2 arm. Diag knobs (default
OFF): `MASHED_POS_SEED_MIN=1` writes only int 1 at the four states, 0 at the four fv and 4.0f at `+0x9e0` instead of
the whole reset. `MASHED_POS_SEED_KEEP=<a-b>,...` runs the whole reset then restores those record byte ranges from
before it. Arms `D2_P1MIN_1` and `D2_P1K<g>_1`, part 1 only, one group restored each: G1 `0-4`, G2 `14-50`, G3
`194-458` (wheel block), G4 `490-498`, G5 `4ac-8f0` (18 contact keys), G6 `9b0-9f8`, G7 `ad0-b2c`, G8 `b2c-bf0`
(the 16-entry -1.0 loop). A group whose restore brings driving-median back to about 1860 contains the write that
breaks the arm. No prediction.

A2.2 **`BMIN` (parts 2 + 3 + the minimal seed): measured, NOT eligible for default ON in this lane.** A partial
`FUN_0046baa0` hides whatever downstream port defect the faithful reset exposes, so it can only inform the user
decision. Same gate set as section 3: `D_BMIN` / `D_BMINRG` (G-SETTLE, G-PROG), `D2_BMIN_1..3`, recovery, and
`H_BMIN` / `H_BMIN-2` collateral if the D2 arm scores at all. G-SETTLE prediction [UNCERTAIN]: with the minimal
seed the settle's A6a may not take its parked full stop (`D_P1`'s vel 0 came with the whole reset).
