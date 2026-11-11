# RESULT RWG-RASTER-BIND (2026-11-10)

Verdict: **GREEN**. On the MASHED_ORIG_TOP RWMOD path, 0x004d52d0 (RwImageSetFromRaster) and RwGlobals +0x60
(stdFunc[rwSTANDARDIMAGEGETRASTER], original 0x004de1b0) are now bound on `rw::Raster`. `texture->raster` stays an
`rw::Raster`. No knob-path caller of 0x004d52d0 exists yet, so a default-off self-test on a real `rw::Raster` checks the
binding. Filed U-9803.

## Identity of 0x004d52d0, from the code (L0 rule: filing and address band are context only)

Headers: `re/prior_art/renderware/gta-reversed-modern/source/game_sa/RenderWare/rw/` (main checkout).

**(1) Structure.** The 51-byte body (`re/match/s21/004d52d0.c`, BYTE-IDENTICAL under the DDK):
`r = (*(RwGlobals + 0x60))(image, raster, 0)`. If `r != 0`, then `if (raster[+0x22] & 1) image[+0x0] |= 2`, and it
returns image. Otherwise it returns 0. That is the contract of `RwImage *RwImageSetFromRaster(RwImage *image, RwRaster
*raster)` (rwcore.h:4141): it returns the image or NULL. Only a thunk exists in the prior art (gta-reversed
`rwcore.cpp:832` -> SA 0x804250), so there is no RW source body to compare text against.

**(2) RW constants and offsets, each matching the headers:**

| body element | RW 3.x meaning | header |
|---|---|---|
| RwGlobals `+0x60` | `stdFunc[6]`. `stdFunc[]` starts at `+0x48`: curCamera +0x0, curWorld +0x4, renderFrame / lightFrame / pad[2] to +0x10, `RwDevice dOpenDevice` 14 x 4 B = 0x38 to +0x48. +0x48 + 6 x 4 = +0x60 | rwplcore.h:6289-6301, 5834-5855 |
| index 6 | `rwSTANDARDIMAGEGETRASTER` "Get image from a raster" | rwplcore.h:5693 |
| args `(image, raster, 0)` | `RwStandardFunc(void *pOut, void *pInOut, RwInt32 nI)` | rwplcore.h:5743 |
| raster `+0x22` bit 0 | `RwRaster.privateFlags` (+0x20 cType, +0x21 cFlags, +0x22 privateFlags), `rwRASTERGAMMACORRECTED = 0x01` | rwcore.h:1622, 1588 (also U-4978) |
| image `+0x0` OR 2 | `RwImage.flags`, `rwIMAGEGAMMACORRECTED = 0x2` | rwcore.h:2263, 2244 |

The same RwGlobals layout puts `fpRenderStateSet` at +0x20 and `fpIm2DRenderIndexedPrimitive` at +0x34. Those are the
device slots PORT-OT-RWG-DEVSLOTS bound from the original's values, and the layout was already validated in U-0125.

**(3) Callers and callees.**
- Callee: only the RwGlobals +0x60 slot. The original holds `0x004de1b0` there (ORIG_RWG.json), a raster-to-image
  converter (`re/match/rwl01/004de1b0.c`).
- Caller 0x00498810 (`re/match/s16/00498810.c`) is RW's WriteRaster sequence. Its counterpart in the prior art is
  gta-reversed `common.cpp:107-115` (SA 0x5A4150): `RwImageCreate(w, h, depth)`, `RwImageAllocatePixels`,
  `RwImageSetFromRaster`, write, `RwImageDestroy`. 0x00498810 does `0x004cdca0(raster+0xc, raster+0x10, 0x20)`,
  `0x004cdd60(img)`, `0x004d52d0(img, raster)`, `0x004ce8e0(img, name)`, `0x004cdd00(img)`. It takes the raster from
  camera `+0x60`, which is `RwCamera.frameBuffer` (rwcore.h:4917-4930: RwObjectHasFrame 0x14, projectionType, beginUpdate,
  endUpdate, viewMatrix 0x40 from +0x20). Raster width and height at +0xc / +0x10 match rwcore.h:1624-1627.
- Caller 0x00555ff0 (`re/match/s24/00555ff0.c:171-182`) uses the same create, allocate, set-from-raster order on
  `texture->raster`.
- Caller 0x0053e830 is not on any ORIG_TOP chain.

The three together identify 0x004d52d0 as RW 3.x RwImageSetFromRaster, so I filed no identity U-row. What is not
available is RW's source text for the body, only its header contract and SA's thunk.

## Binding (LibRw/RwApiBind.cpp, RWMOD path)

