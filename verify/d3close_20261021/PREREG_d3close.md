# PREREG d3close: re-score every D3 gate on the soflip default (2026-10-21)

Branch `work/d3close` from `665951c20` (work/soflip merged). Committed before any arm runs. No code change, no knob.
Build: `MASHED_NO_DEPLOY=1 mashedmod\build.bat` in `.worktrees/d3close`, exe `053df088` (soflip's own build was `50558a73`,
same source at a different worktree path; the behavioural hashes below are what must match, not the exe hash). Private root
`D:\MashedWT\d3close_root` (fresh copy of `original/`: 265 files, 506,063,661 bytes, equal to the main checkout). Evidence in
`D:\MashedWT\d3close_data` (`MOVED_OFF_GIT.txt`), never committed. Runner `run_arms.py` (soflip's, paths changed, adds the
`NOPC24` bisect stack and the report-only `SR` stack). (b) summary `vsum.py` (soflip's copy).

Default under test: FAITH stack + u9774 five + `WCS_TRINORMAL` + `WCS_COMBINE_PC24` + `A5_RUBBER_LIVE` + `MODE_LIVE` +
`SLOT_TYPE_LIVE` + `SUBSTEP_OUTER` (`Core/FaithDefaults.cpp`), recipe F (`MASHED_ROUND_RULE=0 MASHED_GAME_MODE=10`).

## 1. Gate table to re-score (ROADMAP.md §D3)

| # | criterion | scorer | recipe / arm | last recorded value (source) | prediction |
|---|---|---|---|---|---|
| A-a | `FUN_00443300`, `FUN_00443dc0` ported, not stubbed | code check, cite file:line | exe TU list `mashedmod/exe_sources.rsp` | met (ROADMAP 2026-10-09 gate table) | met |
| A-b | ctrl-byte envelope per regime | `vsum.py` = `ai_ctrl_window.check_regime` on `re/tools/b_envelopes.json` | `B_DEF_K0 / K1 / K26` (1200 frames, `MASHED_RING_SKIP`) | A 3/3, B 3/3, C 3/3 (`verify/soflip_20261021/vsum_b.txt`) | **9/9**, step hashes `9bd86ffe / 433abcd2 / 1a1ff727`, flag_a368 0 / 0,1 / 0,2 |
| A-c | opponents driven by ported ctrl bytes, no scaffold motion model | code check | `D3d9Render/TrackRenderer.cpp` | met (2026-09-26) | met |
| A-d | `MASHED_AI_TICK` deleted | code check (no `getenv`) | source tree | met | met |
| A-e | launch and `ft_median_m0` within ±2% of ref on cars 1..3 | `re/tools/ai_speed_env.py --check` | gating: `B_DEF_K0` step dump (the only phase with flag_a368 0 on every car); second witness: `H_DEF` step dump | PASS 3/3 on 2026-10-09: launch 1426.4 / 2053.0 / 2055.2, ft_median_m0 2550.6 / 2053.0 / 2278.2 (`verify/d3_b2flip_20261009/FLIP_SCORE.txt`, old rule-4 recipe) | **PASS 3/3**. Values are not predicted to the digit: (e) has not been scored since 2026-10-09, and the FAITH stack, u9774, WCS, A5 rubber, MODE / SLOT_TYPE and SUBSTEP_OUTER flips and the recipe change to F all landed after it |
| P-a | dispatcher ticks every frame | code check | `Powerup/PowerupSystem.cpp` caller | met (2026-09-26) | met |
| P-b | slots 1..3 own and fire | code check + recorded live run | | met (2026-09-27) | met |
| P-c | 9 types decision clean; contact outcomes | `re/tools/pu_replay/sweep.ps1` (pu_replay built from this tree's `Powerup/*.cpp` + `pu_diff.py`) | the 11 captures | decision CLEAN 11/11, contact CLEAN on c2 c3 g2 g4 m1 m2 s1 s2, g3 DIVERGES on R_FLAME query `0x45afcc` only (546 vs 548), o3 / o4 have no contact channel (`verify/d3_box_20260928/sweep.txt`; guard lines in ROADMAP 2026-09-29b and 2026-10-03) | same per capture |
| M-a | live oracle, all 11 rules | `re/frida/scenario_launch.py --oracle` on the stock original (JS law = `Race/RuleEngine.cpp`) | one run per rule, §3 | 0 mismatches on every rule (`re/analysis/D3_MODES_2026-09-26.md` §1; rule 0 1448/1448 2026-09-29; rule 3 newest 2478/2478 2026-10-03) | GREEN, 0 mismatches on every rule |
| M-b | APPEND fires | same, `appends` | rules 4 / 7 / 8 / 9 | 1 each | >= 1 on each of 4 / 7 / 8 / 9 |
| M-c | round target from the rule | code check | `Race/RuleEngine.cpp` / `RaceModes.cpp` | met | met |
| M-d | rule engine default-on | code check | | met | met |
| G-D2 | D2 band | `verify/d2rebase_20261019/score_d2_v2.py`, whole log, 8-run band | `D2_DEF_1..3`, run ALONE | PASS 3/3: slip1500 0.1900, slip2000 0.2550, drvmed 1966.66 / 1965.67 / 1965.67, H1 398/400 (`verify/soflip_20261021/score_d2_def.txt`) | PASS 3/3, slip identical, drvmed inside 1964..1967 |
| G-CTRL | default identity | sha8 of step / gates / ptrace | `H_DEF` | `f78de789 / 43bbe0fb / 161b1a6a` (soflip x2) | same |
| G-13 | 13 tracks | `run13.py` | not re-run unless a step changes code | 877 rows 0 RED (soflip `tracks_def_score.txt`) | cited from soflip |

## 2. What cannot be re-measured as written, and how it is handled

1. **P-c: 4 of the 11 captures are gone.** `m1`, `m2`, `s1`, `s2` (`verify/d3_contact_20260928b/`) have no `.msd.puhook.csv`
   in git or on disk (searched the main checkout and `D:\MashedWT`). They were never committed, and the last sweep that read
   them wrote `re/tools/pu_replay/sweep_out/` on 2026-10-02. `sweep.ps1` prints `MISSING` and skips them. Handling: the 7
   present captures (`o3 o4 c2 c3 g2 g3 g4`) are replayed as written. The 4 missing ones are **re-captured on the original with
   their recorded argv** (from each `.msd.provenance.json`) as `m1r m2r s1r s2r` into `D:\MashedWT\d3close_data\pu\`, then
   replayed through the same pu_replay. A re-capture is a new capture, not the same one, so it is reported per capture
   with the recorded verdict next to it, and a difference in event counts is expected. Prediction for each: decision CLEAN,
   contact CLEAN. `s2` exercised the MORTAR -> MISSILE box-state gate (the 181-mismatch defect). Whether `s2r` reaches box
   state 3 again is reported, not assumed: `b1` / `b2` (2026-09-28) never left state 1 naturally.
   Runner `run_pu.py` (this dir) drives the 4 captures + the sweep.
2. **M-a re-runs the original only.** The oracle hooks the stock original and checks the ported law transcribed in JS. It
   does not execute `mashed_re.exe`. `Race/RuleEngine.cpp` changed once since 2026-09-26 (`19e95b2f2`, 2026-10-16,
   `CollectAllDone` now calls the one-body `CollectDoneBody405890`, same predicate). Handling: the oracle is re-run on all 11
   rules, AND the port's own unit tests (`Race/tests/ruleengine_test.cpp`, `racemodes_test.cpp`) are built from this tree and
   run. `MODE_LIVE` / `SLOT_TYPE_LIVE` change what the standalone feeds the rule engine (game mode word, slot types), not
   the engine's law. Their effect on the default is already inside G-CTRL / (b) / D2 above.
   Per-rule runs (base `--track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --oracle --hold 40`, `MASHED_MUTE=1`,
   `MASHED_ROOT` = the main checkout so the worktree's script finds the install, JSON to the worktree's `log/`):
   r0, r1, r2, r3, r6 base; r4 and r8 `--poke-lap 0:3`; r7 `--poke-lap 1:3`; r9 `--cars 2 --poke-lap 1:3`; r5 `--track 3
   --poke-collect 1:1`; r10exp `--mode 3`; r10win `--mode 3 --poke-lap 0:2`. 12 runs. Counts depend on wall clock and are
   not predicted. Only GREEN, `mis` = 0 and the segment-end / APPEND facts are.
3. **(e) recipe.** (e) was last scored on the `run_h4` step dump under the old rule-4 recipe. Recipe F is now canonical, and
   (e)'s window is the same 220 calls (b) uses, so the B_K0 step dump is the gating capture and H_DEF the witness. A car
   with `regime0=0` is not scored. It is re-taken on another K, or the run is re-taken.
4. **(e) scorer caveat (report only, no change).** (e) gates on `rec_9e4`. Memory `plus-0x9e4-is-not-comparable-port-vs-original`
   records that the original rewrites `+0x9e4` every frame after A4 (`FUN_00470c70` loop `0x004710a1..0x004710da`,
   `|+0x9b0|`) while the default port leaves A6a's value there (the refresh is opt-in `MASHED_SPEED_REFRESH`,
   `Vehicle/VehiclePhysicsRun.cpp:1707`). The step dump carries no `+0x9b0..+0x9b8`, so `|v|` cannot be scored. One
   report-only arm `B_SR_K0` (`MASHED_SPEED_REFRESH=1`) shows whether (e) is sensitive to that. It does not gate.

## 3. Arms

| arm | purpose | gates |
|---|---|---|
| `B_DEF_K0`, `B_DEF_K1`, `B_DEF_K26` | (b) per regime, (e) on K0 | yes |
| `H_DEF` | G-CTRL identity, (e) second witness | yes (identity) |
| `B_SR_K0` | (e) with `MASHED_SPEED_REFRESH=1` | report only |
| `D2_DEF_1..3` | D2, alone, after everything else | yes |
| pu sweep (7 + 4 re-captured) | P-c | yes |
| 12 oracle runs + 2 unit tests | M-a, M-b | yes |

**If (e) fails:** nothing is tuned. Report car and stat, then bisect on the K0 step dump over today's flips with their opt-outs:
`B_NOSX_K0` (`MASHED_NO_SUBSTEP_OUTER`), `B_OLDNOK_K0` (old rule-4 recipe + `MASHED_NO_MODE_LIVE` + `MASHED_NO_SLOT_TYPE_LIVE`),
`B_NR_K0` (`MASHED_NO_A5_RUBBER_LIVE`), `B_NOPC24_K0` (`MASHED_NO_WCS_COMBINE_PC24`), and `B_NF_K0` (`MASHED_NO_FAITH`) as the
outer bracket. The flip whose opt-out puts (e) back inside the band is named. If none does, the FAITH bracket says whether it
predates today.

**Crash rule:** an original-side crash at `MASHED.exe+0x1509b4` or `+0xe66db` is a piz collision (U-9677) and is re-run. A step
dump that is a byte-identical shorter prefix is re-run before a hash change is believed.

## 4. Deliverables

`RESULT_d3close.md` (gate table: criterion, scorer, value, band, verdict, evidence path) and `ROADMAP_D3_BLOCK.md` (draft "D3
closure state 2026-10-21" block). ROADMAP.md, CLAUDE.md and NEXT_SESSION.md are NOT edited: closing D3 is Mariano's decision.
Trackers only via an apply script, and only if something is resolved or newly found.
