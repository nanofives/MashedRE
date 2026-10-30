# RESULT D5-INTROMOVIES-FLIP: MASHED_INTRO_MOVIES default ON is hash-neutral on every default gate

Verdict: **GREEN, hash-neutral.** With `MASHED_INTRO_MOVIES` seeded ON, the exe gives G-CTRL `8284be46 / ba407a16 / 31fe043e`,
B `1d5bf4f9 x3`, D2 3/3 in the O42 band, and a 13-track table byte-identical to `run13_reference`
(`verify/run13p3_20261029/tracks_score_sub_p3.txt`, 2431afb1). No gate and no run13 row moved, so this merges in a normal
`-Full` source pass. No ref edit.

Branch `orch/D5-INTROMOVIES-FLIP` off `orch/integration` @ `009a74b2d` (D5-PARTMORPH-FLIPPREP and D5-PARTMORPH-MEASURE landed).
LIVING-PC, 2026-10-30. Decision L-63 (Mariano 2026-10-30), evidence before it: `RESULT_D5-INTROMOVIES-I1B.md` (this folder).
Private roots `D:/MashedWT/D5-INTROMOVIES-FLIP_root` (arms) and `..._root13` (run13), robocopies of `original\`. No
`MASHED.exe` launched, no `.asi` deployed, no `.ps1` edited, `merge_train.json` not edited.

## 1. The change

- `mashedmod/src/mashed_re/Core/FaithDefaults.cpp`: `{ "INTRO_MOVIES", "1" }` appended to `kFaith`. The site
  (`exe_main.cpp` `RunBootIntroMovies`, `IntroEnvInt("MASHED_INTRO_MOVIES") != 1` returns) is unchanged. The seed follows the
  FAITH rules: `MASHED_NO_INTRO_MOVIES=1` skips it, an explicit `MASHED_INTRO_MOVIES=0` wins, `MASHED_NO_FAITH=1` seeds nothing.
- `exe_main.cpp`: two comment lines that said "default OFF" now say default ON (comment only, line count unchanged).
- Built from the worktree with `MASHED_NO_DEPLOY=1`, rva-lint `NEW=0`. `mashed_re_on.exe` SHA-256
  `8e03a2db5c189267f1b3ef0e41d647e43074e42c103e320825eb33512189abe0`. The measured source is the committed source.

## 2. Gates (`GATES_FLIP.txt`, runner `run_measure.sh` = the PARTMORPH-MEASURE runner with this step's paths)

Quiet rule: the run waited until at most 2 test-slot holders sat outside Mashed (3 SimGolf holders from 11:17, quiet at 11:45:32).
Everything ran under one test slot, arms through `mashed_lock.py run` (D2 `--solo`), run13 inside a `solo-begin` window.

| Gate | Arms | Result | Reference | |
|---|---|---|---|---|
| G-CTRL | H_DEF | 8284be46 / ba407a16 / 31fe043e | merge_train.json gctrl.ref | PASS, first run |
| B | B_DEF_K0 / K1 / K26 | 1d5bf4f9 x3 | merge_train.json b.ref | PASS |
| (b) vsum | B arms | 3/3 arms, all cars A PASS | | PASS |
| (e) ai_speed_env | B_DEF_K0 | v1..v3 PASS, launch 1425.7 / 2052.5 / 2055.0 | e_launch | PASS, launch same |
| D2 | D2_DEFH_1..3, `score_d2_v2.py port` | drvmed 1920.25 / 1917.10 / 1919.62, slip1500 0.1934 / 0.1933 / 0.1933, slip2000 0.2515 x3, all `in` | O42 band | 3/3 PASS |
| run13 | 13 tracks, RUN13_PRE_FRAMES default 0 | 2431afb1, `^NN_` row diff empty, INSTRUMENT GATES: PASS | tracks_score_sub_p3.txt 2431afb1 | identical |

- D2 arm lengths 1636 / 1639 / 1638 motion_diag lines (min 1600), no rerun, no load bounce. run13: 13/13 on attempt 1.
- D2 drvmed spread is wall-clock spread, not a move. The arms are wall-clock bound (`MASHED_DRIVE_HOLD_MS=42000`). The hold
  clock `el` counts from InRace entry (`exe_main.cpp:1740-1741`, checked at `:1787`), so the boot movies before the frontend do not shorten the window.
  1917.10 and 1919.62 both occur on the same default stack before this flip (`verify/d2band42_20261029/SCORE_PORT.txt`,
  D2_SUBH_1..3: 1919.62 / 1919.62 / 1917.10), and this run's D2_DEFH_2 `motion_diag.log` is byte-identical to
  D5-PARTMORPH-MEASURE's D2_DEFH_3 (`e1029fa7`). 1920.25 is in the band. The D2 gate is the band, and it holds.

## 3. The movies ran in every measured boot

Every gate log (`H_DEF`, `B_DEF_K0/K1/K26`, `D2_DEFH_1..3`) and the run13 root log carry 4 `INTRO movie ... complete` lines
plus `INTRO small.mpg: BUILT (graph built, not run)`. The roots hold the 167,414-byte skip files (`patch_mashed_skip_intro.py`,
real movies in `MOVIES\backup\`), so each movie completes in 2-3 frames (78..172 ms), as in I1B's I3 arm. This is why no gate moved:
the gate scenarios boot through ~0.4 s of skip movies, then the frontend and race paths run as before.

Opt-out boots (20 s each, same exe, private root, inside the lock, PIDs spawned and killed by the script):
`MASHED_NO_INTRO_MOVIES=1`, `MASHED_INTRO_MOVIES=0` and `MASHED_NO_FAITH=1` each log 155 lines with 0 INTRO lines.

## 4. Trackers (re-classify skill)

- **D-11062** struck with `re/tools/deferred_close.py close` (CHANGELOG line written by the helper).
- **U-9799** filed (semantic): boot movie audio untested. Every I1 run logged `audio_hr=0x80040256` on empire / supersonic /
  intro and `0x00000001` on renderware, LIVING-PC has no audio endpoint. Path: run the I1 arm with the real movies on a machine
  with an audio endpoint, then resolve with `re/tools/uncertainty_resolve.py`. Blocks nothing. CHANGELOG line prepended.
- No `hooks.csv`, `STUBS.md` change.

## 5. What lands, and what it does not change

- Merge-train pass: SOURCE, FULL (it changes a default). Expected refs after the merge, unchanged from today's
  `merge_train.json`: G-CTRL `8284be46 / ba407a16 / 31fe043e`, B `1d5bf4f9 x3`, D2 arms D2_DEFH_1..3 with
  `verify/d2band42_20261029/score_d2_v2.py`, `run13_reference` `verify/run13p3_20261029/tracks_score_sub_p3.txt` (2431afb1).
- Residue for the lead: the LAN release stages `original\` minus `PC\MOVIES\backup\` (`scripts/lan_release/stage_lan_release.py`),
  and the shared `original\` holds the skip files. So player PCs get the flip but play the 167,414-byte skip files, not the
  real logo movies. Shipping the real movies is a staging change (out of scope here, no file touched).

## 6. Raw data (not committed), `D:/MashedWT/D5-INTROMOVIES-FLIP_data`

| File | SHA-256 |
|---|---|
| mashed_re_on.exe | 8e03a2db5c189267f1b3ef0e41d647e43074e42c103e320825eb33512189abe0 |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0 / K1 / K26 .step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |
| arms/D2_DEFH_1.motion_diag.log | a3e605030dfc65e6e36e34f012e93c2755b49f5900e3843d65cdf34eaf4ac1e9 |
| arms/D2_DEFH_2.motion_diag.log | e1029fa7ee9a07e8036f2762b9b24e69bc31635c636fe6aef85cceac268e907a |
| arms/D2_DEFH_3.motion_diag.log | fe38081b72afd4ea1ad2c44aa451677598a7c7368bffedb4b6099b13c7042d84 |
| arms/H_DEF.log (INTRO lines) | 87c9023c63d635b0e89eb973202aefa2b44581bea49ad58f8afbe0e2caa2c63e |
| tracks_score.txt | 2431afb1da35f95db9bb01535f071da5400d53fb2cc2550210465c0404be27c3 |
| RUNLOG.txt | 402f25054fa45dc4365665a24e008a35c151cf3773a0be2a3049b6176c7e27e0 |
| build.log | e68a99e804b2688fd92c5e997cc2de2b3d3c51b748ecec5dad35bd6996712841 |
| optout.txt | 3161c398233ed5177a6e9ed9c40874c4ed968b54b50f5d7e4d990ef73480a535 |
| optout/NO_INTRO.log, INTRO0.log, NO_FAITH.log | c463c29e0d859fcd304195cd1ebdeafdea8e45386247a7a3c0104b9c6aa8b88a (all three) |
| quiet.txt | b50baf82b44861cd18c0b10d95beebf505520faec7b8542b5547ceb1295684d9 |
