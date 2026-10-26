# PREREG D3-CF-COLLLUT (L-18): MASHED_COLL_LUT re-measured on the A5 race-timer default (2026-10-26)

Branch `orch/D3-CF-COLLLUT` from `orch/integration` `992687ebc` (`4fd6191c5`, the D3-CF-A5TIMER-FLIP merge, is an ancestor).
No source change. `MASHED_COLL_LUT=1` (`Collision/ContactDeps.h:40`, default OFF) is the knob. The five u9774 knobs are default
ON since f5flip (`Core/FaithDefaults.cpp:36`), so `CL` on this exe is the faithful stack CLSF4 + the race timer
(D3-CF-A5TIMER's `D2_CLSF4RN_1`). This step does not flip the default. The flip comes back as a decision row.

Runner `verify/colllut_20261026/run_arms.py` (a5flip's, paths changed, adds `CLNORN` = `CL` + `MASHED_NO_A5_RACE_TIMER_RUN`).
Root `D:\MashedWT\D3-CF-COLLLUT_root`, 13 tracks on `D:\MashedWT\D3-CF-COLLLUT_root13`, raw data `D:\MashedWT\D3-CF-COLLLUT_data`.

## Arms

| arm | stack | prediction | source of the prediction |
|---|---|---|---|
| `H_DEF`, `B_DEF_K0/K1/K26` | default | `5943c5f1/b493262b/09551e6a`, `3a03670c/2516d903/53778664` | merge_train.json refs |
| `H_CL`, `B_CL_K0/K1/K26` | COLL_LUT | measured, reported as the expected flip refs | none (u9774 had different defaults) |
| `D2_DEF_1..3` | default | 0.1874 / 0.2488 / ~1929, in band | a5flip `D2_DEF_1..3` |
| `D2_CL_1..3` | COLL_LUT | 0.1942 / 0.2492 / 1896.74, in band 3/3 | a5timer `D2_CLSF4RN_1`, u9775 `D2_CLSF4A_1..3` |
| `D2_CLNORN_1..3` (control, Mariano) | COLL_LUT + timer opt-out | 0.1939 / 0.2537 / 1891.51, drvmed OUT (floor 1893.63) | u9775 `D2_CLSF4RT_1` |
| `D2_NORN_1` | timer opt-out | 0.1900 / 0.2550 / 1965.35 | a5flip `D2_NORN_1` |
| 13 tracks, `MASHED_COLL_LUT=1` | COLL_LUT | measured against `verify/rpct_20261022/tracks_flip_score.txt` with `cites_rres.json` | none |

D2 runs in the solo window (`mashed_lock.py run --solo`), scorer `verify/d2rebase_20261019/score_d2_v2.py port`, an arm under
1600 motion_diag lines is re-run (up to 2 times).

## Verdict rule

- Default gates unequal to the refs: RED (the exe is not the integration default).
- `D2_CL` 3/3 in band, 13 tracks identical with INSTRUMENT GATES PASS: DECISION (flip recommended), with `H_CL` / `B_CL` as the
  new refs.
- `D2_CL` 3/3 in band but the 13-track table moves: DECISION, with the moved rows listed.
- `D2_CL` under 3/3: DECISION (flip not recommended) or RED if a gate breaks.
