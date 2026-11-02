# PREREG D4-DC-77b0-ST5-HOLD (2026-11-01)

Follow-up to L-76 (a) on `D4-DC-77b0-STATE5-LEG2` (branch base `3e9345cc8`). Committed alone, before any knob arm.

## Measured before this PREREG (original, no knob arm yet)

Three original boots, ORIGREAD recipe (`--track 0 --mode 10 --cars 4 --rule 0 --poke-ctrl-slots --hold 30`) plus
`--statediff-car 1 / 2 / 3 --watch-write 958,960 --statediff-aictrl`, private root `D:\MashedWT\D4-DC-77b0-ST5-HOLD_root`,
one `mashed_lock.py run` hold, VB-Audio first before each boot. All exited 0.

- The held values reproduce `verify/d4dc77b0st5leg2_20261101/SUMMARY_orig_ai_hold.csv` bit for bit (car 1 +0x958/+0x960 =
  `bf8cf4b0 / bf66e579` from frame 2 to GO-1, car 2 `3f918022 / 3e4f4eee`, car 3 `beee4d02 / 3fa6548b`).
- The position is written on every held frame, with the value it already had. Car 1: 822 hits each at pc `0x0046ea62` and
  `0x0046ea76` (FUN_0046e9e0, the stores after the 0x0046ea1e..0x0046ea4c gate), caller 0x00470948 (FUN_004709a0). Cars
  2 / 3: 815-816 / 890 per site. So the hold is not "no physics". The substep runs and adds a zero increment.
- On every held-frame hit at 0x004c4a29 (the ortho under FUN_0046e9e0, ret 0x0046ef63), the stack holds `0x007f19b8`, the
  neutral ctrl block. After GO the same slot holds `0x007f1084` = 0x007f1038 + 1 * 0x4c, car 1's own block.
- Car 1's own block carries accel 255 (`c4`) on held frames 819..823 (aictrl sidecar), and leg 1's FUN_0046d7f0 counts it
  (+0xbf4 = 50, 100, .., 250). +0xb1c (drive force) stays 0.0 and +0x9e4 stays 0.0 on those frames. GO = frame 824.

Static (pool0 decomp + capstone on MASHED.exe.unpatched):
- FUN_0040e350 (0x0040e350) is `return DAT_0063ba8c`, the race sub-state.
- FUN_00470c70 (0x00470fd6..0x0047100a) and FUN_004709a0 (0x00470a1b..0x00470a4f) hand A4 and FUN_0046e9e0 the block
  0x007f19b8 unless FUN_0040e350() is 6, 0xb or 0xa. FUN_0046e9e0 zeroes the position increment under the same test
  (0x0046ea1e..0x0046ea4c). FUN_0046c7d0 picks its block the same way (decomp, three FUN_0040e350 tests). The address
  ranges are the ones VehiclePhysicsRun.cpp:1364-1365 and :1846 already cite. The tests themselves are in the decomp.
- Leg 2's units are leg 1's delta: FUN_004111c0 case 5 does `mov esi,[esp+0xc]` / `mov eax,esi` (0x004112b5) before
  FUN_004103a0 and FUN_0040fc00, and FUN_0040fc00 pushes esi into FUN_00425a40 (0x0040fc6b).
- On the exit call leg 1 stores 6 at 0x004104e3 before FUN_0040fc00 runs (ORIGREAD RESULT), so that call's leg 2 sees 6.

So on the sub-state-5 calls the original's leg 2 runs with FUN_0040fc00's mode = 5: neutral block, zero position increment.
The exe's leg 2 runs with `Fi_GameMode()` = 6 (the in-race stand-in, ForceIntegratorStubs.cpp:128) and applies the neutral
block and the position gate only under `RoundEnd7PhysActive()` (VehiclePhysicsRun.cpp:1367, :1858). That is the knob-ON
drift of 0.090..0.186.

## Change (MASHED_ST5_LEG2 only, default OFF stays OFF)

In `TrackRenderer::St5Leg2`, on every call that is not the exit call: set `g_fiGameModeHold = 5` (DAT_0063ba8c on case-5
ticks) for the whole leg-2 body, and raise a leg-2 flag that makes the two existing gate sites in VehiclePhysicsRun.cpp
(neutral block at :1367, position increment at :1858) apply as they do under ROUND_END7. Restore both before returning.
The exit call keeps mode 6 (0x004104e3). The d2engage settle hold (VehiclePhysicsRun.cpp:1240) is not touched. Knob OFF:
the flag is never raised, so the default build executes no new path.

`MASHED_ST5_HOLD_DIAG` (default OFF, log only) prints each AI car's ctrl bytes and position on every leg-2 call. Arm
`B_ST5L2D_K0` with the pre-change exe gave step-dump sha8 `36c6ebdc`, equal to run 2's `B_ST5L2_K0`, so the log adds no
behaviour.

## Arms

Exe from this branch, `MASHED_NO_DEPLOY=1`, private root `D:\MashedWT\D4-DC-77b0-ST5-HOLD_root`, all under one
`mashed_lock.py run`: `H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_ST5L2 B_ST5L2_K0 B_ST5L2_K1 B_ST5L2_K26 B_ST5L2D_K0`
(`verify/savelink_20261023/run_arms.py`).

## Witnesses and band

- G0: H_DEF and B_DEF_K0/K1/K26 equal `re/orchestrator/merge_train.json` refs (G-CTRL 8284be46/ba407a16/31fe043e,
  B 1d5bf4f9 x3).
- W1: `[st5leg2] leg1 calls=112`, `exit calls=112`.
- W2': `first_eq_post=1` (car 0's record after every leg-2 step equals its first-step value).
- W5 (hold): B_ST5L2D_K0, every AI car's `[st5hold]` position is the same on every non-exit call from call 2 to call 112.
- W6 (band, cars 1..3, against `SUMMARY_orig_ai_hold.csv`): |x - x_held| <= 0.05 and |z - z_held| <= 0.05 for the
  position at the exit call's entry (the last held state), and |own_x - x_go| <= 0.05 and |own_z - z_go| <= 0.05 for the
  B_ST5L2_K0 step dump at frame 0.
- All arms exit 0 by themselves.

## Outcome rule

All of G0, W1, W2', W5, W6 hold: verdict DECISION, the default flip is filed as an L-n decision row with the H_ST5L2 /
B_ST5L2 hashes as the expected new refs (D2 and run13 left to the flip pass). G0 fails: RED, fix before anything else.
W5 or W6 fails: RED, MISMATCH reported with the per-car numbers, no flip.
