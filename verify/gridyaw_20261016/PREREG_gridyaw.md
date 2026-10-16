# PREREG gridyaw: the original's grid frame (FUN_00426cc0 direction) for the initial spawn

Branch `work/gridyaw` from `work/d2lateral` `5e3fa3a8` (= `race/first-frame-parity` `f9cdbb94` + d2lateral, every
d2lateral knob default OFF). Base exe built from `5e3fa3a8` unmodified: sha8 `30ff67b3`. Model claude-opus-5-5,
account3. Registered before any run of this lane. User request 2026-10-16: "port FUN_00426cc0 for the initial spawn
behind a knob" (d2lateral U-9713, d2posmodel 5.2).

## 0. Facts read before any run (the bytes and existing captures only)

0.1 **The original's grid.** `FUN_0040e590` (race-start / round-start placement) calls, per car,
`FUN_00408b00(DAT_0063ba80, slotValue, 4, &pos, &yaw)` and then `FUN_004704c0(car, pos.x, pos.y, pos.z, yaw,
&DAT_00803324)` (`verify/d3_bulk_20261011/roundgrid/stageb/FUN_0040e590_decomp.txt`).
- `FUN_00408b00` (`verify/grid_orig_ts/FUN_00408b00_decomp.txt`): A = `FUN_00426cb0` (node record + 0xc),
  L = `FUN_00426cc0(node)` = `&DAT_00663658 + node*0x4c` (record + 0), F = normalize(`FUN_00426d00(node,0)` -
  `FUN_00426d00(node,3)`).
- **Yaw law:** `FUN_004c3c60` 2-D normalizes (L.x, L.z) with the RW LUT. `FUN_004726b0` takes the 2-D dot with
  (0, 1), clamped to [-1, 1] by `_DAT_005cc33c` = -1.0 (0xbf800000) and `_DAT_005cc320` = 1.0. `FUN_004a3384` (CRT
  acos, hooks.csv) times `_DAT_005cc98c` = 57.29578 (0x42652ee1). If the normalized x is < `DAT_005d757c` (0.0), the
  yaw becomes `_DAT_005ccac4` (360.0) minus that.
- `FUN_004704c0`: `RwMatrixTranslate(m, pos, 0)` (`0x004c51a0`), then `RwMatrixRotate(m, &DAT_006146fc, yaw, 1)`
  (`0x004c4d20`). `DAT_006146fc` = (0, 1, 0). So the car faces R_y(yaw) (0, 0, 1) = (sin yaw, 0, cos yaw) = the
  2-D-normalized L.

0.2 **The port.**
- Slot 0 is placed twice, at R6 (`TrackRenderer.cpp`, `car_yaw_ = atan2(g1 - g0)` + `ResetOrientation`) and in
  `StartRound` (`place`: `*yaw = atan2(dir)`, `dir` = normalize(gate1 - gate0), anchor `gates_[0]`, `lat` =
  perp(dir)). Both are stand-ins for 0.1.
- `MASHED_ROUND_GRID` (default OFF, needs PATHPROG or the publish bit) already reads the node record
  (`RoundGrid_AnchorAt`: A, lat = x/z of normalized F, dir = x/z of the raw L) at the snapped `DAT_0063ba80`.
- The course block that holds the records is built by default since u9685 (`RacePctOrig_Enabled`, PathNodes.cpp),
  so it exists on a default Training run.
- The port's yaw convention: fwd = (cos yaw, 0, sin yaw) (`BodyOrient_Init`).

0.3 **Conflicting witnesses for the original's slot-0 heading.**
- (a) **Measured on the original:** spawn fwd (-8.742278e-08, 0, -1) (`scenario_launch` spawn line, `orig_lx1`)
  and body heading -89.9989 deg at launch (d2lateral A3.2), i.e. yaw 180.0 exactly in 0.1's law.
- (b) **Derived, not measured:** d2posmodel's `D_BRG` (`MASHED_ROUND_GRID`, the record path) puts slot 0 at port
  yaw -1.568874, which is 0.11 deg off -z.

