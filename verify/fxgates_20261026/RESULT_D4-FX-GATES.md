# RESULT D4-FX-GATES: the original's skid-emission gates, `MASHED_FX_ORIG_GATES` (2026-10-26)

**Verdict: DECISION.** The original's gates are derived and cited, ported behind `MASHED_FX_ORIG_GATES` (default OFF),
and knob-OFF gates are exact. With the knob ON the G-CTRL hashes stay the same (the knob does not touch the simulation).
On the G-CTRL race the knob's per-active-frame rate is closer to the original's than the scaffold's on all three AI
slots, but it stays outside 2x on slots 1 and 2. P3 holds, P4 and P5 are refuted (section 4). The flip is filed as a
decision. Branch `orch/D4-FX-GATES`, exe `mashed_re.exe` sha8 71354344 (code commit 24725c2ba, PREREG 3450f6060
committed alone first).

## 1. The original has no skid emitter. Its wheel effects come from one chain

`FUN_0040ca00` (0x0040ca00..0x0040cd8b, once per race frame from `FUN_0040ce00`) -> `FUN_0040c690`
(0x0040c690..0x0040c9f9, roll and effect id keyed on the wheel colour) -> `FUN_0048cf90` (one record into a 256-slot
ring per id). Every gate and constant, with its instruction, is in `PREREG_fxgates.md` section 1 and the source comment
above `TrackRenderer::EmitCarFxOrigGates`. In summary:
- per car: slot word `[0x007f1a14+v*0x10] >= 0` (0x0040ca31), `(int)rec+0x9e0 > 1` (0x0040ca74),
  `|rec+0x9b0|^2 >= 62500.0` [0x005ccd20] (0x0040cae0) when `rec+0xcfc == 0`;
- skid flag: `rec+0xb0c > 100.0` [0x005cc568] (0x0040caf5), or `FUN_0046d570` range `< 90.0` [0x005ccad0] with ctrl
  byte `[idx*0x4c+0x007f103d] > 0x40` (0x0040cb21, 0x0040cb37). `rec+0xcfc != 0`: skid when `rand >= 0.3`
  [0x005cc99c], else no call (0x0040cb4e);
- one spawner call per gated frame (wheel 0 or 2 by `[0x007f101c]` parity), three when skid (0x0040cbd5);
- roll threshold by colour `rec+0x1f0+w*0xc4` (0x0040c6e6..0x0040c734): `0xff968080` on track 0x1e 0.3 / 0.1,
  `0xffc88080` 0.1 / 0.05, other 0.7 / 0.4 (skid / not); emit iff `roll <= p` (0x0040c73e). Id 6 (`0xff968080`)
  also needs skid or the launch counter `[0x007f0a04+v*0xc]` (0x0040c8e8), which `FUN_004103a0` seeds from rec+0xbf4
  at the countdown end and `FUN_0040ca00` lowers by 0x18 per frame.
- The original's rolls come from the shared PRNG `FUN_00472650` (0x0040c6a2, 0x0040cb49). So the original's FX
  draws from the same random stream as gameplay. The knob uses a private generator and cannot move any sim value.

Measured on the original (capture below): `[0x00636adc]` = 0 (the 0x0040c6bd early-out never fires), `FUN_00426c00`
= 30 = 0x1e on Training, and every `FUN_0048cf90` call in the run came from `FUN_0040c690` (2885 of 2885). Training's
wheel colours: `0xffc88080` 17663, `0xffb48080` 2804, `0xffff0080` 108, `0xff008000` 50 calls. Effect ids emitted:
9 (`0xffb48080`), 10 (`0xffc88080`), 11 (default). No id 6.

**PREREG correction:** section 2 assumed the port's +0x1f0 key is 0 (`VehiclePhysicsRun.cpp:282`). It is not: F1's
non-skid calls emit at 715 / 14161 = 0.0505, the `0xffc88080` row (0.05). So `MASHED_FX_ORIG_SURFACE` never engaged
and F2 is byte-identical to F1 (sha8 331976e8). [UNCERTAIN] which writer fills +0x1f0 in the port, and whether
the port's per-wheel colour mix matches the original's. The port has no colour histogram yet (the original's
`0xffb48080` share is 14% of calls).

## 2. Port gates (scaffold) vs the original

| | scaffold (`EmitCarFx`, default) | original |
|---|---|---|
| trigger | yaw-rate x speed > `R*0.12` or lateral slip > `R*0.02`, speed > `R*0.04` (world units from the position step) | grounded wheels > 1, `|v|^2 >= 62500` internal, per-call Bernoulli by wheel colour |
| skid notion | the same trigger | `+0xb0c > 100`, or brake > 0x40 while range < 90, or both pedals (`+0xcfc`) with p 0.7 |
| rate | 36 x min(skidI,3) per s, cap 4 per frame | 1 call per gated frame (3 when skid), p 0.05..0.7 per call |

## 3. Gates (knob OFF, this exe, `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` under `mashed_lock.py`)

G-CTRL 5943c5f1 / b493262b / 09551e6a, B 3a03670c / 2516d903 / 53778664: all six equal `merge_train.json` refs.
The knob-ON arms F1 / F2 (and F0) carry the same three G-CTRL hashes (`fx_arms1.txt`).

## 4. Counts (`fx_score.txt`). Original: `scenario_launch.py --track 0 --mode 10 --cars 4 --car 1 --rule 0`, 13600 frames

