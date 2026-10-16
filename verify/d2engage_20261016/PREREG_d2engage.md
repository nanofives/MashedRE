# PREREG d2engage: fix B's three D2 failure causes (engagement d=14, contact cache, wall pin)

Branch `work/d2engage`, base `9420dde2` (race/first-frame-parity). Model claude-opus-5-5 (account2).
Committed BEFORE the first measuring run of this lane. Amendments are appended and committed before the runs
they govern. Brief: orchestrator 2026-10-16 (TAG d2engage). D2 is un-parked (user decision 2026-10-16), every D2
watch-list hit is reported. A sibling lane (d2bsp) ports `FUN_00538c80` + the chunk loop at the same time: the
changes here stay knob-gated and confined to the settle, the A4-first `ReassertContacts` call and the GO frame.

## 0. Facts read before any run (existing captures and the bytes only, no new run)

0.1 **The 15-frame engagement delay is the `+0xbf8 == 2` arm, read in the original's own capture.**
`verify/d2_reopen_20260929/orig_solo3.msd` (a8_launch.py's reference, release index 886), by capture index:

| idx | d | `+0xbf4` | `+0xbf8` | `+0xb14` | states `+0x198/25c/320/3e4` | `+0x9e4` |
|---|---|---|---|---|---|---|
| 878..885 | <0 | 3000 | 0 | 0 | 1,1,1,1 | 0 |
| 886 | 0 | 2800 | 2 | 0 | 1,1,1,1 | 0 |
| 887..899 | 1..13 | 2600 .. 200 (-200/frame) | 2 | 0 | 1,1,1,1 | 0.64 .. 2.00 |
| 900 | 14 | 0 | 0 | 0 | 1,1,1,1 | 2.11 |
| 901 | 15 | 0 | 0 | -265866.2 | 1,1,1,1 | 14.94 |

- The release is `FUN_0046d780`: `+0xbf8 = 2` at `0x0046d7a2` when the charge `+0xbf4` > 1000 (`0x0046d79a`), the
  charge left unmodified (`LaunchRevCharge.cpp:168-195`).
- A6a `FUN_00467650` zeroes `+0xb14..+0xb1c` in its `+0xbf8 == 2` arm (`0x00467dee..0x00467e44`). It decrements
  `+0xbf4` by dt at two sites per qualifying wheel: the `FUN_0040e350() == 6` site (`0x00467cf1..0x00467cfe`) and
  the arm's own site. A qualifying wheel passes `p[-0xf] == 2 && p[-3] != 0` (`Integrate2.cpp:246`). `p[-0xf]` is
  `+0x168/+0x22c`, which A3 writes as 2, 2, 1, 1 (`VehicleInit.cpp:235`), so the two front wheels qualify when
  their state is nonzero. 2 wheels x 2 sites x dt 50 = 200 per frame, and 3000 / 200 = 15 frames.
- idx 886 already holds 2800 / state 2, so the release and one decrementing physics tick share the frame d=0.

0.2 **The original's tick order, from the bytes (ported verbatim in `Race/SubStateMachine.cpp:198-256`).** In
`FUN_004103a0` the per-car loop runs `FUN_0046baa0` (`0x0041043a`) and the charge `FUN_0046d7f0` (`0x00410441`) on
every tick. The release `FUN_0046d780` (`0x0041049b`) follows, behind `t >= 1.86` (`0x0041045c..0x0041046e`). The
sub-state tick runs before the frame's physics (`FUN_0040fc00` -> physics at `0x0040fc9b`,
`TrackRenderer.cpp:3857-3859`). So every hold tick before the release is a seeded tick with `+0xbf8 == 0`. The
release tick is itself seeded (`0x0041043a`) before its physics, which runs with `+0xbf8 == 2`.
`FUN_0046baa0` writes neither `+0xbf4` nor `+0xbf8` (d2posmodel PREREG 0.3).

0.3 **The exe's order on the GO frame (`TrackRenderer.cpp:3775-3890`).** `LaunchRev_PreRaceTick` runs (charge),
then `LaunchRev_Release()` (slot 0 only, `LaunchRevCharge.cpp:245-249`), and then `StepPlayer`. The first
`StepCar` call runs the spawn-settle step and then the d=0 step (`VehiclePhysicsRun.cpp:777-863`), both after the
release. Default build: the settle's A6a sees states 0 (memset record), so the gate is closed and nothing is
decremented. That gives d=0..14 = 15 decrements and engagement at d=15, as on the original. Under `MASHED_POS_SEED`
the settle's A6a sees the seeded states 1 (d2posmodel: "A4 sees states 1"), so the gate opens and the settle step
decrements 200 after the release. That gives 16 decrementing ticks and engagement at d=14. **This is the
explanation proposed for cause 1.** It is confirmed or refuted by the knob-took witness in 3.
The exe's settle step stands in for the original's pre-release hold ticks. It runs after the release, and
the release tick's own `FUN_0046baa0` (0.2) has no counterpart in the exe.

