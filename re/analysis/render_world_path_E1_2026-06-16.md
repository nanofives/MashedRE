# WS-E / E1 — Mashed's RenderWare world render path (RVA-cited map)

Session 2026-06-16, pool6 (read-only). Anchor verified:
`original/MASHED.exe.unpatched` SHA-256
`bdcae093a30fbf226bdd852b9c36798a987aee33b3ae82bf7404b0336efd3c0e`
== CLAUDE.md anchor. Every RVA below was decompiled this session via Ghidra
MCP on `Mashed_pool6`; no value is inferred (NO-GUESSING).

Goal of E1 (ROADMAP "Completion plan" WS-E): RE + port the RW world render
path (RpWorld sector render, atomic/clump) to replace the D3D9 spike
(`D3d9Render/TrackRenderer`). This note is the prerequisite map; it ends with
the **port-boundary decision** the user must make before the verbatim port
starts (architecture gate, CLAUDE.md "renderer choice").

## 1. The per-frame loop — FUN_00492e90

`FUN_00492e90` (C1, body 0x00492e90..0x00493364) is the per-frame game tick +
render. It branches on the game-state global `DAT_00771968` (1=menu/frontend,
3 & 7 = in-race A, 5 & 6 = in-race B; observed from the switch arms). The
render bracket recurring in the in-race arms is pure RenderWare camera flow:

```
cam = FUN_004671a0(idx)        // get the active RwCamera
FUN_00426670(cam)             // WorldRenderBegin  → FUN_004e4320(world, cam)
if (FUN_004c1a00(cam)) {      // RwCameraBeginUpdate (camera->beginUpdate, +0x18)
    FUN_00410b30()           //   IN-GAME RENDER DISPATCHER  ← scene composition
    FUN_004c19f0(cam)        // RwCameraEndUpdate  (camera->endUpdate)
}
... HUD camera via FUN_004671c0 / FUN_0040de30 (minimap ortho) ...
FUN_004266b0(cam)            // WorldRenderEnd    → FUN_004e4350(world, cam)
...
cam = FUN_004671a0(0,0,1); FUN_004c1be0(cam)   // RwCameraShowRaster (present)
```

Supporting facts:
- `FUN_004671a0(idx)` (C2, 0x004671a0): returns the race camera `FUN_0042f510()`
  when `FUN_0042b930()==3` (in-race) and `idx != -1`; otherwise returns the
  main camera `DAT_006905b0`. So idx 0 in-race = race cam; idx -1 = always main.
- `FUN_004c1a00(cam)` (0x004c1a00): single indirect call `(**(cam + 0x18))()` —
  the RwCamera `beginUpdate` callback at struct offset **+0x18**. Confirms the
  camera is a real RwCamera with begin/end update fn-ptrs.
- `FUN_00426670`/`FUN_004266b0` already ported verbatim
  (`Render/FrameWorldPasses.cpp`): guard `DAT_0066d704` (world-loaded), select
  world handle A=`DAT_0065742c` / B=`DAT_00656ee8` by `DAT_0066d700`, call
  `FUN_004e4320`/`FUN_004e4350(world, cam)` (set/clear `world+0xc` = current
  camera slot via `DAT_007d716c`).

## 2. The in-game render dispatcher — FUN_00410b30

`FUN_00410b30` (C1, "InGameRenderDispatcher", 0x00410b30..0x00410d03) is the
**scene composition** layer — it does NOT itself emit geometry; it sets RW
render states through the device dispatch and calls the sub-renderers in order.

The device dispatch is the recurring `(*(code *)DAT_007d3ff8[8])(state, value)`
(equivalently `(**(code**)(DAT_007d3ff8 + 0x20))(...)`): a function pointer in
the RW device object `DAT_007d3ff8` at byte **+0x20** = **RwRenderStateSet**
(state ids seen: 0x14, 0xe, 6, 8, 10, 0xb, 0xf, 0x10 — render-state/blend/zwrite
toggles). `DAT_007d3ff8` is the RW device/pipeline object; `*DAT_007d3ff8` (its
first field) is the device's object-list base.

