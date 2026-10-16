# PREREG: U-9683, refdist producer order vs the AI tick (and U-9685)

Written and committed before any measuring run. Branch `work/u9683`, base `1caf1a10` (race/first-frame-parity).
Model: claude-sonnet-5-5 (account2, no Ghidra MCP; bytes read with `re/tools/decomp_pc.py`, read-only pool slot).

## 0. What was read before writing this

Bytes only (raw output committed beside this file: `decomp_order.txt`, `decomp_order2.txt`, `xrefs_order.txt`,
`xrefs_order2.txt`, `callees_18560.txt`). Nothing was measured.

**The original's per-frame order (G-ORDER, from the bytes).** `FUN_0040fc00` (the per-frame race tick, sole caller
`FUN_004111c0`) calls, in address order:

| call site | callee | role |
|---|---|---|
| `0x0040fc6c` | `FUN_00425a40` (guarded by `DAT_005f29c0 != 0`) | AI spine + physics |
| `0x0040fc75` | `FUN_0040da50` | per-car progress / race_pct (`FUN_00408610` loop), car registrants |
| `0x0040fc7c` | `FUN_0040d470` | camera / director |

Inside them:

- **Consumer chain.** `FUN_00425a40` -> `0x00425a73` `FUN_00418860` -> `0x00418935` / `0x00418967` `FUN_00418560` ->
  `0x004187b7` `FUN_00416250` -> `0x004163c3` `FUN_00414c30`. Two more callers of the consumer:
  `0x00416bad` in `FUN_00416a30` (called at `0x004187c6`) and `0x00417f13` in `FUN_00417da0` (called at `0x00418780`).
  After the AI spine `FUN_00425a40` runs the physics: `FUN_00424eb0`, `FUN_00470c70`, `FUN_00422ba0`, `FUN_004252c0`.
- **Producer chain.** `FUN_0040d470` -> `0x0040d4f3` `FUN_00448220` -> `0x004483c2` `FUN_00446520` ->
  `0x004469b6` `FUN_00442a60`. `FUN_00442a60` has exactly one caller. The producer branch of `FUN_00446520` is taken for
  `FUN_0040e350() == 6 && FUN_0042f6a0() != 0xb` (decomp `FUN_00446520` lines 156-157).

So in one frame the original runs AI consumer, then physics, then progress, then the refdist producer. The consumer
of frame f reads the array the producer wrote in frame f-1, from the end-of-frame-(f-1) state. This is the same
fact `verify/d3_u9219_20261013` measured (G-PROBE' 561 / 561: the consumer at clock c reads the producer output of
c - 50), now with the call sites that cause it.

