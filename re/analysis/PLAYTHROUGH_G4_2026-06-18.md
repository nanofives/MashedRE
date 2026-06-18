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
resolve; extends the result-demo wait to 300s), MASHED_LAP_DIAG, MASHED_AI_DIAG, MASHED_AI_NAV
(per-car nav trace), MASHED_AI_PUREPURSUIT (opt-in experimental proportional steer),
MASHED_AI_STEERFLIP (steer-sign A/B for the pursuit path).

## UPDATE — deepened diagnosis (per-car nav trace, MASHED_AI_NAV -> ai_nav.log)
Traced one opponent (v1) frame-by-frame. The target is correctly NORTH of the car
(tgt=(-25.8,27.8) then (-26.8,36.1)), but the car drives WEST/SOUTH AWAY from it
(own (-27,16)->(-47,21)->(-50,13)->(-32,3)), then hits a track edge, velocity is zeroed
(spd=0), and it sticks. Two findings:
1. STEER HAS NO EFFECT on the trajectory: flipping the AI steer byte (ctrl[0]<->ctrl[1],
   MASHED_AI_STEERFLIP) produced an IDENTICAL path. So the issue is not the steer sign.
2. ROOT = grounding-loss + edge-trap. The car flings off the racing line near spawn; once off
   the collision mesh GroundHeight fails so (a) the standalone sets io.grounded=0 -> SetGrounded
   writes NO contact load -> A6a produces no +0x9c0 -> the car has ZERO yaw/steer authority (the
   exact G2 mechanism), and (b) UpdateCar zeroes the world velocity on the GroundHeight miss ->
   the car is trapped at the edge (spd=0). err then thrashes (186/48/225...) because the heading
   is reconstructed from a near-zero velocity. So steer can never recover the car.

So natural completion is blocked by a COMPOUND opponent-integration issue, not a single tune:
the cars leave the drivable surface early and lose all steering authority. Fixing it needs
(in order): (a) keep AI cars grounded + on-line near spawn (correct spawn orientation along the
spline + ensure the first frames don't fling them off); (b) NON-TRAPPING off-track response —
slide along / project back onto the collision surface instead of zeroing velocity, so a car that
clips the edge recovers with steer authority intact; (c) THEN the AI target/steer (pure-pursuit or
the verbatim bands) can actually hold the line -> full laps -> match -> cup. This is a focused
methodical debugging effort (per-frame physics+grounding trace), scoped here.

## UPDATE 2 — edge-trap FIXED; remaining blocker = under-damped steering (needs PD)
(b) DONE [U-A8-OFFTRACK], TrackRenderer player + opponent paths: on a GroundHeight-miss next-pos
we now RETAIN (0.5x-damped) velocity instead of zeroing it, so the chain keeps producing a +0x9c0
yaw rate and the AI keeps steer authority — the car is no longer permanently trapped at the edge.
Plus a signed continuous steer (removes the bang-bang flip at err=180) + a low steer cap.

REMAINING (the real wall): with the trap gone the cars MOVE but OSCILLATE / donut near spawn — the
per-car nav trace shows the car briefly align (err=42, velocity toward the target) then swing past
and away, repeatedly; gate progress crawls (a car occasionally reaches gate ~7 then swings back).
This is an UNDER-DAMPED steering control loop: the AI has only a PROPORTIONAL term, but G2's
contact-load fix made +0x9c0 strong + kYawScale=0.34 gives high yaw authority, so the yaw OVERSHOOTS
alignment (momentum) and swings back -> a limit cycle. The player's G2 smooth circle used a CONSTANT
steer (no feedback) so it never oscillated; the AI's target-tracking feedback loop does.

THE fix (focused, next session): a PD steering controller — add a DERIVATIVE/damping term that
counter-steers proportional to the body yaw rate (+0x9c0), so the car settles onto the target
heading instead of overshooting. Needs the Ai::Host to expose the per-car yaw rate (+0x9c0) (one
new getter) so ControlStep can damp. Then: cars hold the line -> full laps -> elimination/laps
resolve -> the cup loop (results -> next track -> re-enter, progression already persists per win).
This is control-systems tuning, not RVA RE; ~a focused session. Env infra in place: MASHED_AI_NAV,
MASHED_AI_PUREPURSUIT, MASHED_AI_STEERFLIP, MASHED_LAP_DIAG, MASHED_PLAYTHROUGH.