Sub-render order (geometry-bearing calls in bold):
1. `RwRenderStateSet(0x14, 2)`
2. **`FUN_004270f0(cam)`** — world render dispatch (§3). cam =
   `*(PTR_PTR_005f2770 + DAT_0063ba78*4)` (active camera from a camera array).
3. `RwRenderStateSet(0xe, 1)`; `FUN_004725c0()`; `FUN_0045b350(device)`
4. **car loop**: `for i in 0..3: FUN_00420050(i, device)` — per-player clump
   render (§4), guarded by `DAT_0063ba98==0`.
5. `FUN_00411ce0()`, `FUN_0041ebb0(device)`, second `FUN_0045b350` loop
6. `FUN_004219c0()`, `FUN_00425e40()` (if `DAT_007f0fd0 != 5`),
   **`FUN_00426640(cam)`** (world overlay pass, §3), then HUD/sprite/particle/
   debug draws (`FUN_0040bde0`, `FUN_004891f0`, `FUN_00475e50`, `FUN_00477810`,
   `FUN_00448730`, `FUN_00490490`, ...). Not world geometry — out of E1 scope
   (E4 HUD/Im2D, separate session).

## 3. World render dispatch — FUN_004270f0 → FUN_00484580 → FUN_004844a0

`FUN_004270f0(cam)` (C1, 0x004270f0) — guarded by `DAT_0066d704`. Sets blend/
zwrite render states, clears/sets up the camera context (`FUN_0041e9b0(cam)`),
then dispatches the world geometry render:

```
if (DAT_0066d700 == 0) {                 // world-A selected
    cam = FUN_004671a0(0);
    FUN_00478cd0(&DAT_00646e58, cam);    // → world render (world-A struct @ DAT_00646e58)
} else {                                  // world-B selected
    RwEngineForAllPlugins(DAT_00656ee8); // 0x004e5660 — world render core (world-B)
}
```

- `&DAT_00646e58` is an array of 0x40-byte view/world descriptors (also indexed
  as `&DAT_00646e58 + idx*0x40` later in the same fn). It is the world-A render
  context passed as arg1.
- `FUN_00478cd0` (0x00478cd0): `MOV ECX,[ESP+4]` then tail-calls
  `FUN_00484580(ctx, cam)`. (Ghidra recovered it as a 0-arg thunk; the raw
  first instruction + the 2-arg call site confirm it forwards both args.)
- `FUN_00484580(ctx, cam)` (C2, 0x00484580): the **render-mode dispatcher**.
  Switches on `DAT_0066d728` (debug/view-mode selector, cases 0x14..0x1e =
  sector-vis / clump-cycle dev modes). The shipping path is
  `DAT_006cfcac==0 → FUN_004844a0(cam)`; the `!=0` path `FUN_00484310(cam)` is
  the alternate/debug render. [The dev view-mode switch is why a prior C1 note
  called this "debug-overlay"; the default `DAT_006cfcac==0` arm is the real
  world render.]

`FUN_004844a0(cam)` (C2, 0x004844a0) — the world geometry render:
```
DAT_006cf070 = FUN_004e5840();   // get world->renderCallback (world+0x68)
FUN_004e5820();                  // set world->renderCallback
RwEngineForAllPlugins(...);      // 0x004e5660 — iterate sectors + render
FUN_004e5820();                  // restore world->renderCallback
if (DAT_006cfca4) { ...FUN_004b61c0(sector, &0x400000ff)... }  // collision/debug overlay
```
- `FUN_004e5820(world, cb)` (0x004e5820): set `world+0x68 = cb` (default
  `FUN_004e5190` if null). `FUN_004e5840(world)` (0x004e5840): return
  `world+0x68`. So **world+0x68 = the world's render callback slot.**

`FUN_00426640(cam)` (C1, 0x00426640): the world overlay pass (step 6) —
`FUN_0041e9b0(cam); if (FUN_0041ea70()) FUN_0041e950();` — second world draw
pass (transparency/immediate). Same world context, separate from the main pass.

## 4. The render core — RpWorldRender / sector traversal / RpAtomic callback

