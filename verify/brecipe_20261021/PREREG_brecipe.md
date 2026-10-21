# PREREG brecipe: the B recipe on the original's rule 0 / game mode 10, one live source (2026-10-21)

Branch `work/brecipe` from `256a6b2de`. Base exe `37f3f820` (P0 measured on it: B K0 / K1 / K26 `7ab70c5f / 15262e4a / d1b4bc64`).
Knob exe built from this commit. Private root `D:\MashedWT\brecipe_root`, evidence `D:\MashedWT\brecipe_data` (`MOVED_OFF_GIT.txt`),
`MASHED_NO_DEPLOY=1`. Committed before every knob arm. Nothing is flipped. The canonical B / G-CTRL recipe is not replaced.

## 1. Facts

### 1a. Why the arms run `MASHED_ROUND_RULE=4`
- First set in `verify/d3_consumer_20261007/PREREG_CONSUMER.md:53-56` (D-11072 leg E1): rule 4 makes the port's rule-gated
  race-metric consumer (`RuleEngine::UpdateFinishOrder`, rules 4/7/8/9) reachable on the Training capture recipe with no code
  change (`exe_main.cpp` `MASHED_ROUND_RULE` -> `SetRaceRule`). Arms `L4` / `L10`, control `L0` (rule 0, the default).
- Inherited with no new reason: `d3_determinism_20261007/PREREG_F2.md:39-42`, `d3_u9186_20261008/PREREG_G1.md:87-89`,
  `d3_gatefire_20261008/PREREG_H4.md:31-32` + `run_h4.ps1:127`, `PREREG_WSHAPE.md:54`, `d3_regimes_20261013/RESULT_REGIMES.md:85`,
  `d1r2_20261016/run_gctrl.py:40`, `u9779_20261021/run_arms.py` `base_env`, `rsky_20261016/run13.py:79`.
- The hook's own comment (`exe_main.cpp` U-9072 residual B): a test hook to reach a max=8 rule; "Not set in normal play".
- `RESULT_F2.md:102` reported rule 4 near-inert on driving state. No source chose 4 to match an original capture.
  `RESULT_CALLWISE.md:55` ran one arm at `MASHED_ROUND_RULE=0` to match `--rule 0`. `RESULT_u9779.md` section 6 raised it.

### 1b. What the original runs for the B captures
No frontend route: `re/frida/scenario_launch.py` `setup()` (`:3992-4016`) pokes the selection globals before phase 2.
Every B capture (`o_c3`, `o_cc3_1`, `o_al_1`, `o_ep_1`, `o_reg_r13/15/18`) has argv
`--track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots ...`, no `--rule` / `--team` / `--difficulty` / `--powerups`
(defaults `:4124-4140`: rule 0, team 0, difficulty / powerups -1 = not written). `o_ep_1.log`:
`[setup] set track=0 mode=10 cars=4 car=0 rule=0 team=0`.

| word | address | value | writer |
|---|---|---|---|
| game mode (FUN_0042f6a0 = `mov eax,[0x67e9fc]`, 0x0042f6a0) | `0x0067e9fc` | 10 | `setup()` :3996 |
| rule | `0x007f0fd0` | 0 | `setup()` :3997 |
| team play (FUN_0042f500 = `mov eax,[0x67ea64]`, 0x0042f500) | `0x0067ea64` | 0 | `setup()` :3999 |
| track | `0x0063ba7c` / `0x0067f17c` | 0 (Training) | `setup()` :3994-3995 |
| player car | `0x0067ea98` | 0 | `setup()` :3998 |
| slot types (FUN_0040e470 = `[[0x5f2770]+v*4+0x34]`, 0x0040e474 / 0x0040e47a) | slot table | 1,2,2,2 | FUN_0040e480 calls :4008-4009 |
| participants (FUN_0040e340 = `mov eax,[0x8a94d0]`, 0x0040e340) | `0x008a94d0` | 4 (measured, o_ep_1) | the game's spawn loop |
| output slots | `0x007f1a14[0..3]` | 0,1,2,3 | `--poke-ctrl-slots` :4194-4202 |

