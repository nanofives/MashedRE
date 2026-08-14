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
| P6 | `src/d3d/d3ddevice.cpp` (`setFogRange`), `src/d3d/rwd3d.h` | **Decouple the fog ramp from the clip distance.** New `rw::d3d::setFogRange(start, end)`, called after `Camera::beginUpdate()`. Upstream derives the fog constant's END from `cam->farPlane` (`:1288`) — which upstream itself flags as provisional — so `fog_end_` could not be honoured independently. See "Why P6" below. Inert unless called. |
| P7 | `src/d3d/shaders/default_VS.hlsl`, `skin_VS.hlsl`, `default_PS.hlsl` (+ the 8 regenerated `.h` blobs), `src/d3d/d3ddevice.cpp` (fog constant upload), `src/d3d/rwd3d.h` (`PSLOC_fogData`) | **Per-PIXEL fog.** Upstream evaluates the whole fog factor in the VERTEX shader and interpolates the already-CLAMPED result; D3D9 table fog (`D3DRS_FOGTABLEMODE = D3DFOG_LINEAR`), which the hand-written D3D9 path uses, evaluates it per pixel. `TexCoord0.z` now carries raw eye depth and the ramp+clamp moved to the PS, which needs `fogData` at PS `c1` as well as VS `c14`. See "Why P7" below — **including the measurement that this was very nearly a no-op.** |

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

## Why P6 — and why the far plane is NOT shortened instead

The premise that made this look unfixable was that "RW ties fog to the far plane"
is a *design* property of RW. It is not. Fog here is not fixed-function at all —
the fixed-function `D3DRS_FOGSTART`/`D3DRS_FOGEND` writes are commented out
(`d3ddevice.cpp:1285-1286`). What is actually used is a vertex-shader constant
`fogData = (start, end, range, disable)` at `c14`, uploaded at `:308`, consumed by
`default_VS.hlsl:48` as

```
TexCoord0.z = clamp((Position.w - fogEnd)*fogRange, fogDisable, 1.0)
```

— a fog factor that is 1 at `w == start` and 0 at `w == end`, with
`range = 1/(start - end)`. `beginUpdate` merely *populates* that constant, taking
`end` from `cam->farPlane` under a comment reading `// TODO: figure out where this
is really done`. The coupling is an unfinished upstream detail, not a constraint.
Nothing downstream reads `fogData.end` as a clip distance, and `devProj` is built
from the real `farPlane` a few lines earlier, so overwriting the constant after
`beginUpdate` changes the ramp and nothing else.

Three options were on the table; the other two were rejected on evidence:

- **Shorten the far plane to `fog_end_`.** Rejected. `proj[10] = far/(far-near)` and
  `proj[14] = -near*proj[10]` (`:1270-1279`), so `far` 643.6 → 70 rewrites the depth
  encoding librw writes into the depth buffer it **shares with the exe's D3D9 path**
  (P5, and D-S3-1's measurement that the surface really is shared). librw-drawn cars
  and props would then occlude-test incorrectly against D3D9-drawn particles and
  pickups. It would also hard-clip all world geometry past 70 rather than fogging it.
  Fixing fog by breaking Z is not a trade worth making.
- **Accept it as a documented delta.** Rejected. It is a uniform ~1.4× on every
  fogged surface — the largest single remaining parity term, and visible — while the
  fix is four lines. A delta register is for differences that cost more to close than
  to live with; this is the opposite.

## Why P7 — and the finding it did NOT produce

**Read this before spending a session on it: the per-pixel move closed almost
nothing. The fog residual it was chased for had a different cause entirely.**

The model difference is real and correctly described. D3D9 pixel/table fog is
evaluated per pixel; `default_VS.hlsl:48` evaluated the whole factor per vertex.
The device's premise was checked rather than assumed — `P7 fogcaps` logs
`WFOG=1 ZFOG=1 FOGTABLE=1` on this hardware, and `TrackRenderer`'s projection has
`m._34 = 1`, so D3D9 genuinely does linear fog on eye-W.

But the delta that predicts is nearly zero, for a reason worth writing down:
`TexCoord0` is interpolated **perspective-correct**, and perspective-correct
interpolation of an attribute that is affine in `w` reproduces the per-pixel value
exactly — for `a_i = (w_i - end)·range`,

```
    Σλ·a_i/w_i        1
    ----------  =  ------- · Σλ·(w_i - end)·range/w_i  =  (w_true - end)·range
    Σλ·1/w_i        Σλ/w_i
```

because `w_true·Σλ/w_i = 1`. So the two models can only differ where the **clamp**
saturates at a vertex, and on this geometry that is a sliver. Measured, librw ON
before vs after the shader change: **0.00 on 5 of 7 gating shots**, 0.06 and 0.05 on
the other two.

**What actually caused the fog residual was a byte-order slip on our side of the
seam** (`LibRw/RwRaceSubmit.cpp`, not a librw defect): `COLOR_ARGB` packs
`0xAARRGGBB`, but `rw::SetRenderState(FOGCOLOR)` unpacks RGBA little-endian
(`red = value`, `blue = value>>16`, `d3ddevice.cpp:672-676`). Passing a D3D-ordered
word **swapped red and blue in the fog colour**. Arctic's fog is `282C30`, so the
shader was blending toward `302C28` — an 8-LSB error in R and B, which at the
frame's mean `(1 - fogFactor) ≈ 0.09` is ~0.75 LSB. The measured signed difference
before the fix was `(d3d9 - librw) = (-0.75, +0.02, +0.76)` over 48% of the frame:
antisymmetric in R/B, **G exactly zero**. Nothing but a channel swap has that shape.

P7 is kept anyway: it is the correct model, it costs 3 PS instructions, it makes the
two fog paths agree by construction rather than by a property of the interpolator,
and it regressed nothing. It is simply not what closed the term — and the way it was
mis-attributed (fog-off closes the gap → librw fogs per-vertex → per-vertex is the
cause) is the same circumstantial-signature error that produced the retracted MATID
instrument and the falsified "perspective" root cause. Three for three.

Interface note for anyone editing these shaders: `default_PS` is fed by **both**
`default_VS` and `skin_VS`, and by `d3d9matfx.cpp:49-61` (the non-env matfx path)
and `d3d9skin.cpp:321-323`. All the VS variants that feed it were regenerated
together. `im2d_VS`/`im2d_PS` and `matfx_env_VS`/`matfx_env_PS` are self-consistent
pairs and were deliberately left on the per-vertex model.

Regenerating the blobs (the DXSDK `make_*.cmd` reference `%DXSDK_DIR%`, which we do
not have — use the Windows 10 SDK fxc, **from PowerShell**; Git Bash mangles `/T`):

```
fxc /nologo /T vs_2_0 [/DDIRECTIONALS …] /Vn g_vs20_main /Fh <out>.h <in>.hlsl
fxc /nologo /T ps_2_0 [/DTEX]            /Vn g_ps20_main /Fh <out>.h <in>.hlsl
```

`/Vn` is required — the headers are included into a `static` declaration that names
`VS_NAME`/`PS_NAME`. Editing a shader header does **not** invalidate `librw_d3d9.lib`
(the staleness check scans `src/**` mtimes only); delete the lib to force a relink.

## What is NOT patched

Render-state ownership beyond the above. librw's `d3drender.cpp` sets states through
its own `rwStateCache`, which does not know what `TrackRenderer` left set, and
`TrackRenderer` likewise re-sets its states each frame. Both renderers currently
re-establish what they need, so no leak has been observed — but this is an
assumption, not a proof, and it is the first place to look if the D3D9 path starts
rendering wrongly only when `MASHED_RENDER_LIBRW=1`. Tracked as a step-3 risk.
