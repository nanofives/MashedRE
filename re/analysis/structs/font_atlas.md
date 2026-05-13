# Font Subsystem Struct Layout — GlyphEntry and FontCtx

**First authored:** struct_extract_phase5_pt2-20260513  
**Sources:** FUN_00555af0 (FontCtx_LoadMetrics_Met), FUN_00555ff0 (FontCtx_LoadMetrics_Atlas), FUN_005571e0 (FontSys_InitFontPool), FUN_00557250 (FontSys_InitDataPools), FUN_00556d20 (FontSys_InitBuffers), FUN_00555f20 (FontCtx_BuildExtTable)  
**Session evidence:** font_text_cont1-20260512 (13 plates)

---

## GlyphEntry (stride 0x20 = 32 bytes)

Confirmed from both metrics parsers (`FUN_00555af0` and `FUN_00555ff0`). Layout is identical between the text-format `.met` path and the bitmap-atlas scan path.

```c
struct GlyphEntry {   // 0x20 bytes (stride)
/*+0x00*/ float  advance;      // advance width ratio = glyph_w / line_h (text) or (strip_w-3)/(strip_h-3) (atlas)
/*+0x04*/ void  *tex_handle;   // RwTexture* for the atlas page containing this glyph
/*+0x08*/ float  u_left;       // UV left edge = (x + 0.5f) / tex_width
/*+0x0c*/ float  v_top;        // UV top edge  = (y + 0.5f) / tex_height
/*+0x10*/ float  u_right;      // UV right edge = (x + w - 0.5f) / tex_width
/*+0x14*/ float  v_bottom;     // UV bottom edge = (y + h - 0.5f) / tex_height
/*+0x18*/ int    unused;       // not written by .met parser (0); [UNCERTAIN U-3684 for atlas path]
/*+0x1c*/ byte   page_index;   // atlas page index (0-based); set to local_54 in atlas path
// (3 bytes padding to stride 0x20)
};
```

### UV half-pixel constants

| Address | Value | Use |
|---------|-------|-----|
| `DAT_005cd088` | 0.5f | Added to x, y for left/top UV edge |
| `DAT_005cc32c` | −0.5f | Applied to x+w, y+h for right/bottom UV edge |

### Address citations

| Field | Address where written | Function |
|-------|-----------------------|----------|
| `+0x00` advance | per-glyph in .met loop | 0x00555af0 |
| `+0x04` tex_handle | per-glyph | 0x00555af0, 0x00555ff0 |
| `+0x08` u_left | per-glyph | 0x00555af0, 0x00555ff0 |
| `+0x0c` v_top | per-glyph | 0x00555af0, 0x00555ff0 |
| `+0x10` u_right | per-glyph | 0x00555af0, 0x00555ff0 |
| `+0x14` v_bottom | per-glyph | 0x00555af0, 0x00555ff0 |
| `+0x1c` page_index | atlas path only | 0x00555ff0 |

---

## FontCtx (font context; no confirmed sizeof yet)

Accessed as `undefined4 *` (int/ptr array). Byte offsets derived from int-index × 4.

```c
struct FontCtx {
/*+0x00*/ undefined4  field_0;           // purpose unknown; FUN_004c5ca0() return saved here (TXD save/restore context)
/*+0x04*/ float       line_height;       // set to parsed line_h from .met (int-index 1)
/*+0x08*/ float       char_aspect;       // char_width / line_height (int-index 2)
/*+0x0c*/ undefined4  field_3;           // set to 0 on finalize (int-index 3)
/*+0x10*/ int         ext_char_flags;    // |= 2 when any glyph char_code >= 0x80 (int-index 4)
// +0x14..+0x23: unknown gap
/*+0x24*/ short       ascii_glyph[0x80]; // ASCII glyph-index lookup; index by char_code (0..0x7F); each entry is 2 bytes
// (0x100 bytes for 128 entries)
// +0x124: extended char table
/*+0x124*/ int *      ext_table_base;    // pointer to extended char glyph-index array (ctx+0x124)
/*+0x128*/ int        ext_table_size;    // capacity of extended char table (ctx+0x128)
/*+0x12c*/ int *      ext_table_ptr;     // working pointer into extended char table (ctx+0x12c)
/*+0x130*/ int        glyph_count;       // total glyph count (int-index 0x4c)
/*+0x134*/ void *     glyph_buf_handle;  // GlyphBuf handle (int-index 0x4d); passed to GlyphBuf_Resize / GlyphBuf_GetBase
};
```

