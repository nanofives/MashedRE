# PREREG D4-DC-77b0-ST5-C4ACCEL (2026-11-02)

Decision L-81 (b) follow-up. `MASHED_ST5_LEG2` stays default OFF. This step finds why the exe AI writes c4 (accel)
0xff / 0x40 from sub-state-5 call 1 once leg 2 runs, fixes it behind the same knob, and re-measures the GO start boost.

## Static finding (MASHED.exe.unpatched, capstone)

FUN_00418560 (per-car AI step, called per car by FUN_00418860):

- 0x0041857b..0x0041858b zero ctrl bytes 4, 5, 0, 1, 7, 6 of the block 0x007f1038 + slot * 0x4c.
- 0x0041858e `call 0x0040e350` (= `mov eax,[0x0063ba8c]; ret`), 0x00418593 `cmp eax,5`, 0x00418596 `jne 0x004185e5`.
- Mode 5: 0x00418598 `mov [0x007f0ff8],0`, 0x0041859e `call 0x00413fe0`, 0x004185a3 `call 0x0040e4a0`
  (= `mov eax,[0x005f29b8]; ret`), 0x004185ab cd = `[ebp*0x74 + 0x0089a4f8]`.
  - cd >= 0: 0x004185b5 `add ecx,0x40`, 0x004185b8 `imul ecx,ecx,0x64`, 0x004185bb `cmp eax,ecx`,
    0x004185bd `jge 0x00418841`, else 0x004185c3 `mov byte [edi+4],0xff`, return.
  - cd < 0: 0x004185cb..0x004185d2 `(0x4a - cd) * 0x64`, 0x004185d7 `jge 0x00418841`, else 0x004185dd c4 = 0xff.
  - 0x00418841 is `pop edi / pop ebp / pop ebx / ret`: no control step (0x00417180 / 0x00416250 / 0x00417640) and no
    replay-slot store.

So on the original, every AI tick while DAT_0063ba8c == 5 (leg 1's FUN_00418860 at 0x0041040e and leg 2's at
0x00425a73) leaves c4 = 0 until 0x005f29b8 drops under the per-car threshold, then 0xff.

The exe's `Ai::VehicleStep` (Ai/AiStandalone.cpp) has this branch, but only behind `MASHED_MODE5_RESET` (default OFF,
set by no gate arm). On every exe state-5 AI tick it ran `AiBankSwitch` + `ControlStep` (FUN_00416250, whose accel
writes are 0xff at 0x004167dc and 0x40 at 0x00416896), which is the 0xff / 0x40 the ST5-HOLD diag logged from call 1.
The branch's old DEVIATION note ("0x005f29b8 is never written in the standalone, elapsed reads 0") is stale: the
sub-state port now writes it (phase-3 decrement, 0x00410371 store of 12000, 0x004103e3 decrement in
SubStateHeldCountdown5_C, 0x004104c0 zero).

## Change (knob ON only)

`VehicleStep` takes the mode-5 branch when `MASHED_MODE5_RESET` is set **or** `MASHED_ST5_LEG2` is on (same parse as
`St5Leg2Enabled`: set, non-empty, not starting with '0'). Knob unset: no code path changes. `MASHED_ST5_HOLD_DIAG`
(log only) appends `cd=` (0x0089a4f8 + v*0x74), `c29b8=` (0x005f29b8) and `bf4=` (record +0xbf4) to each `[st5hold]` line.

## Arms (private root D:\MashedWT\D4-DC-77b0-ST5-C4ACCEL_root, `mashed_lock.py run`)

`verify/savelink_20261023/run_arms.py`: H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_ST5L2 B_ST5L2_K0 B_ST5L2_K1 B_ST5L2_K26
B_ST5L2D_K0, then B_RD1_K0 B_ST5L2RD1_K0 B_RD3_K0 B_ST5L2RD3_K0.

## Witnesses (decided before any arm)

- **G0 (gate).** Knob unset: G-CTRL 8284be46 / ba407a16 / 31fe043e and B 1d5bf4f9 x3 (merge_train.json refs).
  Any change = RED for the step.
- **W-C4 (the fix).** B_ST5L2D_K0: on every non-exit `[st5hold]` line (cars 1..3), byte 4 of `blk` equals
  `0xff` if c29b8 < T(cd) else `0x00`, with T(cd) = (cd + 0x40) * 100 for cd >= 0 and (0x4a - cd) * 100 for cd < 0.
  0 mismatches required. ST5-HOLD's arm had 0xff / 0x40 on all 111 non-exit calls; this witness fails if any
  0x40 remains.
- **W-HOLD (no regression of ST5-HOLD).** B_ST5L2D_K0: each AI car's position identical on calls 2..112, and equal to
  ST5-HOLD's held x/z (car 1 -1.10121727 / -0.901938975, car 2 1.1367228 / 0.20244953, car 3 -0.465431273 /
  1.29945505). The hold runs on the neutral block 0x007f19b8, so c4 must not move it.
