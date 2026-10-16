# RESULT rstates: RenderWare render states and world draw order (U-9689, U-9690, U-9694)

**Verdict: kept, default ON.** Eight render-only fixes pass every pre-registered rule
(`PREREG_rstates.md` §3 and amendments 1-3) on the 9 d1r1 frames: F1 (d1r1) and F2-F8 (new).
Each fix has an opt-out knob.

**Re-measured on `race/first-frame-parity` 34ea5b71 (section 7, carlod and pathprog_a merged in): same verdict.**
Rows per frame, new-base build -> merged default build: TRAINING 72/71/71 -> 22/21/21, Arctic 73/73/74 -> 35/36/37,
City 121/119/120 -> 23/23/25. 0 RED, world-sector sequence 9/9, G-CTRL `716cb2fd / 2a2afda1 / ba90ed88` unchanged,
all-knobs-off byte-identical to the new-base build 9/9, smoke 13/13.

Final base: `race/first-frame-parity` 04dee0ed (rprops, rfx, d2posmodel, librwrest_checks and
dupinstall2 merged). Arms: U0 base, U2 F1-F6, V7 F1-F7, W8 final default F1-F8, and W0 final with
all eight knobs at 0.
- Rows per frame against the original, base -> final default build:
  - TRAINING 77/76/76 -> 28/27/27
  - Arctic 78/77/77 -> 41/41/40
  - City 126/124/122 -> 29/29/27
- 0 RED on every frame. Matched draws are unchanged (198/195/195, 145/144/105, 474/462/415).
- The standalone's world-sector draw sequence equals the original's on **9/9** frames (0/9 on the
  base).
- G-CTRL: base `716cb2fd / 2a2afda1 / ba90ed88`, final `716cb2fd / 2a2afda1 / ba90ed88` (unchanged, `gctrl_04dee0ed.txt`).
- With all eight knobs at 0 the final build is byte-identical to the base on 9/9 frames (`noop_final.txt`).
- Pixels (`imgdiff` mean, vs the original BMP):
  - TRAINING 14.24/9.98/8.66 -> 12.12/8.61/7.31
  - Arctic 9.24/7.06/6.50 -> 9.18/7.05/6.48
  - City 10.56/6.92/6.63 -> 10.55/6.88/6.61

Earlier bases, same rules, all PASS (`scores.txt`). F6-F8 came in by amendment after the d074095d
round:
- d074095d (F1-F5): 78/77/77 -> 29/28/28, 78/77/77 -> 43/43/42, 128/126/124 -> 33/33/31.
- 0020d521: 77/76/76 -> 30/29/29, 76/76/76 -> 44/43/41, 153/151/149 -> 57/57/55.
- ec3301f3: 86/85/85 -> 39/38/38, 81/81/81 -> 49/48/46, 157/155/152 -> 61/61/58.
- G-CTRL was unchanged on each.

PREREG committed before any measuring run. Amendments 1-3 (F6, F7, F8) were each committed before
that fix's first run.

## 0. The rebase (what had to change)

- **librwrest** draws cars and copters with per-frame poses, env passes, per-model draw order and
  RW states. The merged loop sent every instance to the walk's per-DFF-atomic path, cars
  included. Now only plain copies take that path: not a car, no poses, no env, no model draw
  order, no per-model states. Cars keep the FUN_00420870 path and copters FUN_00425e40's order
  and cull. Copters are queued `kInstAfterWorld`, cars `kInstCar` (`AddInstancePosed` gained
  the `kind` argument).
- **rtex** P9 renders DUAL sectors and props through `matfxRenderCB_Shader`. The empty-mesh
  skip is applied there too. Arctic and City worlds carry no DUAL material (rtex census), so
  F3's sector handling is unaffected.
- **d1r3** deleted the legacy D3D9 renderer: the prop/car queue sites and the track-load hook
  were re-applied to the librw-only code.
- **rprops** (on d074095d) draws the sky through its own librw pass `g_sky` and places camera-men,
  the Arctic ships and every `Clump_Mts_Pair` copy. F3's sticky flag is set before that sky pass.
  The copies are queued with their clump's kind, so they are tied into sectors when the clump is
  in the world. The original does the same, for a reason a call search misses: section 1, "Clones".
