# librw sizing brief — M3 shipping renderer (gate D2 consequence)

**Date:** 2026-07-31 · **Branch:** `fix/u9025-recharacterise-and-regabi-defects` @ `b10c5c30`
**Scope:** sizing only. No renderer code lands in this session. Deliverable = go/no-go + bounded
E1'–E4' sessions + risk register.
**Trigger:** gate **D2 resolved 2026-07-31** — librw is the shipping renderer, reversing the
2026-06-10 RW-verbatim ratification (`RE_MASTER_PLAN_2026-07.md` §3-M3, §5).

---

## 0. Verdict — **GO**, with one costed complication

librw is a clean fit for Mashed on every axis that could have killed the lane:
MIT, alive, RW 3.1–3.7 (Mashed is **3.7.0.2**), first-class `win-x86-d3d9`, and it
**compiles clean under our exact locked toolchain — measured this session, not claimed.**

The two things the README warns about (*"BSP is not supported at all"*, PS2 DFF gaps) are
**not blockers for us**, because we do not need librw's stream readers: we already parse
Mashed's BSP, DFF and TXD ourselves into renderer-agnostic structs. librw is being adopted as
a **rendering pipeline**, not as a file-format library.

The real cost is not librw. It is **`TrackRenderer.cpp` — 4139 LOC that fuses the D3D9 draw
path with race gameplay state.** That TU must be split before anything can be swapped. See
Risk R1; it dominates the estimate.

**Stop-and-ask items are all cleared:** license is unambiguous (§1.1), the 32-bit MSVC path is
alive and measured (§1.3), so **D2 does not reopen**. The one decision that needs the user is
formally adding librw as a dependency (§3.4) — see the question at the end.

---

## 1. Web facts — librw upstream (aap/librw)

### 1.1 License — MIT, unambiguous, permits our use
`LICENSE`, 1071 bytes, verbatim MIT, `Copyright (c) 2014 aap`; GitHub API reports
`spdx_id: MIT`. MIT permits use, modification, and redistribution in a closed-or-open
derivative provided the copyright notice ships. **No ambiguity, no copyleft, no
attribution-in-UI requirement.** Concretely we must ship the notice with `mashed_re.exe`
(a `THIRD_PARTY_NOTICES.txt` beside the exe satisfies this).

Contrast with `re/prior_art/`'s SciLor repos, which are unlicensed and therefore
knowledge-only. librw is the opposite: we may vendor and ship the actual source.

### 1.2 Repo health
| Fact | Value |
|---|---|
| Upstream | `https://github.com/aap/librw` |
| Last push | 2026-07-14 (17 days before this brief) — **actively maintained** |
| Archived | no · Stars 794 · Open issues 46 · Size ~4.1 MB |
| Local reference clone | `re/prior_art/renderware/librw` @ `1252b90e` (2026-04-28, *"more tex uniforms"*) |

`re/prior_art/renderware/` is **gitignored** (`.gitignore:101`) and line 98 already records
`librw … https://github.com/aap/librw.git (MIT)`. So the clone is present as reference but is
**not** a tracked dependency today.

### 1.3 x86 / MSVC 2022 support — **MEASURED, not assumed**
`premake5.lua` declares `win-x86-d3d9` as a first-class platform (alongside `win-x86-gl3`,
`win-x86-null`, and the amd64 variants) with a Windows-only `ReleaseStatic` configuration.

Rather than trust that, this session **built it** with our locked toolchain:

```
vcvars32.bat (MSVC BuildTools 2022, cl 19.44, x86)
cl /nologo /c /EHsc /O2 /MT /DRW_D3D9 /DNDEBUG /I<librw> /I<librw>\src /wd4996 /wd4244 \
   src\*.cpp src\d3d\*.cpp src\lodepng\*.cpp
lib /out:librw.lib *.obj
```

| Result | Value |
|---|---|
| Errors | **0** |
| Warnings | **3** — all `C4838` narrowing, all `src/d3d/xbox.cpp:542-544` (`D3DFMT_UNKNOWN` enum → `uint32`), all benign |
| Objects produced | 45 |
| Static lib | `librw.lib`, 1,232,788 bytes |
| Architecture verified | `engine.obj` COFF machine word = **`0x014C` = IMAGE_FILE_MACHINE_I386** |

Notes that make this cheap:
- **premake is not needed.** The whole library is a flat set of `.cpp` with no generated
  sources; `cl` over two globs is sufficient. (`premake5.exe` ships in-tree anyway, and
  `premake-vs2019.cmd` exists, but we do not need either.)
- **fxc / the DirectX SDK are not needed.** The D3D9 shader blobs are **pre-compiled and
  committed** as `.h` byte arrays (`src/d3d/shaders/*_VS.h`, `*_PS.h`). `make_default.cmd`
  et al. only exist to *regenerate* them and reference `%DXSDK_DIR%` — we never invoke them.
- `d3d9.h` comes from the Windows SDK already installed with BuildTools. No extra SDK.

**This is the single most important de-risk in the brief: E1' is a solved problem.**

### 1.4 Backend maturity and format coverage
- **Backends:** D3D9 and OpenGL ≥2.1/ES≥2.0 are production; PS2 is *"working as a test only"*.
  D3D9 is the backend re3/reVC ship on Windows, i.e. the best-exercised path in the project.
- **Stream versions:** `src/rwbase.h` maps `0x04000000`=3.1 … `0x1C000000`=3.7 with
  `libraryIDPack`/`libraryIDUnpackVersion`. **Mashed's assets are 3.7.0.2 — the top of the
  supported range**, the same band as GTA:SA.
- **Native-data platforms read:** PS2, D3D8, D3D9, Xbox (`src/d3d/`, `src/ps2/`).
- **Stated limitations:** *"Not all pre-instanced PS2 DFFs are supported"* (irrelevant — we are
  PC), and **"BSP is not supported at all"** (see §3.2 — neutralised).
- **Default D3D9 pipeline is shader-based:** `d3d9.cpp:697` assigns
  `pipe->renderCB = defaultRenderCB_Shader` (vs_2_0/ps_2_0). A fixed-function callback
  `defaultRenderCB_Fix` (`d3d9render.cpp:78`) exists and can be assigned instead. Our current
  spike is fixed-function, so this is a deliberate switch, not an accident — see Risk R6.

### 1.5 Device ownership — librw creates its own D3D9 device
```c
struct EngineOpenParams { HWND window; };   // src/d3d/rwd3d.h
extern IDirect3DDevice9 *d3ddevice;         // src/d3d/rwd3d.h
```
`src/d3d/d3ddevice.cpp` calls `Direct3DCreate9` (:1518) and `CreateDevice(…, D3DDEVTYPE_HAL, …)`
(:1622), owns `d3d9Globals.present`, handles `Reset` on resize (:1299-1308) and calls
`Present` itself (:1356). You hand it an `HWND`; it owns everything below that.

Because `d3ddevice` is a plain extern global and the post-create init is a small isolated block
(`:1192-1211` — grab default render target + depth surface into the device cache), **adopting an
externally-created device is a ~30-line local patch** if we ever need it. See §3.3.

---

## 2. Our surface (worker survey, read-only, off-quota)

### 2.1 Render subtrees
| Subtree | LOC | Role |
|---|---:|---|
| `Txd/` | 395 | `TxdDecoder` — Mashed's **proprietary chunk-id `0x23`** TXD → `Txd::Dictionary` |
| `Rws/` | 285 | nested-chunk walker; `RwsStreamRead.cpp` is .asi-only |
| `Track/` | 1304 | `TrackWorld` (BSP→`Track::World`), `DffModel` (DFF→batches), `TrackData` (SPL/ANM/UVA/MTS/LAPDATA) |
| `D3d9Render/` | **8433** | the actual standalone pixel path (28 files) |
| `Render/` | 8395 | **not the pixel path** — 53 harvested C2→C3 leaf-function TUs for the .asi |

Pixel entry points in `exe_main.cpp`: `RwIm2DBridge_Install(g_device)` (:5652) for menu 2D,
`g_track.Render(g_device, t, &ci)` (:2539) for race 3D, plus the native video quad (:2843).

`RwWorldRender` / `RwWorldLoad` / `RwWorldLoadStubs` / `RwWorldStream` (~820 LOC) are the old
WS-E1/E2 RpWorld seam and are **inert** — `RwWorldRender_Enabled()` is false in the exe. Under
D2 these become either the librw seam or dead code; decide in E2'.

### 2.2 Loader output structs — all ours, all clean
- `Txd::Mip{w,h,depth,stride,pixels,pixel_bytes,palette,palette_bytes}`,
  `Txd::Texture{name[33],mask_name[33],filter_addressing,mip_count,mips[16]}`,
  `Txd::Dictionary{textures_[256],count_,device_id_}`
- `Track::DffMaterial{tex_name,rgba,uv_anim}`,
  `Track::DffBatch{material,atomic,abox[6],verts,uvs,normals,prelit,tris,lit,modulate_mat}` (model-space, frame-baked),
  `Track::DffModel{materials,batches,bbox,total_tris,total_verts}`
- `Track::Material{...}`, `Track::Sector{verts,uvs,prelit,tris}`,
  `Track::World{materials,sectors,bbox,total_tris,total_verts}`
- `TrackData.h`: `Spline`, `HAnim`/`HKeyFrame`, `UVDict`/`UVEntry`, `MtxList`/`MtxInstance`, `LapData`

**Zero D3D9 tokens and zero RW types in any of these headers.**

### 2.3 The renderer-agnostic claim — TRUE as written, but read the scope
The R4 exit note (`TrackRenderer.h:22`) and `RENDERER_GATE_BRIEF.md:34-37` claim the **parsed
data** is renderer-agnostic. **Verified: it holds.** What is emphatically *not* agnostic is the
**consumer**. `TrackRenderer` bakes D3D9 into its public API, members, and vertex structs:

`D3d9Render/TrackRenderer.h` — `:25` `#include <d3d9.h>`; `:48` `Load(IDirect3DDevice9*)`;
`:66` `Render(IDirect3DDevice9*)`; `:71` `struct V{float x,y,z; D3DCOLOR c; float u,v;}`;
`:91` `LoadCar(IDirect3DDevice9*)`; `:100` `LoadCarLiveries(IDirect3DDevice9*)`;
`:142` `kFVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1`; `:145` `vector<IDirect3DTexture9*> textures_`;
`:231`/`:248`/`:303`/`:311` more `IDirect3DTexture9*` vectors; `:232` `vector<D3DMATRIX> instances`;
`:258` `LoadCopters(IDirect3DDevice9*)`; `:266`/`:273`/`:281`/`:298` `D3DCOLOR` fog/ambient/sun;
`:345` `RenderCarsRelit(IDirect3DDevice9*, const D3DMATRIX&)`.

Also D3D9-bound: `PickupField.h` (`:20,:47,:57,:75,:78`), `ParticleSystem.h` (`:16,:27,:39,:58,:65`),
`MpegVideoTexture.h` (`:14,:28,:34,:44`), `QuadRenderer.h` (`:25,:45,:83,:97,:98,:101`),
`RwIm2DBridge.h` (`:29,:39,:45`).

D3D9-**clean** and therefore reusable as-is: `TextRenderer.h`, `MashedFont.h`, `PngLoader.h`,
`MenuStringTable.h`, `DrawStreamDump.h` (they return CPU-side BGRA / strings; the caller uploads).
`RwWorldRender.h` leaks nothing — its signatures are `void*`.

**Sizing boundary: a librw swap replaces all of `D3d9Render/`'s draw layer and touches none of
`Track/`, `Txd/`, `Rws/`.**

### 2.4 Vendored RW headers — there are none
`mashedmod/deps/` contains exactly one vendored library: **qhull-2002.1**. Grep for
`rwcore|rpworld|rwplcore|RenderWare|librw` across the whole build tree → **no matches**. Every
RW-shaped struct we use is hand-rolled (including the `kRpWorldBaseSize=0x70` /
`kRpWorldSectorSize=0x88` constants in `RwWorldLoad.h:46-47`). **librw would be a genuinely new
dependency, not a swap of an existing one** — hence the approval question.

### 2.5 Build wiring
`mashedmod/build.bat` lists exe TUs **explicitly** (`:82-276`, no globbing); the .asi reads
`mashedmod/asi_sources.rsp` (366 lines, one quoted `.cpp` each). **qhull is the precedent for a
vendored subtree and it is *not* per-file listed** — it builds to a static lib via its own
`deps\qhull-2002.1\build_qhull.bat` (invoked at `build.bat:24-28`), the single bridge TU
(`Collision\QhullBridge.cpp`) compiles in isolation with a dedicated `/I` (`:35-39`), and the lib
is pulled via `/link "%QHULL_LIB%"` (`:279`, `:287`).

---

