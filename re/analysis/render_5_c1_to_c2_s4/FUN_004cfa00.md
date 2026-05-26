# FUN_004cfa00 — RW/D3D pixel-format capability table init

**RVA:** 0x004cfa00  
**Session:** batch-render-5-s4  
**U-id:** U-5176  
**Confidence promoted:** C1 → C2

## Signature
```c
undefined4 FUN_004cfa00(void);  // returns 1 on success, 0 on failure
```
Body: 0x004cfa00 .. 0x004cfdc6

## Decomp summary

- Calls `FUN_004c7690(0x24, 0x40c, &LAB_004cfdd0, &LAB_004cfdf0, 0)` — RW plugin registration (plugin ID 0x40c).
  Result stored at DAT_00911ae4. Returns 0 if result < 0.
- Zeroes 0x200 bytes at DAT_00911b00 (128 × 4-byte slots) via loop.
- Fills a table from 0x911b50 to ~0x911cd5 with 3-byte entries: `{enabled_byte, bit_depth_byte, format_word}`.
  Examples: offset +0 → {0, 0x18, 0}, {1, 0x20, 0x500}, {0, 0x20, 0x600}, etc.
  These are D3D/RW raster format capability entries (bit depth + D3DFORMAT-equivalent code).
- Returns 1.

## Key globals
| Address | Role |
|---------|------|
| 0x00911ae4 | RW plugin slot index (result of plugin registration) |
| 0x00911b00 | Start of 0x200-byte zeroed region / format table base |
| 0x00911b50 | First live table entry (format #0: depth=0x18, fmt=0) |
| 0x00911cd5 | Last table entry written |

## Callers/Callees
- Callees: FUN_004c7690 (RW plugin registration).
- Callers: 0 direct (likely called via function-pointer at engine-init time).

## Evidence for C2
- Existing C1 plate `re/analysis/rw_engine_init_d3/004cfa00.md` already characterizes this correctly.
- Decompiler output: deterministic table-fill, no ambiguous control flow.
- All DAT addresses confirmed. Consistent with existing note: "zeroes 0x200B@0x911b00; fills 0x911b50..0x911cd5 with RW raster format entries".
