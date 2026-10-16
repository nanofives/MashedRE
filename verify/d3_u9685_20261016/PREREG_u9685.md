# PREREG: U-9685, the faithful pre-start-line race_pct (and the G-CTRL of the merged base)

Branch `work/u9685`, base `37834d8c` (race/first-frame-parity, the merge of `work/u9683`). Model claude-sonnet-5-5 (account2, no Ghidra MCP,
bytes via `re/tools/decomp_pc.py` run from the main checkout). Written and committed before any run of the arms of section 3 except the
two G-CTRL arms, which were already running when this file was written (their expected values are the orchestrator's, quoted in section 2, and
nothing about them was adjusted).

## 0. Task (orchestrator, 2026-10-16)

1. Build `37834d8c` in a fresh worktree, run G-CTRL with a private root: default expected `c2e4d60d / 603ab244 / ba90ed88`, opt-out
   `MASHED_NO_REFDIST_AFTER_AI` expected `c2e4d60d / b50bd39b / ba90ed88`.
2. Close U-9685 the faithful way: the bridge reads race_pct 0 for all cars until frame 20 where the original reads 98 to 100. Find the
   original's writer and port it, instead of turning `MASHED_PATHPROG` on wholesale. Score against the existing original captures
   (in-window returns of 2 = 49 / 0 / 0, (b) per regime, collateral). Original-side runs are blocked (audio device): none is used.

## 1. What was read before writing this (bytes and committed data only)

- **The writer.** `FUN_00408610` (`0x00408610`, 1066 B) is the only writer of the per-car block `0x008a9620 + v * 0x30c`; its race_pct is
  `*(0x008a96ec + v*0x30c) = (&DAT_008a95c8)[iVar2 * 10]` (decomp line 114, `decomp_racepct.txt`), a field of the 10-float output of the track-segment
  probe `FUN_00407e20` called at `0x00408610`'s two probe sites, with the node taken from the AI-BSP triangle under the car
  (`FUN_00538c80` sphere query, callback `FUN_00407a90`). It is called per active car from `FUN_0040da50` (call `0x0040fc75` in `FUN_0040fc00`,
  AFTER the AI path `0x0040fc6c` and BEFORE the camera / producer path `0x0040fc7c`). `FUN_00408ad0` (`0x008a96ec + v*0x30c`) is the reader the producer uses.
- **The port already exists.** `Race/PathProgStep.cpp` is a verbatim naked transcription of `FUN_00408610` + 17 callees (replay of 11,996 original
  calls byte-identical at PC_24, RESULT_P2). It is gated by `MASHED_PATHPROG`, which does four things at once: builds the course block
  (`PathNodes_Build`), runs the step on the LIVE block `0x008a9620` (so the lap counter `+0x28` and the node progress `+0xc8` go live too, and
  `AiStandalone.cpp:1656` reads the lap counter into the race metric), runs it in `UpdateRace` BEFORE the AI tick, and stands the bridge down.
- **Data from the committed `u9683` round** (`R_M_PP_OPTOUT.gates.csv`, `R_M_OPTOUT.gates.csv`, `o_rd3.msd.rd.tsv`): the live array at frames 1 to 19 is
  99.09 / 98.20 / 97.28 (cars 1 / 2 / 3, rising) with `MASHED_PATHPROG` and 0 / 0 / 0 with the bridge. The original's producer input at driving calls
  12 to 18 is 99.62 / 98.96 / 98.05 and rising. The two sides cross the line one frame apart (car 1: 0.047 at exe frame 20, 0.052 at original call 19).
  So the existing step is faithful for the pre-line value, and the open question is which of its side effects the AI needs.
- `MASHED_PATHPROG` is known to drop (b) to 0/3 on bases before `0bc7ccd0` (NEXT_SESSION 2026-10-10, "Knob ON changes the AI windows via the faithful race_pct").
  It has NOT been scored on the current base.

## 2. G-CTRL of the merged base (job 1)

Arms `G_DEF` (no knobs) and `G_OPTOUT` (`MASHED_NO_REFDIST_AFTER_AI`), run_h4 recipe, 14400 frames, private game root. Expected, from the
orchestrator's message: `G_DEF` = `c2e4d60d / 603ab244 / ba90ed88`, `G_OPTOUT` = `c2e4d60d / b50bd39b / ba90ed88`. A mismatch is reported as a failed control
and stops the second job.

## 3. The arm and the gates

**Arm R** (`MASHED_RACEPCT_FUN408610`, opt-in at first): the race_pct the AI and the producer read comes from the verbatim `FUN_00408610` step, run on a
PRIVATE shadow row, with nothing else of the block published:

- per frame, per car, at the original's place (end of `UpdateCar`, after the opponent block and before the after-AI producer call, i.e. after the AI path
  and physics, before the producer, as `0x0040fc75` sits between `0x0040fc6c` and `0x0040fc7c`): copy the real row aside, load the shadow row, run the
  step, save the shadow row, restore the real row, write only `+0xcc` (race_pct) into the real row;
