# PRE-REGISTRATION: how criterion (b) is scored, step 1 = measure the port's ring-phase spread

Date 2026-10-09. Motivation: RESULT_414180 §3 and RESULT_LEG2 §4. (b) moved from 5 to 10 failing items
when nothing changed but the RNG ring's phase (4 extra draws). Committed BEFORE the sweep runs. **No
criterion is changed by this step.** It produces the facts the scoring decision needs, and the decision
itself is the user's (ROADMAP §D3 pre-registered the (b) rule on 2026-09-26).

## 0. Facts already in hand (read-only, `orig12.json`)

The (b) envelope is min..max over 12 original observations (4 runs x cars 1..3). Per run and car:

| run | car 1 c0#/c1#/steer#/abs~/acc#/acc~ | car 2 | car 3 |
|---|---|---|---|
| orig_step_slots | 13/21/33/7/3/255 | 35/50/85/11/2/255 | 27/70/96/23/2/255 |
| o_spread1 | identical | identical | identical |
| o_spread2 | identical | identical | identical |
| o_spread3 | 13/17/29/0/4/25 | 37/46/84/16/3/102 | 34/39/73/8/3/102 |

- **Three of the four original runs are identical on every metric for every car**, although their
  windows start at different frames (871 / 837 / 902). So the original's window is not free run-to-run
  noise. It takes **2 distinct outcomes**, and the "12-observation envelope" is 2 outcomes x 3 cars,
  pooled ACROSS cars (e.g. c0# 13..37 spans car 1's 13 to car 2's 35/37).
- [UNCERTAIN] which mechanism separates the two outcomes. `o_spread3`'s accel median 102 =
  trunc(255 x 0.4), and the only known writer of a x0.4 accel is the `DAT_0089a368 == 1` branch
  (`0x004169e0`; port `AiStandalone.cpp:1065`), set by the one-shot difficulty roll (`0x00417c43`).
  The original aistep captures carry no 368 column, so this is an inference from the accel byte, not a
  measurement.

## 1. Measurement

Diagnostic knob `MASHED_RING_SKIP=K` (`AiStandalone.cpp`, after `RwRandomOpen`'s 310 discards; unset =
byte-identical). Default build (no `MASHED_SUBSTATE`), the run_h4 recipe, 1200 frames, K = 0..11
(`run_bscore.ps1`). Each run is scored with `ai_ctrl_window.py --json` per car, plus the step dump's
`flag_a368` over each car's window.

| # | prediction | refuted if |
|---|---|---|
| B1 | K = 0 reproduces the L2_CTRL / L1B_CTRL (b) line exactly (same build path, same phase) | differs |
| B2 | the port's window ALSO falls into a small number of distinct per-car outcomes (<= 3 per car over 12 K) | > 3 distinct outcomes for any car |
| B3 | the outcome is determined by `flag_a368` during the window: runs with the same 368 history give identical per-car metrics | two runs with the same 368 history differ |
| B4 | (report) per car, pass/fail against today's pooled envelope at each K: the fraction of K that pass | n/a |

## 2. What the decision will be offered (after the sweep, not before)

- **(i) keep the pooled envelope, score at one fixed K.** Today's rule. Measured fragility: B4.
- **(ii) pooled envelope, scored over the K sweep.** The rule is unchanged per run, but the verdict
  is the fraction of K that pass, with a threshold to be decided.
- **(iii) regime-matched, per car.** Classify each port run by its `flag_a368` history and compare
  each car to the ORIGINAL's same car in the same regime. This needs B3 to hold and the original's
  regime labelled by a capture that records `0x0089a368` (one `orig_rampwatch.py` run gives it).

The choice is NOT made here.
