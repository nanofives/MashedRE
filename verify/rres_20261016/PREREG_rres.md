# PREREG rres: render residuals (C10 car-shadow render-to-texture, SuperG, leaf sprites)

Written 2026-10-16 before the first measuring run. Base `2cc15b2cf` (race/first-frame-parity), branch `work/rres`, worktree `.worktrees/rres`.
Instrument: `verify/rsky_20261016/run13.py` (13 tracks x 3 frames, matched-pose replay, `drawlist_diff.py` 3D scoring) with the cite set
`verify/rskyverify_20261016/cites_merged.json`. Private game root (`original\` inside it), `MASHED_NO_DEPLOY=1`, `MASHED_PLAYERTRACE_OUT` per run.

## Arms

- **base**: clean build of `2cc15b2cf` (`log/rres/exe_base.exe`).
- **after**: this branch, each lane default ON with an opt-out, built from the same tree.

## Predictions (stated before any run)

- **Pr-0 (baseline reproduces).** The base arm gives **1047 rows, 0 RED** over the 39 frames, row for row the `RESULT_rskyverify.md` addendum
  (Egypt 67, Neustein 64, Arctic 103, Highway 58, Sands 70, SuperG 210, Roundabout 88, Storm 69, Forest 57, Dump 60, Warzone 79, City 59, Training 63), and
  G-CTRL = step `d3b90ee0`, gates `80f01f35`, ptrace `56d62ae7`. If it does not, nothing below is interpretable and the lane stops at the cause.
- **Pr-1 (C10 is a MISSING-only group).** Every row under the C10 cite (`#WxH:21` / `@shadowrt`) is a MISSING row (the original draws it, the standalone has no
  draw site). After the port no C10 MISSING row remains on a track whose shadow triangles match, and no EXTRA row under that texture appears.
- **Pr-2 (what the shadow draws are).** `FUN_004228f0` (`0x004228f0`) draws, per car, the ground triangles `FUN_00422570` (`0x00422570`) collected (up to `0x40`
  from the collision query `FUN_004b4ad0`, filtered by surface and normal), as RwIm3D triangle lists with the render-to-texture raster bound (RenderState 1),
  vertex alpha on (RS `0xc` = 1), texture address 3 (RS 2). The per-track texture size is the camera table index `b = (rec+0x298 == 2)`:
  `FUN_0041eaa0` (`0x0041eaa0`) creates `[0]` 256x256 and `[1]` 64x64 through `FUN_004b48e0`, so 64x64 on the nine tracks of r10 and 256x256 on TRAINING.
- **Pr-3 (G-CTRL unchanged).** The shadow pass only reads simulation state, so G-CTRL stays `d3b90ee0 / 80f01f35 / 56d62ae7`.
- **Pr-4 (no new rows).** Acceptance A3: no row identity absent from the base arm appears, other than rows that exist because a previously MISSING draw is now
  present but differs (they must carry an existing cite).
- **Pr-5 (SuperG).** SuperG's 210 rows are dominated by a small number of causes (ORDER / TEX1 / FOG states under existing cites); the largest single cause is
  portable. To be refined after the base arm's per-cite breakdown (section "Amendments").

## Acceptance

- A1: 0 RED on the 39 frames. A2: G-CTRL unchanged. A3: no new row identities (above). A4: every lane has an opt-out and the opt-out arm
  (7 tracks x 3 frames) equals the base arm row for row. A5: row counts per track reported against the base arm.
- A gate is not relaxed to make a row pass; a row left stays cited with its cause.

## Amendments

### Amendment 1 (before any arm of the port was measured; base arm and HUD-replay arm were measured first, see RESULT)

Measured facts that replace guesses above. Baseline arm: **1047 rows, 0 RED, G-CTRL d3b90ee0 / 80f01f35 / 56d62ae7**, gates FAIL only at capfix Neustein row 1169
(I-NAME 73/77, the known r10 miss), Pr-0 confirmed.

1. **Row inventory by cite** (39 frames): C08 234, C19 132, C05 103, C21 91, C06 78, C07 51, C03 39, C18 39, **C10 38**, C09 26, C20 24, C14 23, C13 21, C11 18, C17 16,
   C16 15, C22 14, C04 13, rest < 13. C10 is 38 rows (3-4 per track, 10 tracks), not the largest group. SuperG's 210 = C19 96 + C21 51 + C08 18 + C16 9 + C05 8 + C06 6 + C07 5 +
   C10 4 + C14 4 + C03 3 + C18 3 + C13 2 + C09 1.
