# FUN_004c6ba0 — C1→C2 plate (batch-render-4-s3)

**RVA:** 0x004c6ba0  
**Body:** 004c6ba0..004c71fe  
**U-id:** U-4962

## Signature (Ghidra)
```c
int * FUN_004c6ba0(int param_1, char *param_2);
```

## Decompilation (verbatim, ~180 lines — within 300-line cap)
Key structure:

```c
/* [C1 2026-05-19] Signature `int* f(name1, name2)`. Entry-point for texture load with */
int * FUN_004c6ba0(int param_1, char *param_2)
{
  // Builds path strings local_500, auStack_600 (256 bytes each) from param_1/param_2 via vtable 0xd0/0xf4
  // Determines uVar8 (format flags): 4, 0x8004, or 0x9004 based on texture plugin slots +0x1c and +0x20
  // Calls optional pre-load callback: *(DAT_007d4054+0x30+DAT_007d3ff8)(local_500, auStack_600, 0, uVar8)
  // iVar3 = FUN_004c6750(local_500, auStack_600, uVar8, &uStack_650, &uStack_658, ...) → raw image
  // iVar4 = FUN_004c77c0(w, h, depth, flags) → platform raster
  // If mipmapped (uStack_654 high byte negative, no flag 0x1000):
  //   For each mip level 1..iStack_668: rebuild path + call pre-load + FUN_004c7860 + FUN_004c6750 per mip
  //   If palette (flags & 0x60): FUN_004c6580 quantize; FUN_004cefd0 per mip
  //   For each mip: FUN_004d5310 to bind to raster
  // Else (flag 0x1000 or no mips): FUN_004cefd0 + FUN_004d5310
  // Allocates texture node: vtable 0x118 call; fills: name(+4, 0x20), dict-name(+0xc, 0x20)
  //   FUN_004d8000(&DAT_00618138, node) — inserts into global texture registry
  // Returns node pointer (or NULL on failure)
}
```

## Mechanical analysis
- `param_1`: texture base name (int / RW string).
- `param_2`: optional dict/group name.
- Format decision: plugin slot +0x1c (non-zero → 0x8004) + slot +0x20 (non-zero → add 0x9000) or default 4.
- Calls pre-load hook at plugin slot +0x30 (@ 004c6bad).
- FUN_004c6750 performs actual file-load + resize for each mip level.
- FUN_004c77c0 allocates platform raster from dimensions.
- Mip count from FUN_004c76f0(iVar4).
- Per-mip: path rebuilt + pre-load again + FUN_004c7860(set mip) + FUN_004c6750.
- Palette images: FUN_004c6580(out_palette, orig, mip_array, mip_count, bits).
- FUN_004d5310: bind raw image to raster mip slot.
- Node allocation via vtable 0x118 (@ 004c6fb0); fields at +0, +4, +0xc, +0x14, +0x15, +0x51.
- Name fields: texture name at node+4 (0x20 chars), dict name at node+0xc (0x20 chars).
- Registry insert: FUN_004d8000(&DAT_00618138, node) (@ 004c6fdd).
- 13 callees; 0 Ghidra callers (likely called via plugin vtable or export).
- Large but fully traceable.

## Evidence for C2
- Body fully visible; all major paths (simple, mipped, palette) traced.
- Stack frame: 0x668 bytes (local_500[255] + auStack_600[255] + locals).
- Format constants: 4 (@ 004c6bd0), 0x8004 (@ 004c6bd8), 0x9004 (@ 004c6be0).
- Vtable offsets: 0xd0 (@ 004c6ba3), 0xf4 (@ 004c6ba8), 0x118 (@ 004c6fb0), 0x30 (@ 004c6bad).
- Mip-palette flags: 0x60 (@ 004c6e1e), 0x40 (@ 004c6e2b).
- Node field layout: name +4 (0x20), dict +0xc (0x20), type +0x51=0x11, flags +0x14=1.
- No guessing; mechanical description only.
