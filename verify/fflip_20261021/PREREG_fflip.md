# PREREG fflip: canonical recipe F, MASHED_MODE_LIVE + MASHED_SLOT_TYPE_LIVE default ON (2026-10-21)

Branch `work/fflip` from `race/first-frame-parity` `4623046c6` (work/brecipe merged). Model claude-opus-5-5, account2. Exe `492e9d9d` built
with `MASHED_NO_DEPLOY=1`, private roots `D:\MashedWT\fflip_root` (arms) and `D:\MashedWT\fflip_root13` (13 tracks), evidence in
`D:\MashedWT\fflip_data` (`MOVED_OFF_GIT.txt`). Written before any arm.

## Mariano's decision

Mariano's decision (2026-10-21), on brecipe's section 7 (`verify/brecipe_20261021/RESULT_brecipe.md`):

1. Swap the canonical B / G-CTRL / 13-track recipe from rule 4 to the faithful recipe F: `MASHED_ROUND_RULE=0 MASHED_GAME_MODE=10`.
2. Flip `MASHED_MODE_LIVE` and `MASHED_SLOT_TYPE_LIVE` to default ON.

## 1. Change

**Code.** `Core/FaithDefaults.cpp` seeds `MODE_LIVE` and `SLOT_TYPE_LIVE` (value 1) after `A5_RUBBER_LIVE`, same mechanism: opt-outs
`MASHED_NO_MODE_LIVE=1` / `MASHED_NO_SLOT_TYPE_LIVE=1`, `MASHED_NO_FAITH=1` seeds neither, an explicit `MASHED_MODE_LIVE=0` still
wins. The three read sites already take any value other than empty / `0` as ON (`TrackRenderer.cpp` `ModeLiveEnabled` /
`SlotTypeLiveEnabled`, `exe_main.cpp` `MenuLaunchRace` launch-mode write and the `MASHED_ROUND` route's game-mode write). Standalone
only: the dev `.asi` does not compile this TU.

**Recipe.** `MASHED_ROUND_RULE=4` -> `MASHED_ROUND_RULE=0 MASHED_GAME_MODE=10` in the live canonical runners:

| runner | line |
|---|---|
| `verify/d1r2_20261016/run_gctrl.py` | 40 |
| `verify/d1r2_20261016/run_replay.py` | 35 |
| `verify/d3_gatefire_20261008/run_h4.ps1` | 127 (+ a `MASHED_GAME_MODE` line) |
| `verify/fe_residuals_20261016/run_h4_wt.ps1` | 125 (+ a `MASHED_GAME_MODE` line) |
| `verify/fe_residuals_20261016/run_fx.py` | 12 |
| `verify/rsky_20261016/run13.py` | 79 |
| `verify/fflip_20261021/run_arms.py` (this lane's copy of the rbflip runner) | `base_env` |

The D2 recipe (`D2BASE`, the `a8_run_port.py` base env, frontend route `MASHED_GOTO=6`) sets no rule and no game mode and is not
changed. On that route the default now also carries the two seeded knobs: `MenuLaunchRace` writes the launch mode into `0x0067e9fc`
and the AI host reads it, and `SetRaceRule` writes rule_ into `0x007f0fd0`. brecipe's D2 arms ran F (rule 0, mode 10, both knobs)
on that route, so the new default's D2 arm (no `MASHED_GAME_MODE`) is a configuration brecipe did not measure.

**Re-grep, hits left alone.** `grep -rlE "ROUND_RULE.{0,14}['\"=]4\b"` over `*.py *.ps1 *.bat *.sh *.js` outside this lane: 118 files,
all under `verify/<lane>_<date>/`. None in `re/`, `scripts/` or `mashedmod/`. Left alone as provenance (each reproduces its own
dated result on its own recipe):

- 56 lane copies of `run_arms.py` (the next lane copies the newest, which is now this lane's).
- 62 other dated lane runners: `camribbon run_cam.py`, `carlod run_gctrl.py / run_replay3d.py`, `carlod2` same two, `d1r1
  run_gctrl.py / run_replay3d.py` (superseded by d1r2), `d1r3 run_gctrl.py`, `d3_b2flip run_fk.ps1`, `d3_bscore run_b2c.ps1 /
  run_bscore.ps1`, `d3_bulk aileaf/run_replay.ps1, roundgrid/run_snap.ps1, roundgrid/stageb/run_order.ps1, tools/peek.py`,
  `d3_consumer run_e2.ps1` (the lane that chose rule 4), `d3_determinism run_f2.ps1`, `d3_gatefire run_a364 / run_gf0 / run_h2 /
  run_m5 / run_wshape.ps1, sa_inputs_poll.py, sa_substate_poll.py` (older siblings of `run_h4.ps1`), `d3_mode7 run_cw /
  run_m1_replay.ps1`, `d3_mode7_m2/m3/m4 run_m*_replay.ps1`, `d3_modes37 run_p1 / run_p2_replay / run_p3_replay.ps1`,
  `d3_pathprog_a`, `d3_u9186 run_g1 / run_h1 / run_h3.ps1`, `d3_u9683`, `d3_u9685`, `dualcopy3 run_arm.py`, `dualcopy4 run_arm.py /
  run_m3replay.py`, `dupinstall / dupinstall2 / fwd494fd0 run_gctrl_private.py`, `gridyaw run_rb.py`, `librwrest run_shade.py`,
  `librwrest_checks run_stand.py`, `pontoon run_arm.py / run_replay3d.py` (pontoon's W_ recipe is rule 8 on Warzone, it overrides
  the rule anyway), `rfx / rfx2 / rhooks / rprops / rstates / rtex run_gctrl.py`, `rprops / rtex run_replay3d.py`, `rprops /
  rstates smoke_tracks.py`, `rsky binmesh_census.py, run_gctrl_priv.py` (run13.py is the live rsky runner), `seek414f00 run_arm.py`.

## 2. The rule-gated consumer

Rule 4 was set on 2026-10-07 (`verify/d3_consumer_20261007/PREREG_CONSUMER.md:53-56`) to reach the port's rule-gated metric
consumer on the Training recipe.

- **What it does.** `Race::RuleEngine::UpdateFinishOrder` (`Race/RuleEngine.cpp:22`, the 0x004177b0 fragment) runs every race
  frame (`TrackRenderer.cpp:8284`). On rules 4/7/8/9 only, it appends each car whose metric reaches 3.0 (`_DAT_005cc31c`) to
  `rulep_.finishOrder[4]` (the 0x0089a870 float slots). On any other rule it returns at once (`RuleEngine.cpp:23`).
- **What reads its output.** Only `EvaluateResult` (0x00410510) through `FinishSlotInt` (FUN_00417740): case 4 winner = first
  finisher + 1, case 8 same plus the not-car-0 end, case 9 any finisher ends the match with no winner (`RuleEngine.cpp`
  switch). `EvaluateResult` sets `match_winner_` / `match_draw_` / `round_winner_` (`TrackRenderer.cpp:8338-8372`). On rule 0 the
  switch takes `default` and the score scan stands: FUN_00410510's score > 11 with 4 participants ends the match.
- **What depends on it.** No gate scorer reads the finish order, the rule-eval log line or the step dump's `rmetric` / `rtick`
  columns: `re/tools/ai_ctrl_window.py` + `re/tools/b_envelopes.json` ((b)), `verify/d2rebase_20261019/score_d2_v2.py` (D2, a
  frontend-route recipe with no `MASHED_ROUND_RULE`), `verify/rsky_20261016/run13.py score` + `drawlist_diff.py` (13 tracks), and
  the G-CTRL trio is a hash identity check, not a pass rule. The only tool that names `rmetric` is `re/tools/det_prefix.py`
  (column handling for the E2 dump). leg E2 (`re/NEXT_SESSION.md:981-994`) measured `UpdateFinishOrder`'s output as read by
  nothing that affects state under rule 4. brecipe measured rule 4 vs 0 as byte-equal on every B dump.

Finding: no gate or score depends on the consumer firing. The swap proceeds.

## 3. Predictions

From brecipe (knob exe `d33d9362`) and rbflip. Arm names are this lane's `run_arms.py` (`H_` G-CTRL, `B_` (b) dumps, `D2_`).

| gate | arm | prediction |
|---|---|---|
| G-CTRL x2 | `H_DEF`, `H_DEF-2` | `ce087436 / b0a2973b / c4f7a000` (brecipe F), both equal |
| G-OPT-F | `H_NOK` (recipe F, `MASHED_NO_MODE_LIVE=1 MASHED_NO_SLOT_TYPE_LIVE=1`) | brecipe "F without the live knobs" F0M `2e01286e / c634bbac / 26a6e4ad` |
| G-OLD | `H_OLDNOK` (rule 4, no game mode, both opt-outs) | the old (rbflip) default `15594ec8 / 7285d708 / 26a6e4ad`. **Must hold.** |
| G-NOFAITH | `H_NF` (recipe F, `MASHED_NO_FAITH=1`) | gates csv moves off `ca4284be` (column `m42f6a0` reads 10 on every row instead of 0, brecipe P2). Step and ptrace equal the old `66d51720 / 1193bbd1` only if the NO_FAITH trajectory reaches no metric 3.0 and no score > 11 where rule 0 and rule 4 part; brecipe never ran NO_FAITH on rule 0, so step / ptrace are **not predicted** |
| G-OLDNF | `H_OLDNF` (rule 4, no game mode, `MASHED_NO_FAITH=1`) | `66d51720 / ca4284be / 1193bbd1` (control) |
| G-B | `B_DEF_K0/K1/K26` | `7ab70c5f / 15262e4a / d1b4bc64`; flag_a368 0 / 0,1 / 0,2 |
| G-B-IL | `B_IL_K0/K1/K26` | `9bd86ffe / 433abcd2 / 1a1ff727` |
| (b) | `vsum.py` | plain A 3/3, B 2/3 (car 2 FAIL), C 3/3; interleave 9/9 |
| G-D2 | `D2_DEF_1..3`, alone | `score_d2_v2.py` whole log, 8-run band: PASS 3/3 |
| G-13 | `run13.py run` on recipe F, `score` against `verify/rres_20261016/cites_rres.json` | 39 frames, 873 rows, all cited, 0 RED |

Default-ON rule: no car passing on the rbflip default (A cars 1, 2, 3; B cars 1, 3; C cars 1, 2, 3) may fail, D2 3/3, 13 tracks 0
RED, and G-OLD must reproduce the old default. If any gate fails, stop and report; the flip is not committed as a pass.

arms/ and tracks/ live in D:/MashedWT/fflip_data (written there directly via A4_OUT / --out), never committed.
