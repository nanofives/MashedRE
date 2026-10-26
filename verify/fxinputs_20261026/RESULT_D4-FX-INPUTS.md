# RESULT D4-FX-INPUTS: killed cars keep their physics, `MASHED_DEAD_COAST` (2026-10-26, attempt 2)

**Verdict: RED on the pre-registered rule, by one criterion.** The original has no dedicated writer that zeroes a
killed car's `+0x9b0`. It keeps stepping the car through the ordinary physics chain on an all-zero input block, and the
car brakes to an exact 0.0. The port skipped the killed car, so its record froze. `MASHED_DEAD_COAST` (default OFF)
ports that. With it ON, slots 1 and 2 stop at exactly 0.0 and stay there (0 and 1 frozen frames out of 13,998, against
13,997 with the knob OFF), and the slot 1 FX rate moves into the 2x band. M1's third criterion fails: the port's
killed cars need 133 and 149 frames to stop, the PREREG limit was 120. The port kills those cars at |v| 2014.9 and
2507.9, the original captures at 297.7 and 174.8. Knob OFF leaves every gate hash unchanged. Knob ON moves two G-CTRL
hashes, listed in section 5 for a flip decision.

Attempt 1 is kept in git history (b122ca5cf). PREREG: `PREREG_deadcoast.md` (80806ffe7, committed alone before the arms).

## 1. The writer, original side

`scenario_launch.py --track 0 --mode 10 --cars 4 --car 1 --rule 0 --hold 300 --statediff-car 2 --watch-write 4,9b0`.
Attempt 1's 70 s watches recorded no kill. This 300 s watch recorded one (`ww3_c2_writers.txt`, `ww_kill_writers.py`):

- `rec+0x4` write: pc `0x0046c5e4` (`FUN_0046c5c0`), returns `0x00422fe0` (`FUN_00422fd0`), `0x0041102a`
  (`FUN_00410d10`), `0x00411608`.
- 19,389 writes to `+0x9b0` while `rec+0x4 == 0`. Every one comes from the physics chain: pc `0x0046e2c2` /
  `0x0046e340` (returns `0x00470948`, `0x00470928`, `0x0047107b`, `0x00425a92`), pc `0x0046868a` / `0x004688cc` /
  `0x0046894c` (return `0x00470948` / `0x0047093b`). 3,852 of them leave `+0x9b0 == 0.0`. Seven writes come from pc
  `0x0046bf94` (returns `0x0047e96a`, `0x0047ea22`, `0x0046cb15`, `0x00467583`, `0x00470e1a`, `0x004187d5`, `0x0041893a`).

The call chain that keeps the car stepping (decomp, `re/tools/decomp_pc.py`, pool slot 10, read-only):

- `FUN_00422fd0` (0x00422fd0..0x00423032) calls `FUN_0046c5c0(v)`: `(&DAT_008815a4)[v*0x341] = 0` (`rec+0x4 = 0`) and
  `rec+0xb10 = DAT_007f1030`. Then `FUN_0046c790(v, 1)`: `(&DAT_008815b0)[v*0x341] = 1` (`rec+0x10 = 1`).
- `FUN_00470c70` A4 loop: a car qualifies when `rec+0x14 == 0 && (rec+0x4 != 0 || rec+0x10 == 1)` (0x00470ffe,
  0x0047100a, 0x00471012). Its input pointer is `&DAT_007f1038 + slot*0x13` dwords, replaced by `&DAT_007f19b8` when
  `rec+0x9f0 != 0`, when `rec+0x10 != 0` (`piVar14[-0x278]`, piVar14 = `rec+0x9f0`), or when `FUN_0040e350()` is not
  6 / 0xb / 0xa. The speed refresh loop has the same guard (0x004710b0 / 0x004710b7). The substep `FUN_004709a0` has the
  same guard and the same substitution (`re/analysis/vehicle_dynamics/004709a0.md`).
- `&DAT_007f19b8` = 0x4c zero bytes on the original (`o_r7_1`, the MASHED_R7_PHYS note in VehiclePhysicsRun.cpp).

