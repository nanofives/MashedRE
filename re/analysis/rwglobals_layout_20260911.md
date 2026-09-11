# `DAT_007d3ff8` is `RwGlobals *` — the full layout, and how it was validated

**Date:** 2026-09-11
**Evidence class:** static, against the anchored `original/MASHED.exe.unpatched`
(SHA-256 `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`)
plus the vendored RenderWare headers under `re/prior_art/renderware/`.

## Verdict

`DAT_007d3ff8` holds a pointer to RenderWare's `RwGlobals`, and the block it points at is
the static one at `DAT_007d3ec8`. Every `*(DAT_007d3ff8) + N` dispatch in the codebase is a
named field of that struct. This closed 20 `UNCERTAINTIES.md` rows that had all been asking
the same question in different words.

The struct definition is `struct RwGlobals` at
`re/prior_art/renderware/gta-reversed-modern/source/game_sa/RenderWare/rw/rwplcore.h:6281`.
CLAUDE.md records that these headers apply directly to this engine.

## Layout (non-`RWDEBUG` build)

| Offset | Field | Type / notes |
|---|---|---|
| `+0x00` | `curCamera` | `void *` |
| `+0x04` | `curWorld` | `void *` |
| `+0x08` | `renderFrame` | `RwUInt16` |
| `+0x0a` | `lightFrame` | `RwUInt16` |
| `+0x0c` | `pad[2]` | two `RwUInt16` |
| `+0x10` | **`dOpenDevice`** | `RwDevice`, 14 fields, `0x38` bytes, spans `+0x10..+0x47` |
| `+0x48` | **`stdFunc[29]`** | `RwStandardFunc`, spans `+0x48..+0xbb` |
| `+0xbc` | `dirtyFrameList` | `RwLinkList` = one `RwLLLink` = two pointers |
| `+0xc4` | **`stringFuncs`** | `RwStringFunctions`, 17 pointers, spans `+0xc4..+0x107` |
| `+0x108` | `memoryFuncs.rwmalloc` | `void *(*)(size_t size, RwUInt32 hint)` |
| `+0x10c` | `memoryFuncs.rwfree` | `void (*)(void *mem)` |
| `+0x110` | `memoryFuncs.rwrealloc` | `void *(*)(void *, size_t, RwUInt32)` |
| `+0x114` | `memoryFuncs.rwcalloc` | `void *(*)(size_t, size_t, RwUInt32)` |
| `+0x118` | `memoryAlloc` | `void *(*)(RwFreeList *fl, RwUInt32 hint)` |
| `+0x11c` | `memoryFree` | `RwFreeList *(*)(RwFreeList *fl, void *pData)` |
| `+0x120` | `metrics` | `RwMetrics *` |
| `+0x124` | `engineStatus` | enum: NONE 0, INITED 1, OPENED 2, STARTED 3 |
| `+0x128` | `resArenaInitSize` | `RwUInt32` |

**Total size `0x12c` = 300 bytes.**

### `dOpenDevice` sub-offsets (add `0x10`)

| Offset | Field |
|---|---|
| `+0x10` | `gammaCorrection` |
| `+0x14` | `fpSystem` |
| `+0x18` | `zBufferNear` |
| `+0x1c` | `zBufferFar` |
| `+0x20` | **`fpRenderStateSet`** |
| `+0x24` | `fpRenderStateGet` |
| `+0x28` | `fpIm2DRenderLine` |
| `+0x2c` | **`fpIm2DRenderTriangle`** |
| `+0x30` | `fpIm2DRenderPrimitive` |
| `+0x34` | `fpIm2DRenderIndexedPrimitive` |
| `+0x38` | `fpIm3DRenderLine` |
| `+0x3c` | `fpIm3DRenderTriangle` |
| `+0x40` | `fpIm3DRenderPrimitive` |
| `+0x44` | `fpIm3DRenderIndexedPrimitive` |

### `stdFunc` slots observed in the binary

Index is `(offset - 0x48) / 4`. `RwStandardFunc` is
`RwBool (*)(void *pOut, void *pInOut, RwInt32 nI)` — **3 arguments**, which is the arity
check used to validate each one.

