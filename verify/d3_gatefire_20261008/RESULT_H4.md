# RESULT: `H4` DECIDED. `MASHED_SLOT_PLAYER` is DEFAULT-ON; `MASHED_WIRE_B2` stays OFF.

Date 2026-10-08. Pre-registration: `PREREG_H4.md` (written before any run). Driver `run_h4.ps1`,
scorer `score_h4.py`. Builds: 18:44 (arms A-D, HEAD `85aa8793`), 18:56 (post-flip arms).
No C-level.

## 0. Verdict first

> **Car 0 is now measured, and `MASHED_SLOT_PLAYER` is inert on all four cars.** Over 14,400
> deterministic frames, `player_trace.log` (car 0) and the stepdump (cars 1-3) are
> **byte-identical** with and without it. Only the gates dump's `slot_state` for v0 moves, 0 -> 1.
> That 1 is the value the original holds (`RESULT_E470.md`, 581/672). Per the registered rule it
> is **default-ON**, with opt-out `MASHED_NO_SLOT_PLAYER`.
>
> **Branch 2 does not touch car 0 either.** With branch 2 firing 18 times on car 2 (frames
> 643+), the three AI cars diverge on 38,481/42,654 stepdump rows, yet car 0's trace is
> byte-identical to the no-fire arm. Car 0 is **moving** through that window (852 position
> changes, steps 23-874), so this is a real null, not a parked-car artefact.
>
> **`MASHED_WIRE_B2` stays OFF**, on its prerequisites, not on this measurement. It fires only
> with `A364_RESET` + `REFDIST` + `RACEPCT_BRIDGE` (all undecided) and the harness override
> `NO_ELIM`.

## 1. Gates, as registered

| gate | verdict | evidence |
|---|---|---|
| H4-FLOOR | **PASS** | A vs A2: ptrace `05984e30` = `05984e30` (26,996 lines); step `c741a4c5` = `c741a4c5`, 0/84 columns differ on 19,418 keys; gates `c8f14d5b` both |
| H4-ARM | **PASS** | v0 `slot_state` = 1 on 13,498/13,498 (B) and 14,218/14,218 (C); = 0 on all rows of A, A2, D. `p5f2770` = `0x005f2728` on every row of every arm |
| H4-C0-INERT | **PASS** | A vs B: ptrace byte-identical; step byte-identical (`c741a4c5`), 0/84 columns differ, so `ss_*` too |
| H4-FIRE | **PASS** | C: `w_fire` max 18 on car 2, first at frame 643. D: 0 on every car |
| H4-C0-B2 (descriptive) | **no car-0 effect** | C vs D ptrace byte-identical (`e418c03b`, 28,436 lines). Step C vs D: first differing frame 643, 38,481 rows, cars 1/2/3, led by `rec_148`, `h4x..h7z`, `rec_9d4` |
| post-flip default = B | **PASS** | `H4_post`: step `c741a4c5`, gates `642af0ac`, ptrace `05984e30`, all equal to B |
| post-flip opt-out = A | **PASS** | `H4_optout` (`MASHED_NO_SLOT_PLAYER`): step `c741a4c5`, gates `c8f14d5b`, ptrace `05984e30`, all equal to A |

## 2. What the gates can and cannot see

- **The car-0 instrument can see car-0 changes.** B vs C (and A vs D) differ from ptrace line
  307 onward (the race-state `post` line: `alive`, `prog`, `gc`), and C runs 28,436 lines vs
  26,996. So identity under H4-C0-INERT and H4-C0-B2 is not blindness.
- **Car 0 moves only early in this recipe.** C: 852 distinct positions, steps 23-874; A: 3,597,
  last change at step 9,457. Car 0 is driven by the scaffold/demo logic (`TrackRenderer.cpp:3205-3257`),
  not by an AI ctrl block. Branch 2's firings (643-660) fall inside C's moving window. **Not
  covered:** a car 0 that is actively raced for the whole round, other tracks, other rules.
- **Why both nulls are expected statically.** Car 0 is never a `ControlStep` subject
  (`AiStandalone.cpp:1879-1881`, `aib_veh_type(0)==0`). The stores' only non-diagnostic readers
  are `AiLeaderTimer.cpp:173` / `:205`, which run only under `MASHED_GF1` or `MASHED_WIRE_B2`.

## 3. Changed defaults, carry these

- **Default controls after this leg:** step `c741a4c5` (unchanged), ptrace `05984e30`, and the
  gates dump **`642af0ac`** (was `c8f14d5b`: the v0 `slot_state` column).
- `MASHED_SLOT_PLAYER` is now a **no-op** (kept so drivers work). An arm that wants the old
  behaviour must set `MASHED_NO_SLOT_PLAYER`.
- Branch 2 now needs `A364_RESET` + `REFDIST` + `RACEPCT_BRIDGE` + `NO_ELIM` + `WIRE_B2`
  (`SLOT_PLAYER` no longer needs setting).
- Harness fix: `run_h4.ps1` captured `$args` before `Where-Object` (inside the block `$args` is
  the block's own, so the first post-flip invocation ran zero arms and printed `H4 DONE`).
