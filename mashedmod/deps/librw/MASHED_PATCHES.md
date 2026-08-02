# Local patches to the vendored librw snapshot

Vendoring strategy: `re/analysis/LIBRW_SIZING_2026-08.md` §3.4 ("vendored snapshot,
qhull-shaped"). Upstream is `aap/librw` (MIT). Every local change is listed here and
is marked in-source with the string `MASHED LOCAL PATCH` so `grep -rn "MASHED LOCAL
PATCH" deps/librw/` reproduces this table exactly.

Keep this file in sync when patching. A snapshot re-vendor must re-apply these.

| # | Files | Why |
|---|---|---|
| P1 | `src/d3d/rwd3dimpl.h`, `src/d3d/rwd3d.h`, `src/d3d/d3ddevice.cpp` (`startD3D`) | **Device adoption.** New `D3d9Globals::adoptedDevice` + `rw::d3d::setAdoptedDevice()`. When set, `startD3D()` skips `CreateDevice()` and takes the exe's device instead, filling `d3d9Globals.present` from the live swapchain via `GetSwapChain(0)->GetPresentParameters()`. |
| P2 | `src/d3d/d3ddevice.cpp` (`termD3D`) | **Don't release an adopted device.** The exe owns its lifetime (`ShutdownD3D9`); the upstream `Release()` would drop the refcount to zero while the D3D9 path is still drawing. |
| P3 | `src/d3d/d3ddevice.cpp` (`beginUpdate`, `endUpdate`) | **Don't own the frame.** Under adoption, suppress (a) the auto-`Reset()` on client-rect mismatch and (b) `BeginScene`/`EndScene`. |
| P4 | `src/d3d/d3ddevice.cpp` (`resyncDeviceState`), `src/d3d/rwd3d.h` | **Re-push cached state onto a device someone else drew with.** Exposes upstream's `restoreD3d9Device()`. librw's `setRenderState` layer is a write-back cache; the D3D9 path changes device state behind it (notably `TrackRenderer::Render` exits with `D3DRS_ZENABLE=FALSE`, `TrackRenderer.cpp:4151`). Without this, librw's first submit inherits whatever the other renderer left. |
| P5 | `src/d3d/d3d.cpp` (`rasterCreateZbuffer`) | **Share the exe's depth buffer.** Upstream decides "is this the main depth buffer?" by comparing the raster size to the **window client rect**. Under adoption that is the wrong question — the backbuffer size is deliberately independent of the window (borderless mode). When it mismatches, librw silently allocates a *private* depth surface and its camera is blind to everything D3D9 already drew. Inert at 640×480 windowed, where the sizes happen to agree. |

## Why P1 — the shape of the problem

Upstream librw owns the device end to end: `Direct3DCreate9` (`d3ddevice.cpp:1518`),
`CreateDevice` (`:1622`), `Present` (`:1356`), and `startD3D()` asserts
`d3d::d3ddevice == nil` before creating.

E2'b step 3 needs the librw world to land **in the exe's backbuffer, inside the
exe's `BeginScene`/`EndScene`/`Present`**, beside `g_track.Render()`. That is what
makes an `imgdiff` measure renderer parity instead of measuring a device swap, and
it is what keeps the capture harness (`MASHED_DBG_BBDUMP`) and the d3d9-shim frame
limiter working untouched. Two devices on one HWND was rejected: they cannot
composite into one frame, so the capture would read only one of them.

Adoption is deliberately *opt-in and inert by default*: with `adoptedDevice == nil`
every patched site takes the original upstream branch, so `MASHED_LIBRW_SMOKE`
(the E1' probe, which still lets librw create its own device) is unaffected.

## Why P3 — the two things that bite

Both were found by reading, before they could produce a mystery blank frame:

1. **Auto-`Reset`** (`beginUpdate`, upstream `:1301-1310`). librw resizes the
   swapchain whenever the client rect differs from `present.BackBuffer*`. mashed_re
   renders to a fixed 640×480 backbuffer whose size is independent of the window by
   design — the borderless native-resolution mode makes them differ deliberately —
   so this fires *every frame* and would `Reset()` the exe's device mid-frame,
   destroying its render targets.
2. **Nested `BeginScene`.** `exe_main.cpp` opens the scene before calling the submit
   path. A nested `BeginScene` returns `D3DERR_INVALIDCALL`, and the matching
   `EndScene` in `endUpdate` would close the exe's scene early — silently dropping
   every draw issued afterwards, i.e. the whole HUD.

## What is NOT patched

Render-state ownership beyond the above. librw's `d3drender.cpp` sets states through
its own `rwStateCache`, which does not know what `TrackRenderer` left set, and
`TrackRenderer` likewise re-sets its states each frame. Both renderers currently
re-establish what they need, so no leak has been observed — but this is an
assumption, not a proof, and it is the first place to look if the D3D9 path starts
rendering wrongly only when `MASHED_RENDER_LIBRW=1`. Tracked as a step-3 risk.