| Offset | Index | Name | Seen at |
|---|---|---|---|
| `+0x58` | 4 | `rwSTANDARDRASTERCREATE` | (by layout) |
| `+0x5c` | 5 | `rwSTANDARDRASTERDESTROY` | `0x004c766a` |
| `+0x78` | 12 | `rwSTANDARDRASTERSUBRASTER` | `0x004c7788`-ish, U-4985 |
| `+0xa4` | 23 | `rwSTANDARDRASTERLOCKPALETTE` | `0x004c76d4` |
| `+0xa8` | 24 | `rwSTANDARDRASTERUNLOCKPALETTE` | `0x004c762f` |

## How it was validated — five arity checks plus three size checks

The layout table alone is not evidence: GTA:SA's RW build could differ, and an `RWDEBUG`
build shifts every field by 12 bytes. So each anchor was confirmed from the instruction
stream independently.

**Arity and semantics:**

1. `+0x20` is called with exactly **2** stack arguments at every site — matches
   `fpRenderStateSet(state, value)`. E.g. `0x00489200 push 8` / `0x004891fe push 0` /
   `0x00489202 call dword ptr [eax + 0x20]`.
2. `+0x2c` is called with exactly **5** — matches
   `RwIm2DRenderTriangle(verts, numVertices, v1, v2, v3)`. The pair at `0x00496bbb` and
   `0x00496bd1` passes the vertex triples `(0,1,2)` and `(2,3,0)`: the two triangles of a
   quad over 4 vertices.
3. `+0x18` is read as a **float** and copied into vertex records at `0x00496a72`,
   `0x00496aa4`, `0x00496adc` — `zBufferNear`. A function pointer could never be used this
   way, which rules out the alternative alignments.
4. `+0x108` is called with **2** arguments whose second is a tag —
   `rwmalloc(size, hint)`. Example `(0x24, param_6 & 0xff0000)` at `0x005540d0`.
5. `+0x10c` is called with exactly **1** pointer argument and `add esp,4` —
   `rwfree(void *mem)`. Sites: `0x00553fe6`, `0x004c5e17`.

**Size:**

6. Both `MOVSD.REP` sites that populate `DAT_007d3ec8` are preceded by `mov ecx,0x4b`
   (`0x004c306c` and `0x004c31c0`). `0x4b` = 75 dwords = **300 bytes** = `0x12c`. The copy
   length equals the computed struct size, at two independent sites.
7. `0x007d3ec8 + 0x12c` = `0x007d3ff4`, and `0x004c3082 mov eax,dword ptr [0x7d3ff4]` reads
   a *different* global sitting immediately past the block, with the self-pointer
   `DAT_007d3ff8` 4 bytes after that. The packing admits no other size.
8. The four `stdFunc` slots each take the 3 arguments the typedef declares, and each one's
   surrounding code matches its name: the unlock-palette slot is followed by
   `0x004c763b and al,0xe7` clearing the raster's lock bits; the lock-palette slot is passed
   a `lea`-derived out-pointer (`0x004c76ce`); the destroy slot is preceded by a registry
   deregistration (`0x004c765b call 0x4d8060`); the sub-raster slot copies dimensions from a
   parent object into the child (`0x004c7778`/`0x004c777c`).

## Scope limit — do NOT name indexed slots from this table

There are two distinct dispatch forms in this codebase and they must not be conflated:

- **Direct**: `*(DAT_007d3ff8) + N`. Covered by the table above.
- **Indexed**: `*(DAT_007d3ff8) + DAT_007d4054 + N`, e.g.
  `0x004c5de5 call dword ptr [eax + edx + 0x2c]`.

`DAT_007d4054` is **not** a static constant. Its single write is
`0x004c5f7b MOV [0x007d4054],EAX`, inside a registration call that also pushes the tag
`0x40006` — EAX is a runtime value. So the indexed form addresses a **plugin/extension
block appended past the 0x12c struct**, and its slot numbers cannot be resolved statically.

This was nearly an error: a plausible-looking chain made
`*(DAT_007d3ff8) + DAT_007d4054 + 0x2c` look like `stdFunc[6]`
(`rwSTANDARDIMAGEGETRASTER`), which would have fit the observed behaviour of
`FUN_004c7200` beautifully. Measuring `DAT_007d4054` instead of assuming it is what
prevented publishing that. `DAT_007d40a8` (`0x004c7672`) is the same pattern.

What *is* assertable about the indexed table is only what the initialiser literally writes:
`0x004c60f6 mov dword ptr [edx + eax + 0x2c],0x4c7200` puts `FUN_004c7200` in that slot,
and `0x004c610a` puts `FUN_004c6140` in `+0x30`.

