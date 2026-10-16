# PREREG d2settle: parked-player first-step settle + U-9181 (+0x4a4)

Branch `work/d2settle`, base `1caf1a10` (race/first-frame-parity). Model claude-opus-5-5 (account2).
Committed BEFORE the first measuring run of this lane. Amendments are appended and committed before the
runs they govern.

## 0. Facts read before any run (static bytes + committed captures)

Every number here is from a committed or pre-existing file. None was produced by this lane's runs.

0.1 **U-9181 premise is a mislabel.** The port figure 692.302 is motion_diag's `susp=` column, which prints
`g_suspScale` (`VehiclePhysicsRun.cpp:1468`, `:1507`), the global `_DAT_0088e5f0`, NOT record `+0x4a4`. The live
original's `_DAT_0088e5f0` was measured at 692.302185 (`VehiclePhysicsRun.cpp:830-831`). The port's real `+0x4a4`
is in `MASHED_AI_STEPDUMP`'s `rec_4a4` column: **0.678002715** on 21,402 / 21,402 rows of
`verify/d3_u9684_20261013/U_DEF.step.csv` (cars 1..3). The original's `+0x4a4` is **0x3f2d93d1 = 0.67803675** on
every frame of `o_p0a.msd` (player, `verify/d3_car2pickup_20261013`), `o_rd1.msd` (car 1) and `o_p2a.msd` (car 2).

0.2 **The real divergence is A3's sqrt flavour.** The only record-relative writer of `+0x4a4` in the image is
`0x0046ba00 fstp [esi+0x4a4]` (`re/tools/dispsweep.py 0x4a4`: 2 writes, the other is a stack slot at `0x0042e0ba`;
`fold_sweep.py 0x4a4`: one folded READ at `0x00470b44`). Its value is `[esp+0x1c] * [0x005cea54]` (1.05), the max
of loop 4's roots, each `call 0x004c3b30` on `(dz*dz + dy*dy) + dx*dx` rounded to float (`0x0046b93b..0x0046b954`).
`0x004c3b30` is RW's LUT sqrt. The port's A3 (`Vehicle/VehicleInit.cpp:27`) uses `std::sqrt`.
Offline model (Python, float32 per op = PC_24, the exe TU is SSE2): from the original's own A3 INPUTS in
`o_p0a.msd` frame 1, the LUT model reproduces **24 / 24** A3 sqrt outputs bit-exactly (`+0x4a0`, `+0x178`,
`+0x23c`, `+0x300`, `+0x3c4`, the 18 radii at `+0x4bc/+0x5bc/+0x7bc` stride 0x40, `+0x4a4`). `std::sqrt`
reproduces **0 / 24**. `std::sqrt` gives `+0x4a4 = 0x3f2d9196` = 0.678002715, which is the exe's measured value.
Downstream: the mass tail `0x0046ba06..0x0046ba90` reads `+0x178/+0x23c/+0x300/+0x3c4`, so `+0x58` (original
`0x44b136a0` = 1417.707) and `+0x5c` (original `0x3a38e833`) move too. Its summation order is
`((t3c4 + t300) + t23c) + t178`; the port's (`VehicleInit.cpp:323-324`) is `t178 + t23c + t300 + t3c4`.

0.3 **The original's first physics tick moves every car.** `o_p0a.msd` (player): frame 1 pos (+0x958/95c/960)
`(0.50093675, 0.43742681, -1.99903715)`, loads `+0x210` 0; frame 2 `(0.50093675, 0.43447936, -1.99894500)`, loads
1083.34 / 541.671; then constant. `vel +0x9b0..` and `+0x9e4` are 0.0 on every frame. x is bit-identical
(`0x3f003d64`). Cars 1 and 2 (`o_rd1`, `o_p2a`): the same dy = -0.0029475 and dz = +0.000092 on frame 1 -> 2.

0.4 **The exe never applies that motion.** On the default build the countdown branch returns before any physics
(`TrackRenderer.cpp:3357-3373`). The first `StepCar` per slot runs a one-shot "spawn settle" step
(`VehiclePhysicsRun.cpp:637-698`) whose position delta is discarded ("No position is touched", `:671-672`), then
zeroes vel / `+0x9e4` / `+0x9e8` / `+0xb0c` / gear. After that the parked player's `+0x9e4` reads 0.6380385 with
vel (0,0,0) on every ptrace line (`verify/d3_u9684_20261013/U_DEF.ptrace.log`).