The u9779 gate probe measured `DAT_0067e9fc` 10, `DAT_007f0fd0` 0, `DAT_008a94d0` 4, `DAT_0067ea64` 0, slot types 1,2,2,2 at the
FUN_0040eee0 call. The real frontend's quick race (0xff420000 arm of FUN_0043dfd0) writes no rule (`exe_main.cpp` D-11053 note).

### 1c. What the port reads (exe; base `256a6b2de`)
| original | port | value on the current H / B recipe | original on B |
|---|---|---|---|
| FUN_0042f6a0 for the AI host | `aib_round_type()` stand-in `return 3` (TrackRenderer.cpp) | 3 | 10 |
| `0x0067e9fc` live | never written on the `MASHED_ROUND` route (the frontend route keeps it in `Nav_GameState().game_mode`) | 0 | 10 |
| `DAT_007f0fd0` for the AI host | `aib_game_mode_fd0()` stand-in `return 0` | 0 | 0 |
| `DAT_007f0fd0` live | never written; read live by AiStandalone.cpp (phase-2 metric, slow-line flag), ElimResolveAI, HUD | 0 | 0 |
| rule | `TrackRenderer::rule_` (RaceSceneState.h default 0), set by `SetRaceRule` from `MASHED_ROUND_RULE` | 4 | 0 |
| FUN_0040e470 for the AI host | `aib_veh_type()` stand-in `v == 0 ? 0 : 2` | 0,2,2,2 | 1,2,2,2 |
| FUN_0040e470 live | `LiveSlotType` (ElimResolveAI only), `SlotStateE470` | 1,2,2,2 | 1,2,2,2 |
| FUN_0040e340 | `g_participantCount = 4` (ForceIntegratorStubs.cpp) | 4 | 4 |
| FUN_0042f500 | `team_play_` from live `0x0067ea64` | 0 | 0 |

Host consumers: `aib_round_type()` -> `Ai_Standalone_Tick` / `Ai_CountdownAlivePoke` aiRound test (set {3,4,5,10} or fd0 in
{4,8,9,10}), `ElimResolveAI` (set {3,4,5,10}). `aib_game_mode_fd0()` -> the AI step dispatch (fd0 4/9 -> ControlStepM49 =
FUN_00416a30, 8 -> FUN_00417da0, else FUN_00416250) and the aiRound test. `aib_veh_type()` -> `LeaderInRange` (FUN_00415190,
`== 1`), `Ai_Standalone_Tick` (`t != 0 && t != 1`), the `== 2` test in AiStandalone.cpp. rule_ -> SegmentCheck / EvaluateResult /
UpdateFinishOrder / SeedMatchScores. u9779 knobs: ER reads `aib_round_type()`, team play, live `0x007f0fd0`, live slot table;
E7 is raised on an `EvaluateResult(rule_)` r == 0 round end; TL reads none of them.

So on the current recipe the port is a hybrid: the rule engine runs rule 4, the AI dispatch runs the rule-0 path (stand-in 0), and
the u9779 resolve fires because the stand-ins answer 3 / 0.