## 3. Fit analysis

### 3.1 Asset reality — Mashed ships stock RenderWare containers
Measured this session by extracting `original/TOASTART/TRACKS/Arctic.piz` and reading chunk headers:

| File | Leading chunk id | Library ID | Meaning |
|---|---|---|---|
| `GRAPH.BSP` (0x9b610) | `0x0000000B` | `0x1C02000A` | `rwID_WORLD` — the drawable world |
| `COLLISIONS.BSP` | `0x0000000B` | `0x1C02000A` | `rwID_WORLD` |
| `AI.BSP` | `0x0000000B` | `0x1C02000A` | `rwID_WORLD` |
| `SKY.DFF` | `0x00000010` | `0x1C02000A` | `rwID_CLUMP` |
| `TEXTURES.TXD` (0x11f9c4) | `0x00000023` | `0x1C02000A` | **not a stock RW texdict** |

`libraryIDUnpackVersion(0x1C02000A)` = **RW 3.7.0.2**, inside librw's declared range.
Every track `.piz` carries the same shape (`AI.BSP`, `COLLISIONS.BSP`, `GRAPH.BSP`, N×`.DFF`,
`TEXTURES.TXD`, plus the Mashed-custom `.SPL`/`.ANM`/`.UVA`/`.MTS`/`.LUA`).

The `0x23` TXD is **Mashed-proprietary** — our own `Txd/TxdDecoder.h:2` says so explicitly
(*"Mashed's proprietary chunk-id 0x23 TXD format"*, twin of `FUN_0054f8d0`), and the byte grid
after the 12-byte root header does not match a canonical `rwID_STRUCT`. librw's
`readNativeTexture` will never read these. Not a problem — see below.

### 3.2 The "librw doesn't support BSP" warning — neutralised, and here is why
`src/world.cpp` is 5.4 KB: librw's `rw::World` is an atomic/light **container**, not a BSP
sector tree, and there is no world-stream reader. Upstream is explicit: *"BSP is not supported
at all."*

**This does not bite us, because we never intended to hand librw a file.** We already have
`Track/TrackWorld.cpp` (224 LOC) turning `GRAPH.BSP` into `Track::World{materials, sectors}`
with per-sector verts/uvs/prelit/tris. The same is true of DFF (`Track/DffModel.cpp`, 382 LOC)
and TXD (`TxdDecoder`, 260 LOC). **All three formats are already ours.** The adoption shape is
therefore:

> parse with **our** loaders → **construct** librw objects in memory → render with **librw's**
> pipelines.

librw's stream layer (`clump.cpp`, `texture.cpp`, native-data readers) is simply unused. This is
a supported use of the library — `rw::Geometry::create`, `rw::Atomic::create`,
`rw::Raster::create`/`setTexels` are public construction APIs, not stream-private.

Corollary: the DFF-variant question never has to be answered. Whether Mashed's `.DFF` would
survive librw's clump reader is moot; `DffModel.cpp` already reads it.

### 3.3 Impedance mismatches — the complete list

| # | Mismatch | Shape of the bridge | Effort |
|---|---|---|---|
| I1 | `Track::Sector` (SoA float arrays + `uint32 prelit` + `uint16 tris`) → `rw::Geometry` (`morphTarget->vertices`, `colors`, `texCoords[]`, `Triangle{v[3],matId}`) + `rw::Atomic` + `rw::World` | straight repack; note RW wants `RGBA` structs not packed `uint32`, and a `MaterialList` per geometry | **M** |
| I2 | `Track::DffBatch` (model-space, frame-baked, has `normals`) → `rw::Geometry` + `rw::Frame` + `rw::Atomic`/`rw::Clump` | same repack; our batches are already frame-baked so the RW frame hierarchy collapses to identity — **this loses nothing today but forecloses skinned/animated parts** | **M** |
| I3 | `Txd::Mip{depth, stride, palette}` → `rw::Raster` via `Raster::create` + `lockTexture`/`setTexels` | needs a format map: Mashed mip `depth`/palette → RW `Raster::C8888`/`C888`/`C1555`/`PAL8`/DXT. **Unknown until we enumerate the depths actually present across all 13 track TXDs — do this first in E2'.** | **M** |
| I4 | `TrackRenderer`'s `D3DCOLOR fog_color_/amb_world_/sun_color_` + fixed-function fog → `rw::Light` (`LIGHT_AMBIENT`/`LIGHT_DIRECTIONAL`) + RW fog render-state | semantics differ (RW lighting is per-vertex in the default pipe); needs a documented delta | **M** |
| I5 | `Track::DffBatch::modulate_mat` / `lit` flags → RW geometry flags (`Geometry::LIGHT`, `Geometry::MODULATE_MATERIAL_COLOR`, `PRELIT`) | near 1:1 — these flags exist in RW for exactly this reason | **S** |
| I6 | `RwIm2DBridge` (fake RW device at `*(0x007d3ff8)`, `+0x30` slot → `DrawPrimitiveUP`) → librw `im2d` | the bridge's *purpose* was to be a stand-in RW device; librw **is** the real thing. This is a simplification, not a port. Keep the vtable shim, redirect its body to `rw::im2d::RenderPrimitive`. | **M** |
| I7 | Device ownership: we create the device (`exe_main`'s InitD3D9); librw wants to (`EngineOpenParams{HWND}`, `Direct3DCreate9`+`CreateDevice`+`Present`) | **Option A (recommended for the exe):** hand librw the HWND, delete our InitD3D9. **Option B:** patch `startD3D` to adopt a pre-made device (set `d3d::d3ddevice`, `d3d9Globals.present/window`, run the `:1192-1211` cache init) — ~30 lines, and **required only if librw ever goes in the .asi.** | **S** (A) / **S–M** (B) |
| I8 | Present + frame limiter: librw calls `Present` at `d3ddevice.cpp:1356`; our frame limiter lives in the d3d9 shim's Present | the shim wraps the *real* d3d9 export, so it still sees librw's Present. **Likely no change** — verify empirically in E1' smoke. | **S** |
| I9 | librw default D3D9 pipe is shader-based (`vs_2_0`/`ps_2_0`, `d3d9.cpp:697`); our spike is fixed-function | either accept shaders (blobs are pre-compiled, no fxc) or assign `defaultRenderCB_Fix`. Shaders are the better-exercised path (re3 ships them). | **S** (decision) |

**Not a mismatch (explicitly):** `Track/`, `Txd/`, `Rws/`, `TrackData` parsers, `TextRenderer`,
`MashedFont`, `PngLoader`, `MenuStringTable`, `LapLogic`, and all of `Render/` (the .asi leaf
clusters) are untouched by this lane.

### 3.4 Vendor strategy — **recommend: vendored snapshot, qhull-shaped**

**Snapshot, not submodule.** Reasons: (a) upstream is alive (pushed 17 days ago) and we want a
pinned, reproducible renderer, not a moving one; (b) we will carry local patches (I7-B if the
.asi ever needs it, plus any `#pragma`/warning suppressions), and a submodule makes patches
awkward; (c) the repo's only existing vendored library — qhull — is a snapshot, so this matches
convention; (d) the three existing submodules are all *read-only prior art*, a different role.

Concretely:
```
mashedmod/deps/librw/            <- tracked snapshot of aap/librw @ <pinned sha>
  LICENSE                        <- MIT, must be preserved and shipped
  PINNED_REV.txt                 <- upstream URL + sha + date + list of local patches
  build_librw.bat                <- mirrors deps/qhull-2002.1/build_qhull.bat
  rw.h, src/**
```
`re/prior_art/renderware/librw` (gitignored) stays as the pristine upstream clone for diffing
our snapshot against upstream.

**Build consumption — static lib, both targets, per the qhull precedent:**
- `build_librw.bat` → `librw.lib` (x86, `/MT`, `/DRW_D3D9`), invoked once near `build.bat:24-28`.
- Exactly **one** bridge TU per subsystem includes `rw.h`, compiled in isolation with a
  dedicated `/I` into a per-target `.obj` (mirroring `build.bat:35-39` for `QhullBridge.cpp`).
  Proposed: `mashedmod/src/mashed_re/LibRw/RwBridge.cpp` (+ `RwSceneBuild.cpp`, `RwIm2D.cpp`).
- `/link … "%LIBRW_LIB%"` added to the exe target (`build.bat:279`).
- **Both-lists rule (memory `project_asi_builds_from_rsp`): every new bridge `.cpp` must be
  listed in BOTH `build.bat` AND `asi_sources.rsp`.** librw's own 45 sources are *not* listed
  anywhere — that is the whole point of the static-lib shape.
- **.asi target: do NOT link librw initially.** The .asi is the Frida-diff harness inside
  `MASHED.exe`, which has its own RW engine and its own device. Linking librw there invites
  symbol and device conflicts for no benefit. If a bridge TU lands in `asi_sources.rsp`, guard
  it so the librw path is exe-only.

Third-party notice obligation: ship `THIRD_PARTY_NOTICES.txt` next to `mashed_re.exe` carrying
librw's MIT text (and qhull's terms while we are at it).

---

## 4. Sizing — E1'–E4'

Effort classes: **S** = one focused session · **M** = 1–2 · **L** = 3+ or needs a split.
Sessions are sized to the token-economy rule (split at phase boundaries).

### E1' — vendor + build + smoke (**S**) — *largely pre-validated this session*
1. Snapshot `re/prior_art/renderware/librw@1252b90e` (or re-pull to a fresh sha) into
   `mashedmod/deps/librw/`; write `PINNED_REV.txt`; preserve `LICENSE`.
2. Author `deps/librw/build_librw.bat` from the probe command in §1.3; wire into `build.bat`.
3. Smoke TU `LibRw/RwBridge.cpp`: `Engine::init` → `Engine::open({hwnd})` → `Engine::start`,
   create an `rw::Camera`, clear to a known colour, `Present`, exit.
4. **Acceptance:** `mashed_re.exe` boots, shows the clear colour, exits clean; frame limiter
   (I8) still caps at `MASHED_FPS_CAP`; no regression to the existing menu path (librw stays
   behind an env gate, e.g. `MASHED_RENDER_LIBRW=1`).
5. Decide I9 (shader vs fixed-function default pipe) and record it.

Risk here is low: the compile is already proven (0 errors, x86 verified). What is unproven is
`Engine::open`/`start` against our window and the shim.

> **E1' OUTCOME — DONE 2026-07-31, same session.** Snapshot landed at
> `mashedmod/deps/librw` (rev `1252b90e`, `PINNED_REV.txt`), `build_librw.bat`
> wired into `build.bat` on the qhull pattern, `LibRw/RwBridge.cpp` smoke TU
> behind `MASHED_RENDER_LIBRW=1`, `THIRD_PARTY_NOTICES.txt` added. Both targets
> build clean. Smoke result (`log/librw_smoke.txt`): librw enumerated the adapter,
> created its own D3D9 device on our HWND, presented 600 frames, and the
> **backbuffer centre pixel read back `0x2080C0` == the clear colour** — so librw
> demonstrably wrote the framebuffer, not merely returned success. Teardown clean,
> exit 0; the default D3D9 path is unregressed (12 s healthy run with the gate off).
>
> Three corrections to the recipe in §1.3, learned by doing:
> 1. **All platform subdirs must be compiled, not just `src` + `src/d3d`.**
>    `src/engine.cpp:233-238` calls `ps2::`/`wdgl::`/`gl3::registerPlatformPlugins()`
>    unconditionally; the bodies are `#ifdef`-guarded but the stub symbols still
>    have to exist. Omitting them = 30 `LNK2019` at exe link. Upstream does the
>    same (`premake5.lua:126-127` is `src/*.*` + `src/*/*.*`). `src/gl/glad/` is the
>    one exception — gl3-only, needs GLFW/SDL headers, excluded.
> 2. **`LODEPNG_NO_COMPILE_CPP`** must be defined (upstream `premake5.lua:125`).
> 3. **`WITH_D3D`** must be defined before `<rw.h>` to get `rw::d3d::d3ddevice`
>    and the D3D9-typed interface (`src/d3d/rwd3d.h:2,36`); `<windows.h>` must
>    precede it for the `HWND` form of `EngineOpenParams`.
>
> I9 (shader vs fixed-function default pipe) is still open — the smoke clears and
> presents without exercising either object pipeline.