`RwEngineForAllPlugins` @ **0x004e5660** (FidDB MIS-NAME — mechanically the
world render dispatcher, NOT plugin iteration):
```
RwEngineForAllPlugins(userdata):
    FUN_004e47b0(*DAT_007d3ff8, FUN_004e5680, userdata)
```
`FUN_004e47b0(listbase, cb, ud)` @ 0x004e47b0 = generic **for-all over a sector
list**: walks `DAT_007d716c + listbase` (head ptr at +0, count at +8), calling
`cb(*element, ud)` until cb returns 0. (`DAT_007d716c` is the same per-object
extension base used by FUN_004e4320 world+0xc camera slot.)

`FUN_004e5680(sector)` @ 0x004e5680 = **the per-sector render callback** — the
canonical RpWorldSector render:
```
if (FUN_004f0900(sector)) {                         // sector frustum-cull test
  for (atomic-link = *(sector+0x38); link != sector+0x38; link = *link) {
     atomic = link[2];
     if ((*(atomic+2) & 4) &&                        // atomic render flag
         *(short*)(atomic+0x60) != *(short*)(DAT_007d3ff8+2)) {  // per-frame stamp dedup
        sphere = RpAtomicGetWorldBoundingSphere(atomic);   // CONFIRMED RW symbol
        if (FUN_004c1b40(*DAT_007d3ff8, sphere))    // RwCameraFrustumTestSphere -> visible?
           (**(code**)(atomic + 0x48))(atomic);     // atomic->renderCallBack(atomic)  ← RpAtomicRender
        *(short*)(atomic+0x60) = *(short*)(DAT_007d3ff8+2); // stamp current frame
     }
  }
}
```

Key structural facts (all RW-canonical):
- **Atomic render callback = vtable/obj offset +0x48**, called `atomic->render(atomic)`.
  This is the SAME +0x48 used by the per-car render in `FUN_00420050` — world
  geometry and vehicles share ONE render primitive (the RpAtomic default render
  callback). This is the single most important fact for the port.
- Sector atomic list = intrusive linked list at sector **+0x38**.
- Per-atomic render-frame stamp at atomic **+0x60**; current frame counter at
  device **+2** (`DAT_007d3ff8+2`) — prevents double-rendering an atomic that
  lives in multiple sectors.
- Atomic flags byte at atomic **+2**, bit 4 (0x4) = rpATOMICRENDER.
- `RpAtomicGetWorldBoundingSphere` — Ghidra already recognizes this name
  (confirmed RW). `FUN_004c1b40(device, sphere)` = camera frustum sphere test
  (returns nonzero=visible). [exact RW name RwCameraFrustumTestSphere — name
  UNCERTAIN, mechanics certain].
- `FUN_004f0900(sector)` = sector visibility/frustum test [exact predicate
  UNCERTAIN; returns nonzero=render this sector].