0.5 **The brief states `+0x9e4` the wrong way round.** It says the original reads 0.638 at rest and the exe 0.0.
`RESULT_U9684.md` section 2.2 and the captures above say the opposite: original 0.0, exe 0.638.

## 1. Stage 0: diagnostics (report only, NOT gates)

D0-a. New default-OFF diag `MASHED_SETTLE_DIAG=<path>`: per slot, the spawn-settle step's `drive_delta`, the
record position / vel / `+0x9e4` / loads before and after it, then the first N=3 real steps of slot 0 with
`+0x9e4` and vel at each write point already instrumented (A4 gate refresh, A6a entry, end of step). Writes
nothing when unset. Used to locate the exe's 0.638 producer and the size of the discarded delta. It decides the
shape of fix B, which is then registered by amendment before any fix-B gate run.

## 2. Fix A (U-9181): A3 radius loops through `Math::LutSqrt`, original operand order

Change: `VehicleInit.cpp` loops 1-5 call `Math::LutSqrt` (`0x004c3b30` with the real tables) in the original's
operand order (loop 1 `0x0046b7f2..0x0046b821`, loop 2 `0x0046b880..0x0046b899`, loops 3-5 as quoted in 0.2), and
the mass tail in its byte order. Opt-out `MASHED_NO_A3_LUT=1` restores the current body exactly.

- **G-A3** (bit-exact): the exe's A3 outputs for slot 0 equal the original's `o_p0a.msd` frame-1 record on 26
  fields (the 24 of 0.2 + `+0x58` + `+0x5c`). Read through a default-OFF one-shot log `MASHED_A3_LOG=<path>`.
  **Prediction: PASS 26/26.** With `MASHED_NO_A3_LUT=1`: predicted 0/24 on the sqrt fields (knob-took check).
- **G-4A4** (bit-exact): `rec_4a4` = `0.678036749` (`%.9g` of `0x3f2d93d1`) on every row of a 1200-frame step
  dump. Prediction: PASS.

## 3. Fix B (settle): targets registered now, mechanism by amendment after Stage 0

Targets, all on the G-CTRL recipe (section 5) with `MASHED_PLAYERTRACE`, player slot 0:
- **G-SETTLE**: the exe player's world x after its first physics step is bit-identical to before it, and its
  z moves by `dz` with `|dz - 0.0000921488| <= 2.4e-7` (2 ulp at |z| = 2). Original dz from 0.3 bit patterns:
  `-1.9989449977874756 - -1.9990371465682983 = +0.0000921487808`.
- **G-9E4**: the exe player's `+0x9e4` reads exactly 0.0 on every ptrace frame before the player's first frame
  with vel != 0 or `c9ec` > 0. Original: 0.0 on every frame before the contact (0.3).
- **G-PROG** (with `MASHED_PATHPROG` + `MASHED_ROUND_GRID` + `_ORDER`, the arm where the grid is already
  bit-exact per `RESULT_U9684.md` 2.2): the player's progress `0x008a96e8` after the settle reads
  29.999977 (`0x41effff4`), tolerance 1e-5. Read from `MASHED_PATHPROG_TRACE` v=0.

## 4. Re-scores (before = base build, after = each default-ON candidate)