> **E2'a TASK 0 — DONE 2026-07-31. Reference captures exist: `verify/librw_ref/`
> (10 shots + MANIFEST).** The capture driver was never broken. `RunRaceDemoStep`
> is documented at `exe_main.cpp:1029-1030` as *"paired with `MASHED_GOTO=6` so we
> start parked on the Challenge Select screen"*; the failed E1' attempt simply
> omitted that companion variable, so the driver never advanced a step, logged
> nothing, and wrote no BMP. That reads exactly like a dead driver and is not one.
> With `MASHED_GOTO=6` (plus `MASHED_RESULT_DEMO=1`, and `MASHED_DRIVE_HOLD=1` for
> the two late chase frames past t=9 s/16 s) the full set captures in two boots.
> Recipe and per-shot sha256 are in `verify/librw_ref/MANIFEST.md`.
>
> Two pre-existing deltas were recorded from the baseline so they cannot later be
> misattributed to librw: **D-REF-1** trackside banner text ("SUPERSONIC",
> "EMPIRE") renders **mirrored** on the current D3D9 path; **D-REF-2** the captured
> course is dark (means 31-36 vs 97-111 for menus), so E3' should judge shots 2-7
> on per-region grid stats rather than whole-image mean.
>
> Coverage gap carried forward: no isolated particle/weather frame and no isolated
> pickup-orb frame — both appear only incidentally in-race. Add capture hooks in
> `ParticleSystem`/`PickupField` during E2'c if E3' needs them isolated.

### E2' — feed librw from our loaders (**L — split into three**)

**E2'a — rasters (M).** First action: enumerate the mip `depth`/palette combinations actually
present across all 13 track TXDs + the vehicle TXDs (a `re/tools/` one-shot, cheap). Then the
`Txd::Mip` → `rw::Raster` bridge (I3). Acceptance: `imgdiff` of a texture-atlas dump vs the
current D3D9 path, per-channel mean-abs at the documented floor.

> **E2'a TASK 1 — DONE 2026-07-31. Census tool: `re/tools/txd_format_census.py`.**
> Walked all 42 PC TXDs / **5194 mips**. The format space is small and closed:
>
> | depth | storage | palette | mips |
> |---|---|---|---|
> | 4 | **1 byte/pixel** | 16 entries (64 B) | 1042 |
> | 8 | 1 byte/pixel | 256 entries (1024 B) | 3620 |
> | 32 | 4 bytes/pixel | none | 532 |
>
> Nothing else exists — no DXT, no 16-bit, no non-power-of-two, one `deviceId` (1),
> two chunk versions (`0x1C02000A`, `0x1803FFFF`, both already known). **I3 is
> therefore much smaller than sized: three formats, two of them the same code path
> with a different palette length.** Downgrade I3 from **M** to **S**.
>
> Two traps the census exposed, both now documented in `TxdDecoder.h`:
> 1. **`depth` is the palette-size exponent, NOT the storage width.** PAL4 is one
>    byte per pixel with only the low nibble used. Evidence: `stride == max(4, w*bpp)`
>    with `bpp==1` for depth 4 *and* 8 (0 violations / 5194), `stride*h + palette`
>    exactly fills the IMAGE chunk (total slack **0**), and no byte in any depth-4
>    mip exceeds `0x0F` — impossible if two indices were packed per byte.
> 2. **Row pitch is always `Mip::stride`; never recompute `w*depth/8`.** A 4-byte
>    minimum makes them disagree for every mip narrower than 4 texels (1973 mips).
>
> **This found a live bug in the shipping D3D9 path** (fixed same session):
> `TrackRenderer::MakeTexture` read PAL4 as packed nibbles (`row[x >> 1]`), which
> squashed every PAL4 track texture to half width and scrambled its indices.
> Proven by decoding `dump.piz::DUMP.TXD` texture 19 "Roadslipblend2" both ways —
> 1 byte/px gives a coherent asphalt-and-gravel road, nibble-packed gives vertical
> stripes. Affects Warzone (288 mips), training (139), rouabout (123), sands (96),
> City, Forest, Neustein, dump, and SFX BADGES. `PixelFormat::Paletted4` added to
> the enum (it previously fell through to `Unknown`).
>
> 17 TXD entries were skipped, correctly: they are `XBOX/` and `PS2/` console
> assets shipped inside the PC archives, with root chunk **`0x16`** — the *standard*
> `rwID_TEXDICTIONARY`, which incidentally confirms Mashed's PC `0x23` really is a
> proprietary variant. librw could read those natively; we have no reason to.

> **E2'a TASK 2 — DONE 2026-07-31. `LibRw/RwRasterBridge.{h,cpp}`.**
> `Txd::Texture` → `rw::Raster` (C8888 | TEXTURE), plus `TextureFromTxdTexture`
> wrapping it in a named, filtered `rw::Texture`. Two design decisions, both
> deliberate and both reversible:
> - **PAL4/PAL8 are CPU-expanded to C8888**, not handed to librw as paletted
>   rasters. librw maps both to `D3DFMT_P8` (`d3d.cpp:397-400`), which modern
>   GPUs no longer support — the same reason the existing D3D9 path expands
>   (`QuadRenderer.cpp:151`). Expanding also keeps one variable in the E3' diff.
> - **Base mip only**, matching the shipping path's single-level `CreateTexture`
>   and therefore the `verify/librw_ref` baseline. The full mip chain would
>   probably look better, but changing renderer *and* mip policy at once would
>   make every E3' delta ambiguous. Revisit once E3' is green.
>
> **Acceptance — two independent implementations agree on 64/64 textures.**
> `RasterBridge_SelfTest()` runs inside the live engine, builds a raster per
> texture from `dump.piz::DUMP.TXD` and `Frontend.piz::TEXTURES.TXD`, reads each
> back through `rw::Raster::lock`, and logs an FNV-1a-32 of the RGBA to
> `log/librw_raster.txt`. `re/tools/txd_format_census.py --rgba-hash` computes the
> same value in Python straight from the TXD bytes — written from the format spec,
> sharing no code with the bridge. `diff` of the two is **empty**. Coverage:
> 11 ARGB, 5 PAL4, 48 PAL8.
>
> **The check is non-degenerate**: recomputing the 5 PAL4 textures with the old
> nibble-packed interpretation changes every one of their hashes (e.g.
> `Roadslipblend2` `9617B0A6` → `3679A285`), so this test would have caught the
> bug it was written after.
>
> **What it does NOT prove** — stated so it is not over-read: the cross-check
> validates pipeline *mechanics* (stride walk, palette indexing, nibble handling,
> channel round-trip through real D3D memory). It does not independently confirm
> that depth-32 TXD bytes are RGBA-ordered; that convention is inherited from the
> existing shipped path (`QuadRenderer` / `TrackRenderer::MakeTexture`), so if it
> were wrong, old and new would be wrong identically. Only the E3' comparison
> against the *original game* can settle that.

**E2'b — static world + props (M).** `Track::World` → geometry/atomic/world (I1), `Track::DffModel`
→ clump (I2), flags (I5), camera + fog/lighting (I4). Acceptance: `imgdiff` at the E3'
viewpoints, world-only, no cars.

> **E2'c STEP 1 — DONE 2026-07-31 (code), acceptance BLOCKED by R10.**
> `Race/RaceSceneState.h` now holds the renderer-neutral simulation state;
> `TrackRenderer` derives from it. **`TrackRenderer.cpp` has ZERO changed lines** —
> the entire commit is 6 insertions / 155 deletions in `TrackRenderer.h`. Both
> targets build clean first try.
>
> Inheritance rather than composition was chosen deliberately: composition would
> have required rewriting several hundred `foo_` → `state_.foo_` references in a
> commit whose whole value is being provably a no-op. Deriving moves only
> declarations, so every existing reference resolves unchanged. This is step 1 of
> 2 — it puts the simulation in a D3D9-free type a librw submitter can take as
> `const RaceSceneState&`, but TrackRenderer still *is-a* state holder. Flip to
> composition after E2'b reveals which members a submitter actually reads.
>
> **The specified acceptance (imgdiff the 10 viewpoints ≤ the noise floor) could
> not be run: the harness turned out to be nondeterministic — see R10.** Measured
> deltas were 0.00–15.16 against baseline while two runs of the *same binary*
> differ by up to 36.74, so the test cannot distinguish a no-op from a regression.
> What stands in its place is structural, and is strong for a pure move:
> `TrackRenderer.cpp` unchanged (0 lines), moved declarations textually identical,
> clean build of both targets. What it does NOT cover is member *ordering* —
> declaration order changed, so initialisation order changed. These are
> default-initialised members with no cross-dependencies, but that is reasoning,
> not measurement. **Do not call E2'c verified until R10 is fixed and the diff
> re-run.**
>
> ### RETRACTED — the verdict below is WRONG. See "E2'c VERDICT, FINAL" underneath.
> It was reached with an inadequate control (n=1 rebuild pair) and blamed the
> refactor for what is actually build-level nondeterminism. Left in place because
> the error is instructive: the second time in one session that an under-powered
> control produced a confident wrong conclusion.
>
> ### E2'c VERDICT after R10 was fixed — **NOT a no-op. Refactor is suspect.**
> With the deterministic harness (13/13 same-binary), the pre- vs post-refactor
> A/B — same source tree, same clock, only `TrackRenderer.h` + `RaceSceneState.h`
> differing — gives **5/13 bit-identical, 8 differing**:
>
> | shot | mean-abs | explained by R10b? |
> |---|---|---|
> | 01_inrace_track | 33.59 | **yes** (control also unstable) |
> | 01_grid | 32.88 | no |
> | car_2_drive | 32.84 | no |
> | 01_action | 31.91 | no |
> | 02_back_to_menu | 16.26 | no |
> | car_3_weave | 15.70 | no |
> | car_4_chase | 13.38 | no |
> | car_5_chase | 12.45 | no |
>
> **Seven differences are not explained by the rebuild control.** The member
> declarations were verified textually identical (all 136: same names, types,
> initializers — diff shows whitespace only), and `TrackRenderer.cpp` is unchanged,
> so the only variable is member ORDER. That points at a latent dependency on
> class layout or on uninitialised memory somewhere on the in-race path — which
> would be a real defect in its own right, independent of librw. Not yet isolated;
> `memset`/`memcpy` in `TrackRenderer.cpp` were checked and none span members.
>
> **My earlier "pure move, provably a no-op" claim was wrong.** The structural
> argument (zero .cpp lines changed) was true but insufficient: it reasoned about
> what the compiler sees, not about what the program does. Measurement overruled it.
>
> Decision needed: revert the refactor, or treat the layout dependency as the
> finding and chase it. Do NOT build E2'b on top of this until it is resolved.
>
> ### E2'c VERDICT, FINAL — **the refactor is EXONERATED. R10b is the real bug.**
> The missing control was "rebuild the *pre-refactor* source and compare against
> itself". Run it and the whole picture inverts:
>
> | comparison | identical | differing shots |
> |---|---|---|
> | same binary, two runs | **13/13** | — |
> | PRE vs PRE (rebuild control) | 5/13 | `01_action 01_grid 01_inrace_track 02_back_to_menu car_2_drive car_3_weave car_4_chase car_5_chase` |
> | PRE vs POST (the "refactor" A/B) | 5/13 | **the same eight, exactly** |
> | POST vs POST (rebuild control) | 12/13 | `01_inrace_track` |
> | PRE2 vs POST2 (refactor, both rebuilt) | 8/13 | `01_action 01_inrace_track car_3_weave car_4_chase car_5_chase` |
>
> **Rebuilding identical source reproduces the entire effect I attributed to the
> refactor** — the same eight files, not a subset. The refactor is not the
> variable. My "NOT a no-op" verdict was an artefact of comparing one build
> against one other build and calling the difference causal.
>
> Note also that the two rebuild controls disagree with each other (5/13 vs
> 12/13). So the instability is intermittent *between build pairs*: a single
> "12/13" run does not demonstrate stability either. Any future control here needs
> several build pairs, not one.
>
> **R10b is therefore much larger than first logged** — up to 8 of 13 shots, not
> one — and it is the genuine latent bug: identical source, same deterministic
> clock, differing behaviour. That signature points at a read of uninitialised
> memory (or something else address/layout-dependent) somewhere on the in-race
> path; MSVC output is not byte-reproducible run to run, so small code-layout
> shifts change whatever garbage is being read. `car_1_spawn` is stable while
> `car_2_drive` onward is not, so it enters after spawn and accumulates.
>
> **E2'c stands as written** (structurally a pure move, and no longer contradicted
> by measurement), but it is still not positively *verified*, because R10b means
> no in-race shot can currently certify anything. E2'b is unblocked from the
> refactor's side; it remains blocked from the acceptance side until R10b is
> fixed — and so does E3', for the same reason.
>
> ### E2'c — **VERIFIED a no-op 2026-08-01.** R10b root-caused and reduced.
>
> R10b was never build nondeterminism. The camera and drive paths read **live
> DirectInput keyboard and mouse**, and the device is opened
> `DISCL_BACKGROUND | DISCL_NONEXCLUSIVE` — it reads the keyboard with no window
> focus. Typing in a terminal during a capture flew the camera; two "identical
> builds" produced a chase-cam frame and a high-orbit frame of the same simulation
> instant. Three live-input sites are now gated on `!g_det_clock` (`01e87920`).
>
> That also exposed a second defect in the recipe: with ambient input gone the car
> never moved (`spd=00000000` throughout), because `MASHED_DRIVE_HOLD` alone leaves
> `human_drive_` true. `MASHED_DRIVE_DEMO=1` is required. The old baseline was
> therefore pictures of a car steered by stray keystrokes; `verify/librw_ref` has
> been re-taken with the corrected recipe.
>
> **The E2'c A/B, re-run properly and WITH controls:**
>
> | comparison | identical | differing |
> |---|---|---|
> | PRE vs PRE (control) | 10/13 | `00_challengeselect 01_inrace_track 02_back_to_menu` |
> | POST vs POST (control) | 12/13 | `02_back_to_menu` |
> | **PRE vs POST (refactor)** | **11/13** | `00_challengeselect 01_inrace_track` |
>
> **Every shot differing in the A/B also differs in the control** — the refactor's
> differing set is a strict *subset* of the control's. It introduces nothing beyond
> what rebuilding the same source already produces. **E2'c is a no-op** to this
> harness's resolution, and E2'b is unblocked.
>
> **R10b residual, still open (MED):** 3 of 13 shots remain build-unstable
> (`00_challengeselect`, `01_inrace_track`, `02_back_to_menu`), down from 8. The two
> control pairs disagree (1 vs 3 shots), so it is intermittent between builds. The
> other **10 gate with a zero noise floor** — bit-identical across rebuilds — so E3'
> has a working gate today on ten viewpoints. `02_back_to_menu` is a menu screen
> with the DirectShow video backdrop running, which the frame clock cannot govern;
> that is a plausible but **[UNCERTAIN]** explanation, not yet tested.
>
> **Next diagnostic step (not yet run) — the instrument already exists.** The
> capture lambda in `exe_main.cpp` emits `RELIGHT_CAP tag=<shot> heading=%.5f` to
> `mashed_re.log` at each in-race capture (confirmed present: three lines per run,
> `01_grid` / `01_inrace_track` / `01_action`, all `heading=1.54978` in the run on
> disk). So the sim-vs-renderer question is a three-number comparison:
>
> 1. Build, run a deterministic capture, save `mashed_re.log`.
> 2. Rebuild the *same* source, run again, save the log.
> 3. Diff the `RELIGHT_CAP` headings.
>
> Headings differ → the divergence is in the **simulation**; bisect the in-race
> update path for an uninitialised read. Headings identical while pixels differ →
> it is **renderer-side** (uninitialised render state, or a texture/batch built
> from stale memory), a much smaller search. Do this before any bisect — it halves
> the space for one build cycle.
>
> Widen the trace first if needed: `car_pos_`, `car_vel_` and the AI car positions
> would localise it further at negligible cost, and unlike the heading they are
> not already collapsed through `atan2`.

