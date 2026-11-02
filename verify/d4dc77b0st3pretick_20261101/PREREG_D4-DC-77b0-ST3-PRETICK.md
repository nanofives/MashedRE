# PREREG D4-DC-77b0-ST3-PRETICK: the AI tick on case-3 ticks, knob `MASHED_ST3_PRETICK` (2026-11-01)

Branch `orch/D4-DC-77b0-ST3-PRETICK` from `orch/integration` `96640c25a`. Committed alone, before every knob arm.
Raw data `D:\MashedWT\D4-DC-77b0-ST3-PRETICK_data`. Private root `D:\MashedWT\D4-DC-77b0-ST3-PRETICK_root`.
Input: `verify/d4dc77b0runtime_20261101/RESULT_D4-DC-77b0-RUNTIME-PARKED.md` (branch `orch/D4-DC-77b0-RUNTIME-PARKED`),
leg 2, "countdown 3 / 4" row.

## 1. Static reading (`original\MASHED.exe.unpatched`, capstone, and headless decomp of a pool clone)

FUN_004111c0 case 3 and case 4:

| address | instruction |
|---|---|
| 0x0041127e | `mov [0x007f0fc8], 0` |
| 0x00411288 | `call 0x004102f0` (FUN_004102f0, the phase-3 handler; stores 4 at 0x00410387 on its release tick) |
| 0x0041128d | `call 0x0040fc00` |
| 0x0041129b / 0x004112a5 / 0x004112ad | case 4: `mov [0x007f0fc8], 0`, `call 0x0040dbd0` (stores 5), `call 0x0040fc00` |

FUN_0040fc00:

| address | instruction |
|---|---|
| 0x0040fc57 | `mov [0x007f100c], eax` |
| 0x0040fc5c / 0x0040fc61 | `mov eax, [0x005f29c0]` / `test eax, eax` |
| 0x0040fc63 | `mov [0x007f1008], esi` |
| 0x0040fc69 | `je 0x0040fc74` |
| 0x0040fc6c | `call 0x00425a40` |
| 0x0040fc75 | `call 0x0040da50`, then `FUN_0040d470(0)` from 0x0040fc7a |

FUN_00425a40: `call 0x00423b00` (0x00425a40), the DAT_007f1a50 == 1 test (0x00425a45..0x00425a6c), `call 0x00418860`
(0x00425a73), then FUN_00424eb0, FUN_00470c70, FUN_00422ba0, FUN_004252c0.

FUN_00418860 (decomp): `if (3 < DAT_00801ca0)`: the round test (FUN_0042f6a0() in {3, 4, 5, 10} or DAT_007f0fd0 in {4, 9, 8,
10}) then FUN_0040e480(v, 2) for v = 1..3 with FUN_0046c7b0(v) == 1; FUN_004177b0(); for v = 0..3: FUN_0040e470(v) not in
{0, 1} or FUN_00443080() == 1, then FUN_00418560(v, type) when FUN_0046c7b0(v) == 1, and FUN_00418560(0, type) when
FUN_0042f6a0() in {3, 4, 5} and DAT_007f0fd0 == 7.

Measured (RUNTIME-PARKED, every arm but rule 6): FUN_004177b0 entered from 0x00418905 on every case-3 tick, reading
0x0063ba8c = 3 (636..777 per match start), and once reading 4 (the release tick, where FUN_004102f0 stored 4 before
FUN_0040fc00 ran). The case-4 tick's call reads 5 (FUN_0040dbd0 stored it first).

## 2. The exe today (`orch/integration` `96640c25a`)

- `TrackRenderer::UpdateCar`'s sub-state block runs the case-3 handler (`ba8c == 3`, FUN_004102f0 emulated), the case-4
  handler (`SubStateEnter4`) and the case-5 handler (`SubStateHeldCountdown5_C`, whose host `ai_tick` is
  `AiBridgeSnapshot` + `Ai_Standalone_Tick`). After them it makes FUN_0040fc00's partial stores (0x007f1008, 0x007f100c,
  `SubStateCameraChain(false)`, `Ai_AdvanceClock`, the clock adds) and returns. It makes no AI tick on case-3 ticks.
  Measured: RUNTIME-PARKED x_r0_t0 / x_r0_t0c, first `Ai_Standalone_Tick` entry with 0x0063ba8c already 5.
- **Overlap with `MASHED_COUNTDOWN_SPAWN` / `Ai_CountdownAlivePoke`: none in today's tree.** `Ai_CountdownAlivePoke`
  (`AiStandalone.cpp`) has no caller since `20efede42` (D5-DEAD-COUNTDOWN-STANDIN deleted the countdown stand-in that called
  it, unreachable since the sub-state block returns on every countdown path). `MASHED_COUNTDOWN_SPAWN` has no reader left in
  `TrackRenderer.cpp`. The knob below calls `Ai_Standalone_Tick`, whose alive-poke block is the same FUN_00418860 code
  (0x004188ac..0x004188db). It does not call `Ai_CountdownAlivePoke` and does not seed the spawn pose.

