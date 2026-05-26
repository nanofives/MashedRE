# FUN_004cfee0 — raster per-obj D3D surface pointer reader

**RVA:** 0x004cfee0  
**Session:** batch-render-5-s4  
**U-id:** U-5178  
**Confidence promoted:** C1 → C2

## Signature
```c
undefined1 FUN_004cfee0(int *param_1);  // u8 f(RwRaster* raster)
```
Body: 0x004cfee0 .. 0x004cfefb (0x1b bytes)

## Decomp (verbatim)
```c
undefined1 FUN_004cfee0(int *param_1)
{
  if (*param_1 != 0) {
    return *(undefined1 *)(*param_1 + 8 + DAT_00911ae4);
  }
  return 0;
}
```

## Mechanical analysis

- param_1 is a pointer to a RW raster object.
- `*param_1` is the raster's plugin-data pointer (first field).
- If non-null: reads byte at `*param_1 + 8 + DAT_00911ae4`.
  - DAT_00911ae4 = plugin slot offset (registered by FUN_004cfa00).
  - Offset 8 within the plugin data block = D3D surface type byte or similar per-raster state byte.
- Returns 0 if raster has no plugin data.
- Existing C1 note: "Signature `u8 f(int* obj)`."

## Key globals
| Address | Role |
|---------|------|
| 0x00911ae4 | RW plugin slot offset |

## Callers/Callees
- Callees: none (pure load from memory).
- Callers: FUN_005412d0 (0x005412d0), FUN_00541d40 (0x00541d40) — both in the 0x541xxx texture/material subsystem.

## Evidence for C2
- Decompiler output unambiguous; existing C1 note matches.
- Consistent with sibling function FUN_004cff00 (reads byte at offset 9+param_1 instead of *param_1+8).
- Cross-check: callers in texture subsystem confirm this reads raster plugin data.
