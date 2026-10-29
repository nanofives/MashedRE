# PREREG: D3-SUBSTATE-FLIP, run 22 (D-11073, user decisions L-53 / L-55 / L-57)

Date 2026-10-29. Committed alone, before the seed and before any arm of this run.
Branch `orch/D3-SUBSTATE-FLIP` @ a8bc95b59 = run 20's branch (amendment 1 + `MASHED_SUBFLIP_DIAG`, both knob-gated)
+ `orch/integration` f6033d936 (PREGO-ALIVE, SETTLE-ORDER, ROW315, the L-56 PREGO + ICONCUBE flip)
+ `orch/D2-BAND-42S` 5df890780 (merge-queued, not yet on integration: the O42 band and the `DEFH` runner stack).
Run 20's evidence stays in `verify/d3subflip_20261028/`.

## 1. The change (one TU)

`mashedmod/src/mashed_re/Core/FaithDefaults.cpp`: seed `MASHED_SUBSTATE=1` and `MASHED_P3_RESET_PAIR=1` in `kFaith`
(the same pair as stack `SUB`, which every H_SUB / B_SUB / D2_SUBH measurement used). Opt-outs, per the TU's own rules:
`MASHED_NO_SUBSTATE=1`, `MASHED_NO_P3_RESET_PAIR=1` (runner stack `NOSUB` = both), `MASHED_NO_FAITH=1`. No other source
edit. Standalone only (the .asi never compiles this TU).

Runner (`verify/savelink_20261023/run_arms.py`, a .py, not a .ps1): a D2 arm whose stack sets `MASHED_DRIVE_HOLD_MS` above
30000 gets a 110 s wall by default (the D2-BAND-42S / SETTLE-ORDER wall for the 42 s hold). `A4_D2_WALL` still wins.
Arms without the hold keep 90 s, so `D2_DEF_n` is unchanged.

## 2. Predictions (all on one exe, the flipped build of this branch)

| id | arm(s) | prediction | source |
|---|---|---|---|
| P1 | `H_DEF` | G-CTRL step / gates / ptrace = `8284be46 / 2b19135b / 31fe043e` | ROW315 H_SUB on exe 58c21e05 |
| P2 | `B_DEF_K0 / K1 / K26` | `1d5bf4f9` x3 | PREGO-ALIVE B_SUB, run 20 flip 2 |
| P3 | `H_NOSUB`, `B_NOSUB_K0 / K1 / K26` | the current refs exactly: G-CTRL `5943c5f1 / 306186d4 / 52052890`, B `3a03670c / 9fcdbd3a / 53778664` | merge_train.json |
| P4 | `D2_DEFH_1..3` (42 s hold, 110 s wall) | in the O42 band (`verify/d2band42_20261029/band.json`) on all 3 metrics, 3 of 3; values = D2_SUBH (0.1934 / 0.2515 / 1919.62, one run 1917.10); motion_diag >= 1600 lines | D2-BAND-42S SCORE_PORT.txt |
| P5 | 13-track table (`run13.py run` + `score --cite verify/rres_20261016/cites_rres.json`) | report every row that differs from `verify/pregocubeflip_20261029/tracks_score.txt`. No prediction of which rows move: the recipe's frames are absolute (`max shot + 350`) and the flip adds ~696 pre-race frames | run 20 section 6 |

If P1 or P2 give other values, the measured values are what this step records (the parent asked for "what the merged tree
gives"), with the reason when it can be shown. P3 failing = RED (the opt-out must reproduce the refs).

## 3. Verdict rule

- GREEN: P3 holds, P4 holds (3/3 in band, no arm under 1600 lines after at most 2 reruns), G-CTRL and B are
  deterministic (H_DEF twice, B once more on K0), and every moved 13-track row scores CITED (0 RED).
- DECISION: any 13-track row RED, or anything changes beyond the G-CTRL / B hash move and the 13-track rows.
- RED: P3 fails, or P4 fails. BLOCKED: a run cannot be made.

## 4. Deliverables

`verify/d3subflip_20261029/expected_refs.json` (new G-CTRL / B refs, the D2 recipe, run13_reference if it moves), RESULT,
scored tables and hash lists here; raw data in `D:/MashedWT/D3-SUBSTATE-FLIP_data/run22/`. The exact `merge_train.json`
change and any `merge_train.ps1` change are described in the RESULT, not applied (no .ps1 edit).
