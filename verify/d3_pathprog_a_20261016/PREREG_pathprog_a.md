# PREREG: why wholesale `MASHED_PATHPROG` costs regime A, split into arms (and the G-CTRL of `0020d521`)

Branch `work/pathprog_a`, base `0020d521` (race/first-frame-parity with U-9683 and U-9685 merged). Model claude-sonnet-5-5 (account2, no Ghidra MCP).
Written and committed before any arm of section 3 except the two G-CTRL arms, which were already running when this file was written (expected values
quoted from the orchestrator's message, nothing adjusted).

## 0. Task (orchestrator, 2026-10-16)

1. Confirm G-CTRL on `0020d521`: default `716cb2fd / 2a2afda1 / ba90ed88`, opt-out `MASHED_NO_RACEPCT_FUN408610` `c2e4d60d / 603ab244 / ba90ed88`.
2. Isolate why wholesale `MASHED_PATHPROG` drops (b) regime A from 1/3 to 0/3. Split its parts into separate pre-registered arms, find which part moves regime
   A and which AI input differs from the original. If the live step at the original's place (`0x0040fc75`, after the AI path) passes, propose it as the
   default and measure (b), collateral and G-CTRL. Answers the pending `MASHED_ROUND_GRID` default question (it needs PATHPROG).

## 1. What was read before writing this (code and committed data only)

- On `0020d521` the default already builds the course block and runs the verbatim `FUN_00408610` step on a PRIVATE shadow row, publishing only race_pct
  (U-9685, `PathProg_RacePctTick`). What wholesale `MASHED_PATHPROG` does on top of that (`PathProg_FrameTick`, `PathProg_RaceInit`, `PathNodes.cpp`):
  1. the step runs on the LIVE row, so **the lap counter `+0x28`** (init -1) and **the node progress `+0xc8`** (`0x008a96e8`) and **the rest of the row**
     (node id `+0x20`, probe fields, visited bitmap, ...) become readable by the rest of the port;
  2. it runs in `UpdateRace`, **before the AI tick**, where the original runs `FUN_0040da50` (call `0x0040fc75`) after the AI path (`0x0040fc6c`);
  3. the real row is zeroed and the lap counter set to -1 at race init (`FUN_00407a60`).
  (The course block and the bridge stand-down are already the default.)
- Consumers of those fields in the default port: the lap counter `0x008a9648 + v*0x30c` is read by `AiPreTickRubberBand` (`AiStandalone.cpp:1656`, the race metric
  `0x0089a880[v] = race_pct * scale + lap`, which only reaches AI speed for game modes 4 / 9, and the finish-order slots for 4 / 7 / 8 / 9); the node progress
  `0x008a96e8` is read by `Fi_UpdateBoostOrder` (`Vehicle/ForceIntegratorStubs.cpp:171`, the port of the `FUN_00470c70` sort by `FUN_00408a50`, which decides the two
  cars that take the 8e6 boost arm: "progress all equal" so far), by the AILEAF / TGT3 / ROUND_GRID readers (those need their own knobs) and by the camera.
- The original (`o_pp1.msd.pathprog.csv`, 12000 calls): the lap counter is **-1 for every car until it crosses the start line, 0 on the first call after**
  (cars 0 to 3 at calls 904 / 901 / 910 / 916), node progress 30.0 / 29.73 / 29.455 / 29.18 and race_pct 100 / 99.09 / 98.18 / 97.28 before the line, 0.013 to 0.022 and
  0.004 to 0.073 just after.
- Wholesale `MASHED_PATHPROG` per car on regime A, K00 (`SCORE_B_u9685.txt`, `u9685` round): default car 1 FAIL `abs_steer~`, car 2 PASS, car 3 FAIL `abs_steer~`;
  wholesale car 1 FAIL `c1#` 13 (< 21), car 2 FAIL `steer#` 100 (> 96), car 3 FAIL `abs_steer~`. So the cost is car 2's `steer#` by 4 and car 1's `c1#` by 8.

## 2. G-CTRL of `0020d521` (job 1)

Arms `G_DEF` and `G_OPTOUT` (run_h4 recipe, 14400 frames, private game root, a copy of the `0020d521` build). A mismatch is a failed control and stops job 2.

## 3. The split

New knob `MASHED_PP_PUBLISH=<mask>` (default 0 = the U-9685 default, no behaviour change) on the shadow-row tick. Bits: `0x1` also publish the lap counter `+0x28`
(and set the real counter to -1 at race init, as `FUN_00407a60`), `0x2` also publish the node progress `+0xc8`, `0x4` also publish the rest of the row (and zero the real row
at race init), `0x8` run the tick in `UpdateRace` before the AI tick instead of at the end of `UpdateCar` (the placement of `PathProg_FrameTick`).
Mask `0x7` = the whole live row at the original's place (`0x0040fc75`, after the AI path). Mask `0xf` = the whole live row at the wholesale placement, which should be
wholesale `MASHED_PATHPROG` exactly.

| arm | knobs | recipe | purpose |
|---|---|---|---|
| `G_DEF`, `G_OPTOUT` | none / `MASHED_NO_RACEPCT_FUN408610` | run_h4 | G-CTRL |
| `D_K` | none | run_fk, K in 0, 2 | the default on this base: the comparison for everything below |
| `X0_K` | `MASHED_PP_PUBLISH=0` | run_fk, K in 0, 2 | no-op control of the new code |
| `X1_K`, `X2_K`, `X4_K`, `X8_K` | `MASHED_PP_PUBLISH=1 / 2 / 4 / 8` | run_fk, K in 0, 2 | one part each |
| `X3_K` | `=3` | run_fk, K in 0, 2 | lap counter + node progress |
| `X7_K` | `=7` | run_fk, K in 0, 2 | the whole live row at the original's place (the proposal) |
| `X15_K` | `=15` | run_fk, K in 0, 2 | decomposition control against wholesale |
| `PP_K` | `MASHED_PATHPROG` | run_fk, K in 0, 2 | wholesale, this base |

Stage 2 (run only for `X7` and for any arm that moves A, on K in 1, 4, 27, 26, 54, 57): regimes B and C. Stage 3, h4 recipe: `X7_H`, `X15_H`, `PP_H` (14400 frames, collateral, G-DECOMP-H),
and the mode-7 stack (`MASHED_REGISTRY MASHED_PICKUPS_ORIG MASHED_REG_CARS MASHED_REG_CALLBACKS MASHED_MODE7` + traces) `M_DEF` and `X7_M` (closure retained: in-window returns of 2 = 49 / 0 / 0),
plus a repeat `X7_H2` / `X7_M2` for determinism.

| gate | statistic | pass rule | prediction |
|---|---|---|---|
| **G-CTRL** | `G_DEF`, `G_OPTOUT` hashes | the two trios of section 0 | PASS |
| **G-NOOP** | `X0_K` step hash vs `D_K` | equal on K = 0, 2 | PASS |
| **G-DECOMP** | `X15_K` step hash vs `PP_K` | equal on K = 0, 2 (and `X15_H` vs `PP_H` on h4). If not: report the differing columns, the split is then not exact | equal (70 %) |
| **G-SPLIT** | (b) regime A n/3 per arm, `score_regimes.py` | report; an arm "moves A" if its n/3 on K00 or K02 differs from `D_K` | the mover is bit `0x2` (50 %), `0x1` (15 %), `0x8` alone (15 %), only a combination (20 %) |
| **G-WHY** | for each arm that moves A: the first AI step row (any control byte `c0 c1 c3 c4 c5`) that differs from `D_K`, per car, and the input columns that differ first (`lap_9648`, `prog`, `rmetric`, `ra_ec`, `val_880`, `rec_*`) | report, with the consumer named from the source | named |
| **G-FIELD** | `X7` published fields vs the original's `o_pp1` timeline, crossing-aligned (original call c <-> exe step frame = the car's crossing frame): lap counter -1 until the car's crossing and 0 after; node progress within 0.05 of the original's pre-line values (30.0 / 29.73 / 29.455 / 29.18 for cars 0 to 3 after the first call) | pass | PASS |
| **(b) per regime** | `score_regimes.py` n/3, `X7_K` vs `D_K` | `X7` >= default in every regime on all 8 K (default A 1/3, B 2/3, C 0/3) | `X7` A 0/3 like wholesale (50 %) |
| closure | `X7_M` in-window returns of 2 (original window 204 / 175 / 182) | 49 / 0 / 0 | retained (70 %) |
| collateral | `collateral.py --mode paired`, `D` vs `X7_H` (floors `G_OPTOUT`, `X7_H2`) and ptrace | report; the player's ptrace must not change before the first AI step-field divergence | report |

