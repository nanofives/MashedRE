# TXD / Texture Struct Layout — Mashed RE

**First authored:** Session texture_loader_d3_cont1 (2026-05-12)  
**Sources:** FUN_004c5a00 (RwTextureCreate), FUN_004c5ae0 (RwTextureSetName), FUN_004c5b50 (RwTextureSetMaskName), FUN_00550130 (RwTextureStreamRead), FUN_004c5890 (RwTexDictionaryCreate), FUN_004c5bc0 (RwTexDictionaryAddTexture), FUN_004cee90 (RwImageStreamRead), FUN_004cdd60 (RwImageAllocatePixels)

---

## STOP-AND-ASK note: librw verbatim match

These structs are **verbatim matches** to librw's `RwTexture`, `RwTexDictionary`, and `RwImage`. Fields are documented here in Mashed's local terms. Renaming Ghidra symbols to match librw is a master-Ghidra mutation deferred to the sweep session. Do not cite librw field names as facts — use the offset citations below.

---

## RwTexture (type tag 0x30006)

sizeof = 0x58 (88 bytes). Stored size at `DAT_007d4054 + 8 + DAT_007d3ff8`.

```c
struct RwTexture {   // 0x58 bytes
/*+0x00*/ RwRaster        *raster;          // ptr to platform raster; param to RwTextureCreate
/*+0x04*/ RwTexDictionary *dict;            // NULL on create; set by AddTexture
/*+0x08*/ RwLLLink         inDictionary;   // link into RwTexDictionary::texturesInDict list
/*+0x0c*/   // (inDictionary.next ptr at +0x08, .prev ptr at +0x0c)
/*+0x10*/ char             name[32];        // texture base name, null-terminated, max 31 chars
/*+0x30*/ char             mask[32];        // mask base name, null-terminated, max 31 chars
/*+0x50*/ RwUInt32         filterAddressing; // packed: byte+0=filter, byte+1=addrMode
/*+0x54*/ RwInt32          refCount;        // reference count, =1 on create
// (struct ends at +0x58)
};
```

### filterAddressing (+0x50) breakdown

| Byte offset within field | Field | Value on create |
|--------------------------|-------|-----------------|
| +0x50 (byte 0) | filter mode | 0x01 = `rwFILTER_NEAREST` |
| +0x51 (byte 1) | address mode (U and V packed) | 0x11 = U=1,V=1 = `rwTEXTURE_WRAP` |
| +0x52..+0x53 | not touched on create | 0 |

RW3 SDK packing: low 4 bits of byte 1 = U-address, high 4 bits of byte 1 = V-address. Both = 1 = `rwTEXTURE_WRAP`.

### Address citations for RwTexture fields

| Field | Address where set | Function |
|-------|-------------------|----------|
| +0x00 raster | 0x004c5a20 | FUN_004c5a00 |
| +0x04 dict=NULL | 0x004c5a2c | FUN_004c5a00 |
| +0x10 name[0]=0 | 0x004c5a32 | FUN_004c5a00 |
| +0x30 mask[0]=0 | 0x004c5a38 | FUN_004c5a00 |
| +0x50 clear dword | 0x004c5a1d | FUN_004c5a00 |
| +0x50 byte=1 | 0x004c5a44 | FUN_004c5a00 |
| +0x51 byte=0x11 | 0x004c5a26 | FUN_004c5a00 |
| +0x54 refCount=1 | 0x004c5a3e | FUN_004c5a00 |
| +0x10..+0x2f name | 0x004c5ae8 | FUN_004c5ae0 (strncpy 32B) |
| +0x2f name[31]=0 | 0x004c5b3c | FUN_004c5ae0 (overflow guard) |
| +0x30..+0x4f mask | 0x004c5b58 | FUN_004c5b50 (strncpy 32B) |
| +0x4f mask[31]=0 | 0x004c5bac | FUN_004c5b50 (overflow guard) |
| +0x08 inDictionary.next | 0x004c5bee | FUN_004c5bc0 (AddTexture) |

---

## RwTexDictionary (type tag 0x30016)

sizeof (estimated) = at least 0x18 bytes. Alloc size at `DAT_007d4054 + 0xc + DAT_007d3ff8`.

```c
struct RwTexDictionary {   // ≥0x18 bytes
/*+0x00*/ RwUInt32    type;            // = 6 = rwTEXDICTIONARY object type tag
/*+0x04*/ RwUInt32    subType;         // (not observed; zero on init via struct clear)
/*+0x08*/ RwLinkList  texturesInDict;  // circular sentinel; .next self-circular on create
/*+0x0c*/   // (tail ptr of texturesInDict sentinel)
/*+0x10*/ RwLLLink    inGlobalTXDs;   // link into global TXD list (head at DAT_007d4054+DAT_007d3ff8)
/*+0x14*/   // (inGlobalTXDs.prev ptr)
};
```

### Address citations

| Field | Address | Function |
|-------|---------|----------|
| +0x00 = 6 (rwTEXDICTIONARY) | 0x004c58a5 | FUN_004c5890 |
| +0x08/+0x0c sentinel init (self-circular) | 0x004c58cc..0x004c58d3 | FUN_004c5890 |
| +0x10/+0x14 link into global list | 0x004c58b4..0x004c58c9 | FUN_004c5890 |
| +0x08 texture linked via AddTexture | 0x004c5bee | FUN_004c5bc0 |

