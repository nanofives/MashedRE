# RESULT D4-FX-INPUTS: the skid-flag inputs of FUN_0040ca00 (2026-10-26)

**Verdict: RED.** The skid-flag inputs (`rec+0xb0c` and the ctrl brake byte) are not what separates the port from the
original. The 0.4-0.9% skid share on port slots 1 and 2 comes from the denominator. Those two cars are eliminated at
6.7 s in the G-CTRL race, and for the remaining ~14,000 frames their physics record stays frozen at racing speed. So
they keep passing the outer speed gate with the brake byte at 0. Counting racing frames only (`g_aib.alive`), the port
gives 157.4 / 59.1 / 215.5 emits per 1000 active frames on slots 1 / 2 / 3, against the original's 166.1 / 215.3 / 191.0.
Slots 1 and 3 are within 2x, slot 2 is not (0.27x, on 220 frames). No producer was ported: I did not find the original
writer that holds a killed car's velocity below the gate. No default behaviour changed.

Branch `orch/D4-FX-INPUTS` off `orch/integration` e11287674. Exe sha8 7c238cc6.

## 1. What the gate reads (decomp, `re/tools/decomp_pc.py`, pool slot 10, read-only)

`FUN_0040ca00`, per car v (cited in full in `verify/fxgates_20261026/PREREG_fxgates.md`):
- outer: slot word `[0x007f1a14 + v*0x10] >= 0`, `(int)rec+0x9e0 > 1` (`FUN_0046cc10`), then for `rec+0xcfc == 0`
  `|rec+0x9b0..+0x9b8|^2 >= [0x005ccd20]` (62500.0)
- skid flag, leg A: `FUN_0046d6a0` (rec+0xb0c) `> [0x005cc568]` (100.0)
- skid flag, leg B: `FUN_0046d570 != 0 && range < [0x005ccad0]` (90.0) `&& [0x007f103c + slot*0x4c + 1] > 0x40`.
  `FUN_0046d570` (0x0046d570..0x0046d653) = `acos(clamp(norm(rec+0x9b0) . rec+0x9d4, [0x005cc33c], [0x005cc320])) *
  [0x005ccae0]`, so `range < 90` is `dot(vel, rec+0x9d4) > 0`.

The port's `EmitCarFxOrigGates` reads the same fields at the same offsets (TrackRenderer.cpp, unchanged logic).

## 2. Measurements

Port: `verify/fxgates_20261026/fx_arms.py F1` (G-CTRL recipe + `MASHED_FX_DEBUG`), new census `ig=` on the debug line
(diagnostic counters only, `fx_ig_`). Original: `orig_fx_inputs.py` (this folder), the same scenario_launch recipe as
the D4-FX-GATES capture (Training, mode 10, rule 0, 4 cars, car 1), CModule probes on `FUN_0040ca00` and the first
`FUN_0040c690` call per car-frame. Full tables: `score_port_F1.txt`, `score_orig1.txt`, `score_orig2.txt`.

**Port, F1 (14,389 frames)**

| slot | active frames | of which frozen `+0x9b0` (bit-identical to last call) | alive active | skid share, alive frames | emits / 1000 active, alive frames | all frames (D4-FX-GATES metric) |
|---|---|---|---|---|---|---|
| 0 | 407 | 20 | 198 | 0.0% | 65.7 | 59.0 |
| 1 | 14,205 | 13,989 (98.6%) | 216 | 20.4% | 157.4 | 52.4 |
| 2 | 14,209 | 14,017 (98.7%) | 220 | 7.7% | 59.1 | 49.1 |
| 3 | 232 | 8 | 232 | 38.4% | 215.5 | 215.5 |