**Decisions, pre-committed (the mechanism is not).**

1. **Propose `X7` as the default** (publish the lap counter, node progress and row, at the end of `UpdateCar`, opt-out `MASHED_NO_PP_LIVE`) only if G-CTRL, G-NOOP, G-FIELD, determinism and the closure
   gate pass, (b) `X7` >= default in every regime on all 8 K, and collateral is clean. Then the default flip is built and G-CTRL re-measured (new trio recorded). Otherwise no default changes.
2. If `X7` fails (b): the split names the part (G-SPLIT) and the AI input (G-WHY). A candidate fix is pre-registered in an amendment before it is built. If the mover
   is an input the original also has (lap counter / progress timeline equal to `o_pp1`), the cost is a downstream port difference, not the field, and the report names the consumer.
3. The `MASHED_ROUND_GRID` answer is read from `X7` / `X15`: if the live row costs regime A, `MASHED_ROUND_GRID` (which needs the live progress) inherits that cost, and the report says so.
4. If G-CTRL fails nothing else is scored. Exe-side runs only: the committed original captures (`o_pp1`, `o_rd3`, the (b) envelopes) are the references. No new original capture is planned.

## 3a. AMENDMENT A1 (committed after the stage 1 ladder was scored, before any arm below exists)

Seen first (`SCORE_S1.txt`, `analyze_why.py`, `run_s1.txt`; G-CTRL passed: `G_DEF` `716cb2fd / 2a2afda1 / ba90ed88`, `G_OPTOUT` `c2e4d60d / 603ab244 / ba90ed88`):