## 3. Knob `MASHED_ST3_PRETICK` (default OFF)

At the end of the exe's case-3 handler (after its release test, so on the release tick 0x0063ba8c already reads 4), when
`g_aib.loaded` and `DAT_00801ca0 > 3`: `Ai_AdvanceClock(units)` (the 0x0040fc57 / 0x0040fc63 stores, which the block below
writes again with the same values), then `AiBridgeSnapshot()` and `Ai_Standalone_Tick()` (FUN_00418860), the same pair the
case-5 host `ai_tick` runs. On the release tick it logs `[st3pretick] ticks state3=<a> state4=<b>` to `mashed_re.log`.

Not ported (named): the rest of FUN_00425a40 on case-3 ticks (FUN_00423b00, FUN_00424eb0, FUN_00470c70, FUN_00422ba0,
FUN_004252c0), and the case-3 `[0x007f0fc8] = 0` store (0x0041127e). The case-4 tick's call (reads 5) is the state-5 class
(D4-DC-77b0-STATE5-LEG2 / ST5-HOLD) and is left to those steps. `Ai_Standalone_Tick` has no form of FUN_00418860's
`FUN_00418560(0, type)` arm (round type 3/4/5 with DAT_007f0fd0 == 7): not reached on the warp-launched match, where
0x0067e9fc reads 10 (RUNTIME-PARKED leg 3).

Lines: this edit is inside the `ba8c == 3` arm only. ST5-HOLD (`MASHED_ST5_LEG2`) edits the `ba8c == 5` arm and the
FUN_0040fc00 partials below it, MATCHEND-SUBSTATE edits the match-end code. No shared line.

## 4. Arms

Runner `verify/d4dc77b0st3pretick_20261101/run_arms.py` (copy of MATCHEND-SUBSTATE's, stack `ST3` =
`MASHED_ST3_PRETICK=1`). Probe `exe_vsprobe.py` (copy of MATCHEND-SUBSTATE's, extra `--env`), summaries by `summarize.py`.
Every exe boot under `mashed_lock.py run --label D4-DC-77b0-ST3-PRETICK`, `MASHED_ROOT` = the private root.

| arm | what |
|---|---|
| H_DEF, B_DEF_K0, B_DEF_K1, B_DEF_K26 | default gates, knob unset |
| H_ST3, B_ST3_K0, B_ST3_K1, B_ST3_K26 | the same with the knob set (reported, not a gate) |
| x_off | exe probe, x_r0_t0 recipe (H_DEF base env, 14400 frames, `--limit 1200`), knob unset |
| x_st3 | x_off + `MASHED_ST3_PRETICK=1` |

## 5. Predictions

- P0: knob unset, G-CTRL = `8284be46 / ba407a16 / 31fe043e`, B = `1d5bf4f9` x3 (`merge_train.json`).
- P1: x_off has no T or B entry before the first one reading 0x0063ba8c = 5 (RUNTIME-PARKED x_r0_t0 shape).
- P2: x_st3. With N = the `[leg2] phase 3 released after N frames` value of its `mashed_re.log`: B entries reading 3 = N - 1,
  B entries reading 4 = 1, and the `[st3pretick]` line reads `state3=N-1 state4=1`. T entries the same counts. N itself is
  reported next to the original's 678 (o_r0_t0: 677 + 1), not predicted (phase 3's length follows the frame clock).
- P3: every x_st3 B entry reading 3 or 4 has the cells (v0..v3) = (1, 2, 2, 2) and 0x00897ffc = 0, as the original
  (RUNTIME-PARKED leg 1: (1, 2, 2, 2) on all calls, leg 3: 0 in 3..8). The first T entry reads (1, 0, 0, 0) and its alive-poke
  writes the 2s before the first B entry.
- P4: from the first B entry reading 5 on, x_st3's B counts per 0x0063ba8c value equal x_off's (the knob adds no call
  after the release tick).
- P5: no crash or hang in any knob arm. A crash is RED for the knob, not a re-run (piz-collision signatures excepted).
- Reported, no prediction: H_ST3 / B_ST3_* hashes. The knob runs FUN_004177b0's phases 1-4 (0x0089a880, the finish-order
  slots) and the per-car FUN_00418560 on 600+ more ticks before GO, so state at GO can differ.

Verdict rule: P0, P1, P2, P3, P4 and P5 hold -> GREEN, flip filed as a decision row. P0 fails -> RED (the knob is not inert
when unset). P2 / P3 / P4 fail -> RED with the measured difference.
