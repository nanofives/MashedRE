# FUN_004cff00 — per-material reflectivity flag reader

**RVA:** 0x004cff00  
**Session:** batch-render-5-s4  
**U-id:** U-5179  
**Confidence promoted:** C1 → C2

## Signature
```c
byte FUN_004cff00(int param_1);  // u8 f(int raster_index)
```
Body: 0x004cff00 .. 0x004cff11 (0x11 bytes)

## Decomp (verbatim)
```c
byte FUN_004cff00(int param_1)
{
  return *(byte *)(DAT_00911ae4 + 9 + param_1) & 0xf;
}
```

## Mechanical analysis

- Reads byte at `DAT_00911ae4 + 9 + param_1`, masks with 0xf.
- DAT_00911ae4 = RW plugin slot offset (set by FUN_004cfa00 plugin registration).
- `param_1` is a per-raster integer index into the plugin data block.
- Offset 9 within plugin block = material reflectivity nibble field.
- Low nibble extracted: 0 = no reflection, non-zero = use reflection vector path in lighting shader.
- Existing C1 note confirms: "returns *(byte*)(DAT_00911ae4+9+param_1)&0xf; non-zero = use reflection vector path in LIGHT_FN".

## Key globals
| Address | Role |
|---------|------|
| 0x00911ae4 | RW plugin slot offset |

## Callers/Callees
- Callees: none (pure memory read + mask).
- Callers: FUN_00541b50 (0x00541b50..0x00541d39) — lighting subsystem.

## Evidence for C2
- Decompiler output is a single expression; fully deterministic.
- Existing C1 plate `re/analysis/render_lighting_alt/render_lighting_alt-20260503.md` confirms meaning.
- Caller is in lighting subsystem (0x541xxx range), consistent with reflectivity interpretation.
