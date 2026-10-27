# RESULT D4-FX-INPUTS: the kill's damage reaches the record, `MASHED_KILL_DAMAGE` (2026-10-27, attempt 3)

**Verdict: GREEN on the pre-registered rule** (`PREREG_killdamage.md`, 8ed732807, committed alone before the arms).
Attempt 2's unexplained 2x decel of a killed car is found and ported. The original's kill `FUN_00422fd0` also pushes
50.0 into the per-car damage table. `FUN_00420e00` copies that table to rec+0x2c / +0x30 every frame, and A5 scales a
dead car's wheel friction by `+0x2c * 0.01`. The port never ran that copy, so its dead cars braked on 0.25 instead of
0.125. With `MASHED_KILL_DAMAGE=1` (default OFF, only together with `MASHED_DEAD_COAST=1`) the port's matched-band
decel is 5.1095 / 5.1181 per frame against the original's 5.1184. Knobs OFF leave every gate hash unchanged. The two
knobs ON move two G-CTRL hashes (section 5).

Slot 2 brake-byte pairing (attempt 1's open lead): the AI tick's brake byte for slots 2 and 3 is rewritten before
the FX gate reads it, on 32 / 215 and 18 / 231 alive gate frames (section 4). The writer is not found yet.

Attempt 1: b122ca5cf. Attempt 2: c9011679a (`PREREG_deadcoast.md`).

## 1. The cause (decomp via `re/tools/decomp_pc.py`, pool slot 10, read-only, plus existing captures)

Matched band (`decel_band.py`, |v| 25..70, consecutive dead frames):

| capture | median decel / frame | n | wheel+0x54 (4 wheels) | rec+0x2c / +0x30 |
|---|---|---|---|---|
| original ww3_c2 | 5.1184 (5.1062..5.1322) | 8 | 0.125 | 50 / 50 |
| original o60_c2 | 5.4170 (5.0804..6.8009) | 8 | 0.125 | 50 / 50 |
| port X1 (attempt 2, DEAD_COAST only) | 10.2172 | 4 | 0.25 | 0 / 0 |
| port X2 (attempt 2, DEAD_COAST only) | 10.2208 | 4 | 0.25 | 0 / 0 |

So the 2x appears at matched speed and does not come from the kill speed.

- Friction term: A6a `FUN_00467650` (original code, the port calls it at 0x00470936) uses
  `piVar12[0x15] * piVar12[0x1b] * _DAT_0088e5f0 * local_e4 * _DAT_005cea0c`, with piVar12 = rec+0x1a4 + w*0xc4, so
  piVar12[0x15] = rec+0x1f8 + w*0xc4.
- Its writer: A5 `FUN_0046ddb0` (original code, called at 0x00470923). The base is `*pfVar14 = 0.25` (pfVar14 =
  rec+0x1f4 + w*0xc4). Then `if (+0x2c != 0) *pfVar14 = +0x2c * *pfVar14 * _DAT_005cc328` and
  `pfVar14[1] = _DAT_005cc318 * v`, replaced by `pfVar14[1] = v` when `rec+0x10 == 1`. Live cars read 0.15 on both
  sides. A dead car reads 0.25 with +0x2c = 0 and 0.125 with +0x2c = 50.
- Writer of rec+0x2c: the only displacement 0x008815cc (rec base 0x008815a0) in the image is 0x0046bee0, case 4 of
  the setter `FUN_0046be50(v, case, value, paired)` (value capped at 100; case 4 = +0x2c / +0x44, case 5 = +0x30 /
  +0x48). Its E8 callers are 0x00420e33 (case 5) and 0x00420f40 (case 4), both in `FUN_00420e00`, called from
  `FUN_004215a0` (4-entry loop, table 0x0063e4b8, stride 0x24). `FUN_00420e00` writes `+0x30 = ftol(t[0])` and
  `+0x2c = ftol(t[1])`. `FUN_00422fd0` calls `FUN_004215c0(v, 50.0f, 0)` / `(v, 50.0f, 1)` (0x42480000) and so
  `FUN_00420de0` gives `t[k] = min(t[k] + 50.0, 50.0)`.
- Per frame (.msd): rec+0x2c / +0x30 go 0 -> 50 on the kill frame on both original captures (1726, 1802) and stay 50.
  The port runs neither `FUN_004215a0` nor `FUN_00420e00` (hooks.csv C2, no port caller).
- Seen, not changed: the original's rec+0x44 / +0x48 read 0xbf800000 from frame 2 (alive and dead). The port's read 0.
  [UNCERTAIN] their reader. It is a live-frame difference, so porting it moves default hashes.

## 2. The port change (default OFF)

`VehiclePhysicsRun.cpp`: `KillDamageEnabled()` (`MASHED_KILL_DAMAGE=1`). `VehiclePhysics_MarkKilled` also writes
+0x2c = 50 and +0x30 = 50. `VehiclePhysics_ClearKilled` writes them back to 0. Both functions only run under
`MASHED_DEAD_COAST=1`. Not ported: the smoke side of `FUN_00420e00` and the +0x44 / +0x48 words. [UNCERTAIN] the
original's reset (`FUN_00420d00` clears the table) on a second round: the G-CTRL recipe plays one round.

`TrackRenderer.cpp`, diagnostic only: `MASHED_FX_PAIRLOG=<path>` (section 4). It reads ctrl byte 5 at three points
and writes a CSV. No sim state. Its Z0 arm reproduces the default G-CTRL triple.

## 3. Measurements (exe 5055fc4b, G-CTRL recipe, arms5)

**M1, matched-band decel** (`decel_killdamage_Y.txt`):

| arm | slot | median / frame | n | wheel+0x54 | +0x2c / +0x30 | PREREG band [4.86, 5.69] |
|---|---|---|---|---|---|---|
| Y1 | 1 | 5.1095 (5.1075..5.1130) | 9 | 0.125 x4 | 50 / 50 | PASS |
| Y2 | 2 | 5.1181 (5.1084..5.1320) | 9 | 0.125 x4 | 50 / 50 | PASS |

**M1b, reported:** kill |v| 2020.2 / 2513.6, 223 / 241 frames to exactly 0.0, frozen-nonzero 0 / 1, FX speed-gate
frames 178 / 196 of 13,998. As predicted, it stops later than attempt 2 (133 / 149) because the decel per frame halves.
The original's kills in this recipe come at |v| 175 to 298, so the stop time is not compared.

**M2, FX emits per 1000 active frames, all frames** (`score_killdamage_Y.txt`; Y1 and Y2 fx_debug are byte-identical):

| slot | attempt 2 X1/X2 | Y1/Y2 | original | within 2x |
|---|---|---|---|---|
| 1 | 113.1 | 106.6 | 166.1 | yes (0.64x) |
| 2 | 52.0 | 55.3 | 215.3 | no (0.26x), as pre-registered |
| 3 | 215.5 | 215.5 | 191.0 | yes |

The alive-only rates are unchanged: 157.4 / 59.1 / 215.5.

## 4. Slot 2: the brake byte the FX gate reads is not the one the AI tick wrote

Arm Z0 = knobs OFF + `MASHED_FX_PAIRLOG`. Each FX gate call that passes the speed gate logs ctrl byte 5 at four
points. `c5_step` is read right after the AI tick, by `AiStepDump`, which runs inside `ai_tick_phase`. Its default
site is `TrackRenderer.cpp` `if (!ai_tick_ran) ai_tick_phase();`, after UpdateRace. `c5_pre_race` is read before
UpdateRace. `c5_car_end` is read at the end of UpdateCar. `c5_fx` is the FX gate's own read (0x0040cb21 / 0x0040cb37
model). Alive rows only (`pair_score_Z0.txt`, `pair_probe_Z0.txt`):

