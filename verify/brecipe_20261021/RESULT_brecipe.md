# RESULT brecipe: the faithful B recipe (rule 0, game mode 10, live words) leaves every B dump, (b) and the regime map unchanged; G-CTRL moves (2026-10-21)

Branch `work/brecipe` from `256a6b2de`. PREREG `PREREG_brecipe.md` (`b346a6e1e`), committed before every knob arm. Base exe `37f3f820`,
knob exe `d33d9362` (copies in `D:\MashedWT\brecipe_data`), `MASHED_NO_DEPLOY=1`, private root `D:\MashedWT\brecipe_root`. Evidence
`D:\MashedWT\brecipe_data` (`MOVED_OFF_GIT.txt`, `arms\RUNLOG.txt`). Nothing flipped, no canonical recipe or scorer changed, no tracker
edit (no C-level, no uncertainty filed or resolved; next free id on the base is U-9782).

## 1. Facts (detail and citations: PREREG section 1)

- **Why rule 4.** First set on 2026-10-07 (`verify/d3_consumer_20261007/PREREG_CONSUMER.md:53-56`) to reach the port's rule-gated
  metric consumer (`UpdateFinishOrder`, rules 4/7/8/9) on the Training recipe. Every later recipe copied it with no new reason. No
  source picked it to match an original capture.
- **What the original runs.** The B captures are launcher pokes, not a frontend route (`scenario_launch.py` `setup()` `:3992-4016`):
  `0x0067e9fc` = 10, `0x007f0fd0` = 0, `0x0067ea64` = 0, track 0, car 0, slot types 1,2,2,2 through FUN_0040e480, `0x008a94d0` 4 from
  the game's spawn loop. `o_ep_1.log`: `[setup] set track=0 mode=10 cars=4 car=0 rule=0 team=0`.
- **What the port read on the current recipe.** rule_ 4 (`MASHED_ROUND_RULE=4`), live `0x0067e9fc` 0 (never written on the
  `MASHED_ROUND` route), live `0x007f0fd0` 0 (never written). AI host stand-ins: FUN_0042f6a0 `return 3`, `DAT_007f0fd0` `return 0`,
  FUN_0040e470 `v == 0 ? 0 : 2`. Leaves on `MASHED.exe.unpatched`: FUN_0042f6a0 `mov eax,[0x67e9fc]`, FUN_0040e470
  `mov ecx,[0x5f2770]` (0x0040e474) / `mov eax,[ecx+eax*4+0x34]` (0x0040e47a), FUN_0042f500 `mov eax,[0x67ea64]`, FUN_0040e340
  `mov eax,[0x8a94d0]`.

## 2. Change (default OFF, knobs unset = base: P0 holds)

| knob | what |
|---|---|
| `MASHED_MODE_LIVE` | AI host FUN_0042f6a0 / `DAT_007f0fd0` read the live words; `SetRaceRule` writes rule_ into `0x007f0fd0`; `MenuLaunchRace` writes the launch mode into `0x0067e9fc`; the `MASHED_ROUND` route writes 3 when `MASHED_GAME_MODE` is unset |
| `MASHED_GAME_MODE=<2..10>` | now also writes `0x0067e9fc` on the `MASHED_ROUND` route (it already forced the frontend launch mode) |
| `MASHED_SLOT_TYPE_LIVE` | AI host FUN_0040e470 reads the live slot table |

`MATCH-SEED` now logs the live words and the host's reads. On F every arm logs `rule=0 mode_e9fc=10 fd0=0 host_mode=10 host_fd0=0`.
At StartMatch the live slot table reads `1,0,0,0`: the AI slots become 2 later through the countdown alive-poke (FUN_00418860's
FUN_0040e480(v, 2), `Ai_CountdownAlivePoke`).

**Faithful recipe F** = current + `MASHED_ROUND_RULE=0 MASHED_GAME_MODE=10 MASHED_MODE_LIVE MASHED_SLOT_TYPE_LIVE`.

## 3. Gate table, both recipes (knob exe `d33d9362`)