### Global TXD list root
- Head sentinel: `DAT_007d4054 + DAT_007d3ff8` (module base offset 0x00)
- Stored sizeof RwTexDictionary: `DAT_007d4054 + 0x0c + DAT_007d3ff8`

---

## RwImage (type tag 0x30018 for pixel buf)

sizeof = at least 0x1c bytes. Layout confirmed from FUN_004cdd60.

```c
struct RwImage {   // ≥0x1c bytes
/*+0x00*/ RwUInt32  flags;      // bit 0 = pixels allocated
/*+0x04*/ RwInt32   width;      // image width in pixels
/*+0x08*/ RwInt32   height;     // image height in pixels
/*+0x0c*/ RwInt32   depth;      // bits per pixel (4, 8, 16, 24, 32 etc.)
/*+0x10*/ RwInt32   stride;     // row stride in bytes (= ((depth+7)>>3)*width, aligned-4)
/*+0x14*/ RwUInt8  *cpPixels;   // pixel data buffer (alloc type 0x30018)
/*+0x18*/ RwRGBA   *palette;    // RGBA palette (for depth==4 or 8), else NULL
// refCount would be at +0x1c based on RW3 SDK but not observed here
};
```

### Stride formula (from 0x004cdd7c)
```
stride = (((depth + 7) >> 3) * width + 3) & 0xFFFFFFFC
```
I.e.: ceil(depth/8) × width, rounded up to 4-byte boundary.

### Palette allocation (from 0x004cdd6d)
- Only when depth == 4 (16 colors) or depth == 8 (256 colors).
- Palette size: `(1 << depth) * 4` bytes (RGBA32 entries).
- Palette immediately follows pixel data in the same allocation: `palette = cpPixels + height*stride`.

### Address citations

| Field | Address | Function |
|-------|---------|----------|
| +0x0c depth | 0x004cdd63 | FUN_004cdd60 |
| +0x10 stride | 0x004cdd8d | FUN_004cdd60 |
| +0x14 cpPixels | 0x004cdd99 | FUN_004cdd60 |
| +0x18 palette | 0x004cde06 | FUN_004cdd60 |
| +0x00 flags |= 1 | 0x004cde0b | FUN_004cdd60 |
| +0x04 width (read) | 0x004cdd7c | FUN_004cdd60 |
| +0x08 height (read) | 0x004cdd84 | FUN_004cdd60 |
| RwImage alloc offset 0x18 | 0x004cf03c | FUN_004cee90 |

---

## Texture plugin globals layout

At `DAT_007d4054 + DAT_007d3ff8` = texture plugin data block:

| Offset | Contents | Source |
|--------|----------|--------|
| +0x00 | global TXD list head (RwLinkList sentinel) | FUN_004c5890 inGlobalTXDs link |
| +0x08 | stored sizeof(RwTexDictionary) | FUN_004c5890 alloc call |
| +0x0c | stored sizeof(something — TXD alloc?) | FUN_004c5890 `DAT_007d4054+0xc+DAT_007d3ff8` |
| +0x08 | stored sizeof(RwTexture) | FUN_004c5a00 alloc call |
| +0x1c | current RwTexDictionary* | FUN_004c5800/5820 |
| +0x20 | current texture slot* | FUN_004c5830/5850; U-3666 |

> Note: offset +0x08 appears in two contexts above. FUN_004c5890's alloc call reads `DAT_007d4054 + 8 + DAT_007d3ff8` as the sizeof(RwTexDictionary) argument; FUN_004c5a00 reads the same slot as sizeof(RwTexture). [UNCERTAIN] — these may be two different runtime slots if the plugin data block is larger than assumed, or both could be reading stored sizes that happen to be indexed via the same formula. Resolution: read memory at `DAT_007d4054+8+DAT_007d3ff8` at runtime.

---

## Allocator vtable slots observed

| Slot | Used for | Seen in |
|------|----------|---------|
| `DAT_007d3ff8 + 0x108` | RwImage pixel buffer (0x30018) | FUN_004cdd60 |
| `DAT_007d3ff8 + 0x108` | RwImage alloc (0x30018) | FUN_004cee90 |
| `DAT_007d3ff8 + 0x118` | RwTexture (0x30006), RwTexDictionary (0x30016) | FUN_004c5a00, FUN_004c5890 |
| `DAT_007d3ff8 + 0x10c` | dealloc | FUN_004d8810 (error path) |
| `DAT_007d3ff8 + 0xd0` | strncpy (name copy) | FUN_004c5ae0, FUN_004c5b50 |
| `DAT_007d3ff8 + 0xf4` | strlen | FUN_004c5ae0, FUN_004c5b50 |

---

## Open uncertainties on this struct

- **U-2678**: vtable+0x108 vs vtable+0x118 — exact difference (pool, alignment, debug) unknown.
- **U-3666**: plugin data slot +0x20 (FUN_004c5830/5850) — no public RW3 API name; purpose within streaming unknown.