- **W-GO (the measure, scored against the original).** Frame-0 +0xbf4 / +0xbf8 from the RD dumps:
  original = default exe = car 1 1100 / 1, car 3 1100 / 1 (ST5-HOLD RESULT). ST5-HOLD knob ON was car 1 2600 / 2,
  car 3 0 / 0. GREEN = both cars equal 1100 / 1 knob ON.
- **Prediction for W-GO, stated now.** Leg 1 reads c4 into FUN_0046d7f0 after its own AI tick, +50 per read with
  c4 > 160 else -50 (clamp 0..3000, 0x0046d845..0x0046d874). With 50 units per call, s29b8 = S - 50k on call k and
  the threshold crossing gives about 2 * (T(cd) - (S - 5600)) / 100 reads of 0xff. If S = 12000 (the 0x00410371
  store carried into state 5) that is 2 * cd reads: with the exe draw cd = 3,1,4,2 (v = 0..3, `[substate] FUN_00414180`
  line of the ST5-HOLD arms) car 1 charge 100, car 3 charge 200, so W-GO would **not** reach 1100 / 1 on both cars.
  The diag's c29b8 / cd columns decide which.
- [UNCERTAIN] The original's cd values for this recipe. In all three original captures (w_c1/w_c2/w_c3, ST5-HOLD data)
  the three AI cars' c4 turn 0xff on the same frame (819 / 813 / 887), which needs equal T(cd) across cars, while the
  exe draws distinct cd. Missing evidence: the original's 0x0089a4f8 + v*0x74 (v = 1..3) and 0x005f29b8 at state-5
  entry in the same recipe. If W-GO is RED because of cd, that read is the next step, not a flip.

## Outcome rule

W-C4 + W-HOLD + G0 green and W-GO green: file the flip as an L-n decision with the knob-ON hashes as expected refs.
W-GO red: no flip filed, RESULT names the remaining difference (cd draw or s29b8 start) with numbers.

## Amendment 1 (2026-11-06, after the main arms, before any amendment arm)

Main arms (exe 34530fab): G0 equal, W-C4 0 mismatches, W-HOLD 0 mismatches, W-GO red (knob ON frame 0: car 1
700 / 1, car 3 800 / 1). The diag gives cd = 1, 4, 2 (cars 1..3) and charges at release 100 / 400 / 200.

New original evidence (two boots, `scenario_launch.py --track 0 --mode 10 --cars 4 --rule 0 --poke-ctrl-slots --hold 30
--statediff-car 1|3 --statediff-aictrl --peek 0089a4f8:i,0089a56c:i,0089a5e0:i,0089a654:i,0089a360:f,005f29b8:i,0063ba8c:i`):
cd = 3 for v = 0..3 on every peek of both boots, DAT_0089a360 = 2.5 (row 2 of 0x005f2d80 = 1..5). So the exe's
3,1,4,2 is a ring phase difference: FUN_00414180 is the exe's first FUN_00534870 consumer (`draws_before=0`), and an
offline copy of the port ring (FUN_00534990 seed, 310 discards) gives 3,1,4,2 at skip 0 and 3,3,3,3 first at skip 8.

Diagnostic arms (not a fix, `MASHED_RING_SKIP` shifts every later ring consumer): B_ST5L2RS8D_K0, B_ST5L2RS8RD1_K0,
B_ST5L2RS8RD3_K0, B_RS8RD1_K0 (stacks = the existing ones + `MASHED_RING_SKIP=8`).

Predictions, stated now:
- A1-CD: the `[substate] FUN_00414180` line reads cd=3,3,3,3 in all four arms.
- A1-C4: score_c4.py on B_ST5L2RS8D_K0 exits 0, each AI car with 5 non-exit 0xff calls and bf4 = 1300 on the exit line
  (charge 300 = 5 held reads + the exit-call read, release +1000 at 0x0046d7c7).
- A1-GO: knob ON frame 0 = 900 / 1 on cars 1 and 3 (1300, less one 200 step on the exit call's leg 2 and one on race
  frame 0). That is the original's value one frame after its release frame (876 = 1100, 877 = 900, p_c1). The default
  with skip 8 stays 1100 / 1 at frame 0 (RD frame 0 is the first race frame, and the default runs no physics on the
  exit call).
- Reading: if A1-GO holds, the knob-ON exe and the original agree frame for frame once cd matches, and W-GO's 1100 / 1
  bar (taken from the default) was one frame off for a knob that adds the exit call's physics. W-GO itself stays red as
  preregistered. No flip is filed from this step either way: the cd draw needs a ring-phase decision first.
