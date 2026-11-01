# RESULT D4-DC-77b0-STATE5-LEG2 (run 28, 2026-11-01)

**Verdict: DECISION.** `MASHED_ST5_LEG2=1` (default OFF) runs FUN_00425a40's body on every sub-state-5 call, in
FUN_0040fc00's order. The default gates are unchanged on this exe. Knob ON meets every preregistered witness (W1, W2', W4)
and moves the gate hashes, so the flip is filed as a decision row, not GREEN.

Base `orch/integration` @ `a147e22d3`. Commits on `orch/D4-DC-77b0-STATE5-LEG2`: PREREG `4faaa3131` (alone), knob
`f07d9320c`, PREREG amendment 1 `be08a5da6` (alone, before run 2), amendment code `a8b6b3a8f`, this RESULT.
PREREG: `PREREG_D4-DC-77b0-STATE5-LEG2.md` (this folder).

## What changed (knob ON only)

`D3d9Render/TrackRenderer.cpp`, `TrackRenderer::St5Leg2`, called from the sub-state block on any call that entered
case 5, including the exit call (leg 1 stores 6 at 0x004104e3 before FUN_0040fc00 runs, so S(0x00425a78) reads 6 on the
original's exit tick, ORIGREAD RESULT):

| original (FUN_00425a40, 0x00425a40) | exe, knob ON |
|---|---|
| FUN_0040fc00 0x0040fc57 / 0x0040fc63 before 0x0040fc6c | `Ai_AdvanceClock(units)` moved ahead of leg 2 and `SubStateCameraChain(false)`, the later call skipped |
| FUN_00423b00 + the DAT_007f1a50 test | same dwords read. Callees not ported, logged if DAT_007f1a50 == 1 is ever seen (0 here) |
| FUN_00418860 (0x00425a73) | `AiBridgeSnapshot(); Ai_Standalone_Tick();`, the guard leg 1 uses |
| FUN_00424eb0 (0x00425a82) | `s_roundEnd7` loop, `HeadingAssist_Frame(units)`, `HeadingAssist_Player(units, 0, 0)` |
| FUN_00470c70 (0x00425a8d) | AI A4 pre-phase from the ctrl blocks, `VehiclePhysics_StepCarsOuter` (fallback `StepPlayer`), write-back. Player input = leg 1's cooked accel byte. Amendment 1: held AI cars get `a.vel = 0` first (FUN_0046baa0 0x0046babb, gate 0x00410425 / 0x0041042c) |
| FUN_00422ba0 (0x00425a95) | `Ai_ContactEventDrain()` under its gate |
| FUN_004252c0 (0x00425a9d) | not ported (no exe port on the race path either). Not run |

Knob OFF: the `else if (ba8c == 5)` arm still calls only `SubStateHeldCountdown5_C`, the `Ai_AdvanceClock` call stays
where it was, and the G0 hashes below are the refs.

## Static read used (decomp, pool0, 2026-11-01)

- FUN_00470c70's per-car gate before A4 and FUN_004709a0's substep gate: record +0x14 == 0 and (+0x4 != 0 or +0x10 == 1).
  o_t0_r1.msd (car 0): +0x4 = 1, +0x10 = 0, +0x14 = 0 on frames 0..1526, so car 0 is stepped during the countdown.
- o_t0_r1.msd car 0 +0x958/+0x95c/+0x960 = bytes `643d003f / 1474de3e / 6eddffbf` on every frame 2..848, first change at
  frame 849. +0x9b0..+0x9b8 = 0 and +0xbf4 / +0xbf8 = 0 on those frames.

## Run 1 (exe 3097a90c, commit f07d9320c)

- G0 = refs: G-CTRL 8284be46/ba407a16/31fe043e, B 1d5bf4f9 x3.
- K1: G-CTRL 2eb711b3/c9abe7ef/83706807, B d6acc2f3 x3.
- W1 held (112 / 112). W2 as first written failed: car 0's record was 00000000 x3 before the first leg-2 step (the
  sub-state path does not seed it) and 3f003d64/3ede7414/bfffdd6e after the last, which is the original's value.
