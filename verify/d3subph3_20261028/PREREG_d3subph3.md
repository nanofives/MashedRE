# PREREG: D3-SUBSTATE-PH3 (D-11073, phase 3 of the race sub-state machine)

Date 2026-10-28. Branch `orch/D3-SUBSTATE-PH3` from `orch/integration` 0d1285dc7. Committed alone, before
any port arm of this step and before the original capture below has produced data (it was queued on the
machine lock when this file was written).

## 0. Starting state (read from the tree, not assumed)

The ledger title says phase 3 is unported. That is stale. D-11073 leg 2 (7ec59a21e, 2026-10-09,
`verify/d3_gatefire_20261008/RESULT_LEG2.md`) ported phase 3 behind default-OFF `MASHED_SUBSTATE`:
the state-2 tail seed (`DAT_005f29b8` = 100000), the `FUN_004102f0` emulation (hold flag, per-car
`FUN_0046baa0`, countdown store, exit test), the camera chain (`FUN_00442600`, `FUN_004464c0` dispatch,
`FUN_00445aa0` type-0 step, `RaceCamera::Update`, the `0x004481c9` copy) and the site (a) release. Four
bodies are C3. U-9201 then showed the phase-3 length is set by `QueryPerformanceCounter`
(`DAT_007f1030`), so it differs per original run (652 / 690 / 740 frames) and the port reproduces a run's
length when that run's ticks are replayed (`RESULT_U9201.md`).

What is left of phase 3, by the leg-2 and U-9201 RESULTs:
- **U-9203**: the camera target is 0.089 / 0.098 off at phase-3 frame 0 under replay and decays to the
  steady offset by about frame 60. Named candidates, not separated: (1) the original's first-frame call
  order / count; (2) the port's first two chain calls; (3) the reset path. The row asks for "an
  original capture of the camera struct at both first-frame calls".
- **U-9202** (grid placement, +0.040 y) is D2-domain and report-only here.
- Named exe skips (types 1/2, cam +0x10..+0x18 orientation, the 5 -> 6 exit-frame camera call) are
  visual or post-phase-3 and are out of this step unless the capture shows they feed entry 0.

## 1. Measurement (original, read-only)

`re/frida/scenario_launch.py --p3cam-probe N` (new in this step): entry + leave hook on `FUN_00446520`,
first N calls after the statediff arm, per call: caller return address, both args, `DAT_0063ba8c`,
`DAT_005f29b8`, `DAT_007f1030`, `DAT_007f1000`, `DAT_007f1008`, `_DAT_007f100c`, cam `+0x00..+0x5c` in and
out, `+0x964..+0x9a0` out, entry 0 (`0x008964c0`) `+0x3c..+0x44` out, the four car positions
(record `+0x958/+0x95c/+0x960`) in. Hex dwords. `FUN_00446520` runs about once per frame, under the
Interceptor rate limit in CLAUDE.md.

Scenario: as U-9201 (`--track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --statediff-out`, the
statediff path keeps phase 3 intact), muted, private root, under `mashed_lock`.

## 2. Questions and hypotheses

Q1 (census). Which calls does the original make from the last state-2 frame through the first ~60
phase-3 frames: count per frame, caller, reset arg, sub-state at entry? Recorded as measured; no
numeric prediction.

H-A (call order). The original makes at least one `FUN_00446520` call with `DAT_0063ba8c` = 3 and
reset = 0 BEFORE its first reset = 1 call (the state-2 tail sets 3, then `FUN_0040fc00` ->
`FUN_0040d470(0)` runs the same frame, before `FUN_004102f0`'s entry block on the next frame). The port
makes none (its tail frame returns before the chain). Refuted if the first call with sub-state 3 is the
reset = 1 call.

H-B (inputs). At the reset = 1 call, the camera inputs the port can see differ from the original's in
at least one of: car positions (beyond the U-9202 offset), `DAT_007f1030`, cam `+0x1c`. Measured, then
compared with the port's `MASHED_LEG2_TRACE` row for the same call.

H-C (reset path). Given the original's own inputs at the reset = 1 call, `RaceCamera::Update` with
force_reset reproduces the original's leave state (+0x40..+0x54 and the spring block) to 1e-4 after the
U-9202 car offset is removed. Refuted if the residual is larger.

## 3. What a fix may do

Only if H-A or H-B names a call-sequence or input difference: a new default-OFF knob, nested under
`MASHED_SUBSTATE` (itself default OFF), that reproduces the original's first-frame call sequence. Its
predictions (frame-0 target error, decay) are added to this file as an amendment, committed alone,
BEFORE the knob arm runs. If the capture shows the transient comes from the cars (U-9202), no code
changes and the RESULT says so.

## 4. Gates

- **G-CTRL / B unchanged** on this branch's exe with every knob at default: `gctrl.ref` and `b.ref`
  from `re/orchestrator/merge_train.json` via `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0
  B_DEF_K1 B_DEF_K26`.
- **G-KNOB-OFF**: `MASHED_SUBSTATE` alone (stack P3) gives the same trace bytes with and without the new
  knob's code compiled in (the new knob is read only under `MASHED_SUBSTATE`).
- **G-U9203** (only if a knob is built): with the original run's ticks replayed, the frame-0 camera
  target error minus the steady offset (port - orig at frame 60) drops below the value the amendment
  predicts, and phase 3's length stays within +-3 frames of the replayed run.

## 5. Report-only

U-9202's car offset on the new capture; phase-3 length of the new original run.
