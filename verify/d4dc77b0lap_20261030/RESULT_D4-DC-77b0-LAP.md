# RESULT D4-DC-77b0-LAP: the exe writer of the lap counter 0x008a9648, run standalone

Verdict: **GREEN on the row's criterion, with a coverage limit.** The laps column is non-decreasing for every car on
4 tracks (Egypt, Highway, City, Training), and the default gates are unchanged. The recipe's round ends 1.9 to 6.5 s
after the step starts, so the arms witness only the -1 to 0 step (start-line crossing). No car completes a lap, so
a 0 to 1 increment is **not** witnessed. kind: text-only (no source change, no knob, no PREREG).

Branch `orch/D4-DC-77b0-LAP` off `orch/integration` @ 9880245d9 (the lead named 0cd4241d4; integration had moved
on by one text-only merge, D4-DC-77b0-VALSRC; 9880245d9 descends from 0cd4241d4). Exe built in the worktree
with `MASHED_NO_DEPLOY=1`, sha8 `a5d28538`. Private root `D:/MashedWT/D4-DC-77b0-LAP_root`. Every run went through
`test-slot` and `mashed_lock.py run --label D4-DC-77b0-LAP`. No original-side run, no Frida.

## 1. Default gates (exe a5d28538, `verify/savelink_20261023/run_arms.py`)

| gate | file | sha8 | ref (merge_train.json) |
|---|---|---|---|
| G-CTRL | H_DEF.step.csv (13962 lines) | 8284be46 | 8284be46 |
| G-CTRL | H_DEF.gates.csv (54085 lines) | ba407a16 | ba407a16 |
| G-CTRL | H_DEF.ptrace.log (27042 lines) | 31fe043e | 31fe043e |
| B | B_DEF_K0 / K1 / K26 .step.csv (762 lines each) | 1d5bf4f9 x3 | 1d5bf4f9 x3 |

The refs the lead quoted match `re/orchestrator/merge_train.json` (`gctrl.ref`, `b.ref`). WORKER_RULES.md still quotes
the older D3-CF-U9779 values in prose. The file wins, as the rules say.

## 2. Lap arms (`run_lap.py`, MASHED_PATHPROG=1 + MASHED_PATHPROG_TRACE)

Recipe = the savelink `base_env` (rule 0, game mode 10, MASHED_ROUND=1, deterministic) with MASHED_TRACK_VIEW
swapped, 7200 frames. Scored by `run_lap.py --score` (full output: `SCORE_laps.txt`).

| arm | trace frames | cars 1..3 laps | first increment (step call) | decreases | round end (mashed_re.log) |
|---|---|---|---|---|---|
| L_Egypt | 6486 | -1 -> 0 | 19 / 24 / 34 | 0 | t=13.7 s, winner car2 |
| L_Highway | 6347 | -1 -> 0 | 19 / 30 / 34 | 0 | t=16.1 s, winner car1 |
| L_City | 6451 | -1 -> 0 | 19 / 30 / 34 | 0 | t=14.4 s, winner car1 (timer 20) |
| L_Training | 6321 | -1 -> 0 | 19 / 28 / 34 | 0 | t=18.3 s, winner car3 (= H_DEF) |

Car 0 (the parked player): -1 throughout on Egypt, 0 throughout on Highway and City, -1 -> 0 at call 22 on Training.
Every increment is exactly +1. The -1 initial value is FUN_00407a60's, ported at PathProgStep.cpp:248.

### Why no lap completes (measured, `L_Egypt_G`, + MASHED_PATHPROG_GATE_LOG, 1500 frames)
The first 115 trace frames of L_Egypt_G are byte-identical to L_Egypt. In the gate log, car 1 has `active=1 alive=1`
through call 110 and `alive=0` from call 112 on, and its +0x928 position is constant from call 110
(-10.995038, 20.1398926). The step is skipped when `VehicleSlotGetter(v) != 1` (PathProgStep.cpp:286), so every row
freezes from about call 113 (prog stops changing at calls 107..114 on every track and car). The round is over by then
(rule 0, elimination). So the recipe gives each car about 113 step calls of driving: enough to cross the start line
from the grid, never enough for a lap.