Per frame (`msd_dead_decay.py`, `decay_deadcoast.txt`):

| capture | kill frame | |v| at kill | frames to |v| == 0.0 | zero frames | frozen non-zero | FX speed-gate passes |
|---|---|---|---|---|---|---|
| original ww3_c2 (watch run, about 19 fps) | 1726 | 297.6676 | 27 | 3852 / 3879 | 0 | 4 |
| original o60_c2 (no watch, about 60 fps) | 1802 | 174.7945 | 17 | 7216 / 7233 | 0 | 0 |

`rec+0x10` reads 1 on every dead frame of both captures.

## 2. The port change (default OFF)

`MASHED_DEAD_COAST=1`:
- `VehiclePhysicsRun.cpp`: `VehiclePhysics_MarkKilled` (`+0x4 = 0`, `+0x10 = 1`), `VehiclePhysics_ClearKilled`
  (`+0x4 = 1`, `+0x10 = 0`). In `StepCar`, a car with `+0x10 != 0` keeps `+0x4` (the per-step `kActiveFlag = 1`
  write is skipped) and gets the block at `0x007f19b8` as input, read in place as MASHED_R7_PHYS does.
- `TrackRenderer.cpp`: the two elimination sites and `KillCar` call MarkKilled. `StartRound` calls ClearKilled. The
  opponent loop steps a dead AI car instead of skipping it, without the scaffold spin-out and the 2.5 s stuck
  relocation.
- Not ported: `rec+0xb10 = DAT_007f1030` (no port reader). [UNCERTAIN] the original writer that resets `+0x4` / `+0x10`
  on a new round is not measured. The G-CTRL recipe plays one round on the port, so no arm reaches it.

## 3. Measurements (G-CTRL recipe + FX debug + `MASHED_RECDUMP`, exe e7cdf357)

**M1, killed-car speed** (`decay_deadcoast.txt`):

| arm | slot | |v| at kill (f 220) | frames to 0.0 | zero frames | frozen non-zero | gate passes | PREREG |
|---|---|---|---|---|---|---|---|
| X0 knob OFF (control) | 1 | 2046.2510 | never | 0 / 13,998 | 13,997 (100.0%) | 13,998 | control valid (> 90%) |
| X1 knob ON | 1 | 2014.9087 | 133 | 13,865 / 13,998 | 0 | 111 (0.8%) | fails the 120-frame limit |
| X2 knob ON | 2 | 2507.8657 | 149 | 13,849 / 13,998 | 1 | 126 (0.9%) | fails the 120-frame limit |

The frozen and gate criteria pass on both slots. The time criterion fails on both. Low-speed decel per frame: X1 loses
10.2 to 10.4 per frame from 200 down to 25.3, then 0.0. The 60 fps original loses 5.7 per frame from 48.4 down to 25.7,
then 0.0. [UNCERTAIN] whether the 2x low-speed decel is a port fault or a different contact or surface state. A
matched-speed comparison would settle it: one original kill at |v| about 2000, or a port arm that kills a slow car.

**M2, FX emits per 1000 active frames, all frames** (`score_deadcoast_X.txt`; original from attempt 1):

| slot | X0 OFF (= attempt 1 F1) | X1 / X2 ON | active frames OFF -> ON | original | within 2x |
|---|---|---|---|---|---|
| 0 | 59.0 | 66.4 | 407 -> 256 | n/a | n/a |
| 1 | 52.4 | 113.1 | 14,205 -> 327 | 166.1 | yes (0.68x) |
| 2 | 49.1 | 52.0 | 14,209 -> 346 | 215.3 | no (0.24x), as pre-registered |
| 3 | 215.5 | 215.5 | 232 -> 232 | 191.0 | yes |

X0's `fx_debug.txt` is byte-identical to attempt 1's F1 (SHA-256 9931cd15...). X1 and X2 match each other byte for
byte (5eda26cf...). Slot 0 moves because the player is killed at f 220 too (`ELIM-RESOLVE ... alive=0,0,0,1`) and now
coasts on the zero block.

