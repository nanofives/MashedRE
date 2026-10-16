# PREREG rstates: RenderWare render states and world draw order (U-9689, U-9690, U-9694)

Committed before the first measuring run. Base: `race/first-frame-parity` ec3301f3 (d1r1 merged).
Branch `work/rstates`. Evidence dir `verify/rstates_20261016/`, bulky captures under `log/rstates/`.

## 0. What is already established (static reads plus offline analysis of d1r1's committed records)

Every claim cites bytes in the anchored `MASHED.exe` (disassembly of `MASHED.exe.unpatched`).

**World-sector order and culling (U-9694).** RenderWare's camera sync hook `0x004e38a0` (installed
over camera `+0x10` by `0x004e3800`) builds the frame's sector list, which `RpWorldRender`
`0x004e5660` -> `0x004e47b0` walks in order with the per-sector callback `0x004e5680`. The walk is
an explicit-stack BSP descent from `world+0x1c`. At a plane sector (type 0/4/8 = x/y/z) it compares
the camera frustum box (`camera+0x10c` sup, `+0x118` inf) against `leftValue`/`rightValue`
(`+0x10`/`+0x14`). Both sides overlapped means the near child (eye `[c] > value`, `+4`) is processed
first and the far one pushed, when `world+0xc` != 2 (0x004e3a5e..0x004e3aa4). At an atomic sector it
tests 6 frustum planes (`camera+0x94`, stride 0x14) against the corner picked by the plane's closest
bytes from `sector+0x48` (sup) / `+0x54` (inf), and drops the sector if `dot(n,c) - d > 0` on any
plane (0x004e396b..0x004e3a2b). The box at `sector+0x48` is the **BSP cell**, not the sector's own
stream box. `FUN_004e51e0` copies the world box (`world+0x50`) and walks the tree. It narrows the
right child to `inf[type] = rightValue` (`0x004e5241`/`0x004e5259`) and the left to
`sup[type] = leftValue` (`0x004e525c`/`0x004e5262`), and stores the running box into each atomic
sector with `rep movsd` (`0x004e5219`/`0x004e5226`). The stream box goes to `+0x60`/`+0x6c`
(`0x004e9eee`..`0x004e9f18`). A Python model of this (`bsp_order.py`, `run_bsp9.py`) reproduces the
original's recorded world-sector sequence **exactly on 9/9 d1r1 frames**. With the stream box
instead of the cell box it gets 6/9 (Arctic's sector 0 is listed by the original although its own
box is behind the camera).

**Tied atomics.** `0x004e5680` draws, after each listed sector, every atomic on the sector's
`+0x38` tie list with the RENDER flag (`atomic+2 & 4`) and `atomic+0x60 != renderFrame`, whose
world bounding sphere (`0x004e6100`) is not outside the frustum (`FUN_004c1b40`,
RwCameraFrustumTestSphere: `radius < dist` on any plane -> skip), then stamps `renderFrame`. Ties
are rebuilt by the atomic sync hook `0x004e3c00`. It boxes the world sphere (center ± radius) and
walks the BSP: left if `inf < leftValue`, right if `sup > rightValue`. Each tie is inserted at the
**head** of the sector list (`0x004e3d30`..`0x004e3d48`). Clumps join the world through
`0x004e4450`, which the course loader calls only for clumps not flagged by
`Clump_Exclude_From_World` (`0x00479da6`/`0x00479db3`). The original's records show RWP_Object
props (Arctic `Tyres`) and course clumps (TRAINING `ROAD.DFF` pieces, City lamp posts) drawn
between sectors. The backtrace of Arctic `Vehicles` returns into `0x004e56cd` (the tied-atomic
call).

**Every atomic's render callback is `FUN_004721e0`.** The atomic plugin `0x26990004` (registered
by `FUN_00472380`, ctor `FUN_00472330`) installs it on creation (`FUN_004722e0`). It draws only if
`dist² × camera.viewWindow.x < (radius / sqrt(1.5544e-5))²`. Here dist runs from the camera frame
`+0x40` to the world sphere centre, radius is `atomic+0x28`, and the sqrt is RW's LUT
`FUN_004c3b30`. The ratio is cached per atomic. A LOD geometry (`ext[2]`) or fade (`ext[1]`)
branch exists; it is zero for props unless set.

