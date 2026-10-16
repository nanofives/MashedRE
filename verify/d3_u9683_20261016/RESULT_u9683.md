# RESULT: U-9683 (refdist producer order) and U-9685 (car 2's mode-7 entries at driving calls 16-17)

Pre-registration `PREREG_u9683.md` (`898d298e`, before any measuring run) plus amendment A1 (`cff40ebf`, before any `M_PP` arm).
Branch `work/u9683`, base `1caf1a10`. Model claude-sonnet-5-5 (account2, no Ghidra MCP: bytes via `re/tools/decomp_pc.py`).

## 0. Verdict first

> **U-9683 is tested and resolved: the port's order now matches the original's, default ON, and the change is inert on every
> recipe scored.** The original runs the AI consumer before the refdist producer in one frame (`FUN_0040fc00`: `0x0040fc6c` AI
> path, `0x0040fc7c` camera path holding the producer at `0x004469b6`). `MASHED_REFDIST_AFTER_AI` moved the port's producer
> to the end of `UpdateCar`, after all physics and the progress tick. Effect: default-stack AI step fields **0 of 244**
> diverge and the player trace is byte-equal over 14400 frames, (b) is **identical on 16 of 16 runs (A 1/3, B 2/3, C 0/3 both
> arms)**, and the consumer's refdist is bit-identical over the original's whole window on the mode-7 stack.
>
> **U-9685 does NOT close by the order.** `M_ON` keeps car 2's two in-window returns of 2 (driving calls 16 and 17, clocks 850
> and 900, held 0, refdist 0). The cause is the producer's **race_pct input**: under the default bridge all four cars read
> race_pct 0 until frame 20 (the original reads 99.2 to 100 there), so the producer's tie rule picks car 2 (the outer index) as
> the reference and `refdist[2]` = 0. With the faithful race_pct (`MASHED_PATHPROG`, report-only probe, not a default
> candidate) car 2's in-window returns are **0** and cars 1 / 3 stay at the original's 49 / 0: **49 / 0 / 0 = the original**.
>
> Default build changes: G-CTRL gates hash only. New default trio **`991a5fff / a52c6b0e / bc75b405`**, opt-out
> `MASHED_NO_REFDIST_AFTER_AI` reproduces the previous `991a5fff / be0d47ea / bc75b405`.

## 1. Bytes (G-ORDER), `FUN_0040fc00` (per-frame race tick, sole caller `FUN_004111c0`)

| call site | callee | chain to the two functions under test |
|---|---|---|
| `0x0040fc6c` | `FUN_00425a40` (if `DAT_005f29c0 != 0`) | `0x00425a73` `FUN_00418860` -> `0x00418935` / `0x00418967` `FUN_00418560` -> `0x004187b7` `FUN_00416250` -> **`0x004163c3` `FUN_00414c30` (consumer)**; also `0x004187c6` `FUN_00416a30` -> `0x00416bad`, `0x00418780` `FUN_00417da0` -> `0x00417f13`. Then the physics: `FUN_00424eb0`, `FUN_00470c70`, `FUN_00422ba0`, `FUN_004252c0` |
| `0x0040fc75` | `FUN_0040da50` | progress / race_pct loop |
| `0x0040fc7c` | `FUN_0040d470` | `0x0040d4f3` `FUN_00448220` -> `0x004483c2` `FUN_00446520` -> **`0x004469b6` `FUN_00442a60` (producer)** |

`FUN_00442a60` has exactly one caller (`FUN_00446520`). `FUN_00418860` has a second caller, `FUN_004103a0` at `0x0041040e` (the pre-race phase). Raw: `decomp_order.txt`,
`decomp_order2.txt`, `xrefs_order.txt`, `xrefs_order2.txt`, `callees_18560.txt`. This is the cause of `verify/d3_u9219_20261013`'s
G-PROBE' (consumer at clock c reads the producer output of c - 50).

The port before: player physics (`TrackRenderer.cpp:3446`) -> `UpdateRace` (producer, `PathProg_FrameTick`, registry, race_pct bridge)
-> `Ai_AdvanceClock` -> `Ai_Standalone_Tick` -> opponent physics. So the producer ran before the AI tick, from the
player's post-step position and the opponents' previous positions. Two further order differences are **registered, not touched**:
`PathProg_FrameTick` also runs before the AI tick (original `0x0040fc75`, after), and the player steps before `UpdateRace` (original: inside
`FUN_00425a40`, after the AI).

## 2. Gates (PREREG section 3, in order)

