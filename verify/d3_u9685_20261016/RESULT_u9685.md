# RESULT: U-9685 closed, the faithful pre-start-line race_pct (and the G-CTRL of the merged base)

Pre-registration `PREREG_u9685.md` (`fbeeb109`, before every arm of its section 3, the two G-CTRL arms were already running and are quoted there)
plus amendment A1 (`2cedd9c5`, after `R_H` / `PP_H`, before the mode-7 and (b) arms were scored). Branch `work/u9685`, base `37834d8c`.
Model claude-sonnet-5-5 (account2, no Ghidra MCP: bytes via `re/tools/decomp_pc.py` from the main checkout). No original-side run (audio device):
the committed original captures are the references. Every run exited 0 (39 of 39), private game root.

## 0. Verdict first

> **Job 1, G-CTRL of `37834d8c`: PASS.** `G_DEF` = **`c2e4d60d / 603ab244 / ba90ed88`**, `G_OPTOUT` (`MASHED_NO_REFDIST_AFTER_AI`) =
> **`c2e4d60d / b50bd39b / ba90ed88`**, both as the orchestrator expected.
>
> **Job 2, U-9685 closes.** The original's writer is `FUN_00408610`, already ported verbatim (`Race/PathProgStep.cpp`). The new arm R runs that step
> on a private shadow row once per frame at the original's place and publishes **only race_pct**. Pre-line race_pct is now within **0.008** of the
> original over calls 5 to 18 (99.24 / 98.42 / 97.51 at call 5, vs 0 / 0 / 0 before). On the mode-7 stack the original's window gives **49 / 0 / 0**
> returns of 2, as the original (was 49 / 2 / 0). (b) per regime equals the default (**A 1/3, B 2/3, C 0/3**) and the AI step output is identical
> in every column except the two that print the race_pct array. `MASHED_PATHPROG` wholesale is NOT needed and costs regime A (0/3).
>
> Default ON, opt-out `MASHED_NO_RACEPCT_FUN408610`. **New default control trio `716cb2fd / 2a2afda1 / ba90ed88`** (step and gates move because their
> `ra_ec` / `val_880` columns print the array, the player trace does not move). The opt-out reproduces `c2e4d60d / 603ab244 / ba90ed88`.

## 1. What the original does (bytes, `decomp_racepct.txt`)

- `FUN_00408610` (`0x00408610`, 1066 B) is the only writer of the per-car block `0x008a9620 + v * 0x30c`. Its race_pct store is
  `*(0x008a96ec + v*0x30c) = (&DAT_008a95c8)[iVar2 * 10]` (decomp line 114), a field of the 10-float output of the track-segment probe `FUN_00407e20`
  (two probe sites, lines 90-95), whose node comes from the AI-BSP triangle under the car (`FUN_00538c80` sphere query, callback `FUN_00407a90`, line 61).
  The reader the producer uses is `FUN_00408ad0` (`0x008a96ec + v * 0x30c`).
- `FUN_0040da50` calls it per active car (decomp line 277). `FUN_0040fc00` calls `FUN_0040da50` at `0x0040fc75`, after the AI path (`0x0040fc6c`) and before the
  camera / producer path (`0x0040fc7c`).
- The port already had a verbatim naked transcription of the step and its 17 callees (RESULT_P2: 11,996 original calls byte-identical at PC_24), behind
  `MASHED_PATHPROG`. That knob does four things at once: builds the course block, runs the step on the LIVE block (lap counter `+0x28` and node progress
  `+0xc8` go live, `AiStandalone.cpp:1656` reads the lap counter), runs it in `UpdateRace` before the AI tick, and stands the bridge down. The bridge reads 0
  for every car until it crosses gate 0, the original reads 97 to 100 (committed `u9683` data).

## 2. The arm (code, commits `fbeeb109`, `fd6147fd`, `2cedd9c5`+)

- `Race/PathProgStep.cpp` `PathProg_RacePctTick`: per active car (same two predicates as `PathProg_FrameTick`), save the real row, load the shadow row, run the
  step, save the shadow row, restore the real row, write only `+0xcc`. `PathProg_RaceInit` zeroes the shadow rows and sets the lap counter -1 as `FUN_00407a60` does.
