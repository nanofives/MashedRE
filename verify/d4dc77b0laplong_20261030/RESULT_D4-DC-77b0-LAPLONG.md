# RESULT D4-DC-77b0-LAPLONG: a standalone recipe in which cars complete laps, lap counter 0x008a9648

Verdict: **GREEN.** One existing env knob, `MASHED_NO_ELIM=1`, added to the D4-DC-77b0-LAP recipe keeps the round open
for the whole 7200-frame run. With MASHED_PATHPROG=1 + MASHED_PATHPROG_TRACE, every AI car completes at least one lap on
Egypt, Highway and City (3 tracks, 2 needed). Every increment is exactly +1, the laps column is non-decreasing for all
4 cars on all 3 tracks, and the default G-CTRL / B hashes equal the `merge_train.json` refs. kind: text-only (no source
change, no new knob, no PREREG).

Branch `orch/D4-DC-77b0-LAPLONG` off `orch/integration` @ e85c368f1. Exe built in the worktree with
`MASHED_NO_DEPLOY=1`, sha8 `21b71c07`. Private root `D:/MashedWT/D4-DC-77b0-LAPLONG_root`. Every run went through
`test-slot` and `mashed_lock.py run --label D4-DC-77b0-LAPLONG`. Standalone only: no original-side run, no Frida, no D2 arm.

## 1. Why the LAP recipe's round ended, and the knob that keeps it open

The LAP recipe (rule 0, game mode 10, MASHED_ROUND=1) runs the rule-engine branch of `TrackRenderer::UpdateRaceRules`.
After the countdown, `race_cam_.EliminationCheck(cc)` (`D3d9Render/TrackRenderer.cpp:8913-8914`) picks a victim, the
block sets `race_[victim].alive = false`, and `AiBridgeSnapshot` then publishes `g_aib.alive[v] = 0`
(`TrackRenderer.cpp:6625`). `PathProg_FrameTick` skips the step for that car (`Race/PathProgStep.cpp:286`,
`VehicleSlotGetter(v) != 1`), so the LAP arms froze every row at about step call 113.

`MASHED_NO_ELIM` (`TrackRenderer.cpp:136-139`, default OFF, registered as a measurement control in
`verify/d3_elim_20261003/PREREG_STEP1.md` STEP 1C) gates both elimination blocks (`:8913` rule-engine branch, `:8999`
legacy branch) and nothing else. With it set, no car is eliminated, `round_alive_` stays at 4, and the rule-0 segment
check never closes the round: the three arms' `mashed_re.log` files hold **0** `RULE-EVAL` lines (the line is written
once per round end, `:8957-8962`). The lap counter writer is unchanged: the verbatim naked FUN_00408610 step
(`PathProgStep.cpp:289`) under MASHED_PATHPROG.

`MASHED_RACE_MODE=L` + `MASHED_LAPS` (the `exe_main.cpp:2165` override) is not needed and was not used. The LAP RESULT
already measured that it does not reach this flow.

Recipe (env only, the LAP recipe + one variable): `MASHED_MUTE=1 MASHED_TRACK_VIEW=<track> MASHED_CAR=1 MASHED_ROUND=1
MASHED_ROUND_RULE=0 MASHED_GAME_MODE=10 MASHED_WIN_POS=primary-bl MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=7200
MASHED_PATHPROG=1 MASHED_PATHPROG_TRACE=<csv> MASHED_NO_ELIM=1`. Runner: `run_lap.py` here with
`LAP_EXTRA="MASHED_NO_ELIM=1" LAP_TAG=_NE`.

Scope note: MASHED_NO_ELIM changes the round's outcome (no car is ever eliminated), so these arms are a witness for the
lap counter's writer and its monotonicity, not a faithful rule-0 round.

## 2. Lap arms (`run_lap.py --score`, full output `SCORE_laps.txt`, every increment in `increments.txt`)

| arm | trace frames | car 1 laps | car 2 laps | car 3 laps | first 0 -> 1 (frame, cars 1/2/3) | decreases | jumps > 1 |
|---|---|---|---|---|---|---|---|
| L_Egypt_NE | 6486 | -1 -> 5 | -1 -> 6 | -1 -> 5 | 1481 / 970 / 1260 | 0 | 0 |
| L_Highway_NE | 6347 | -1 -> 1 | -1 -> 2 | -1 -> 2 | 4667 / 2616 / 2659 | 0 | 0 |
| L_City_NE | 6451 | -1 -> 2 | -1 -> 2 | -1 -> 2 | 2740 / 2537 / 2595 | 0 | 0 |

Car 0 (the parked player): Egypt -1 -> 0 at frame 2282 (node 39 -> 0, one row), Highway and City 0 throughout.
All four cars NONDEC on all three tracks (`ALL NONDECREASING`).

Every increment, on every car and track, happens on the frame the node column (+0x20) wraps from the last path node to
node 0 and prog (+0xc8) wraps from about N to about 0: Egypt node 39 -> 0, Highway 126 -> 0, City 116 -> 0
(`increments.txt`, 44 rows). Each AI car visits every node of its track (distinct_nodes 40 / 127 / 115).

## 3. Default gates (exe 21b71c07, `verify/savelink_20261023/run_arms.py`)

| gate | file | sha8 | ref (merge_train.json) |
|---|---|---|---|
| G-CTRL | H_DEF.step.csv (13962 lines) | 8284be46 | 8284be46 |
| G-CTRL | H_DEF.gates.csv (54085 lines) | ba407a16 | ba407a16 |
| G-CTRL | H_DEF.ptrace.log (27042 lines) | 31fe043e | 31fe043e |
| B | B_DEF_K0 / K1 / K26 .step.csv (762 lines each) | 1d5bf4f9 x3 | 1d5bf4f9 x3 |

No source changed, so the gate run only confirms the branch point's exe.

## 4. Suggested ledger note for D4-DC-77b0 (report, not edited)

"0x008a9648 lap counter: exe writers PathProgStep.cpp:248/289 (MASHED_PATHPROG) and :240/335 (MASHED_PP_PUBLISH=0x1),
both OFF. D4-DC-77b0-LAPLONG: with MASHED_NO_ELIM=1 on the LAP recipe, 0 -> 1 and later laps witnessed on Egypt,
Highway, City, every step +1 on the node N -> 0 wrap, non-decreasing for all cars."

## 5. Raw files (D:/MashedWT/D4-DC-77b0-LAPLONG_data, not committed)

| file | sha256 |
|---|---|
| lap/L_Egypt_NE.pathprog.csv | 294a72e6d16470a69bf5de26f9efab1c99d8ac46650510ef9d93726fe15857f8 |
| lap/L_Highway_NE.pathprog.csv | e80bb97434cb71d9f825ef0d2f0e1ee5a4163bccc2b6d8921165cd824a2b0179 |
| lap/L_City_NE.pathprog.csv | 995943bccadcd4d2ad830388e1dae4a1fb6f756bb7f2d10d863380115ad23272 |
| lap/L_Egypt_NE.log | 0077afba753d56f69d2bbfbf7dd725d739ce9d5138eb8679c6b7c7e0a2d12693 |
| lap/L_Highway_NE.log | 802b3e4670c080d79eb5e45c70952a0d3546f302a788e4af8f4f69a7ea8a6246 |
| lap/L_City_NE.log | 0f88e836ff34a6b0f11923ad77f2311cb173d137ca4251c18019f7bfdc393f72 |
| gates/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| gates/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| gates/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| gates/B_DEF_K0,K1,K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |

Provenance lines: `RUNLOG.txt` in lap/ and gates/ (copied to `RUNLOG_all.txt` here).