| gate | result |
|---|---|
| G-ORDER | **PASS** (section 1) |
| G-CTRL-HEAD / -OFF | **PASS** both `991a5fff / be0d47ea / bc75b405` (`H_HEAD` = HEAD build, `H_OFF` = new build knob OFF). Harness finding: the first `H_OFF` run had no ptrace (written in the main root, not read), the next two came back contaminated (33,562 / 33,046 lines vs 27,356) because the exe chdirs to `MASHED_ROOT` and appends to a `player_trace.log` shared with other sessions. Step and gates matched all along. The accepted `H_OFF` and every later run used a private copy of `original/` (memory `mashed-root-main-shares-append-logs`) |
| G-TOOK | **PASS.** `M_OFF` 13678 P rows all site `U`, lag 0 on 13678 / 13678 T rows. `M_ON` 13678 P rows all site `A`, lag 1 on 13677 / 13677 (the first T row has no producer yet). Array at every T row == the latest P row (100 %). `M_PP` / `M_PP_ON` the same on 14038 rows. Game-side witness `[U-9683] ...` in `mashed_re.log` |
| G-ARM-DET | **PASS.** `H_ON` = `H_ON2` = `991a5fff / a52c6b0e / bc75b405`, `M_ON` = `M_ON2` = `1d128269 / 6054879b / c2ef93b0`. `M_OFF` = the `M_FIX` hashes of `verify/d3_car2pickup_20261013` (`fd498815 / 342b989b / c899c830`) from another session and build |
| G-REFDIST | **FAIL, both arms, as predicted.** Refdist-0 consumer calls in the original's window (204 / 175 / 182 calls): exe 140 / 6 / 23 vs original 164 / 4 / 29 (cars 1, 3 outside 3, car 2 inside), `M_ON` identical to `M_OFF` (`SCORE_u9683.txt`) |
| G-REFDIST-VAL (report) | 238 / 561 within `0.01 + 0.08 * orig` on both arms, **ON - OFF = +0.00 points** (predicted below 3) |
| G-MODE7 / U-9685 | **U-9685 does not close** (predicted 60 %). `M_OFF` and `M_ON` in-window returns of 2: 49 / **2** / 0 (original 49 / 0 / 0). The two car-2 rows: driving call 16 clock 850 and call 17 clock 900, held 0, refdist 0, original refdist at the same calls `3feb3cf3` (1.838) and `3fe97d8a` (1.824) |
| G-MODE7-X (report) | car 2 first held != 0 at driving call 38 in both arms (original 27); the pickup-miss residue of U-9682 is unchanged |
| (b) per regime | **A 1/3, B 2/3, C 0/3 on all 16 runs** (K = 0, 2 / 1, 4, 27 / 26, 54, 57, OFF and ON) and on the post-flip default `B_DEF_K00 / 01 / 26`. Every `B_ON_K` step hash equals its `B_OFF_K` (K00 `3cb5c20e`, K02 `3f884d39`, K01 = K27 `b47ae9cd`, K04 `4b4f0535`, K26 `4e0f01eb`, K54 = K57 `6934df54`), so the scores are equal by construction. `B_OFF_K00/01/26` equal the `B_K00/01/26` hashes recorded in RESULT_CAR2PICKUP. `B_DEF_K00/01/26` (post-flip default) are hash-equal to them. `SCORE_B_u9683.txt` |
| collateral | **clean.** Default stack: `COLLATERAL_H_stdout.txt` **0 of 244** step fields diverge (floors `H_HEAD`, `H_ON2`), ptrace equal. The gates dump (it logs the array at tick time) differs: `refdist` 4946 / 54712 rows (median 0.004, p95 0.033, max 14.8 on one pair flip at frame 3621), `other_idx` 16 rows, first at frame 23. Mode-7 stack: `COLLATERAL_M_stdout.txt`, first divergence frame **530**, floors `M_FIX` / `M_ON2` 0, 169 of 244 fields after it, none before. First consumer-visible refdist difference: clock 26550 (frame 531), consumer call 348 / 530 / 347 of cars 1 / 2 / 3, all beyond the original's window. First step divergence frame 530 (cars 1, 2) and 580 (car 3), first ptrace difference frame 582 |

Why the effect is nil on these recipes: the recipe's player is parked, so the only input the two orders disagree on (the player's
position after this frame's step) does not change. The first differences appear when contact moves the player (frame 530 on the mode-7 stack).

## 3. U-9685, the mechanism and the report-only probe (amendment A1)

Data, no inference. In `M_OFF.gates.csv` the array the producer reads (`racepct_ec`, `0x008a96ec + v * 0x30c`) is 0 for all four cars
until frame 20. In `o_rd3.msd.rd.tsv` the same race_pct is 98.0 to 100 at driving producer calls 12 to 18. With four equal values the port's
transcription of the reference choice (`SpectatorDistances.cpp` ComputeDistancesCore, annotated `0x00442b3a..0x00442b72`: `pa <= pb` -> reference = `b`) takes the outer index, so at driving calls 16 to 20 the exe's
reference is car 2 and the original's is car 1 (`REF_COMPARE.txt`; reference index equal on 183 / 220 calls overall).

