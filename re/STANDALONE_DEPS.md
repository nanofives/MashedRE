# Standalone exe external-dependency audit

Phase B (M2). Enumerates every external symbol `mashed_re.exe` (the
standalone target) needs to either link against, stub, or reimplement
before it can replace `MASHED.exe`.

Two layers: (1) Win32/DirectX **DLL imports** the original `MASHED.exe`
links against; (2) **MASHED-internal RVAs** our `.asi` reimpls tunnel
into. Both must be resolved for the standalone exe to run.

Generated 2026-05-24 from:
- `re/analysis/standalone_deps/imports_raw.txt` — `dumpbin /imports MASHED.exe`
- `re/analysis/standalone_deps/asi_callees.txt` — RVAs referenced as
  function pointers in `mashedmod/src/mashed_re/**/*.cpp`
- `re/analysis/standalone_deps/asi_external_callees.txt` — callees minus our
  393 RH_ScopedInstall'd RVAs

## Layer 1 — DLL imports (190 symbols across 7 DLLs)

`MASHED.exe` is surprisingly modest at the DLL boundary. RW3 is
**statically linked** (no RenderWare DLL imports; `_rwcseg` / `_rwdseg`
sections in the PE confirm). DirectShow is not imported — likely loaded
dynamically via `LoadLibraryA("quartz.dll")` at intro-video time, or
absent (the intro player may use MCI through `WINMM`).

| DLL | symbols | disposition for standalone exe | rationale |
|---|---|---|---|
| `KERNEL32.dll` | 130 | **proxy** (link normally) | OS primitives — heap, file I/O, threads, sync, locale, env, memory. Our standalone exe needs the same. Using MSVC link defaults gets these for free. |
| `USER32.dll` | 40 | **proxy** (link normally) | Window, message pump, dialog, cursor, timer. Same as MASHED's use. |
| `GDI32.dll` | 2 | **proxy** | `DeleteObject`, `GetStockObject` — likely for window-icon bitmap. Negligible. |
| `ADVAPI32.dll` | 4 | **proxy** | Registry reads (joypad config, install-detect). Can be **stubbed** if standalone doesn't need to read MASHED's registry settings. |
| `ole32.dll` | 6 | **proxy** | `CoInitialize`, `CoCreateInstance`, `CoUninitialize`, `CoTaskMemAlloc/Free`, `CoFreeUnusedLibraries`. Needed for DSound + D3D9 init. |
| `WINMM.dll` | 5 | **proxy** | `timeBeginPeriod`, `timeEndPeriod`, `timeGetTime`, `timeSetEvent`, `timeKillEvent`. Multimedia timer / MCI. Used for frame timing. |
| `DINPUT8.dll` | 1 | **proxy** | `DirectInput8Create` only. The standalone exe uses DInput too; reuse. |
| `d3d9.dll` | 1 | **proxy** | `Direct3DCreate9` only. The standalone exe needs this verbatim. |
| `DSOUND.dll` | 1 (ord 11) | **proxy** | `DirectSoundCreate` (ord 11). Audio is in scope for standalone? See "deferrals" below. |

**Net for Layer 1: link the standalone exe against the same 7 DLLs.**
`mashedmod/build.bat`'s exe target already passes `user32.lib` and
`d3d9.lib`; need to add the rest:

```
kernel32.lib  user32.lib  gdi32.lib  advapi32.lib  ole32.lib
winmm.lib  dinput8.lib  d3d9.lib  dsound.lib
```

(All ship with the Win10 SDK. MSVC's auto-linking via `#pragma comment(lib, ...)`
or `/link <libs>` works equivalently.)

## Layer 2 — MASHED-internal RVAs (960 external callees)

These are functions inside `MASHED.exe` that our `.asi` reimpls reference
via raw RVA. Counted from `asi_external_callees.txt` after subtracting
our 393 own `RH_ScopedInstall` RVAs.

| RVA range | # callees | category | disposition |
|---|---|---|---|
| `0x00400000`–`0x00499fff` | 698 | **app code** (game logic, UI, dispatch) | **reimplement** — what Phase A is doing |
| `0x004a0000`–`0x004bffff` | 128 | **MSVC CRT static** | **proxy** — link the standalone with default MSVC CRT; the CRT functions our hooks tunnel into (e.g. `__stricmp`, `_atexit`, `_alldiv`) exist in our own CRT identically |
| `0x004c0000`–`0x004cffff` | 126 | **RenderWare 3 static** (RW engine, camera, raster, texture, freelist, immediate-mode 2D) | **reimplement** — Phase F's "biggest unknown". Stub-tier MVP per the roadmap: clear + RwIm2DRenderPrimitive + textured-quad upload + 2D camera |
| `0x004d0000`–`0x004fffff` | 8 | trailing app-side functions | **reimplement** |

### CRT region (~128 RVAs) detail

The MSVC CRT functions statically linked at `0x004axxxx`–`0x004bxxxx`
include: heap manager (`_heap_init`, `_heap_alloc`, `_heap_free`), exit
chain (`_atexit`, `_cexit`, `__crtExitProcess`), environment
(`_setenvp`, `_setargv`), locale tables, string functions (`__stricmp`,
`__strnicmp`), math helpers (`__alldiv`, `__allmul`, `__aulldiv`), SEH
(`__SEH_prolog`, `__SEH_epilog`), exception filters, FPU control.

**Disposition: link the standalone with `/MD` or `/MT` and let MSVC's
own CRT satisfy these.** Our 38 `0x004axxxx`-region hooks already
verify our local CRT bit-identical against MASHED's; the symbols match
because both are MSVC. No standalone-exe action required beyond
default linkage.

