# G4 — full playthrough on real systems: status + blocker (2026-06-18)

Goal: a complete race AND a championship cup, end-to-end on real physics/AI/collision/
powerups/audio → results → progression persists, playable start-to-finish with NO scaffold
fallbacks ("it's a playable port" milestone).

## What works on real systems (verified this session)
- Full Arctic track loads: radius 80, **94 gates**, 16480 tris.
- All 4 cars drive the REAL physics chain (G2) + REAL AI controller (G3): player via
  MASHED_AI_DRIVES_PLAYER (slot-0 AI ctrl) or human; opponents 1..3 via the .AI race line.
- AI THROTTLE BUG FIXED: ControlStep's game-mode gate read game_mode_fd0() (=DAT_007f0fd0=0)
  instead of the sub-mode FUN_0040e350 (game_sub_mode, race=6) its decomp comment cites, so
  the race-class {6,5,9,10,11} check failed and ctrl[4]/[5] were zeroed every frame -> AI never
  accelerated. Now game_sub_mode() -> AI drives.
- Per-car speed spread (kSlotCapMul, MASHED_AI_SPREAD) so the field differentiates.
- The match -> results -> progression CHAIN is wired + reachable: match_winner ->
  Campaign_OnRaceResult (unlock next + autosave) -> GameFlow_RequestResults -> Results screen
  (verified via the MASHED_RESULT_DEMO force-end path reaching 01_results earlier).
- Lap/progress made robust: forward nearest-gate projection (TrackRenderer UpdateRace) +
  monotonic "nearer-to-next" AI waypoint follower (AiStandalone) — both replace strict
  proximity that the spline-following cars never satisfied.

## BLOCKER — a race does not finish NATURALLY (no scaffold)
The cars do not complete a lap. LAP-DIAG (MASHED_LAP_DIAG, round_mode=1, laps mode, 94 gates):
all 4 cars advance from spawn to ~gate 7 then **STALL there for the entire run** (gate=7 from
t=6s to t=110s). Reducing the AI lookahead from 4 -> 1 only moved the stall point (gate 2 -> 4 ->
7); it does not break it.

Root cause = AI navigation quality at the first corner (~gate 7):
- ControlStep's ported bands apply FULL-LOCK steer (ctrl[0]/[1]=255) whenever the bearing error
  is 30..180 deg (the "mildly off -> hard corrective" band, FUN_00416250). The original keeps the
  error <30 deg via its curvature-walk target selection (FUN_00443300 / FUN_00443dc0 tail), so the
  full-steer band rarely fires. That refinement is STUBBED in the standalone, so at the first real
  corner the error stays in the full-steer band -> the car orbits / drives into the track edge,
  where the standalone's ground-collision edge-stop (UpdateCar zeroes velocity on GroundHeight
  miss) traps it. All 4 cars follow the same line -> identical stall at gate 7.
- So neither LAPS (no lap-line / start-finish wrap) nor ELIMINATION (field never spreads to the
  RaceCamera required_zoom==10 threshold) ever resolves; the only "match over" seen earlier was the
  MASHED_RESULT_DEMO force-end SCAFFOLD (now gated behind !MASHED_PLAYTHROUGH).

## What G4 needs next (focused effort, beyond a quick tune)
1. Port the AI lookahead/curvature-walk refinement (FUN_00443300 + the FUN_00443dc0 tail) so the
   AI picks a well-aligned target through corners (error <30 deg) -> the full-steer band stops
   firing -> cars negotiate corners + drive full laps. (Or build a standalone pure-pursuit tuned
   to the physics turn radius.)
2. Soften the off-track response: slide along the collision edge instead of hard-zeroing velocity,
   so a car that clips a corner recovers instead of sticking.
3. Then the cup loop: results -> next cup track -> re-enter -> repeat, verifying progression
   persists across the cup (Campaign_OnRaceResult already unlocks + autosaves per win).

## Env knobs added this session (all default-off / inert)
MASHED_AI_DRIVES_PLAYER, MASHED_AI_SPREAD(default on), MASHED_PLAYTHROUGH (let the real match
resolve; extends the result-demo wait to 300s), MASHED_LAP_DIAG, MASHED_AI_DIAG.