**Clump atomic order.** `RpClumpAddAtomic` `FUN_004e6f80` and the stream reader's inline copy
(`0x004e7947`..`0x004e795d`) insert at the **head** of `clump+8`, and `RpClumpRender` `0x004e6680`
walks from the head. RW therefore renders a clump's atomics newest-first. librw's
`Clump::addAtomic` appends (`deps/librw/src/clump.cpp:103`).

**U-9689, the Arctic sector blend.** RW's D3D9 driver blends when texture alpha
(`DAT_007d6b14`, set per stage-0 raster in `FUN_004d6ce0`) OR vertex alpha (`DAT_007d6b10`) is
on. Only `FUN_004d7100` writes the vertex-alpha flag. Its callers are the default atomic callback
(per mesh: `material alpha != 0xff || instanced vertexAlpha`, against the live flag read by
`FUN_004d71f0` at `0x004ec437`; an unlit atomic without prelight forces 0 at `0x004ec2af`), the
car/sky pipelines (`FUN_00543500` `0x00543622` and siblings), and `RwRenderStateSet(12)`
(`FUN_004d7480` case 12 `0x004d78db`). Arctic's slot 0 (`0x00448947`/`0x00448951`/`0x00448963`,
the only callers) installs world-sector render callback `0x0049a0c0` -> mode 1 `0x0049a750`. That
callback sets textures and stage states per mesh but **never** calls `FUN_004d7100`, so every
Arctic sector inherits the flag the previous RW draw left. In the recorded frames the draw before
the first sector is the slot-3 default sky (backtrace `0x00426da2`, pipeline `0x005436da`), which
leaves the flag at 1. The first tied atomic after that sector (`Vehicles`, opaque) leaves it at 0.
That is the whole "one sector blends" pattern.

**U-9690, the cull leak.** `FUN_00499d90` sets D3D CULLMODE 1 directly through the state queue
(`FUN_004d5480(0x16,1)` at `0x0049a084`). RW's cull cache `DAT_007d6b18` is written only by its
own handler `FUN_004d7c10` (`0x004d7c52`) and device init (`0x004d6163`). In race the function is
reached from `FUN_004914b0` (`0x0049151a`), gated by `DAT_00771534`, which only the Lua command
`RainEnable` (`0x004917a0`/`0x004917a5`, name at `0x005cdb28`) sets. Arctic and Storm call
`RainEnable()` in COURSE.LUA (Arctic line 162). `FUN_004270f0` runs it after the world and the
sea, so on Arctic every car draw after it reads D3D cull 1. The other caller, `FUN_0048ad50`
(`0x0048ad97`), draws after cars and copters on TRAINING/City; only `RWObjShad` follows it there.

## 1. Fixes under test (all in render code; the sim is untouched)

- **F1** (d1r1, unchanged): RW D3D9 alpha flag per raster, ALPHAFUNC GREATER / REF 0, CULLBACK.
  Knob `MASHED_LIBRW_RWSTATES`.
- **F2 sector walk**: librw world sectors drawn in the `0x004e38a0` order with the cell-box test.
  After each sector its tied in-world atomics are drawn (tie rule `0x004e3c00`, head insertion,
  RENDER, renderFrame, sphere test, then the `FUN_004721e0` distance test). In-world: RWP_Object
  instances and Clump_Filename clumps not excluded. After the world, queue order unchanged:
  excluded clumps and the Arctic sea grid (slot 2 `0x00449030`, RpClumpRender, which has no
  frustum test). Cars unchanged. Knob `MASHED_LIBRW_SECTORWALK`.
- **F3 Arctic sticky vertex alpha**: on the track whose slot 0 installs `0x0049a0c0` (Arctic,
  Course_Id 0), sector meshes skip their own vertex-alpha set. librw's vertex-alpha state at world
  start is the slot-3 sky's last-mesh need, and tied atomics set it per mesh as librw's default
  pipeline already does. Knob `MASHED_LIBRW_STICKYVA`.
- **F4 rain cull leak**: on tracks with an active `RainEnable()`, D3D cull is set to NONE through
  the device, bypassing librw's state cache, after the post-world props and before the cars.
  Knob `MASHED_LIBRW_RAINCULL`.