- **G-NOOP passes**: `X0` = `D` (`774fadda` on K00, `5052df10` on K02).
- **G-DECOMP passes**: `X15` = wholesale `PP` on both K (`42aebd91`, `bcfc5505`). The four-part split is exact.
- **G-SPLIT**: regime A is 1/3 for `D`, `X0`, `X1`, `X4`, `X8` and 0/3 for `X2`, `X3`, `X7`, `X15`, `PP`, on both K. The mover is bit `0x2`, the live node progress `0x008a96e8`.
  `X1` (lap counter) changes only `val_880` and `lap_9648` (no control byte, 0 of 1018 / 285 rows), `X4` (rest of the row) and `X8` (placement of the race_pct-only tick) are byte-equal to `D`.
  `X3` = `X7` (same hash): the rest of the row adds nothing. `X7` != `X15`: the placement matters once progress is live (car 1 differs from row 214, car 3 from row 248).
- **G-WHY, first look**: with `X2` car 2's `rec_9e4` already differs at step row 2 (frame 3), before any control byte (first control-byte difference car 2 frame 3, car 1 frame 103, car 3 frame 122).
  That is a physics difference at the start, not a late AI drift.

Hypothesis registered now, to be tested, not a result: the consumer is `Fi_UpdateBoostOrder` (`ForceIntegratorStubs.cpp:171`, the port of the `FUN_00470c70` descending sort by `FUN_00408a50`), which with progress
all 0 keeps the seeded pair `{2, 3}` as the two cars on the 8e6 boost arm. The original's progress at the lights is 30.0 / 29.73 / 29.455 / 29.18 for cars 0 to 3 (`o_pp1`, call 1), so the two LEAST progressed cars are `{2, 3}`.
The port's player sits just past the line (race_pct 0.003, progress about 0, `R_H` gates frame 1), so with live progress the two least are `{3, 0}`: car 2 loses the 8e6 arm and the player gains it.

