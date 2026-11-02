# RESULT D4-DC-77b0-ST5-HOLD (2026-11-01)

**Verdict: DECISION.** The hold is found and ported. On the original, leg 2 runs every sub-state-5 call at
FUN_0040e350() = DAT_0063ba8c = 5. That hands every car the neutral ctrl block 0x007f19b8 and zeroes FUN_0046e9e0's
position increment. With `MASHED_ST5_LEG2=1` the exe now does the same and holds cars 1..3 at the original's held
x/z **bit for bit**. Every preregistered witness holds (G0, W1, W2', W5, W6). The default gates are unchanged.
The flip is **not recommended yet**: knob ON, the AI cars lose the original's start boost at GO, for a reason this
step does not fix (section "Not fixed").

Branch `orch/D4-DC-77b0-ST5-HOLD` from `3e9345cc8` (orch/D4-DC-77b0-STATE5-LEG2). Commits: PREREG `4eb550230` (alone,
before any knob arm), code `d81ff0ba1`, this RESULT. PREREG: `PREREG_D4-DC-77b0-ST5-HOLD.md` (this folder).

## What holds a held AI car on the original

Three original boots: ORIGREAD recipe + `--statediff-car 1/2/3 --watch-write 958,960 --statediff-aictrl`. Private root
`D:\MashedWT\D4-DC-77b0-ST5-HOLD_root` (MASHED.exe sha256 b9977dab...). One `mashed_lock.py run` hold. VB-Audio was first
before each boot. All three exited 0, with no piz collision.

- The held values reproduce `verify/d4dc77b0st5leg2_20261101/SUMMARY_orig_ai_hold.csv` bit for bit.
- **The position is written on every held frame, with the same value.** Car 1: 822 hits at pc 0x0046ea62 and 822 at
  0x0046ea76. Both are FUN_0046e9e0's stores right after the 0x0046ea1e..0x0046ea4c test, called from 0x00470948
  (FUN_004709a0). Car 2: 815/816 per site. Car 3: 890. So physics does run during the hold. It adds zero.
- **The substep runs on the neutral block.** On every held-frame hit at 0x004c4a29 (ret 0x0046ef63), the stack holds
  0x007f19b8. After GO the same slot holds 0x007f1084, car 1's own block (0x007f1038 + 1 * 0x4c).
- **The AI's accel is ignored during the hold.** Car 1's own block has c4 = 255 on held frames 819..823, and leg 1's
  FUN_0046d7f0 counts it (+0xbf4 50..250). +0xb1c and +0x9e4 stay 0.0 until GO (frame 824).
- Static, decomp pool0 + capstone on MASHED.exe.unpatched:
  - FUN_0040e350 = `return DAT_0063ba8c`.
  - FUN_00470c70, FUN_004709a0 and FUN_0046c7d0 pick 0x007f19b8 unless that value is 6, 0xb or 0xa.
  - FUN_0046e9e0 gates the position increment on the same test.
  - Leg 2's units are leg 1's delta (0x004112b5 `mov esi,[esp+0xc]` / `mov eax,esi`, pushed at 0x0040fc6b). So the
    hold is not a zero step.
  - A6a's drive arm (FUN_00467650) has no sub-state test of its own. The front wheels are state 2 throughout.

## Change (knob ON only)

`TrackRenderer::St5Leg2`: on every non-exit call, a scope guard sets `Vehicle::g_fiGameModeHold = 5` and raises
`s_st5LegHold` for the whole leg-2 body. The exit call keeps 6 (leg 1 stores 6 at 0x004104e3 first).
`SubStateModeGateActive()` = `RoundEnd7PhysActive() || s_st5LegHold` now drives the two existing gate sites in
VehiclePhysicsRun.cpp: the 0x007f19b8 block substitution and the 0x0046ea1e position-increment gate.
The d2engage settle restores the previous `g_fiGameModeHold` instead of writing 0. That is identical when nothing else
set it.
`MASHED_ST5_HOLD_DIAG` (log only) prints the ctrl bytes and position of every AI car on every leg-2 call. Check: arm
`B_ST5L2D_K0` with the pre-change exe = step sha8 36c6ebdc, the same as run 2's B_ST5L2_K0.

## Arms (exe fefde1bb, commit d81ff0ba1), all exited 0 by themselves

| arm | sha8 | ref |
|---|---|---|
| H_DEF step / gates / ptrace | 8284be46 / ba407a16 / 31fe043e | equal (G0) |
| B_DEF_K0 / K1 / K26 | 1d5bf4f9 x3 | equal (G0) |
| H_ST5L2 step / gates / ptrace | 4689b9ef / 0d074af9 / 193eba5e | moved |
| B_ST5L2_K0 / K1 / K26, B_ST5L2D_K0 | 8f15d7b4 x4 | moved |

Witnesses:
- W1: `leg1 calls=112`, `exit calls=112`.
- W2': `first_eq_post=1`.
- W5: each AI car's position is identical on calls 2..112 (cars 1, 2, 3).
- W6: band 0.05. Table: `SCORE_hold_band.csv`.

| car | held x/z, exe vs orig | dx, dz held | frame 0 own_z vs z_go | dz go |
|---|---|---|---|---|
| 1 | -1.10121727 / -0.901938975 both | 0, 0 | -0.901938975 vs -0.910264313 | 0.0083 |
| 2 | 1.1367228 / 0.20244953 both | 0, 0 | 0.20244953 vs 0.18949911 | 0.0130 |
| 3 | -0.465431273 / 1.29945505 both | 0, 0 | 1.29920828 vs 1.28650463 | 0.0127 |

The original's frame-1-to-2 settle (z +9.2e-5) is also reproduced. Call 1 enters at the spawn z (-0.902031064),
and calls 2..112 hold the original's frame-2 value.

## Not fixed: the start boost at GO (knob ON)

Record dumps of car 1 and car 3 (`MASHED_RECDUMP`, stacks `RD1/RD3`, `ST5L2RD1/ST5L2RD3`), frame 0:

| car | default +0xbf4 / +0xbf8 / +0x9e4 | knob ON +0xbf4 / +0xbf8 / +0x9e4 |
|---|---|---|
| 1 | 1100 / 1 / 180 | 2600 / 2 / 0.638 |
| 3 | 1100 / 1 / 280 | 0 / 0 / 10.92 |

- The default matches the original: release at +0xbf4 = 250 gives 1250 and +0xbf8 = 1. The original's first moving
  frame equals the default's frame 1 bit for bit (SUMMARY cross-check).
- Knob ON, car 1 releases over the 1000 cap. +0xbf8 = 2 zeroes the drive force (+0xb1c = 0.0) for 13+ frames, and
  +0x9e4 climbs 0.64 to 2.17 over frames 0..12 (default: 180 to 1495).
- Knob ON, car 3 gets no boost at all.
- Race outcome knob ON: winner car 1 at 17.7 s. Default: car 3 at 18.3 s.

Cause, measured: the ctrl byte c4 that leg 1's FUN_0046d7f0 reads.

| | car 1 | car 2 | car 3 |
|---|---|---|---|
| original (aictrl) | 0 until GO-5 frames, then 255 | 0 until GO-5 frames, then 255 | 0 until GO-5 frames, then 255 |
| exe, knob ON (`[st5hold]` on every leg-2 call) | 0xff on all 112 calls | 0xff on all 112 calls | 0xff on call 1, then 0x40 x 111 |

The default exe releases at 250 (+0xbf4 1100 on frame 0), so its accel timing matches the original's. In run 2
(knob ON, no hold) A6a's mode-6 site re-set +0xbf4 on every leg-2 call, which hid this. Mode 5 removes that site, as on
the original, so the difference now shows.

[UNCERTAIN] Why the exe's AI writes accel from call 1 once leg 2 runs. The knob adds a second `Ai_Standalone_Tick()`
per call (leg 2's FUN_00418860) and moves `Ai_AdvanceClock(units)` ahead of leg 2. The original also ticks the AI twice
per call and still holds accel at 0 until GO-5 frames. Missing evidence: a per-call log of the exe AI's accel decision
(c4 writer and its inputs) on leg 1 vs leg 2, knob ON vs OFF.

## Decision filed

`roadmap.ps1 decision D4-DC-77b0-ST5-HOLD`. Options:
- (a) Flip `MASHED_ST5_LEG2` ON now. New refs G-CTRL 4689b9ef/0d074af9/193eba5e, B 8f15d7b4 x3. D2 and run13 would run
  in the flip pass.
- (b) Keep it OFF. Add a follow-up step to find why the exe AI's c4 is 0xff / 0x40 from call 1 under leg 2, then
  re-measure the GO boost (+0xbf4 / +0xbf8 at frame 0 against the default's 1100 / 1).
- (c) Close with no flip.

Recommendation: **(b)**. The hold is now exact, but flipping would trade a 0.09..0.19 position error during the hold
for a wrong launch, where the default currently matches the original at GO.

## Files

Committed:
- `PREREG_D4-DC-77b0-ST5-HOLD.md`, this RESULT, `SCORE_hold_band.csv`
- `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp`, `mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp`
- `verify/savelink_20261023/run_arms.py` (stacks ST5L2D, RD1-3, ST5L2RD1-3)

Raw (not committed), `D:/MashedWT/D4-DC-77b0-ST5-HOLD_data/`:

| file | SHA-256 |
|---|---|
| orig/w_c1.msd | 39190f00a02cf3349a3e599dcac6276e4cbed6ef1727f46429ce856d89bfad37 |
| orig/w_c1.msd.watch.csv | 7331a0d8bccdc56e701d9133b55de4d728bf189d66a6d9b98f625ccac11b8a24 |
| orig/w_c1.msd.aictrl.csv | 5800d44c29c17b42616bb03a6d03fdfd02d2b1e1ec6761410ae0417fe35b4e53 |
| orig/w_c1.log | 0c427f23506ed337dce1cac5f0c8a51a0f04996a392d8746b0f504e2b49f2a67 |
| orig/w_c2.msd | 29568752bce31ce979f79cb48fce54430b9aadec4c7a7ece3580b5de9e91608f |
| orig/w_c2.msd.watch.csv | 4937c58e4ffd5a9722c99720fce12e9540011a01b660151f289d2cde90ba2caf |
| orig/w_c2.msd.aictrl.csv | df1087c41f84cbb07b4e3a10cd717554cd9210701c01b5518b581d8180e8b546 |
| orig/w_c3.msd | 7cc475b2e9abfb416470f7482274c2980f0d89b583e249eed9523e6fbd51b9dd |
| orig/w_c3.msd.watch.csv | 3d1244f237168eee437543f365058fbed922d81bd9c0fc24b2619b7cbbfb9d0c |
| orig/w_c3.msd.aictrl.csv | 035e4bab216bb097cdfa4732bb9d13a3dd605979500c5f126a8dc50462e2fac9 |
| orig/boots.log | 664e7d9265f20d3a8f05e4c24318ee5ff7078cce2fb9e5d21afd2b166df8fd7f |
| arms/B_ST5L2D_K0.log (pre-change exe 1c511dd9) | c62c648a2e29556ffecc2a23daa7fdb65b2c85436090a0b6b3276f8a10c4310d |
| arms/B_ST5L2D_K0.step.csv (pre-change) | 36c6ebdcbe46eca17e083d371dcc42d44ad18d06b90bae32dffbeae57d36dd71 |
| arms3/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms3/H_ST5L2.step.csv | 4689b9efd6ae24e30f0df3fe75853ed5b56651826c72800ce343b4acdfe1d9f7 |
| arms3/H_ST5L2.gates.csv | 0d074af92d0bce21c6d81f62eb8b676277aaf44427af84c57e16d16617f83b89 |
| arms3/H_ST5L2.ptrace.log | 193eba5eb6c2da599db5e5002eb2339d7dc9c4cb2b5bd6b62a5a7e0435aa8d5b |
| arms3/H_ST5L2.log | 44e83606390af8294490396b2da8eb125c83e3d9dd0876fa8e43f6a43a98b6e5 |
| arms3/B_ST5L2_K0.step.csv | 8f15d7b4f09d7234d2ceb9ca5ccde5c07ee4b15be1d715508ed0cccf9ff38e83 |
| arms3/B_ST5L2D_K0.log | baad5ecf6ad3448f1b8f3f1bea9e56f6200ec37517f55e937e1e890302b161dd |
| arms3/RUNLOG.txt | ff062117f0a45ea7b9ee24961d8e47dbf2cc76fbe37c3f6f229941309f229e27 |
| arms4/B_RD1_K0.rec.msd | 1741c6a02360ba219ead613d31dc1dbd6d2e49a7cfa888541d2689554a3a0b24 |
| arms4/B_ST5L2RD1_K0.rec.msd | dab0fe1f34c9a96ac0924d75ad72e059b9a1b8f74b81e049a3520b0f06f3abba |
| arms4/B_RD3_K0.rec.msd | d3f7b3aed0832aad4d8784d39aa41837e964df9925ddbf4c2c7c74d799144a31 |
| arms4/B_ST5L2RD3_K0.rec.msd | f11e19a4963a828ceec352f7a684b8de56ef63973a1a12e32dbafd990a15d81c |

Readers (scratch, not committed): `msd_fields.py` (MSD1 field dump, 16-byte header) and `ww_summary.py` in the same
data folder.