> **E2'b (world + props) — GEOMETRY BUILT AND VALIDATED 2026-08-01.**
> `LibRw/RwSceneBuild.{h,cpp}`: `Track::World` → `rw::World` (I1),
> `Track::DffModel` → `rw::Clump` (I2), geometry flags (I5), textures via the
> already-verified `RwRasterBridge`. Exe-only, isolated TU.
>
> Self-test on `Arctic.piz` (`log/librw_scene.txt`):
> ```
> parsed: 13 materials, 12 sectors, total_verts=16229 total_tris=16480
> materials with a texture name: 12, resolved against the TXD: 12
> built: 9 atomics, 16229 verts, 16480 tris  (3 sectors skipped as empty)
> triangle validation: 0 out-of-range vertex indices, 0 out-of-range matIds
> ```
> Counts match the parser exactly. The content validation matters: a builder that
> mis-sliced `Track::Sector::tris` — which is `(mat,v0,v1,v2)`, stride **4**, not 3
> — would still produce plausible totals, but every `matId` would then be a vertex
> index (up to 16228) against a 13-entry material list. `badMat == 0` is what rules
> that out.
>
> **librw finding worth recording: `World::addAtomic` adds the atomic to no list.**
> It only sets `atomic->world` (`world.cpp`), and `World::render()` walks the
> **clump** list only — upstream's own comment there is *"this is very wrong, we
> really want world sectors"*. A bare atomic added to a world silently never draws.
> Sectors are therefore built as one `rw::Clump` added via `addClump`. This is the
> concrete shape of "librw's `rw::World` is a container, not a BSP" (§3.2).
>
> **Not yet done in E2'b:** nothing is *drawn* through librw yet — the scene is
> built and validated structurally, but the submit path (camera, fog/lighting I4,
> Im2D I6) is still ahead, and `TrackRenderer` remains the live renderer. Next is
> wiring `World::render()` behind the `MASHED_RENDER_LIBRW` gate and taking the
> first real E3' imgdiff against the ten gating viewpoints.