**The port's order.** `TrackRenderer::UpdateCar` (`TrackRenderer.cpp`): player physics (`:3446`) -> `UpdateRace`
(`:3666`; inside it `RaceComputeDistancesTick()` `:5836`, `PathProg_FrameTick()` `:5840`, registry ticks, race_pct bridge)
-> `Ai_AdvanceClock` (`:3711`) -> `Ai_Standalone_Tick` (`:3713`) -> opponent physics loop (`:3753..:3845`). So the port's
consumer reads a value computed in the SAME frame, before the opponents' physics of that frame and from the player's
position after this frame's player step. Two ordering residues beyond the producer, **registered and not touched here**:
(1) `PathProg_FrameTick` (the port's `FUN_0040da50` loop) also runs before the AI tick where the original runs it after;
(2) the player steps before `UpdateRace` where the original steps every car inside `FUN_00425a40`, after the AI.

## 1. The arm

`MASHED_REFDIST_AFTER_AI=1` (opt-in, default OFF in this build; the default decision is section 4). With it set:

- `UpdateRace` skips `RaceComputeDistancesTick()`.
- `UpdateCar` calls it once, after the whole AI / opponent block (just before `yaww::Frame()`), i.e. after every car's
  physics and after `PathProg_FrameTick` of the same frame. That is the producer's place in the original relative to
  physics, progress and the AI tick. The AI of frame k then reads the array from the end of frame k-1.

Measurement-only trace `MASHED_REFDIST_TRACE=<file>` (default OFF, writes no game state): one row per producer call
(`P`: site `U` = UpdateRace, `A` = after-AI, frame ordinal, `ba8c`, clock `0x007f0ff4`, `refdist[0..3]` bits, ref, other)
and one row per AI tick entry (`T`: frame ordinal, `refdist[0..3]` bits as the consumers will read them). The frame
ordinal is a counter bumped once per `UpdateCar` race frame, before player physics, so it does not depend on the order
under test.

## 2. Instruments and arms

Driver `run_u9683.ps1` (worktree root, `MASHED_ROOT` = main repo for assets, spawns and stops only its own PIDs).
`run_h4` recipe (Training, car 1, round 1, rule 4, `MASHED_DETERMINISTIC`, PLAYERTRACE, AI_STEPDUMP, U9186_GATES,
14400 frames) and `run_fk` recipe (same, 1200 frames, `MASHED_RING_SKIP` = K, step dump only).

| arm | exe | knobs | purpose |
|---|---|---|---|
| H_HEAD | HEAD build (`mashed_re_head.exe`) | none | G-CTRL before (machine check) |
| H_OFF | new build | none | G-CTRL after = no-op control for the added code |
| H_ON, H_ON2 | new build | `MASHED_REFDIST_AFTER_AI` | determinism floor of the ON arm, collateral |
| M_OFF | new build | mode-7 stack + `MASHED_REFDIST_TRACE` | G-REFDIST, G-MODE7, U-9685 control |
| M_ON | new build | mode-7 stack + `MASHED_REFDIST_AFTER_AI` + trace | the same, knob ON |
| B_OFF K, B_ON K | new build | K in 0,2 (A), 1,4,27 (B), 26,54,57 (C), 1200 frames | (b) per regime |

Mode-7 stack = `MASHED_REGISTRY MASHED_PICKUPS_ORIG MASHED_REG_CARS MASHED_REG_CALLBACKS MASHED_MODE7
MASHED_MODE7_TRACE=<x>.c30.tsv MASHED_PKCB_TRACE=<x>.cb.tsv` (the `M_FIX` recipe of
`verify/d3_car2pickup_20261013`). Original references (committed, no new original run): `o_rd2` = `o_rd3`
(`verify/d3_u9219_20261013`, driving consumer calls 204 / 175 / 182 per car, refdist 0 on 164 / 4 / 29, returns of 2
on 49 / 0 / 0); (b) envelopes `re/tools/b_envelopes_regimes.json` via `score_regimes.py` (A / B 220 calls, cars
pooled, C 196 per car).

## 3. Gates, predictions, decisions

| gate | statistic | pass rule | prediction |
|---|---|---|---|
| **G-ORDER** | the call sites in section 0 | the producer call site `0x0040fc7c` chain follows the consumer call site `0x0040fc6c` in `FUN_0040fc00` | PASS (already read) |
| **G-CTRL-HEAD / -OFF** | `H_HEAD`, `H_OFF` step / gates / ptrace sha8 | both equal `991a5fff / be0d47ea / bc75b405` | PASS. A mismatch on `H_HEAD` means the machine or the recipe drifted: stop, no claim is made from this session |
| **G-TOOK** | `M_OFF` / `M_ON` traces | ON: zero `U` rows, one `A` row per `T`, and for >= 99 % of `T` rows the latest `P` row has frame ordinal k-1. OFF: zero `A` rows and ordinal k. Plus the array at every `T` row equals the latest preceding `P` row's output (>= 99.9 %) | PASS 100 % (mechanical) |
| **G-ARM-DET** | `H_ON` vs `H_ON2` hashes | identical (the port is bit-deterministic across boots) | PASS |
| **G-REFDIST** | per car, count of consumer calls with refdist 0 in the first 204 / 175 / 182 driving calls, exe vs original 164 / 4 / 29 | `|exe - orig| <= 3` per car (the reproducible original round has spread 0, so the floor is the 3) | FAIL on cars 1 and 3 in BOTH arms (the M4_ON counts are 102 / 20 vs 164 / 29 and the cause is pace, U-9185 / U-9186). Report only for ON vs OFF |
| **G-REFDIST-VAL** (report) | per car, fraction of driving-call-aligned consumer refdist values within `0.01 + 0.08 * orig` of the original | ON minus OFF, pooled cars | `|ON - OFF| < 3` points (the lag moves cars by one step). An improvement is a rise >= 5 points |
| **G-MODE7 / U-9685** | `ret == 2` consumer calls of car 2 inside the first 175 driving calls on the mode-7 stack | `M_ON` = 0 (original 0; `M_OFF` = 2 at driving calls 16 and 17, frames 850 and 900) | **U-9685 does NOT close (60 %)**: those two calls have held word 0 and refdist 0 on a grid whose pose is 1.6 cm off, and a one-frame lag moves the cars by at most one step |
| **G-MODE7-X** (report) | same for cars 1 and 3, pickup tick of car 2 (original tick 42, `M_FIX` tick 37 = delta -5, RESULT_CAR2PICKUP.md section 4) | report | unchanged |
| **(b) per regime** | `score_regimes.py` n/3 per regime, OFF vs ON | report both | A / B / C n/3 equal OFF (current default A 1/3, B 2/3, C 0/3). Per-car item verdicts identical on >= 90 % of items |
| **collateral** | same-side OFF vs ON on `H_*`: first divergent frame per car, ptrace equal / not, step and gates equal / not; `re/tools/statediff/collateral.py --mode paired` on the `.msd` if one is produced | report, ranked by magnitude | the first divergence is at or after the first frame where the two arms' refdist arrays differ (nothing moves earlier) |

**Decisions, pre-committed (the mechanism is not).**

1. **Default-ON flip** (opt-out `MASHED_NO_REFDIST_AFTER_AI`) only if: G-ORDER, G-CTRL-HEAD / -OFF, G-TOOK,
   G-ARM-DET pass, **and** (b) ON >= OFF in every regime (no regime loses a car), **and** collateral is clean (first
   divergence not before the arms' first refdist difference, no field of the player's ptrace changes earlier than that).
   The flip changes G-CTRL; the new trio is recorded. Otherwise the knob stays default OFF with a recommendation.
2. **U-9685 closes** iff `M_ON` car-2 in-window returns of 2 = 0. It is then attributed to the producer order
   (U-9683) and the grid-pose candidate is left pending, untouched. If `M_ON` keeps 2, U-9685 stays open, U-9683's
   candidate cause is **refuted** for these two calls (the lag is the original's order, now matched), and the grid
   pose (`MASHED_ROUND_GRID`, not flipped here) stays the remaining candidate.
3. **U-9683 resolves** (structural, tested) whatever (b) and the mode-7 counts do: the question the row asks is
   "whether the one-tick lead changes any mode-7 entry". The report states the counts of both arms.
4. If G-CTRL-OFF fails the added code changed behaviour: it is reverted and nothing is scored.
5. If any run hits a piz-collision AV (`MASHED.exe+0x1509b4` / `+0xe66db`, original side only) it is re-run, not
   reported. The exe-side runs here do not touch the original.

## 3a. AMENDMENT A1 (committed after the H_* and M_* arms were scored, before any M_PP arm exists)

What was seen first (committed data: `H_*`, `M_*`, `SCORE_u9683.txt`, `REF_COMPARE.txt`):

- G-CTRL-HEAD / -OFF pass (`991a5fff / be0d47ea / bc75b405`, HEAD and the new build with the knob OFF; the OFF ptrace had to be
  re-taken in a private game root, see `RESULT_u9683.md` section 1). `M_OFF` = `fd498815 / 342b989b / c899c830`, the `M_FIX` hashes.
- G-TOOK pass 13678 / 13678 on both arms. `H_ON` = `H_ON2` (G-ARM-DET pass). On the default stack `H_ON` step and ptrace equal
  `H_OFF` (only the gates dump differs, it logs the array).
- On the mode-7 stack the consumer's refdist is bit-identical between `M_OFF` and `M_ON` over the first 204 / 175 / 182 driving
  calls, so car 2's two returns of 2 at driving calls 16 and 17 stay (U-9685 does not close by the order).
- In `M_OFF.gates.csv`, `racepct_ec` (the array the producer reads, `0x008a96ec + v * 0x30c`) is 0 for all four cars until frame 20
  (car 1 then 0.054). The original's race_pct at the same driving calls is 99.2 .. 100 (`o_rd3.msd.rd.tsv`). With all four equal the
  producer's tie rule (`pa <= pb` -> reference = the outer index, `SpectatorDistances.cpp` ComputeDistancesCore) picks car 2 as the
  reference at calls 16-20, so `refdist[2]` = 0. The original picks car 1 there (183 / 220 reference indices equal overall).

Registered now, report-only, no default decision attached: two exploratory arms on the mode-7 stack with the faithful race_pct
producer on (`MASHED_PATHPROG`, the D-11072 P2 `FUN_00408610` loop, default OFF, known to drop (b) to 0/3 on its own, so it is a
causal probe and not a candidate default): `M_PP` = mode-7 stack + `MASHED_PATHPROG` + traces, `M_PP_ON` = the same + `MASHED_REFDIST_AFTER_AI`.
Reported: reference index equal to the original's on N / 220 driving producer calls (`M_OFF` baseline 183 / 220), refdist-0 counts
per car vs the original (164 / 4 / 29), car-2 in-window returns of 2 (`M_OFF` 2). **Prediction:** the reference index agrees on
>= 200 / 220 and car 2's two in-window returns vanish (55 %), i.e. U-9685's cause is the race_pct input of the producer. If the
returns stay, the cause is not race_pct and U-9685 keeps both its other candidates (grid pose, car-2 state at frames 16-17).

## 3b. AMENDMENT A2 (committed after the first full measurement round, before any `R_*` arm exists)

The orchestrator moved the base (`race/first-frame-parity` -> `232f9f76`: race camera drives the renderer, `MASHED_PLAYERTRACE_OUT`,
A3 `FUN_0046b540` LUT sqrt default ON). `work/u9683` is rebased onto `232f9f76` (clean, no conflicts; the knob sites are intact:
`UpdateRace`, the after-AI call, the trace hooks). The new default control trio is `c2e4d60d / b50bd39b / ba90ed88`
(`MASHED_NO_A3_LUT=1` reproduces the old `991a5fff / be0d47ea / bc75b405`). Everything measured so far stays on record as the
pre-rebase round (commits `96df7719`..`0ffc33fb` before the rebase) and is NOT reused as final evidence.

Re-run on the new base with the SAME gates, rules and decisions of section 3, arms prefixed `R_`:

| arm | knobs | purpose |
|---|---|---|
| `R_H_OPTOUT` | `MASHED_NO_REFDIST_AFTER_AI` | G-CTRL: must equal `c2e4d60d / b50bd39b / ba90ed88` (the orchestrator's number for `232f9f76`). It replaces the HEAD-exe arm: the opt-out path is the base's call site, and the base trio comes from another session's run |
| `R_H_DEF`, `R_H_DEF2` | none | new default, determinism floor |
| `R_M_OPTOUT`, `R_M_DEF`, `R_M_DEF2` | mode-7 stack (+ opt-out for `R_M_OPTOUT`) | G-TOOK, G-REFDIST, G-MODE7 / U-9685, collateral floor |
| `R_M_PP_OPTOUT`, `R_M_PP_DEF` | mode-7 stack + `MASHED_PATHPROG` (+ opt-out) | the report-only race_pct probe of A1 |
| `R_B_OFF_K`, `R_B_ON_K` (K = 0, 2, 1, 4, 27, 26, 54, 57) | opt-out / none | (b) per regime |

Validity rule: if `R_H_OPTOUT` does not reproduce the base trio, nothing from the round is scored and it is reported as a failed control.
Predictions carry over unchanged (the hashes will differ): `R_H_DEF` step and ptrace equal `R_H_OPTOUT` (gates differ); `R_M_*` in-window returns of 2
stay 49 / 2 / 0 (default, opt-out) and 49 / 0 / 0 (PATHPROG probe); (b) per regime A 1/3, B 2/3, C 0/3 on both orders (the orchestrator
reports the base's (b) unchanged, which is the OFF side of this comparison). If any prediction fails the round is reported as it came
out and the default decision (section 3 decision 1) is re-evaluated on it. The player trace is read from the driver's private game root (single
process per root), and its line count is checked against the opt-out arm's.

## 4. Not done here (registered)

- No hooks.csv change (`0x00442a60` is an EXE-ONLY TU, no `.asi` body, no run_diff / path2 leg, no C-level moves).
- `MASHED_ROUND_GRID` is not flipped. `PathProg_FrameTick` and the player-before-`UpdateRace` order are not moved
  (section 0 residues), so the port still differs from the original in those two places.
- No tracker is edited by hand. The proposed `re-classify` transaction is written in `RESULT_u9683.md`.