Port: G-CTRL recipe (Training, rule 0, 14400 frames). Active frames = spawner calls[non-skid] + calls[skid]/3, the
frames that passed the outer gates. Rounds eliminate cars, so per-active-frame is the fair rate (the original's cars
0, 2, 3 stopped after round 1 at about 3100 frames, port slots 0 and 3 had 407 and 232 active frames).

| slot | original emits / per 1000 active | scaffold F0 skids / per 1000 active | knob F1 emits / per 1000 active | skid share of calls, orig / F1 |
|---|---|---|---|---|
| 1 (AI) | 1444 / 166.1 | 58 / 4.1 | 744 / 52.4 | 35% / 0.9% |
| 2 (AI) | 405 / 215.3 | 150 / 10.6 | 698 / 49.1 | 47% / 0.4% |
| 3 (AI) | 431 / 191.0 | 162 / 698.3 | 50 / 215.5 | 48% / 65% |
| 0 (player) | 605 / 266.9 | 19 / 46.7 | 24 / 59.0 | 43% / 0% |

Predictions (registered metric = emits per 1000 total frames, AI slots 1..3):
- P1 GREEN (section 3).
- P2 not separately measured. The knob-OFF path differs only by the `if (s_origGates) ... else if` wrap, and the debug
  line gained fields, so the fx_debug hash cannot be compared with a base-exe run.
- P3 holds: original emit per call 0.127 / 0.147 / 0.130 for cars 1..3, all below 0.4.
- P4 refuted: F1 / original = 0.49 / 1.63 / 0.11 (51.7 vs 106.2, 48.5 vs 29.8, 3.5 vs 31.7). None is above 2x.
- P5 refuted: F2 = F1 (the stand-in never engaged), so 0.49 / 1.63 / 0.11. Slot 2 is inside [0.5, 2], slots 1 and 3 are outside.

Exploratory, not in the PREREG (`fx_score_r4.txt`, rule 4 = the 2026-10-16 recipe): scaffold 0 / 256 / 120 / 269,
knob 0 / 165 / 2855 / 183. The 5139 / 5489 residual from `RESULT_FX.md` does not reproduce on today's default in
either rule. It was measured on base 017fc698. The "241 / 378 original" figure in the step brief is the 2026-08-15
STANDALONE baseline, not an original capture (`RESULT_FX.md:9-10`). This capture is the first original-side skid count.

## 5. What is left, and where it lives

The knob reproduces the original's gate law. The remaining gap comes from its inputs, not its thresholds:
1. skid-flag frequency. On AI slots 1 / 2 the original sets the flag on 35-47% of spawner calls, the port on 0.4-0.9%.
   The flag reads `rec+0xb0c` (slide measure) and the ctrl block brake byte. Both come from AI and physics state
   (D2/D3), not from FX.
2. wheel colour mix. The original rolls 14% of calls on `0xffb48080` (p 0.4 / 0.7). The port's mix is unmeasured
   ([UNCERTAIN] above).
3. not modelled: the launch counter `[0x007f0a04]` (only gates id 6, which Training never emits), `FUN_0048cf90`'s
   per-id ring cap, the shared-PRNG draws.

Decision filed (ledger D4-FX-GATES): flip `MASHED_FX_ORIG_GATES` to default ON or keep it OFF.

## Raw data (D:/MashedWT/D4-FX-GATES_data, not committed), SHA-256

```
b6e2beb69dc6d85c247b648f7e03c178befce8b10cc5e9d4362b2ee0679e83cc orig_cap4.json   (original capture, scored)
39e78b043abe0798c605be2a386ba5aeed2747475297aa81d1a6a66ddc7f669f orig_cap4.txt
be1c0c65830e7d3f195f2c3c59c802aec8a4ac0676dcd9199b76dfff31b79bb5 orig_cap3.json   (calls + colours valid, emits 0: attribution bug)
cd6da81d361bdcc855f8ecb092734953936d84bcf78cb22db598950bc7f59390 arms/F0.fx_debug.txt
331976e81b588b387576ebe2da34e9c27e3a07896e7168cc4b0441630df9632c arms/F1.fx_debug.txt
331976e81b588b387576ebe2da34e9c27e3a07896e7168cc4b0441630df9632c arms/F2.fx_debug.txt
c0deacbba1bc686ceb6dba43caf2fa7153bd5726b61b480cba5c8718e32f5f71 arms/F0R4.fx_debug.txt
1306c9ffde785e81bcc9da72750f9d7c081daed151b56591ad0d81b625ef2a67 arms/F1R4.fx_debug.txt
5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 arms/H_DEF.step.csv
b493262bed8ea6ecad4399f0ebaa53b902965423e74bfa07359188559fff479f arms/H_DEF.gates.csv
09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1 arms/H_DEF.ptrace.log
3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f arms/B_DEF_K0.step.csv
2516d9034a1774893bf2f59edd368c3783ad6cfe0ed491dc68b3d84a0afec7f6 arms/B_DEF_K1.step.csv
5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d arms/B_DEF_K26.step.csv
47b0194bb136530094f3c0d255e9acbde8c9e73bf8403f1b679e24eb72039b07 gates_run1.txt
03a2253e1aeb578e1161f33271b947aeb29e9555b1120ee5aa9b25df00c3dc13 fx_arms1.txt
```

Capture-harness note (`orig_fx_gates_count.py`): CModule static variables are not writable (runs 1-2 hit an AV inside the
CModule, so the game exited at 4 s), and `gum_invocation_context_get_return_address` in a probe callback did not give
the caller (run 3: 0 emits matched). The fixed harness keeps its state in allocated memory and reads `[esp]`.