- **rfx** (on 04dee0ed) queues a car-FX marker (shadow, beam) after the cars. It is handled first
  in the instance loop, before the walk's kind checks. rfx left 6 Arctic CULL STATE rows on the
  shadow and beam cited to U-9690. Under F4 they are drawn with the leaked cull NONE (section 3).
- **U-ids** are renumbered at each rebase (d1r3 took U-9698, rprops U-9699..U-9702, rfx
  U-9703..U-9705). This lane's rows are **U-9709** (tie order), **U-9707** (car glass order) and
  **U-9708** (after-world clumps no slot-2 list draws, F6).

## 1. What the original does (cited to bytes; extracts in `evidence/dis_*.txt`)

**World sectors.** The camera sync hook `0x004e38a0`, installed over camera `+0x10` by
`0x004e3800`, rebuilds the frame's sector list. `RpWorldRender` `0x004e5660` -> `0x004e47b0`
walks that list in order.
- The walk is an explicit-stack BSP descent from `world+0x1c`.
- At a plane it tests the frustum box (`camera+0x10c`/`+0x118`) against `leftValue`/`rightValue`.
  When the frustum reaches both sides it takes the camera's side first and pushes the other
  (front-to-back, `0x004e3a5e..0x004e3aa4`).
- At an atomic sector it tests six planes (`camera+0x94`) at the closest corner of the box at
  `sector+0x48`/`+0x54`.
- **That box is the BSP cell, not the sector's own box.** `FUN_004e51e0` copies the world box,
  narrows `inf[type] = rightValue` / `sup[type] = leftValue` down the tree, and `rep movsd`s it
  into every atomic sector (`0x004e5219..0x004e5262`). The stream box goes to `+0x60`/`+0x6c`.
- The Python model of this (`bsp_order.py`) reproduces the original's recorded sector sequence
  on 9/9 frames with the cell box and on 6/9 with the stream box (`bsp9.txt`). Arctic's sector 0
  is listed although its own box is behind the camera.

**Tied atomics.** After each listed sector, `0x004e5680` draws every atomic on the sector's `+0x38`
tie list that is RENDER-flagged and has `renderFrame != now`.
- The atomic is stamped whether or not it passes.
- It is drawn if `FUN_004c1b40` (sphere vs planes, `radius < dist` = outside) does not reject it.
  The draw goes through its render callback `FUN_004721e0`, which the atomic plugin ctor
  `FUN_00472330` (`FUN_00472380`, id `0x26990004`) installs on every atomic. That callback draws
  only if `dist² × viewWindow.x < (radius / LUTsqrt(1.5544e-5))²`.
- Ties come from the atomic sync hook `0x004e3c00`. It boxes the world sphere (`0x004e6100`),
  descends left if `inf < leftValue` and right if `sup > rightValue`, and inserts at the head
  of each sector list (`0x004e3d30..0x004e3d48`).
- Clumps join the world through `0x004e4450`, only when not `Clump_Exclude_From_World`
  (`0x00479da6`).

**Clones.** A clone of an in-world clump is in the world.
- The world plugin registers a clump extension (`0x004e34c6..0x004e34e3`, `call 0x004e7d70`,
  offset `DAT_007d7174`) whose copy callback `0x004e3e70` reads the source's world and calls
  `0x004e4450(world, clone)` at `0x004e3e88`.
- The atomic extension (`DAT_007d7170`) has a no-op copy (`0x004d7ff0`).
- The camera-man clones (TRAINING `0x004511c0`, Arctic `0x00448820`, RpClumpClone `0x004e6ab0`)
  are made in slot 0. Slot 0 runs after the course load: `0x00426f3c` calls `0x00479330`
  (registration `0x00479db3`) before `0x0042709f` reaches slot 0 through `0x0041e8b0`.
- The teardowns remove the clones from the world (`0x00451539` -> `0x004e45b0`).
- So every copy takes its base clump's world membership, as the port's per-prop kind does.
- On the 9 frames the visible camera-men are the base clumps (camera table row 1: TRAINING
  `(2, 0.625, -26.125)`, Arctic `(-27.513, 1.004, 43.407)`). The draw records cannot witness
  the clones, only the bytes.