| arm | reference index == original | refdist 0 (cars 1 / 2 / 3) | in-window returns of 2 (cars 1 / 2 / 3) |
|---|---|---|---|
| original `o_rd3` | 220 / 220 | 164 / 4 / 29 | 49 / 0 / 0 |
| `M_OFF` (mode-7 stack) | 183 / 220 | 140 / 6 / 23 | 49 / 2 / 0 |
| `M_ON` | 183 / 220 | 140 / 6 / 23 | 49 / 2 / 0 |
| `M_PP` (+ `MASHED_PATHPROG`) | 191 / 220 | 148 / 1 / 21 | 49 / **0** / 0 |
| `M_PP_ON` | 189 / 220 | 148 / 1 / 21 | 49 / **0** / 0 |

Prediction check (PREREG A1): car 2's returns vanish, as predicted (55 %). Reference agreement of at least 200 / 220 was **not** met (191). The two
`M_PP` arms share step `bafe7207` and ptrace `6ef4dbdc`, so the order is inert there too. `MASHED_PATHPROG` is a probe: it is default OFF,
alone it drops (b) to 0 / 3 (NEXT_SESSION 2026-10-13), and car 2's first held word moves to call 47 (original 27). The grid pose
(`MASHED_ROUND_GRID`, not flipped) is not needed for the two returns to vanish.

## 4. Code (commits `96df7719`, `f58ddc1f`)

- `Race/SpectatorDistances.cpp`: `RaceComputeDistancesTick(int site)`, `RaceRefDistAfterAi()`, `MASHED_REFDIST_TRACE` rows (P / T, frame
  ordinal bumped once per race frame before player physics). EXE-ONLY TU, no `.asi` body.
- `D3d9Render/TrackRenderer.cpp`: `UpdateRace` calls the producer only when the knob is off, `UpdateCar` calls it after the opponent
  block with the same `gates_.empty()` coverage, plus the two trace hooks.
- Default ON, opt-out `MASHED_NO_REFDIST_AFTER_AI`, `MASHED_REFDIST_AFTER_AI` is now a no-op. Post-flip arms: `H_DEF` =
  `991a5fff / a52c6b0e / bc75b405` (= `H_ON`), `H_OPTOUT` = `991a5fff / be0d47ea / bc75b405` (= `H_OFF`), `M_DEF` = `M_ON`, `M_OPTOUT` = `M_OFF`.
- No hooks.csv change (no `.asi` body, no run_diff / path2 leg, no C-level moves). Decision 1 of the PREREG fired as written.
  The knob arms before the flip ran on `96df7719` (opt-in), the `*_DEF` / `*_OPTOUT` arms on `f58ddc1f`.

## 5. Not done, stated

- The two other order differences of section 1 are untested.
- No original run was taken (the committed `o_rd3` was the reference). `MASHED_ROUND_GRID` was not touched.
- No tracker was edited. `scripts/*.ps1` cannot be written on this account (managed policy), so the driver is `run_u9683.py`.

## 6. Proposed re-classify transaction (not applied)

- **U-9683** -> resolved 2026-10-16 (structural, tested): order matched from `0x0040fc6c` / `0x0040fc7c`, default ON, inert on the default stack,
  (b) 16 / 16 identical, mode-7 in-window identical. New default G-CTRL `991a5fff / a52c6b0e / bc75b405`.
- **U-9685** -> keep open, re-scope: cause = the producer's race_pct input before the start line (bridge 0 for all cars until frame 20, original 98 to
  100), not the order and not the grid pose. Probe: `MASHED_PATHPROG` gives in-window 49 / 0 / 0. Resolution path: a faithful pre-line race_pct
  (PATHPROG default-ON, user decision, or a bridge fix), pre-registered, then re-score G-MODE7 and (b).
- **New row (structural):** `PathProg_FrameTick` and the player step run before the AI tick in the port, after it in the original
  (`0x0040fc75`, `FUN_00425a40`). Not tested.
- CHANGELOG line: `2026-10-16 U-9683 resolved: refdist producer now after the AI block (FUN_0040fc00 0x0040fc6c before 0x0040fc7c), default ON,
  opt-out MASHED_NO_REFDIST_AFTER_AI, inert on (b) 16/16 and on the mode-7 window. U-9685 not closed by it, cause = race_pct input. verify/d3_u9683_20261016/RESULT_u9683.md`

## 7. Decisions needed (user)

1. Keep the default-ON order on the merge, or revert to opt-in? It changes only the gates-dump hash (`be0d47ea` -> `a52c6b0e`) of the default control trio.
2. U-9685: default-ON `MASHED_PATHPROG` (known (b) cost) or a bridge fix for the pre-line race_pct? Neither was started.

## 8. Raw

`run_u9683.py` (driver, private `MASHED_ROOT` copy, PIDs tracked, every run exit 0 and self-exited), `score_u9683.py`, `ref_compare.py`,
`SCORE_u9683.txt`, `SCORE_PP_u9683.txt`, `SCORE_B_u9683.txt`, `REF_COMPARE.txt`, `REF_COMPARE_PP.txt`, `COLLATERAL_*`, the arms' `*.step.csv`
(all), `*.rdtrace.tsv`, `*.cb.tsv`, `*.c30.tsv`, gates dumps of `H_OFF` / `H_ON`. ptrace logs and `*.log` are gitignored, hashes are above.