New arms (run_fk, K in 0, 2, except `X2_H`):

| arm | knobs | purpose |
|---|---|---|
| `X0_BS`, `X2_BS`, `X7_BS` | `MASHED_PP_PUBLISH=0 / 2 / 7` + `MASHED_PP_BOOSTSEED=1` | mutant: the boost-order sort ignores progress (seeded `{2, 3}`), measurement-only. `X0_BS` is its no-op control (must equal `D`) |
| `X2_H` | `MASHED_PP_PUBLISH=2`, run_h4 | read the published progress at the lights (`prog_96e8` in the gates dump) |
| `X7_RG`, `PP_RG` | `MASHED_PP_PUBLISH=7` / `MASHED_PATHPROG`, + `MASHED_ROUND_GRID` | the `MASHED_ROUND_GRID` question: the grid anchor at the start node, which needs live progress (`RoundGrid_Enabled` now also accepts the shadow-row tick with bit `0x2`) |

| gate | statistic | pass rule | prediction |
|---|---|---|---|
| **G-NOOP-BS** | `X0_BS` vs `D` step hash | equal on K = 0, 2 | PASS |
| **G-MUTANT** | `X2_BS` vs `D`: control bytes (`c0 c1 c3 c4 c5`) of every car over the whole run, and regime A n/3 | control bytes equal and A 1/3 on both K => the boost order is the whole consumer of the live progress; anything else is reported with the first differing row | PASS (70 %) |
| G-PROG-LIGHTS (report) | `X2_H` gates `prog_96e8` frames 1 to 5, cars 0 to 3 | report, against the original's 30.0 / 29.73 / 29.455 / 29.18 | player about 0, cars 1 to 3 within 0.05 of the original |
| **G-RG** (report, the orchestrator's question) | regime A n/3 of `X7_RG`, `PP_RG` on K = 0, 2 | report against `D` (1/3) | `X7_RG` A 1/3 (40 %), 0/3 (45 %), other (15 %) |

Decision 1 of section 3 stands unchanged: `X7` fails (b) (A 0/3), so it is NOT proposed as the default. Decision 2 applies: the part is the node progress, the consumer is the boost-order sort if G-MUTANT passes,
and the fix is not the field but the input it reads (the player's grid pose relative to the line), which belongs to the grid arms (`MASHED_ROUND_GRID`), reported by G-RG.
If G-MUTANT passes AND `X7_RG` or `PP_RG` restores regime A to >= 1/3 on both K, the next arms (regimes B / C, h4 collateral, closure) for that combination are pre-registered in a second amendment before they run.

## 3b. AMENDMENT A2 (committed after the A1 arms were scored, before any arm below exists)

Seen first (`SCORE_A1.txt`, `X2_H`):

- **G-NOOP-BS passes** (`X0_BS` = `D`). **G-MUTANT passes, completely**: `X2_BS` is byte-identical to `D` on both K (`774fadda`, `5052df10`, A 1/3). `X7_BS` differs from `D` only in `val_880` / `lap_9648` (A 1/3).
  With the boost-order sort ignoring progress, the live node progress and the whole live row change no control byte: `Fi_UpdateBoostOrder` is the sole consumer in this recipe.
- **G-PROG-LIGHTS** (`X2_H` gates): frame 1 live progress cars 0 to 3 = 0.00086 / 29.728 / 29.460 / 29.185, descending order [1, 2, 3, 0], two least = [3, 0]. The original has 30.0 / 29.73 / 29.455 / 29.18 (two least = [2, 3]).
- The original player's world position at frame 1 (`o_p0a.msd`, `+0x958 / +0x960`) is x 0.500937, z **-1.999037**; the port's gate anchor puts it at z **-2.00247** (0.0034 beyond, 0.00086 nodes past the line). The `MASHED_ROUND_GRID` node-0 anchor
  (`RESULT_KNOB_ON.md`: z -1.99904) is the original's pose.
- **G-RG**: `X7_RG` and `PP_RG` are A 0/3 on both K, so the pose fix alone does not give A back. Car 2 fails `steer#` (110 / 102 vs <= 96), the same car and item family as `PP`, further out. The original's car 2 has `c1#` 50, `steer#` 85; the port's `D` has 67 / 95 (passing by 3 and 1 against the envelope maxima 70 / 96).

Hypothesis registered now, to be tested, not a result: under `MASHED_ROUND_GRID` the player is before the line, the boost pair is already `{2, 3}`, and the remaining regime-A failure is not the live progress but the port's car-2 command distinctness against the envelope.

New arms:

| arm | knobs | recipe | purpose |
|---|---|---|---|
| `X7_RG_BS_K` | `MASHED_PP_PUBLISH=7` + `MASHED_ROUND_GRID` + `MASHED_PP_BOOSTSEED` | fk, K in 0, 2 | mutant under the faithful pose: if the pair is already `{2, 3}` this equals `X7_RG` on every control byte |
| `X7_RG_H` | `MASHED_PP_PUBLISH=7` + `MASHED_ROUND_GRID` | h4 | progress at the lights under the pose fix (gates `prog_96e8`) |
| `X7_H` | `MASHED_PP_PUBLISH=7` | h4 | G-FIELD: the published lap counter (`lap_9648`) and progress against the original's `o_pp1` timeline |
| `D_K`, `X7_K` | none / `MASHED_PP_PUBLISH=7` | fk, K in 1, 4, 27, 26, 54, 57 | stage 2: regimes B and C |

| gate | statistic | pass rule | prediction |
|---|---|---|---|
| **G-MUTANT-RG** | `X7_RG_BS` vs `X7_RG`: control bytes of every car over the whole run, K = 0, 2 | equal => under the pose fix the pair is already `{2, 3}` and the boost order is not what fails A there | equal (75 %) |
| G-PROG-RG (report) | `X7_RG_H` gates `prog_96e8` frames 1 to 5, cars 0 to 3 | report; the player should be above the three AI cars' progress (30.0-like) | player highest |
| **G-FIELD** | `X7_H`: lap counter `lap_9648` and progress per car vs `o_pp1`, crossing-aligned | lap counter -1 until the car's crossing and 0 after for cars 1 to 3; progress pre-line within 0.05 of the original's for cars 1 to 3; the player's progress is reported (expected to differ, 0.00086 vs 30.0) | PASS for cars 1 to 3 |
| (b) stage 2 | `score_regimes.py` n/3, `X7_K` vs `D_K`, K in 1, 4, 27 (B) and 26, 54, 57 (C) | report (default B 2/3, C 0/3) | equal to `D` (70 %) |

Decision 1 of section 3 is unchanged and `X7` is not proposed as default if A 0/3. If G-MUTANT-RG passes, the report states the chain: live progress -> `Fi_UpdateBoostOrder` pair -> the player's pose relative to the line; the fix of the input is the grid anchor
(`MASHED_ROUND_GRID`, a user decision), and the live row then needs a second (b) look because regime A is 0/3 for a different reason (car 2's command distinctness).

## 3c. AMENDMENT A3 (committed after the A2 arms were scored, before any arm below exists)

Seen first (`SCORE_S2.txt`, `SCORE_FIELD_X7.txt`, `SCORE_FIELD_X7RG.txt`, `analyze_why.py`):

- **Stage 2**: regime B n/3 is 1/3 for `X7` on K01, K04 and K27 (default 2/3), regime C is 0/3 for both. The live row costs B as well as A.
- **G-FIELD passes for cars 1 to 3** (`X7_H`): published progress at frame 1 within 0.005 of the original (29.7277 / 29.4599 / 29.1849 vs 29.7277 / 29.4549 / 29.1828), lap counter -1 until exactly the frame the progress wraps (20 / 34 / 35), then 0, as the original.
  The player is the only car that differs: 0.0009 (gate anchor) against the original's 30.0.
- **The A2 hypothesis is refuted**: under `MASHED_ROUND_GRID` the player stands at the original's z (-1.99904) yet its published progress at frame 1 is **0.0000**, so the two least are still `[3, 0]`, and `X7_RG_BS` is NOT equal to `X7_RG`
  (car 2 differs from frame 3). Moving the pose to the original's z did not move the node the player resolves to.
- So the open link is **which node the probe resolves for a car that stands on the start line**: the original resolves the player to node 29 (progress 30.0, race_pct 99.9999 / 100.0), the port to node 0 (progress 0.0000 to 0.0009). The only code between the pose and the node is the sphere query (`PP_SphereQuery`, the port's walk of the AI-BSP triangles
  in stream order, `FUN_00407a90` keeps the LAST hit, `[UNCERTAIN U-9209]` in the source: the within-sector order of the original is not reproduced).

Hypothesis registered now, to be tested, not a result: the player's sphere (centre y 0.47, r 0.6) hits triangles of both node 29 and node 0, and the stream-order last hit is node 0 where the original's order leaves node 29 last.

Measurement-only knobs (default OFF): `MASHED_PP_SPHERE_DUMP=<file>` logs every hit of car 0's first 6 sphere queries (stream index, node id, distance); `MASHED_PP_SPHERE_FIRST` makes the first hit win instead of the last.

| arm | knobs | recipe | purpose |
|---|---|---|---|
| `X7_DMP`, `X7_RG_DMP` | `MASHED_PP_PUBLISH=7` (+ `MASHED_ROUND_GRID`) + `MASHED_PP_SPHERE_DUMP` | fk, K00 | the player's hit set and order |
| `X7_SF_K`, `X7_RG_SF_K` | `MASHED_PP_PUBLISH=7` (+ `MASHED_ROUND_GRID`) + `MASHED_PP_SPHERE_FIRST` | fk, K in 0, 2 | first hit wins (mutant) |
| `X7_SF_H` | `MASHED_PP_PUBLISH=7` + `MASHED_PP_SPHERE_FIRST` | h4 | the player's published progress at the lights (gates) |
| `X7_SFD_K` | `MASHED_PP_PUBLISH=0` + `MASHED_PP_SPHERE_FIRST` | fk, K in 0, 2 | no-op control of the mutant: with nothing published but race_pct it must equal `D` on every control byte |

| gate | statistic | pass rule | prediction |
|---|---|---|---|
| G-HITS (report) | `X7_DMP`, `X7_RG_DMP` sphere dumps, queries 0 to 5 | the node ids hit and their order | both node 29 and node 0, node 0 last (70 %) |
| **G-SF-NOOP** | `X7_SFD_K` control bytes vs `D` | equal | PASS |
| **G-FIRST** | `X7_SF_H` gates `prog_96e8` frame 1, car 0 | >= 29.99 (the original's 30.0) and the two least `[2, 3]`; then the control bytes of `X7_SF_K` equal `X7_BS_K` (the seeded-pair counterfactual, the u9683-style `X7_BS` of A1) | PASS (45 %) |
| (b) | `X7_SF_K`, `X7_RG_SF_K` n/3 on regime A, K = 0, 2 | report against `D` (A 1/3) | `X7_SF` A 1/3 if G-FIRST passes |

Decision 1 of section 3 stands. If G-FIRST passes, `X7` + first-hit-wins is a faithful-pair counterfactual, NOT a fix: the original's order inside a sector is not known, so the report names the open item (the original's BSP triangle visiting order for the node-29 / node-0 triangles at the start line, `U-9209`) and no default changes.
If G-FIRST fails, the report states that the player's node is decided by something other than the hit order (pose y, sphere overlap), with the dump as the evidence.

## 4. Not done here (registered)

- No hooks.csv change (`FUN_00408610` is an EXE-ONLY naked transcription). No tracker is edited by hand. The memory index is not touched (orchestrator's).
- `MASHED_ROUND_GRID`, `MASHED_TGT3`, `MASHED_AILEAF` stay default OFF and are not measured here.
