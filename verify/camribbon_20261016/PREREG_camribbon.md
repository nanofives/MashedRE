# PREREG camribbon: the camera-path ribbon corners (`FUN_00426d00(n, 0/3)`) from the original's classification

Committed BEFORE the first measuring run. Branch `work/camribbon`, base `race/first-frame-parity` 118fb99d.
Brief: `verify/d1r2_20261016/RESULT_d1r2.md` section 5 item 2 (the standalone's `c3` is a top gate vertex, Y about
1.5 above `c0`, on all 30 TRAINING nodes) and item 3 (`MASHED_SUBSTATE` / `0x004481e5`).

## 1. What the bytes say (static, before any run)

| site | what it does |
|---|---|
| `FUN_00426d00` (hooks.csv C3, `Frontend/BatchAA_s6.cpp:96-99`, asm `Race/PathProgStep_naked.inc:1490`, `0x00426d0e`) | `&DAT_00663670 + k*0xc + n*0x4c` and nothing else. **It selects no corner.** It indexes a table somebody else filled |
| `0x00663670` | `= 0x00663658 + 0x18`: node record (stride 0x4c) corners at `+0x18 / +0x24 / +0x30 / +0x3c` for k = 0 / 1 / 2 / 3 (`Race/PathNodes.cpp:11-12`) |
| writer: `FUN_00478660`, per-node body `0x004786a0..0x00478c90`, classification loop `0x0047897c` | the node's 4 vertices go through the inverse of the node matrix (right, up, at = dir, pos = centre) into node-local space, then `x < 0 & y < 0 -> +0x18`, `x < 0 & y > 0 -> +0x24`, `x > 0 & y > 0 -> +0x30`, `x > 0 & y < 0 -> +0x3c` (`Race/PathNodes.cpp:253-265`, already ported and byte-identical to the original's live block, gate G-P1, `PREREG_PATHPROG.md`) |
| live original block, `verify/d3_modes37_20261009/o_course_t0.bin` (TRAINING, 30 nodes) | node 0: `c0 = (2.27054596, 0.425074965, -1.99563503)`, `c3 = (-2.22869587, 0.425074965, -2.00428462)`, `c1`/`c2` Y = 1.92492497. Both `c0` and `c3` are the two BOTTOM corners (local y < 0) |
| `FUN_00441820` fallback `0x004418e9..0x00441984` | only when the LED entry's first dword `== 0xbf800000` (-1.0f): out_dir = `FUN_00426cc0(n)` (record +0), then `FUN_00426d00(n, 0)` and `FUN_00426d00(n, 3)`, axis = `c0 - c3`, rotate out_dir by `0xc1c80000` (-25.0f) about it with `0x004c4d20` / `0x004c3df0` |
| the standalone today, `D3d9Render/TrackRenderer.cpp:2229-2267` | per gate: the unique vertex indices in stream order, SORTED ascending, `c0 = gv[0]`, `c3 = gv[3]`; `dir` = cross of two edges, forced toward the next gate. This ordering is not the rule the original's bytes show (`0x0047897c`). d1r2 measured `c0` equal and `c3` different on all 30 TRAINING nodes |

So the defect is in the **writer** (which vertex becomes `+0x3c`), not in `FUN_00426d00`. The fix is to take the node
corners from the ported classification (`BuildNode`) instead of the sorted-index guess.

## 2. The change under test

1. `Race/PathNodes.{h,cpp}`: new `PathNodes_CameraNodes(const Track::World&, std::vector<RaceCamNode>&)`. It runs the
   existing `BuildNode` (unchanged) on a local 0x4c record per AI node under the same PC_24 scope as
   `PathNodes_Build`, and keeps record `+0x00` (dir), `+0x18` (c0), `+0x3c` (c3). It does NOT touch the course block
   at `0x00657498` and is NOT gated by `MASHED_PATHPROG`.