- `D3d9Render/TrackRenderer.cpp`: the tick runs at the end of `UpdateCar`, before the after-AI producer call (the place of `0x0040fc75`); the course block is built at track
  load when the arm is on; the bridge stands down only when the block exists (`RacePctOrig_Active`). `MASHED_PATHPROG` is untouched.
- Default ON, opt-out `MASHED_NO_RACEPCT_FUN408610`; `MASHED_RACEPCT_FUN408610` is now a no-op. Game-side witness in `mashed_re.log`: `[U-9685] race_pct from the verbatim step
  on a shadow row` (present in `R_H.log` / `F_DEF.log`, absent in `G_DEF.log` / `F_OPTOUT.log`). EXE-ONLY, no `.asi` body, no hooks.csv change.

## 3. Gates

| gate | result |
|---|---|
| G-CTRL (job 1) | **PASS** `G_DEF` `c2e4d60d / 603ab244 / ba90ed88`, `G_OPTOUT` `c2e4d60d / b50bd39b / ba90ed88` |
| G-TOOK-R | **as registered FAIL** (0 / 39, max abs diff 0.235): my relation `R(f) == PP(f-1)` was derived wrongly. **Corrected (A1) PASS**: frame 1 bit-equal on cars 1 to 3, frames 1 to 14 within 0.169 of `PP_H` (bit-equal 30 / 42, they diverge because `PP` also makes the lap counter and node progress live). `SCORE_RACEPCT.txt` |
| G-PRELINE | **PASS.** Cars 1 to 3 in [97, 100] on frames 1 to 19, max abs difference to the original over calls 5 to 18 **0.008** (alignment orig call c <-> gates frame c + 1, A1) |
| G-ARM-DET | **PASS.** `R_H` = `R_H2` (`716cb2fd / 2a2afda1 / ba90ed88`), `R_M` = `R_M2` (`4b6dcb0e / dcc1377c / be82fcc4`) |
| **G-MODE7 / U-9685 closes** | **PASS.** `R_M` in-window returns of 2: **49 / 0 / 0** (original 49 / 0 / 0, `M_DEF` 49 / 2 / 0). Car 2's refdist at driving calls 16 to 20 is `3febaf63 / 3fe9fe27 / 3fe8432f / 3fe67ebb / 3fe4b12b` (original `3feb3cf3 / 3fe97d8a / 3fe7c610 / 3fe5f483 / 3fe41f40`), was 0. `SCORE_R_M.txt` |
| G-REFIDX (report) | reference index equal to the original on **189 / 220** driving producer calls (`M_DEF` 183, the PATHPROG probe of the `u9683` round 191). `REF_COMPARE_R.txt` |
| G-REFDIST (report) | refdist-0 calls 146 / 1 / 22 vs original 164 / 4 / 29 (`M_DEF` 140 / 6 / 23): cars 1 and 3 still outside 3, car 2 inside. Within-band 243 / 561 (43.32 %, `M_DEF` 42.42 %, +0.89 points) |
| **(b) per regime** | **R = default in every regime**: A 1/3, B 2/3, C 0/3 on all 8 `K`. The step CSVs differ only in `ra_ec` / `val_880` (checked column by column on all 8). Reference `MASHED_PATHPROG`: **A 0/3**, B 2/3, C 0/3. `SCORE_B_u9685.txt` |
| collateral | default stack (`COLLATERAL_H_stdout.txt`, floors `G_OPTOUT`, `R_H2`): **6 of 244** fields diverge, `ra_ec` and `val_880` of cars 1 to 3, all from frame 1 (they print the array). Every control byte, position and state field is within the floor, ptrace equal (28076 lines, `ba90ed88`). Mode-7 stack (`COLLATERAL_M_stdout.txt`): car 2 first differs at frame 16 (the removed entry), cars 1 and 3 at frame 160, the player's ptrace first at frame 24, after car 2's. 181 of 244 fields diverge after those frames |
| post-flip arms | `F_DEF` = `716cb2fd / 2a2afda1 / ba90ed88` (= `R_H`), `F_OPTOUT` = `c2e4d60d / 603ab244 / ba90ed88` (= `G_DEF`), `F_MDEF` = `R_M`'s `4b6dcb0e / dcc1377c / be82fcc4`, `F_B_K00 / 01 / 26` = `B_R`'s `774fadda / eda1cd61 / 741f163b`. After the last text-only edit (witness string) `F_B_K00` was re-run: `774fadda` |