- Extracts: `evidence/dis_clump_world_copy_cb_004e3e70.txt`,
  `evidence/dis_slot0_after_course_load_0042709f.txt`.

**After-world order (F6).** Excluded clumps are not in the world, so they are drawn by course
code.
- The per-track hook record (`0x005f33f8`, stride `0x48`, Course_Id at `+0x10`) has an
  after-world slot at `+0x1c`. It calls RpClumpRender `0x004e6680` on course clump slots
  `[course + 0x10110 + 4*i]`, in its own order.
- City `0x0044b8f0` draws 6, 7, 8, 9, 10, **12, 11**, 13 (`0x0044b920..0x0044b97b`): the trees
  before the road. TRAINING `0x00451460` draws 9, 3, 4, 5, 6, 7, 8.
- `slot2_order.py` reads all 13 records, giving `slot2_order.json`. Dump loops over slots
  (`0x00449583`), and Arctic and Egypt draw no course clump slot there.
- `excluded_vs_slot2.py` shows each list is a subset of COURSE.LUA's
  `Clump_Exclude_From_World` set, and the whole set on TRAINING and City. Where the rest are
  drawn is U-9708.

**Mesh order (F7).** RenderWare draws an atomic's meshes in its geometry's BinMesh PLG order
(chunk `0x50E`), which the stream stores and which need not be material order.
- City STANDARD.DFF lists matIndex `[0,1,2,4,5,6,7,8,9,10,3]`, so the blended `Path2` (3) comes
  last. BUILD04.DFF also ends on `Path2` (`binmesh_order.py`).
- The original's row-761 record draws each copy in exactly that order. The port drew material
  order.

**Mts pair copies (F8).** The course load's pair loop (`0x00479cc0..0x00479cff`) works per record:
- record 0 keeps the clump's own atomic (`je 0x00479cf7`);
- every later record is cloned (`0x004e69a0`) and added with RpClumpAddAtomic (`0x00479ce5` ->
  `0x004e6f80`, head insertion), and its new frame takes the record (`0x004c1480`).
- So RpClumpRender draws the copies from the last record to record 0. In row 761 the six STANDARD
  copies and the 27 TreeTrunk copies are in exactly the reverse of the port's order (copy centres).
- Extract: `evidence/dis_mts_pair_clone_loop_00479cc0.txt`.

**Clump order.** `RpClumpAddAtomic` `FUN_004e6f80`, and the stream reader's inline copy at
`0x004e7947`, insert at the head. `RpClumpRender` `0x004e6680` walks from the head, so RW draws
a clump's atomics newest-first. librw appends (`clump.cpp:103`).

**U-9689, the Arctic sector blend.**
- RW blends on texture alpha `DAT_007d6b14` (`FUN_004d6ce0`) OR the vertex-alpha flag
  `DAT_007d6b10`. Only `FUN_004d7100` writes that flag.
- Its callers:
  - the default atomic callback, per mesh: `material alpha != 0xff || instanced vertexAlpha`,
    against the live flag (`0x004ec437`; forced 0 for unlit unprelit geometry at `0x004ec2af`);
  - `FUN_00543500` (`0x00543622`) and the other car/sky pipelines;
  - `RwRenderStateSet(12)` (`FUN_004d7480` case 12, `0x004d78db`).
- Arctic's slot 0 installs world-sector callback `0x0049a0c0`. Its only callers are `0x00448947`,
  `0x00448951` and `0x00448963`, so the callback is Arctic's alone. Mode 1 is `0x0049a750`. It
  sets textures and stage states per mesh and never touches the flag.
- The sky pass `FUN_004492b0` (slot 4) ends with `RwRenderStateSet(12, 0)`. The slot-3 default sky
  `FUN_00426d90` then draws `sky` (backtrace return `0x00426da2`, pipeline `0x005436da`) and leaves
  the flag at 1. So the first listed sector blends.
- The first tied opaque atomic after it (`Vehicles`, returning into `0x004e56cd`) leaves 0.