0.4 **The post-A4 `ReassertContacts(r)` (`VehiclePhysicsRun.cpp:1219`, default A4-first order) has no counterpart
in the bytes (cause 3).** It writes the wheel states (0 -> 1 if grounded, all 0 if not), the per-wheel contact
normals `+0x200 + w*0xc4` and `+0x9e0`. It reads `g_cGrounded` / `g_cOnMesh` / `g_cNormal` / `g_cAvgN`, which are
globals that `SolveWheelContacts` sets for whichever car ran last (`:355-358`, `:521-546`). For slot 0's first step
those are the initial values (grounded false), so the four seeded states are zeroed. Disassembled from
`MASHED.exe.unpatched` (this lane), `FUN_00470c70` between the end of the A4 loop (`0x00471099 jne 0x470f80`) and
the substep loop (`0x00471106`) runs only:
- the per-car speed refresh, which reads `+0x9b0` / `+0x9bc` and writes `+0x9e4` / `+0x9e8`
  (`0x004710bc..0x004710da`, gate `0x004710b0..0x004710ba`);
- `call 0x0046dc20` at `0x004710f4`, gated by `fild [rec+0xc]` vs `[0x005ceaac]` (`0x004710e3..0x004710f1`).
  `0x0046dc20` is hooks.csv `VehicleOOBDebugDraw` (C2). It reads `rec+0x9f0` (`0x0046dc30`) and writes only the
  queue `0x007e9de0..` (`0x0046dc75..`).

No wheel state, normal or `+0x9e0` is written there. The port's A6a writes no wheel state (grep of `Vehicle/`:
the only state writers are `ReassertContacts`, the solver and the two seed sites), so the reason given at
`:1585-1586` ("A6a's drive block can clear wheel states") has no writer behind it.

0.5 **The wall pin (cause 2), from committed captures.**
- **Original** (`verify/d2_bounce_20260930/orig_fp2.msd.fixupprobe.csv`, D2-style drive + steer 1): the first
  impact (f=978, pre (-1704.9, 0, 671.3)) is followed by a train of re-contacts every 12-13 frames (f=997, 1010,
  1022, 1034, 1046, 1059, 1071, 1083, 1096). Their pre-fixup vz grows 179.8 -> 398.8 and the contact arm z grows
  0.234 -> 1.051, so the car rotates off the wall.
- **Default port** (`verify/d2posmodel_20261016/DX.wc.log`): a similar train, with pre vz 139.6 -> 466.1 and arm z
  0.21 -> 1.15.
- **Fix B** (`BX.wc.log`): pre vz 125.5 -> 68.8 -> 40.5 -> 30.7 and then about 23-30 for the rest of the run, arm z
  about 0.18-0.23. This is the pin.
- Per frame, DX and BX are nearly identical up to the second contact (`D2_DX_1` idx 96-115 vs `D2_BX_1` idx 95-113,
  one frame apart from the d=14 shift).
- **Lateral velocity on the body axes**, original `orig_solo3` idx 982-992 against DX idx 97-107. Original:
  166 -> 142 -> 123 -> 106 -> 93 -> 82, then 82 for 5 frames while the forward component climbs -73 -> +95. Port:
  180 -> 148 -> 128 -> 110 -> 96 -> 85 -> 75 -> 67 -> 51 -> 25 -> 7.