| gate | current recipe | F (faithful) |
|---|---|---|
| G-CTRL step / gates / ptrace | `15594ec8 / 7285d708 / 26a6e4ad` (P0) | `ce087436 / b0a2973b / c4f7a000` |
| B K0 / K1 / K26 | `7ab70c5f / 15262e4a / d1b4bc64` (P0) | **same** |
| B + IL | `9bd86ffe / 433abcd2 / 1a1ff727` | **same** |
| B + R7 (ER + E7 + TL) | `400a045d / d978840c / 9ce021cc` | `400a045d / d978840c / 56b4a235` (x2) |
| B + IL + R7 | `803ab845 / 894dd04c / 2136aa4b` | `803ab845 / 894dd04c / a813c98c` |
| flag_a368 per K | 0 / 0,1 / 0,2 | **same** (K0 = A, K1 = B, K26 = C) |
| (b) | A 3/3, B 2/3 (car 2), C 3/3; IL 9/9; R7 same; IL+R7 9/9 | **same** in every arm |
| D2 x3 alone | (u9779 default band) | 0.1900 / 0.2550 / 1965.35, 1965.67, 1964.25, H1 398/400: **PASS 3/3** |
| 13 tracks | (default 873 rows, 0 RED) | 39 frames, 873 rows, all cited, **0 RED**, instrument gates PASS |

Regime map: unchanged. K0 still matches `o_c3` (flag_a368 stays 0 on K0).

First diverging call per car on K0 (`stepcmp2.py`: c0, c1, c3, c4, c5, substate, rec_9e4, clock), identical for the current recipe and F,
against both `o_cc3_1` and `o_ep_1`:

| stack | car 1 | car 2 | car 3 |
|---|---|---|---|
| plain | none through 219 | 205 `c0` | 206 `rec_9e4` |
| IL | none through 219 | none through 219 | 220 `c4 c5 substate` |
| IL + R7 | none through 219 | none through 219 | 221 `rec_9e4` (U-9781, sub-state-7 hold) |

`own_x` / `own_z` differ from k=0 at both the same-k and the k+1 alignment on every arm, so they are left out (memory
`posdrift-reads-ai-own-pos-not-physics`).

## 4. Attribution (each step of F)

| stack | B K0 / K1 / K26 | G-CTRL step / gates / ptrace |
|---|---|---|
| F0 = rule 0 | = current | `2e01286e` / `7285d708` / `26a6e4ad` |
| F0M = + game mode 10 | = current | `2e01286e` / `c634bbac` / `26a6e4ad` |
| F0ML = + `MASHED_MODE_LIVE` | = current | `2e01286e` / `c634bbac` / `26a6e4ad` |
| F = + `MASHED_SLOT_TYPE_LIVE` | = current (R7 K26 moves, below) | `ce087436` / `b0a2973b` / `c4f7a000` |
| ST = current + `MASHED_SLOT_TYPE_LIVE` | = current | `a858f574` / `4debc12f` / `c4f7a000` |
| ML = current + `MASHED_MODE_LIVE` | `46475132 / d575ae32 / 46475132` | `6f4bcac1` / `9c318090` / `f5d44e2d` |

- **Rule 4 vs 0 (P1 holds on B, G-CTRL moves).** Inside 1200 frames no car's metric reaches 3.0 and no score passes 11, so the B dumps
  are byte-equal. On G-CTRL round 4 ends at scores `0,3,10,12`. Rule 0: `RULE-EVAL ... r=4` and `MATCH OVER t=74.8s` (FUN_00410510's
  score > 11 with 4 participants). Rule 4: EvaluateResult's case 4 replaces the score-scan winner with the first finisher + 1 (none,
  0), so `r=0`, a fifth round starts, and the match ends at 77.9 s on the separate check. First step difference: frame 3771, car 3,
  `rmetric` / `rtick`. The gates csv and the player trace do not move.
- **Game mode word (P2 holds).** Writing 10 moves only the gates csv column `m42f6a0` (0 -> 10, all 54712 rows). Step and ptrace equal.
- **`MASHED_MODE_LIVE` on F (P3 holds).** The host reads 10 instead of 3, and every host consumer tests the set {3,4,5,10}. fd0 stays 0.
  No hash moves.
