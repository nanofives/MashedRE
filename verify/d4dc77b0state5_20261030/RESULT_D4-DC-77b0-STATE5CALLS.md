# RESULT D4-DC-77b0-STATE5CALLS (run 25, 2026-10-30)

**Verdict: GREEN (static, step 1 closes it).** The default exe does **not** run the second leg. In sub-state 5 the
original reaches FUN_004177b0 twice per dispatcher call (FUN_004103a0 -> FUN_00418860, then FUN_0040fc00 ->
FUN_00425a40 -> FUN_00418860), and the exe reaches `AiPreTickRubberBand` once per `UpdateCar` sim step. The missing leg
is the whole FUN_00425a40, so the exe also skips its second FUN_00418860 (alive-poke + per-car FUN_00418560 loop) and
FUN_00424eb0 / FUN_00470c70 / FUN_00422ba0 / FUN_004252c0 in state 5. Step 2 (counter knob + race) was not needed and
not run. No code change, no flip, no promotion, no tracker change.

Base: `orch/integration` @ `9d3e2421c`. Exe default: `MASHED_SUBSTATE=1` is a faith default
(`Core/FaithDefaults.cpp:66`), so the sub-state path below is the default build's path.

## Method

- `re/tools/decomp_pc.py 0x0040fc00 0x00425a40 0x00418860 0x004103a0 0x004111c0 --callees` against pool clone
  `Mashed_pool0` (read-only). Note for the next worker: the tool resolves `mashed_pool\` relative to the cwd, so it must
  run from the main checkout. From a worktree it fails with "Could not find project".
- Port side read at `9d3e2421c`: `Race/SubStateMachine.cpp:168-300`, `D3d9Render/TrackRenderer.cpp:5176` (UpdateCar),
  `:5270-5405`, `:5600-5640`, `:5669`, `:6034`, `Ai/AiStandalone.cpp:2073-2140`, `exe_main.cpp:3858-3880`,
  `Race/SubStateCallees.cpp:69`, `Core/FaithDefaults.cpp:63-66`. Plus a grep of every `Ai_Standalone_Tick()` and
  `AiPreTickRubberBand()` call in `mashedmod/src`.

Raw dump (not committed), `D:/MashedWT/D4-DC-77b0-STATE5CALLS_data/`:

| file | SHA-256 |
|---|---|
| decomp.txt | 673602ed6a0d939c6e48c72f2cd9ba7e1abe1a2ae5bba0da380c3d86d8222fcf |

## Original call tree, sub-state 5 (decomp)

- FUN_004111c0 `case 5:` = `FUN_004103a0(); FUN_0040fc00(); return;` (decomp of 0x004111c0, case-5 arm).
- FUN_004103a0: the FUN_0040e560 gate, the 0x005f29b8 / 0x007f1014 stores, FUN_00426c90, then **FUN_00418860**
  (call at 0x0041040e), the hold loop (FUN_0046baa0 + FUN_0046d7f0 per car), FUN_0041da90, and on `t >= _DAT_005ccdf4`
  the release loop and `DAT_0063ba8c = 6` (0x004104e3).
- FUN_0040fc00: `DAT_005f29c0 = 1` when `FUN_00496930(6) != 0`, `= 0` when `FUN_00496930(5) != 0`, then
  `DAT_007f1008 = units`, `_DAT_007f100c = step`, then **`if (DAT_005f29c0 != 0) FUN_00425a40();`** (call at 0x0040fc6c,
  the first call after the stores), then FUN_0040da50, FUN_0040d470(0), FUN_00426ab0, ... FUN_0041d930, the round-type
  switch, and the 0x007f0fec / 0x007f0ff4 / 0x007f0ff8 adds. `0x005f29c0` is 1 in the image (WRITERS RESULT).
- FUN_00425a40: `FUN_00423b00();` then returns 0 **before** FUN_00418860 only when `DAT_007f1a50 == 1` and
  (`DAT_007f1a54 != 4` or (`DAT_007f1a74 == 0` and `DAT_007f1a70 == 0`)). With `DAT_007f1a50 == 1` and the second test
  false it increments `DAT_007f1a78` and continues. Otherwise: **FUN_00418860**, FUN_00424eb0(param_1, &DAT_00803324),
  FUN_00470c70(param_1, &DAT_00803324), FUN_00422ba0(param_1), FUN_004252c0(param_1), return 1.
- FUN_00418860: `if (DAT_00801ca0 > 3)`: the round test and FUN_0040e480(v, 2) alive-poke for v = 1..3, then
  **FUN_004177b0**, then the 4-car FUN_00418560 loop.

## Table: original vs exe, one dispatcher call / one sim step in sub-state 5

| step | original | exe (default, MASHED_SUBSTATE=1) | exe cite |
|---|---|---|---|
| 1 | FUN_004103a0 | `SubStateHeldCountdown5_C(units)` | TrackRenderer.cpp:5369-5370 |
| 1a | -> FUN_00418860 at 0x0041040e (FUN_004177b0 call #1) | `s_ssHost.ai_tick()` -> `AiBridgeSnapshot(); Ai_Standalone_Tick();` -> `AiPreTickRubberBand()` (call #1) | SubStateMachine.cpp:198, TrackRenderer.cpp:5282-5288, AiStandalone.cpp:2129 |
| 2 | FUN_0040fc00 | partials only: P3 reset pair, `SubStateCameraChain(false)`, `Ai_AdvanceClock`, race-timer / countdown-clock adds, `CarParts_FrameTick`, `CopterTick`, `PreGoRegistryTick`, `HudSlideBillboardTimerTail` | TrackRenderer.cpp:5376-5399 |
| 2a | -> FUN_00425a40 at 0x0040fc6c when `DAT_005f29c0 != 0` | **not called**: no FUN_00425a40 port or call in the block | TrackRenderer.cpp:5270-5405 |
| 2b | -> FUN_00418860 (FUN_004177b0 call #2), unless the 0x007f1a50 early return | **absent** | |
| 2c | -> FUN_00424eb0, FUN_00470c70, FUN_00422ba0, FUN_004252c0 | **absent**: the block zeroes the car speeds (:5271-5272) and the comment at :5267-5268 says "the frame returns before any physics" | TrackRenderer.cpp:5267-5268, 5271-5272 |
| 3 | return to the frame loop | `return;` (unconditional at the end of the `SubState_Enabled() && countdown_ > 0.f` block) | TrackRenderer.cpp:5404 |

Why there is no other exe call in the same step:

- `Ai_Standalone_Tick()` has exactly two call sites in `mashedmod/src`: the state-5 lambda (TrackRenderer.cpp:5287,
  reached only through SubStateMachine.cpp:198) and `ai_tick_phase` (:5635). `AiPreTickRubberBand()` has one call site,
  AiStandalone.cpp:2129, inside `Ai_Standalone_Tick`.
- `ai_tick_phase` is invoked at :5669 and :6034, both in `TrackRenderer::UpdateCar` (starts :5176) **below** the :5404
  return, so they do not run on a step that enters the sub-state block.
- `UpdateCar` runs once per sim step (exe_main.cpp:3862 single step, :3876 fixed-step loop, up to 6 per render frame).
  So the count is per sim step, the analogue of one FUN_004111c0 call.
- `Ai_CountdownAlivePoke` (AiStandalone.cpp:2089) is the alive-poke only and does not call the rubber band.

Exe count per sub-state-5 step: 1 when `g_aib.loaded && I32(kSplineRaceCnt) > 3` (lambda guard, :5284), 0 otherwise.
Original count per case-5 call: 2 when `DAT_00801ca0 > 3`, `DAT_005f29c0 != 0` and FUN_00425a40 does not take its
early return, else 1 (or 0 when `DAT_00801ca0 <= 3`).

On the exit call (the one that stores 6 at 0x004104e3) the original's second FUN_004177b0 reads `DAT_0063ba8c == 6`
and its first reads 5 (WRITERS RESULT, line 68). The exe has no second read on that step.

## Open items

- [UNCERTAIN] The runtime values of `DAT_007f1a50` / `DAT_007f1a54` / `DAT_007f1a70` / `DAT_007f1a74` on the original
  in sub-state 5, so whether the original's second leg runs in the canonical scenario. `Ai/AiState.h:84` names
  0x007f1a50 `kDbgSplineEn` (FUN_00418560 debug override enable) and TrackRenderer.cpp:4559 states it is 0 in the
  standalone. Neither is an original-side measurement. Missing: one original-side read of the four dwords and of
  `DAT_005f29c0` during sub-state 5 (needs an original boot, out of scope for this standalone-only step).
- The exe's missing FUN_00425a40 leg in state 5 is a faithfulness gap of the whole leg (second AI tick plus the
  physics frame of every car, FUN_00470c70), not of the rubber band alone. Porting it changes behaviour, so it is a
  default-OFF knob + PREREG step, then a decision row. Not done here.

## Gates

Not run: this step changes no build input (one new file under `verify/`). The default exe is byte-identical to
`orch/integration` @ `9d3e2421c`, so G-CTRL / B stay at the `merge_train.json` refs (G-CTRL `8284be46/ba407a16/31fe043e`,
B `1d5bf4f9` x3).
