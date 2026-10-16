# RESULT rstates: RenderWare render states and world draw order (U-9689, U-9690, U-9694)

**Verdict: kept, default ON.** Five render-only fixes, F1 (d1r1) and F2-F5 (new), pass every
pre-registered rule (`PREREG_rstates.md` §3) on the 9 d1r1 frames.

- Rows per frame against the original, base -> final default build:
  - TRAINING 86/85/85 -> 39/38/38
  - Arctic 81/81/81 -> 49/48/46
  - City 157/155/152 -> 61/61/58
- 0 RED on every frame. Matched draws are unchanged.
- The standalone's world-sector draw sequence equals the original's on **9/9** frames (0/9 before).
- G-CTRL is unchanged: `c2e4d60d / b50bd39b / ba90ed88`.
- With all five knobs at 0 the build is byte-identical to the base on 9/9 frames.

Base `race/first-frame-parity` ec3301f3. PREREG committed before any measuring run (d0cb7014).
Implementation is 356adf9e.

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

`MASHED_LIBRW_WALKLOG=<file>` (diagnostic) logs each frame's sector list and tied atomics.

## 3. Rules (`PREREG_rstates.md` §3)

| rule | result |
|---|---|
| 1 targets gone | PASS. Arctic: the first sector's 10 STATE rows and the Advantage/AdvantageSmall/Glass CULL rows are gone on 3/3 frames (STATE 13-14 -> 1). World-sector EXTRA rows are gone (EXTRA 29->8, 18-20->11, 33->8). World-sector ORDER inversions are gone: the surviving ORDER rows invert only against causes outside F2-F5 (§4). |
| 2 no new row | PASS with 9 listed re-keys (`rowdelta_B0_D2.txt`). Six are C13 MOVED -> MISSING: CamManHot on TRAINING x3, Arctic CamManCold x1, Vehicles x2. The standalone places these props at the world origin (U-9695), and the sphere test now culls them there; before, it drew them off-screen at the origin. Three are the Arctic `World` STATE row, which differs only in Z/ZW/ZF since F1: `windows.dff` is drawn depth-off in the sky pass, cited C15c, U-9696. RED 0 on 9/9. |
| 3 G-CTRL | PASS: base, opt-in build, final default build all `c2e4d60d / b50bd39b / ba90ed88` (`gctrl.txt`) |
| 4 no-op control | PASS: knobs unset on the opt-in build == base, byte for byte, 9/9. All five knobs `=0` on the final build == base, byte for byte, 9/9. The final default == the opt-in build with all knobs on, 9/9 (`noop.txt`). |
| 5 pixels | PASS: TRAINING 28.52/21.30/15.76 -> 28.33/21.18/15.42. Arctic 10.97/7.07/6.43 -> 10.93/7.05/6.42. City 16.91/13.15/12.10 -> 16.92/13.10/12.11. |

Per-frame counts for every arm are in `scores.txt`: B0 base, B1 F1, S1 F1-F4, S5 F1-F5, D2 final
default, O2 final with all knobs at 0. F5 removes City's `Pathpattern` and `road2` ORDER rows and
cuts `Path2` from 1966 to 130 inversions. It adds nothing on any frame (`rowdelta_S1_S5.txt`).

## 4. What is left, and why

- **Glass ORDER** (all tracks, 88 inversions per frame): the original draws each car's opaque
  parts, then each Glass part with its additive CarGloss pass. The standalone's car keeps stream
  order, and F5 is not applied to cars. U-9704 (new).
- **Arctic first-sector meshes and WetRoad vs `World`; `World` vs sea**: the sky-pass
  `windows.dff` mesh is drawn after the world by the port. U-9696 (sky pass).
- **City `Path2` 130, `railings` 79, `thickgirders` 14**: the building copies the original places
  per instance (U-9695), and the order of tied atomics within a sector (U-9703, new). TRAINING
  sector 22: the original draws ROAD.DFF atomics 0, 17, 10, 16, 20, the port 0, 10, 16, 17, 20.
  The frame-sync order behind tie creation is not decoded.
- Storm runs F4 (RainEnable), but no original capture of Storm exists. The 13-track smoke boots
  and races every track (§5).

## 5. Predictions and smoke

Pr-1 confirmed (9/9). Pr-2 confirmed. Pr-3 confirmed, with more EXTRA rows gone than the sim's
sector-only estimate, because prop culling also drops off-screen props. Pr-4 confirmed: Glass, City
buildings and the Arctic sky-pass mesh survive, and TRAINING `Wet_Water00` and Arctic WetRoad's
Tyres inversions are gone, so the tie interleave is right on those frames. Pr-5 confirmed.
`smoke_tracks.py` (final default, 900 deterministic frames, every track piz): **13/13 exit 0**, the sector walk logged on all 899 rendered frames of each (`smoke.txt`).

## 6. Reproduce

```
py -3.12 verify\rstates_20261016\run_bsp9.py [cell|stream]
py -3.12 verify\rstates_20261016\run9.py <exe> <arm> [KNOB=0 ...]
py -3.12 verify\rstates_20261016\rowdelta2.py B0 <arm> ; seqcheck.py <arm> ; noop_check.py B0 <arm>
py -3.12 verify\rstates_20261016\run_gctrl.py <exe> <name>
```
