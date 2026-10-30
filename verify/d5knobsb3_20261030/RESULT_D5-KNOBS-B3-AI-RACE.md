# RESULT D5-KNOBS-B3-AI-RACE (2026-10-30)

Verdict: **GREEN**. 7 of the 11 named knobs deleted (AI / race opt-outs), exe builds, rva-lint 0 NEW,
G-CTRL and B unchanged, (b) 3/3, (e) 3/3 with launch unchanged, D2 DEFH 3/3 in the 42 s band. 4 knobs left out
(section 1): the gate runner still sets them.

Step: third batch of class (b) knob deletions from `verify/d5knobsinv_20261030/RESULT_D5-KNOBS-INVENTORY.md`.
Branch `orch/D5-KNOBS-B3-AI-RACE` from `orch/integration` @ `e73711e7c`. No default flipped: every deleted read
was an opt-out of a path that runs when the variable is unset, so the default build executes the same code before
and after.

## 1. Audit, one line per knob (read site at `e73711e7c`)

Every deleted read is `std::getenv("MASHED_NO_<x>") == nullptr` (any value opts out, unset = ON), except
`MASHED_NO_A5_RACE_TIMER_RUN`, which opted out only on a non-empty value other than `"0"`
(`Ai_RaceTimerRunEnabled`, unset = ON). The four kept knobs read the same `!(no && no[0] && no[0] != '0')` form
(`MASHED_NO_AI_LUT` returns mask 5 when unset). `Core/FaithDefaults.cpp` seeds none of the 11 `MASHED_NO_*` names
(it seeds the positive `AI_COUNTDOWN_CLOCK`, `GRID_DIR`, `AI_LUT`, untouched). `AS` = `mashedmod/src/mashed_re/Ai/AiStandalone.cpp`,
`TR` = `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp`.
Script check = `grep -rlE "MASHED_NO_<name>\b"` over `scripts/`, `re/tools/`, `re/frida/`, `verify/`,
`re/orchestrator/` (`.py .ps1 .sh .bat .js .json`).

| Knob | Read site | Default | Set by a live script | In batch |
|---|---|---|---|---|
| MASHED_NO_A364_RESET | `Race/AiDifficultyReset.cpp:89` | ON | only closed `verify/d3_gatefire_20261008/run_h4.ps1`, `verify/fe_residuals_20261016/run_h4_wt.ps1` | yes |
| MASHED_NO_A5_RACE_TIMER_RUN | `AS:2237` (`Ai_RaceTimerRunEnabled`, callers `AS:2244`, `AS:2250`) | ON | only closed copies `verify/a5flip_20261026/run_arms.py`, `verify/colllut_20261026/run_arms.py` (not in the gate runner) | yes |
| MASHED_NO_WIRE_B2 | `AS:1140` | ON | only closed `run_h4.ps1` / `run_h4_wt.ps1` (as A364) | yes |
| MASHED_NO_SEEK414F00 | `Ai/AiTrackObjSeek.cpp:156` (`Seek414f00_Enabled`, callers `AS:1105`, `AS:1451`) | ON | none (text only: closed `verify/seek414f00_20261016/tracker_seek.py`) | yes |
| MASHED_NO_REFDIST | `Race/SpectatorDistances.cpp:370` | ON | only closed `run_h4.ps1` / `run_h4_wt.ps1` | yes |
| MASHED_NO_REFDIST_AFTER_AI | `Race/SpectatorDistances.cpp:333` (callers `TR:6304`, `TR:8738`) | ON | only closed `verify/d3_pathprog_a_20261016/run_pathprog_a.py`, `verify/d3_u9683_20261016/run_u9683.py`, `verify/d3_u9685_20261016/run_u9685.py` | yes |
| MASHED_NO_RACEPCT_FUN408610 | `Race/PathNodes.cpp:347` (`RacePctOrig_Enabled`, 9 callers) | ON | only closed `run_pathprog_a.py`, `run_u9685.py` | yes |
| MASHED_NO_AI_LUT | `AS:155` | ON (mask 5) | **yes**: `verify/savelink_20261023/run_arms.py:160` stacks `NOCL5`, `NOL5` (arms `H_/B_/D2_NOCL5*`, `*_NOL5*`), plus 88 runner copies under `verify/` | **no** |
| MASHED_NO_AI_CLOCK_LATE | `AS:2164` | ON | **yes**: `run_arms.py:160` stack `NOCL5` | **no** |
| MASHED_NO_AI_COUNTDOWN_CLOCK | `AS:2208` | ON | **yes**: `run_arms.py:57` stack `NOCC`, `:439` stack `NCC` (arms `*_NOCC*`, `*_NCC*`) | **no** |
| MASHED_NO_GRID_DIR | `Race/RoundGrid.cpp:142` | ON | **yes**: `run_arms.py:158` stack `NOD2` (arms `*_NOD2*`) | **no** |

