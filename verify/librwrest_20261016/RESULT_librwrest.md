# RESULT librwrest: the rest of the race render onto librw (D1-residue R3 follow-ups)

Branch `work/librwrest`, base `5d1e2b0a`. PREREG `PREREG_librwrest.md` committed first (`f5ae95a1`).
Model claude-opus-5-5 (account2, no Ghidra MCP: decomp via `re/tools/decomp_pc.py`).

**Verdict.** Liveries, wheel/hub/body frame poses, per-car relight and the copters now render through
librw with the original's logic. All default ON with an opt-out knob. The unpaired librw probes and the
inert `RwWorldRender.{h,cpp}` are deleted.

Results, all against a baseline exe built from `5d1e2b0a`:
- **D-LIST (d1r1's 9 frames):** RED stays 0, rows drop on every frame, and no matched draw stops
  matching (section 1).
- **G-CTRL:** unchanged, `c2e4d60d / b50bd39b / ba90ed88` before and after (section 1).
- **Not ported:** pickups (DEFERRED D-11076) and the car part 0x12 / 0x14 animations (D-11077).
- **World UV scroll:** measured empty. No `GRAPH*.BSP` on any of the 13 tracks binds a UV animation.
- **Original-side captures** (livery witness L1, wheel-law witness W1) did not run. The PC's only audio
  endpoint is in state Unknown, so `MASHED.exe` AVs at boot (`0x005baf48`, NULL audio renderer).
  Parent informed.

## 1. Gates

| gate | result |
|---|---|
| G-CTRL TRAINING, 14400 f, `verify/d1r2_20261016/run_gctrl.py` | base `5d1e2b0a` `c2e4d60d / b50bd39b / ba90ed88` (18664 / 56153 / 28076 lines); final `745379aa` identical hashes and line counts (`gctrl.txt`) |
| D-LIST, 9 d1r1 frames, `dl_compare.py` (`DLIST_<track>.md`) | rows TRAINING 80/79/79 -> 77/76/76, Arctic 80/80/80 -> 76/76/76, City 156/154/152 -> 153/151/149; RED 0 -> 0; original draws newly unmatched: 0 on 9/9; matched draws TRAINING +2 per frame |
| L1 livery (original rec+0x268 DFF per car) | NOT RUN (no audio device). Rule checked statically instead (section 2) |
| W1 wheel/body law vs live inputs | NOT RUN (same) |
| W2 frame structure, from the EXISTING d1r1 original draw records (`wheel_struct.py`) | PASS 136/136 wheel draws: `W * Q^-1` at a hub frame's streamed position, max error 4.2e-5; right.y = 0 exactly, i.e. `Rx(spin) * Ry(steer)` with the wheel frame below its hub; front-wheel steer nonzero on several rows (e.g. TRAINING 959 car 2 +0.84 deg) |
| R1 relight shade vs the original's live rec+0x284 (rtex `orig_matfx2_*` grid snapshots, `run_shade.py`) | 12/12 cars within 0.0047. City 4/4 within 1e-5, Arctic within 0.0018, TRAINING within 0.0047. The residual follows the grid-position difference (0.3 to 7 cm) |
| I-N0 no-op control | PASS: the final exe twice on the TRAINING trace gives byte-identical draw lists and BMPs on 3/3 frames |

Rows per citation that changed, base -> post (`DLIST_*.md`):
- **C01-CULL:** TRAINING 22 -> 20, Arctic 12/13/14 -> 8/9/10, City 27 -> 24. The off-screen copters
  are now culled, as the original does.
- **C14-ANIM** (TRAINING): 2 -> 1. JetRanger and tail-rotor draws now match in geometry and state. The
  main rotor's MOVED fell from 1.01 to 0.0233 but stays cited.

## 2. What the original does, and what was ported

**Liveries.**
- `FUN_0040d110` @ `0x0040d110`: car i gets vehicle id `DAT_007f1a1c[i*4] + (id/6)*6`
  (`FUN_0042f6a0` = `DAT_0067e9fc` == 0xb: `i + (id/6)*6`).
- `FUN_00420420` loads that id's DFF from the table at `0x005f37a8`: stride 0x84, +0x10 DFF, +0x80 id
  (`FUN_0041f330`, id in ESI).
- Dump of all 78 entries (`vehtable` output, this session): every DFF is name + id % 6.
- Port: `TrackRenderer::ResolveCarLiveries`, one librw model per livery DFF, re-resolved when a colour
  changes. `MASHED_CAR_LIVERY=0` reverts.
- At colour 0 (every d1r1 car, the all-RED reference) this is a no-op.

**Car frames.**
- `DffModel` keeps the FRAMELIST and each batch's frame. `BuildClump(hierarchy)` builds the DFF's own
  frame tree with un-baked vertices, so an atomic's world matrix is its frame LTM, as in RW.
- `FUN_00420870` is called from `FUN_0040da50`, at `0x0040fc75`, before the physics at `0x0040fc9b`.
- `FUN_0041f3d0` @ `0x0041f3d0`:
  - Spin: `rec+0x220+w*4 += units * [0x005cc948] * spin(w)` (`fimul` at `0x0041f434`, one `fst`),
    then +-360 folds. spin(w) = `FUN_0046bd60`, veh +0x1a4 + w*0xc4.
  - `RwFrameRotate(wheel frame, X, angle, REPLACE)`.
  - `RwFrameRotate(parent, Y, FUN_0046bda0 16-float mean, REPLACE)`, then translate back
    (POSTCONCAT).
- `FUN_0041f4f0` @ `0x0041f4f0`, part 0x13's frame: Z by `(b.x + a.x) * 40`, then X by
  `(b.z + a.z) * -100`, with a = veh+0xaf4 and b = veh+0xb00.
- Constants are read off the image.
- Port: `CarParts_FrameTick` (at FUN_0040fc00's position, before `StepPlayer`) plus `BuildCarPoses`,
  using the ported `RwMatrixRotate` 0x004c4d20 and `FUN_004c51a0`. Steps are single precision (the race
  loop runs at x87 PC_24).
- Both LOD sets (parts 0..3 and 0x21..0x24) are posed: the original poses the set rec+0x298 selects and
  hides the other (car LOD is carlod's), and the d1r1 records show set B drawn on 9/9 frames.
- `MASHED_CAR_PARTS=0` reverts.
- Diagnostic replay (`MASHED_CAR_REPLAY`): root = Q{i}, and the body pose is recovered as
  `X^-1 F (Y Q)^-1`.

**Relight.**
- `FUN_00420050` @ `0x00420050` scales `DAT_0063d850` around each car's `RpClumpRender`
  (`0x004200ea..0x00420149`, restore read `0x004201d7`).
- `DAT_0063d850` = `FUN_004260b0` = `[0x0065743c]` = course `0x00646e58` + 0x105e4, the DIRECTIONAL
  light. The scale is rec+0x284, rtex's `FUN_0041f590` port, now run for every car.
- Port: `RaceSubmit_AddInstancePosed(..., sun_scale)`. `MASHED_CAR_RELIGHT=0` reverts.
- Found on the way: `TrackWorld` read rpWORLDNORMALS sectors with 12-byte normals. RW world sectors
  carry 4-byte `RpVertexNormal`.
  - Only Arctic AI.BSP (0x71) and COLLISIONS.BSP (0x79) have the flag. For them the solver had matched
  "no normals, no prelit, 1 UV set", so Arctic's collision prelit was dropped and the shade read 1.0
  (`FUN_00479100` returns 1.0 only for a sector without colours).
  - Fixed for flagged worlds only. Triangle offsets are unchanged, so the sim is unchanged.

**Copters.**
- `FUN_00425d50` @ `0x00425d50` (called from `0x0040fcb9`):
  - Anim clock `slot+0x40 += units/3000`, less the duration when past it.
  - `FUN_004752f0` sampler: slerp with `FUN_00546e70`'s fdlibm acos and sin polynomial, position by
    `FUN_00482ae0` over four keys.
  - Rotors by `units * +-0.05` deg about their own up axis (`FUN_00425ca0`).
- `FUN_004516d0` slots: the 1st/2nd/3rd atomic in RW clump order go to body / +8 / +0xc. RW clump order
  is the reverse of the stream order, which the d1r1 draw order confirms (body, tail rotor = streamed
  atomic 0, main rotor = atomic 1).
- `FUN_00425e40`: frustum test (`FUN_004c1b40`) on the body atomic's world sphere (the streamed geometry
  sphere is now kept), then body, slot+0xc, slot+8.
- The original's copter draws run with the RW driver states (d1r1 F1 rule on these rasters only):
  `RaceSubmit_SetModelRwStates`.
- `FUN_00482ae0`'s port (`SplineCubicBlend3`, naked) read its `.rdata` coefficients as 0 in the exe. It
  got an exe arm (rva-lint caught the duplicate body).
- The D3D9 copter path and its `MASHED_RPLIGHT` bake are deleted. `MASHED_RPLIGHT=0` no longer feeds
  anything in the copter path. The `drawstream3d` record loses its `copters` key.

## 3. Cleanup

- **Deleted probes:** `MASHED_WORLD_MATID/ONLYMAT/PRELITONLY/FLATSNOW/VDUMP` and `MASHED_PROP_VDUMP` in
  `RwSceneBuild.cpp`. A grep of every .py/.js/.ps1/.bat under re/, verify/, scripts/ and mashedmod/
  found 0 users.
- **Deleted TU:** `D3d9Render/RwWorldRender.{h,cpp}`, removed from `exe_sources.rsp`.
- **re-classify:** the 8 hooks.csv rows lose their `exe_file` (004e5680, 004d40d0, 004e47b0, 004e5190,
  004e5660, 004e5f90, 004f0900, 004c1b40). All stay C2, evidence unchanged, plus 8 CHANGELOG lines.
- `FLAG_INVENTORY_2026-08-15.md` updated.

## 4. World UV scroll

`uvanim_scan.py` (`uvanim_scan.txt`): world-BSP UV-anim bindings = 0 on 13/13 tracks. Every binding sits
on a DFF prop or sky, e.g. Arctic `SEA.DFF`/`SKY.DFF`, City `ANIMATIONS.DFF` (18), TRAINING `LAKE.DFF`,
Storm and Sands `WATER.DFF`. Props already scroll on librw (`RegisterUvAnim`, prop scope), and the sky
is rprops'. TRAINING's lake flipbook (C11, water06..31) is a texture swap, not UVAnim.

## 5. Not done, follow-ups

- **L1 and W1 owed.** On audio return, run `camera_probe.py --carparts` (added, `f5ae95a1`) on the 3
  tracks. Then check the rec+0x268 DFF per car and the spin/steer/lean law, and add the inputs replay to
  the standalone. Also one original race with non-zero colours.
- **Pickups, D-11076.** The IconCube pose and skin are pool state that only the `FUN_00459000` port
  (`MASHED_PICKUPS_ORIG`, default OFF) simulates. The glow is the particle emitter, which is rfx's scope.
- **Parts 0x12 / 0x14, D-11077.** Morph interpolation and `FUN_00422470`.
- **Body matrix.** The render places every car by `car_pos_`/`car_yaw_`. The original's root is the
  full physics matrix (`FUN_0046d4a0` at `0x0040dac2`), so cars do not pitch or roll on slopes. This is
  physics scope.
- **Grid positions.** They differ from the original by 0.3 to 7 cm (`SHADE.txt`).
- **hooks.csv exe_file.** The new bodies (0x004752f0 `CopterAnimMatrix`, 0x00425d50 `CopterTick`) could
  be recorded there. No C-level was claimed: none of these bodies has a Frida diff.
