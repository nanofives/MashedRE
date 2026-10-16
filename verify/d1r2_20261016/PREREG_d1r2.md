# PREREG d1r2: the verbatim race camera drives the renderer (D1-residue R2)

Committed BEFORE the first measuring run. Branch `work/d1r2`, base `race/first-frame-parity` 1caf1a10.

## 1. What the bytes say (static, before any run)

`re/analysis/race_camera/FUN_00446520.asm`, the tail of the director's main block:

| site | what it does |
|---|---|
| `0x00447f61..0x00447f82` | eye `+0x40..+0x48` = cam_pos |
| `0x00447fc2..0x00448007` | `+0x4c..+0x54` = tgt - pos (aim DIRECTION, float subtracts) |
| `0x0044800d` | `+0x58` = `0x3f19999a` (0.6, the view window SetupFOV reads) |
| `0x00448014..0x004480b1` | yaw `+0x38`: dz != 0 -> `180 - (-(atan(dx/dz) * [0x5ccae0]))`, +180 when dz > 0; dz == 0 -> 270 (dx < 0) / 90 |
| `0x004480b4..0x0044819d` | pitch `+0x34` = `360 - p`, p from `atan(dy / FastSqrt(dx*dx + dz*dz))` by the same law (`call 0x4c3b30` = the LUT FastSqrt) |
| `0x004481a3` | roll `+0x3c` = 0 |
| `0x004481aa..0x004481b6` | `DAT_007f0f38 != 0` (overhead) -> yaw = 0 |
| `0x004481c1` | `Camera::Apply` 0x00441760: identity (0x004c15c0), rotate Y by `+0x38`, own right by `+0x34`, own at by `+0x3c`, translate `+0x40` (0x004c1340), SetupFOV 0x00441700 |
| `0x004481e5..0x004481f9` | `+0x10..+0x18` = tgt |
| `0x004481ff..0x00448208` | only if `cam[0] != 0` (the phase-3 hold flag): `0x00442a20` overwrites the frame with entry 0's matrix `0x0089650c` and `+0x58` with `0x00896580` |

And `dec_00446520.c:152-153` (survey 2026-10-16): in sub-state 6 the entry dispatcher `0x004464c0` is
short-circuited. So, statically: **racing frames render Camera::Apply(yaw, pitch, roll = 0, eye)**, and only
phase-3 frames render entry 0's rolled matrix. The roll memory note was measured in phase 3.

What the standalone does today (`TrackRenderer.cpp:6586-6609`, `:6713`): `MatLookAtLH(race_cam_.pos(),
race_cam_.target())`, no basis published to librw, the invented chase rig on every frame where the director
has not run (the whole default-build countdown), and `MASHED_CHASE_RIG=1` reverts to the rig.

## 2. The change under test

1. `RaceCamera::Update` transcribes `0x00447fc2..0x004481b6`: aim, yaw, pitch (pitch through
   `Math::LutSqrt`, the 0x004c3b30 LUT), roll = 0, overhead yaw = 0.
2. New `Race/CameraApply.cpp` (exe): Camera::Apply's frame build from those angles with the ported
   `RwMatrixRotate` (0x004c4d20) and `RwMatrixTranslate` (0x004c51a0). The view window published to the
   renderer is SetupFOV's: `vw.y = 0.75 * +0x70 * +0x58` with `+0x70` = 1.0 measured (`verify/d1_lens`).
3. When the hold flag `0x00897fe0` is set (only under `MASHED_SUBSTATE`), the renderer takes entry 0's
   matrix at `0x0089650c` and its `+0xc0` view window instead, which is `0x00442a20`.
4. TrackRenderer publishes the basis (`last_basis_valid_ = true`), so librw and the legacy path build the
   view from the same 12 floats. The chase rig and `MASHED_CHASE_RIG` go. The default-build countdown now
   advances `race_cam_` (first call with reset, as `FUN_0040d470(1)` does on phase-3 entry) so the
   director exists on every in-race frame.
5. Opt-out `MASHED_NO_RACE_CAM_BASIS=1`: the previous resolution, byte for byte (LookAt, rig fallback, no
   countdown update). It exists only for A/B and goes with R3.