**U-9690, the cull leak.**
- `FUN_00499d90` writes D3D CULLMODE 1 through the state queue (`0x0049a084`). RW's cull cache
  `DAT_007d6b18` is written only by `FUN_004d7c10` and device init.
- In race the function is reached from `FUN_004914b0` (`0x0049151a`), gated by `DAT_00771534`.
  Only Lua `RainEnable` (`0x004917a0`/`0x004917a5`, name `0x005cdb28`) sets that gate. Arctic
  (COURSE.LUA line 162) and Storm call it.
- `FUN_004270f0` runs it after the world and the sea, so every Arctic car draw reads cull 1.
- The other caller, `FUN_0048ad50`, draws after cars and copters on every track. Only `RWObjShad`
  follows it, and the standalone does not draw that.

## 2. The fixes (librw default build, `LibRw/RwRaceSubmit.cpp` unless noted)

| fix | knob (`=0` opts out) | what |
|---|---|---|
| F1 | `MASHED_LIBRW_RWSTATES` | d1r1's raster alpha flag + ALPHAFUNC GREATER / REF 0 / CULLBACK, now default ON |
| F2 | `MASHED_LIBRW_SECTORWALK` | sectors in the `0x004e38a0` order over cell boxes (`Track/TrackWorld` now keeps the plane tree); in-world props (RWP_Object, non-excluded Clump_Filename) tied per DFF atomic (`Track/DffModel` keeps the stream sphere) and drawn after their first listed sector with the sphere and distance tests; after-world clumps (excluded, Arctic sea grid) keep queue order with the distance test |
| F3 | `MASHED_LIBRW_STICKYVA` | on Arctic (Course_Id 0) sector meshes keep the vertex-alpha flag; the world starts with the sky's last-mesh need; librw skips empty material meshes (`deps/librw/src/d3d/d3d9render.cpp`, `MASHED_PATCHES.md`) |
| F4 | `MASHED_LIBRW_RAINCULL` | on `RainEnable()` tracks D3D cull NONE is written to the device, past librw's cache, before the cars |
| F5 | `MASHED_LIBRW_CLUMPORDER` | non-car clumps draw their DFF atomics newest-first |
| F6 | `MASHED_LIBRW_AFTERWORLDORDER` | the after-world props a track's slot 2 draws are queued in slot-2 order, as one group where the first of them sits (`D3d9Render/TrackRenderer.cpp` BuildPropOrder, PREREG amendment 1) |
| F7 | `MASHED_LIBRW_BINMESHORDER` | DFF batches (one librw atomic each) in the geometry's BinMesh PLG mesh order instead of material order (`Track/DffModel.cpp`, PREREG amendment 2); world sectors untouched |
| F8 | `MASHED_LIBRW_PAIRORDER` | an after-world `Clump_Mts_Pair` queues its copies from the last Mts record to the first (`D3d9Render/TrackRenderer.cpp`, PREREG amendment 3) |

`MASHED_LIBRW_WALKLOG=<file>` (diagnostic) logs each frame's sector list and tied atomics.

## 3. Rules (`PREREG_rstates.md` §3, amendments 1-3)