| v | rows | step >0x40 | fx >0x40 | step == fx | pre_race == car_end | car_end == fx | step.csv c5 agrees |
|---|---|---|---|---|---|---|---|
| 1 | 203 | 31 | 31 | 203 | 203 | 203 | 203 / 203 |
| 2 | 215 | 36 | 4 | 183 | 215 | 215 | 215 / 215 |
| 3 | 231 | 65 | 83 | 213 | 231 | 231 | 231 / 231 |

- The mismatches come in runs (v=2 step frames 23..28 and onward, v=3 frames 209..214). A frame offset does not explain
  them: offsets -2..+2 give 179 / 181 / 183 / 180 / 177 matches for v=2. A cross-slot read does not either: each
  slot's FX byte matches its own step byte best, and each slot keeps one block address (0x7f1084 / 0x7f10d0 / 0x7f111c).
- On every row, byte 5 at UpdateCar's end equals byte 5 before this frame's AI tick. So whatever the AI tick writes to
  byte 5 of blocks 2 and 3 is gone again by the end of the frame. The FX gate reads that restored byte. For slot 2 that
  hides 32 of its 36 brake frames, which is consistent with its 0.26x rate.
- [UNCERTAIN] the writer that restores byte 5 between `ai_tick_phase` and UpdateCar's end. Checked and excluded: the
  exe's ctrl-block writers (`AiControlStep.cpp`, `AiPostStepBrake.cpp`, `VehicleStep` / `AiVehicleStep`) all run inside
  the AI tick. `AiControllerAB.cpp`'s roll-back is the .asi A/B harness only. HeadingAssist, the A4 pre-phase and the
  powerup input read only. No block-sized memcpy touches 0x007f1038. Missing evidence: a port-side write-watch on
  0x007f10d5 (block 2 + 5) across one mismatch run, or more probes inside the opponent loop.
