# Mashed TXD On-Disk Format

**First authored:** Milestone B3 (2026-05-17)
**Source-of-truth:** `FUN_0054f8d0` (Mashed's TXD reader for chunk-id 0x23) and
                     `FUN_0054fd60` (per-texture reader for the simple device-0 path),
                     decompiled live in this session via the Ghidra MCP.
**Verified against:** `original/TOASTART/Common/Frontend.piz` entry 0
                     (`TEXTURES.TXD`, 1,031,400 bytes, 8 textures).

---

## Overview

Mashed stores its texture dictionaries in `.TXD` files (e.g.
`TEXTURES.TXD` inside `Frontend.piz`). The on-disk layout is a hybrid of the
standard RenderWare 3.x RWS chunk stream and an internal, undocumented
"per-texture wrapper" that holds the metadata directly rather than wrapping it
in a TEXTURE_NATIVE (0x15) chunk.

The CONTAINING chunk has section id **0x23**, NOT the standard
`rwID_TEXDICTIONARY = 0x16`. Inside, the format is recognisable as
"RW3-derived" because:

- Every nested chunk uses the standard 12-byte RW header (id, size, version).
- The version field is the standard packed RW3 form (0x1803FFFF → decodes to
  RW 3.6.0.3 / build 65535) and is range-checked by Mashed's `RwStreamFindChunk`
  re-impl at 0x004cc5e0.
- IMAGE chunks (id=0x18 = `rwID_IMAGE`) and TEXTURE chunks (id=0x06 =
  `rwID_TEXTURE`) are stock RW3 sub-formats.

What's non-standard:

- No outer `rwID_TEXDICTIONARY` chunk; the root chunk id is 0x23.
- No outer `TEXTURE_NATIVE` (0x15) chunk per texture. Instead, each texture
  emits raw metadata (4 bytes: numMips) followed by N stock IMAGE chunks and
  one TEXTURE chunk — back-to-back.
- The top-level 4-byte `(numTex, deviceId)` field at offset 0x0C is NOT inside
  a STRUCT (1) chunk wrapper; it's raw uint32 data.

This is read at runtime by `FUN_0054f8d0`, which is dispatched from
`FUN_004b3d80` when `FUN_0042a530` (the asset-locator) returns slot 1 — i.e.
this format is the "slot-1 piz" texture dictionary specifically.

---

## Layout (Frontend.piz's TEXTURES.TXD, deviceId=1 path)

```
+0x0000  12B   ROOT CHUNK HEADER
              id      = 0x00000023               (Mashed TXD)
              size    = uint32 (file_size - 12)
              version = 0x1803FFFF               (RW 3.6.0.3 packed)

+0x000C  4B    numTex_deviceId
              low  uint16 = numTextures
              high uint16 = deviceId             (1 for the slot-1-piz format)

+0x0010   For each of `numTextures` textures, back-to-back, no padding:

          4B    numMips                          (uint32)

          For each of `numMips` mip levels:
            12B   IMAGE chunk header (id=0x18, size, version=0x1803FFFF)
            12B   STRUCT chunk header (id=0x01, size=0x10, version=0x1803FFFF)
            16B   STRUCT payload      width, height, depth, stride  (uint32 x 4)
            (stride * height) bytes  pixel data (linear, top-down)
            ((1 << depth) * 4) bytes palette     ONLY if depth < 9 (paletted formats)

          12B   TEXTURE chunk header (id=0x06, size, version=0x1803FFFF)
          12B     STRUCT chunk header (id=0x01, size=0x04, version)
           4B     STRUCT payload  filterAddressing (uint32; packed RW filter+addrU+addrV)
          12B     STRING chunk header (id=0x02, size=N, version) — texture name
           NB     STRING payload  null-padded ASCII (length rounded up to 4)
          12B     STRING chunk header (id=0x02, size=M, version) — mask name (usually empty)
           MB     STRING payload
          12B     EXTENSION chunk header (id=0x03, size=K, version) — usually K=0
```

End-of-file is exactly `0x0C + root.size`.

### What "deviceId" controls

`FUN_0054f8d0` has two branches:

- `deviceId == 0` → SIMPLE path: each texture is one chunk of 0x48 bytes of
  metadata (name[32], mask[32], numMips, filterAddressing) followed directly
  by N IMAGE chunks. Frontend.piz does NOT use this path.
- `deviceId != 0` → COMPLEX path (the one Frontend.piz uses): N IMAGE chunks
  with no metadata header up front (just 4 bytes for numMips), then a stock
  RW3 TEXTURE chunk for the name/mask/filter at the end.

For B3 we only implement the COMPLEX path because that's what the Frontend.piz
asset uses. The SIMPLE path can be added if/when a deviceId=0 file is observed.

### Pixel format

`depth` is bits-per-pixel. Observed values in `TEXTURES.TXD`:

- 8 — paletted (palette follows pixels, 256 entries × 4 bytes RGBA each)
- 32 — direct color (RGBA8888, no palette)

`stride` is the row stride in bytes. Mashed uses RW3's stride formula
(`stride = (((depth + 7) >> 3) * width + 3) & ~3`) but stores the resolved
stride directly in the on-disk struct.

### Filter / addressing (in the TEXTURE chunk's STRUCT subchunk)

The 4-byte `filterAddressing` field is the RW3 packed filter/addressing word:

- byte 0: filter mode (1 = nearest, 2 = linear, etc.)
- byte 1: address mode (low nibble = U, high nibble = V; 1 = wrap)
- byte 2 (extended filter, optional)
- byte 3 (reserved)

Observed value in every Frontend.piz texture: `0x00001104` (filter=4=mip-nearest?,
addrU=1, addrV=1). Documented at 0x004c5a00 (RwTextureCreate sets defaults
0x01 / 0x11, so 0x04/0x11 here is a later override).

---

## Address citations (FUN_0054f8d0, COMPLEX path)

| Operation | RVA |
|-----------|-----|
| Read 4-byte (numTex, deviceId) header | 0x0054f8e0 (FUN_004cbd30, 4 bytes) |
| Branch on `deviceId == 0` | 0x0054f923 |
| Read 4-byte numMips for one texture | 0x0054f9a0 (FUN_004cbd30, 4 bytes) |
| `FindChunk(0x18)` for each mip | 0x0054f9f4 (FUN_004cc5e0, type=0x18) |
| Per-mip `RwImageStreamRead` | 0x0054fa01 (FUN_004cee90) |
| `FindChunk(6)` for TEXTURE chunk | 0x0054fa2e (FUN_004cc5e0, type=6) |
| `RwTextureStreamRead` | 0x0054fa45 (FUN_00550130) |
| Read width from RwImage[0] (+4) | 0x0054fa66 |
| Read height from RwImage[0] (+8) | 0x0054fa6b |
| Read depth from RwImage[0] (+0xC) | 0x0054fa70 |
| `RwImage` payload layout | FUN_004cee90 (re/analysis/texture_loader_d3/0x004cee90.md) |

---

## Verified texture inventory (`TEXTURES.TXD`)

| # | Name | numMips | w | h | depth | format |
|---|------|---------|---|---|-------|--------|
| 0 | skydomegs    |  6 |   32 |   32 |  8 | paletted RGBA8 |
| 1 | Shad_groundgs |  8 |  128 |  128 |  8 | paletted RGBA8 |
| 2 | Shad_car     |  9 |  256 |  256 | 32 | ARGB8888       |
| 3 | Shad_bild    |  9 |  256 |  256 | 32 | ARGB8888       |
| 4 | rendergs     |  7 |   64 |   32 | 32 | ARGB8888       |
| 5 | main         |  9 |  256 |  256 |  8 | paletted RGBA8 |
| 6 | Gravel       |  8 |  128 |  128 | 32 | ARGB8888       |
| 7 | carbase      |  9 |  256 |  256 |  8 | paletted RGBA8 |

End offset matches file size (0xFBCE8) exactly.
