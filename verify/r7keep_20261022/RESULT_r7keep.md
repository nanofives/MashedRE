# RESULT r7keep: rule 7 keeps the score-scan winner (0x00410510) (2026-10-22)

Branch `work/r7keep` from `e7f4de987`. PREREG `PREREG_r7keep.md` committed first (`ad2bba53d`), code after (`56aef576c`). Exe
`b20134fe`. Private roots `D:\MashedWT\r7keep_root` / `r7keep_root13`; evidence `D:\MashedWT\r7keep_data` (`MOVED_OFF_GIT.txt`).

**Verdict: every prediction held (P0-P4).** The knob makes `EvaluateResult` match `0x0041075a..0x00410798` and changes nothing in
any natural run on either side, which section 3 of the PREREG predicted from reachability. Per the decision rule, the
recommendation is to flip `MASHED_RULE7_KEEP` default ON with an opt-out. That is Mariano's call.

## What changed

- `Race/RuleEngine.{h,cpp}`: `Persist::rule7KeepWinner` (default false). With it, case 7 does not clear the score-scan winner; a car
  with metric above 3.0 still overrides it. Optional `Rule7Trace*` out-param (scan winner, metric winner, returned value).
- `D3d9Render/TrackRenderer.cpp`: `MASHED_RULE7_KEEP` sets the field before every `EvaluateResult`; `MASHED_RULE7_TRACE=<file>`
  writes one line per rule-7 call with running `n_eval` / `n_div` (scan winner set and no metric winner).
- `Race/tests/ruleengine_test.cpp`: 10 seeded checks (P3).
- `re/frida/scenario_launch.py` oracle: `predEval(en, keep)`; every `FUN_00410510` call is scored against both laws
  (`ev.keepAgree`, `ev.lawsDiffer`, a `misrec` row whenever the two laws differ).

## Predictions

| id | arm | result |
|---|---|---|
| P0 | knob OFF, recipe F | **held.** G-CTRL `c3bfcef3 / 4ed4849b / 4c3548a1` x2; B `4985eb3b / b6e3d073 / 6717c15e`; (b) 9/9 (`vsum_b_def.txt`); (e) PASS 3/3, launch 1425.7 / 2052.5 / 2055.0 (`e_check_def.txt`); D2 PASS 3/3, slip 0.1900 / 0.2550 x3, drvmed 1963.95 / 1965.35 / 1963.95, H1 398/400 (`score_d2_def.txt`); 13 tracks table identical to dcmix, gates PASS (`tracks_def_score.txt`) |
| P1 | knob ON, recipe F (rule 0) | **held.** `H_K-1` `c3bfcef3 / 4ed4849b / 4c3548a1`; B_K `4985eb3b / b6e3d073 / 6717c15e` |
| P2 | rule 7, OFF vs ON, trace | **held.** `H_R7-1` and `H_R7K-1` both `8c0f42fe / 55ef9552 / ee2b21d8`; trace `n_eval=1 n_div=0` on both (car 4 crossed 3.0, scores 6,6,6,6, all alive, returned 4) |
| P3 | `ruleengine_test` | **held.** ALL PASS, 0 failures; the 10 r7keep checks: OFF -> 0 (trace scan 3, metric 0); ON -> 3, `p0Won` false; metric winner car 1 still wins (2); car 0 scan winner -> 1, `p0Won` true; nothing -> 0 |
| P4 | stock original, `--oracle --rule 7`, 120 s | **held.** 4 cars: SegmentCheck 2922 / 0 mis, EvaluateResult 1 / agree 1 / keepAgree 1 / lawsDiffer 0 (returned 4, scores 6,6,6,6). 2 cars: SegmentCheck 2713 / 0 mis, EvaluateResult 1 / 1 / 1 / 0 (returned 2, scores 4,4). Logs `D:\MashedWT\r7keep_data\oracle\c4.json`, `c2.json` |

Both sides ended their rule-7 race the same way (a car crossing metric 3.0, original `ev got 4` with 4 cars, exe `ret=4`).

**What the evidence does not show:** the divergent branch (segment end with no metric above 3.0 while a score sits at the target)
has no natural witness on either side. Its evidence is the disassembly (PREREG section 1) plus the seeded unit test (P3). Scores did
not move off the seed in any rule-7 run (6 with 4 cars, 4 with 2), as section 3 predicted.

## Decision for Mariano

1. Flip `MASHED_RULE7_KEEP` default ON (opt-out `MASHED_NO_RULE7_KEEP`)? No default hash or score moves (P0, P1), so the flip
   needs no rebase of any reference.
2. Then `0x00410510`'s exe body matches the .asi body's rule-7 law; the remaining split between `RuleEngine.cpp` and
   `ScoringHooks.cpp` (the end-block side effects, live inputs) is input plumbing, the dual-copy finding stays allowlisted.
3. Not merged.
