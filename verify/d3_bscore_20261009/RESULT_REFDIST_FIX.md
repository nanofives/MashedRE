# RESULT: the reference-distance bridge picked the wrong car (U-9205, corrected and fixed)

Date 2026-10-09. Pre-registration: `PREREG_REFDIST_FIX.md` (`50757997`, before the fix).
Fix: `Race/SpectatorDistances.cpp` (`FUN_00442a60` bridge, behind `MASHED_REFDIST`, default OFF).

## 0. Verdict first

> - **The player input already matched.** The port's player gets no input; `RESULT_B2GATE.md`'s
>   "the port drives its player" was wrong. Both players are pushed by AI car 2 at frame ~22, and both
>   trail by frame 200 (`PUSH_ORIG.txt`).
> - **The bridge inverted the reference car.** The original's `FUN_00442a60` (`0x00442b3a..0x00442b97`)
>   makes the pair's LEADING car (higher race_pct) the reference, unless that car's state is non-zero.
>   The port took the rearmost. Fixed as an exact transcription, including the no-store exits.
> - **F1 PASS:** knob OFF byte-identical (`c741a4c5` / `642af0ac` / `05984e30`).
> - **F2 PASS:** first race frame distances v0..v3 = **0 / 1.550 / 1.823 / 2.739** (original
>   0 / 1.553 / 1.833 / 2.750). Before the fix: 2.739 / 1.829 / 1.553 / 0.
> - **F3:** branch 2 first fires at **117 / 119 / 192** (cars 1 / 2 / 3). Cars 1 and 2 hit the
>   prediction (117, 120). Car 3 misses it (296 +-15): the corrected distances also feed other AI
>   readers, which moved its trajectory before its first firing, the registered caveat. Original:
>   149 / 216 / 180. Before the fix, only car 2 ever met the condition, at 643.
> - **F4 (report; B2 set + fix, not the default build):** (b) regime A: car 1 PASS, **car 2 FAIL on 1
>   item** (abs_steer_median 0 vs 7..23; it failed 5 items before), car 3 PASS.

## 1. What it means

The branch-2 gate and its timing are now within ~30-100 frames of the original on all three cars,
where it was 400+ frames late. The default build is unchanged, because the whole B2 set (bridge,
`MASHED_RACEPCT_BRIDGE`, `MASHED_A364_RESET`, `MASHED_WIRE_B2`, harness `MASHED_NO_ELIM`) is
default-OFF. Whether to make it default is a user decision (ROADMAP: knob defaults). Car 2's
remaining regime-A item, |steer| median 0, is the next thing to look at under that configuration.

## 2. Raw

`PREREG_REFDIST_FIX.md`, `predict_refdist_fix.py`, `PUSH_ORIG.txt`, `FIX_CTRL.txt`, `FIX_B2.txt`,
`FIX_SCORE.txt`, `B2K00.{step,gates}.csv` (fixed), `B2K00_prefix.{step,gates}.csv` (before).