> ### E2'b step 3 — **IMPLEMENTED 2026-08-01.** In-loop, deterministic, three deltas open.
>
> The design below was followed, with one thing it had not accounted for: **librw
> owns the D3D9 device**, so "beside `g_track.Render(dev, …)`" was not expressible
> as written — there was no `dev` both renderers agreed on, and the
> `MASHED_RENDER_LIBRW` gate terminated the process *before* `InitD3D9`. Resolved
> by **device adoption** (user-decided, option A): librw is handed the exe's
> device via three documented local patches to the vendored snapshot
> (`deps/librw/MASHED_PATCHES.md` P1–P3), so one device, one `BeginScene`, one
> `Present`. The E1' probe kept its own-device behaviour and moved to
> `MASHED_LIBRW_SMOKE`; `MASHED_RENDER_LIBRW` now selects the in-loop path.
>
> **Landed:** `fog_color_`/`amb_world_`/`sun_color_` moved to `RaceSceneState` and
> retyped `D3DCOLOR → uint32_t` (§3.4's deferred retype, now closed), joined by the
> resolved-camera quartet `last_fov_/last_aspect_/last_near_/last_far_` — the
> projection constants were function-local at `TrackRenderer.cpp:3733` and nothing
> exposed them. `TrackRenderer` now builds its own D3D9 projection *from* those
> members, so there is one source of truth. New exe-only TU `LibRw/RwRaceSubmit.cpp`
> (build.bat only, librw `/I`, never `asi_sources.rsp`). The scene is built at the
> tail of `TrackRenderer::Load` from the *same* `world`/`dicts` locals — nothing is
> re-parsed.
>
> **The retype was not free**, and the compiler caught why: `D3DCOLOR` is
> `unsigned long`, `uint32_t` is `unsigned int`. Every *value* site converted
> silently; the one *pointer* site (`ParseLightsDffDirectional(…, D3DCOLOR*, …)`,
> `TrackRenderer.cpp:550`) did not, and had to be retyped too.
>
> #### Evidence — controls first, per the R10b rule
>
> | Comparison | Result | What it establishes |
> |---|---|---|
> | librw **OFF**, this build vs `verify/librw_ref/` | **0.00 on 7/7** gating shots reproduced | The state move + gate rename are a **verified no-op** on the D3D9 path. Also a *cross-build* control: a different binary reproduced the reference exactly. |
> | librw **ON**, run 1 vs run 2 (**two different builds**) | **0.00 on both shots checked** | The in-loop path is deterministic run-to-run, and the D3D9 world contributes **zero visible pixels** once librw draws (the world-skip gate changed nothing). |
> | librw **ON** vs reference | mean-abs 24.9–39.9 | **NOT a parity number** — see below. |
>
> The path demonstrably consumed real state rather than defaults:
> `log/librw_race.txt` records `fog=1[0.1..70.0]` (Arctic's `Setup_Fog(0.1, 70, …)`),
> `far=643.6` (`radius_*8`), `fov=1.0472`, and `sectors=12 mats=13 tris=16480
> verts=16229` — matching the E2'b step 2 parse exactly.
>
> #### Delta register — the 24.9–39.9 is NOT parity
>
> It measures a frame where librw draws the static world and **nothing else is
> submitted through it**. Three causes, none yet closed:
>
> ## ✅ RESOLVED — D-S3-1 and D-S3-2 both closed. One root cause, not two.
>
> **Result: librw draws the static world in-loop at 0.39–0.93 mean-abs against the
> reference across all 7 reproduced gating shots** (was 24.9–39.9), with the D3D9
> path still bit-identical (0.00) when the gate is off.
>
> | shot | librw ON vs ref | librw OFF vs ref |
> |---|---|---|
> | `01_action` | 0.56 | 0.00 |
> | `01_grid` | 0.93 | 0.00 |
> | `car_1_spawn` | 0.93 | 0.00 |
> | `car_2_drive` | 0.93 | 0.00 |
> | `car_3_weave` | 0.93 | 0.00 |
> | `car_4_chase` | 0.39 | 0.00 |
> | `car_5_chase` | 0.70 | 0.00 |
>
> **Non-degeneracy proven, not assumed:** with `MASHED_LIBRW_NODRAW=1` on the *same
> binary* the diff explodes to **19.31** (the world vanishes). So the ~0.9 is
> genuinely "librw drew this world", not "D3D9 quietly drew it and librw did
> nothing" — the failure mode that would have made a near-zero number meaningless.
>
> ### The root cause was never occlusion
>
> D-S3-1 was mis-framed for most of the session, by me, as "librw's world overdraws
> the car". Every occlusion mechanism was measured and found correct: shared depth
> **and** colour surface (pointer-identical), clip-space z identical to four
> decimals (`ndc.z=0.9709 w=1.717` from *both* pipelines for the same world point),
> `zenable=1 zwrite=1 zfunc=LESSEQUAL` before and after the draw, live per-frame
> camera. Nothing was occluding anything.
>
> The real mechanism is **bidirectional render-state leakage between two renderers
> sharing one device** — the same cause as D-S3-2, which is why fixing it fixed
> both. librw's write-back state cache and the D3D9 path each assume they own the
> device. `resyncDeviceState()` (P4) fixes the inbound direction. The outbound
> direction — state librw leaves behind, e.g. `D3DRS_ALPHABLENDENABLE` stuck on
> (measured `ablend` 0→1 across the draw) — corrupted the D3D9 draws that followed,
> **including the next frame's car and props**. That is why the car appeared to be
> occluded: it was being drawn wrongly, not covered up.
>
> Fix: an `IDirect3DStateBlock9` (`D3DSBT_ALL`) captured before the submit and
> applied after, in `RwRaceSubmit.cpp`. Not a librw patch — it lives on our side of
> the seam, and it beats restoring a curated register list that would rot the moment
> librw's pipeline changes.
>
> **Ordering is load-bearing, and I got it wrong once.** Capturing *after*
> `resyncDeviceState()` snapshots librw's state and restores that — the exact
> opposite of the intent. Measured cost of the mistake: `01_action` regressed
> 14.99 → 39.25. Capture must precede the resync.
>
> ### Instanced props / cars — **ON BY DEFAULT since 2026-08-02**
>
> Both staging regressions are closed (D-S3-7 transform inversion, D-S3-SEA missing
> UV animation). `MASHED_LIBRW_INST=0` reverts to world-only. The flip only applies
> when the librw renderer is on at all — with no env set the shipping D3D9 path
> still runs and still diffs **0.00**.
>
> Verified in three configurations on one binary:
>
> | config | gating shots | sea region ratio |
> |---|---|---|
> | A. no env (shipping D3D9) | **0.00** ×7 | 1.00 / 1.00 / 1.00 |
> | B. `MASHED_RENDER_LIBRW=1` (instances default ON) | **0.06 – 0.59** | 1.02 / 1.00 / 0.99 |
> | C. `+ MASHED_LIBRW_INST=0` (revert) | 0.04 – 0.45 | 1.00 / 1.00 / 1.00 |
>
> B reproduced the earlier explicit `MASHED_LIBRW_INST=1` run to the hundredth on
> all seven shots, which is the same-config repeat this harness requires before a
> difference may be believed.
>
> | shot | pre-UVanim | **B (default)** | C (world-only) |
> |---|---|---|---|
> | `01_grid`     | 4.65 | **0.26** | 0.32 |
> | `01_action`   | 0.59 | **0.59** | 0.45 |
> | `car_1_spawn` | 7.12 | **0.26** | 0.31 |
> | `car_2_drive` | 4.64 | **0.26** | 0.32 |
> | `car_3_weave` | 6.95 | **0.58** | 0.37 |
> | `car_4_chase` | 0.06 | **0.06** | 0.04 |
> | `car_5_chase` | 1.77 | **0.41** | 0.29 |
>
> The instanced path is now at or below the world-only path on four of seven shots
> — drawing props and cars through librw as well as the world is closer to the
> reference than drawing the world alone was.
>
> #### How UV animation is carried over (no librw patch needed)
>
> The D3D9 path scrolls with a texture transform. That lever does not exist here:
> librw's d3d9 shader pipe passes `input.TexCoord` through untouched
> (`default_VS.hlsl:26`) and ignores `D3DTSS_TEXTURETRANSFORMFLAGS`, and librw's
> UVAnim plugin is stream-only — nothing under `src/d3d/` references it. So the
> **coordinates themselves are moved**, entirely on our side of the seam:
>
> - `BuildClump` now reports each atomic's material index as it creates the atomic.
>   Batch index is NOT atomic index — empty batches are skipped and produce none —
>   so re-deriving the mapping by re-walking `model.batches` would drift.
> - `RaceSubmit_RegisterModel` takes the per-material `(du,dv)` rates and snapshots
>   the authored UVs of each scrolling atomic. Every frame the UVs are re-derived
>   from that base as `base + fmod(rate*t, 1)` — **absolute, never accumulated**,
>   matching the D3D9 formula exactly, so the two cannot drift apart over a run.
> - `RaceSubmit_SetAnimTime(t)` is fed the *same* `t` `TrackRenderer::Render`
>   scrolls with. A private clock would put the renderers at different phases,
>   which is indistinguishable from a shading bug — which is how this read for
>   several rounds. It also honours `MASHED_NO_UVSCROLL`, so that kill-switch still
>   disables both paths together and remains usable as the control that found this.
> - Cost is one texcoord re-upload per animated atomic per frame, not a full
>   re-instance: `lockedSinceInst = LOCKTEXCOORDS` makes `d3d9.cpp:416` call
>   `instanceCB(reinstance=1)`, where the vertex/prelight/normal blocks (`:588`,
>   `:598`, `:627`) are each guarded by their own lock bit and skipped.
>
> Registration is confirmed live rather than assumed: `uvanim=1` on exactly
> `model[0]` (sky) and `model[4]` (sea), 0 on the other seven.
>
> **Historical note — what the staging block used to say:**
>
> What landed: `RaceSubmit_RegisterModel` / `AddInstance` / `BeginTrackLoad`.
> Registration happens at load time from the live `DffModel` locals — inside
> `load_prop`'s lambda and `LoadCar` — because **nothing in the codebase retains a
> parsed `DffModel`**; every one is a function local destroyed right after
> `BuildDffBatches`. Each owning struct keeps an `int rw_model` handle; **-1 means
> "keep drawing through D3D9"**, so the port is incremental by construction and one
> bad model cannot black out the scene. Instance transforms are the *same*
> `D3DMATRIX` the D3D9 path would pass to `SetTransform(D3DTS_WORLD)` — `rw::Matrix`
> and `D3DMATRIX` agree field-for-field (right/up/at/pos), so nothing is re-derived.
>
> Measured with it on: 9 models registered (the 71-atomic one is the car), 27
> instances/frame, props visibly drawing through librw.
>
> **Two open regressions:**
> - **D-S3-6 — a large ground/sea surface renders black.** ⚠️ **This entry was
>   wrong twice; the corrections are the useful part.**
>   1. First written as "the static world goes black, caused by `World::addCamera`".
>      **Both halves were wrong.** `addCamera` only sets `cam->world`
>      (`world.cpp:69-75`); binding it unconditionally while props stayed on D3D9
>      left the gating shots **unchanged at 0.93/0.39/0.93**, which exonerates it.
>      It is also *required* — `lightingCB_Shader` derefs `engine->currentWorld` for
>      `rw::Geometry::LIGHT` geometry (`d3drender.cpp:357-359`), a hard **segfault**
>      without it — so it is now unconditional.
>   2. Then suspected as "all props render black, because `World::enumerateLights`
>      skips lights without `LIGHTATOMICS` (`world.cpp:162-163`) and `Light::create`
>      leaves flags 0". The flag fix **is correct on its own terms** and is kept
>      (0x3 = `LIGHTATOMICS|LIGHTWORLD` is also the asset-verified Arctic value),
>      but it produced **bit-identical output** — so it is not the cause either.
>
>   3. Suspected an unresolved texture. **Instrumented `BuildClump` and disproved
>      that too.** The log (`log/librw_scene.txt`) is now definitive about what the
>      instanced set actually contains — 8 props plus the car:
>
>      | clump | texture | resolved? | nv | prelit | normals | lit |
>      |---|---|---|---|---|---|---|
>      | 0 | `sky` | yes | 258 | 1 | 0 | 0 |
>      | 1,2 | `Tyres` | yes | 192 | 0 | 1 | 1 |
>      | 3 | `Crate02` | yes | 36 | 0 | 1 | 1 |
>      | **4** | **`sea`** | **yes** | **864** | **1** | **0** | **0** |
>      | 5,6 | `Vehicles` | yes | 993 / 351 | 1 | 0 | 0 |
>      | 7 | `CamManCold` | yes | 531 | 1 | 0 | 0 |
>      | 8 | (car) | 44 of 71 named | — | 0 | 1 | 1 |
>
>      **Every prop texture resolves.** The black surface is `clump[4]`, the **sea**
>      — and its flags are *identical* to the banner props (`clump[5]/[6]`,
>      `prelit=1 normals=0 lit=0`) which render correctly. So it is neither a
>      missing texture, nor lighting, nor the prelit/LIGHT flag split.
>
>   Also ruled out: **texture addressing**. librw's `Texture::create` already
>   defaults to `(WRAP<<12)|(WRAP<<8)|NEAREST` (`texture.cpp:279`), so a large plane
>   with out-of-range UVs is not clamping to a black edge texel. (That line does
>   reveal a separate delta: librw defaults to **NEAREST** filtering where the D3D9
>   path sets `LINEAR` — a quality difference to fold into the E3' register.)
>
>   4. **Instance counts measured. The sea IS submitted and IS drawn — it is too
>      dark, not missing.** Per-model counts at f400:
>      `model[0] sky inst=0` (drawn by the separate `sky_` path, never instanced),
>      `model[1] inst=5`, `model[2] inst=9`, `model[3] inst=5`,
>      **`model[4] sea inst=1 @ (0,0,0)`**, `model[5..7] inst=1 each @ (0,0,0)`,
>      **`model[8] car inst=4`**.
>
>      Sampling the black region: **(2.9, 4.2, 4.0)**, versus the clear colour
>      `fog_color_` = (24,28,40) and the baseline's (12.5, 19.0, 21.7) in the same
>      region. It is **not** the background showing through — geometry is drawn
>      there, about **4–5× too dark**. Culling and "never drawn" are both dead.
>
>      ⚠️ **This also corrects D-S3-7.** The car reports **4 instances** (player +
>      3 AI), so it IS submitted. The earlier claim that `car_via_rw` was false was
>      inferred from a constant instance total of 27 — which already *included* the
>      car. D-S3-7 is very likely not a separate bug but the same too-dark shading.
>
>   5. **CONFIRMED, and largely fixed.** The two vertex colours settle it. The raw
>      prelit `BuildClump` receives for the sea is `0xFF0C0E0B` = **(12,14,11)**;
>      the track ambient is **(51,77,77)** (`0xFF334D4D`, logged by TrackRenderer);
>      so what the D3D9 path bakes is **(63,91,88)** — a **5.3–8×** brightening,
>      against a **4.3–5.4×** deficit measured on screen. The arithmetic matches.
>
>      Fix: `BuildClump` now takes the ambient and folds it into the prelit of
>      batches that are **prelit-but-not-LIGHT** — exactly the set librw's lighting
>      cannot reach (no normals → no `LIGHT` flag → `setAmbient(black)`). `LIGHT`
>      batches are left alone because they *do* receive the `rw::Light` ambient, and
>      baking it there would double-count.
>
>      Measured effect (instances ON, vs reference): `car_1_spawn` 15.41 → **10.13**,
>      `car_2_drive` 9.96 → **7.36**, `01_grid` 9.74 → **7.31**, `car_3_weave`
>      10.27 → **9.32**, `car_5_chase` 4.07 → **3.58**. The sea now renders as a
>      textured surface instead of near-black.
>
>      **D-S3-7 — CLOSED. `MakeAtomic` had its arguments inverted.**
>      `Frame::addChild(child)` makes `this` the PARENT (`frame.cpp:87-99`), so
>      `f->addChild(parent)` hung the clump root off the atomic instead of the
>      atomic off the root. Moving the clump's frame moved a *child*; the atomic's
>      own frame stayed at identity, and every instanced model drew at the world
>      origin no matter what transform was submitted.
>
>      Measured directly, which is what found it after reasoning had failed:
>      `clumpframe=(-25.21,0.04,15.78)` while `atomicLTM=(0.00,0.00,0.00)`. The
>      preceding clue was a **null experiment** — `MASHED_LIBRW_LIFT=4` raised every
>      instance and produced a **bit-identical** frame, proving the submitted
>      transform reached nothing.
>
>      This is a **pre-existing E2'b step 2 defect**, not new. It was invisible
>      because the only consumer was the static world, whose frame is identity —
>      wrong and right parenting are indistinguishable at identity. It surfaced the
>      moment something had to move.
>
>      After the fix (instances ON, vs reference): `01_action` 1.77 → **0.71**,
>      `01_grid` 7.31 → **5.51**, `car_1_spawn` 10.13 → **8.33**, `car_2_drive`
>      7.36 → **5.56**, `car_3_weave` 9.32 → **7.64**, `car_4_chase` 1.57 →
>      **0.41**, `car_5_chase` 3.58 → **2.24**. **The player car now renders through
>      librw**, correctly positioned and oriented, and props sit at their real
>      placements instead of the origin.
>
>      **Colour cast — CLOSED. Channel order.** `amb_world_` is `0x00RRGGBB`, but
>      `DffModel` prelit is RW-native RGBA bytes (`0xAABBGGRR`) — `FillVertexData`
>      reads red from the LOW byte and blue from bits 16-23. The first ambient bake
>      used the ARGB layout for *both*, so it added the ambient's RED to blue and
>      its BLUE to red; with Arctic's (51,77,77) that pushed red up and blue down.
>      Each side is now unpacked in its own convention.
>
>      Measured on the sea region (mean RGB):
>
>      | | R | G | B |
>      |---|---|---|---|
>      | baseline D3D9 | 11.1 | 17.7 | 20.6 |
>      | librw, ARGB bake (olive) | 21.9 | 25.7 | 20.0 |
>      | librw, channel fix | 15.8 | 25.9 | 28.2 |
>
>      The hue ordering now matches the baseline (blue > red = teal, not olive).
>      Shots: `01_grid` 5.51 → **5.24**, `car_1_spawn` 8.33 → **7.88**,
>      `car_2_drive` 5.56 → **5.28**, `car_5_chase` 2.24 → **2.18**.
>
>      **Residual is brightness, not hue** — librw runs a uniform ~1.4× hot
>      (15.8/11.1, 25.9/17.7, 28.2/20.6). That is consistent with the already
>      registered **I4 fog delta**: RW ties the fog END to the camera far plane, so
>      at this distance librw applies less fog than D3D9's `fog_end_`=70 ramp and
>      the surface stays brighter. Consistent, **not proven** — closing it means
>      giving the librw path an equivalent fog ramp. `[UNCERTAIN]`
>
>   **Superseded hypothesis (kept for the record):** the D3D9 path bakes the ambient
>   the vertex colours at build time (`BuildDffBatches` takes an `AtomicLight`, and
>   `TrackRenderer`'s own note says the dim baked prelight "is meant to be combined
>   with this ambient at render — without it the world is a dark void"). The librw
>   path feeds `BuildClump` the **raw** `DffModel` prelit with no ambient added, and
>   `lightingCB_Shader` then sets ambient to **black** for non-LIGHT geometry. That
>   would produce exactly a uniform darkening. **The objection I cannot yet answer:
>   the static world is also prelit and non-LIGHT and renders correctly at 0.93.**
>   Until that is reconciled the hypothesis is not established. `[UNCERTAIN]`
>
>   Next step: compare a single sea vertex's colour as the D3D9 path bakes it
>   against what `BuildClump` uploads — one number each, no rebuild-and-guess.
> - **D-S3-7 — the player car does not appear** on the instanced path. The instance
>   count sits at a constant 27 with no car entry, so `car_via_rw` is evaluating
>   false; whether `rw_car_model_` or `car_ready_` is the reason is **not yet
>   measured**. `[UNCERTAIN]`
>
> Also logged: **D-S3-5** — the librw car path bypasses `RenderCarsRelit`, the
> per-frame world-space sun relight (`MASHED_RPLIGHT`, default ON and active on
> Arctic). Through librw the body carries baked prelight plus the `rw::Light`
> terms instead of the ported per-vertex N·L. A real visual delta, to be closed by
> giving the librw path an equivalent relight pass.
>
> ### Residual — I4 fog CLOSED; the fog hypothesis for the SEA is DISPROVED (2026-08-02)
>
> The premise that "RW ties fog to the far plane" is a *design* property of RW was
> wrong. Fog on this path is not fixed-function at all: the `D3DRS_FOGSTART`/
> `D3DRS_FOGEND` writes are commented out (`d3ddevice.cpp:1285-1286`), and what is
> actually used is a vertex-shader constant `fogData=(start,end,range,disable)` at
> c14 (`:1287-1296`), read by `default_VS.hlsl:48` as
> `clamp((Position.w - fogEnd)*fogRange, fogDisable, 1.0)`. `beginUpdate` merely
> *populates* `end` from `cam->farPlane`, under an upstream comment reading
> `// TODO: figure out where this is really done`. An unfinished upstream detail,
> not a constraint.
>
> Fix: **patch P6** — `rw::d3d::setFogRange(start, end)`, called immediately after
> `Camera::beginUpdate()` (which would otherwise overwrite it). The projection is
> still built from the true `farPlane`, so the depth encoding is untouched.
>
> *Shortening the far plane was rejected*, not merely unchosen:
> `proj[10]=far/(far-near)`, `proj[14]=-near*proj[10]` (`:1270-1279`), so 643.6→70
> rewrites the depth values librw writes into the depth buffer it **shares** with
> the exe's D3D9 path (P5) — librw-drawn cars/props would then occlude-test wrongly
> against D3D9-drawn particles and pickups. It would also hard-clip world geometry
> past 70 instead of fogging it. Fixing fog by breaking Z is not a trade worth making.
>
> Measured on **one binary** with an env-gated control (`MASHED_LIBRW_FOGFIX=0`), so
> this is a true A/B and not a rebuild artefact. The control reproduces the
> previously recorded numbers to the hundredth on 6 of 7 shots — which validates the
> harness end to end:
>
> | shot | world-only OFF | world-only ON | INST OFF | INST ON |
> |---|---|---|---|---|
> | `01_grid`     | 0.93 | **0.32** | 5.24 | **4.65** |
> | `01_action`   | 0.56 | **0.45** | 0.71 | **0.59** |
> | `car_1_spawn` | 0.93 | **0.31** | 7.88 | **7.12** |
> | `car_2_drive` | 0.93 | **0.32** | 5.28 | **4.64** |
> | `car_3_weave` | 0.93 | **0.37** | 7.55 | **6.95** |
> | `car_4_chase` | 0.39 | **0.04** | 0.41 | **0.06** |
> | `car_5_chase` | 0.70 | **0.29** | 2.18 | **1.77** |
>
> 7/7 improved. World-only **0.39–0.93 → 0.04–0.45**, reproduced identically across
> two separate builds and runs. So about half the world-only residual *was* fog —
> the hypothesis was right about the world.
>
> **It was wrong about the sea.** Closing the ramp moved the sea region the WRONG
> way: 1.52/1.57/1.46 → **1.58/1.59/1.47**. The direction settles it: a shorter ramp
> applies *more* fog, fog colour is (24,28,40), and the D3D9 baseline is
> (11.1,17.8,20.7) — **darker than the fog colour**. Blending harder toward
> (24,28,40) can only brighten. Less fog therefore cannot be what made the sea 1.4×
> bright. The `[UNCERTAIN]` fog attribution is retired: the instanced sea's
> brightness is an **unidentified, still-open cause**, not an I4 consequence.
>
> > Sea region = `car_2_drive`, box (0,240)-(240,480) = 8x6 grid cells (0,3) span
> > 3x3. The box was never written down; it was recovered by scanning the reference
> > for the region reproducing the recorded (11.1,17.7,20.6) baseline, which it does
> > to 0.07. Recorded here so the next measurement is comparable.
>
> ### D-S3-SEA — ROOT-CAUSED. The librw instanced path applies no UV ANIMATION.
>
> Not lighting, not the bake, not fog, not filtering, not blending. **The sea is
> drawn with static UVs.** The D3D9 path scrolls each material's UVs per frame via
> a texture transform (`mat_scroll` / `uv_rate`, F3; applied at
> `TrackRenderer.cpp:3839-3843` and `:3968-3980`). Nothing in `LibRw/` carries
> `uv_anim` — `RwBridge.cpp:63` registers the UVAnim *plugin* and that is all. The
> scroll phase decides where the wave highlights land, and that sets the region mean.
>
> **Proved by positive control**, which is what makes this different from the earlier
> guesses: disabling UV scroll on the *shipping D3D9 renderer* (`MASHED_NO_UVSCROLL=1`)
> reproduces the defect on the reference itself.
>
> | sea region | R | G | B |
> |---|---|---|---|
> | D3D9 baseline | 11.1 | 17.8 | 20.7 |
> | **D3D9, `MASHED_NO_UVSCROLL=1`** | **17.4** | **28.2** | **30.5** |
> | **librw instanced** | **17.6** | **28.2** | **30.3** |
>
> The control lands within 0.2 of librw on every channel, closing **97%** of a
> (6.5,10.4,9.6) gap. It is also non-degenerate: it moves all 7 shots hard
> (`01_action` 0.59→5.67, `car_4_chase` 0.06→8.13), so UV animation is a large
> general term for the instanced path, not a sea-only curiosity.
>
> The elimination chain behind it, each step measured or cited, so none is re-tried:
>
> | candidate | verdict | evidence |
> |---|---|---|
> | ambient bake divergence | **exonerated** | D3D9 bakes `0xFF3E5B59` = (62,91,89); librw uploads `0xFF595B3E` = (62,91,89). **Bit-identical.** This is the per-vertex comparison the previous entry asked for. |
> | `amb_world_` vs `amb_f_` quantisation | ruled out | `amb_world_` is a quantised mirror of `amb_f_` (`TrackRenderer.cpp:1032`) |
> | ambient double-counted by librw | ruled out | `lightingCB_Shader` takes `setAmbient(black)` + `setNumLights(0,0,0)` for non-LIGHT geometry (`d3drender.cpp:357-364`); sea is `lit=0` |
> | texture decode / PAL4 nibble bug | ruled out | both decoders identical (`RwRasterBridge.cpp:45-67` vs `TrackRenderer.cpp:363-395`); sea is depth 8 anyway |
> | alpha blending | ruled out | decoded sea alpha is `[255..255]` — fully opaque, nothing to blend |
> | texture filtering | ruled out | both LINEAR, with a non-degeneracy control (above) |
> | fog | **disproved** | closing the ramp moved it the *wrong way* |
>
> **FIXED same session.** The scroll is carried over by moving the texture
> coordinates (librw's shader pipe has no texture matrix) — see "How UV animation
> is carried over" above. Sea region went 1.58/1.59/1.47 → **1.02/1.00/0.99**, and
> the gating shots 0.06–7.12 → **0.06–0.59**. `MASHED_LIBRW_INST` is now ON by
> default.
>
> ### Residual itemised per region (world-only, post-fog-fix)
>
> "~0.9, not itemised, probably fog + lighting" is superseded. After the fog fix the
> residual is **not diffuse** — it is one localised object plus a faint sky band. On
> `01_action`, **43 of 48 grid cells are exactly 0.0** and only 1.73% of pixels
> exceed threshold 16. The pattern is stable across shots:
>
> - **rows 3–5 (the whole lower half — ground and sea): 0.0.** Bit-identical.
> - **one horizon-band object** — cells (1,2)/(2,2) in the car shots (3.3–6.4),
>   (6,3)/(6,4) in `01_action` (7.6/5.1). Different screen position per camera, so a
>   world object seen from different angles, not a screen-space artefact.
> - **faint sky/horizon band**, row 1, ~0.3–0.9.
>
> Cropped side by side, D3D9 renders that surface smooth and librw renders it sharp
> and speckled. **The obvious mip-level explanation is disproved:** both uploaders
> are single-level — `RwRasterBridge.cpp:73` takes `tex.mips[0]` only, and the D3D9
> track path calls `CreateTexture(w,h,1,...)` (`TrackRenderer.cpp:358`), also
> `Levels=1`.
>
> #### It is TWO residuals, not one (2026-08-02)
>
> Separated by turning fog off on **both** paths and re-diffing against a matching
> no-fog D3D9 reference. (`MASHED_NO_FOG` only gated the D3D9 device state and left
> `st.fog_on_` set, so librw kept fogging — a one-sided switch is useless as a
> control and actively misleading. Now honoured on both paths.)
>
> | shot | fog ON both | fog OFF both |
> |---|---|---|
> | `car_1_spawn` | 0.31 | **0.03** |
> | `car_3_weave` | 0.58 | **0.34** |
> | `01_action`   | 0.59 | 0.58 (hotspot 7.6 → **7.7**, unchanged) |
>
> **1. Fog model — identified.** D3D9 uses per-pixel table fog
> (`D3DRS_FOGTABLEMODE = D3DFOG_LINEAR`); librw computes the fog factor in the
> VERTEX shader and interpolates it (`default_VS.hlsl:48` → `default_PS.hlsl`
> `lerp`). This accounts for **all** of `car_1_spawn` (0.31 → 0.03, 0.04% of pixels
> left over threshold) and the faint sky/horizon band everywhere. Closing it means
> moving the fog computation into the pixel shader, which requires recompiling
> librw's HLSL — the shaders are vendored as precompiled bytecode headers, so that
> needs `fxc` and is a **toolchain decision, not a code change**. Not taken here.
>
> **2. The snow-bank hotspot — still unidentified**, and fog is now off its list:
> the 7.6/4.6/5.1 cells are 7.7/4.7/5.2 with fog disabled on both sides. It is world
> geometry (identical cells with instances on and off), warm-weighted
> (R=0.99 / G=0.61 / B=0.16), ~2.3% of pixels, 42 of 48 cells still exactly 0.0.
>
> Eliminated for it so far, each measured or cited — do not re-try:
>
> | candidate | verdict | evidence |
> |---|---|---|
> | mip level | ruled out | both uploaders `Levels=1` |
> | UV animation | ruled out | `world=0` scrolling materials on Arctic (F3 log) |
> | texture resolution | ruled out | world 12/12 named materials resolve |
> | per-material triangle assignment | ruled out | **13/13 tallies identical** across the two builds |
> | vertex colours | ruled out | both render `s.prelit` raw — `TrackRenderer.cpp:1082-1088`, `RwSceneBuild.cpp:174` |
> | texture addressing | ruled out | TXD says `addrU=addrV=1` (WRAP) on every texture; D3D9 hardcodes WRAP |
> | filtering | ruled out | both LINEAR, with a non-degeneracy control |
> | alpha test / blend | ruled out | lowest alpha in any texture is 97, D3D9's `ALPHAREF` is 0x30=48 — nothing is discarded; blending off both |
> | material colour on untextured `mat[6]` | ruled out | librw gates matCol on `Geometry::MODULATE` and uses **white** otherwise (`rwd3d.h:321-328`), matching D3D9's `SELECTARG2` |
> | fog | **ruled out** | hotspot unchanged with fog off on both paths |
>
> **UVs and mesh ordering — both examined, both ruled out (2026-08-02).**
>
> - **UVs**: copied raw on both paths, no transform. `FillVertexData` writes
>   `uvs[i*2+0]/[i*2+1]` straight into `texCoords[0]` (`RwSceneBuild.cpp:105-108`);
>   the D3D9 world build does the same (`TrackRenderer.cpp:1092-1093`). No flip,
>   no scale.
> - **Draw order / coplanar depth ties**: there IS a real ordering difference —
>   D3D9 accumulates `batches_[mat]` across all sectors (**material-major**) while
>   librw gives each sector its own geometry and draws matIds 0..n within it
>   (**sector-major**, `geometry.cpp` `buildMeshes`) — and neither sets
>   `D3DRS_ZFUNC`, so both run D3D9's default `LESSEQUAL`, under which the last
>   draw wins a depth tie. But reversing this path's material order
>   (`MASHED_WORLD_REVORDER=1`) produces a **bit-identical frame, 0 pixels over
>   threshold**. The gate logs `D-S3-BANK: world material order REVERSED` when it
>   fires, because a reversal that changes nothing and a gate that never fired
>   produce the same 0.00 — the log is what separates them. So the world holds no
>   coplanar surfaces whose winner depends on order, and the ordering difference,
>   though real, has no visual consequence.
> - Also ruled out: a **second texture stage** (no `SetTexture(1,...)` or stage-1
>   state anywhere in `TrackRenderer`), and a **silhouette/occlusion flip** — the
>   diff heatmap is a filled area over the whole slope face, not a thin edge.
>
> **Numeric characterisation, for whoever picks this up.** In the hot cell librw
> loses red and green but keeps blue *exactly*, and cold cells match to 0.1:
>
> | cell | D3D9 | librw | ratio |
> |---|---|---|---|
> | (6,3) hot | (105.1, 108.0, 87.6) | (91.5, 100.1, 88.7) | 0.87 / 0.93 / **1.01** |
> | (7,3) | (87.7, 110.6, 110.2) | (79.6, 105.6, 110.4) | 0.91 / 0.96 / **1.00** |
> | (0,0) cold | (136.3, 116.7, 96.8) | (136.3, 116.7, 96.8) | 1.00 / 1.00 / 1.00 |
>
> It is a channel-dependent warm deficit over one slope face — **not** a uniform
> scale (which would hit all three channels) and **not** a hue swap (which would
> move blue too).
>
> **Refined over differing pixels only** (the grid cell above averages slope and
> sky together, so it describes a mixture, not a surface):
>
> | mask | pixels | deficit (D3D9 − librw) | R:G | B:G |
> |---|---|---|---|---|
> | diff ≥ 8 | 13 055 (4.25%) | (19.3, 12.6, **0.3**) | 1.53 | **0.02** |
> | diff ≥ 24 | 3 886 (1.26%) | (31.5, 21.7, **2.0**) | 1.45 | 0.09 |
>
> The term is **additive** — blue is untouched (ratio 0.997), so it is not a
> multiplicative scale — and its hue is **R:G ≈ 1.5 with essentially no blue**,
> ≈ (255, 167, 3).
>
> **The sun is DISPROVED against it.** `sun = 0xFF99B3B3` = (153,179,179),
> R:G = **0.855** with B = G — a cool teal. The ambient (51,77,77) is R:G = 0.66,
> also cool with B = G. Both are opposite in shape to a red-dominant, blue-free
> deficit, not merely different in magnitude. Independently, the D3D9 world path
> applies **no runtime light at all** — it renders `s.prelit` as-is
> (`TrackRenderer.cpp:1082-1088`) — so there is no light term on that side for
> librw to be missing.
>
> #### ANSWERED: librw draws `Snow` where D3D9 draws `World`
>
> Settled by making both renderers output a **material-ID map** instead of shading
> (`MASHED_WORLD_MATID=1`, both paths, flat colour `(20 + i*18, 200, 60)` so the
> index reads back as `(R-20)/18`), then reading ownership off the pixels rather
> than inferring it from hue. In the hot region (x 480–640, y 240–400), **32.3% of
> pixels disagree about which material owns them**:
>
> | D3D9 draws | librw draws | pixels |
> |---|---|---|
> | `World` (mat 5) | **`Snow`** (mat 4) | 6 724 |
> | untextured (mat 6) | **`Snow`** (mat 4) | 1 551 |
>
> The hues corroborate it: `World` is warm (106.4, 95.3, 86.5), `Snow` is cool
> (197.0, 196.3, 206.1) — warm-in-D3D9, cool-in-librw, exactly the measured
> deficit. This is a **depth-resolution disagreement on near-coplanar surfaces**,
> not a shading bug: nothing about how either renderer *shades* differs, only
> which surface survives the depth test.
>
> It is specifically NOT draw order — reversing D3D9's material order is
> bit-identical (above), so within D3D9 `World` genuinely wins on depth rather than
> by drawing last.
>
> #### The z-precision theory is DISPROVED (2026-08-03)
>
> Two independent grounds:
>
> **1. The matrices agree to float rounding.** The DEPTHPROBE now compares the two
> *combined* transforms element-wise, which settles it for every point at once
> rather than by sampling positions (both transforms are linear):
> `max|d3d9−librw| = 3.8e-06` (in the translation row), **maxrel = 2.7e-07**, and
> NDC z differs by ~1.8e-07. For scale: depth precision goes as
> `near·far/((far−near)·z²)`, so at the probe point (w = 1.72) that 1.8e-07 is
> ~1e-5 world units — nothing — while at the horizon distances where the bank sits
> (z ≈ 200–400) it is **0.14–0.6 world units**. So a near-tie *could* in principle
> be flipped at that range. It is not what is happening, because:
>
> **2. The disagreement is one solid surface, not a marbled interleave.** A
> float-rounding near-tie z-fights: many small components, high boundary fraction.
> Measured on the material-ID maps, the disagreeing pixels form **1 connected
> component holding 100% of them, with only 4.4% boundary pixels** — a clean
> contiguous region. That is the signature of a structural difference, not of
> depth-test noise.
>
> Also ruled out here: **backface culling** — the D3D9 world pass sets
> `D3DCULL_NONE` (`TrackRenderer.cpp:3814`) and librw sets `D3DCULL_NONE` once at
> device init and never changes it (`d3ddevice.cpp:1793-1794`); the live probe
> reads `cull=1` (NONE) and `zfunc=4` (LESSEQUAL) on both.
>
> So: identical geometry, matrices agreeing to rounding, identical cull and depth
> func — yet one whole surface's worth of pixels is owned differently. The
> remaining explanation is that the two disagree about **where `Snow` rasterises**,
> not about who wins a depth test. That is the next thing to measure.
>
> > **A failed probe, recorded so it is not trusted or repeated.** An attempt to
> > isolate coverage with `MASHED_WORLD_ONLYMAT=N` (draw only material N on both
> > paths) was **removed rather than kept**: a run that should have drawn only
> > `Snow` still showed `World` and the untextured material on the D3D9 side, so
> > the filter was not doing what it claimed, and only one world draw loop exists
> > to explain it. No conclusion was drawn from it. Anyone re-attempting coverage
> > isolation should verify the filter with a liveness check first — the same
> > discipline the REVORDER probe needed.
>
> `[UNCERTAIN]`
>
> Worth settling before treating this as a defect: whether the overlap is authored
> (the original may z-fight here too), in which case librw's answer is a different
> valid resolution of ambiguous geometry and this becomes a documented delta rather
> than a bug.
>
> ### Texture filtering (NEAREST vs LINEAR) — NOT a delta. Closed.
>
> Registered on the strength of librw's `Texture::create` default
> (`texture.cpp:279`, `NEAREST`). But `TextureFromTxdTexture` overwrites that
> default from the TXD, and Arctic's TXD already asks for LINEAR. Forcing LINEAR
> (`MASHED_LIBRW_LINEAR=1`) gave a **bit-identical** frame on all 7 gating shots.
>
> That alone proves nothing — it is exactly what a dead override looks like — so a
> **non-degeneracy control** was run: `MASHED_LIBRW_LINEAR=2` forces NEAREST and
> moves every shot (`01_grid` 4.65→5.00, `car_1_spawn` 7.12→7.50, `car_5_chase`
> 1.77→2.74). The override reaches the draw; the textures were already LINEAR.
> Worth **0.00** of the E3' residual on Arctic. Probe kept for other tracks.
>
> ---
>
> **Original log of the investigation follows — the wrong turns are the useful part,
> and the disproved hypotheses are recorded so they are not re-tried.**
>
> - **D-S3-4 — the librw view was horizontally MIRRORED. FIXED.** `beginUpdate`
>   builds the view matrix with the X component of every basis row negated
>   (`d3ddevice.cpp:1229-1240`), so handing it a plain D3D-`LookAtLH` basis renders
>   the scene flipped. Cancelled by negating `right` in the camera frame.
>   **Measured, not argued:** mirroring the captured image dropped mean-abs against
>   the D3D9 control 25.37 → 15.41, and the real fix then gave 14.31.
>   ⚠️ **The E2'b step 2 probe (`RwSceneBuild.cpp RenderWorldProbe`) builds its
>   basis the same un-compensated way, so `verify/librw_e2b/world_probe_arctic.png`
>   is mirrored too.** It was only ever checked structurally, never against a
>   reference — a concrete instance of "structural validation is not visual
>   validation".
> - **D-S3-2 — render-state leakage. CAUSE CONFIRMED, partially fixed.** Fixing the
>   direction *into* librw (patch P4, `resyncDeviceState`) restored the fog and
>   lighting. The direction *out of* librw is still open: the isolation control
>   below shows the HUD pips and the yellow lap digit are correct until librw
>   submits, so librw leaves state that corrupts the D3D9 draws that follow it.
> - **D-S3-1 — the player car is occluded. STILL OPEN, but now precisely scoped.**
>   The decisive test was `MASHED_LIBRW_NODRAW=1`, which runs the entire path
>   (engine, resync, camera, fog, lights, scene build) and submits nothing: **the
>   car draws perfectly, and so does the HUD.** So D3D9 draws both correctly and
>   librw's world draw is what destroys them.
>   Four hypotheses were tested and **all four disproved by evidence**, which is
>   worth recording so they are not re-tried:
>   1. *Depth encoding differs.* No — RW's `proj[10]=far/(far−near)`, `proj[11]=1`,
>      `proj[14]=−near·far/(far−near)` is algebraically identical to
>      `MatPerspectiveFovLH`, and the view-window mapping matches too.
>   2. *librw uses a private depth buffer.* Not here — `rasterCreateZbuffer`
>      (`d3d.cpp:499`) shares `defaultDepthSurf` when the raster matches the client
>      rect, which it does at 640×480. Forcing the binding (P5) produced
>      **bit-identical** output. P5 is kept only as a guard for the borderless case.
>      (An earlier attempt patched `recreateVidmemRasters` — the post-`Reset` path,
>      which never runs at startup — and its "no change" result was misread as
>      proof of sharing. Identical output is equally consistent with dead code.)
>   3. *The librw camera is frozen.* No — instrumentation shows input → frame LTM →
>      `cam->devView` all tracking per frame. The two shots that looked identical
>      simply sit at near-identical camera poses.
>   4. *Depth state is off at submit.* No — explicitly forcing `ZTESTENABLE`/
>      `ZWRITEENABLE` after the resync changed nothing.
>
>   **Remaining hypothesis, untested:** the depth *values* librw writes are not
>   comparable with the D3D9 path's even though the projection parameters agree —
>   e.g. a transpose/handedness difference in `RawMatrix::mult(&combined,
>   &worldview, &cam->devProj)` (`d3drender.cpp:400-416`) that leaves x/y correct
>   (the scene is correctly framed) while z is not. **[UNCERTAIN]** — needs a depth
>   readback or a z-only probe, not another reading pass.
> - **D-S3-3 — I4 fog/lighting.** Much reduced by P4. **RW ties the fog END to the
>   camera far plane** (`fogData.end = cam->farPlane`), so `fog_end_` (70) cannot be
>   honoured independently of the clip distance (643.6) — the ramp is necessarily
>   longer than D3D9's. A genuine documented delta, not a bug.
>
> **Sim is renderer-independent — verified.** `log/r10b_sim.txt` under librw is
> byte-identical to the D3D9 control (same frames, same `pos`/`yaw`/`spd`), so none
> of this perturbs the simulation.
>
> **Scoreboard after the fixes** (mean-abs vs the D3D9 control, not vs the
> reference): `car_2_drive` 25.37 → **14.31**, `01_grid` → 14.57, `01_action` →
> 14.99, `car_4_chase` → 21.79. Still dominated by D-S3-1, so still not a parity
> number.
>
> #### Caveats that must not be lost
>
> - **Only 10 of the 13 manifest shots regenerate** under the documented recipe in
>   this environment: `00_results`, `01_cupstandings`, `01_results` were not
>   produced (they need further cup rounds), so **7 of the 10 gating shots** were
>   exercised, not 10. The stale files of those names in `verify/race1` are from
>   earlier sessions and must not be mistaken for run output.
> - **Captures land in the MAIN repo's `verify/`**, because `MASHED_ROOT` makes the
>   exe `SetCurrentDirectory` to the main repo to reach `original/`. A worktree run
>   therefore writes into shared scratch — snapshot results out immediately, and
>   coordinate before capturing while another session is active.
>
> ---
>
> ### E2'b step 3 (camera + I4 fog/lighting) — original design block (kept for the record)
>
> Attempting this from the standalone probe was rejected after looking at where the
> inputs actually live:
> - **Fog** is parsed inline inside `TrackRenderer::Load` from COURSE.LUA
>   `Setup_Fog(near, far, r, g, b)` (`TrackRenderer.cpp:1268-1291`).
> - **Lights** are parsed inline from LIGHTS.DFF in the same function
>   (`:936-1031`), including the default-light fallback when a track declares none.
> - **The chase rig** (`:3703-3712`) needs `car_len_` / `car_height_`, which only
>   exist after a vehicle DFF has been loaded and measured.
>
> Duplicating any of that into `RwSceneBuild` would create a second copy of a
> parser that must agree with the first — the "wrong plate propagates into ports"
> failure mode. And a parity shot built on *approximated* camera constants would
> look authoritative while measuring nothing, which is the same trap as the three
> measurement failures logged above.
>
> **Decision: the librw path must render IN-LOOP, not from a probe.** Add a librw
> submit path alongside `g_track.Render(dev, t, &ci)` in `exe_main.cpp`'s frame
> loop, selected by `MASHED_RENDER_LIBRW`, consuming the camera/fog/light state
> `TrackRenderer` has already computed — reading the *same* values, never
> re-parsing them. `RaceSceneState` (E2'c) is what makes that reachable; the three
> `D3DCOLOR` members (`fog_color_`, `amb_world_`, `sun_color_`) should move there
> and retype to `uint32_t` as part of this step, which is also when §3.4's deferred
> retype is finally worth doing.
>
> Only once that is in place does an imgdiff against the ten gating viewpoints
> measure renderer parity rather than "librw does not draw a car yet".

**E2'c — the TrackRenderer split (L — this is the real cost).** Before cars/particles/pickups can
move, `TrackRenderer.cpp`'s 4139 LOC must be separated into *race state* (keep, renderer-neutral)
and *draw submission* (replace). Suggested cut: extract a `Race/RaceSceneState` holding the
gameplay members, leave `TrackRenderer` as the D3D9 submitter, add `LibRw/RwSceneSubmit` as the
librw submitter, both consuming the same state. **Do this as its own session with a no-op
refactor commit first** (pure move, existing D3D9 path unchanged, existing parity GREEN
preserved) — that de-risks everything after it. Then port cars, particles, pickups, sky.

### E3' — viewpoint parity pass with documented deltas (**M**)
**Acceptance is explicitly NOT bit-parity** (gate D2). It is behavioural parity with every
remaining difference written down.

Protocol:
- **`imgdiff.py` — primary gate, survives the swap untouched.** It compares presented pixels
  (`MASHED_DBG_BBDUMP` → `verify/dbg_backbuffer.bmp`, or `capture_window.ps1`), so it is
  renderer-agnostic by construction. Fixed viewpoint set, captured on the D3D9 path *before*
  E2' as the reference: (1) main menu, (2) Arctic race start-line, (3) Arctic mid-lap with
  props + copters, (4) a car-heavy pack shot, (5) a particle/weather-active frame, (6) a
  pickup-orb frame, (7) sky/fog horizon, (8) results screen. Per-viewpoint mean-abs and
  %-over-threshold recorded in a **delta register** inside this brief's successor doc; each
  non-zero delta gets a one-line cause or an `[UNCERTAIN]`.
- **`nav_coverage.py` — survives untouched.** Pure source linter over `Frontend/MenuNavSM.cpp`
  + `exe_main.cpp`; never touches the renderer. Keep running it.
- **`drawlist_diff.py` — schema survives, emit sites must be re-planted.** The record format is
  the *original's* RW Im2D vertex layout (`decode_raw_blob`, `<ffffIff`), not a D3D9 layout, so
  the differ and the original-side capture (`menu_draw_burst.py`) are unchanged. What moves:
  - `RwIm2DBridge.cpp:121` (the primary emit, inside `Bridge_DrawPrimitive`, fired on the raw
    source blob + mirrored RW state *before* conversion) → an equivalent one-line
    `DrawStreamDump_OnDraw(...)` at librw's im2d submit entry (`src/im2d`-equivalent).
  - `exe_main.cpp:2858` (the native video quad that bypasses the bridge) — hand-built record,
    unaffected.
  - `TrackRenderer.cpp:3601/3611/3620/3951` (`DrawStreamDump_Race3DBegin/Cat`, camera-invariant
    geometry tallies) → re-insert in the librw world/atomic submit path.
  Net harness cost: **two emit hooks re-wired, ~S**, not a harness rebuild.
- **Draw-list checks that still apply:** the full 2D menu channel (MISSING/EXTRA/MISMATCH
  classification is renderer-independent) and the 3D geometry tallies. What does *not* apply:
  any expectation that D3D9 draw-call counts match — librw batches differently by design, and
  that difference is a documented delta, not a failure.

### E4' — verbatim RW islands, demand-driven only (**open-ended, unscheduled**)
Per D2, verbatim RW ports now happen **only** where an E3' behavioural delta cannot be closed
inside librw. Each island is opened by a specific cited delta, ported per `hook-author`, and
verified per `diff-original`. **Do not pre-queue islands.** The ~770 rows + ~217 stubs the D2
reversal avoided stay avoided unless a delta names them.

### Sequencing
```
E1' (S)  ->  E2'a (M)  ->  E2'c refactor, no-op commit (M)  ->  E2'b (M)  ->  E2'c ports (M–L)  ->  E3' (M)  ->  E4' (as-needed)
                              ^ the earlier this lands, the cheaper everything after it
```
Reference captures for E3' should be taken **before** E2' starts, on the current GREEN D3D9 path.

---

## 5. Risk register

| # | Risk | Sev | Evidence / mitigation |
|---|---|---|---|
| **R1** | **`TrackRenderer.cpp` fuses 4139 LOC of D3D9 draw path with race gameplay state.** Any swap risks dragging gameplay behaviour along with the renderer. | **HIGH** | The dominant cost. Mitigate with the E2'c **no-op refactor commit first** (pure move, D3D9 path unchanged, parity still GREEN) before any librw submission lands. |
| R2 | Acceptance is behavioural, not bit-exact — "how different is too different?" has no mechanical answer. | HIGH | Gate D2 chose this deliberately. Mitigate with the fixed 8-viewpoint set + a written delta register where every non-zero diff has a cause or an `[UNCERTAIN]`. No delta may be closed by "looks fine". |
| R3 | RW per-vertex lighting/fog semantics differ from our fixed-function approximation (I4). | MED | Expect visible deltas in ambient/sun/fog. These are *candidate documented deltas*, not bugs — but if the track reads wrong, that is an E4' island. |
| R4 | Mashed TXD mip formats may not map cleanly onto RW raster formats (I3). | MED | Unknown until enumerated. **E2'a's first action is the enumeration** — do not start the bridge before it. |
| R5 | librw owns device + Present; our boot path and d3d9 shim assume we do (I7, I8). | MED | Known InitD3D9/CreateDevice hang history (memory `project_initd3d9_createdevice_hang`). Prove in E1' smoke behind `MASHED_RENDER_LIBRW=1` so the working path stays reachable. |
| R6 | Default librw D3D9 pipe uses vs_2_0/ps_2_0; our spike is fixed-function (I9). | LOW-MED | Shader blobs are pre-compiled and committed (no fxc). `defaultRenderCB_Fix` is the fallback. re3 ships the shader path on Windows. |
| R7 | librw is alive upstream (pushed 2026-07-14); drift vs our snapshot. | LOW | Snapshot + `PINNED_REV.txt` + keep the gitignored pristine clone for diffing. |
| R8 | Frame-baked `DffBatch` collapses the RW frame hierarchy (I2) — forecloses skinned/animated parts. | LOW | Costs nothing today (nothing in Mashed's DFFs is skinned as far as our loader exposes). Re-open only if an animated part appears. |
| R9 | librw in the .asi would collide with `MASHED.exe`'s own RW engine and device. | LOW | Avoided by decision: **exe-only** (§3.4). Guard any bridge TU that lands in `asi_sources.rsp`. |
| **R10b** | **Build-level nondeterminism: up to 8 of 13 shots differ between two builds of IDENTICAL source** (measured over two rebuild-control pairs; one pair gave 5/13 identical, the other 12/13, so it is intermittent between build pairs and a single control run proves nothing). Same source, same deterministic clock, differing behaviour. **This blocks E3' just as R10 did** — no in-race viewpoint can certify anything until it is fixed. It also produced a false "the refactor broke it" verdict for E2'c. | **HIGH — blocks the lane's gate** | Signature points at a read of uninitialised memory or other address/layout-dependent behaviour on the in-race path: MSVC output is not byte-reproducible, so small code-layout shifts change what garbage is read. `car_1_spawn` is stable, `car_2_drive` onward is not, so it enters after spawn and accumulates. Next step: diff the per-lap/split log lines between two builds to decide sim-side vs renderer-side, then bisect. Menu shots (`00_challengeselect`, `01_cupstandings`, `01_results`, `00_results`) are stable and usable today. |
| ~~R10~~ | ~~The E3' acceptance protocol does not work — the capture harness is nondeterministic~~ | **CLOSED 2026-07-31** | Fixed by `MASHED_DETERMINISTIC=1` + `MASHED_DET_FRAMES=N` (commit `46869d1d`). **13/13 captures bit-identical across two runs of the same binary**, from a starting point of up to 36.74 mean-abs. Two causes: wall-clock `dt` driving the sim, and — the larger term — the harness killing the process on a wall-clock timeout, which under a synthetic clock stops it at a different synthetic instant each run. |
| ~~R10-orig~~ | **The E3' acceptance protocol does not work as specified — the capture harness is nondeterministic.** Two boots of a byte-identical binary differ by up to **36.74 mean-abs** across the ten viewpoints (measured 2026-07-31). Any realistic librw delta is far below that, so imgdiff-vs-baseline currently tests nothing. | **HIGH — blocks the lane's gate** | Root cause: `exe_main.cpp:2318` derives `dt` from the GetTickCount wall clock and `:2416` feeds that wall-clock `dt` into `UpdateCar`, so the sim advances frame-rate-dependently; capture triggers also fire on wall-clock `t`. The seed is fine (spawn state is bit-identical: `gate0=(-26.08,0.04,17.00) yaw=1.55`), so this is fixable, not fundamental. Needs a deterministic capture mode: fixed `dt` on every sim path + frame-count-driven capture triggers — the "clock = render frames" approach that already yields +0 drift in `replay_verify.py`. **Only `05_car_spawn` (t=0.8 s, 0.10 run-to-run) is numerically usable today.** |
| ~~R0~~ | ~~librw's 32-bit MSVC path is dead → D2 reopens~~ | **CLOSED** | Measured: 0 errors, 45 objs, `librw.lib` 1.23 MB, COFF machine `0x014C`. See §1.3. |

---

## 6. Complication list (things the user should know before E1')

1. **librw is a new tracked dependency.** Nothing RW is vendored into the build today (§2.4).
   This needs approval — see the question below.
2. **The expensive part is ours, not librw's.** R1: the `TrackRenderer` split is the single
   largest item in the lane and it buys nothing visible on its own.
3. **We keep all three of our loaders.** librw's stream layer is unused; the "BSP unsupported"
   warning is moot (§3.2). Anyone reading upstream's README cold will think this lane is
   blocked — it is not.
4. **The .asi does not get librw.** Exe-only, by decision.
5. **Reference captures must be taken before E2' starts**, on the current GREEN path, or E3' has
   nothing to diff against.

---

## Appendix — evidence index

| Claim | Where measured |
|---|---|
| MIT license text | `re/prior_art/renderware/librw/LICENSE` (read verbatim); GitHub API `spdx_id: MIT` |
| Upstream alive, 2026-07-14 | GitHub API `/repos/aap/librw` |
| RW 3.1–3.7 range | `librw/src/rwbase.h` version comment table |
| `win-x86-d3d9` platform | `librw/premake5.lua` |
| **x86/MSVC build clean** | this session: `vcvars32` + `cl 19.44` over `src/*.cpp src/d3d/*.cpp src/lodepng/*.cpp`; 0 errors, 3× C4838, 45 objs, `librw.lib` 1,232,788 B, `engine.obj` COFF machine `0x014C` |
| Shaders pre-compiled | `librw/src/d3d/shaders/*_VS.h`, `*_PS.h` committed; `make_*.cmd` only regenerates |
| Device ownership | `librw/src/d3d/rwd3d.h` (`EngineOpenParams{HWND}`, `extern IDirect3DDevice9 *d3ddevice`); `d3ddevice.cpp:1518,1622,1356,1192-1211` |
| Default pipe is shader-based | `librw/src/d3d/d3d9.cpp:697`; fallback `d3d9render.cpp:78` |
| No BSP reader | `librw/src/world.cpp` (5.4 KB, container only); README *"BSP is not supported at all"* |
| Mashed assets are RW 3.7.0.2 | this session: `piz_extract.py extract Arctic.piz`; chunk headers `0x0B`/`0x10`/`0x23`, libid `0x1C02000A` |
| TXD is proprietary `0x23` | `mashedmod/src/mashed_re/Txd/TxdDecoder.h:2`, `.cpp:49` |
| Our render surface / LOC / D3D9 leaks | account2 worker survey, 2026-07-31 (read-only; cost $2.41, off this account's quota) |
| Build wiring + qhull precedent | `mashedmod/build.bat:24-28,35-39,82-276,279,287`; `mashedmod/asi_sources.rsp` |
| Parity harness emit sites | `RwIm2DBridge.cpp:121`; `exe_main.cpp:2858`; `TrackRenderer.cpp:3601,3611,3620,3951` |