- **D2** (bounds `d81a8df6`, unchanged): `re/tools/statediff/a8_run_port.py <dir> 90 MASHED_MEASURE_SOLO=1
  MASHED_TRACK_SEL=12 MASHED_STEER_HOLD_AFTER=0`, three runs; `a8_slip_axis.py` and `a8_momentum.py
  --orig-steer-min 33.0 --port-steer-min 0.9` against `verify/d2_reopen_20260929/orig_solo3.msd`, NO
  `--max-lines` (this reproduces attempt 20's t1 exactly: 0.1943 / 0.2524 / 1852.66 n=1367). Report
  slip 1500-2000, slip 2000-2600, driving-median per run. U-9180: does driving-median move?
- **D3 (b)**: `B_<arm>_K{0,1,26}` (1200 frames, `MASHED_RING_SKIP`), scored with
  `verify/d3_regimes_20261013/score_regimes.py re/tools/b_envelopes.json verify/d3_regimes_20261013/ORIG_LABELS.txt`.
  Baseline expectation: A 1/3, B 2/3, C 0/3 (`RESULT_U9684.md` 3.2).
- **G-CTRL**: 14,400 frames, `run_arms.py` base env (MUTE, TRACK_VIEW=Training, CAR=1, ROUND=1, ROUND_RULE=4,
  DETERMINISTIC, PLAYERTRACE, AI_STEPDUMP, U9186_GATES), sha8 of step / gates / ptrace. Base expectation
  `991a5fff / be0d47ea / bc75b405`, run twice (determinism).
- **Collateral**: `collateral.py --mode banded` as in `RESULT_U9684.md` 3.3 (A = `o_p2b`, floor `o_p0a`,
  600 frames from the car-2 sub-state>=6 anchor). Baseline 39 / 94.

## 5. Controls

- **No-op control**: every opt-out knob set at once (`MASHED_NO_A3_LUT=1` + fix B's opt-out) must reproduce
  the base G-CTRL hashes bit-exactly. Prediction: PASS.
- **Knob took**: each fix's own diag log shows the changed value with the knob unset and the old value with it set.

## 6. Default rule (decided now)

A fix is wired default ON, with its opt-out, only if: its own gates PASS; the no-op control PASSES; D2 slip
1500-2000 and slip 2000-2600 stay inside their bounds on 3/3 runs; D3 (b) loses no car-regime pass against the
base; collateral is not worse than the base by more than 2 paired fields. Otherwise it stays default OFF with a
recommendation. Any D2-metric move is reported (D2 watch-list hits are allowed this round, user 2026-10-16).

## Amendment 1 (2026-10-16, before any original-side run; Stage 0 D0-a already run, report only)

D0-a result (`D_DEF.settle.txt`, base build + default-OFF diags, 300 frames): the exe's settle step leaves
vel `(0, -216.668411, 0)` after A4, runs 2 substeps of 25 with `c9ec = 0` (no world contact, no fixup), and
produces `drive_delta = (0, -0.0100310249, 0)` -- no z component -- which is then discarded. Its spawn record y is
0.47481969 against the original's 0.43742681 (+0.0374, `car_ground_off_ = -model.bbox[1]`,
`TrackRenderer.cpp:2954`). The 0.638 is A6a's pre-full-stop `|v|` (n=2 postA4: vel 0, `+0x9e4` 0.638038516).
`MASHED_A3_LOG` (base): `4a4=3f2d9196 58=44b13443 5c=3a38eaab` (original `3f2d93d1 / 44b136a0 / 3a38e833`).

D0-b (report only, NOT a gate): one ORIGINAL capture of the player's first ticks,
`re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --statediff-out
verify/d2settle_20261016/o_fx1.msd --statediff-car 0 --fixup-probe --hold 20`. Read: the first substep /
fixup rows of slot 0 (velocity at the substep entry, `+0x9ec`, contact slot depth / normal / arm), to name the
velocity write that carries the original's dz. A piz-collision crash (`+0x1509b4` / `+0xe66db`) means re-run.

Fix C (registered now, gate G-9E4 of section 3): port the every-frame refresh `0x004710a1..0x004710da`
(`+0x9e4 = |+0x9b0|`, `+0x9e8 = |+0x9bc|` for cars with `+0x4 != 0 || +0x10 == 1`, after the A4 loop, before
the substep loop at `0x00471110`). The port does it only on A4-skipped frames (`VehiclePhysicsRun.cpp:991-994`).
Opt-out `MASHED_NO_SPEED_REFRESH=1`. Prediction: G-9E4 PASS (exe player `+0x9e4` = 0.0 at rest).

## Amendment 2 (2026-10-16, before D0-c; D0-b and fix A/C short-arm results are report only)

D0-b (`o_fx1.msd`, original, player): `0x0046ef70` fixup fired 0 times in 20 s; every sampled point of the
first ticks (substep entry, A6a entry, wheel solver `0x0046f6c0`, scan `0x00469aa0`) has vel = 0. So the
original's first-tick move is not a velocity integral. `0x0046f6c0` itself moves the active ring translation
(`WheelContactSolver.cpp:255-260`, `pos -= mean(fv * n)` over the grounded wheels). The original's
dz/dy = -0.0312615 equals the Training tri normal's n.z/n.y = -0.031250 (port probe n = (0, 0.999512076,
-0.0312347524), `D_DEF2.settle.txt`), i.e. the move is -0.002949 * n.
The port runs the same solver on a stand-in wheel geometry (every wheel at groundY - 0.05,
`VehiclePhysicsRun.cpp:517-521`): fv = -0.0499756 on all four wheels, correction +0.0499512 * n (up) on EVERY
call, discarded by `SyncContactRingMatrix` (`:1196`). The exe has no vertical position authority
(`car_pos_[1] = GroundHeight + car_ground_off_` every frame, `TrackRenderer.cpp:3475`).
Fix A short arm: G-A3 26/26 PASS (`D_DEF2.a3.txt`), 0/26 with `MASHED_NO_A3_LUT=1` (`D_OFF.a3.txt`).
Fix C short arm: parked player `+0x9e4` = 0 after A4 from step 2 (`D_DEF2.settle.txt`).
No-op control (short arm): `D_OFF` ptrace `97785f2e` = base `D_DEF` ptrace.