### ASCII glyph table (at +0x24)

`*(short*)(font_ctx + char_code*2 + 0x24) = glyph_index`  
For `char_code < 0x80` (ASCII range only).  
Extended char codes (≥ 0x80) use `DAT_0091325c` temp table rebuilt by `FontCtx_BuildExtTable`.

### Address citations

| Field | Evidence |
|-------|---------|
| `+0x04` line_height | `font_ctx[1] = (float)line_height` in 0x00555af0 finalize block |
| `+0x08` char_aspect | `font_ctx[2] = (float)char_width / line_height` in 0x00555af0 |
| `+0x0c` = 0 | `font_ctx[3] = 0` in 0x00555af0 finalize |
| `+0x10` ext_char_flags | `font_ctx[4] |= 2` at 0x00555af0 for ext char |
| `+0x24` ascii_glyph | `*(short*)(param_1 + char_code*2 + 0x24) = glyph_index` at 0x00555af0 |
| `+0x124..+0x12c` | `ctx+0x124/128/12c` cited in font_text_cont1 SESSION_END (BuildExtTable output) |
| `+0x130` glyph_count | `font_ctx[0x4c] = glyph_count` in 0x00555af0 finalize |
| `+0x134` glyph_buf_handle | `font_ctx[0x4d]` passed to GlyphBuf_Resize / GlyphBuf_GetBase |

---

## Font subsystem global addresses

| Address | Role | Source |
|---------|------|--------|
| `DAT_00912a00` | 120-byte pool free list handle (GlyphNode pool?); [UNCERTAIN U-3684] | 0x00556d20 |
| `DAT_00912a04` | Vertex buffer base (0x2400 = 9216 bytes); confirmed by FontSys_InitBuffers | 0x00556d20 |
| `DAT_00912a20` | Font atlas RwTexDictionary handle; set as current TXD during glyph load | 0x00555af0, 0x00555ff0 |
| `DAT_00912a2c` | Font-object free list handle; 256-byte entries | 0x005571e0 |
| `DAT_00912a34` | 8-byte pool free list handle (FontSys_InitDataPools pool 1) | 0x00557250 |
| `DAT_00912a38` | 20-byte pool free list handle (FontSys_InitDataPools pool 2) | 0x00557250 |
| `DAT_0091325c` | Temp extended char code table (global ptr); reset after BuildExtTable | 0x00555af0, 0x00555ff0 |
| `DAT_00913260` | Extended char table capacity | 0x00555af0 |
| `DAT_00623f68` | Font pool initial count (read by InitFontPool) | 0x005571e0 |
| `DAT_00623f6c` | Font pool growth increment (read by InitFontPool) | 0x005571e0 |
| `DAT_007dc81c` | RwFreeList list head for font-object pool | 0x005571e0 |

---

## Font init chain (confirmed order)

From font_text_cont1 session discovery:

1. `FontSys_InitDataPools` (pos 1) — creates 8B + 20B pools at `DAT_00912a34/a38`
2. `FontSys_InitGlyphNode` (pos 2) — not yet analyzed
3. `FontSys_InitBuffers` (pos 3) — creates vertex buffer + 120B pool at `DAT_00912a04/a00`
4. Unknown (pos 4)
5. `FontSys_InitFontPool` (pos 5) — creates 256B font-object pool at `DAT_00912a2c`
6. `FontSys_InitRenderState` (pos 6) — not yet analyzed
7. `FontSys_SetActiveCamera` (pos 7) — not yet analyzed

---

## Uncertainties

| ID | Question |
|----|---------|
| U-3684 | `DAT_00912a00` 120-byte pool — entry struct and usage not identified |
| U-3685 | `VFS_Open` flag bit semantics vs `VFS_Open_Impl` |
| U-3724 | FontCtx +0x14..+0x23 gap (20 bytes) — unknown fields; not touched by LoadMetrics |
| U-3725 | FontCtx `field_0` (+0x00) — TXD save/restore usage suggests it stores `RwTexDictionaryGetCurrent()` return; but `font_ctx[0]` may be a different local vs ctx field |