## 4. Gates

Knob OFF, `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, exe e7cdf357:
G-CTRL `5943c5f1 / b493262b / 09551e6a`, B `3a03670c / e85ea6fb / 53778664`. Both equal `merge_train.json`. The
diagnostic X0 arm gives the same G-CTRL triple.

Knob ON, `run_arms.py` copy in this folder, stack `DC`: G-CTRL `5943c5f1 / 364350ae / 000a8b2e`, B `3a03670c /
e85ea6fb / 53778664`. H_X1 and H_X2 reproduce the same G-CTRL triple. `gates.csv` and `player_trace.log` first differ
on frame 221, the frame after the kill at frame 220. `ptrace` line 441: `sp` 882.7593 OFF, 873.1369 ON. The step dump
does not move, because it logs live cars only.

## 5. Decision for the lead

A flip of `MASHED_DEAD_COAST` would change the refs to G-CTRL `5943c5f1 / 364350ae / 000a8b2e`, B unchanged. Options:
(a) flip now. The mechanism matches the original and removes the 98.6% frozen frames. The stop takes 133 to 149 frames
from a kill speed 7 to 14x the original's. (b) hold the flip until the matched-speed decel check in section 3 is run.
Recommendation: (b). The failing criterion was pre-registered and the 2x low-speed decel is not explained. No default
changed here.

## 6. Next

1. Matched-speed decel check (section 3). The original's kills in this recipe come at |v| 175 to 298. The port's come at
   2000 to 2500.
2. Slot 2 FX rate (0.24x): the brake-byte pairing of attempt 1, section 3, still open.
3. The round-restart writer of `+0x4` / `+0x10` (section 2), once a port recipe plays a second round.

## Raw data (D:/MashedWT/D4-FX-INPUTS_data, not committed), SHA-256

```
5c6fea64974e3a69b7f1c170c6ea3cca4a11ca0d30c7e8903eef03116ea7050c ww3_c2.msd
dc6c9ef992680c05cc4a4d9831e0a7167aa4b13433c8bfed559a8a6bcf7902a9 ww3_c2.msd.watch.csv
19bde9957000bbf709133d06e3d14ee0675fc47b95f6a2278928939fbe261bdd o60_c2.msd
5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 arms2/H_DEF.step.csv
b493262bed8ea6ecad4399f0ebaa53b902965423e74bfa07359188559fff479f arms2/H_DEF.gates.csv
09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1 arms2/H_DEF.ptrace.log
364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4 arms2/H_DC.gates.csv
000a8b2e8cd6a4427a2e8924a917546ffce88d434e291553d0cdf6a191aef98f arms2/H_DC.ptrace.log
5e844d18ba113d02d6411f5dc426917e26b311f598ca8a8947be45bfbae2ba68 arms2/H_X0.msd
3601b20ab793ac1b0363a15d2bcb3a94e7d8ce9f0d29fa9a28f00486bf4f3f6a arms2/H_X1.msd
f2668b7bfec5c396a2878f106b58ea91debab60f6d1d21c92cde7677d9519a2d arms2/H_X2.msd
9931cd1581e517033ee430cc06a1ec04a0a5a06a0aa06320ee9af382cb88fe84 arms2/H_X0.fx_debug.txt
5eda26cf672016ea85d46c13ee1bfa8d889b1380d8214595e985e4182629510a arms2/H_X1.fx_debug.txt
5eda26cf672016ea85d46c13ee1bfa8d889b1380d8214595e985e4182629510a arms2/H_X2.fx_debug.txt
e7cdf357c9e8e2739244eb5f2c74445008adc01e4692cc6bb9e3fe8abcceffc0 mashedmod/build/mashed_re.exe
```
Logs: `ww3_c2.txt`, `o60_c2.txt`, `gates_off.txt`, `arms_on.txt`, `build3.log`, `arms2/RUNLOG.txt`. The build ran
inside `mashed_lock.py run` (label D4-FX-INPUTS-build). All game runs went through the lock, and only the spawned PIDs
were touched.
