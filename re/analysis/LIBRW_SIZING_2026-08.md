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

> ### E2'b step 3 (camera + I4 fog/lighting) — DESIGN DECIDED, NOT STARTED
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