## Knock-on corrections

- **Render states are RenderWare's, not D3D9's.** Several rows planned to look their
  first argument up in `D3DRENDERSTATETYPE`. `fpRenderStateSet` takes `enum RwRenderState`
  (`rwplcore.h:5069`): NA 0, TEXTURERASTER 1, TEXTUREADDRESS 2, ADDRESSU 3, ADDRESSV 4,
  TEXTUREPERSPECTIVE 5, ZTESTENABLE 6, SHADEMODE 7, ZWRITEENABLE 8, TEXTUREFILTER 9,
  SRCBLEND 10, DESTBLEND 11, VERTEXALPHAENABLE 12, BORDERCOLOR 13. Using the D3D9 enum
  would have mis-named every one — D3D9's `D3DRS_ZWRITEENABLE` is 14, not 8.
- **The memory "tags" are RenderWare memory hints.** `rwmalloc`'s second parameter is a
  documented hint, which is why the `0x30xxx`/`0x40xxx` family has a meaningful upper byte
  and why the allocator masks with `0xff0000` (U-5153, U-5621).
- **There are two different deallocators.** `+0x10c` `rwfree` takes a bare pointer;
  `+0x11c` `memoryFree` takes an `RwFreeList *` and returns it. Code choosing between them
  is choosing heap-vs-arena ownership, not duplicating a call (U-5106).
- **A `JMP` trampoline still forwards arguments.** `FUN_005aea00` tail-jumps to `+0x108`;
  declaring it `void(void)` because it "does no parameter handling" would drop
  `rwmalloc`'s two arguments and its return value (U-0125).

---

# Addendum: `RwRaster`, and the real `IDirect3DDevice9` vtable

Added 2026-09-11, same session. These came from the same two reference sources and closed
six more rows — including a **correction to U-5160 that I had landed earlier the same day**.

## `RwRaster` (`rwcore.h:1622`)

| Offset | Field | Notes |
|---|---|---|
| `+0x00` | `parent` | `RwRaster *`, top-level raster if this is a sub-raster |
| `+0x04` | `cpPixels` | pixel pointer when locked |
| `+0x08` | `palette` | |
| `+0x0c` | `width` | |
| `+0x10` | `height` | |
| `+0x14` | `depth` | |
| `+0x18` | `stride` | |
| `+0x1c` / `+0x1e` | `nOffsetX` / `nOffsetY` | `RwInt16`, sub-raster offset |
| `+0x20` | **`cType`** | `enum RwRasterType` (`:1506`), mask `0x07` |
| `+0x21` | **`cFlags`** | `rwRASTERDONTALLOCATE = 0x80` |
| `+0x22` | **`privateFlags`** | `enum RwRasterPrivateFlag` (`:1588`) |
| `+0x23` | **`cFormat`** | format **>> 8** — see below |
| `+0x24`.. | `originalPixels`, `originalWidth`, `originalHeight`, `originalStride` | |

**`cFormat` holds the format shifted down by 8.** `rwRASTERFORMATMASK = 0xff00` is
documented as "the whole format" while the field is one byte, so bit *n* of `cFormat` is
format bit *n+8*:

- `0x10` maps to `0x1000` `rwRASTERFORMATAUTOMIPMAP` ("RenderWare generated the mip levels")
- `0x80` maps to `0x8000` `rwRASTERFORMATMIPMAP` ("mip mapping on")
- `0x40` maps to `0x4000` `PAL4`, `0x20` maps to `0x2000` `PAL8`
- low nibble is the pixel format (`rwRASTERFORMATPIXELFORMATMASK = 0x0f00`)

**`privateFlags` (`+0x22`)**: `rwRASTERGAMMACORRECTED` `0x01`, `PIXELLOCKEDREAD` `0x02`,
`PIXELLOCKEDWRITE` `0x04`, `PALETTELOCKEDREAD` `0x08`, `PALETTELOCKEDWRITE` `0x10`,
`PIXELLOCKEDRAW` `0x20`.

**`cType` (`+0x20`)**: `NORMAL` 0, `ZBUFFER` 1, `CAMERA` 2, `TEXTURE` 4, `CAMERATEXTURE` 5,
mask `0x07`; plus `rwRASTERPALETTEVOLATILE 0x40` and `rwRASTERDONTALLOCATE 0x80` in the same
flag space.

These decode several previously-opaque masks exactly:

- `0x004c763b and al,0xe7` on `+0x22` clears `0x08` and `0x10` — the two **palette** lock
  bits, immediately after the unlock-palette `stdFunc` call. The mask matches the slot.
- `0x004d033a and al,0xf9` on `+0x22` clears `0x02` and `0x04` — the two **pixel** lock bits.
- `0x004d030c test al,4` on `+0x22` is `PIXELLOCKEDWRITE`.
- `0x004d0491 and al,7` then `cmp al,5` on `+0x20` is
  `(cType & rwRASTERTYPEMASK) == rwRASTERTYPECAMERATEXTURE`.
- `0x004d532d test byte ptr [edi],2` on an `RwImage` (`rwIMAGEGAMMACORRECTED`) followed by
  `0x004d5332 or byte ptr [esi + 0x22],1` on the raster (`rwRASTERGAMMACORRECTED`) is a
  clean image-to-raster gamma-flag propagation, which confirms both names from opposite
  directions.

### The U-5160 correction

Earlier the same day I landed a statement that `+0x23` was an unnamed flags byte and that
bit `0x10` meant "contents must be read back into an image, **not** either label the row
proposed". That was wrong. `+0x23` is `cFormat` and bit `0x10` is
`rwRASTERFORMATAUTOMIPMAP`, so the row's own "has mipmaps" reading was substantially right.
The gate reads: *on pixel unlock, if the format says RenderWare generated the mip levels,
regenerate them* — which is exactly what `hooks.csv` already said for `FUN_004d0290`
("calls FUN_004c5dd0 on mipmap invalidation"), a note I had discounted as possibly a guess.
The mechanics in the earlier entry stand; only the naming was wrong.

I reached "neither label is supported" from call-graph mechanics while the field's name was
sitting in a vendored header on disk. **Check `re/prior_art/renderware/` before concluding
that a RenderWare field is unnameable.**

## `IDirect3DDevice9` vtable

Enumerated from the Windows SDK header
`C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/shared/d3d9.h` by matching every
`STDMETHOD` / `STDMETHOD_` declaration inside `DECLARE_INTERFACE_(IDirect3DDevice9)`.
Slot = `offset / 4`. `DAT_007d4110` holds the device (U-0007, via `FUN_004caea0`).

**Parse hazard, hit and fixed here:** a regex that handles only `STDMETHOD(Name)` silently
drops the `STDMETHOD_(type, Name)` forms — `GetAvailableTextureMem`, `SetCursorPosition`,
`ShowCursor`, `GetNumberOfSwapChains`, `SetGammaRamp`, `GetGammaRamp`. That yields **115**
methods and shifts every slot after the first omission, which made `+0xe4` look like
`EndStateBlock`. The correct count is **119**, and it reproduces `+0xe4` = `SetRenderState`,
agreeing with U-0007. If a device-slot name disagrees with an already-established one,
suspect the enumeration before the established fact.

| Offset | Slot | Method | Confirmed by |
|---|---|---|---|
| `+0x014` | 5 | `EvictManagedResources` | see U-5190: the swap-chain `+0x14` is a *different* interface |
| `+0x094` | 37 | `SetRenderTarget` | 2 args; cache is an array indexed by arg 1 (`0x004c7a28`) |
| `+0x09c` | 39 | `SetDepthStencilSurface` | 1 arg; single-value cache `DAT_007d4574` |
| `+0x0b0` | 44 | `SetTransform` | 2 args; `push 0x100` = `D3DTS_WORLD`; arg 2 is a 16-dword `rep movsd` block = 64-byte `D3DMATRIX` |
| `+0x0e4` | 57 | `SetRenderState` | 2 args (U-0007) |
| `+0x104` | 65 | `SetTexture` | |
| `+0x178` | 94 | `SetVertexShaderConstantF` | 3 args (`0x004cbb17`), which is why U-5033's "third argument" was never a mystery |

**Method note that generalises:** every one of these was confirmed by **argument count**
against the header signature, not by the slot arithmetic alone. Two rows in this batch had
guessed a name the arity contradicted — U-4988 guessed `SetStreamSource` (slot 96, 29 slots
away) and U-5033 guessed `SetTexture` (2 args versus the 3 actually pushed). In U-5033 the
disagreement had even been *recorded* in the row, as a puzzle about a mystery parameter
rather than as evidence the name was wrong. When the arity disagrees with the assumed
signature, the name is the thing to doubt.