- Defect: the AI cars drove off during the countdown. B_ST5L2_K0 frame 0 own_z = -22.41 / -24.95 / -19.13, rec_9e4 =
  7044.65 / 3089.95 / 7563.92 (default -0.90 / 0.20 / 1.30, 0). Cars 1 and 3 had no rows after frame 0, and the round ended
  at 14.6 s instead of 18.3 s. Cause: the block top zeroes car_vel_ and cur_speed, not a.vel, and leg 2's physics takes
  aio.vel = a.vel. Fixed by amendment 1 (PREREG, committed before run 2).

## Run 2 (exe dd27c937, commit a8b6b3a8f), the measured result

| arm | file | lines | sha8 | ref |
|---|---|---|---|---|
| H_DEF | step / gates / ptrace | 13962 / 54085 / 27042 | 8284be46 / ba407a16 / 31fe043e | equal |
| B_DEF_K0 / K1 / K26 | step | 762 x3 | 1d5bf4f9 x3 | equal |
| H_ST5L2 | step / gates / ptrace | 13824 / 54085 / 27042 | c2093a9b / 80b1132b / fd4adc15 | moved |
| B_ST5L2_K0 / K1 / K26 | step | 624 x3 | 36c6ebdc x3 | moved |

All 8 arms exited 0 by themselves.

Witnesses (H_ST5L2.log and B_ST5L2_K0.log give the same lines):
- W1 holds: `[st5leg2] leg1 calls=112`, `exit calls=112` (original: 112 on both tracks).
- W2' holds: `first=3f003d64/3ede7414/bfffdd6e post=3f003d64/3ede7414/bfffdd6e first_eq_post=1`. Car 0's record position
  after every leg-2 step from the first to the last equals the original's countdown value, bit for bit.
  (`pre=00000000 x3` is the unseeded record before the first step.)
- W4 holds. Step-dump frame 0, knob ON vs default:

| car | own_x ON / DEF | own_z ON / DEF | abs dz | rec_9e4 ON / DEF | rows at frame 10 |
|---|---|---|---|---|---|
| 1 | -1.10121727 / -1.10121727 | -1.0442313 / -0.902031064 | 0.142 | 180 / 0 | yes |
| 2 | 1.1367228 / 1.1367228 | 0.0162096713 / 0.202357411 | 0.186 | 280 / 0 | yes |
| 3 | -0.465431273 / -0.465431273 | 1.20977938 / 1.2993629 | 0.090 | 5.33341455 / 0 | yes |

  The default's first race tick (frame 1) gives rec_9e4 180 / 280 / 280. With the knob, the exit call's leg 2 steps the
  cars after leg 1's release (0x0041049b), as the original orders it, so that drive lands one tick earlier.
- Race outcome, knob ON: winner car 1 at 17.1 s (`ELIM-RESOLVE alive=0,1,0,0`). Default: winner car 3 at 18.3 s. AI rows
  ON: car 1 to frame 13520 (H), cars 2 and 3 to frame 150. Default: car 3 to the end, cars 1 and 2 to frame 219.

## Open item

[UNCERTAIN] The AI cars' 0.090..0.186 z offset at GO with the knob ON (W4's band is 0.5) is not compared against the
original. The ORIGREAD capture records car 0 only (`statediff.car = 0`), and car 0 has no throttle in that recipe
(+0xbf4 / +0xbf8 = 0). Missing: one original boot with the record capture on cars 1..3 through sub-state 5 (same recipe,
`--track 0 --mode 10 --cars 4 --rule 0 --poke-ctrl-slots`), to compare each AI car's +0x958/+0x960 at frame 849 with the
exe's frame 0.

## Not run

