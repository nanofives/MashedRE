# RESULT d1r2: the verbatim race camera drives the renderer (D1-residue R2)

**Verdict: R2 closes.** Every pre-registered gate passed (`PREREG_d1r2.md`, committed 417e2b2b before the
first run). The change is wired default ON on `work/d1r2`, with the opt-out `MASHED_NO_RACE_CAM_BASIS=1`.
All numbers below are in `scores.txt`.

## 1. What the original renders, measured (G-SRC)

Two live captures on TRAINING Quick Battle (`re/frida/camera_probe.py --frame --early`, which hooks
`FUN_00446520` onLeave). Each holds the whole round, from the phase-3 reset to the round end at sub-state 7:

| | trace 1 | trace 2 |
|---|---:|---:|
| rows (director calls) | 1054 | 1029 |
| sub-state 3 / 4 / 5 / 6 | 766 / 1 / 112 / 175 | 741 / 1 / 112 / 175 |

The round ends after 175 racing frames in both captures. No input is held, and the zoom-saturation
elimination (`DAT_00898980` = 10) ends it.

- **Racing (sub-state 6), 175 / 175 rows in both captures.** The hold flag `cam[0]` = 0 and roll `+0x3c` = 0.
  The frame's modelling matrix equals Camera::Apply(`+0x38`, `+0x34`, 0, `+0x40`) composed in float64,
  max |d| 1.6e-7. The next row's LTM equals this row's modelling matrix exactly (0 on 174 / 174 pairs), so
  nothing rewrites the camera between the director and the render. The RwCamera viewWindow is
  (0.6000000238, 0.4500000179) on every row. That second value is the float SetupFOV's
  `0.75 * (1.0 * 0.6f)` produces. The old literal `0.45f` (0.4499999881) was 1 ulp below it.
- **Phase 3 (sub-state 3, hold flag set), 765 and 740 rows.** The frame equals entry 0's matrix at
  `0x0089650c`, |d| = 0, and `e0+0xc0` = 0.6. This is where the ±30° roll lives. The 2026-08-30 roll
  measurement (memory "race camera rolls") was taken in phase 3.
- **Sub-states 4 / 5 (flag clear), 114 rows.** Camera::Apply, max |d| 1.2e-7.

This settles the survey's open question ("racing frame [NOT ESTABLISHED] live"). It confirms the static
reading of `0x004481a3..0x00448208` and `dec_00446520.c:152-153`.

## 2. The port

- `RaceCamera::Update` now writes the Camera::Apply inputs, transcribed from `0x00447fc2..0x004481b6`: aim
  `+0x4c = tgt - pos`, yaw `+0x38`, pitch `+0x34` with the LUT FastSqrt `0x004c3b30` via `Math::LutSqrt`,
  roll `+0x3c = 0`, and overhead yaw 0. Fed the original's own recorded aim, this formula reproduces the
  original's yaw bit-exact on 1046 / 1054 and 1025 / 1029 rows, and pitch on 1033 / 1054 and 1016 / 1029.
  The worst row is off by 1 to 2 ulp (1.5e-5° / 3.1e-5°).
- `Race/CameraApply.cpp` builds the frame the way `0x00441760` does: identity (`0x004c15c0`), then the
  ported `RwMatrixRotate` about Y, about its own right and about its own at, then `RwMatrixTranslate`.
  When `DAT_00897fe0 != 0` it uses entry 0's matrix and view window instead, which is `0x00442a20`.
- TrackRenderer publishes the 12 floats (`last_basis_valid_`) and both renderers draw from them. The lens
  is SetupFOV's arithmetic on cam+0x58.
- The default countdown advances `race_cam_`, as the original's director runs through the pre-race
  sub-states. The first in-race Render, which comes before the first UpdateCar, renders from a copy of the
  director on its snap path. The sim's `race_cam_` is not touched.
- **The invented WS-E s3 chase rig and `MASHED_CHASE_RIG` are deleted.** The rig survives only inside the
  opt-out, so an A/B can reproduce the pre-d1r2 default.

## 3. Gates

