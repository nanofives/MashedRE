# PREREG librwrest: the rest of the race render onto librw (D1-residue follow-ups of R3)

Branch `work/librwrest`, base `race/first-frame-parity` `5d1e2b0a`. Written before the first
measuring run. Model claude-opus-5-5 (account2: no Ghidra MCP, decomp through
`re/tools/decomp_pc.py`).

## 1. Items, in the order of work (visible impact first)

| id | item | original code (decomp read this session) |
|---|---|---|
| L | AI car liveries | `FUN_0040d110` @ `0x0040d110`: vehicle id per car = `DAT_007f1a1c[i*4] + (id/6)*6` (mode 0xb: `+ i`), `FUN_00420420(i, id)`; `FUN_0041f330` @ `0x0041f330` (id in ESI) returns the 0x84-byte entry of the table at `0x005f37a8` (+0x10 DFF name, +0x20 TXD, +0x30 env texture, +0x80 id); `FUN_00420420` loads the entry's DFF (`FUN_0042a5d0`, desc+0x10) per car and stores the entry at rec+0x268 |
| W | wheel spin + steer, body lean | `FUN_00420870` @ `0x00420870` (from `FUN_0040da50` @ `0x0040da50`, called at `0x0040fc75` with FUN_0040fc00's 1/3000 s units, BEFORE the physics calls at `0x0040fc9b`): `FUN_0041f3d0` @ `0x0041f3d0` per wheel w: `rec+0x220+w*4 += units * [0x005cc948] (1/3000) * spin(w)` (`FUN_0046bd60`, int at veh+0x1a4+w*0xc4), wrapped at +-360 (`[0x005ccac4]`, `[0x005cd270]`); `RwFrameRotate(wheel frame, X [0x006146f0], angle, REPLACE)`; `RwFrameRotate(parent frame, Y [0x006146fc], steer(w), REPLACE)` (`FUN_0046bda0`, 16-float mean) then translate back to its saved position (POSTCONCAT). Wheel atomics = parts 0..3 when rec+0x298 == 0, parts 0x21..0x24 when == 1. `FUN_0041f4f0` @ `0x0041f4f0`: part 0x13's frame: rotate Z by `(b.x + a.x) * [0x005cd274] (40)` REPLACE, then X by `(b.z + a.z) * [0x005cd038] (-100)` POSTCONCAT, translate back (a = veh+0xaf4 via `FUN_0046bce0`, b = veh+0xb00 via `FUN_0046bd20`). Frame primitives: `FUN_004c1520` = RwFrameRotate (RwMatrixRotate `FUN_004c4d20` on frame+0x10), `FUN_004c1340` = RwFrameTranslate, `FUN_004c1480` = RwFrameTransform |
| R | car relight | `FUN_00420050` @ `0x00420050`: around each car's `RpClumpRender` (`FUN_004e6680`), `RpLightSetColor` (`FUN_004e4900`) on `DAT_0063d850` with its colour times rec+0x284, restored after. `DAT_0063d850` = `FUN_004260b0` = `[0x0065743c]` = course struct `0x00646e58` + 0x105e4, the DIRECTIONAL light (FUN_00479330 stores subtype 1 there). rec+0x284 = `FUN_0041f590` @ `0x0041f590` (already ported by rtex for the env coefficient) |
| C | copters onto librw | (survey pending) |
| P | pickups onto librw | (survey pending) |
| U | world UV scroll | (survey pending) |
| X | cleanup | unpaired librw probes in `RwSceneBuild.cpp`; `RwWorldRender.{h,cpp}` (8 hooks.csv rows through re-classify) |

## 2. Instruments

- Existing: d1r1 matched-pose per-draw recorder (`re/frida/camera_probe.py --drawrec --carframes`,
  standalone `MASHED_DBG_D3DREC`), `re/tools/drawlist_diff.py` 3D mode with
  `verify/d1r1_20261016/cites_d1r1.json`, the 9 shots (TRAINING 884/924/959, Arctic
  1051/1091/1126, City 761/801/836), `verify/d1r1_20261016/run_replay3d.py`.
- NEW original side: `camera_probe.py --carparts` writes `<trace>.carparts.json`: once, per car
  the colour `DAT_007f1a1c[i*4]` and the rec+0x268 table entry (name, DFF, TXD, env, id); for the
  60 rows ending at each shot row, per car: rec+0x220 angles, rec+0x298, rec+0x284, rec+0x294,
  the spin ints, the steer means, the two vec3s, and for parts 0..3, 0x21..0x24, 0x12..0x14 the
  atomic frame's modelling + LTM and the parent frame's modelling; plus the colour of
  `DAT_0063d850`. Rows come from the FUN_00446520 hook, which runs from `0x0040fc7c`, after
  `FUN_0040da50` and before this frame's physics, so a row holds the inputs FUN_00420870 used and
  the frames it wrote.
- NEW standalone side (default OFF, diagnostic): replay of the original's per-row part inputs in
  the d1r1 replay, so wheel/body poses are compared at matched poses.

Original-side captures that only read state (this probe) are instrument runs. Every number
quoted as a result comes from runs after this file is committed.

## 3. Gates

- **G-CTRL** (default build): `verify/d1r2_20261016/run_gctrl.py`, TRAINING 14400 frames, private
  ptrace. Baseline exe built from `5d1e2b0a` must give `c2e4d60d / b50bd39b / ba90ed88`; the
  branch default build must give the same three. These items are render-side; a sim hash change
  is investigated with a no-op layout control (U-9698) before any conclusion.
- **D-LIST** (default build, 9 d1r1 shots, replay knobs as d1r1): per frame, rows from
  `drawlist_diff.py --cite cites_d1r1.json`: RED stays 0, the total row count does not rise against
  the baseline exe on the same traces, and no previously matched draw stops matching. Rows the
  items address (C06 PICKUP, C14 ANIM copter rows, copter/pickup STATE rows) are reported before
  and after.
- **L1** livery: the standalone's per-car DFF entry (logged at load) equals the original's
  rec+0x268 entry DFF for all 4 cars on the 3 d1r1 tracks.
  **Prediction Pr-L:** every car in the 3 d1r1 captures has colour 0 and `ADVANTAGE0.DFF` (the
  race_hud all-RED reference, Finding 16), so on those frames the livery port changes nothing.
  If Pr-L holds, L1 is also run on one original scenario with a non-zero colour if the scenario
  tooling can set one; otherwise L rests on L1 plus the selection rule's code citation.
- **W1** law, on the carparts windows of all 3 tracks: for every row, car, and wheel, the frame
  matrices the port's code computes from that row's inputs (spin, steer, vec3s, previous row's
  angle, units = the ff4 delta) equal the original's recorded modelling matrices, |d| <= 1e-4 per
  element; and the recorded angle equals the port's accumulated angle.
- **W2** matched pose: in the d1r1 replays with the part replay on, the world matrix of every
  matched car-part draw (wheels, body subtree) equals the original draw's D3DTS_WORLD within 1e-3
  per element (the standalone builds the car clump with the DFF's own frame hierarchy so its
  atomic world matrix IS the frame LTM, as in RW).
- **R1** relight: at the shot rows the standalone's per-car directional scale equals the
  original's rec+0x284 within 1e-3 where the shade inputs match (rtex's `CollisionPrelitSample`
  carries U-9697); reported per car, not averaged.
- **No-op control**: two runs of the same exe on one trace give byte-identical draw lists
  (I-N0 as d1r1).

## 4. Defaults

A ported item that passes its gates is default ON with an opt-out knob. One that fails stays
default OFF with a recommendation. Copters/pickups move onto librw only if D-LIST does not get
worse; otherwise they stay on D3D9 and the reason is reported.

## 5. Not in scope

Sky (rprops), particles/FX (rfx), car LOD visibility and car gloss (carlod), physics (d2posmodel).
The car body matrix itself (the render uses `car_pos_`/`car_yaw_`, the original the full physics
matrix via `FUN_0046d4a0` at `0x0040dac2`) is reported as a finding if it bears on W2, not changed.
