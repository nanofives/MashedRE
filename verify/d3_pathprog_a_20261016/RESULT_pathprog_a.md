# RESULT: why wholesale `MASHED_PATHPROG` costs regimes A and B, split into arms (and the G-CTRL of `0020d521`)

Pre-registration `PREREG_pathprog_a.md` (`31812fc3`, before every arm of its section 3 except the two G-CTRL arms already running, whose expected values are the orchestrator's), amendments A1 to A6 (each committed after the
previous round was scored and before the arms it adds). Branch `work/pathprog_a`, base `0020d521`. Model claude-sonnet-5-5 (account2, no Ghidra MCP). No original-side run: the committed captures
(`o_pp1`, `o_p0a`, `o_rd3`, the (b) envelopes) are the references. All 80 runs exited 0, private game roots.

## 0. Verdict first

> **Job 1, G-CTRL of `0020d521`: PASS.** `G_DEF` = **`716cb2fd / 2a2afda1 / ba90ed88`**, `G_OPTOUT` (`MASHED_NO_RACEPCT_FUN408610`) = **`c2e4d60d / 603ab244 / ba90ed88`**. The final binary with every new knob compiled in and unset gives the same two trios (`F_DEF`, `F_OPTOUT`).
>
> **Job 2.** Of the four things wholesale `MASHED_PATHPROG` does, **exactly one moves regime A (and B): the live node progress `0x008a96e8`.** The lap counter, the rest of the row and the placement are inert alone. The progress has **one consumer**
> in this recipe, the boost-order sort `Fi_UpdateBoostOrder`, proved by a mutant that is byte-identical to the default. It costs the regime because the port's PLAYER is the least-progressed car where the original's is the most: the port's
> player starts on the wrong side of the node-0 gate plane (z -2.00247 or -1.99904, progress 0.0009 or 0.0000) and **never settles**, while the original's player settles +0.0000921 in z on its first step (-1.999037 -> -1.998945), ends up 7.7e-5 before the plane
> and resolves to the end of the last segment (progress 30.0). So the "two least progressed" pair is `{3, 0}` where the original's is `{2, 3}`, and car 2 loses the 8e6 boost arm.
> **Not an AI input**: the first divergence is physics, car 2's `rec_9e4` at step row 2.
>
> With the player on the original's side of the plane the live row at the original's place (`MASHED_PP_PUBLISH=7`) is **(b)-neutral on every regime**: A 1/3, B 2/3, C 0/3 on all 8 K, control bytes equal to the seeded-pair counterfactual. So the live step at the original's
> place is not proposed as the default NOW (it fails (b), A 0/3 and B 1/3), and it becomes a candidate as soon as the player's first-step settle is ported (the player-at-rest row of U-9684, a D2-watch-list change). **No default changed here.**

## 1. The split (stage 1, regime A, `SCORE_S1.txt`)