2. `TrackRenderer.cpp`: `cam_nodes_` comes from that call by default (nodes = AI BSP material count, as
   `FUN_00478660`'s loop). Opt-out `MASHED_NO_CAM_RIBBON_FIX=1` keeps the previous stream-order corners byte for
   byte (A/B only).
3. Nothing else moves. `gates_` (AI scaffold) is untouched.

## 3. Instruments

- Standalone: `MASHED_CAM_DUMP=<csv>` writes `<csv>.nodes.txt` (dir, c0, c3, `%.9g`) at track load (d1r2).
  Extended here with a per-node LED column (`led_elev` present or not) so no-LED nodes are listed, not inferred.
- Original, nodes: `verify/d1r2_20261016/orig2_nodes.txt` (live capture, TRAINING) and the live course-block dumps
  `o_course_t0.bin` (TRAINING) / `o_course_t3.bin` (ARCTIC, 94 nodes), records at `0xc1c0 + i*0x4c`.
- Original, camera frame: `re/frida/camera_probe.py --frame --early` (extended here with `--track N`, engine
  track index as `scenario_launch.py --track`, and `--force-prog X`, see G-NOLED).
- G-CTRL: `verify/d1r2_20261016/run_gctrl.py`, default arm, baseline exe (frozen copy of the base build) and new exe.

## 4. Gates and predictions

| gate | pass criterion | prediction |
|---|---|---|
| **G-C3-TRAIN** | new exe, default, TRAINING: `c3` equals the original bit-for-bit (`float` equality) on 30 / 30 nodes against BOTH `orig2_nodes.txt` and `o_course_t0.bin` `+0x3c`. `c0` stays equal 30 / 30 | PASS |
| **G-DIR-TRAIN** (secondary) | same, `dir` equals `+0x00` 30 / 30 (the old normalised dir was 7.6e-5 off). If it fails, `dir` reverts to the old value and only `c0`/`c3` ship | PASS |
| **G-NODE-ARCTIC** | ARCTIC, 94 nodes, `dir`, `c0`, `c3` equal `o_course_t3.bin` on 94 / 94 (a second track: the classification is not tuned to TRAINING) | PASS |
| **C-OPTOUT** (control, must FAIL the gate) | `MASHED_NO_CAM_RIBBON_FIX=1` on TRAINING: `c3` differs from the original on 30 / 30 and the nodes file is byte-identical to `standalone_nodes.txt` (the d1r2 baseline). It is also the no-op control: the opt-out must reproduce the previous behaviour exactly | `c3` 30 / 30 differ, file identical |
| **N0** | two identical default runs give byte-identical nodes files | identical |
| **G-SCAN** | all 13 tracks load; per track report node count, nodes whose `c0`/`c3`/`dir` changed against the opt-out arm, and nodes with no LED elevation (`elev == -1`). A track with >= 1 such node must exist (offline scan of `LED.piz`: City `LE26` nodes 25, 26, 27, Forest `LE34` from 162) | >= 1 track found |
| **G-NOLED** (camera, original vs standalone, before/after) | on a track with a no-LED node pair: original `camera_probe.py --frame` capture with every car's path progress pinned inside that pair (`0x00408a50` return forced, the ONLY instrumentation), standalone replay of that trace. After arm: published pose vs the original's frame within the d1r2 G-POSE tolerances (eye median <= 0.001, p95 <= 0.01; at-axis and right-axis angle median <= 0.005 deg, p95 <= 0.05 deg) on the rows that sample the pair. Before arm (opt-out): must EXCEED those tolerances, otherwise the scenario did not exercise `c3` and G-NOLED is reported as vacuous | after PASS, before FAIL |
| **G-CTRL** | `run_gctrl.py` default arm: step / gates / ptrace = `991a5fff / be0d47ea / bc75b405` on the baseline exe AND the new exe, and on the opt-out arm. Reason: `cam_nodes_` is read only by `RaceCamera::NodeDir`, whose only sim consumer is `EliminationCheck` via `required_zoom_` (`RaceCamera.cpp:359-362`), and TRAINING has LED elevation on every node | unchanged |
| **G-P1-REGRESS** | `MASHED_PATHPROG=1 MASHED_PATHPROG_DUMP` course block, TRAINING: byte-identical between the baseline exe and the new exe (the refactor reuses `BuildNode`; it must not change the block) | identical |

Knob-took checks: the nodes dump header line prints which resolution produced the nodes (`classified` or
`stream-order (opt-out)`), and the log line names the opt-out when it is on.

## 5. Decision rule

G-C3-TRAIN, G-NODE-ARCTIC, C-OPTOUT (failing as required), N0, G-SCAN, G-CTRL, G-P1-REGRESS pass -> default ON with the
opt-out. G-NOLED passes -> the camera effect is shown. If G-NOLED cannot be captured (original refuses the pin,
scenario does not load) it is reported as NOT ESTABLISHED, and the default-ON decision rests on the node-level
gates, which compare against the original's own live node records. Anything else failing -> default OFF and the
recommendation goes in `RESULT_camribbon.md`.

Collateral after each attempt: no new build warnings; race-demo flow still produces in-race frames; a 12-float
`MASHED_CAM_POSE` still overrides; `MASHED_SUBSTATE` still runs without a crash.

## 6. Item 3 of the brief (`MASHED_SUBSTATE` / `0x004481e5`)

Decided after the main work: changed only if the fix is small and cited from the bytes
(`re/analysis/race_camera/FUN_00446520.asm` `0x004481e5..0x004481f9` copies tgt into `+0x10..+0x18`; the
`DAT_007f101c` advance). Otherwise filed as a tracker row with the evidence needed.