Predictions: closure 60 % held, (b) unchanged held (the AI is in fact byte-equal outside the two array columns), G-PRELINE 70 % held, G-REFIDX >= 185 held (189).
The registered G-TOOK-R relation was wrong (A1). Decision 1 of the PREREG fired as written (default ON).

## 4. What this changes in the earlier claims

- U-9685: the cause was the producer's race_pct input before the start line, not the order (U-9683) and not the grid pose. Fixed.
- "`MASHED_PATHPROG` alone drops (b) to 0/3" is refined on the current base: wholesale it gives A 0/3, B 2/3, C 0/3 (default A 1/3). The regime-A cost comes from the parts R does not
  do (live lap counter, live node progress, the `UpdateRace` placement), not from race_pct. R proves race_pct is AI-neutral on the default stack.

## 5. Residues, stated

- The very first AI tick of a race still sees race_pct 0 (the port has no pre-race `FUN_0040da50` ticks, the original has dozens). No AI column differs, so it is cosmetic, but it is a difference.
- Lap counter `+0x28`, node progress `+0xc8` and the `UpdateRace` placement of the step stay as before (`MASHED_PATHPROG` territory).
- Cars 1 and 3 still have 140-146 refdist-0 calls against the original's 164 / 29: the pace gap of U-9185 / U-9186, unchanged.

## 6. Proposed re-classify transaction (not applied)

- **U-9685** -> resolved 2026-10-16: cause = the producer's race_pct input before the start line (bridge 0, original 97 to 100). Fix: the verbatim `FUN_00408610` on a shadow row publishes race_pct,
  default ON, opt-out `MASHED_NO_RACEPCT_FUN408610`. Gate: in-window returns of 2 = 49 / 0 / 0, pre-line race_pct within 0.008, (b) equal default. New default G-CTRL `716cb2fd / 2a2afda1 / ba90ed88`.
- hooks.csv: no change (`FUN_00408610` has no `.asi` body here; the port step is the verbatim transcription behind both knobs).
- CHANGELOG line: `2026-10-16 U-9685 resolved: race_pct for every car now comes from the verbatim FUN_00408610 (shadow row, only +0xcc published, 0x0040fc75 place), default ON,
  opt-out MASHED_NO_RACEPCT_FUN408610. Pre-line race_pct within 0.008 of the original, mode-7 in-window 49/0/0, (b) A 1/3 B 2/3 C 0/3 unchanged. verify/d3_u9685_20261016/RESULT_u9685.md`
- Memory note `default-ctrl-hashes-after-b2-flip`: new default trio.

## 7. Decisions needed (user)

1. **DECIDED 2026-10-16 (user): keep the default-ON arm.** It changes the default control trio to `716cb2fd / 2a2afda1 / ba90ed88` (step and gates print the array), the player trace is unchanged. No code change follows: the branch already has it default ON.
2. The regime-A cost of `MASHED_PATHPROG` (A 0/3 vs 1/3) is the next question for any `MASHED_ROUND_GRID` default (it needs PATHPROG). Not started.

## 8. Raw

`run_u9685.py`, `score_u9685.py` (= `score_u9683.py` with both arms on the after-AI order), `score_racepct.py`, `ref_compare.py`, `SCORE_RACEPCT.txt`, `SCORE_R_M.txt`, `REF_COMPARE_R.txt`,
`SCORE_B_u9685.txt`, `COLLATERAL_H*`, `COLLATERAL_M*`, `decomp_racepct.txt`, the arms' `*.step.csv`, `*.rdtrace.tsv`, `*.cb.tsv`, `*.c30.tsv`, gates of `G_DEF` / `G_OPTOUT` / `R_H` / `PP_H`.
ptrace logs and `*.log` are gitignored, their hashes are above.