- **`MashedRw_ImageGetRaster_004de1b0`** is placed at RwGlobals +0x60 in `RegisterRwModules`.
  - `pOut` is an RW-layout `RwImage` (`MashedRwImageRW`, static_asserts on +0xc / +0x10 / +0x14). The game-code caller
    0x00555ff0 reads depth, stride and cpPixels at those offsets, so the image type is fixed by the caller, not chosen.
  - `pInOut` is an `rw::Raster`. The function calls `rw::Raster::toImage`, then `convertTo32`, then copies row by row
    into the image.
  - Only 32-bit images are filled. Both callers use depth 0x20. Other depths return 0, filed as **U-9803**.
- **`MashedRw_ImageSetFromRaster_004d52d0`** takes the same steps as the matched body:
  - It calls through the live RwGlobals at 0x007d3ff8 +0x60.
  - It then tests `rw::Raster::privateFlags` bit 0 and ORs `RwImage.flags` with 2.
  - The matched body cannot run on the knob path: it would read the `rw::Raster` at +0x22, which is inside librw's
    `privateFlags` int at +0xc.
  - librw sets no raster `privateFlags` bits, so a knob-path raster never reports gamma-corrected.
- `MASHED_DBG_RASTERBIND_SELFTEST=1` (default off) runs a self-test in `RegisterRwModules`.
- BINDINGS: two new rows, 0x004d52d0 and 0x004de1b0, both `bound`. They were written by `update_bindings_rwgrasterbind.py`
  through `bindings_rows.apply`, taking BINDINGS.tsv from 507 to 509 rows.
- Port column regenerated: 0 rows changed. A binding is not a ported body, and nothing calls 0x004d52d0 yet.

## Correctness: self-test on a real rw::Raster

`knob_on_selftest.log`:360 (`MASHED_ORIG_TOP=1 MASHED_ORIG_TOP_RWMOD=1 MASHED_DBG_RASTERBIND_SELFTEST=1`):

```
[RWG-RASTER-BIND] self-test 0x004d52d0 on rw::Raster 4x3 C8888: ret image, 0 of 48 bytes differ (first px 11181fff),
flags 0; privateFlags|=1: ret image, flags 2
```

- **Round trip.** A known RGBA image goes to `rw::Raster::setFromImage` (D3D9 C8888 texture), then through the binding
  into an RW-layout 32-bit image. All 48 bytes are equal and the RGBA order is kept: the first source pixel is
  0x11 0x18 0x1f 0xff.
- **Gamma flag.** With privateFlags bit 0 clear, image flags stay 0. With it set, they become 2.
- **Slot path.** `ret image` shows the call went through RwGlobals +0x60. The binding returns NULL when that slot is 0.

## Gates

| check | result |
|---|---|
| build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, `.asi` not deployed. mashed_re.exe sha256 `50561acbf78a25404c3837d0fe45cdf721bd0e45e5ae2b6960f0357237414611` |
| knob boot, RWMOD off | rc 0, `WinMain 0x00492370 returned 0 after 3000 frames` |
| knob boot, RWMOD on | rc 0, 3000 frames. The registration line (`knob_on_rwmod.log`:364) still lists "device slots +0x24 +0x34": the summary text was not extended, but the slot is set (the self-test above). |
| knob boot, RWMOD on + self-test | rc 0, 3000 frames |
| 12 captures, RWMOD off / on (`capflake_runs.py nav1`, `nav_rwmod`) | 8/8 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12` |
| walk_04 | `ba34030d1b7e2fe4` in all 8 rows |
| G-CTRL (H_DEF) | `8284be46 / 67931834 / 31fe043e`, unchanged |
| B (K0 / K1 / K26) | `1d5bf4f9` x3, unchanged |
| rva-lint | OK, 35 known, 0 new |

Launch setup:
- All launches ran inside one `test-slot bash -c 'mashed_lock.py run --label RWG-RASTER-BIND -- ...'`.
- The build waited for the merge train's lock.
- Roots: `D:/MashedWT/POWERUPINIT-C_root` for boots and captures, `_rootA` for gates.

**Default path.** The +0x60 slot and the self-test exist only inside `RegisterRwModules`, which runs only under
MASHED_ORIG_TOP_RWMOD. The default-path gates are unchanged.

## Open, for the consumer row (not this row)

0x00555ff0 is the first live consumer, reached once NEARMATCH-555360 runs 0x00555360 and installs 0x00555910. It reads
`tex->raster->width` / `->height` at RW 3.x +0xc / +0x10 (`re/match/s24/00555ff0.c:173-174`). On the knob path
`tex->raster` is an `rw::Raster` (`MashedRw_TextureRead_004c5cb0`, RwApiBind.cpp:734), where +0xc is `privateFlags`
and +0x10 is `format`. This binding handles its own call correctly. The caller's own width and height reads still need
an RW-layout raster view, or a resolution, in the row that ports 0x00555ff0.

Its image calls 0x004cdca0 / 0x004cdd60 / 0x004cdd00 are not bound yet (BINDINGS kind `rw`, "RW 3.x identity not
confirmed"). They must return RW-layout `RwImage`s for this binding and for 0x00555ff0's own reads.