`MASHED_PP_PUBLISH=<mask>` (default 0) on the shadow-row tick of U-9685: `0x1` lap counter `+0x28`, `0x2` node progress `+0xc8`, `0x4` the rest of the row, `0x8` run before the AI tick (`UpdateRace`) instead of at the end of `UpdateCar` (the original's place, `0x0040fc75`).

| arm | mask | K00 / K02 regime A | step hash K00 | note |
|---|---|---|---|---|
| `D` (default) | none | 1/3 | `774fadda` | car 2 PASS at the margin (`c1#` 67 / 70, `steer#` 95 / 96) |
| `X0` | 0 | 1/3 | `774fadda` | no-op control = `D` (**G-NOOP PASS**) |
| `X1` | lap | 1/3 | `b55f0042` | only `val_880` and `lap_9648` differ, 0 control bytes in 1018 / 285 rows |
| `X2` | progress | **0/3** | `f2599ada` | car 2 `rec_9e4` differs at row 2 |
| `X4` | rest of row | 1/3 | `774fadda` | byte-equal to `D` |
| `X8` | placement only | 1/3 | `774fadda` | byte-equal to `D` |
| `X3` | lap + progress | 0/3 | `52ecead4` | = `X7` |
| `X7` | lap + progress + row, at the original's place | **0/3** | `52ecead4` | car 1 `c1#` 13 (< 21), car 2 `c1#` 74 (> 70), car 3 `abs_steer~` |
| `X15` | all four, before the AI tick | 0/3 | `42aebd91` | = wholesale `PP` (**G-DECOMP PASS**: the split is exact, K02 `bcfc5505` too) |
| `PP` | `MASHED_PATHPROG` | 0/3 | `42aebd91` | car 1 `c1#` 13, car 2 `steer#` 100 (> 96) |

`X7` != `X15` (car 1 differs from row 214, car 3 from 248): the placement matters once progress is live. Stage 2 (`SCORE_S2.txt`): `X7` is **B 1/3** on K01, K04, K27 (default 2/3, car 1 `c1#` 10 < 17), C 0/3 = default.

## 2. Why: the consumer and the input

- **Consumer (G-MUTANT, A1).** `MASHED_PP_BOOSTSEED` (measurement-only) makes `Fi_UpdateBoostOrder` (`Vehicle/ForceIntegratorStubs.cpp`, the port of the `FUN_00470c70` descending sort by `FUN_00408a50`, which names the two cars on the 8e6
  boost arm, annotated `0x00467d55 .. 0x00467d6a` in `Integrate2.cpp:328-341`) ignore progress. `X2_BS` is **byte-identical to `D`** on both K (`774fadda`, `5052df10`). `X7_BS` differs from `D` only in `val_880` / `lap_9648`. On B and C (G-BS-B/C, A4):
  `X7_BS` has the control bytes of `D` on K01 and K26, n/3 = `D` on all of B (2/3) and C (0/3). So this sort is the only consumer of the live row in this recipe, on all three regimes.
- **Input (G-PROG-LIGHTS, A1).** `X2_H` gates, live progress at frame 1, cars 0 to 3: **0.00086 / 29.728 / 29.460 / 29.185**, descending order `[1, 2, 3, 0]`, two least `[3, 0]`. The original (`o_pp1` call 1): **30.0 / 29.728 / 29.455 / 29.183**, order `[0, 1, 2, 3]`, two least `[2, 3]` (the measured 8e6 pair).
  Cars 1 to 3 agree within 0.005 and the lap counter flips -1 -> 0 exactly at the progress wrap, frames 20 / 34 / 35 (**G-FIELD PASS**, `SCORE_FIELD_X7.txt`). The player is the only car that differs.
- **Why the player differs (A2, A3, A4, A5).** The AI BSP "nodes" are vertical gate quads (two triangles each: node 0 at z about -2.0, node 1 about -6, node 28 about +6, node 29 about +2.0, `X7_DMP2.sphere.txt`), so a car resolves to a segment by which side of a gate plane it stands on.
  The plane of node 0 at the player's x is z about -1.99902. The original's player: z -1.999037 at frame 1, **-1.998945 from frame 2** (`o_p0a.msd` `+0x960`, the first-step settle +0.0000921, NEXT_SESSION U-9684), i.e. 7.7e-5 before the plane, progress 29.99998. The port's player:
  -2.002471 (gate anchor, past the plane by 0.0034) or -1.999037 (`MASHED_ROUND_GRID` node-0 anchor, on the plane), and it never moves. Its sphere hits exactly ONE triangle, node 0, at both poses (G-HITS), so hit order is not the cause (`X7_SF` = `X7` byte for byte, G-FIRST fails as registered).
- **Counterfactual (G-PZ-PROG, G-PZ-PAIR, G-PZ-B/C, A5, A6).** `MASHED_PP_PLAYERDZ` (measurement-only) shifts the player's start z. At the original's settled z under the round-grid anchor (`+0.0000921`) or at 3.5 mm before the plane under the gate anchor: player progress **30.0000** and two least `[2, 3]` at frame 1,
  and the control bytes of `X7_PZ` / `X7_RG_PZ` equal `X7_BS` / `X7_RG_BS` on every car over the whole run (K00, K02, and `X7_PZ` on K01 to K57). Regime A is 1/3, B 2/3, C 0/3 = `D`.
- **`MASHED_ROUND_GRID` (the orchestrator's question, G-RG).** `X7_RG` and `PP_RG` are A 0/3: moving the player to the original's z alone does not help (progress 0.0000, pair `[3, 0]`; `X7_RG_BS` != `X7_RG`, the hypothesis of A2 that the pose fix restores the pair was REFUTED). With the settle added (`X7_RG_PZ`) A is 1/3 = default.
  So the grid anchor and the live progress are not the problem by themselves: once the player settles, neither costs regime A through this path.

Predictions that missed: G-FIRST (the hit set has both nodes) wrong, the A2 hypothesis (pose fix alone) wrong, G-NEAR (a rejected node-29 triangle within 0.6) wrong (the node-29 quad is 4.04 away). Held: G-NOOP, G-NOOP-BS, G-DECOMP, G-MUTANT, G-FIELD, G-PZ-*.

## 3. Default decision

Decision 1 of the PREREG: `X7` is NOT proposed as the default now (A 0/3, B 1/3). Nothing in the default path changed: `X0` = `D`, `F_DEF` / `F_OPTOUT` = `G_DEF` / `G_OPTOUT`.
Candidate path (not started, needs a user decision because it is a player-physics change on the D2 watch list): port the original's player first-step settle (z +0.0000921, y about 0.4374 -> 0.4345, `o_p0a` frames 1 to 2; the player-at-rest row of U-9684), then re-measure `X7` as the default on (b) A / B / C, the mode-7
closure (U-9685 must stay 49 / 0 / 0), collateral on h4 and G-CTRL.

## 4. Code (commits `31812fc3` .. `88c0e580`, all default OFF, `X0` = `D`)

`Race/PathProgStep.cpp`: `MASHED_PP_PUBLISH` mask (`PathProg_PublishMask`), the publish logic in `PathProg_RacePctTick`, the real-row init for bits `0x1` / `0x4`, `MASHED_PP_SPHERE_DUMP` / `MASHED_PP_SPHERE_FIRST` in the sphere query.
`D3d9Render/TrackRenderer.cpp`: the tick at either placement, `MASHED_PP_PLAYERDZ` in `StartRound`. `Vehicle/ForceIntegratorStubs.cpp`: `MASHED_PP_BOOSTSEED`. `Race/RoundGrid.cpp`: `RoundGrid_Enabled` also accepts the shadow tick with bit `0x2`.
All measurement-only knobs are unset in every default arm; no hooks.csv change (EXE-ONLY).

## 5. Proposed re-classify transaction (not applied)

- **New row (semantic):** the original's player settles +0.0000921 in z (and about -0.0029 in y) on its first step and the port's never moves. Consequence measured here: with live node progress the boost-order pair is `{3, 0}` instead of `{2, 3}`; the live row costs (b) A 0/3 and B 1/3 until it exists. Same row as the U-9684 decision "open a row for the player at rest": link them. Evidence `verify/d3_pathprog_a_20261016/RESULT_pathprog_a.md`.
- **Note on `MASHED_ROUND_GRID` default (user decision pending):** under live progress it costs regime A only through the same player-settle gap (`X7_RG` A 0/3, `X7_RG_PZ` 1/3).
- CHANGELOG line: `2026-10-16 PATHPROG split: of wholesale MASHED_PATHPROG's four parts only the live node progress moves (b) (A 0/3, B 1/3); its sole consumer is Fi_UpdateBoostOrder (mutant byte-identical to default); the cause is the port player's missing first-step settle (+0.0000921 z), which puts it on the wrong side of node 0's gate plane (progress 0.0009 vs the original's 30.0). With the settle the live row at 0x0040fc75 is (b)-neutral on A/B/C. No default changed. verify/d3_pathprog_a_20261016/RESULT_pathprog_a.md`
- Memory: the orchestrator maintains the index; no memory file edited by this session this round.

## 6. Decisions (user)

1. **DECIDED 2026-10-16 (user): do not port the player's first-step settle now. Keep the default unchanged.** The settle (z +0.0000921, y about -0.0029) stays an open row (the U-9684 player-at-rest row), not started. No code or default follows from this note.
2. Parked with it: the live row at the original's place (`MASHED_PP_PUBLISH=7` form) as a default, and the `MASHED_ROUND_GRID` default question. Both are unblocked only by the settle, and nothing here changes them. The measurement-only knobs stay in the branch, default OFF.

## 7. Raw

`run_pathprog_a.py`, `analyze_why.py`, `score_field.py`, `SCORE_S1.txt`, `SCORE_S2.txt`, `SCORE_A1.txt`, `SCORE_A4.txt`, `SCORE_A5.txt`, `SCORE_A6.txt`, `SCORE_FIELD_X7*.txt`, `*.sphere.txt`, `decomp_racepct.txt` (not here: u9685), the arms' `*.step.csv`, gates of the h4 arms.
ptrace logs and `*.log` are gitignored, their hashes are in the logs of the driver.
