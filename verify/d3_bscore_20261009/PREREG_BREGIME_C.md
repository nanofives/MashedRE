# PRE-REGISTRATION: find a port ring phase in regime C (and test a roll model), committed before the sweep

Date 2026-10-09. Follows `RESULT_BREGIME.md` §3.

**Model (`predict_regime_k.py`).** The band-0 roll (`AiStandalone.cpp:1705-1716`, row 2 band 0:
`prob1` = 20, `prob2` = 10, from `kDiffProb1/2[10]`) takes r1; r1 <= 20 gives B. Otherwise it takes r2;
r2 <= 10 gives C, else A. Assumption under test: the number of ring draws before the roll does not
depend on the draws' values, so at ring offset K the roll reads draw `n0 + K` (after the 310 open
discards). Three n0 reproduce the observed K = 0..11 (B at 1 and 4, A elsewhere): 0, 926, 2642.
**n0 = 0 is the prior favourite.** It means the roll is the first ring draw of the race in the
default build.

| n0 | predicted C in K = 12..63 | predicted B in K = 12..63 |
|---|---|---|
| **0** | **26, 54, 57** | 27, 36, 47, 48, 55, 56, 58, 59, 60 |
| 926 | 46 | 28, 29, 36, 37, 39, 47, 54, 62 |
| 2642 | 19, 28, 61 | 14, 18, 20, 21, 22, 29, 32, 42, 46, 55, 59, 62 |

**Sweep:** `run_bscore.ps1 12..63` (default build, 1200 frames), regimes labelled by
`ai_ctrl_window.py window_regime` (first non-zero `flag_a368` in the window).

| # | prediction | refuted if |
|---|---|---|
| C1 | the 52 labels match ONE candidate's table exactly (expected: n0 = 0) | no candidate matches |
| C2 | at least one K lands in C (expected 26, 54, 57) | none does |
| C3 | every C phase gives identical per-car windows (as A and B did, `RESULT_BSCORE.md` B3) | two C phases differ |

If C2 holds, the port is scored in C against `b_envelopes.json`'s C envelope (original run 2, car 3
only; n = 1, reported as thin).