This is the open U-9156 / U-9173 result (`re/analysis/D2_REOPEN_2026-09-29.md` §21.2 :2980-2989, the lateral held at
-81.7..-81.9 against the port's -3.76). §26.3 :5195 reads clamp #6 on the LOW arm with the port's `k` 0.540954
against the original's 0.1 floor at 100-150. §26.4 says the next lane is an A6b-entry capture. Proposed
attribution for cause 2: the low-speed lateral damping gap (U-9173) keeps the post-bounce velocity on the body
heading, into the wall. Which side of the wall's contact train a trajectory lands on decides whether it escapes.
This lane does NOT attempt U-9173's fix. It measures the attribution (4.6).

## 1. The change (all default OFF until section 5 says otherwise)

New knobs in `Vehicle/PosModelKnobs.h` (value `1` = on). `MASHED_POS_MODEL=1` now turns on parts 1-5. The
d2posmodel fix B is `MASHED_POS_SEED` + `_WHEELS` + `_TRANS` (stack `B0` below).

- **Part 4, `MASHED_POS_ORDER` (cause 1).**
  - (a) For slot 0, the spawn-settle step runs on the GO frame BEFORE `LaunchRev_Release()`. It is the same
    settle block, moved into a callable `VehiclePhysics_SpawnSettle(slot, dt, io)` that `StepCar` also uses, so
    the code path is shared. With part 3 on, `TrackRenderer` takes its position as it takes the step's.
  - (b) After the settle step's post-processing, `VehiclePhysicsStateReset(slot)` (`FUN_0046baa0`) runs once more,
    for every slot. This is the release tick's own `0x0041043a`, before that tick's physics.
  - (c) The settle step runs with `Fi_GameMode()` = 5. `FUN_0040e350` is `return DAT_0063ba8c`
    (`ForceIntegratorStubs.cpp:73`). The exe stubs it to 6. On `FUN_004103a0`'s countdown ticks it reads 5, and 6 is
    written only at the release (`0x004104e3`). So A6a's `== 6` decrement site does not run on a hold tick. The
    original's hold frames confirm it: `+0xbf4` stays 3000 with the gate open (0.1, idx 878-885). Under the stub, a
    pre-release settle would take 100 off the charge before the release reads it. In the exe the mode-6
    dependence is A6a's site plus the inert `RubberBandGate` (`ForceIntegratorStubs.cpp:93-103`).
  - STATED: the exe still steps one hold tick where the original steps about 800. The AI slots are not released
    in the exe (`LaunchRevCharge.cpp:245-249`, unchanged).
- **Part 5, `MASHED_POS_CONTACTS` (cause 3).** The A4-first `ReassertContacts(r)` (`:1219`) is skipped, which is
  the bytes' order (0.4). Every remaining `ReassertContacts` call then reads the cache its own car's
  `SolveWheelContacts` set in the same call (`:505-511`, `:521`, `:702`). The legacy `!s_a4First` order (`:1587`)
  is not touched.
- **Diagnostic, default OFF:** ` bf4=<int> bf8=<int> st=[s0,s1,s2,s3]` appended at the END of the motion_diag
  line (every existing token stays byte-identical), plus one `settle slot=0` marker line when the settle step
  runs. The marker carries no `sp=` or `b14=`, so a8_launch.py's series skips it, as it skips
  `launchrev release`.

## 2. Arms (`run_arms.py`, recipes identical to d2posmodel's; `MASHED_ROOT` = private copy with `original\` inside)

Stacks: `DEF` (none); `B0` (SEED + WHEELS + TRANS = d2posmodel B); `BO` (B0 + ORDER); `BK` (B0 + CONTACTS);
`B` (MODEL = parts 1-5); `K` (CONTACTS alone); `O` (ORDER alone); `RG` / `BRG` (PATHPROG + ROUND_GRID + ORDER,
without / with B).

| arm | what | role |
|---|---|---|
| base exe `H_DEF`, `H_DEF-2` | G-CTRL, untouched base build | baseline (predicted `716cb2fd / 2a2afda1 / ba90ed88`) |
| `H_DEF`, `H_DEF-2` | G-CTRL, this build, no knob | no-op control + collateral floor |
| `H_B`, `H_B-2` | G-CTRL | determinism + collateral |
| `H_K`, `H_O` | G-CTRL | report: are parts 5 / 4 alone inert on the default? |
| `D_DEF`, `D_B0`, `D_B`, `D_BRG`, `D_RG` | 300 frames, settle diag + ptrace | knob-took, G-SETTLE x2 grids, G-PROG |
| `D2_DEF_1` | D2 90 s | same-build control |
| `D2_B0_1`, `D2_BO_1`, `D2_BK_1` | D2 90 s | per part (report): which part moves engagement / pin |
| `D2_B_1..3` | D2 90 s | gates |
| `D2_K_1`, `D2_O_1` | D2 90 s | report: parts 5 / 4 alone on the default |
| `B_DEF_K{0,1,26}`, `B_B_K{0,1,26}` | D3 (b) 1200 frames | report |
| `D2_BX_1`, `D2_DX_1` | D2 + wc log + `MASHED_A6ADUMP` | cause-2 attribution (4.6), report only |

## 3. Gates (scorers as in d2posmodel)

- **Knob-took, part 4** (`D2_BO_1`, `D2_B_1`): in motion_diag the `settle slot=0` marker and the settle's own
  series line (index 0) come BEFORE `launchrev release`, and the settle line reads `bf4=3000 bf8=0` (the original's
  idx 885). The d=0 line (index 1) reads `bf4=2800 bf8=2`, which is the original's idx 886. Base `D2_B0_1`: settle
  line `bf4=2800`, d=0 line `bf4=2600`. (`bf4` and `bf8` are logged after the step's physics, as the .msd is.)
- **Knob-took, part 5** (`D_B`): `SOLVER_IN` of the settle's first solver call reads st=(1,1,1,1) fv=(0,0,0,0)
  gc=4. Base `D_B0`: st (0,0,0,0) gc 0.
- **G-LAUNCH**: `+0xb14` engages at d=15 under B on 3/3 (a8_launch.py; original d=15).
- **G-SETTLE, both grids** (`g_settle.py`): the player's move over its first `StepCar` against the original's
  frame 1 -> 2 (0, -0.0029474455, +0.0000921488): x bit-identical, |dz - 0.0000921488| <= 2.4e-7,
  |dy + 0.0029474455| <= 1.0e-5. Two gate arms: `D_B` (default grid) and `D_BRG` (the original's grid). d2posmodel
  failed the default grid on the grid pose (yaw -1.576028 vs -1.568874). The user decision d2posmodel 5.2 (which
  arm is the witness) is NOT made here, and both are reported as gates.
- **G-PROG** (`D_BRG`, d2posmodel amendment 1 reading): the v=0 prog on every pathprog row before the player's
  ptrace velocity first becomes nonzero is 29.999977 +- 1e-5. Control `D_RG` 0.0.
- **D2** (`D2_B_1..3`): slip 1500-2000 in 0.18855..0.19635, slip 2000-2600 in 0.24488..0.25487, driving-median in
  1904.70..1982.44 (bounds `d81a8df6`, unchanged), 3/3.
- **Recovery H1** (`D2_B_1..3`): >= 50 % of the 400 post-trough frames >= 100 AND median >= 900, 3/3.
- **Collateral**: `H_B` (floor `H_B-2`) at most 2 fields above `H_DEF` (floor `H_DEF-2`).
- **G-CTRL**: no-op `H_DEF` = `H_DEF-2` = the base exe's `H_DEF` (predicted `716cb2fd / 2a2afda1 / ba90ed88`).
  Determinism `H_B` = `H_B-2`.
- **D3 (b)** per regime: report against the base A 1/3, B 2/3, C 0/3.

## 4. Predictions (registered now)

4.1 No-op: this build's `H_DEF` = `H_DEF-2` = the base exe's hashes. PASS.
4.2 Part 4 knob-took as in 3, and engagement at d=15 on `D2_BO_1` and `D2_B_*`. `D2_B0_1` reproduces d2posmodel's
    d=14 / 1338.62 n=54. If d=14 persists under ORDER, 0.3 is refuted and that is reported.
4.3 Part 5 knob-took as in 3. With the seed reaching call 1, the 4 -> 3 drop translation lands on call 1 (as
    d2settle D0-e), so the settle's end position equals d2posmodel's to within 1e-6.
4.4 `H_K` (part 5 alone, default build): [UNCERTAIN] whether inert. It changes behaviour only on frames where the
    previous car's cache differs from this car's own (grounded flag or normals), or where a reader of the
    normals / `+0x9e0` runs between A4 and the solver. No direction predicted.
4.5 D2, recovery, D3 (b), collateral under B: no direction predicted. If B's launch returns to d=15, the trajectory
    should follow the default's to the first impact (DX vs BX are near-identical there, 0.5). Whether it escapes
    the wall is not predicted, because the U-9173 gap that decides it is untouched.
4.6 **Cause-2 attribution (report).** On a pinned arm the frames between re-contacts are in the low-speed band,
    and `MASHED_A6ADUMP` shows clamp #6 on the LOW arm with `act.kvel` well above the original's 0.1 floor
    (§26.3: 0.54 at 100-150). On those frames the lateral component falls by k per frame where the original's is
    held. Read with `a8_latinc.py`-style decomposition on the port's own log against `orig_solo3` idx 982-992.

## 5. Default rule (decided now)

Fix B (`MASHED_POS_MODEL`, parts 1-5) goes default ON on this branch, with opt-out `MASHED_POS_MODEL=0`, only if
ALL of these pass:
- G-SETTLE on both grids
- G-PROG
- G-LAUNCH 3/3
- no-op
- determinism
- the D2 three metrics 3/3
- recovery H1 3/3
- collateral within +2 of the base

Otherwise it stays default OFF, and the RESULT names the failed gate. Part 5 alone (`K`) goes default ON
independently only if `H_K` equals the base hashes (inert, so it is a pure fidelity fix of a dead path), or if
`D2_K_1..3` pass D2 + H1 3/3 with collateral within +2. Part 4 alone is not eligible: it only matters with the
seed. If driving-median (U-9180) moves into its bound in any arm, that is reported first.

## 6. D2 watch list (RE-PICKUP conditions of DEFERRED D-11071)

This lane changes `ReassertContacts` (watch item), the settle / release order and the release-tick reset (inputs
of `0x0046f6c0` and of A6a's boost arm). It does not change `+0x4a4`, `ProduceTerrainBatch` / `FUN_00538c80`,
grip-clamp #6 or the chunk loop. Every measured hit is reported in the RESULT.

## Amendment 1 (2026-10-16, after the D arms, round-1 D2 (`D2_DEF_1`, `D2_B0_1`, `D2_BO_1`, `D2_BK_1`, `D2_B_1`) and `BASE_H_DEF`/`-2`; before `D2_B0X_1` and before reading any other arm)

Recorded (exe `cf303269`, base exe `056295f9`; `SCORE_D2_ROUND1.txt`, `G_SETTLE.txt`, `RUNLOG.txt`):
- Base G-CTRL `BASE_H_DEF` = `BASE_H_DEF-2` = `716cb2fd / 2a2afda1 / ba90ed88` (prediction 4.1's reference holds).
- Knob-took part 5: PASS (`D_B` settle call 1 enters st (1,1,1,1) fv 0 gc 4, corr (0, -0.00296074, +0.0000925064);
  `D_B0` reproduces d2posmodel's `D_B` sha8 `8a6efcb4 / 7df792b1`). Part 4(b) visible: the first real step enters
  st (1,1,1,1) fv 0, corr 0. Prediction 4.3: end position bit-identical to `D_B0`. PASS.
- G-SETTLE: `D_BRG` PASS (dz +1.226e-7, dy -4.593e-6, x bit-identical); `D_B` FAIL (dz +3.575e-7, dy -1.330e-5),
  same margins as d2posmodel (default-grid spawn pose). G-PROG (amendment-1 reading of d2posmodel) `D_BRG` PASS
  29.999977 x23, control `D_RG` 0.0 x27.
- Knob-took part 4: PASS on `D2_BO_1` and `D2_B_1`. Marker at line 0, settle series line `bf4=3000 bf8=0`,
  release next, d=0 line `bf4=2800 bf8=2`. `D2_B0_1`: settle `bf4=2800`, d=0 `2600`. Section 0.3's explanation of
  cause 1 is confirmed by the witness.
- G-LAUNCH: d=15 on `D2_BO_1` and `D2_B_1` (fit 0.13 %), d=14 on `D2_B0_1` and `D2_BK_1` (fit 6.93 %).
- `D2_B_1`: 0.1953 in / 0.2500 in / 1865.19 OUT (n 368 / 526 / 1290); recovery 391/400, median 1293.1, H1 PASS.
  `D2_BO_1` 0.1953 / 0.2500 / 1864.87. `D2_BK_1` = `D2_B0_1` = d2posmodel's B (NA / NA / 1338.62 n=54, 11/400).
  `D2_DEF_1` 0.1927 / 0.2489 / 1860.27, 398/400.

Added, REPORT ONLY (no gate): `D2_B0X_1` = stack B0 + the DX diagnostics (wc log, off-mesh log, `MASHED_A6ADUMP`).
B no longer pins, so the pinned trajectory for the cause-2 attribution (4.6) is B0's. Prediction 4.6 applies to
`D2_B0X_1` (pinned) against `D2_BX_1` / `D2_DX_1` (not pinned): on the pinned frames between re-contacts, clamp #6
is on the LOW arm (`act.arm`) with `act.kvel` above 0.1. No numeric threshold.