`MASHED_RACE_MODE=L` + `MASHED_LAPS=3` (the exe_main.cpp:2165 dev override) was tried on Egypt (`L_Egypt_LM`). Its trace
is byte-identical to L_Egypt (sha256 42f3a72e...), so that override does not reach this flow. No further recipe was
built: a lap-length round needs either a recipe this lane does not have or a source change, and neither is in scope.

## 3. The stale ledger note on D4-DC-77b0 (report, not edited)

D4-DC-77b0's note says "The remaining blocker is 0x008a9648, which has no exe writer by grep", and its `next` says "Port a
faithful exe writer for the lap counter 0x008a9648". Both are stale. 0x008a9648 = 0x008a9620 + 0x28, and the exe writes
it as `0x008a9620u + v * 0x30cu` + 0x28, so a grep for `8a9648` finds only readers. Exe writers, all in
`mashedmod/src/mashed_re/Race/PathProgStep.cpp`:

- :248, PathProg_RaceInit under MASHED_PATHPROG: -1 into every real row (FUN_00407a60).
- :289, PathProg_FrameTick under MASHED_PATHPROG: the step `PP_408610` on the real row. The step is the verbatim naked
  transcription of FUN_00408610 (header :7-8, PathProgStep_naked.inc), so the +0x28 store is the original's bytes.
  This is the writer section 2 measured.
- :240 and :335, default arm R (RacePctOrig ON, PathNodes.cpp:347-349) with MASHED_PP_PUBLISH bit 0x1: -1 at init and the
  shadow row's +0x28 copied into the real row each tick. Default mask is 0 (:211-213), so by default nothing writes the
  real 0x008a9648 and its readers (Ai/AiStandalone.cpp:1691 a20, TrackRenderer.cpp:7253) see the BSS value.

Also stale: "FUN_00408610 sits behind MASHED_PATHPROG (OFF)". By default the step already runs every frame on a shadow row
(PathProg_RacePctTick :312-340, called at TrackRenderer.cpp:6297) and publishes +0xcc (0x008a96ec) at :337. Only the
publication of +0x28 is gated. The same two clauses in `re/tools/dual_copy_allowlist.txt:38` ("reads from places no exe TU
writes (0x008a9648 / 0x008a96ec, ...)") are stale for both addresses. Not edited here (the allowlist route is not this
row's deliverable).

Suggested replacement note for D4-DC-77b0: "0x008a9648 has exe writers (PathProgStep.cpp:248/289 under MASHED_PATHPROG,
:240/335 under MASHED_PP_PUBLISH=0x1, both OFF). D4-DC-77b0-LAP: non-decreasing on 4 tracks, -1 -> 0 only (rounds end
before a lap). Next: the AiBankSwitch recipe for AiPreTick with the lap counter published (PP_PUBLISH=0x1 or PATHPROG)."

## 4. Raw files (D:/MashedWT/D4-DC-77b0-LAP_data, not committed)

| file | sha256 |
|---|---|
| gates/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| gates/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| gates/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| gates/B_DEF_K0,K1,K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |
| lap/L_Egypt.pathprog.csv | 42f3a72e6bc9206f65c521fd8a74864fb0a73249beb70a7282c1f52310b2e0f7 |
| lap/L_Highway.pathprog.csv | c77da208f8bd250bb06b07f1c886a3d4df5059404a621baf8a6ee150ca8e7696 |
| lap/L_City.pathprog.csv | cc11713782f8aac5d156c5da69a7b44d11cbd012028572a6c54fbbab617b805c |
| lap/L_Training.pathprog.csv | 1765a04dfad1436ae80de6f5f8b969cafb0e4798e430d64b2ed1087bbc1550b6 |
| lap/L_Egypt_G.pathprog.csv | 6f43760aa355255c4817240bb57a0bd1d9e0581caaeb1e2cd7b395c2998ee30b |
| lap/L_Egypt_G.gatelog.txt | 991ca03d63f34f298e789c5a47f60534ba34167fbd83550436dd3575f6ca32f8 |
| lapmode/L_Egypt_LM.pathprog.csv | 42f3a72e6bc9206f65c521fd8a74864fb0a73249beb70a7282c1f52310b2e0f7 |

Provenance lines: `RUNLOG.txt` in each of gates/, lap/, lapmode/ (copied to `RUNLOG_all.txt` here).
