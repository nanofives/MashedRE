# FUN_004c5ca0 — C1→C2 plate (batch-render-4-s3)

**RVA:** 0x004c5ca0  
**Body:** 004c5ca0..004c5caf  
**U-id:** U-4955

## Signature (Ghidra)
```c
undefined4 FUN_004c5ca0(void);
```

## Decompilation (verbatim)
```c
/* [C1 2026-05-19] Tiny accessor. Returns `*(undefined4*)(DAT_007d4054 + 0x10 + */

undefined4 FUN_004c5ca0(void)
{
  return *(undefined4 *)(DAT_007d4054 + 0x10 + DAT_007d3ff8);
}
```

## Mechanical analysis
- Reads and returns `*(DAT_007d4054 + 0x10 + DAT_007d3ff8)`.
- DAT_007d4054 is a plugin-data offset; DAT_007d3ff8 is the RW engine base; offset 0x10 selects a sub-field.
- No callees; 5 callers: FUN_0053fec0, FUN_0054f8d0, FUN_00554390, FUN_00555af0, FUN_00555ff0.
- Simple getter, slot at offset +0x10 (different from FUN_004c5830/5850 at +0x20).

## Evidence for C2
- Body fully visible; trivial load + return.
- Constants: offset 0x10 (@ 004c5ca4), DAT_007d4054 (@ 004c5ca2), DAT_007d3ff8 (@ 004c5ca8).
- 5 callers confirm live, well-used accessor.
- No guessing; mechanical description only.
