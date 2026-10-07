# PRE-REGISTRATION (UNRUN) — D-11072 leg E: reach a LIVE consumer of the race-position metric

Date 2026-10-07. Follows `verify/d3_racepos_20261006/RESULT_BRIDGE.md` (the `0x008a96ec` bridge is
correct and proven consumed by `AiPreTickRubberBand`, but behaviourally INERT because that
consumer's downstream is gated `fd0 ∈ {4,7,8,9}` and the standalone's `fd0` global `0x007f0fd0`
is 0). Nothing below re-derives that.

**Status at commit time: UNRUN.** No run has been executed, no source edited.

## 0. Scope, and what this leg will NOT do

- No C-level promotion. A bridge/metric swap is not a behavioural diff of any RVA.
- `original/` is untouched; it is the diffing reference. Anchor `BDCAE093…` is re-verified before
  and after any run.
- Nothing ships default-ON out of this leg. Any code added is behind a default-OFF knob.
- Leg B (reference distance) stays unstarted.

## 1. The static finding that reframes the lane (measured by reading source, no run)

The session brief asked: *what writes `0x007f0fd0`, and can a standalone recipe reach
`fd0 ∈ {4,7,8,9}`?* Both halves are answered statically, and the answer moves the lane.

**1a. Who writes `0x007f0fd0`.** In the ORIGINAL it is written by the race-launch action of
`FUN_0043dfd0` from the cup event-type table `DAT_005f65c8`, transcribed verbatim in the port as
`Race/RaceModes.cpp`: `EventType(gameMode, track)` = high word of `kCupEvent[modeOff + track*3]`
(`RaceModes.cpp:45-55`, modeOff 0/1/2 for game modes 3/4/5, 13 tracks), then
`RaceRuleFromEvent` (`RaceModes.cpp:58-75`) maps event → rule: `0x2/0x3/0xb → 4`, `0x5 → 9`,
`0x6 → 8`, `0x7 → 5`, `0x4 → 10`, `0x8 → 7`, everything else → 0. Non-championship modes
(2, 6..10) leave rule 0 except the MP game-length path `RaceRuleFromGameLength` (0/1/2 only,
`RaceModes.cpp:98-105`).

**1b. The standalone never writes the global, but it DOES carry the rule.** `exe_main.cpp:2185`
computes `cfg.raceRule = RaceModes::RaceRule(cfg.gameMode, cfg.trackId)` and the race layer stores
it as `TrackRenderer::rule_` (`TrackRenderer.cpp:4642-4644`, `SetRaceRule`). The raw global
`0x007f0fd0` is never assigned anywhere in the port; `aib_game_mode_fd0()` is a hard
`return 0` (`TrackRenderer.cpp:96`). So the port has two parallel mode states, and the one the
port's own race code uses is `rule_`.

**1c. There is already a LIVE, rule-gated consumer of the race-position metric, and it is not
behind `fd0`.** `TrackRenderer::UpdateRace` runs the port's rule engine every frame
(`TrackRenderer.cpp:5119`, default-ON — `MASHED_RULE_ENGINE=0` is the only way off) and feeds it

```
rc.metric[i] = race_[i].laps + fmod(race_[i].progress, n)/n        // TrackRenderer.cpp:5132-5134
```

then calls `RE::UpdateFinishOrder(rule_, rc, rulep_)` (`:5136`), `RE::SegmentCheck(rule_, …)`
(`:5166`) and `RE::EvaluateResult(rule_, …)` (`:5190`). In `Race/RuleEngine.cpp` the metric is
read under: `UpdateFinishOrder` for `rule ∈ {4,7,8,9}` with threshold 3.0 (`RuleEngine.cpp:21-23`);
`SegmentCheck` cases 4/7/8/9/10 (`:76, :90, :97, :103-104, :114`); `EvaluateResult` cases 7/9/10
(`:187, :203, :208`).

**1d. That consumer's gate is reachable from the existing capture recipe with no code change.**
The `MASHED_ROUND` dev route reads `MASHED_ROUND_RULE=<n>` and calls `g_track.SetRaceRule(n)`
directly (`exe_main.cpp:8508-8515`). So the same Training capture used by every (e)/(b) scorer can
be run at `rule_ = 4` — same track, same car set, same tooling.

**Consequence.** "Reach the consumer" does not require writing `0x007f0fd0` at all. The
`0x007f0fd0` mirror reaches one *additional* consumer (the mode-4/9 `GearConstSet` AI speed
scaling, `AiStandalone.cpp:1492-1524`); the finish-order/segment half is already live under
`rule_` and is currently fed by the **non-monotone** `progress`, which is exactly what leg A's
`arcpct` was built to replace. That is this leg's target.

## 2. The known obstacle, stated before measuring

Every metric consumer in §1c compares against 3.0 (rules 4/7/8/9) or 2.0 (rule 10) — i.e. two or
three completed laps. The standard 60 s capture does not get there: in `Y1.csv` (the bridge run,
Training, 2984 frames) the maximum `lap` is **1** on all three AI cars and `arcpct` spans 0..99.9,
so one lap takes roughly the whole window. **A 60 s capture cannot witness this consumer**, and a
run that does must be ~3x longer. This is pre-registered as the reason E1 exists: it is a liveness
question, not a behaviour question.

## 3. Leg E1 — LIVENESS, zero source change (run first)

Does the rule-gated metric consumer execute at all in a feasible standalone capture?