## 2. Change (all default OFF; knobs unset = the base build)
- `MASHED_MODE_LIVE`: `aib_round_type()` returns live `0x0067e9fc`, `aib_game_mode_fd0()` returns live `0x007f0fd0`, and
  `SetRaceRule` writes rule_ into `0x007f0fd0`. `MenuLaunchRace` writes the launch mode into `0x0067e9fc`. On the `MASHED_ROUND`
  route an unset `MASHED_GAME_MODE` writes 3 (the frontend route's 0 -> 3 default and the old stand-in).
- `MASHED_GAME_MODE=<2..10>` on the `MASHED_ROUND` route writes `0x0067e9fc` (recipe input, as `setup()` writes `--mode`). On the
  frontend route it already forced the launch mode.
- `MASHED_SLOT_TYPE_LIVE`: `aib_veh_type()` reads the live slot table (FUN_0040e470's two loads).
- `MATCH-SEED` log line also prints the live words, the host's reads and the host slot types (log only, not hashed).

## 3. Recipes and stacks
- Current recipe (canonical, unchanged): `base_env` = Training, car 1, `MASHED_ROUND=1`, `MASHED_ROUND_RULE=4`.
- Faithful recipe **F** = current + `MASHED_ROUND_RULE=0 MASHED_GAME_MODE=10 MASHED_MODE_LIVE MASHED_SLOT_TYPE_LIVE`.
- Attribution: `F0` = `MASHED_ROUND_RULE=0`; `F0M` = F0 + `MASHED_GAME_MODE=10`; `F0ML` = F0M + `MASHED_MODE_LIVE`; `F` = F0ML +
  `MASHED_SLOT_TYPE_LIVE`. Contrast: `ML` = current + `MASHED_MODE_LIVE` (rule 4 reaches the AI dispatch); `ST` = current +
  `MASHED_SLOT_TYPE_LIVE`.
- On F: `FIL` (+ `MASHED_SUBSTEP_INTERLEAVE`), `FR7` (+ ER + E7 + TL), `FILR7`.

## 4. Predictions
- P0 (knobs unset, knob exe): G-CTRL `15594ec8 / 7285d708 / 26a6e4ad`, B `7ab70c5f / 15262e4a / d1b4bc64`.
- P1 (F0): rule 4 vs 0 differ only in SegmentCheck case 4 (`metric[0] >= 3.0` -> 1), EvaluateResult case 4 (winner = first
  finisher + 1, replacing the score scan's winner) and UpdateFinishOrder (rule 4, metric >= 3.0); seed 6 for both. If no car's
  metric reaches 3.0 and no score passes 11 inside 1200 frames, the B dumps equal the current recipe's byte for byte. No G-CTRL
  value is predicted (14400 frames).
- P2 (F0M): step dumps and ptrace = F0's (no exe reader of the live word changes an AI or physics value with the knob off; the HUD's
  case 10 arm gives the default arm's panels for team 0, RwRaceFx.cpp). The G-CTRL gates csv may move (its `m42f6a0` / `pred_360`
  columns read the live word).
- P3 (F0ML): = F0M. The host reads 10 instead of 3, and every host consumer tests membership in {3,4,5,10}. fd0 stays 0.
- P4 (F): `LeaderInRange` now finds the player. No value predicted; if the B dumps move, report the first differing row per car.
- P5 (ML): fd0 4 reaches the AI dispatch (ControlStepM49) and the live fd0 readers. The B and G-CTRL hashes move from the first AI
  racing step.
- P6: flag_a368 per K on F = 0 / 0,1 / 0,2 if P1..P4 leave the dumps unchanged. If anything moves, flag_a368 is re-read per K and
  the regime labels are re-mapped against the original captures before (b) is read.
- P7: (b) on F (IL off / on, u9779 knobs off / on) equals the current recipe's when the dumps are equal. The u9779 ER resolve still
  fires on F, now with the live words (host 10, fd0 0).
- P8: first diverging call per car vs `o_cc3_1` / `o_ep_1` (`stepcmp.py`) is reported on K0 for F, FIL, FILR7.
- P9: D2 x3 alone on F and 13 tracks on F: reported against the D2 band and the 13-track cites, no value predicted. On the D2 route
  `MASHED_GAME_MODE=10` forces the launch mode 10 (the D2 reference is also `scenario_launch.py` mode 10 / rule 0). If the launch
  gate refuses mode 10 there, D2 is re-run on F without `MASHED_GAME_MODE` and both are reported.

## 5. Gates
| gate | arms |
|---|---|
| G-CTRL knob-off | `H_DEF` on the knob exe (= P0) |
| B dumps | `B_<s>_K0/K1/K26` for s in DEF, F0, F0M, F0ML, F, ML, ST, FIL, FR7, FILR7 |
| G-CTRL per recipe | `H_F0`, `H_F`, `H_ML` |
| (b) | `vsum.py` on the B dumps (regime from flag_a368 per K) |
| first divergence | `stepcmp.py` vs `o_cc3_1`, `o_ep_1` |
| D2 | `D2_F_1..3` alone, `score_d2_v2.py` |
| 13 tracks | `run13.py run` with the F knobs, `score` against `verify/rres_20261016/cites_rres.json` |
