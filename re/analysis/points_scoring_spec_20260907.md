# Spec — Playtest #4 "points miscounted in single-player" (scoping only)

Scoping only (read-only, produced 2026-09-07 by orchestrator child C on account2 —
NO Ghidra). For a later account3 / Ghidra-capable session.

## Verdict (HYPOTHESIS, pending the witness in §3)
The port's `TrackRenderer::ScoreOnElimination` faithfully reproduces the generic branches
of the original per-elimination scorer `FUN_0040eee0`, but omits one corner: in a
4-participant, no-teams race in game mode `DAT_0067e9fc ∈ {3,4,5,10}` with 3 cars
remaining, the original calls finish-order resolver `FUN_0040d590`, which awards POSITIVE
points to the two leading (AI) cars (+delta, +delta*2) and eliminates the trailer. The
port instead runs only `ScoreAward(victim,-2)` (TrackRenderer.cpp:3677) and gives
opponents nothing → AI never accrues points → "points miscounted." HYPOTHESIS until the
mode-log witness confirms single-player is actually in `{3,4,5,10}` and `FUN_0040d590`
fires; if not, the defect is elsewhere (see §3 second hypothesis).

## 1. RVAs needing Ghidra decomp
| RVA | Why | Port status |
|---|---|---|
| `0x0040d590` `FUN_0040d590(a,b,c,d)` | the resolver itself: 3-way progress compare (thresholds `_DAT_005cc730`@0x0040d5a4, `_DAT_005ccd6c`@0x0040d5ae, `_DAT_005cc568`@0x0040d5b6) + award pattern last=`FUN_00422fd0(last)`+`FUN_0040b290(last,-d)`, mid=`FUN_0040b290(mid,+d)`, first=`FUN_0040b290(first,+d*2)` | NOT ported |
| `0x0040eee0` `FUN_0040eee0`, `DAT_008a94d0==4` branch 0x0040f5a0..0x0040f948, call site 0x0040f8bc | the guard reaching the call: `FUN_0042f500()==0` (no teams) AND `iVar5==3` (3 remaining) AND `FUN_0042f6a0()=DAT_0067e9fc ∈ {3,4,5,10}` AND `DAT_007f0fd0==0`; plus which 3 car indices are a,b,c and what d is (caller delta=1). Confirm literally. | branch ported as `ScoreOnElimination` (:3672); this corner missing |

Callees of `FUN_0040d590` need NO new decomp — all have port equivalents:
`0x00408ad0` progress getter -> `RacePct`/`ProgBehind` (TrackRenderer.cpp:3515/3525);
`0x0040b290` score adder -> `ScoreAward` (:3492); `0x00422fd0` mark-eliminated -> `KillCar`
(:3538). So porting `FUN_0040d590` is self-contained once its body is read.

## 2. Port fix site
- `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp`, `ScoreOnElimination(int victim)`
  lines 3672-3688. FFA (`!team_play_`) branch: remaining==3 -> `ScoreAward(victim,-2)`
  (:3677 = FIX HERE); ==2 -> -1 (:3679); ==1 -> runner-up +1 capped>10, survivor +2 (:3684-3686).
- Called from `UpdateRace` :3891 (rule-engine path) and :3938 (legacy), right after
  `race_[victim].alive=false; --round_alive_`.
- `scores_[]` = `DAT_008a94e0`. `RuleEngine` does NOT award per-car points (segment/result
  only), so the fix does NOT touch RuleEngine. `RuleEngine::Persist.teams`=`DAT_0067ea64`
  (RuleEngine.h:85), `rule`=`DAT_0067e9fc`.
- Fix: in the remaining==3 FFA arm, replace bare `ScoreAward(victim,-2)` with the
  mode-gated 3-way resolver (guard participants==4 && !team_play_ && rule_∈{3,4,5,10} &&
  single-player), transcribed verbatim from 0x0040d590 using existing
  RacePct/ProgBehind/ScoreAward/KillCar. Keep the generic -2 for modes outside {3,4,5,10}.

## 3. Instrumentation FIRST (before porting; neither side needs Ghidra)
**Port side** — one env-gated log in `ScoreOnElimination` (TrackRenderer.cpp:3672), shape of
the existing `TEAM_ELIM` log (:3598), per elimination: victim, round_alive_ (remaining),
scores_[0..3], rule_, team_play_, and computed `would_d590 = (participants==4 &&
!team_play_ && remaining==3 && rule_∈{3,4,5,10} && single-player)`.

**Original side (load-bearing)** — Frida on `MASHED.exe` during a MANUALLY driven scored
single-player race (NOT NAV_DEMO — see 2026-09-07 correction; MASHED_WIN_POS=left-bl,
explicit PID, kill only that PID):
- `Interceptor.attach(0x0040d590)` logging a,b,c,d — per-elimination (low-rate), SAFE under
  the hot-path rule. Do NOT attach `FUN_00408ad0` (hot).
- `Interceptor.attach(0x0040b290)` logging (car,delta) for every actual award.
- On each hit read `DAT_008a94e0[0..3]`, `DAT_0067e9fc`, `DAT_0067ea64`.

Answers: does `FUN_0040d590` fire in a normal single-player race, with what deltas. If it
fires, diagnosis confirmed + trace is the port reference. If `DAT_0067e9fc ∉ {3,4,5,10}` and
it never fires, STOP — second hypothesis is round-end timing / a never-eliminated car
getting no award in `FUN_00410510` (0x00410510); porting `FUN_0040d590` would be wasted.

## 4. Acceptance gate — do NOT port blind
1. Original-side trace showing `FUN_0040d590` fires in a scored single-player race, with arg
   indices + resulting deltas / `DAT_008a94e0` progression. No trace -> no port.
2. Per-elimination score diff GREEN: port's scores_[0..3] progression per elimination matches
   the original trace for an equivalent race — not merely "opponents now nonzero" (that's the
   failure mode the existing TEAM_AWARD instrumentation warns about, :3555).
3. Verbatim transcription from 0x0040d590 (award -d/+d/+2d, wrap-compare thresholds, trailer
   elimination), citing every RVA; no smoothing the delta asymmetry.
Synthetic/port-only = C3 at best; C4 needs the canonical original-side scenario run
(CONFIDENCE.md). Standalone gameplay fix -> analogue is a full scored-race score-progression
diff, not a screenshot.

### RVA/global map
`FUN_0040d590` 0x0040d590; `FUN_0040eee0` 0x0040eee0 (`DAT_008a94d0==4` branch 0x0040f5a0,
call 0x0040f8bc); `FUN_0042f6a0` 0x0042f6a0 -> `DAT_0067e9fc` 0x0067e9fc (mode, needs
∈{3,4,5,10}); `FUN_0042f500` 0x0042f500 -> `DAT_0067ea64` 0x0067ea64 (teams, needs ==0);
`DAT_008a94e0` 0x008a94e0 (scores, port scores_[]); `DAT_008a94d0` 0x008a94d0 (participant
count =4); `DAT_007f0fd0` 0x007f0fd0 (team/split enum, needs ==0 single-player); port fix
site TrackRenderer.cpp:3676-3677, callers :3891/:3938.
