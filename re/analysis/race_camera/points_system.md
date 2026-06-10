# The points system + match rule — RE notes (2026-06-10)

Decompiled in Mashed_pool0 (read-only) from the anchored binary. Ported into
`mashed_re.exe` (TrackRenderer::ScoreAward / ScoreOnElimination /
NextRoundOrEnd). Companion to `race_camera.md` (the elimination trigger).

## 0x0040b290 — score adder `(car, delta)`

- `DAT_008a9570[car] = DAT_008a94e0[car]` (prev-score snapshot)
- `DAT_008a9520[car] = delta` (signed display delta — the "+1/−1")
- `DAT_008a9500[car] = delta` (clamped ≥0 when `DAT_007f1014`)
- replay event ring write (id 0x2ff01 + timestamp `DAT_007f1030`) unless
  race-phase ∈ {1..5}
- `DAT_008a94e0[car] += delta`; `DAT_008a9510[car] = 6000` (ms flash timer);
  floor: score < 0 → 0
- Mode 1 (tag): negative deltas zeroed; with 4 active + no teams,
  delta = (delta==2)?1:0; only the "it" player/team scores.
  Mode 2: team-1 never scores; 4-alive remap delta = (delta==2).

## 0x0040eee0 — elimination scoring callback `(victim, delta)` (delta = 1)

Called from the round-end check (0x00410d10) AFTER the eliminator
(FUN_00422fd0). Appends victim to elimination order `DAT_008a94c0[]`
(−1-terminated). `DAT_008a94d0` = participant count:

- ==2: 1 remaining → loser −delta, winner +delta; 0 → both 0.
- ==3 (no teams): 2 remaining → victim −delta; 1 remaining → victim 0,
  winner +delta.
- ==4 (no teams): 3 remaining → victim −2×delta; 2 remaining → victim
  −delta; 1 remaining → victim +delta **zeroed if its score > 10**
  (`FUN_0040b6d0` getter on `DAT_008a94e0`), winner +2.
- Teams (`FUN_0042f500` ≠ 0): team ids at `DAT_007f1a18` stride 0x10;
  whole-team +1 / −1 awards (not ported — standalone is FFA).
- AI fast-forward: race types 3/4/5/10, when the remaining cars are all
  type-2 (`FUN_0040e470`), the trailer (wrap-adjusted progress 80/20/100)
  is auto-eliminated (not ported — player always present).
- Return 1 (round over) when ≤1 remaining; equalizes every car's
  camera-path progress to the survivor's (FUN_00408a50/FUN_00408a70).

## 0x00410510 — Race::EvaluateResult (match end)

Standard modes: a player wins the MATCH when their score reaches **8**
(participants 2–3, or teams) or **exceeds 11** (participants == 4).
On conclusion: race-phase `DAT_0063ba8c = 0xb`, `DAT_0063b914` = winner,
`DAT_0063b90c = 1`. Mode 4/5/7/8/9/10 special end conditions not ported.

## Presentation residual (ledger #5)

The Current Standings drawer is NOT yet verbatim: candidates are
FUN_0043a610 (4-column lobby scoreboard, X = 65/196.75/324.625/450.875) and
the 0x0041af50/0x0041b7a0/0x0041c410/0x0041ce00 HUD sprite cluster (the four
of them call the score getter 0x0040b6d0). The standalone currently renders
the REAL data (scores, ±delta, 6000 ms flash) through an approximate layout.