2. **C08 is the harness, not the port.** `run13.py` replays camera and car poses but not the HUD state; `MASHED_HUD_REPLAY=verify/rfx_20261016/hud_inputs.txt` on the base exe
   takes the 13 tracks from 1047 to **813 rows (C08 234 -> 0), no other cite moves, gates unchanged** (`log/rres/hud13`). Not a default-build change.
3. **SuperG shares Arctic's world-sector callback.** A scan of every `E8 rel32` in `.text` finds exactly two callers of `FUN_00496d00` (`SetSectorCallback`): `0x00448951` (Arctic
   slot 0) and `0x00450f35` (SuperG slot 0 `FUN_00450f00`, which also calls `FUN_00496d20` `0x00450f43`, `FUN_00484000` `0x00450f59`). SuperG's 96 C19 rows are the sticky
   vertex-alpha flag (sectors S27..S12 draw AB 1 after the frame's screen rect `FUN_00450b10`, called at `0x004271e8`, leaves RS `0xc` = 1) and a cull leak (see 5).
4. **Car shadow render-to-texture (C10) decoded** (probe `shadowstate`, `re/frida/camera_probe.py --shadow-out`, Neustein / Training / SuperG racing samples): per car `FUN_0041f8f0`
   places the RT camera at `carpos + lightAt * -4.0` (`_DAT_005cca00`, light frame `DAT_0063d850`+4 modelling `at`), looks at the car with up (0,1,0) (`FUN_004b42c0`), parallel
   projection, view window 0.75 x 0.75, RT `256x256` A8R8G8B8 (`FUN_0041eaa0`: `[0]` 256, `[1]` 64 chosen by `rec+0x298 == 2`; 1 on all 4 cars of the three tracks), clears to
   (0,0,0,0), draws hull part `0x3b` (44 triangles, black, Z test and write off), then `FUN_00422570` collects up to `0x40` collision triangles in a 1.5 sphere around the car
   (`0.75 <= normal.y`, plane distance <= 1.0 in front or <= 0.1 behind, three material colours and the switch-table colours excluded, y += 0.01), UV = the camera `viewMatrix`
   x,y (= `0.5 - vx/1.5`, `0.5 - vy/1.5`, verified against 291 recorded vertices, error <= 2e-4 from sampling skew), colour white with alpha `floor(153 * rec+0x288 * rec+0x284)`
   (12 of 12 samples), drawn by `FUN_004228f0` after the car with RS 8 = 0, RS 12 = 1, texture address clamp. Gate: `DAT_00657490` = course+0x10638, written at the end of
   `Course::CreateFromDescription` (`0x00479330`): 1 unless Course_Id is 0, 0x1a or 0x24 (Arctic, City, Storm), the three tracks with no C10 row.
5. **SuperG's CULL rows**: the frame opens with the Reflections.dff clump drawn into a reflection RT camera (slot 4 `FUN_00451090`, `FUN_00484000(0,0x100,0x100,0x600)`) with the
   cull state the previous frame's HUD pass (`OrangeDisplay`, CULL NONE) left; not part of this lane unless time remains.

### Revised predictions

- **Pr-6 (C03 hull rows move with C10).** The 44-triangle untextured depth-off draw per car (C03 case (a)) is the hull draw of the RT pass (backtrace `0x0041faa0 <- 0x004200c6` is
  the `call [ebp+0x48]` inside `FUN_0041f8f0`); porting C10 removes those rows as well.
- **Pr-7 (SuperG sticky VA).** Extending F3 to Course_Id 25 removes SuperG's AB-type C19 rows and moves nothing on Arctic. Measured before the port (arm S1, HUD replay on):
  SuperG 192 -> 164 rows, Arctic 85 -> 85.
- **Pr-1 amended.** After the port a track's C10 and C03(a) rows are 0 where the collected triangle count and AABB match; a count mismatch leaves a MOVED / MISSING row that is cited.

### Amendment 2 (written before the final arms f0 / f1 / fo and the G-CTRL run on the final exe were measured)

Facts resolved while porting, each measured before it was coded:

6. **Cache threshold is a double.** `FUN_00422570` compares the squared move distance with `fcom qword` at `0x005cd318` = **0.125** (Ghidra prints the double as float 0.0). The collect cache
   is re-queried when the car moved more than sqrt(0.125) from the last query position. Under the matched-pose replay the standalone's pre-replay grid positions mis-anchor that chain, so
   the cache is reset when the replay row counter is 0.
7. **Alpha source.** `153.0` at `0x005cd320`, applied as `fmul` on `FUN_00422570`'s 4th stack argument (`rec+0x288 * rec+0x284`), truncated to a byte.
8. **Hull frame.** The hull frames hang off the body frame, not the clump root: root x hull misses the original's hull AABB by 4e-3 on average (up to 1.2e-2 pitched), body x hull by 2e-4
   (120 hulls, 13 tracks). The standalone bakes the hull batch into the hull parent's frame and draws it with the body LTM.
9. **RT size by LOD state.** `rec+0x298 == 2` selects the 64x64 raster (`FUN_00446520` `push 1` / `push 2`, `0x00446965` / `0x00446975`). The state is taken from the previous frame's eye
   (default). It removes the one boundary car at Neustein row 1169 (d2 148.7 previous eye / 142.9 this eye) and adds no row on 39 frames. `MASHED_CAR_LOD_EYE=last` restores this frame's eye.
   Where the director reads the camera is not decoded: U-row.
10. **UV.** The RT camera's `viewMatrix` is only re-synced at its BeginUpdate (librw `cameraSync`), after the collect; UV is computed in closed form `0.5 - (d . axis) / 1.5` from the
    camera basis, matching the original's recorded vertices (291 of 291, error <= 2e-4).

Pixel witness (not an acceptance gate, the draw list is): Neustein grid frame, original vs shadow-on vs shadow-off, 4x crop of the near car: the original's dark band under the
rear and along the right wheel arch is reproduced by shadow-on and absent from shadow-off (`verify/rres_20261016/evidence/shadow_zoom_700.png`).

Final predictions (stated before the final arms): **Pr-8** default build, no HUD replay: 1047 -> 1047 minus every C10 row (38) minus the C03 hull rows, plus 0 rows (any added row fails the
lane); SuperG 210 -> about 180 (sticky VA). **Pr-9** `--hud` arm: 813 -> about 745 (the arm `s3h13` measured 745 on the pre-UV-fix exe, same draw geometry). **Pr-10** opt-out
(`MASHED_CAR_SHADOW_RT=0 MASHED_LIBRW_SUPERG_VA=0 MASHED_CAR_LOD_EYE=last`) reproduces the 1047 baseline row for row on the 7 tracks it is run on. **Pr-11** G-CTRL stays
`d3b90ee0 / 80f01f35 / 56d62ae7` (render-only change, no sim reads).

### Amendment 3 (after the final arms of amendment 2 and the rebase onto 7f5f2ed30, before the SuperG pre-world lane was measured)

11. **SuperG's first 81 draws are `Reflections.dff`.** The recorded frame opens with 81 draws (Box, RustySteel, Snow, Cargo, LandingStageEdge, Rust, Yellow) with CULL NONE, AB 0, ahead of FUN_004270f0's screen
    rectangle (draw 81). SuperG's COURSE.LUA has no Sky_Filename (the sky array `DAT_00656f48` is empty, `FUN_00426d90` draws nothing), and `FUN_00451090` (slot 4) renders `*(course + 0x10114)`, clump 1 = `Reflections.dff`,
    between `FUN_004c1a00` / `FUN_004c19f0`. The standalone draws the same 81 draws after the world (B draws 195..287) with CULL BACK, which is the whole of SuperG's C21-ORDER (51 rows) and C19 CULL (54 rows).
    **Pr-12 (stated before the arm):** drawing clump 1 ahead of the world walk with CULL NONE removes both groups and adds no row. This lane was not in the predictions of amendment 2.
12. The tip baseline is built from 7f5f2ed30 itself (arms t0 / t1) and the final arms are measured on the rebased tree; the pre-rebase arms (base, f0, f1, fo, fo2 on 2cc15b2c) stay as evidence of the same lanes.
    **Pr-13:** default 1047 -> about 870 (C10 -38, C19 -82, C21 -51, small), HUD arm 813 -> about 635, opt-out equal to the tip baseline, G-CTRL unchanged.