6. Diagnostic knobs, default OFF: `MASHED_CAM_DUMP=<csv>` (per-update pose + published basis),
   `MASHED_CAM_REPLAY=<trace csv>` (feed the original's per-frame camera inputs into `race_cam_`),
   `MASHED_CAM_REPLAY_SHOTS=K,...` (backbuffer at replay row K).

## 3. Instruments

- **Original:** `re/frida/camera_probe.py --frame` (extended): hook `FUN_00446520` onLeave (about 60/s,
  under the hot-path limit), per row the existing inputs plus `DAT_0063ba8c`, `cam[0]`, `cam[7]`, `+0x3c`,
  `+0x58`, `DAT_007f101c`, the RwCamera frame modelling matrix (`*(*(cam+0x84)+4)+0x10`) and LTM (`+0x50`),
  the RwCamera viewWindow (`+0x6c/+0x70`), entry 0's matrix (`0x0089650c`) and `+0xc0`. `--shots K` writes
  the d3d9 shim's request file from inside onLeave of row K, so the next Present dumps that row's frame.
- **Offline:** `re/tools/camera/cam_driver.exe` (adds aim / yaw / pitch columns).
- **Standalone:** `MASHED_CAM_DUMP`, `MASHED_CAM_REPLAY`, `MASHED_CAM_REPLAY_SHOTS`.
- **G-CTRL:** the `verify/d3_gatefire_20261008/run_h4.ps1` recipe (default arm), pointed at this worktree's exe.

Comparable row (used by G-ANGLE and G-POSE): `cam == 0x00897fe0`, `mode == 0`, all four cars `alive == 1`,
`dead == 0`, velocity gate 0 (the port models the standard path only, `cam_driver.cpp:18-26`).

## 4. Gates and predictions

| gate | pass criterion | prediction |
|---|---|---|
| **G-SRC** (original, live) | rows with `DAT_0063ba8c == 6`: (a) `cam[0] == 0` and `+0x3c == 0` on 100%; (b) frame modelling matrix == Camera::Apply(`+0x38`, `+0x34`, 0, `+0x40`) composed in float64, every basis component and pos within 1e-4, on >= 99%; (c) LTM(r+1) == modelling(r) within 1e-6 on >= 99% of consecutive pairs; (d) RwCamera viewWindow == (0.6, 0.45) +-1e-4 on 100%. Report only: flag rows, modelling == entry 0 matrix | PASS. If (b) or (c) fails, STOP: racing does not render Camera::Apply and R2's target is wrong |
| **G-ANGLE** (offline) | comparable rows: eye median <= 0.001 and p95 <= 0.01; yaw and pitch median <= 0.005 deg and p95 <= 0.05 deg; aim angle median <= 0.005 deg | PASS, medians at the 2026-08-26 level (eye 0.0001, aim 0.0007 deg) |
| **G-WIRE** (exe) | every in-race frame after the first camera update: `src = director`, `basis_valid = 1`, published eye/right/up/at bit-equal to the composed basis | PASS 100% |
| **G-POSE** (exe, replay of the G-SRC capture) | comparable rows with `DAT_0063ba8c == 6`: published eye vs original modelling pos median <= 0.001, p95 <= 0.01; at-axis and right-axis angle median <= 0.005 deg, p95 <= 0.05 deg; published view window == (0.6, 0.45) | PASS |
| **M-SHIFT** (mutant control) | the G-POSE scorer with the standalone rows offset by 30 | FAILS G-POSE (pos p95 > 0.01) |
| **N0** (no-op control) | two identical replay runs | byte-identical `MASHED_CAM_DUMP` |
| **G-FRAME** (side by side) | for each shot row K in `DAT_0063ba8c == 6`: imgdiff(orig@K, stand@K) mean-abs < imgdiff(orig@K, stand@K-60) and < imgdiff(orig@K, stand@K+60) | PASS on every K |
| **G-CTRL** | default build, run_h4 default arm: step / gates / ptrace = `991a5fff / be0d47ea / bc75b405` on the baseline exe AND the new exe; opt-out arm the same | unchanged. Reason: the only sim reader of `race_cam_` is `EliminationCheck` on `required_zoom_`, which `Update` recomputes from the current cars with no history (`RaceCamera.cpp:359-362`) |

Knob-took checks: the replay prints rows loaded and rows consumed; the dump's `src` column; the opt-out prints
one line naming itself.

## 5. Decision rule

All of G-SRC, G-ANGLE, G-WIRE, G-POSE (with M-SHIFT failing and N0 identical), G-FRAME pass, G-CTRL unchanged,
collateral clean -> default ON with the opt-out. Anything else -> default OFF, recommendation in RESULT_d1r2.md.

Collateral after each attempt: no new build warnings; the default race-demo flow still produces in-race frames;
a 12-float `MASHED_CAM_POSE` transplant still overrides; `MASHED_RENDER_LIBRW=0` still renders; a
`MASHED_SUBSTATE` run renders phase 3 from entry 0 without a crash.

## 6. Named gaps, out of scope

- The default build has no phase 3 (the `countdown_` stand-in, D-11073). Racing frames are R2's target.
- Under `MASHED_SUBSTATE` the exe never advances `DAT_007f101c`, so entry 0's roll phase stays 0, and
  `+0x10..+0x18` (tgt, `0x004481e5`) is not copied. Both are D-11073 lane items.
- `RwMatrixRotate` normalises its axis with the exe's exact-sqrt `FastInvSqrt` fallback, not the LUT
  (memory: exe RW sqrt is exact). For unit axes both give 1.0 to the ulp. G-POSE measures what remains.