### RW3 region (~126 RVAs) detail — dominant Phase F risk

The RW3 region at `0x004c0000`–`0x004cffff` contains MASHED's statically
linked RenderWare 3.x. From our hook set + skeleton call tree, the
functions our frontend reaches include:

| Function | RVA | Phase F disposition |
|---|---|---|
| `RwEngineInit` | tbd | stub (return 1) for boot |
| `RwEngineOpen` | tbd | stub |
| `RwEngineStart` | tbd | stub |
| `RwCameraBeginUpdate` | tbd | reimplement on D3D9 directly |
| `RwCameraEndUpdate` | tbd | reimplement (Present) |
| `RwCameraClear` | `0x004c1bb0` | reimplement (D3D9 Clear) |
| `RwIm2DRenderPrimitive` | tbd | **reimplement** (textured-quad upload + draw) |
| `RwIm2DRenderIndexedPrimitive` | tbd | reimplement |
| `RwRasterCreate` | tbd | reimplement on D3D9 `CreateTexture` |
| `RwTextureCreate` | `0x004c5a00` | reimplement (already in hook set) |
| `RwTexDictionaryCreate` | `0x004c5890` | reimplement |
| `RwTexDictionarySetCurrent` | `0x004c5800` | reimplement (slot store) |
| `RwTexDictionaryGetCurrent` | `0x004c5820` | reimplement (slot load) |
| `RwMatrixScale` | `0x004c5010` | reimplement (pure math) |
| `RwMatrixTranslate` | `0x004c51a0` | reimplement (pure math) |
| `RwMatrixMultiply` | `0x004c4600` | reimplement (pure math) |
| `RwMatrixInvert` | `0x004c4dc0` | reimplement (pure math) |
| `RwV3dTransformPoint` / `Vector` | `0x004c3xxx` | already reimplemented (`Math/RwV3dTransform.cpp`) |
| `RwStreamRead` / `Write` / `Skip` | `0x004cbd30`+ | already reimplemented (`Save/RwStream.cpp`) |
| `RwFreeListCreate` | `0x004cc820` | reimplement (pool allocator) |
| ... (remaining ~100 RVAs) | tbd | enumerate during Phase F |

**Strategy (per roadmap F2): start with the stub renderer** — just
enough RW3 to clear + draw textured 2D quads. ~10–15 functions on the
critical path; the rest can return 0 / no-op until they become
load-bearing.

## Deferrals (out of scope for the minimum-viable standalone)

- **DirectShow / intro-video player.** No DirectShow imports in
  `MASHED.exe`; the intro player likely uses MCI through `WINMM`'s
  `mciSendCommandA` (loaded dynamically). The roadmap calls for
  `HardwareShowIntroVideo` to be `return;`-stubbed in the standalone,
  skipping intros entirely. **Disposition: stub.**
- **Audio (DSound).** Per roadmap: "Audio can be silent-stubbed
  through the entire frontend path. Saves us a whole subsystem."
  Re-implement only if standalone needs sound. **Disposition: silent
  stub for menu demo; revisit when scope expands.**
- **Registry reads (`ADVAPI32`).** Used by MASHED's installer-detect /
  config-restore. Standalone exe can hardcode defaults. **Disposition:
  stub.**
- **MSVBVM60 wrapper (`launch.exe`).** Not in `MASHED.exe`'s imports.
  Standalone exe replaces `launch.exe` entirely; no VB6 dependency.
  **Disposition: skip.**

## Smallest-viable-demo scope (per roadmap escape hatch)

For "standalone exe shows the MASHED title screen, no input":

- Link Layer 1's 7 DLLs (~5 min toolchain setup).
- Stub Layer 2 trailing (~8 RVAs) + CRT (link standalone normally,
  inherits everything).
- Reimplement Layer 2 app code (~698 RVAs) — most already done; **the
  remaining 27 phase-a2-no-registry-deferred hooks are mostly in this
  bucket** and may not all be reachable from the title-screen path.
- Reimplement RW3 stub tier (~10–15 of the 126 RVAs) — clear,
  textured-quad upload + draw, 2D camera setup.
- Stub everything else (DSound, DirectShow, registry).

That collapses the dependency surface from ~960 callees to **~25–40
new reimpls** for the title-screen demo. Most of the rest can be
deferred to the menu-navigation milestone (M8) or beyond.

## What unblocks now

Phase A2 hooks tagged `needs-canonical-*-state` will be **naturally
validated** when the standalone exe walks the same call chain. No
additional dev-time hook scaffolding needed for those.

Phase A4 boot-crashers (`CrtPreInit`, `WinMainEntry`, etc.) are
**not blockers** for the standalone exe — the standalone has its own
`WinMain` (Phase D) and uses MSVC's CRT prelude directly, bypassing
MASHED's boot chain entirely.

`MenuChromeShellB`'s `reimpl-divergent-error` tag is the one Phase A
loose end that *might* matter — but only if menu chrome rendering is
in the title-screen scope. If we're just showing the LOGO sprite,
this hook is unused.

## Next: Phase C — link the standalone exe

Per FRONTEND_ROADMAP C1: extend `mashedmod/build.bat`'s exe target to
include the full reimpl `.cpp` set (minus `Core/HookSystem.cpp` and
`dll_main.cpp` which are dev-only). Add Layer 1 DLLs to the link line.
Expect ~700 unresolved-symbol errors initially (the Layer 2 app-code
callees not yet reimplemented). Each one is a known address —
`mashedmod/src/mashed_re/Stubs/` can declare them as `extern "C"`
no-op stubs returning 0, then we implement bottom-up.