- [UNCERTAIN] which of the two values the original's FX gate sees at `FUN_0040ca00`. That needs the same pairing on
  the original: the aistep capture's c5 against a read at 0x0040cb37.

## 5. Gates

Knobs OFF, `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, final exe 5055fc4b: G-CTRL `5943c5f1 / b493262b /
09551e6a`, B `3a03670c / e85ea6fb / 53778664`. Both equal `re/orchestrator/merge_train.json`. The same holds on the
intermediate exes 9828b461 (arms3) and f2ea4544 (arms4). Z0 (pair log on) gives the same G-CTRL triple.

`DEAD_COAST + KILL_DAMAGE` ON (stack `DCKD`, report only): G-CTRL `5943c5f1 / 364350ae / 52052890`, B `3a03670c /
e85ea6fb / 53778664`. Y1 and Y2 give the same G-CTRL triple. The gates file equals attempt 2's DEAD_COAST-only value.
The ptrace moves from attempt 2's `000a8b2e` to `52052890`, because the player, killed at frame 220, now also coasts on
the halved friction.

Decision for the lead: flip `MASHED_DEAD_COAST` and `MASHED_KILL_DAMAGE` together. New refs would be G-CTRL
`5943c5f1 / 364350ae / 52052890`, B unchanged. For: the dead-car mechanism now matches the original at matched speed
(5.11 vs 5.12 per frame, exact 0.0 stop, 0 to 1 frozen frames). Against: the round-restart reset of +0x4 / +0x10 /
+0x2c / +0x30 has not been measured on a second round. Recommendation: flip both together (DEAD_COAST alone leaves the
measured 2x decel in place). No default changed here.

## 6. Next

1. Slot 2: a port-side write-watch on 0x007f10d5 across a mismatch run (Z0 recipe, step frames 23..28) to name the
   writer that restores byte 5. Then the original-side pairing at 0x0040cb37.
2. rec+0x44 / +0x48 = 0xbf800000 on the original from frame 2 (section 1): find the reader before porting.
3. Round-restart reset of the kill words, once a port recipe plays a second round.

## Raw data (D:/MashedWT/D4-FX-INPUTS_data, not committed), SHA-256

```
5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 arms5/H_DEF.step.csv
b493262bed8ea6ecad4399f0ebaa53b902965423e74bfa07359188559fff479f arms5/H_DEF.gates.csv
09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1 arms5/H_DEF.ptrace.log
3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f arms5/B_DEF_K0.step.csv
e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8 arms5/B_DEF_K1.step.csv
5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d arms5/B_DEF_K26.step.csv
364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4 arms5/H_DCKD.gates.csv
520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757 arms5/H_DCKD.ptrace.log
0b9457dc238fa3b98b55390d443b9e8dfcc93bb820de010bd3d1d585b0038300 arms5/H_Y1.msd
dd7593852e7c4d9d171729577fbbc7038d4302fabbc1ecfd298a5cd1452fbd8a arms5/H_Y2.msd
7476bd32e1c8ff6f8c3cbf0136f291a35375fee619e4ee227c3f259d0a88c609 arms5/H_Y1.fx_debug.txt
7476bd32e1c8ff6f8c3cbf0136f291a35375fee619e4ee227c3f259d0a88c609 arms5/H_Y2.fx_debug.txt
5e15b80ce7abd2edc7037766bf5b28e88e6ac49d990af21bdc1296e4dd62e2dd arms5/H_Z0.pair.csv
5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 arms5/H_Z0.step.csv
5c6fea64974e3a69b7f1c170c6ea3cca4a11ca0d30c7e8903eef03116ea7050c ww3_c2.msd
19bde9957000bbf709133d06e3d14ee0675fc47b95f6a2278928939fbe261bdd o60_c2.msd
3601b20ab793ac1b0363a15d2bcb3a94e7d8ce9f0d29fa9a28f00486bf4f3f6a arms2/H_X1.msd
f2668b7bfec5c396a2878f106b58ea91debab60f6d1d21c92cde7677d9519a2d arms2/H_X2.msd
5055fc4b515bd23819edfc7603a3e20a57cd5bf70ab139437328a61a1ff6cac9 mashedmod/build/mashed_re.exe
```
Logs: `build4.log`..`build6.log`, `arms3_run.txt`..`arms5_run.txt`, `arms3/RUNLOG.txt`..`arms5/RUNLOG.txt`. Every build
and game run went through `mashed_lock.py run`, and only the spawned PIDs were touched. arms3..arms5 ran before the
2026-10-26 test-slot rule broadcast arrived (arms5 was mid-run), so they are not wrapped in `proyectos_test_slot.py`.
