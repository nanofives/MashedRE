# RESULT RW-SIZING: what each RW option costs for the top-down route (L-123 input)

Branch `orch/RW-SIZING` from orch/integration `ee3562e27`. Text and scripts only: no `mashedmod/` edit, no tracker
edit, no build, no game run. Inputs: hooks.csv, `verify/porttop_20261108/BINDINGS.tsv`, `re/analysis/match_ledger.tsv`
at `ee3562e27`; `re/coverage/reach.tsv` (the original's INT3 census, 2026-10-11, majority of 3 runs);
`original/MASHED.exe.unpatched` (anchor SHA-256 BDCAE093...); `mashedmod/deps/librw` (the vendored librw).

Scripts (this folder, all static, all re-runnable):

| script | output | what |
|---|---|---|
| `rw_census.py` | `RW_FUNCS.tsv` | every RW-band function: component, executed phase, RE / match state, COM vcalls, d3dx callees |
| `rw_surface.py` | `RW_SURFACE.tsv`, `RW_BIND_CLASS.tsv`, `RW_INLINE_READERS.tsv` | the option (A) binding surface, the binding bodies, the inline-offset readers |
| `rw_layout.py` | `RW_LAYOUT.tsv` | RW 3.x against librw, field by field, from two compiled probes |
| `origtop_size.py --list fam:rwcore` / `fam:rt2d` (PORT-ORIGTOP-SIZE's tool) | `RWCORE_TO_BIND.tsv` | the RW core / Rt2d rows to bind in reach |
| (inline, below) | `RW_EXPANDED_ON_ROUTE.tsv` | RW-band functions the route runs as matched game code |
| `probe_rw3.*`, `probe_librw.*` | `layout_rw3.txt`, `layout_librw.txt` | MSVC 19.44 `/d1reportAllClassLayout` dumps |
| `d3dx_link_test.*` | (stdout) | the Summer 2003 `d3dx9.lib` linked into a VS2022 /MT exe |

**RW band** (`rw_census.py` docstring): by address, as the 2026-10-11 relabel ruling decides the library side
(`re/analysis/library_relabel_2026-10-11.md`, "Method"):
- `[0x004c0700, 0x004ec760)`
- non-d3dx / non-libpng rows in `[0x004ec760, 0x00516000)`
- `[0x0052df70, 0x00558c50)`
- RW-Physics `[0x00558c50, 0x0057c5b0)`

That is 1,550 functions:
- 547 carry the `renderware` label. The hooks.csv label count is 550 rows, 322 C4 (the brief's number, confirmed).
- 289 are `RenderWare-Physics-3.7`.
- 390 are `render`.
- 95 are other game subsystems (hud 37, util 18, unknown 10, frontend 10, save 5, vehicle 4, track / audio /
  gameplay 3 each, boot 2). These are the Rt2d fonts, RtFS, RtQuat / RtSpline.
- 4 carry another library's label (zlib 2, lua 1, qhull 1).
- 225 have no row.

Components are a keyword + band heuristic (estimate). `d3d9` is measured from the bytes: a COM vcall
`global -> object -> vtable -> call [vtbl+disp]` on a 0x007d41xx global, a reference to the device pointer 0x007d4110
(hooks.csv: "IDirect3DDevice9* 007d4110"), or the Direct3DCreate9 import.

## The table

Numbers are counts of functions unless marked B (bytes) or lines. "est." = an estimate (heuristic, method stated in the
row's source). RW-Physics is the same under all three options (L-98: ported like game code), so it is one row at the end
and not part of any column's difference.

| item | (A) bindings to librw | (B) librw fork with RW 3.x layouts | (C) matched RW core on d3d9 |
|---|---|---|---|
| RW functions the original executes (menu + race_load + race_run), RW-Physics excluded | 668 (177,926 B), `RW_FUNCS.tsv` | same 668 | same 668 |
| what the option must supply for them | a librw call per RW API that game code reaches | same as A, with layout-bound bodies turned into forwards | the RW function itself |
| already done | 81 `MashedRw_*` bodies in `LibRw/RwApiBind.cpp` (`RW_BIND_CLASS.tsv`); BINDINGS kinds bound 63 / rw 27 / shim 22 / port 45 (all families) | the 81 bodies, plus the vendored librw (54,524 lines) | 446 of the 668 are C4 by match (45,955 B) |
| left to do on the executed path | 70 RW core + Rt2d rows to bind in reach, 52 of them executed (`RWCORE_TO_BIND.tsv`; reproduces SIZE.tsv's 64 `fam:rwcore` + 6 `fam:rt2d`) | the same 70, plus the fork work below | 222 unmatched, 131,971 B: 14 at >= 95% (6,957 B), 22 at 90-95%, 27 at 80-90%, 73 at 50-80%, 83 below 50% (77,165 B), 2 never attempted, 1 C4 by another route |
| static-only part (no census row, in the static closure below executed RW code) | n/a: the route stops at the binding | n/a | 220 functions (36,498 B), 218 of them with no hooks.csv row. Executed or not is not measured |
| bindings that needed a hand body (est., rules in `rw_surface.py` `bind_classes`) | 42 forward, 21 hand-layout (uses an RW-layout record type or the raw RwGlobals pointer), 10 hand-semantics, 8 shim | the 21 hand-layout ones become forwards once the moved fields match. The 10 hand-semantics ones stay | all 81 retire |
| hidden part: matched game functions in reach that read RW state inline (`RW_INLINE_READERS.tsv`) | 180 functions, 47,841 B. 93 dereference an RW object at a constant offset (9 by return-value taint in the disassembly, exact lower bound. 91 by RW-flow identifier in the matched C source, est.). 87 read only RwGlobals | the 93 object readers work once their fields sit at RW 3.x offsets. The 87 RwGlobals readers use the RW-layout record either way | 0: the objects are RW objects |
| RW-band functions the route runs as matched game code (tracker labels render / hud / util ...) | 92 object functions, not math or physics: 77 matched, 13 C0..C3, 2 no row. 67 of the 92 are executed (`RW_EXPANDED_ON_ROUTE.tsv`). Each runs RW code on a librw object unless bound | the 92 still need the full layout plus the RW internals librw lacks (below) | part of the matched core |
| struct work | none. One RW-layout record or view per case: 10 record types exist today (`MashedRwCamera`, `MashedRwLight`, `MashedRwGlobals`, `MashedRwImageRW`, `MashedRwPTankAtomic`, five `*View`) | 17 types compared, 171 fields: 57 same, 61 moved, 2 width, 51 absent (`RW_LAYOUT.tsv`). Game-read fields 25: 11 same, 8 moved, 6 absent (all RwGlobals sub-blocks, which the record already covers). 4 types have moved game-read fields: RwRaster (17 of 17 fields moved), RwCamera (16 moved, 1 absent), RpAtomic (6 moved, 5 absent), RpGeometry (8 moved, 1 width) | none |
| fields with no librw equivalent | handled per case in the bindings | 51: RpWorldSector (all 17, librw has no sector type, so no BSP), RpWorld 11, RwGlobals 12, RpAtomic 5, RpLight 3, RpClump / RpMaterial / RwCamera 1 each | none |
| plugin data | librw plugin offsets. Game code reads plugin-offset globals inline (0x007dc57c: 11 functions, T2) | a fork must also reproduce Mashed's plugin registration sizes and order, and the D3D9 raster extension (game reads raster +0x50 on the 0x004c77c0 return, past the 52-byte RwRaster). [UNCERTAIN] not measured | RW's own plugin registry |
| D3D9 device layer | librw's d3d9 backend, exe-created device adopted (`MashedRw_EngineStart_004c2fb0`) | same as A | 48 executed functions make COM calls (25 C4-match 7,046 B; 4 >= 95%; 18 lower 16,653 B; `_rwDeviceSystemFn` 0x004c7a70 2,810 B never attempted). 23,030 B unmatched |
| d3dx | not used | not used | no executed RW function calls d3dx (0 direct edges). 8 d3dx-labelled rows execute, all C2 band-level, none libid, and 5 of them are called from RtPNG / libpng code (0x00514330 is the bound RtPNGImageRead, labelled d3dx9-backward-extension). [UNCERTAIN] those labels. The 327 libid C4 d3dx rows (DirectX 9.0b SDK Summer 2003 `d3dx9.lib`, `verify/c4rdxlib_20261107/RESULT_C4R-DXLIB.md`) do not execute. That lib links into a VS2022 /MT exe and runs (`d3dx_link_test`, measured: `0.877583 -0.479426 0.267261`) |
| device needs on this machine | none beyond today | none beyond today | what the d3d9 shim does for MASHED.exe today (`mashedmod/src/d3d9_shim/d3d9_shim.cpp`, 854 lines): CreateDevice forced windowed with a 640x480 backbuffer (vtbl[16]), the Present frame limiter (60 FPS), the window border. Plus the `fix_camera_res` coupling: the game's screen-dim getters 0x00498bc0 / 0x00498bd0 must match the backbuffer, or the camera frameBuffer raster fails and RW AVs at 0x004c7785 (`re/analysis/BOOT_PATCHES.md`). No boot patch sits in the RW band. The two code caves (0x0050f6c4, 0x00508bd5) are in the d3dx band and serve `fix_fopen` / `fix_joypad`, which a matched build does not need |
| glue left after the flip | RwApiBind.cpp 1,989 lines + RwBridge 461 + RwRasterBridge 541, growing with each new binding and each view. 10 record types | the 10 hand-semantics bodies, the RW-layout RwGlobals record, a librw fork kept in step with upstream | an in-exe IDirect3D9 / IDirect3DDevice9 wrapper with the shim's behaviours (est. under 1,000 lines: the shim is 854) |
| draw calls identical to the original's | no: librw's pipelines issue their own sequence (mesh order, state cache, texture find are hand-reproduced case by case, e.g. CLUMP-TEXREAD) | no, for the same reason | yes by construction, except where the wrapper changes CreateDevice and Present |
| what stops working | nothing today | upstream librw merges get harder (struct reorder in `rwobjects.h` plus every backend that relies on it) | librw's other backends (gl3, null) on the top-down path; the librw-side Mashed fixes (`mashedmod/deps/librw/MASHED_PATCHES.md`, 7 sections) no longer apply there |
| finished work discarded | none | most of the 21 hand-layout bodies and the 10 record / view types | the 81 binding bodies (2,991 lines in the three bridge files) and librw on the top-down path. The default-path librw renderer (RwRaceFx / RwRaceSubmit / RwSceneBuild, 6,024 lines) is not affected until the flip. [UNCERTAIN] whether the route reuses any of it |
| main risk | the hidden readers grow with every matched game function that runs, and the 92 RW-band functions run RW code on librw objects | librw lacks sectors / BSP and Rt2d, so RW internals still cannot run on it. The fork covers the game-side readers only | the 205 attempted-but-unmatched executed functions, 83 of them below 50% (77,165 B). Matching speed on the RW D3D9 driver is not measured |
| RW-Physics, every option | 191 executed (75,109 B): 71 C4-match, 57 C4 by another route (27,250 B), 1 >= 95%, 61 lower, 1 never. 69,341 B unmatched | same | same |

## The lists

| file | rows | content |
|---|---|---|
| `RW_FUNCS.tsv` | 1,550 | every RW-band function: band, component (est.), tracker subsystem, confidence, phase, `static_below`, size, `best_pct`, match state, BINDINGS kind, COM globals, device reference, imports, d3dx callees, game callers |
| `RW_SURFACE.tsv` | 275 | RW-band functions called from non-RW code in ORIG_TOP reach. Route class: match 94, fam:rwcore 50, bind 84, fam:rwphysics 25, game-c0c3 16, fam:rt2d 6. 220 executed |
| `RWCORE_TO_BIND.tsv` | 70 | SIZE.tsv's 64 `fam:rwcore` + 6 `fam:rt2d` in reach. 52 executed, 44 already C4-match (they need a binding under A only because they are library rows) |
| `RW_BIND_CLASS.tsv` | 81 | each `MashedRw_*` body: source line, code lines, librw references, offset references, class |
| `RW_INLINE_READERS.tsv` | 218 | matched game functions in reach with T1 / T2 / T3 hits. `librw_relevant=y` for 180. 38 touch only math-band or physics objects (RwV3d / RwMatrix keep their layout in librw: `RW_LAYOUT.tsv` RwMatrixTag 8 of 8 same) |
| `RW_EXPANDED_ON_ROUTE.tsv` | 153 | RW-band functions in reach that the route expands as game code, grouped (rwcore 46, rwphysics 37, rt2d 30, math band 22 ...) |
| `RW_LAYOUT.tsv` | 171 | RW 3.x field, offset, width, librw field and offset, status, and the evidence where game code reads it |

The 30 largest librw-relevant inline readers (`RW_INLINE_READERS.tsv`, sorted by size). T1 is the RW API whose return
value is dereferenced, T3 the RW API that receives or returns the dereferenced identifier, T2 the RwGlobals offsets
read.

| rva | name | subsystem | B | via |
|---|---|---|---|---|
| 0x0043c5b0 | MenuDrawLoop | frontend | 3301 | T2 RwGlobals +0x20 |
| 0x00444ff0 | FUN_00444ff0 | ai | 2706 | T3 0x005586f0; T2 +0xc4 |
| 0x00430b90 | FUN_00430b90 | frontend | 1693 | T2 +0x20 |
| 0x00429e10 | FUN_00429e10 | render | 1593 | T3 0x004c19f0 (+2); T2 +0x20 |
| 0x00473ee0 | LogoOverlayDraw | frontend | 1460 | T2 +0x18 +0x20 +0x30 |
| 0x00428d30 | FUN_00428d30 | frontend | 1291 | T2 +0x20 |
| 0x00492e90 | FUN_00492e90 | boot | 1236 | T3 0x004c1c80; T2 +0x20 |
| 0x0041b7a0 | FUN_0041b7a0 | render | 1048 | T3 0x004c13e0 (+1) |
| 0x00478cf0 | CourseAssetDestroy | track | 826 | T3 0x004c5930 |
| 0x00404320 | PerModeRenderMachine | render | 807 | T3 0x004c19f0 (+2); T2 +0x0 +0x20 |
| 0x004770c0 | FUN_004770c0 | vehicle | 800 | T3 0x004e7e30 (+1); T2 0x007dc57c |
| 0x00402750 | AppInitialiseOnBootup | boot | 721 | T1 0x004caea0; T3 0x004caea0 |
| 0x0042fb70 | MenuSpriteDispatchB | frontend | 700 | T2 +0x20 |
| 0x004270f0 | CourseRenderFrame | render | 689 | T2 +0x20 |
| 0x00476df0 | FUN_00476df0 | render | 686 | T3 0x00535700 (+1); T2 0x007dc57c |
| 0x0042fe90 | MenuSpriteDispatchC | frontend | 646 | T2 +0x20 |
| 0x0041a5d0 | FUN_0041a5d0 | gameplay | 616 | T3 0x004c1480 (+1) |
| 0x004739f0 | TextSpriteScaled | frontend | 556 | T2 +0x18 +0x20 +0x30 |
| 0x00483a70 | FUN_00483a70 | render | 545 | T2 +0x20 |
| 0x0041f8f0 | FUN_0041f8f0 | render | 499 | T3 0x004c1340 (+6); T2 +0x20 |
| 0x00410b30 | sub_00410b30 | render | 467 | T2 +0x0 +0x20 |
| 0x00478220 | FUN_00478220 | render | 449 | T3 0x004cd070; T2 +0x20 |
| 0x00428a30 | FUN_00428a30 | frontend | 433 | T2 +0x20 |
| 0x004736c0 | BorderQuadFourAlpha | frontend | 423 | T2 +0x18 +0x20 +0x30 |
| 0x00441990 | FUN_00441990 | util | 406 | T3 0x004c1c10 |
| 0x00473220 | FUN_00473220 | render | 393 | T2 +0x18 +0x20 +0x30 |
| 0x004733b0 | FUN_004733b0 | render | 393 | T2 +0x18 +0x20 +0x30 |
| 0x0048b960 | FUN_0048b960 | particle | 386 | T3 0x004c1340 |
| 0x00472dc0 | FUN_00472dc0 | frontend | 378 | T2 +0x18 +0x20 +0x30 |
| 0x00473870 | TextSpriteUVExplicit | frontend | 378 | T2 +0x18 +0x20 +0x30 |

RwGlobals offsets read inline (T2, functions per offset): +0x0 10, +0x18 14, +0x20 65, +0x24 3, +0x30 13, +0x34 2,
+0xc4 2, +0xcc 3, +0xd4 4, +0xf4 2, +0x108 4, +0x10c 9. In the RW 3.6 layout these are curCamera, the RwDevice block
(zBufferNear +0x18, fpRenderStateSet +0x20, fpRenderStateGet +0x24, fpIm2DRenderPrimitive +0x30,
fpIm2DRenderIndexedPrimitive +0x34, the last three confirmed in `RESULT_RWG-RASTER-BIND.md`), the string functions
(+0xc4..+0x104) and memoryFuncs malloc / free (+0x108 / +0x10c). librw's `rw::Engine` has none of the blocks past
+0x08 (`RW_LAYOUT.tsv`). The route covers them with the RW-layout record (`MashedRw_InstallRwGlobals`).
`RESULT_PORT-OT-RWG-SLOTS.md` scores it: 47 rows OK and 25 that still need a binding.

## How the counts were made, and their limits

- **Executed** = `re/coverage/reach.tsv`. It is an INT3 on every entry the 2026-10-11 census knew, so it is not
  bounded by bound entry points and covers RW internals directly. 229 RW-band functions have no census row (Ghidra
  created them later). 220 of those are in the static callee closure of executed RW code (`static_below=y`). Whether
  they run is not measured.
- **Match state** = hooks.csv confidence plus the best `best_pct` in `match_ledger.tsv`. "Attempted" means a ledger row
  exists. Only 3 executed RW functions have none.
- **T1** (disassembly): eax after `call <RW-band fn>` is followed through register moves. An operand `[reg+disp]`
  counts. Stack spills, arguments and objects held in game structs are not followed, so T1 is a lower bound (9
  functions on object APIs).
- **T2**: absolute addresses that only RW-band code writes (789 addresses), read by a matched game function in reach
  (108 functions). Offsets come from following `mov reg,[G]`.
- **T3** (matched C source): an identifier passed to or assigned from an RW-band call, and dereferenced at a constant
  offset or `->field` in the same body.
  - It does not track reassignment, so it can over- and under-count: an estimate.
  - It catches the ATOMIC-VIEW case 0x0048fef0 (`g{004e89a0}[8]`), which T1 / T2 miss.
  - Math-band callees `[0x004c3700, 0x004c5800)`, RtQuat / RtSpline / RtRandom and RW-Physics are excluded from
    "librw-relevant": those types keep their layout or are not in librw at all.
- **Bindings class** is a regex heuristic over each body (`bind_classes`). Spot-check: `MashedRw_ImageSetFromRaster_004d52d0`
  (RWG-RASTER-BIND) and the camera / light records come out hand-layout. `MashedRw_ClumpForAllAtomicsAsViews_004e66d0`
  comes out hand-semantics although it builds views, so hand-layout 21 is a floor.
- **Layouts**: compiled, not hand-counted.
  - [UNCERTAIN] The RW side is gta-reversed's RW 3.6 headers, and Mashed links 3.7.0.2.
  - Every offset a RESULT file measured on Mashed agrees with the 3.6 layout: atomic +0x18, raster +0x22, camera +0x60,
    frame +0x98 / +0x9c / +0xa0, RwGlobals +0x20 / +0x34 / +0x60 / +0x108 / +0x10c / +0x11c.
  - Unmeasured fields are header values.
- **Option C device needs** come from the shim source and BOOT_PATCHES.md. No matched-RW device bring-up was run.

## Decision input for Mariano

**What each option costs.**

- **(A) Per-function bindings to librw.** The cheapest next step: 70 more bindings, 52 of them on code the original
  runs. It does not end, though:
  - 180 matched game functions already read RW state inline. 93 of them read RW objects, a counted estimate.
  - Another 92 RW functions that the tracker files as game code run RW code on librw objects.
  - So every new stretch of matched game code brings its own hand bodies, the way ATOMIC-VIEW, CLUMP-TEXREAD and
    RWG-RASTER-BIND did.
  - Today 31 of the 81 bindings are already hand bodies (21 for layout, 10 for semantics), not forwards.
  - Draw calls stay librw's, not the original's.
- **(B) A librw fork with RW 3.x layouts.** Fixes the game-side readers:
  - Of the 25 fields game code is seen reading, 8 have moved, in 4 types: raster, camera, atomic and geometry.
  - Each of those 4 structs needs a full reorder, plus Mashed's plugin sizes. The plugin part is not measured.
  - It does not let RW's own code run: librw has no world sectors (BSP) and no Rt2d.
  - The 10 semantic bindings and the RwGlobals record stay.
  - Draw calls still differ.
  - The project then owns a fork of a maintained upstream.
- **(C) The matched RW core on d3d9.** Of the 668 RW functions the original runs:
  - 446 are already byte-matched. 222 are not (132 KB), and 83 of those (77 KB) are below 50%.
  - The D3D9 driver is the hardest part: 48 functions, 23 KB unmatched.
  - The extra glue is the d3d9 shim's behaviour moved into the exe (windowed 640x480, frame limiter, camera-resolution
    coupling), which exists today as 854 lines.
  - It removes the binding layer (about 3,000 lines) and every future hand body.
  - It gives draw calls identical to the original's by construction.
  - d3dx is not needed on the executed path. If it ever is, the Summer 2003 library links into the exe (measured).
- **Common to all three.** RW-Physics (191 executed, 69 KB unmatched).

**Recommendation: (C).** It is the only option whose cost is a fixed, counted list:
- 222 functions, 132 KB, plus 220 static-only functions to check.
- (A) and (B) grow with every matched game function the route runs.

**Main uncertainty.** How fast the 205 below-95% RW functions can be matched, especially the 83 below 50% and the D3D9
driver, and how many of the 220 functions with no census row actually run.

A middle path exists inside (C):
- An unmatched RW function gets an exe hand body written against RW 3.x structs, not librw.
- The route keeps moving, and the body goes away when its match lands.
- That keeps every object in one layout from the start.

This RESULT does not take the decision.