**Runs.** `re/tools/sa_capture.py`, Training, the standard knob set
(`MASHED_MUTE=1 MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1 MASHED_WIN_POS=primary-bl
MASHED_AI_STEPDUMP=<path>.csv`) plus:

| run | extra knobs | shots |
|---|---|---|
| `L4` | `MASHED_ROUND_RULE=4` | 8,120,240 |
| `L10` | `MASHED_ROUND_RULE=10` | 8,120,240 |
| `L0` (control) | *(none — rule 0, the current default)* | 8,120,240 |

**Gates.** Each states its threshold and the number it is compared against on the same line.

| gate | threshold | measured |
|---|---|---|
| `E1-RULE` | `mashed_re.log` `MATCH-SEED rule=` equals 4 on `L4`, 10 on `L10`, 0 on `L0` — 3 of 3 runs | |
| `E1-LAPS` | max `lap` over AI cars 1..3 in the stepdump ≥ 3 on `L4` and ≥ 2 on `L10`; denominator = the 3 dumped AI cars, over all frames of the 240 s run | |
| `E1-EVAL` (**control that can fail**) | `RULE-EVAL` lines present on `L4`/`L10` and **absent or `r=0`-only on `L0`** where `L0`'s rule-0 path takes no metric branch; count of `RULE-EVAL` lines reported per run | |
| `E1-DET` | `E1-RULE` + `E1-LAPS` verdicts reproduce on a second `L4` run | |

**Decision.**
- `E1-LAPS` PASS → the consumer's threshold is reachable; proceed to E2.
- `E1-LAPS` FAIL (no car reaches 3 laps in 240 s) → **do not** extend the window indefinitely and
  **do not** lower a threshold. Record the leg as blocked on capture length, report the measured
  max lap and the implied window, and stop. Thresholds are the original's; changing one to make a
  gate pass is forbidden by this pre-registration.
- `E1-EVAL` showing the metric block never executes even with laps ≥ 3 → add the §3b probe.

**§3b probe (only if `E1-EVAL` is ambiguous).** A default-OFF `MASHED_RULEMETRIC_PROBE=1` counter
logging, per frame, `rule_`, `rc.metric[0..3]`, each `UpdateFinishOrder` slot write and each
`SegmentCheck` return. Default-OFF, log-only, no behaviour. Its own control: the counters must be
0 on an `MASHED_RULEMETRIC_PROBE` unset run (absent log proves nothing on its own).

## 4. Leg E2 — the behavioural swap (ONLY if E1 passes)

Feed the rule engine the **monotone** metric instead of the non-monotone one, behind a default-OFF
knob `MASHED_RACEMETRIC_ARC=1`:

```
rc.metric[i] = race_[i].laps + race_[i].arcpct * 0.01f        // replaces TrackRenderer.cpp:5132-5134
```

(unit check: `arcpct` is 0..100 per lap, `progress`-derived `pct` is 0..1, hence the `*0.01`.)

**Gates.** Pre-registered now, to be filled on the run:

| gate | threshold | measured |
|---|---|---|
| `E2-WROTE` | with the knob ON, logged `rc.metric[i]` equals `lap + arcpct*0.01` on 100 % of dumped rows; OFF arm equals `lap + progress-pct` on 100 % | |
| `E2-DIFF` (**control that can fail**) | the ON and OFF arms differ on ≥ 1 % of frames in metric *ordering* of cars 1..3 (denominator = frames where all 3 cars are dumped). If 0 %, the swap is INERT and no behaviour claim follows | |
| `E2-EFFECT` | `RULE-EVAL` finish order / `r` differs between arms, or is stated identical → INERT | |
| `E2-NOREG-E` | criterion (e), `ai_speed_env.py --check`, knob OFF vs the committed baseline: all six digits identical | |
| `E2-NOREG-B` | criterion (b), `ai_ctrl_window.py --check`, knob OFF: the same 5 bands | |
| `E2-KNOBOFF` | knob OFF reproduces the committed baseline stepdump cell-for-cell; denominator = shared `(frame,seq,v)` keys × pre-existing columns | |
| `E2-DET` | (e) digits + bands identical across 3 repeats per arm | |

Default-ON shipping is **out of scope for this leg** regardless of outcome: that needs the
car<->car no-regression set (G-NOREG-E/-B, G-DET, G-KNOBOFF) **plus** modes oracle rule 3 **plus**
the power-up sweep, per the standing rule.

## 5. The `0x007f0fd0` mirror — registered, NOT run in this leg

A knob mirroring `rule_` into `0x007f0fd0` would additionally reach `AiPreTickRubberBand`'s
mode-9 (`AiStandalone.cpp:1492-1502`) and mode-4 (`:1505-1524`) `GearConstSet` AI-speed scaling,
and the mode-gated `0x0089a368` difficulty branches (`:1542-1543`). Those are real AI *speed*
multipliers, so this interacts with the open over-speed metric (e) and could move it in either
direction. It is **not** run here because the global has many other readers in the port
(`AiController.cpp:260/322/338`, `HudDispatch.cpp`, `Thresholds_ah4.cpp`, `ScoreMasks_ah3.cpp`,
`BatchAA_s4.cpp`, `ScenarioLeaves_sa2.cpp`) and its blast radius must be enumerated first — the
standing lesson from the `+0x9a8` ring slot. Registered here as the next sub-lane, with its
precondition: a per-file LIVE/dead classification of those readers.

## 6. Non-goals

Leg B; any threshold edit; any default-ON change; any write to `original/`; any claim of effect
without a control that could have failed.