- **`MASHED_SLOT_TYPE_LIVE` (P4).** The B dumps without the u9779 knobs do not move. With them, K26 moves (on either recipe: STR7 =
  FR7 = `56b4a235`). First difference: frame 479, car 2, `c7`, in round 2 (the round-2 resolve logs scores `2,9,6,7` before,
  `2,6,6,10` after). On G-CTRL the first F0 -> F difference is frame 1194, car 2, `c7`. `c7` is `ctrl[7]`, written only by
  AiFireDecision's fire (`AiStandalone.cpp:868`), and in AiFireDecision the only input the knob changes is LeaderInRange's
  `FUN_0040e470(c) == 1` scan (FUN_00415190, `AiStandalone.cpp:844-851`), which never matched the player under the stand-in. Both moves
  are outside every (b) window (220 / 220 / 196 calls per car).
- **ML, the contrast (P5 holds).** Under `MASHED_MODE_LIVE` the current recipe's rule 4 reaches the AI: fd0 4 sends every AI step to
  ControlStepM49 (FUN_00416a30), flag_a368 reads 2 from the first row on every K, and (b) scores **0/3** on every K. The current recipe
  matches the original's AI only because the stand-in hides its rule from the AI.

## 5. What a canonical swap to F would move

- **Hashes.** G-CTRL trio `15594ec8 / 7285d708 / 26a6e4ad` -> `ce087436 / b0a2973b / c4f7a000`. B K0 / K1 / K26 and B + IL: none.
  B + R7 K26 `9ce021cc` -> `56b4a235`, B + IL + R7 K26 `2136aa4b` -> `a813c98c` (only arms with the u9779 knobs).
- **(b), regime map, flag_a368, first-divergence rows:** none.
- **Scripts that set `MASHED_ROUND_RULE=4`:** the lane `run_arms.py` lineage `base_env` (57 lane copies carry the variable; the next
  lane copies the newest), `verify/d1r2_20261016/run_gctrl.py:40`, `verify/d1r2_20261016/run_replay.py:35`,
  `verify/d3_gatefire_20261008/run_h4.ps1:127` (and the older `run_h2` / `run_gf0` / `run_a364` / `run_m5` / `run_wshape.ps1`,
  `sa_*_poll.py` there), `verify/fe_residuals_20261016/run_h4_wt.ps1` and `run_fx.py`, `verify/rsky_20261016/run13.py:79`. A swap
  would add `MASHED_ROUND_RULE=0 MASHED_GAME_MODE=10` to the live ones (and the two knobs, or their flip). Older lane copies stay as
  provenance.
- **`re/tools/b_envelopes.json`:** no change. The envelopes come from the original captures, and the regime labels did not move.
- **Memories:** `default-ctrl-hashes-after-b2-flip` (new trio), `race-metric-consumer-is-rule-engine-not-fd0` (rule 4 was the reason
  the consumer was reachable; on rule 0 UpdateFinishOrder returns at once). `b-recipe-k0-matches-o-c3` still holds.

## 6. 13 tracks

`run13.py run` with the four F settings appended (they override run13's `MASHED_ROUND_RULE=4`), exe `d33d9362`, root
`D:/MashedWT/brecipe_root`: all 13 tracks OK on the first attempt. `score` against `verify/rres_20261016/cites_rres.json`: 39
frames, 873 rows, every row cited, 0 RED, INSTRUMENT GATES: PASS (`D:/MashedWT/brecipe_data/tracks_F_score.txt`). Same totals as
u9779's run.

## 7. Decisions for Mariano

1. **Recipe.** Swap the canonical B / G-CTRL recipe to F, or keep rule 4. On B nothing measurable moves. On G-CTRL the rule-0 match
   ends at round 4 as FUN_00410510 does, where rule 4 runs a fifth round.
2. **`MASHED_MODE_LIVE` flip.** Inert on F. On the current recipe it sends the AI down the rule-4 path and (b) drops to 0/3, so it can
   only flip together with the recipe swap (1).
3. **`MASHED_SLOT_TYPE_LIVE` flip.** Removes the player-type stand-in that kept LeaderInRange from ever matching the player. Moves the
   G-CTRL ptrace and the R7 K26 dump. (b), D2 and the B dumps do not move.