D0-c (report only, NOT a gate, decides nothing by itself): diag knob `MASHED_D2S_RINGY=<y>` (default OFF) seats
the slot-0 solver's ring translation at (x, y, z) = (io.pos.x, <y>, io.pos.z) on its FIRST call only, then runs
`0x0046f6c0` twice back to back keeping the solver's own corrected translation, logging both corrections.
Arm: `MASHED_ROUND_GRID` + `_ORDER` + `MASHED_PATHPROG` (x/z bit-exact to the original's frame 1),
`MASHED_RW_WHEELPOS=1` (real suspension points through the ring matrix), `MASHED_D2S_RINGY=0.4374268054962158`
(the original's frame-1 y, `0x3edff668`). PREDICTION (mechanism test): call 1 + call 2 correction =
(0, -0.0029475, +0.0000921) within 2e-6 per component. A miss is reported as evidence against "the solver
transcription alone carries the settle" and the y / geometry inputs are listed as the open terms.

## Amendment 3 (2026-10-16, before D0-d; report only, NOT a gate)

Recorded so far (report): no-op control `H_OFF` = `991a5fff / be0d47ea / bc75b405` (base default, PASS).
D0-c prediction MISSED (port solver on real wheel points at the original's frame-1 y: call 2 corrects
+0.00994 * n, original -0.002949 * n).
D0-d: original capture `--wheelstate-probe-count` (rate gate) then `--wheelstate-probe`, same scenario as D0-b,
`--statediff-car 0 --hold 12`, output `verify/d2settle_20261016/o_ws1.msd`. Read the first rows: per-wheel
state `+0x198`, fv `+0x194`, key `+0x1ec` at the player's first `0x0046f6c0` entries, against the exe's D0-c
call-1 entry (states 0, fv as initialised). Names the input the port's settle call lacks; decides nothing alone.

## Amendment 4 (2026-10-16, before D0-e; report only, NOT a gate)

D0-d result (`o_ws1.msd.wheelstate.csv`, original player rows): the FIRST `0x0046f6c0` call of every frame enters
with states (1,1,1,1) and fv (0,0,0,0) (`FUN_0046baa0`, the per-tick hold reset at `0x0041033e`, writes int 1 to
`+0x198/+0x25c/+0x320/+0x3e4` at `0x0046bb23` and 0 to `+0x194/+0x258/+0x31c/+0x3e0`). Frame 1's second call
enters with fv (0.009396, 0.009396, -0.009946, -0.009946), i.e. what call 1 computed. Arithmetic: all four
latch, `bVar16 == 4` drops the most-loaded wheel (w2), mean of three = 0.0029486, correction -0.0029486 * n =
(0, -0.0029472, +0.0000921); measured original move (0, -0.0029474, +0.0000921). Call 2 nets ~0 (fv
0.006447 / -0.012895): equilibrium.
D0-e (diag knob `MASHED_D2S_SEED1=1`, only inside the D0-c block): seed those 8 fields as `FUN_0046baa0`
does before D0-c's call 1. PREDICTION, from D0-c's own call-1 classifier output (0.00940529443, 0.00938524026,
-0.00993062556, -0.00996109098): call 1 drops w3 and corrects (0, -0.0029519, +0.0000922), 2e-6 per component.