- **F5 clump order** (separate decision): librw clumps built by BuildClump render their atomics in
  RW's newest-first order. Knob `MASHED_LIBRW_CLUMPORDER`.

## 2. Instrument (d1r1's, unchanged)

Original side: d1r1's 9 recorded frames (`verify/d1r1_20261016/evidence/*_orig_row_*.draws.json`,
traces `log/d1r1/orig_{training,arctic,city}/trace.csv`), re-captured only if a gate demands it.
Standalone: `verify/d1r1_20261016/run_replay3d.py` with the per-draw recorder and both pose
transplants. Differ: `re/tools/drawlist_diff.py` 3D mode, cites `verify/d1r1_20261016/cites_d1r1.json`.
Collateral: `verify/d1r1_20261016/rowdelta.py` logic (row keys unchanged).

Gates per run: I-SYNC (shot row == K, recorder armed) and I-N0 (one repeated replay byte-identical),
as in PREREG_d1r1 §2.

## 3. Rules (pre-registered decision)

The **baseline** is a fresh replay of this base (ec3301f3) build: default (`B0`) and F1 (`B1`).
A fix set is kept and wired default ON (opt-out knob) only if:
1. every row it targets is gone on every frame where it occurs. Targets: the Arctic F1 STATE rows
   (the first sector's 10 meshes, and Advantage/AdvantageSmall/Glass CULL) and the world-sector
   ORDER and EXTRA rows;
2. **no new row**: against B0, no (kind, texture[, state detail]) row appears on any of the 9
   frames that B0 does not have, except rows re-keyed by a change the fix intends (listed by
   key in RESULT with the reason). The same check runs against B1 for F1's own rows;
3. **G-CTRL** default-build fingerprint unchanged: `c2e4d60d / b50bd39b / ba90ed88`;
4. **no-op control**: with the fix's knob at 0, the draw record equals B0 byte for byte on one
   frame per track;
5. TRAINING pixel scores (`imgdiff` vs the original BMP) do not rise by more than 0.5.
Failing 1-4 -> default OFF with a recommendation. Failing only 5 -> kept, rise reported.
F1 goes default ON only together with F2+F3+F4 passing 1-4 on all 9 frames.

## 4. Predictions (from `sim_fix.py`, a rewrite of d1r1's F1 records; not evidence)

- **Pr-1** The standalone's world-sector draw subsequence equals the original's (sector id
  sequence) on 9/9 frames.
- **Pr-2** Arctic STATE rows drop from 13-14 to at most 1 per frame. The survivor is the
  `World` 68-tri Z row, sky pass, U-9696.
- **Pr-3** EXTRA rows drop on every frame (TRAINING 29->14, Arctic 18-20->13-15, City 33->17 in
  the sim, which removes culled sectors only; prop culling can remove more).
- **Pr-4** ORDER rows that survive: Glass (car part order, untouched by F2-F4), City
  Path2/Pathpattern/road2/railings/thickgirders (post-world building order, U-9695 placement),
  Arctic first-sector meshes vs the sky-pass `World` mesh (U-9696). Arctic WetRoad's Tyres
  inversions and TRAINING `Wet_Water00` go only if the tied-prop interleave is right.
- **Pr-5** G-CTRL unchanged (render only).

## 5. Amendments

Appended here with a timestamp and reason before the next affected run.

### Amendment 1 (2026-10-16, before any F6 run): F6, the after-world clump order

Reason: on base d074095d (rprops placed City's 27 TreeTrunk copies) the City ORDER rows `Path2`
(2014 inversions), `Pathpattern` (108) and `road2` (162) are against `bark`/`Grass`. The original
draws the excluded clumps from the per-track hook record's `+0x1c` slot (table `0x005f33f8`), in
code order: City `0x0044b8f0` renders course clump slots 6, 7, 8, 9, 10, **12, 11**, 13
(`0x0044b920..0x0044b97b`), so the trees (12) come before the road (11). The port queues them by
clump index. `slot2_order.py` lists every track's slot-2 order and `excluded_vs_slot2.py` shows
it is always a subset of COURSE.LUA's `Clump_Exclude_From_World` set (the whole set on TRAINING
and City).

**F6** (`MASHED_LIBRW_AFTERWORLDORDER`, `=0` opts out): the after-world props whose clump the
track's slot 2 draws are queued as one group, in slot-2 order, at the queue position of the first
of them. Every other prop keeps its place. Tracks whose slot 2 renders through an indexed loop
(Dump) or renders no course clump slot (Arctic, Egypt) are unchanged.

Rules: §3 rules 1-5 against **T2** (the final F1-F5 build on d074095d) instead of B0. Rule 1
targets: City `Pathpattern` and `road2` ORDER rows gone; `Path2` inversions fall. Rule 4: F6 `=0`
equals T2 byte for byte on 9/9 frames.

Predictions (not evidence): **Pr-6** City `Pathpattern` and `road2` rows go on 3/3 frames and
`Path2` falls to the order of the 130 it had on 0020d521, before the trees were placed.
**Pr-7** Arctic records equal T2 byte for byte (no slot-2 clump list). **Pr-8** TRAINING gains no
row (its slot 2 moves clump 9 ahead of 3..8).

### Amendment 2 (2026-10-16, before any F7 run): F7, mesh order within an atomic

Reason: on 04dee0ed with F1-F6 (arm U2) City `Path2` keeps 502 inversions, against the building
textures (`rooftop`, `shopfront04`, `Apart5windows NT`, `buildconc2`). In row 761 the original
draws each STANDARD.DFF copy as brooklyn1, shops, buildconc2, residential02 NT, shopfront02,
shopfront04, windows03 NT, Apart5windows NT, windows06 NT, rooftop, **Path2**. That is the
geometry's BinMesh PLG (chunk 0x50E) mesh order, matIndex `[0,1,2,4,5,6,7,8,9,10,3]`
(`binmesh_order.py`; BUILD04.DFF the same with Path2 last). The port draws material order
(`Track/DffModel.cpp` batch loop), so Path2 comes fourth.

**F7** (`MASHED_LIBRW_BINMESHORDER`, `=0` opts out): DffModel emits each atomic's batches in its
geometry's BinMesh mesh order, and any material without a mesh follows in material order.
World sectors are not touched: they are one librw geometry each, meshed by librw.
The F3 sky rule "last mesh of DFF atomic 0" takes the last batch of atomic 0 in batch order,
which is the highest material when F7 is off (unchanged).

Rules: §3 rules 1-5 against **U2** (F1-F6 on 04dee0ed). Rule 1 target: City `Path2` inversions
against building textures fall on 3/3 frames. Rule 3: G-CTRL unchanged. Rule 4: F7 `=0` equals
U2 byte for byte on 9/9.

Predictions (not evidence): **Pr-9** City `Path2` falls below 502 on 3/3. **Pr-10** the Glass rows
are unchanged (car glass is separate atomics, U-9707). **Pr-11** no new row on TRAINING or Arctic.

### Amendment 3 (2026-10-16, before any F8 run): F8, the copy order of an after-world Mts pair

Reason: with F1-F7 (arm V7) City `Path2` keeps 444 inversions, against the STANDARD.DFF copies'
opaque meshes. In row 761 the original draws the six STANDARD copies and the 27 TreeTrunk copies
in exactly the reverse of the port's order (copy centres, `log/rstates/V7` vs the d1r1 record).
The bytes give the reason. The course load's pair loop (`0x00479cc0..0x00479cff`) keeps the clump's own atomic
for record 0 (`je 0x00479cf7`) and, for every later record, clones it (`0x004e69a0`) and adds it
with RpClumpAddAtomic `0x004e6f80` (`0x00479ce5`), which inserts at the head. RpClumpRender
(`0x004e6680`) walks from the head, so the copies draw from the last record to record 0.

**F8** (`MASHED_LIBRW_PAIRORDER`, `=0` opts out): an after-world `Clump_Mts_Pair` prop queues its
copies from the last Mts record to the first. In-world paired clumps are not touched: their
draw order is the sector tie order (U-9709).

Rules: §3 rules 1-5 against **V7** (F1-F7). Rule 1 target: City `Path2` inversions fall on 3/3.
Rule 4: F8 `=0` equals V7 byte for byte on 9/9.

Predictions (not evidence): **Pr-12** City `Path2` falls below 444 on 3/3. **Pr-13** TRAINING and
Arctic records equal V7 byte for byte (no after-world Mts pair is drawn in their frames).
