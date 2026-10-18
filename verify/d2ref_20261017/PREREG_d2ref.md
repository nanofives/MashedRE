# PREREG d2ref: the D2 refresh chain on the current default (2026-10-17)

Branch `work/d2ref` from `race/first-frame-parity` `6242c84b6` (default G-CTRL `262504cc / 39fc6210 / 6797ffe5`). Model
claude-opus-5-5, account2. Written and committed before any arm runs.

## 1. Where the chain stands

- U-9212: the original rewrites `+0x9e4 = |+0x9b0|` / `+0x9e8 = |+0x9bc|` after A4 every frame (`0x004710a1..0x004710da`,
  `0x004c3ac0` RW LUT); the port has it as `MASHED_D2LAT_REFRESH` (default OFF). d2lateral (base `10c18c59`) measured that
  the impact fixup `0x0046ef70` reads this value and the refresh closes the impact gap +7.7 % -> +2.9 %, but pins the D2
  player (9/400). With the original's exact spawn heading (diagnostic `MASHED_D2LAT_SPAWN_YAW=-1.5707964`) the car tracks the
  original through two wall contacts, slips in, H1 398/400, driving-median 1985..1987 (bound 1982.44).
- gridyaw (base `5e3fa3a8`): `MASHED_GRID_DIR` ports `FUN_00408b00`'s spawn frame (yaw 180.0 on Training); grid + refresh
  tracks two contacts but slips go out, driving-median 2007..2011; fix B + grid + refresh 1896.97.
- Since then the default gained d2close's walk + K, the countdown clock, the player heading assist, clock-late and the LUT
  lookahead. On this default the refresh alone pins D2 again (c2k93 `D2_CLR_*`: H1 9/400, driving-median 1812.98).

## 2. Arms (`run_arms.py`, D2 recipe, each run alone, 90 s, `verify/d2close_20261016/score_all.py d2`)

| tag | knobs | question |
|---|---|---|
| DEF | none | the default's own numbers on this build (control) |
| R | `MASHED_D2LAT_REFRESH` | the pin on this default (expected: as `D2_CLR`) |
| G | `MASHED_GRID_DIR` | the spawn frame alone |
| RG | R + G | the faithful pair |
| RY | R + `MASHED_D2LAT_SPAWN_YAW=-1.5707964` | diagnostic: the exact heading without the rest of the grid frame |
| RGB | RG + `MASHED_POS_MODEL` | + d2posmodel's fix B |

## 3. Rule and expectations

Pass = slip 1500-2000 in [0.18855, 0.19635], slip 2000-2600 in [0.24488, 0.25487], driving-median in [1904.70, 1982.44], H1
398/400, on 3/3 runs with >= 1600 `motion_diag` lines (short runs re-run). A stack that passes D2 then gets D3 (b) (K0 / K1 /
K26, no passing car may turn to FAIL), the 13-track replay (0 RED) and G-CTRL, and is reported as a default-ON candidate.

Expectations: DEF passes; R pins (0/3); G not predicted; RG and RY track the first contacts (as d2lateral / gridyaw) but
whether driving-median lands in bounds on this default is not predicted; RGB not predicted.

## Amendment 1 (committed before its runs)

Section 2 result: DEF 3/3 in bounds; R pins (H1 9/400); G out on three metrics; RG and RY pass four of five (slip 2000-2600
0.2557 / 0.2551 against the upper bound 0.25487, original 0.2498); RGB out on two. Both refresh pairs sit closer to the original
than DEF on slip 1500-2000 and driving-median and overshoot only the high-speed slip.

New knob `MASHED_A5_RACE_TIMER=1` (default OFF, U-9751): A5 reads `DAT_007f0ff8` (`0x0046e0a9`) instead of the stub
`g_raceTimer` (0 forever). The D2 runs last 90 s, past the ramp's 20 s start. Arms: `T5` (the knob alone), `RGT`, `RYT`
(the refresh pairs + the knob), 3 runs each, each alone; `MASHED_A5_RACE_TIMER_LOG` records the timer.

Expectations: if the exe's `DAT_007f0ff8` does not advance in the D2 recipe the knob is a no-op there (the log shows it);
otherwise not predicted. Pass rule unchanged (section 3).