The closed runners named above are frozen evidence of lanes closed 2026-10-08 .. 2026-10-26. Nothing calls them and
no gate reads their output. Re-running one on this branch would make its opt-out arm identical to its default arm.
Left unedited (same treatment as B1 / B2).

Separate name, not in this batch: `run_arms.py:158` `NOD2` also sets `MASHED_NO_A5_RACE_TIMER` (no `_RUN`), read at
`Vehicle/VehiclePhysicsRun.cpp:1181`. Untouched.

## 2. Change

12 files, 33 insertions, 84 deletions (commit `5ee7e5476`).

- `Race/AiDifficultyReset.cpp`: `AiDifficultyResetTickImpl` always calls `AiDifficultyResetTail`.
- `AS` WIRE_B2: the branch-2 block tests `mode == 0` only.
- `AS` A5 timer: `Ai_RaceTimerRunEnabled` deleted (decl in `Ai/AiStandalone.h`); `Ai_RaceTimerRoundStart` keeps the
  round-1 test, `Ai_RaceTimerCountdownTick` runs unconditionally.
- SEEK414F00: `Seek414f00_Enabled` deleted (`Ai/AiTrackObjSeek.cpp/.h`); both mode-10 call sites test the course
  index 0x21 only.
- `Race/SpectatorDistances.cpp`: `ComputeDistancesTickImpl` always computes. `RefDistAfterAiOn`, `RefDistAfterAi`
  and the extern `RaceRefDistAfterAi` deleted, with the one-shot `[U-9683] ...` line it wrote to `mashed_re.log`
  (no script reads it: grep over `verify/ re/tools re/orchestrator scripts` = 0). `TR:6304` calls the producer
  when `gates_` is non-empty, the UpdateRace site-0 call (`TR:8738`, dead under the default) is gone, both
  externs removed.
- RACEPCT: `RacePctOrig_Enabled` deleted (`Race/PathNodes.cpp/.h`). Each caller drops the term with its default
  value `true`: `TR:3288` course-block build runs unconditionally, `TR:6299` / `TR:8745` test the publish mask only,
  `RacePctOrig_Active` = block exists, `PathNodes_Build` loses its early return, `Race/PathProgStep.cpp` 225 / 233 /
  313 test `PathProg_Enabled()` only, `Race/RoundGrid.cpp:49` keeps the mask term. The `[U-9685]` log line keeps its
  text minus the opt-out mention.
- Comments that named a deleted knob as a live opt-out now say it was deleted.

Built exe check (`grep -ao "MASHED_NO_<x>\0"` on the new `mashed_re.exe`): the 7 deleted names occur 0 times,
the 4 kept names once each.

## 3. Gates

Build: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` = `=== Build OK ===` (shared `.asi` not deployed).
rva-lint: `scripts/lint_rva_bodies.py` rc 0, `16 finding(s): CROSS-TARGET=16 | allowlisted=16 NEW=0`.

Arms: `verify/savelink_20261023/run_arms.py`, private root `D:/MashedWT/D5-KNOBS-B3-AI-RACE_root`, under
`mashed_lock.py run` (D2: `run --solo`) and `test-slot`. Refs from `re/orchestrator/merge_train.json`:
G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9 x3`, `full.e_launch` `1425.7 / 2052.5 / 2055.0`.