| rule | result (04dee0ed) |
|---|---|
| 1 targets gone | PASS. Base U0 -> final W8 on 3/3 frames of each track. Arctic: the first sector's STATE rows and the car CULL rows are gone (STATE 31 -> 3). The world-sector EXTRA rows are gone (TRAINING 28 -> 6, Arctic 16 -> 7, City 30 -> 5). ORDER inversions (frame 1 of each track): Arctic first-sector meshes 27 -> 1 each, WetRoad 106 -> 4. TRAINING `Wet_Water00` 234 -> 0. City `Path2` 3977 -> 114, `Pathpattern` 240 -> 0, `road2` 354 -> 0, `railings` 281 -> 75, `thickgirders` 100 -> 14. Per amendment: F6 (U5 -> U2) `Pathpattern` 108 -> 0, `road2` 162 -> 0, `Path2` 2014 -> 502. F7 (U2 -> V7) `Path2` 502 -> 444, `railings` 79 -> 75. F8 (V7 -> W8) `Path2` 444 -> 114. |
| 2 no new row | PASS. U0 -> W8 has 15 new rows (`rowdelta_U0_W8.txt`), all cited STATE re-keys of sky draws whose only remaining differences are depth encoding or cull. TRAINING `Sky` (Z/ZF, C20) x3. Arctic `sky` x2 (Z/ZF, and the first one CULL, C20) and `World` (Z/ZW/ZF, C15c, U-9696), each x3. City `sKYLINE` (Z/ZF, C19) x3. F6, F7 and F8 each add 0 rows against the arm before them (`rowdelta_U5_U2.txt`, `rowdelta_U2_V7.txt`, `rowdelta_V7_W8.txt`). RED 0 on 9/9. |
| 3 G-CTRL | PASS: base 04dee0ed and final default build both `716cb2fd / 2a2afda1 / ba90ed88` (`gctrl_04dee0ed.txt`). The same on d074095d (base and F1-F5), 0020d521 (`gctrl_rebased.txt`), and ec3301f3 `c2e4d60d / b50bd39b / ba90ed88` (`gctrl.txt`). |
| 4 no-op control | PASS: all eight knobs `=0` on the final build == U0, byte for byte, 9/9 (`noop_final.txt`). F8 `=0` == V7 9/9 (`noop_F8.txt`). F7 `=0` on the F1-F7 build == U2 9/9 (`noop_F7.txt`). On d074095d all five F1-F5 knobs `=0` == base 9/9 (`noop_round3.txt`), and on 04dee0ed all six F1-F6 knobs `=0` == base 9/9 (`noop_round4.txt`). |
| 5 pixels | PASS: no frame rises. TRAINING 14.24/9.98/8.66 -> 12.12/8.61/7.31, Arctic 9.24/7.06/6.50 -> 9.18/7.05/6.48, City 10.56/6.92/6.63 -> 10.55/6.88/6.61 (`pixels.txt`). |

Predictions:
- Pr-1 to Pr-5 (the original registration) are confirmed on every base. Pr-1 is the sector
  sequence, 9/9 on the final build (`seqcheck_W8.txt`). For Pr-3, more EXTRA rows went than the
  sim's sector-only estimate, because prop culling also drops off-screen props.
- Pr-6 is partly confirmed. City `Pathpattern` and `road2` went, but `Path2` fell only to 502, not
  to about 130, until F7 and F8.
- Pr-7 is confirmed: Arctic is byte-identical with F6 on and off (`bytecompare_U5_U2.txt`).
- Pr-8 is confirmed.
- Pr-9 to Pr-13 are confirmed: TRAINING and Arctic are byte-identical across F7 and F8.

rfx's 6 Arctic CULL rows (`car_shadow`, `Beam`; their RESULT cites them to U-9690) are on U0 and gone
on U2/W8. The shadow and beam draw after the cars, so under F4 they read the leaked cull NONE, as
the original's do.

## 4. What is left, and why

- **Glass ORDER** (88 inversions per frame, all tracks): the original draws each car's opaque parts,
  then each Glass part with its CarGloss pass. U-9707.
- **Arctic `World` vs sea (77-82) and the one inversion each on the first-sector meshes**: the
  sky-pass `windows.dff` mesh (`FUN_004492b0`, return `0x004493b0`) is drawn after the world by the
  port. U-9696 (sky pass).
- **City `Path2` 114, `railings` 66-75, `thickgirders` 14**: tied atomics within a sector, against
  `buildconc2`, `Camstand`, lamp posts. The original's tie order comes from RW's frame sync, which
  is not decoded (U-9709). TRAINING sector 22: the original draws ROAD.DFF atomics 0, 17, 10, 16,
  20, the port 0, 10, 16, 17, 20.
- **Arctic's first `sky` draw has CULL NONE in the original** and CW in the port (cited C20). The
  sky clump `+0x100f0` renders at `0x0044938b`, before `RwRenderStateSet(0x14, 2)` at
  `0x004493a1`, so it inherits the previous frame's last cull. That is the sky pass's lane.