| gate | result |
|---|---|
| **G-SRC** | PASS on both captures (section 1) |
| **G-ANGLE** (offline `cam_driver`, 1054 / 1029 rows) | PASS. Eye median 0.00013 / p95 0.0005. Yaw median 0.000015°, pitch median 0.0009° (p95 0.0035°), aim median 0.0007°. Tail: the phase-3 reset row and the 5 rows after it, eye 5.9 decaying to 0.005. That is the known reset transient (U-9203), on phase-3 rows where entry 0 renders |
| **G-WIRE** | PASS. librw's camera frame, read back after `SetCameraBasis`, is bit-equal to the published basis on 1299 / 1299 replay frames and 1498 / 1498 own-race frames, all `src = Camera::Apply` |
| **G-POSE** (exe replays the trace into `race_cam_`, published basis vs the original's frame, 175 racing rows) | PASS. Trace 1: eye median 0.00011 / p95 0.0010 / max 0.0026, at-axis median 0.0005° / p95 0.0038°, right-axis median 0.00002°. Trace 2: eye median 0.00011 / p95 0.0010, at-axis median 0.0004° / p95 0.0035°, right-axis median 0.00001°. View window float-equal on 175 / 175 |
| **M-SHIFT** (same scorer, 30-row offset) | FAILS as required: eye median 3.66, at-axis 10.8° |
| **N0** (two identical replay runs) | byte-identical dumps, sha8 `1E216EE8` twice |
| **G-FRAME** (original backbuffer at row K vs standalone at K and K±60) | PASS on 3 / 3. K=884: 27.77 vs 43.63 / 46.81. K=924: 21.94 vs 40.66 / 42.64. K=959: 15.75 vs 46.00 / 38.76. Side by side: `sbs_row_00884/00924/00959.jpg` (original left) |
| **G-CTRL** | PASS. On the final exe (b71e597a), default and opt-out arms both give `991a5fff / be0d47ea / bc75b405`, the recorded default. The two earlier builds give the same trio. Baseline exe: step and gates match, ptrace contaminated (section 5). All lines in `gctrl.txt` |

The camera residual adds nothing at the pixel level. Transplanting the original's own row-959 frame
(`MASHED_CAM_POSE`, 12 floats) scores 15.75 against `orig_row_00959.bmp`, which is exactly the ported
camera's 15.75. What remains in the side-by-sides is cars (only the camera is replayed; the standalone's
race runs underneath), the absent HUD, shadows and lighting, all outside R2.

## 4. Collateral

- No new warning class. The new lines add C4996 `fopen` / `getenv` instances, the same as the rest of
  these files.
- Race-demo flow (`MASHED_RACE_DEMO=1`, TRAINING) on the final exe: exit 0, all eight captures written,
  and the game camera on 397 / 397 in-race frames. Before the snap fix, one frame showed the track orbit.
- A 12-float `MASHED_CAM_POSE` still overrides: `src = 3` on 1299 / 1299 frames.
- `MASHED_RENDER_LIBRW=0` (legacy path) renders from the same published basis (G-POSE passes on its dump).
  Its frames are further from the original than librw's, 41.2 vs 21.9 at K=924. That is the legacy
  renderer R3 deletes, not the camera.
- `MASHED_SUBSTATE MASHED_MODE5_RESET`: no crash. 762 hold-flag frames render from entry 0 (`src = 2`),
  then 735 from Camera::Apply.

## 5. Found on the way

1. **The G-CTRL ptrace is a shared CWD file.** Every worktree run chdirs to the main root (`MASHED_ROOT`),
   so concurrent children append to one `player_trace.log`. My baseline arm hashed `91d73bac`, with lines
   from three PIDs interleaved. New knob `MASHED_PLAYERTRACE_OUT=<path>` (unset = the old path). The
   orchestrator was told at the time. My first runner also deleted that shared file once at about 01:58.
   It no longer touches the main root.
2. **The standalone's camera-path ribbon has the wrong `c3` corner on all 30 nodes.** It takes a top gate
   vertex (Y about 1.5 above `c0`), while the original's `FUN_00426d00(n, 3)` is the other bottom corner at
   `c0`'s height (`orig2_nodes.txt` vs `standalone_nodes.txt`). `dir` agrees to 7.6e-5 and `c0` exactly.
   The director reads `c3` only in `FUN_00441820`'s no-LED fallback (`RaceCamera.cpp:142-148`), so it is
   inert on TRAINING (LED elev 15 on all 30 nodes). Any track with a node lacking an LED elevation tilts
   about the wrong axis. Not fixed here.
3. Under `MASHED_SUBSTATE` the exe never advances `DAT_007f101c`, so entry 0's roll phase stays 0, and
   `0x004481e5`'s tgt copy into `+0x10` is missing (named in PREREG section 6, D-11073 lane).

## 6. Notes for D1-R1 (matched-pose draw-list adjudication)

- **The frame-sync method that works is row-indexed replay.** Run `camera_probe.py --frame --early
  --shots-race 30,70,105` on the original. It traces every director call from the phase-3 reset and writes
  the d3d9 shim's request file from inside onLeave of row K, so the next Present dumps row K's frame. Then
  `run_replay.py <exe> <out> --trace <that csv> --shots K,...`. The standalone consumes one trace row per
  camera update, and its shot is the frame whose camera consumed row K.
- **This is exact to the frame.** For each of the three original shots, the standalone frame at K is a
  sharp minimum: 27.77 / 21.94 / 15.75 at K, against 33.2 to 35.3, 26.7 to 29.3 and 23.4 to 28.3 at K±1
  and K±2.
- **Phase 3's length differs per run** (QPC clock: 766 vs 741 rows here), so pick shots relative to the
  first sub-state-6 row (`--shots-race`), not by absolute row. Racing lasts only 175 rows before the
  elimination ends the round with no input held. Capturing more needs a held-accelerate press in the probe.
- **Only the camera is matched, not the cars.** For draw-list adjudication R1 also needs car placement.
  The trace already carries each car's world position (`c{i}x..z`) but not its orientation.
- The committed canonical input is `orig2_trace_v3.csv` with `orig2_static.json` / `orig2_nodes.txt`.
  The original BMPs and the exe dumps are under `log/d1r2/` (BMPs are gitignored).
