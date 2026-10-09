# RESULT: criterion (b) and the RNG ring phase (step 1, measurement)

Date 2026-10-09. Pre-registration: `PREREG_BSCORE.md` (`54061b08`, before the sweep). No criterion
changed. The default build is byte-identical: `MASHED_RING_SKIP` is unset by default, and K = 0
reproduces `L2_CTRL`.

## 0. Verdict first

> **(b)'s ring-phase sensitivity is exactly one thing: the one-shot difficulty roll.** Over 12 ring
> phases (K = 0..11, default build):
> - **B1 holds:** K = 0 equals `L2_CTRL`'s window.
> - **B2 holds:** every car takes exactly **2** distinct outcomes.
> - **B3 holds:** the outcome is fully determined by `DAT_0089a368` in the window.
>   - **Regime A** (flag 0 for all 220 calls): **10/12** phases.
>   - **Regime B** (0 -> 1 at call 60, the roll won): **2/12** (K = 1, 4). 17 %, consistent with the 20 %
>     roll at `0x00417c43`.
>
> Under today's pooled envelope:
> - regime A passes **2 of 3 cars** (car 2 fails 5 items), at every A phase;
> - regime B passes **0 of 3**.
>
> P414's "5 -> 10 items" was a regime flip, not noise.
>
> **The original has the same structure.** Three of its four runs are identical on every metric for
> every car. The fourth (`o_spread3`) carries accel median **102** = trunc(255 x 0.4) on cars 2 and 3,
> the same value the port shows in regime B (`AiStandalone.cpp:1065`, `0x004169e0`). So the pooled
> "12-observation envelope" mixes 3 regime-A runs, 1 regime-B run, and 3 cars.

## 1. The two port outcomes per car

| car | regime | c0# | c1# | steer# | c1~ | abs~ | acc# | acc~ | brk# | pooled envelope |
|---|---|---|---|---|---|---|---|---|---|---|
| 1 | A | 15 | 69 | 83 | 0 | 7 | 2 | 255 | 2 | PASS |
| 1 | B | 15 | 67 | 81 | 0 | 39 | 3 | 102 | 2 | FAIL 1 |
| 2 | A | 39 | 75 | 113 | 5.5 | 44.5 | 2 | 255 | 2 | FAIL 5 |
| 2 | B | 43 | 73 | 115 | 0 | 55 | 2 | 102 | 1 | FAIL 5 |
| 3 | A | 30 | 48 | 77 | 0 | 11.5 | 2 | 255 | 2 | PASS |
| 3 | B | 30 | 81 | 110 | 0 | 19.5 | 2 | 102 | 1 | FAIL 3 |

## 2. The original, split the same way (from `orig12.json`)

| car | regime A (3 identical runs) | regime B (`o_spread3`, n = 1) |
|---|---|---|
| 1 | 13 / 21 / 33 / abs 7 / acc# 3 / acc~ 255 | 13 / 17 / 29 / abs 0 / acc# 4 / acc~ 25 |
| 2 | 35 / 50 / 85 / abs 11 / acc# 2 / acc~ 255 | 37 / 46 / 84 / abs 16 / acc# 3 / acc~ 102 |
| 3 | 27 / 70 / 96 / abs 23 / acc# 2 / acc~ 255 | 34 / 39 / 73 / abs 8 / acc# 3 / acc~ 102 |

[UNCERTAIN] the original's regime labels are inferred from accel ~102; no original capture records
`0x0089a368`. One `orig_rampwatch.py` run with that column settles it.

## 3. Options for the scoring rule (USER DECISION; nothing changed)

| | rule | measured consequence today |
|---|---|---|
| (i) | keep: pooled envelope, one run at the default phase | 2 / 3 at 10 of 12 phases, 0 / 3 at the other 2. A code change that adds or removes a ring draw can flip the verdict with no behaviour change (P414) |
| (ii) | pooled envelope, scored over a K sweep, verdict = pass fraction | car 1 10/12, car 2 0/12, car 3 10/12. Ring-proof, but it still mixes regimes and cars |
| (iii) | **regime-conditioned**: label each run's regime (port: `flag_a368`; original: a capture with `0x0089a368`), score against the envelope of the SAME regime (cars pooled, as ROADMAP's rule); report both regimes, each reached by a chosen K | A: 2 / 3 (car 2 fails). B: 0 / 3 against a 1-run reference. The verdict can no longer move with the ring phase, only with behaviour |
| (iv) | (iii) and per car (port car v vs original car v, same regime) | strictest, and it shows the pooling hides per-car mismatches (e.g. port car 1 regime A c1# 69 vs the original's 21). Needs car identity to be confirmed equal on both sides first |

**Recommendation: (iii).** It is the smallest change that removes the ring-phase fragility while
keeping ROADMAP's pooled-across-cars rule. Two owed items come with it: one original capture recording
`0x0089a368` to label the regimes, and at least one more original regime-B run, since n = 1 is thin.
(iv) is the follow-up once car identity is checked.

## 4. Raw

`PREREG_BSCORE.md`, `run_bscore.ps1`, `SWEEP_RUN.txt`, `K00..K11.step.csv`, `SWEEP_SCORE.txt`,
`score_bscore.py`, `orig12.json`. PIDs: each sweep run's own, all exited on their own.