D2 arms and the 13-track run13 table, under the knob. The flip pass's merge train runs both if the flip is decided.

## Decision filed

`roadmap.ps1 decision D4-DC-77b0-STATE5-LEG2`, options:
(a) flip `MASHED_ST5_LEG2` default ON now (new refs G-CTRL c2093a9b/80b1132b/fd4adc15, B 36c6ebdc x3, plus D2 and run13
in the flip pass);
(b) one original boot first, capturing the AI records of cars 1..3 through sub-state 5 (the open item), then flip;
(c) no flip.
Recommendation (the PREREG rule: G0 equal, W1, W2', W4 hold, no crash): (a). (b) costs one original boot and closes the
only unmeasured part.

## Files

Committed: `PREREG_D4-DC-77b0-STATE5-LEG2.md`, this RESULT, `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.{cpp,h}`,
`verify/savelink_20261023/run_arms.py` (stack `ST5L2`).

Raw (not committed), `D:/MashedWT/D4-DC-77b0-STATE5-LEG2_data/`. Exe `mashed_re.exe` (run 2) sha256
`dd27c9373a12a1e6bd4628c4e420a546d6e7441b7404175ed37ba7cf7d4abc83`.

| file | SHA-256 |
|---|---|
| arms2/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms2/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms2/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms2/B_DEF_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms2/H_ST5L2.step.csv | c2093a9b4ba52fec1ff6f34d34c36a9e3aa8d7f96dce20140bbd18c75625d972 |
| arms2/H_ST5L2.gates.csv | 80b1132bf98a9a897d877d7370009a308b246cf2e253a58a34d28d548f514066 |
| arms2/H_ST5L2.ptrace.log | fd4adc1531d4ad4a6087848c63ecd241a6561282d60b1a2d889949b3af03e648 |
| arms2/H_ST5L2.log | 078304e8e9fdb224f14e762da7c6d0699e45e3820aa15f46eb07f1f89a905f39 |
| arms2/B_ST5L2_K0.step.csv | 36c6ebdcbe46eca17e083d371dcc42d44ad18d06b90bae32dffbeae57d36dd71 |
| arms2/B_ST5L2_K0.log | 990231dd4edda7f69c40363ce8b7cc0dc22f8b0572032366280d76812cc3cf0c |
| arms2/RUNLOG.txt | 1dc7a395b8088a9592ed22ab324ae7cc75d69dec51a5dfdbf952aa7ac48ff1be |
| arms/H_ST5L2.log (run 1) | 818fc7a5629a70d5272f1c6a6c892379e8393c58dcbeb17b7720b3f8d2912e6d |
| arms/B_ST5L2_K0.step.csv (run 1) | d6acc2f312420ff28e7b50e66351a20a5b3fac531718ab4e07440c9d3cf9f83c |
| arms/B_DEF_K0.step.csv (run 1) | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/RUNLOG.txt (run 1) | 676a05e62310b44df46d4d67b1184b073dd8fc5efb2791f141f776b8a5e8f099 |

Original-side input, from `D:/MashedWT/D4-DC-77b0-STATE5-ORIGREAD_data/` (hashes in the ORIGREAD RESULT): `o_t0_r1.msd`.

## Follow-up: L-75 answered (b), original AI capture (2026-11-01), outcome MISMATCH

PREREG amendment 2 (`25d6afb5a`, committed alone before the boots) set the band. Three original boots, ORIGREAD recipe
plus `--statediff-car 1 / 2 / 3`, private root `D:\MashedWT\D4-DC-77b0-STATE5-LEG2_root` (MASHED.exe sha256 b9977dab...),
one `mashed_lock.py run` hold, VB-Audio first in `Get-PnpDevice -Class AudioEndpoint -PresentOnly` before each boot.
All three exited 0, no crash, no piz collision. Summary: `SUMMARY_orig_ai_hold.csv` (this folder).

Original, record +0x958/+0x960 per AI car (o_c1/2/3.msd):

| car | held value from frame 2 (x, z) | last held frame | dx, dz over the hold | first moving frame: z, vel z |
|---|---|---|---|---|
| 1 | -1.10121727, -0.901938975 | 801 | 0, 0 | 802: -0.910264313, -180 |
| 2 | 1.1367228, 0.20244953 | 821 | 0, 0 | 822: 0.18949911, -280 |
| 3 | -0.465431273, 1.29945505 | 868 | 0, 0 | 869: 1.28650463, -280 |

Frames 2 to the last held frame (800 / 820 / 867 frames, which contain the 112-tick sub-state-5 window) give bit-identical
+0x958/+0x960 and +0x9b0..+0x9b8 = 0. The original does not move a held AI car during the countdown. Frame 1 to 2 is a
one-time settle (z +9.2e-5, y -0.00295), the same as car 0 in o_t0_r1.msd.

Port, knob ON (run 2): dz = -0.142 / -0.186 / -0.090, dx = 0. Differences against the original: |dz| 0.142 / 0.186 / 0.090,
all above the 0.05 band. **MISMATCH on all three cars.** No flip committed.

Cross-check: the original's first moving frame equals the DEFAULT exe's first race tick bit for bit (car 1 z -0.910264313,
vel z -180; car 2 0.18949911, -280; car 3 1.28650463, -280; B_DEF_K0 step dump frame 1). So for the AI cars, the default
build already matches the original at GO, and the knob moves them away from it. Car 0 (W2') matches with the knob
ON as well, but car 0 has no throttle in this recipe.

What explains part of it (code, not measured): with the knob ON the exe's AI cars get the ctrl block's throttle in
leg 2's physics on every held tick. The amendment-1 velocity zero runs before the step, not after it, so each tick's
two substeps still move the car. Over 112 ticks that adds up to the measured drift. On the original the held car's position
does not change at all, so something there keeps a held car's integrated position fixed (candidates, unverified:
FUN_0046baa0 resets more than velocity, A4's +0xaec countdown skip at 0x00470fe0.., or the A6a drive gate). [UNCERTAIN]:
which one. Missing: a watch-write on an AI record's +0x958 during sub-state 5 on the original (zero writes vs writes of
the same value).

Raw (not committed), `D:/MashedWT/D4-DC-77b0-STATE5-LEG2_data/orig/`:

| file | SHA-256 |
|---|---|
| o_c1.msd | 6337c44553e7051be0fd6ededf3c628a999f9fc82d7adc58238cb620279f5b10 |
| o_c1.log | fcb16b74320db2eaf0a153c4cd51d5bc173002f9769ce38716b5ccb3caba3962 |
| o_c1.msd.provenance.json | b76abd89d8839cebb6767ea67634510f9b6d4ec1a985545704b4f24313713050 |
| o_c2.msd | dfd048a251d79b477c9d8d42065571b414e09ba47f0913a50db7df7d48a8495f |
| o_c2.log | c32da288a63ad57a4545f1a73475dcfbbe430c240c7b6da98eeb11774384a1b7 |
| o_c2.msd.provenance.json | 7f3176c37906c15b8fea7dbf5ce4dc984d7b3045e5f4d456ccdedbb1a61be94e |
| o_c3.msd | 4b621b3cabb4e28f5d17d45aadb17b87dde0f659557b8192d4b1acd328cd3ee2 |
| o_c3.log | 622be3e35fb7ae8a712dafebf3904c8b6f632132a63d090dd2f264768e23f0a9 |
| o_c3.msd.provenance.json | 8bbfb98aa6887fd58f37035e9bc06a1a90bf083b3d6555a94966a74ffed54a1b |

Gates: not re-run (no build input changed since run 2; default exe dd27c937, G-CTRL 8284be46/ba407a16/31fe043e,
B 1d5bf4f9 x3).
