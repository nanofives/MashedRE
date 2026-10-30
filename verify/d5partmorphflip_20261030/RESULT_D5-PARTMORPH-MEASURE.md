# RESULT D5-PARTMORPH-MEASURE: the D-11077 flip holds every default gate, run13 rows unchanged

Verdict: **GREEN**. Built with `MASHED_FLIP_D11077=1` (MASHED_PARTMORPH + MASHED_CAR_STATE0 seeded ON), the exe gives
G-CTRL `8284be46 / ba407a16 / 31fe043e`, B `1d5bf4f9 x3`, D2 3/3 in the O42 band, and a 13-track table byte-identical to
`run13_reference` (`verify/run13p3_20261029/tracks_score_sub_p3.txt`, 2431afb1). **No run13 row moved**, so the reference
is not re-based. Per L-62 (b) (Mariano 2026-10-30) the decided flip is committed on this branch.

Branch `orch/D5-PARTMORPH-MEASURE` off `orch/integration` @ `c36bbf631` (D5-PARTMORPH-FLIPPREP merged). LIVING-PC, 2026-10-30.
Private roots `D:/MashedWT/D5-PARTMORPH-MEASURE_root` (arms) and `..._root13` (run13). No MASHED.exe launched, no .asi
deployed, no .ps1 edited, no tracker touched, merge_train.json not edited.

## 1. Exe and runner

- Exe: worktree build, `MASHED_NO_DEPLOY=1`, `MASHED_FLIP_D11077` 0 -> 1 in `Core/FaithDefaults.cpp` (the only code
  change), rva-lint NEW=0. `mashed_re_on.exe` SHA-256 `cb1084f520ef63f54117798baf54bf851eb657d5b8ecf57629eae7b0fc9796a4`.
  It differs from FLIPPREP's EXE_ON (2e37dc2f) because integration moved from `0cd4241d4` to `c36bbf631`.
  Static witness: the strings `PARTMORPH` / `CAR_STATE0` appear one more time in this exe than `P3_RESET_PAIR` does (3 vs 2),
  the extra copy being the `kFaith` row. FLIPPREP F2 measured that this define seeds both knobs at runtime (law logs).
- Runner `run_measure.sh` (this folder) = the merge train's Gate-Default + Gate-Full (`re/orchestrator/merge_train.ps1`
  Run-Arms / Run-D2Arms / Gate-Full, same scripts and arguments), run under one test-slot, arms through
  `mashed_lock.py run` (D2 `--solo`), run13 inside a `solo-begin` window. Quiet check before start: 1 test-slot holder outside
  Mashed (SimGolf boot tests), 0 at launch.
- After the measurement the flip commit also rewrites the comment lines that said "prepared, NOT flipped"
  (FaithDefaults.cpp, two TrackRenderer.cpp site comments). Comment-only: no line count changes, so codegen is the measured one.

## 2. Gates (outputs: `GATES_MEASURE.txt`)

| Gate | Arms | Result | Reference | |
|---|---|---|---|---|
| G-CTRL | H_DEF | 8284be46 / ba407a16 / 31fe043e | merge_train.json gctrl.ref | PASS (first run, no retry) |
| B | B_DEF_K0 / K1 / K26 | 1d5bf4f9 x3 | merge_train.json b.ref | PASS |
| (b) vsum | B arms | 3/3 arms 3/3 (all cars A PASS) | | PASS |
| (e) ai_speed_env | B_DEF_K0 | 3/3 (e) PASS, launch 1425.7 / 2052.5 / 2055.0 | e_launch | PASS, launch same |
| D2 | D2_DEFH_1..3, `score_d2_v2.py port` | drvmed 1917.10 x3, slip1500 0.1930 / 0.1930 / 0.1933, slip2000 0.2515 x3, all `in` | O42 band | 3/3 PASS |
| run13 | 13 tracks, stock budget (RUN13_PRE_FRAMES default 0) | table 2431afb1, `^NN_` rows diff empty, INSTRUMENT GATES: PASS | tracks_score_sub_p3.txt 2431afb1 | identical |

- D2 arm lengths 1640 / 1641 / 1639 motion_diag lines (min 1600), no rerun, no load bounce.
- run13: all 13 tracks passed on attempt 1.
- D2 numbers against earlier runs: drvmed 1917.10 equals D2_SUBH_3 in `verify/d2band42_20261029/SCORE_PORT.txt` (the SUBSTATE
  stack under the 42 s hold, which is what the default became at D3-SUBSTATE-FLIP). The O42 band is the gate, and all three are
  in it.

## 3. Why no run13 row moved

FLIPPREP section 4 named the risk: the flip draws parts 0x12 / 0x14 and LOD 0 on every car after a round end. The 13-track
table is unchanged byte for byte, so no draw that table scores changed. [UNCERTAIN] Whether any run13 shot window reaches a
round end was not logged in this step. The table says that, if one does, the flipped draws match what the table measures.
No visual parity capture of the flipped draws exists (FLIPPREP section 4 gap, unchanged).

## 4. What lands

- `mashedmod/src/mashed_re/Core/FaithDefaults.cpp`: `MASHED_FLIP_D11077 1` (D-11077 default ON, L-62 b). Opt-outs unchanged:
  `MASHED_NO_PARTMORPH=1`, `MASHED_NO_CAR_STATE0=1`, `MASHED_NO_FAITH=1` (FLIPPREP F3 measured them).
- Comment lines in `FaithDefaults.cpp` and `D3d9Render/TrackRenderer.cpp` updated to say flipped.
- Expected references after the merge, unchanged from today's `merge_train.json`: G-CTRL `8284be46 / ba407a16 / 31fe043e`,
  B `1d5bf4f9 x3`, D2 arms D2_DEFH_1..3 with `verify/d2band42_20261029/score_d2_v2.py`, `run13_reference`
  `verify/run13p3_20261029/tracks_score_sub_p3.txt` (2431afb1). No ref edit is needed.
- Merge-train pass kind: SOURCE, and FULL (it changes a default). The train's own full run is the confirming check.

## 5. Raw data (not committed), `D:/MashedWT/D5-PARTMORPH-MEASURE_data`

| File | SHA-256 |
|---|---|
| mashed_re_on.exe | cb1084f520ef63f54117798baf54bf851eb657d5b8ecf57629eae7b0fc9796a4 |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0 / K1 / K26 .step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/D2_DEFH_1.motion_diag.log | dea61fb24a373c2cb7500257d669a628bf9f29f123cfeb0296f6c92a1398d1e9 |
| arms/D2_DEFH_2.motion_diag.log | e078ab49cd1ef379535551d19679b49516dc609e8b712eea71219d8d292c4258 |
| arms/D2_DEFH_3.motion_diag.log | e1029fa7ee9a07e8036f2762b9b24e69bc31635c636fe6aef85cceac268e907a |
| tracks_score.txt | 2431afb1da35f95db9bb01535f071da5400d53fb2cc2550210465c0404be27c3 |
| RUNLOG.txt | 9fbf7561feac3787d35c125b8fe00baf488dd3931efaf5d793e7d6dc06e47255 |
| build.log | b2291c7c9ebc7de4721e766e6ceb307686aa35c96cea957b0d887e7e44806874 |