Either the record direction at the snapped node is (0, ·, -1) and (b) took another node or another quantity, or the
original's initial spawn heading does not come from `FUN_00408b00`. That is decided by M0 before any port.

## 1. M0, the original-side grid capture (report, decides the port)

`orig_grid1`: the original, `orig_solo3`'s argv (`--statediff-drive --statediff-drive-late --statediff-steer 1
--hold 38 --poke-ctrl-slots`) plus:
- `--grid-probe`: `FUN_00408b00` entry + leave, giving the node, slot value, mode, `*pos` and `*yaw`;
- `--dump-range 0x00663658:0xa0a0:<out>.nodes.bin`: node records, the Safe_Start_Lines table `0x0066d658`, its count
  `0x0066d6e0` and the node count `0x0066d6d8`, after phase 3;
- `--dump-range 0x0063ba80:4:<out>.node.bin`.

Own PID only, the shared `original\` read-only. A piz-collision crash (U-9677 signatures) is re-run.

M0 gates: grid-probe `err null` with at least one row, and a known answer (the recorded slot-0 `*pos` equals the
`.msd`'s first player position, within 0.01 in x and z).

**M0 decision (fixed now).**
- **M0-A:** the slot-0 row's `*yaw` = 0.1's law applied to the dumped record direction at the row's node, and it is
  the yaw the car spawns with (fwd (sin yaw, 0, cos yaw) = (a)). Then port 0.1 for the initial grid (section 2).
- **M0-B:** `FUN_00408b00` is not called at the initial spawn, or its yaw is not the spawned heading. Then report,
  author no knob, and name the writer of the spawn heading from the capture as the next measurement.

## 2. The knob (written only if M0-A): `MASHED_GRID_DIR=1`, default OFF

- In `StartRound`, when `RoundGrid_Anchor` is not active: node = `SnapNodeToSafeStartLines(0)` (`FUN_00409290`,
  `DAT_0063ba80` = 0 at race init, `0x0040fef0`). `RoundGrid_AnchorAt(node)` supplies A, lat and dir, and every
  slot's yaw comes from 0.1's law on that record's L, converted to the port's convention
  (`yaw = atan2(cos t, sin t)`, t = the law's degrees in radians).
- At R6 the same yaw replaces the gate stand-in.
- If the course block has no nodes, the gate frame stays.
- The port grids every round at node 0 (U-9686), so the knob applies to every StartRound, as the stand-in does.
- `MASHED_D2LAT_SPAWN_YAW` (diagnostic) still wins if set.

## 3. Gates

- **no-op:** `H_DEF`, `H_DEF-2` on this build = `BASE_H_DEF` on `30ff67b3` (sha8 step / gates / ptrace).
- **knob-took:** the knob's trace line gives node, record L, the law's yaw in degrees and the port yaw. On `D2_GFX_1`,
  the slot-0 body heading at the first moving step equals the original's (-89.9989 deg) within 0.001 deg.
- **G-GRID (report):** the four slot positions (x, z) on `H_G` (`MASHED_DBG_CARPOS` or ptrace frame 0) against the
  original's frozen grid (`verify/grid_orig_ts/ANALYSIS.md`), within 0.01.
- **G-SETTLE** (`g_settle.py`, d2posmodel's gate) on `D_G`: x bit-identical, |dz - 0.0000921488| <= 2.4e-7,
  |dy + 0.0029474455| <= 1e-5.
- **determinism:** `H_G` = `H_G-2`.
- **D2 three metrics** (`score_d2.py`, bounds `d81a8df6`) and **H1**, 3/3, on `D2_G_1..3` (knob alone) and on
  `D2_GL_1..3` (knob + `MASHED_D2LAT_REFRESH`, as one unit).
- **D3 (b)** per regime on `B_G_K{0,1,26}` (and `B_GL_K*`): not below `BASE_B_DEF_K*`.
- **collateral:** `H_G` (floor `H_G-2`) at most 2 fields above `H_DEF` (floor `H_DEF-2`).

**Default rule:** `MASHED_GRID_DIR` goes default ON (opt-out `=0`) only if every gate passes on `G`. The pair `G+L`
is scored as a unit: if it passes every gate while `G` alone does not, the RESULT says so and no default moves (user
decision). If driving-median enters its bound in any arm, that is reported first.

## 4. Predictions

- 4.1 (no-op) PASS. The knob code is not reached with the knob off.
- 4.2 M0: no direction predicted between M0-A and M0-B. [UNCERTAIN] which; 0.3 holds two witnesses that disagree.
- 4.3 If M0-A: knob-took PASS. Then D2 on `G` alone matches d2lateral's `D2_Y_1` (slips OUT, H1 pass), and on `GL` it
  matches `D2_YL_*` (slips in, driving-median about 1985).

## Amendment 1 (2026-10-16, after `orig_grid1`; before any code change)

A1.1 **M0 gates.** The grid-probe returns 2 rows, `err null` (frames 1 and 2, caller `0x0040ee56`, node 0, slot value 0,
mode 1, rank codes -1000 x4). Known answer: slot-0 `*pos` = (0.50093675, 1.175, -1.99903715), and x/z equal the
original player's frame-1 position bit-exactly (g_settle.py `O1`: 0.50093675, -1.99903715). Dumps:
`DAT_0063ba80` = 0, node count 30, Safe_Start_Lines count 2, ranges (0..11), (14..29).

A1.2 **M0-A holds.** `*yaw` = **180.0** on both rows, and the spawned fwd is (-8.742278e-08, 0, -1) = (sin 180, 0,
cos 180) in float. The node-0 record direction is L = (0.00192259776, 0, -1.00007451) (bits 0x3afbffad / 0x00000000 /
0xbf800271). 0.1's law reproduces 180.0 exactly (`orig_grid1` reduction, numpy float32 + the LUT tables of
`Math/RwLutTables.cpp`), as follows:
- `FUN_004c3c60`: m2 = L.z*L.z + L.x*L.x = 1.0001527, LUT inverse sqrt of it = **1.0** (the table bucket), so the
  normalized pair stays (0.0019226, -1.0000745).
- `FUN_004726b0`: the dot with (0, 1) = -1.0000745, **clamped to -1.0** by `_DAT_005cc33c`.
- acos(-1) * 57.29578 = 180.0. x >= 0, so no mirror.
- `RwMatrixRotate(Y, 180.0)` gives at = (-8.742278e-08, 0, -1): the measured fwd, bit-exact. Port yaw =
  atan2(at.z, at.x) = **-1.5707964**.

The raw `atan2(L.z, L.x)` = -1.5688739 is d2posmodel's `D_BRG` value: `RoundGrid`'s yaw (`*yaw = atan2(dir)` on the
raw record direction) is **not** the original's law, and on Training it is 0.11 deg off. Witness (b) of 0.3 is
explained.

A1.3 **The knob, refined (replaces section 2's yaw sentence).** `MASHED_GRID_DIR=1` computes every slot's yaw with
0.1's law, LUT 2-D normalize and clamp included, from the record direction of the node the grid is anchored at: the
`RoundGrid_Anchor` node when that knob is active, else `SnapNodeToSafeStartLines(0)` with `RoundGrid_AnchorAt` (A,
lat, dir) as section 2 says. A new helper `Math::LutNormalize2D` carries `FUN_004c3c60`'s arithmetic. The rotation
runs in float: rad = deg * 0.017453292f, at = (sin, cos). The conversion to the port is `yaw = atan2(at.z, at.x)`.
The R6 spawn uses the same node-0 yaw.

A1.4 **Prediction (registered now).** Knob-took: the trace reads node 0, L bits 0x3afbffad / 0xbf800271, law 180.0,
port yaw -1.5707964 on Training. On the D2 arm, slot 0's first-step heading equals `D2_Y_1`'s
(`MASHED_D2LAT_SPAWN_YAW=-1.5707964`), and the position x/z equals `D_BRG`'s (0.50093675, -1.99903715) within 1e-6.
