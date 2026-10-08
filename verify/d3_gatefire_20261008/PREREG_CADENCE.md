# PREREG — D-11073 exit-flag cadence + clip-handle liveness (tasks 1 and 3)

Date 2026-10-08. Written BEFORE the run. Read-only on the game (ReadProcessMemory poll via
`re/tools/orig_rampwatch.py --cam`; scenario_launch.py owns the process). No build, no C-level.

## Static reading under test (task 1, from Ghidra decomp of the pool clone)

- `FUN_00445aa0` is reached per frame in sub-state 3 by: `FUN_004111c0` case 3 ->
  `FUN_0040fc00` -> `FUN_0040d470(0)` -> `FUN_00448220` -> (`FUN_0040e350() != 6/7/-1`, goto
  `LAB_004483a3`) -> `FUN_00446520(&DAT_00897fe0, ..)` -> call at `0x004468f9`
  (reached when sub-mode != 6, `0x004468e2..0x004468f3`) -> `FUN_004464c0(&DAT_00897fe0)` ->
  `FUN_00445aa0(entry, &DAT_00897fe0)` for every entry with type (`entry+4`) == 0.
- Gates on that path: `FUN_0040d470` returns early when `DAT_0063ba8c == 3 && FUN_00405430() != 0`
  (`FUN_00405430` = `DAT_00639d78 != 0 && DAT_00639d74 <= *(DAT_00639d70+0xc)`);
  `FUN_00448220` diverts to `FUN_00441990` when `DAT_007f1a50 == 1`; `FUN_00446520` takes a
  different branch when `DAT_007f0fd0 == 5 && FUN_00405890() != 0`.
- `param_2` of `FUN_00445aa0` IS `&DAT_00897fe0`, so `*param_2` is the exit flag itself
  (push at `0x00448706`, and `[ebp+8]` pushed at `0x004468f5`). Both clear sites
  sit in the flag-SET branch and need `entry+0xa8 == 0`:
  (a) `fVar3 < _DAT_005ce18c` = **0.02f** (bytes `0ad7a33c`), fVar3 = horizontal distance
      camera -> target; (b) any byte `0x007f1042 + k*0x4c != 0`, k = 0..7 (input bytes,
      written by `FUN_00496530`).
- `FUN_004102f0` order at `0x004102fc..0x00410313`: `FUN_00448700(0,0)` (100 camera ticks,
  flag still 0) -> `FUN_004430a0(1)` -> `FUN_0040e590` -> `FUN_0040d470(1)`.
- Prediction P1: the clear is CONDITIONAL, not immediate. The flag reads 1 across the
  sub-state-3 span and drops to 0 at most one poll before sub-state leaves 3.

## Gates

| id | registered threshold | can see |
|---|---|---|
| G-ARM | exactly one new PID adopted; >= 1 sample with `substate == 3`; `flag_fe0` non-None on every sample with a readable clock | that the poll ran on phase 3 at all |
| G-PATH | on sub-state-3 samples: `n994 >= 1` and `e0_type == 0` on >= 1 sample, `f1a50 != 1` on all | that FUN_00445aa0 is reachable for >= 1 entry (entry 0 only is sampled) |
| G-CLIP (task 3) | report `h657448`, `d70`, `d78` over all sub-state-3 samples. ZERO-ALL => "no computed-base writer fires in this scenario"; any nonzero => a writer exists that static refs cannot see | values at 60 Hz poll; a write-then-clear inside one frame is invisible |
| G-HOLD (task 1) | `flag_fe0 == 1` on >= 50% of sub-state-3 samples => the flag HOLDS (cheap estimate stands). < 50% => the clear is immediate in practice (estimate collapses) | poll-rate granularity only |
| G-CAUSE | at the last flag==1 / first flag==0 pair inside or at the end of sub-state 3: all eight input bytes zero on both samples => clear attributed to the distance branch [poll cannot exclude a 1-frame input pulse; reported as such] | inputs only at poll instants |

Scenario: `--track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --hold 60`, `--hz 60`.
One run. Verdicts reported as registered even if inconvenient.

## ADDENDUM (written after run 1, before run 2)

Run 1 (`o_cadence.csv`) clear coincided with `in0` 0->255 at the same sample. Source:
`scenario_launch.py:3347` pulses control 4 (`E.press(4, 250)`, "to skip the start intro") on
every run that does not set `--statediff-out`. Run 1's hold length is therefore
HARNESS-TRUNCATED. Run 2 measures the natural hold: same argv plus
`--statediff-out <scratch>` (the only switch that suppresses the presses,
`scenario_launch.py:3346`).

| id | registered threshold |
|---|---|
| G-NOPRESS | `in0..in7` == 0 on every sub-state-3 sample (verifies the knob took) |
| G-NATURAL | report the sub-state-3 length (samples and clk span), and at the flag 1->0 sample the entry-0 distance to (`tgt_fe4`,`tgt_fec`) and all eight input bytes. All inputs 0 => clear attributed to site (a) (or to a type-0 entry other than entry 0, which is not sampled). If the flag never clears inside `--hold`, report "no clear within hold" |
