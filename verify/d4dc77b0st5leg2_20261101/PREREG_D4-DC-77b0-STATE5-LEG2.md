# PREREG D4-DC-77b0-STATE5-LEG2 (run 28, 2026-11-01)

Committed alone, before any code change and before any arm. Base `orch/integration` @ `a147e22d3`.

## What the original does (cited)

- FUN_004111c0 case 5 = `FUN_004103a0(); FUN_0040fc00();` (STATE5CALLS RESULT, `verify/d4dc77b0state5_20261030/`).
- FUN_0040fc00 stores DAT_007f1008 / _DAT_007f100c (0x0040fc57), adds the clock (0x0040fc63), then calls FUN_00425a40 at
  0x0040fc6c when DAT_005f29c0 != 0, then FUN_0040da50 (0x0040fc75) and FUN_0040d470(0).
- FUN_00425a40 (decomp of 0x00425a40, pool0 2026-11-01): `FUN_00423b00();` then, when DAT_007f1a50 == 1, `return 0` if
  DAT_007f1a54 != 4 or (DAT_007f1a74 == 0 and DAT_007f1a70 == 0), else `DAT_007f1a78 += 1`. Then FUN_00418860,
  FUN_00424eb0(units, &DAT_00803324), FUN_00470c70(units, &DAT_00803324), FUN_00422ba0(units), FUN_004252c0(units),
  `return 1`. FUN_00423b00 calls its four callees only when DAT_007f1a50 == 1.
- Measured (`verify/d4dc77b0st5orig_20261101/`, TRAINING and Egypt): every one of the 112 case-5 calls takes this body
  (0x005f29c0 = 1, 0x007f1a50/54/70/74/78 = 0, FUN_00425a40 returns 1).
- FUN_00470c70's per-car gate before A4 (0x00470f80..0x00471099, decomp): record +0x14 == 0 and (+0x4 != 0 or +0x10 == 1).
  FUN_004709a0 uses the same gate (+0x14 == 0, +0x4 != 0 or +0x10 == 1) for the substeps. On o_t0_r1.msd (car 0) +0x4 = 1,
  +0x10 = 0, +0x14 = 0 for frames 0..1526, so car 0 passes the gate during sub-state 5.
- Car 0's record position +0x958/+0x95c/+0x960 in o_t0_r1.msd is `643d003f / 1474de3e / 6eddffbf`
  (0.500936747 / 0.434479356 / -1.998945) on every frame from 2 to 848 and first changes on frame 849. +0x9b0..+0x9b8 = 0
  on every one of those frames. +0xbf4 / +0xbf8 = 0 on every frame of the capture. So on the original the second leg's
  physics step does not move car 0 during the countdown, bit for bit.

## What the exe does today

TrackRenderer.cpp's sub-state block (`if (countdown_ > 0.f)`, ~5205-5400) runs `SubStateHeldCountdown5_C(units)` (leg 1:
the AI tick through `s_ssHost.ai_tick`, the hold loop, the release) and then only FUN_0040fc00 partials. No FUN_00425a40
body runs in sub-state 5.

## The change (default OFF)

Knob `MASHED_ST5_LEG2=1`. Only on a call where DAT_0063ba8c == 5 at entry to the case-5 arm (so the exit call, which stores
6 at 0x004104e3 inside leg 1, runs leg 2 too, as measured: S(0x00425a78) reads 6 on the exit tick):

1. FUN_0040fc00 order: the unit / step stores (already there), then `Ai_AdvanceClock(units)` (0x0040fc57 / 0x0040fc63),
   moved ahead of leg 2 and of `SubStateCameraChain(false)`. The later `Ai_AdvanceClock` call is skipped on that frame.
2. FUN_00425a40 body, in order:
   - FUN_00423b00 + the 0x007f1a50 test, read from the same dwords. The four FUN_00423b00 callees are not ported: a logged
     `ExeOnce` if DAT_007f1a50 == 1 is ever seen (0 in the exe, 0 on the original).
   - FUN_00418860: the same host call leg 1 uses (`AiBridgeSnapshot(); Ai_Standalone_Tick();`, same guard).
   - FUN_00424eb0: the same exe pieces the race frame uses (`s_roundEnd7` loop, `Race::HeadingAssist_Frame(units)`, and the
     player form `HeadingAssist_Player` before the physics, as on the race frame).
   - FUN_00470c70: the race frame's physics block for every car: AI cars' A4 pre-phase from their ctrl blocks, then
     `VehiclePhysics_StepCarsOuter` (fallback `StepPlayer` / `StepCar`), write-back of position / velocity / yaw / speed.
     Player input = the cooked accel byte leg 1's hold loop already uses (`player_accel_byte`), brake 0, steer 0.
     No off-mesh recovery on this frame (a step that lands off the mesh keeps the old position).
   - FUN_00422ba0: `Ai::Ai_ContactEventDrain()` under its existing gate, as the race frame.
   - FUN_004252c0: not ported (no exe caller on the race path either, AiStandalone.cpp:2247). Named, not run.
3. Then the rest of the block unchanged (`SubStateCameraChain(false)`, the clock tails, `CarParts_FrameTick` ...).

Log only (knob ON): a `[st5leg2]` line in mashed_re.log on the first and the last leg-2 call of the window with the call
count and car 0's record +0x958/+0x95c/+0x960 before the first physics step and after the last one, plus car_pos_.

## Arms

Exe from this branch, `MASHED_NO_DEPLOY=1`, private root `D:\MashedWT\D4-DC-77b0-STATE5-LEG2_root`, under
`mashed_lock.py run`. Runner `verify/savelink_20261023/run_arms.py` with one new stack `ST5L2 = ['MASHED_ST5_LEG2=1']`.

- G0 (default gates): `H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`. Must equal `merge_train.json` gctrl.ref / b.ref
  (G-CTRL 8284be46/ba407a16/31fe043e, B 1d5bf4f9 x3). Not equal = RED, stop.
- K1 (knob ON): `H_ST5L2 B_ST5L2_K0 B_ST5L2_K1 B_ST5L2_K26`. Hashes reported, no target.

## Witnesses and predictions

- W1 (count): the knob-ON log shows leg 2 on every case-5 call of the window, i.e. the count equals leg 1's count for the
  window (112 on the original, both tracks). Prediction: equal.
- W2 (car 0 stays put): car 0's record +0x958/+0x95c/+0x960 after the last leg-2 physics step equals its value before the
  first one, bit for bit (the original: constant over frames 2..848). Prediction: [UNCERTAIN] the exe's physics on a held
  car is not measured. Either result is reported as measured.
- W3 (gates move): K1 hashes differ from G0 on at least the G-CTRL step file. Prediction: they differ (the second AI tick
  writes ctrl and AI state the race reads after GO).

## Verdict rules

- G0 differs from the refs: RED.
- G0 equal, W1 equal, W2 holds, no crash: DECISION row recommending the flip (it moves gate hashes, so never GREEN).
- G0 equal, but W1 or W2 fails or a knob-ON arm crashes: DECISION row recommending no flip, with the measured failure
  and the next step named.