- **Excluded clumps outside a slot-2 list** (eleven tracks) keep clump order. U-9708.
- Storm runs F4 (`RainEnable`), but no original capture of Storm exists.

## 5. Smoke

`smoke_tracks.py` (final default F1-F8 on 04dee0ed, 900 deterministic frames on every track piz): **13/13 exit 0**, with the sector walk logged on all 899 rendered frames of each track (`smoke_final.txt`). Earlier builds also passed 13/13 (`smoke_rebased.txt`, `smoke.txt`).

## 6. Reproduce

```
py -3.12 verify\rstates_20261016\run_bsp9.py [cell|stream]
py -3.12 verify\rstates_20261016\run9.py <exe> <arm> [KNOB=0 ...]
py -3.12 verify\rstates_20261016\rowdelta2.py B0 <arm> ; seqcheck.py <arm> ; noop_check.py B0 <arm>
py -3.12 verify\rstates_20261016\run_gctrl.py <exe> <name>
py -3.12 verify\rstates_20261016\slot2_order.py --json slot2_order.json ; excluded_vs_slot2.py
py -3.12 verify\rstates_20261016\binmesh_order.py City [texture filter]
```

Arms: `log/rstates/<arm>/` (not committed). The final arm's diffs are `evidence/<track>_W8_diff_<K>.json`.

## 7. Rebase onto race/first-frame-parity 34ea5b71 (carlod, pathprog_a)

work/rstates b2592dbd was merged with 34ea5b71 (merge base 05e8c4f0). `work/carlod` (car part visibility
`FUN_00420870`, glass pass `FUN_00420050`, env gloss default ON, env coefficient from the previous row's
camera) and `work/pathprog_a` landed after this lane's base. Four files conflicted; each kept both sides.

- **Queue API** (`LibRw/RwRaceSubmit.h/.cpp`): `RaceSubmit_AddInstance(handle, m44, kind = kInstAfterWorld,
  vis = nullptr)` and `RaceSubmit_AddInstancePosed(..., env_m44, kind = kInstCar, vis = nullptr)`. `Inst` holds
  `kind` (rstates) and `vis`, `vis_state`, `vis_flags` (carlod). Call sites in `D3d9Render/TrackRenderer.cpp`: the
  player and AI-slot 0..3 submit lambda passes `kInstCar, pv`; AI slots 4+ pass `kInstCar, carvis(...)`; the copters
  keep `kInstAfterWorld` with no vis; props pass `p.rw_kind`.
- **Render loop** merged without a manual edit: a car copy is not a `plain` copy, so it keeps the car path, where
  carlod's per-part RENDER bits and glass pass run. Plain copies take rstates's per-atomic path. The two
  features do not share a branch.
- **TrackRenderer.cpp**: carlod's `eye_prev_` env coefficient and `carvis()` lines and rstates's `gloss_frame_`,
  car-fx marker and F8 pair order are all present.
- **UNCERTAINTIES.md**: U-9694 (RESOLVED, this lane) and U-9693 (carlod's text, which supersedes rtex's) kept;
  carlod's U-9706 (visibility inputs) kept. This lane's tie-order row was U-9706 and is now **U-9709**
  (also renamed in PREREG, section 0 and section 4 here, the `cites_rstates.json` note, the `RwRaceSubmit.cpp`
  comment and CHANGELOG). U-9707 (car glass order) and U-9708 (after-world clumps) keep their ids. The U-9694 row's
  residual list reads U-9709 (tie order), U-9707 (car glass).