### Per-car render — FUN_00420050(playerIdx, cam)
`FUN_00420050` (C2, 0x00420050): per-player render. Indexes the per-player
struct array `&DAT_0063dc38` (stride 0x2ac). Sets the player's camera, applies
a light scale to `DAT_0063d850` (light @ +0x18..+0x24 × per-car factor
`DAT_0063dc64` — this is the **vehicle lighting consumer, ledger #9**), then:
```
if (flags & 0x10/0x1000 clear)  body = *(player + 0x?);  if (body) (**(body+0x48))(body);  // body atomic render
if (flags & 0x20/0x2000 clear)  shadow = ...;            if (shadow) (**(shadow+0x48))(shadow); // shadow/2nd atomic
```
Same `obj->render` @ +0x48 callback. Light setup via `FUN_004e4900(light, vec)`.

## 5. The bottom — the RW device & the +0x48 atomic render callback

Above the +0x48 callback everything is GENERIC RW (sector traversal, frustum
culling, the for-all dispatch, the begin/end-update camera ops). These are
small, leaf-ish, and diff-checkable: `FUN_004e5660`, `FUN_004e47b0`,
`FUN_004e5680`, `FUN_004f0900`, `FUN_004c1b40`, `FUN_004671a0`, `FUN_004270f0`,
`FUN_004844a0`, `FUN_00420050`.

The **+0x48 atomic render callback** is where material/mesh iteration + lighting
live (the diff-checkable BEHAVIOR — ledger #9). It bottoms into the RW raster/
material/immediate-mode pipeline → the **RW D3D9 device object `DAT_007d3ff8`**
(render-state set @ +0x20; first field = object list). That device layer is the
RW D3D9 driver — the vendored-library band kept at C1 (and the qhull/RW-Physics
sibling of it). Below the device, RW calls `IDirect3DDevice9` vtable methods.

### librw fallback status (Option A) — already evaluated, NOT viable as-is
`re/analysis/librw_plugin_compat/REPORT.md`: MASHED registers 14 RW engine
modules (`rwVENDORID_CRITERIONINT` ids 0x401..0x412); librw implements only 4
(Frame/Image/Raster/Texture) and ALL 4 have different struct sizes; 10 are
absent. Plus librw's WORLD/BSP sector streaming is not implemented (gate brief).
So Option A is not a drop-in; it would require deriving Mashed's module struct
layouts AND feeding it our world parser either way.

## 6. Port-boundary decision (the architecture gate for E1)

Top-level renderer choice is RATIFIED (Option B incremental, RENDERER_GATE_BRIEF
2026-06-10). The OPEN sub-decision — material consequences, weeks vs months —
is **how deep does "verbatim" go**, given the render bottoms out in the full RW
D3D9 driver. Three coherent boundaries:

- **B-generic (recommended).** Port the generic RW render layer verbatim — the
  §4 functions: RpWorldRender dispatch (0x004e5660 + 0x004e47b0), the per-sector
  callback (0x004e5680) with frustum culling (0x004f0900 / 0x004c1b40), the
  RpAtomic render callback contract (+0x48), and the camera begin/end-update +
  world-render bracket (already partly ported). Implement the RW **device
  function table** (`DAT_007d3ff8`: RwRenderStateSet @ +0x20, raster bind,
  primitive submit) on our OWN D3D9 backend — the spike already proves D3D9
  geometry submission. Result: traversal/culling/draw-order/material/lighting
  become diff-checkable verbatim ports; only the D3D plumbing is ours (same
  status as the spike's D3D usage today). Cost: weeks; bounded function set.
- **B-full.** Also port the RW D3D9 driver verbatim down to the
  `IDirect3DDevice9` calls (raster manager, im3d pipeline, render-state cache,
  format conversion). Maximally faithful incl. the D3D plumbing, but it is
  porting the entire RenderWare D3D9 driver — the bulk of the C1 vendored band.
  Cost: months.
- **A (librw fallback).** Adopt librw for the generic layer; per the compat
  report this needs Mashed-side module-struct rederivation + our world parser
  anyway, and absorbs the codebase into librw's API. The gate ranked it the
  fallback only if the RW core "proves impractical to port" — §4 shows the
  generic core is small and tractable, so the fallback condition is NOT met.

Recommendation (mine): B-generic, for cost. **DECISION (user, 2026-06-16):
B-FULL** — port the entire RW render path verbatim, including the RW D3D9 driver,
down to the `IDirect3DDevice9` calls. So the device function table, the RxPipeline
machinery, the raster manager and the render-state translator are ALL in-scope as
verbatim ports (§7–§8 below map the device table + the bottom chain that B-full
must port). Maximally faithful + fully diff-checkable; the cost is reimplementing
the RenderWare D3D9 driver. WS-E sessions sequence: E1 (this map) → device-table
+ RxPipeline node graph RE → port the generic layer → port the D3D9 driver leaves
→ E2/E3/E4 fall out as the material/lighting/Im2D pipeline nodes.

## 7. The RW device & the D3D9 driver function table (B-full port targets)

`DAT_007d3ff8` = active **RwGlobals** pointer. Initially `&DAT_007d3ec8` (a
0x12c-byte / 0x4b-dword .bss template, ZEROED statically — the table is filled at
init); after RwEngineOpen (`FUN_004c30b0`, 0x004c30b0) it points to a 0x12c-byte
heap clone. State field `RwGlobals[0x49]` (+0x124): 1=init, 2=opened.

The **RwDevice** is embedded at **RwGlobals+0x10**, so the game's `DAT_007d3ff8[8]`
(+0x20) = RwDevice+0x10 = fpRenderStateSet. The static RwDevice template lives in
.data at base **0x00618220** (the D3D9 driver descriptor); decoded literally
(`memory_read`):

| RwDevice off | RwGlobals off | value / RVA | RW field |
|---|---|---|---|
| +0x00 | +0x10 | `1.0` | gammaCorrection |
| +0x04 | +0x14 | **0x004c7a70** | fpSystem — 23-case `rwDEVICESYSTEM_*` dispatch |
| +0x08 | +0x18 | `0.0` | zBufferNear |
| +0x0c | +0x1c | `1.0` | zBufferFar |
| +0x10 | +0x20 | **0x004d7480** | fpRenderStateSet ✓verified |
| +0x14 | +0x24 | **0x004d6910** | fpRenderStateGet |
| +0x18 | +0x28 | **0x004db5b0** | fpIm2DRenderLine |
| +0x1c | +0x2c | **0x004db840** | fpIm2DRenderTriangle |
| +0x20 | +0x30 | **0x004dba60** | fpIm2DRenderPrimitive |
| +0x24 | +0x34 | **0x004dbc90** | fpIm2DRenderIndexedPrimitive |
| +0x28.. | +0x38.. | 0 (in template) | fpIm3D* / others installed at OPEN |

- `fpSystem` `_rwDeviceSystemFn` @ **0x004c7a70** (C0, FPO prologue — Ghidra
  undefined; `re/analysis/render_d3d9_device/0x004c7a70.md`): 23-case switch
  (table @ 0x004c856c). case0 OPEN = `Direct3DCreate9(31)` via `FUN_004dd140` →
  IDirect3D9* `[0x007d4108]`; case2 START = `IDirect3D9::CreateDevice` (vtbl
  +0x40) → IDirect3DDevice9* `[0x007d4120]`.
- `fpRenderStateSet` @ **0x004d7480** (verified): switch on render-state id
  (1..0x1e); maps RW states→D3D9 via a DEFERRED render-state token queue
  (`&DAT_007d5168[DAT_007d6c14]`, flushed before draw) and direct
  `IDirect3DDevice9` vtbl calls (`*DAT_007d4110 + 0x114` = SetTexture for the
  texture-stage states 2/3/4/0xd). State ids match InGameRenderDispatcher's
  usage (8 zwrite, 0xe cull/lighting, 0xf fogcolor, 0x10 tex-filter, ...).
- `DAT_007d4108` = IDirect3D9*, `DAT_007d4110` & `DAT_007d4120` = IDirect3DDevice9*
  (the d3d9 device the driver drives; d3d9 vtbl offsets: +0x114 SetTexture, +0x148
  DrawIndexedPrimitive).

## 8. The atomic render callback & the render pipeline (the §4 +0x48 target)

`RpAtomicCreate` @ **0x004e67b0** (FidDB-named) — allocates the atomic via the
RwGlobals allocator `(**(DAT_007d3ff8+0x118))(alloc, 0x30014)` and initializes the
**RpAtomic struct** (fields cited verbatim):
- `+0x00`=1 (rpATOMIC type), `+0x02`=5, `+0x03`=1 (flags)
- `+0x10`=`FUN_004e6880` (object sync callback)
- `+0x44`=0 (frame), `+0x48`=**`FUN_004e5f90`** ← DEFAULT render callback
- `+0x4c`=3 (render flag / interpolation), `+0x54`/`+0x58`=1.0/1.0
- `+0x60` (short) = per-frame render stamp (compared to `RwGlobals+2` in §4)
- `+0x64`/`+0x68` = clump-membership list links; `+0x6c`=0 (atomic pipeline ptr)

So **world atomics AND car atomics share one default render callback,
`FUN_004e5f90` @ atomic+0x48** (cars use the same unless an atomic is given a
matfx/skin pipeline via the +0x6c override — none observed yet; [UNCERTAIN:
matfx/skin variants exist? check vehicle DFF atomics' +0x6c at runtime]).

`FUN_004e5f90(atomic)` @ **0x004e5f90** (the render callback):
```
pipe = atomic->+0x6c;                               // atomic's own pipeline
if (pipe == 0) pipe = *(DAT_00911ae0 + 0x3c + DAT_007d3ff8);  // engine default obj pipeline
FUN_004d40d0(pipe, atomic, 1);                       // RxPipelineExecute
```

`FUN_004d40d0(pipeline, atomic, flush)` @ **0x004d40d0** = **RxPipelineExecute**:
sets the global exec context (`_DAT_00911ac0`=pipeline, `_DAT_00911ad0`=atomic,
`_DAT_00911ad4`=`DAT_007d4710` render heap/cluster mgr), then runs the pipeline's
first node via `(**(*(pipeline+8) + 4))(*(pipeline+8), &DAT_00911ad0)` — i.e.
`node->vtable[1](node, &io)`. The node graph (instance → transform/cull → light →
submit) is the RxD3D9 atomic pipeline; the submit leaf issues
`IDirect3DDevice9::DrawIndexedPrimitive`. **This node graph is the E2 (material/
mesh/draw-order) + E3 (lighting) work.**

### Complete chain (per-frame → D3D9 draw), all RVAs verified this session
```
FUN_00492e90 frame loop
 └ FUN_00410b30 InGameRenderDispatcher (sets RW render-states via fpRenderStateSet)
    ├ FUN_004270f0(cam) world dispatch ─ guard DAT_0066d704
    │  └ FUN_00478cd0→FUN_00484580→FUN_004844a0(cam)   (DAT_0066d728 view-mode)
    │     └ RwEngineForAllPlugins 0x004e5660  → FUN_004e47b0(*RwGlobals, FUN_004e5680, ud)
    │        └ FUN_004e5680 per-sector: FUN_004f0900 cull; for-atomic:
    │              RpAtomicGetWorldBoundingSphere + FUN_004c1b40 cam-frustum;
    │              atomic->render(+0x48) = FUN_004e5f90
    └ FUN_00420050(i,cam) i=0..3 per-car: body/shadow atomic->render(+0x48)
                          (+ vehicle light scale DAT_0063d850, ledger #9)
                              ↓ (both paths)
                  FUN_004e5f90  → FUN_004d40d0 RxPipelineExecute(pipe, atomic)
                              ↓
                  pipeline nodes (instance/cull/light/submit)  ← E2/E3
                              ↓
                  RW D3D9 driver: fpRenderStateSet 0x004d7480 + raster bind
                              ↓
                  IDirect3DDevice9 (DAT_007d4120) :: DrawIndexedPrimitive (vtbl+0x148)
```

## 9. Open / next (B-full port sequence)
- **E2 prep:** decompile the RxPipeline node graph from `FUN_004d40d0` — walk
  `pipeline+8` node vtable[1] chain; identify instance/cull/light/submit nodes;
  the submit node's D3D9 `DrawIndexedPrimitive` call (device `DAT_007d4120`,
  vtbl +0x148). Find the default object pipeline `*(DAT_00911ae0+0x3c+RwGlobals)`.
- **Device table completion:** the OPEN handler (0x004c7a70 case0) installs the
  Im3D + dynamic fns into RwGlobals at runtime — enumerate those; map the raster
  manager (RASTERMODULE ctor 0x004c78e0) + texture bind path.
- Confirm whether any atomic overrides `+0x6c` (matfx/skin pipeline) — check the
  vehicle DFF atomics at runtime (scenario-attach).
- `FUN_004c1b40` exact frustum predicate; `FUN_004f0900` sector test predicate.
- **Verification (WS-H1):** boot-original `diff-original` on the generic leaves
  (0x004e47b0, 0x004e5680, 0x004e5f90, 0x004d40d0, 0x004f0900, 0x004c1b40,
  fpRenderStateSet 0x004d7480) once a render slice runs.