`mashed_re.log` of the same recipe: `ELIM-RESOLVE ... alive=0,0,0,1`, `R6 round 1 OVER t=6.7s winner=car3`. The AI
step dump has v=1 and v=2 rows for frames 0..219 only. Slot 3, the survivor, is slow on 13,977 frames after the round
end (`MASHED_ROUND_END7`, default ON, scales its velocity by 0.85 per frame, the original's `FUN_0046c570`).

**Original, run 2 (12,979 frames, round 1 ends at about frame 3200)**

| car | counted frames | skid share | leg A `b0c>100` | leg B `rng && brk` | `brk > 0x40` | brake byte values |
|---|---|---|---|---|---|---|
| 0 | 2185 | 17.8% | 5.8% | 12.1% | 25.0% | 0 or 0xff only |
| 1 | 7691 | 15.4% | 3.8% | 12.2% | 27.9% | 0 or 0xff only |
| 2 | 2041 | 22.6% | 5.8% | 18.2% | 26.7% | 0 or 0xff only |
| 3 | 2407 | 19.2% | 5.8% | 15.2% | 23.3% | 0 or 0xff only |

Run 1 (2797 frames, the race ended at phase 5) gives the same shape: skid share 13.5-21.7%, leg B 9.3-13.1%, leg A
3.8-9.1%. Leg B (braking while moving forward) carries about two thirds of the original's skid frames. The port's
slot 3 shows the same split (leg B 35.9%, leg A 6.5% of its 231 active frames).

**What a killed car presents to the gate** (original run 2, frames with `rec+0x4 == 0`, the word `FUN_0046c5c0` clears
from the kill path `FUN_00422fd0`):

| car | dead frames | `|v|^2 == 0` | pass the speed gate | slot word < 0 |
|---|---|---|---|---|
| 0 | 2097 | 1885 | 202 | 0 |
| 1 | 996 | 580 | 323 | 0 |
| 2 | 10,247 | 10,140 | 39 | 9776 |
| 3 | 9877 | 0 (frozen non-zero) | 101 | 9776 |

Inside round 1 the dead episodes are bounded: 93-106 frame passing episodes and 106-397 frame zero-velocity episodes
per 10 s window. None runs for the rest of the race. After round 1 the original sets the slot word to -1 for cars 2
and 3 (`slots=[0,1,-1,-1]`), so they leave the gate there even with a frozen velocity.

## 3. The input that differs, and why

The port's eliminations (`TrackRenderer.cpp` elimination at the `race_[victim].alive = false` site and `KillCar`) clear
only `race_[v].alive` and the AI car speed. The ported physics then stops writing that record, so `+0x9b0..+0x9b8`
freezes at its last racing value (98.6% / 98.7% bit-identical frames). The original's kill path `FUN_00422fd0`
(0x00422fd0..0x00423032) runs `FUN_0046c5c0` (rec+0x4 = 0, rec+0xb10 = `DAT_007f1030`), `FUN_0046c790(v, 1)`
(rec+0x10 = 1), `FUN_004215c0` x2 (-> `FUN_00420de0`), `FUN_0045ba00(v, 2)` (`[0x0068d1f0 + v*4] = 2`) and, when
`FUN_0040e470(v) == 1`, `thunk_FUN_00419760(v, 1)`. None of these writes `+0x9b0`. The measured result is that a dead
car's velocity is mostly 0 and its passing episodes last about 100 frames.

[UNCERTAIN] the original writer that zeroes or holds a killed car's `+0x9b0..+0x9b8`, and what ends the about 100 frame
dead episodes. Evidence missing: a hardware write-watch on `rec+0x4` / `rec+0x9b0` across a kill. Two 75 s watches
(`scenario_launch.py --watch-write 4,9b0`, car 2 and car 1) recorded no kill (`rec+0x4` stayed 1, no write from
`FUN_0046c5c0`). The zero-velocity writes they did catch are `FUN_0046baa0` (pc 0x0046bc0c) from the countdown and
race-start loops `FUN_004102f0` (ret 0x00410343) and `FUN_004103a0` (ret 0x0041043f). Both are gated on
`FUN_0046c7b0(v) == 1`, so they are not the dead-car path.

Lead for slot 2, not resolved: the AI step dump has `c5 = 0xff` on 41 of 219 fast v=2 rows (frames 0..219), while the
FX gate sees `brake > 0x40` on 4 of 215 active alive frames. [UNCERTAIN] whether the step dump and the render-time
read see the same frame's block. A per-frame pairing of the two reads would settle it.

## 4. Gates

Instrumentation only (counters on the `MASHED_FX_DEBUG` line, no sim read-back). F1 arm on exe 7c238cc6:
G-CTRL step / gates / ptrace = 5943c5f1 / b493262b / 09551e6a, equal to `merge_train.json`. B not run: no default
behaviour change. The second build (12:5x) overlapped the merge-train SOLO window opened 12:50:06. The build ran outside
the lock (game runs all went through `mashed_lock.py run`, which queued behind that window). Noted as a protocol slip.

## 5. Next

1. Find the dead-car velocity writer: watch-write `rec+0x4` and `+0x9b0` on each car in turn until one records a kill,
   or watch-write on `0x0068d1f0 + v*4` (written to 2 by the kill path) and break on the next `+0x9b0` writes.
2. Port it behind a default-OFF knob, then re-measure the FX rates on all frames. This changes the sim, so it needs the
   full merge-train gates and a decision.
3. Slot 2: pair the step-dump brake byte with the FX read per frame.

## Raw data (D:/MashedWT/D4-FX-INPUTS_data, not committed), SHA-256

```
66803a54c9f5fa5f985b2c7646bb3e3bd953430892c7fc64d11554e65456a849 orig_inputs1.json
714ffa039d94d196b5ac9c13eb911dcbca9bda2ac184cc1c8ceeb9ea00d0a1d4 orig_inputs2.json
9931cd1581e517033ee430cc06a1ec04a0a5a06a0aa06320ee9af382cb88fe84 arms/F1.fx_debug.txt
5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 arms/F1.step.csv
b493262bed8ea6ecad4399f0ebaa53b902965423e74bfa07359188559fff479f arms/F1.gates.csv
09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1 arms/F1.ptrace.log
12d930aaa9c945b8c0df95b4dd2c8d421cd85a7bbd801ef638daf21c7426ea18 ww_c1.msd.watch.csv
7f675cd59675933c9f1258e0222e81f44cdb2c847bce704795b668315e719383 ww_c2.msd.watch.csv
7c238cc6958f31c36b330a11725d693e8f07cd91577ecfa77efcc22f18bbdc75 mashedmod/build/mashed_re.exe
```
Logs: `fx_arms1.txt`, `fx_arms2.txt`, `orig_inputs1.txt`, `orig_inputs2.txt`, `ww_c1.txt`, `ww_c2.txt`, `build1.log`, `build2.log`.