- the shadow row starts as `FUN_00407a60` initialises it (zero, lap counter -1), the course block is built at track load as `PathProg` does;
- the bridge stands down while R is on. Everything `MASHED_PATHPROG` also does (live lap counter, live node progress, the `UpdateRace` placement) is NOT done.

| arm | knobs | purpose |
|---|---|---|
| `G_DEF`, `G_OPTOUT` | none / opt-out | G-CTRL (section 2), and `G_DEF` is the default-stack control for collateral |
| `R_H`, `R_H2` | `MASHED_RACEPCT_FUN408610` | run_h4, default stack: G-TOOK-R, collateral, determinism |
| `PP_H` | `MASHED_PATHPROG` | report-only reference: the wholesale arm on the default stack |
| `M_DEF` | mode-7 stack (`MASHED_REGISTRY` `MASHED_PICKUPS_ORIG` `MASHED_REG_CARS` `MASHED_REG_CALLBACKS` `MASHED_MODE7` + traces) | control for the closure gate |
| `R_M`, `R_M2` | mode-7 stack + R | closure gate, determinism |
| `PP_M` | mode-7 stack + `MASHED_PATHPROG` | report-only reference (u9683: in-window 49 / 0 / 0) |
| `B_DEF_K`, `B_R_K`, `B_PP_K` | none / R / `MASHED_PATHPROG`, K in 0, 2, 1, 4, 27, 26, 54, 57, 1200 frames | (b) per regime |

| gate | statistic | pass rule | prediction |
|---|---|---|---|
| **G-CTRL** | `G_DEF`, `G_OPTOUT` step / gates / ptrace sha8 | the two trios of section 2 | PASS |
| **G-TOOK-R** | `R_H` gates `racepct_ec` (live array at the AI tick) vs `PP_H` | for cars 1 to 3, frames 2 to 14, `R_H` frame f == `PP_H` frame f-1 (R publishes at the end of the previous frame), >= 90 % bit-equal. Also `mashed_re.log` carries the game-side witness line | PASS (the same step on the same inputs) |
| **G-PRELINE** | `R_H` array cars 1 to 3, frames 1 to 19 | all in [97.0, 100.0]; max abs difference to the original's producer-input race_pct at the crossing-aligned call (`o_rd3.msd.rd.tsv`, calls 5 to 18, exe frame = call + 1) <= 0.25 | PASS (70 %) |
| **G-ARM-DET** | `R_H` = `R_H2`, `R_M` = `R_M2` hashes | identical | PASS |
| **G-MODE7 / U-9685 closes** | `R_M` consumer returns of 2 in the original's window (204 / 175 / 182 driving calls per car), `score_u9683.py` | = 49 / 0 / 0 | closes (60 %) |
| G-REFIDX (report) | reference index equal to the original's on N / 220 driving producer calls | report (`M_DEF` 183, the `MASHED_PATHPROG` probe 191 / 189) | >= 185 |
| **(b) per regime** | `score_regimes.py` n/3 per regime, `B_R_K` vs `B_DEF_K` | R >= default in every regime (default A 1/3, B 2/3, C 0/3) | unchanged (45 %), a regime loses a car (45 %), gains (10 %) |
| (b) reference | `B_PP_K` | report | report |
| collateral | `collateral.py --mode paired`, `G_DEF` vs `R_H` (floor `R_H2`), `M_DEF` vs `R_M` (floor `R_M2`); ptrace equal or first differing frame | report, ranked | the array differs from frame 1, the first step-field divergence is after it |

**Decisions, pre-committed (the mechanism is not).**

1. **R default-ON** (opt-out `MASHED_NO_RACEPCT_FUN408610`) only if G-CTRL, G-TOOK-R, G-PRELINE, G-ARM-DET and the closure gate pass, **and** (b) R >= default
   in every regime, **and** no collateral field of the player's ptrace changes before the first AI step-field divergence. Otherwise it stays default OFF with a recommendation.
2. **U-9685 closes** iff `R_M` in-window returns of 2 = 49 / 0 / 0. If R keeps car 2's two returns, the producer's race_pct input as seen by R is not
   the whole cause and U-9685 stays open with the PATHPROG-only side effects (lap counter, node progress, placement) as the remaining candidates.
3. If R closes U-9685 but (b) regresses in a regime: R stays OFF, and a scoped variant (the step's value only while a car is pre-line, bridge after) is
   pre-registered in an amendment before it is built. If R does not close it, no scoped variant is built.
4. If G-CTRL fails, nothing else is scored.
5. Exe-side runs only. No original-side capture is taken (audio endpoint). The committed original captures are the references.

## 4. Not done here (registered)

- No hooks.csv change (`FUN_00408610` is an EXE-ONLY naked transcription, no `.asi` body). No tracker is edited by hand.
- `MASHED_PATHPROG` stays default OFF. The `UpdateRace` placement of its step, the live lap counter and `MASHED_ROUND_GRID` are not touched.