- Both targets build (`MASHED_NO_DEPLOY=1`, nothing written to `original\`): `Build OK`, rva-lint 0 NEW (56
  allowlisted), asm-lint OK.

Arms (`log/rstates/<arm>/`, not committed):
- **Z0**: 34ea5b71 built clean from `git archive` (no rstates code).
- **Z9**: the merged build, default (F1-F8 on).
- **Z1**: the merged build with the eight `MASHED_LIBRW_*` knobs at 0.

Rows per frame (`scores_Z.txt`, `drawlist_diff.py` 3D mode, `cites_rstates.json`):

| track | Z0 new base | Z9 merged default | matched draws (Z0 = Z9) |
|---|---|---|---|
| TRAINING 884/924/959 | 72/71/71 | 22/21/21 | 206/203/201 |
| Arctic 1051/1091/1126 | 73/73/74 | 35/36/37 | 153/149/104 |
| City 761/801/836 | 121/119/120 | 23/23/25 | 482/466/413 |

The same frames on 04dee0ed gave 77/76/76, 78/77/77, 126/124/122 (base) and 28/27/27, 41/41/40, 29/29/27 (final). The
drop in both columns comes from carlod (parts, glass, env), not from this lane. 0 RED on all 9 frames in all three arms.

| rule | result (34ea5b71) |
|---|---|
| 1 targets gone | PASS. Z0 -> Z9 on 3/3 frames of each track. World-sector EXTRA rows: TRAINING 25 -> 3, Arctic 13 -> 4, City 27 -> 2. Arctic STATE 31-32 -> 3. ORDER inversions (frame 1 of each track): TRAINING `Wet_Water00` 234 -> 0; Arctic first-sector meshes 27 -> 1 each, `WetRoad` 106 -> 4, `World` 108 -> 82; City `Path2` 3977 -> 114, `Pathpattern` 240 -> 0, `road2` 354 -> 0, `railings` 281 -> 75, `thickgirders` 100 -> 14. |
| 2 no new row | PASS. Z0 -> Z9 has 15 new rows (`rowdelta_Z0_Z9.txt`): TRAINING `Sky` x3, Arctic `sky` x2 and `World` (C15c, U-9696) x3 each, City `sKYLINE` x3. These are the same cited STATE re-keys of sky draws as on 04dee0ed. RED 0 on 9/9. |
| 3 G-CTRL | PASS. Z0 build and merged default both `716cb2fd / 2a2afda1 / ba90ed88` (`gctrl_34ea5b71.txt`). |
| 4 no-op control | PASS. Z1 (eight knobs at 0) == Z0, byte for byte, 9/9 (`noop_Z.txt`). |
| 5 pixels | PASS. No frame rises (`pixels.txt`): TRAINING 14.15/9.97/8.65 -> 12.04/8.60/7.30, Arctic 9.27/7.03/6.48 -> 9.22/7.02/6.46, City 10.44/6.87/6.59 -> 10.44/6.83/6.57. |
| Pr-1 sector sequence | PASS: 9/9 frames equal the original's (`seqcheck_Z9.txt`). |
| Smoke | 13/13 exit 0, sector walk logged on all 899 rendered frames of each track piz (`smoke_Z9.txt`). |

Notes for the merge:
- **U-9707 (car glass order) is closed in the data by carlod's glass pass.** No Glass ORDER row exists on any of the
  9 frames in Z0 (new base without this lane) or Z9; on 04dee0ed it was 88 inversions per frame. The row in
  `UNCERTAINTIES.md` is still open as written; closing it is a `re-classify` edit, not done in this leg.
- The residuals in section 4 otherwise stand with the new counts: Arctic `World` vs sea 82, City `Path2` 114,
  `railings` 75, `thickgirders` 14, Arctic first `sky` draw (cited C20), eleven tracks' excluded clumps outside a
  slot-2 list (U-9708).

Reproduce:

```
git archive 34ea5b71 -- . ':(exclude)verify' ':(exclude)log' ':(exclude)original' ':(exclude)re/analysis' | tar -x -C <scratch>
MASHED_NO_DEPLOY=1 <scratch>\mashedmod\build.bat          # Z0 exe -> log\rstates\base6\
py -3.12 verify\rstates_20261016\run9.py <exe> Z9|Z0 ; run9.py <exe> Z1 MASHED_LIBRW_{RWSTATES,SECTORWALK,STICKYVA,RAINCULL,CLUMPORDER,AFTERWORLDORDER,BINMESHORDER,PAIRORDER}=0
py -3.12 verify\rstates_20261016\noop_check.py Z0 Z1 ; rowdelta2.py Z0 Z9 ; seqcheck.py Z9
py -3.12 verify\rstates_20261016\run_gctrl.py <exe> <name> ; smoke_tracks.py <exe> <out_dir>
```
