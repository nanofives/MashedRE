# PRE-REGISTRATION: criterion (b), regime-conditioned envelope (USER DECISION (iii), 2026-10-09)

Follows `RESULT_BSCORE.md` §3. Committed BEFORE any original capture for this step and before the
checker changes.

## 1. The rule (decided)

- **Window** unchanged: per car v = 1..3, the 220 AI-step calls from the first call with accel `c4 != 0`.
- **Regime of a run**: **B** if `DAT_0089a368 == 1` at any point inside the window, else **A**. The flag
  is one global, so one regime per run.
  - Port: the step dump's `flag_a368`.
  - Original: the `orig_rampwatch.py` poll's `mode368` over game frames [GO, GO + 220] (GO = first
    `substate 6` sample; frames by `clk_0ff4` / 50).
- **Envelope per regime**: min..max of the same 10 metrics, cars pooled (as ROADMAP §D3's rule), over the
  original runs labelled with that regime.
- **Verdict**: per regime, each port car must lie inside that regime's envelope. The port is scored
  in both regimes, each reached with a chosen ring phase (`MASHED_RING_SKIP`: regime A at K=0, B at K=1;
  `RESULT_BSCORE.md` §1). Reported as "A: n/3, B: m/3".

## 2. Captures and the labelling of the 4 existing runs

Recipe as `o_spread1..3` (`--track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --statediff-car 1
--statediff-aistep --hold 60`), wrapped in `orig_rampwatch.py --hz 60` for the flag. Runs
`o_breg_r1..` are taken one at a time and stop at the first run that is **labelled B**, capped at
**8 runs**. If there is no B in 8, report that and score A only.

| # | prediction | refuted if |
|---|---|---|
| R1 | every new run's 3-car metrics equal one of the two existing outcome sets EXACTLY (`orig_step_slots`'s, or `o_spread3`'s) | a new run matches neither |
| R2 | runs labelled A match the `orig_step_slots` set; runs labelled B match the `o_spread3` set | a label-to-set mismatch |
| R3 | (report) the share of B among the runs taken | n/a |

**If R1 and R2 hold**, the 4 existing runs inherit labels from the set they equal (A, A, A, B), and the
envelopes use all labelled runs, old and new. If R1 or R2 fails, only the new labelled runs build the
envelopes, and the failure is reported.

## 3. Predicted port verdict (from `RESULT_BSCORE.md`, if R1/R2 hold)

- Regime A envelope (= `orig_step_slots`'s 3 cars): c0# 13..35, c1# 21..70, steer# 33..96, abs~ 7..23,
  acc# 2..3, acc~ 255, the medians 0, brk# 2. Port regime A: car 1 PASS, car 2 FAIL (c0# 39, c1~ 5.5,
  steer# 113, abs~ 44.5), car 3 PASS -> **A: 2/3**.
- Regime B envelope (= `o_spread3`'s 3 cars, plus any new B run): c0# 13..37, c1# 17..46, steer#
  29..84, abs~ 0..16, acc# 3..4, acc~ 25..102. Port regime B: all three fail at least c1# and abs~
  -> **B: 0/3**.
