# RESULT: criterion (b), regime-conditioned envelope (user decision (iii))

Date 2026-10-09. Pre-registration: `PREREG_BREGIME.md` (`db548ead`, before any capture). ROADMAP §D3
carries the amendment. The 2026-09-26 rule (`--check`) is kept and still runs unchanged.

## 0. Verdict first

> **(b) under the decided rule: regime A 2/3 cars (car 2 fails). Regimes B and C are not scored yet:
> no labelled original reference (B), and no port ring phase landing in it (C).**
>
> - **Captures:** 8 original runs with `DAT_0089a368` polled; 7 reached GO (run 6 did not).
>   - 6 are **regime A** (flag 0 all window). All 6 are **identical** to each other and to the 3 earlier
>     A-runs, on every metric for every car.
>   - 1 (run 2) shows the flag going 0 -> **2** at game frame 61. That is the same roll the port fires at
>     call 60, with its other outcome. Its window differs from A.
>   - **No run reached flag 1** (regime B) within the pre-registered cap of 8.
> - **Correction, stated as such:** the pre-registered regime definition ("B if the flag reaches 1, else
>   A") was a wrong diagnosis. The roll has **three** outcomes, and folding flag 2 into A would have
>   widened A's envelope with different behaviour. The decision (iii) is unchanged. The regime is now
>   the first non-zero flag value in the window: none -> A, 1 -> B, 2 -> C (`ai_ctrl_window.py
>   window_regime`).
> - **Pre-registered consequences honoured:**
>   - R1 FAILED (run 2 matched neither existing set), so envelopes come only from the newly labelled
>     runs. For A the values are identical anyway.
>   - No B within the cap, so B is reported, not scored. The earlier `o_spread3` stays an unlabelled,
>     inferred-B run.

## 1. Envelopes (`re/tools/b_envelopes.json`, from `b_envelopes.py`)

| metric | A (6 runs x 3 cars) | C (run 2, car 3 only) |
|---|---|---|
| c0 distinct | 13..35 | 27 |
| c1 distinct | 21..70 | 95 |
| steer distinct | 33..96 | 121 |
| c1 median | 0 | 27.5 |
| abs steer median | 7..23 | 36.5 |
| accel distinct / median | 2..3 / 255 | 2 / 255 |
| brake distinct / median, c0 median | 2 / 0, 0 | 2 / 0, 0 |

Regime A's envelope is narrower than the pooled 2026-09-26 one (c0# 13..35 vs 13..37, c1# 21..70 vs
17..70, steer# 33..96 vs 29..96, abs~ 7..23 vs 0..23, accel~ 255 vs 25..255). The pooled envelope's
extra width came from the regime-B run.

## 2. Port verdict (`BREG_PORT_SCORE.txt`)

| regime | port ring phase | car 1 | car 2 | car 3 | verdict |
|---|---|---|---|---|---|
| A | K = 0 (and 9 more of the 12 sweep phases) | PASS | FAIL: c0# 39, c1# 75, steer# 113, c1~ 5.5, abs~ 44.5 | PASS | **2/3** |
| B | K = 1, 4 | no labelled envelope | | | not scored |
| C | none in K = 0..11 | | | | not scored |

The A verdict now cannot move with the ring phase: every A-phase run of the port gives the same window
(`RESULT_BSCORE.md` B3).

## 3. Owed (filed)

- **A labelled regime-B original run.** P(no B in 7 runs) is not small at the observed rates (0 of 7
  here, 1 of 4 inferred before). Extend the capture past 8, or seek a deterministic route to B on the
  original.
- **A port phase in regime C**, to score car 3 against run 2. Extend the K sweep past 11.
- Port car 2 in regime A is the standing (b) failure (5 items), unchanged by the rule.

## 4. Raw

`PREREG_BREGIME.md`, `capture_breg.sh`, `label_regime.py`, `BREG_CAPTURE.txt`,
`o_breg_r{1..8}.{poll.csv,msd.aistep.csv,msd.provenance.json,log}` (r6 did not reach GO),
`BREG_R1R2.txt`, `BREG_ENVELOPES.txt`, `BREG_PORT_SCORE.txt`, `re/tools/b_envelopes.{py,json}`,
`re/tools/ai_ctrl_window.py --check-regime`.