| Gate | This exe (`mashed_re.exe` sha256 `b21b9929`) | Ref | |
|---|---|---|---|
| G-CTRL H_DEF step / gates / ptrace | `8284be46` / `ba407a16` / `31fe043e` | `8284be46` / `ba407a16` / `31fe043e` | same |
| B_DEF_K0 / K1 / K26 step | `1d5bf4f9` / `1d5bf4f9` / `1d5bf4f9` | `1d5bf4f9` x3 | same |
| (b) `vsum.py` on the 3 B arms | 3/3 arms 3/3 | 3/3 | same |
| (e) `ai_speed_env.py --check B_DEF_K0` | 3 `(e) PASS`, launch 1425.7 / 2052.5 / 2055.0 | 3, same launch | same |
| D2 DEFH_1..3, `verify/d2band42_20261029/score_d2_v2.py port` | drvmed 1917.10 / 1919.62 / 1919.62, slip1500 0.1933 / 0.1930 / 0.1930, all `in`, H1 398/400; motion_diag 1639 / 1642 / 1642 lines (min 1600, no re-run) | 3/3 in band (D5-INTROMOVIES-FLIP 2026-10-30: drvmed 1920.25 / 1917.10 / 1919.62, H1 398/400) | same (3/3) |

All six gate arms and the three D2 arms ran on the first attempt (no re-run, no piz collision). Committed copies: `score_d2.txt`, `vsum_b.txt`, `e_check.txt` in this folder. The 13-track table was not run: this step touches no render or track file (the train runs it under `-Full`).

## 4. Raw files (not committed)

Folder `D:/MashedWT/D5-KNOBS-B3-AI-RACE_data/` (sha256):

- `arms/H_DEF.step.csv` 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198
- `arms/H_DEF.gates.csv` ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb
- `arms/H_DEF.ptrace.log` 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3
- `arms/B_DEF_K0.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K1.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/B_DEF_K26.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `arms/D2_DEFH_1.motion_diag.log` e1029fa7ee9a07e8036f2762b9b24e69bc31635c636fe6aef85cceac268e907a
- `arms/D2_DEFH_2.motion_diag.log` e11be3a771fc58879c1f7d61502ba7ddca581a7e7c84650fec84ceaff9ba0c6f
- `arms/D2_DEFH_3.motion_diag.log` e11be3a771fc58879c1f7d61502ba7ddca581a7e7c84650fec84ceaff9ba0c6f
- `score_d2.txt` bab1c5817ec48805c1d149ec577e4eb638214f4fb103b321b3bf3fbb81fb5bd5
- `vsum_b.txt` 6b0c8feada572b7600d357e6cc3ee1302fa5bb25d59b34fbb6d4f01a9ee2956a
- `e_check.txt` 7a42f73457b0e6e41e0acc615f2d2e8ac4460fc314236a1231afae2727e514b8
- `build.log` 7ea54de7fd7521f9faae8b5164e05be6a009116ad3e64254730fb359748f21af
- `lint.txt` e9dd7a2547772d2722eb778f62fb83ad83e8923d9a9fe732eaf3f772c2aa42c2
- `arms_run.log` e7e6f149223b6d9e9dcb83b3852c32bacaee3ad720e94e9fcaa189be89a3ea43
- `d2_run.log` 2583858de4938f75708d628b5c4d755959e22c08cbd162cd569d495d5bc1fcba
- exe `mashedmod/build/mashed_re.exe` b21b9929a2980b61d315ada16e616a888d9f2f6a9b9e9f7489c02650c50996a7

## 5. For the lead

- No `.ps1` edit needed.
- Left out: `MASHED_NO_AI_LUT`, `MASHED_NO_AI_CLOCK_LATE`, `MASHED_NO_AI_COUNTDOWN_CLOCK`, `MASHED_NO_GRID_DIR`.
  Setter: the gate runner `verify/savelink_20261023/run_arms.py` stacks `NOCC` (:57), `NOD2` (:158), `NOCL5` / `NOL5`
  (:160), `NCC` (:439). They can go once those report-only stacks are retired from the runner (a run_arms change the
  lead owns). `MASHED_AI_LUT=<mask>` (`AS:153`) stays a separate override either way.
