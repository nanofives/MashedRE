# FUN_004cff20 — RW raster lock / D3D surface acquire

**RVA:** 0x004cff20  
**Session:** batch-render-5-s4  
**U-id:** U-5180  
**Confidence promoted:** C1 → C2

## Signature
```c
undefined4 FUN_004cff20(undefined4 param_1, int *param_2, uint param_3);
// bool f(u32 flags_a, RwRaster* raster, u32 lock_flags)
```
Body: 0x004cff20 .. 0x004d0269 (0x349 bytes, ~100 decompiled lines)

## Decomp summary

Early-out: if `param_2[1] != 0` → push error (FUN_004d7ff0(0x8000000e)) and return 0.
Walk raster linked list via `piVar9 = *piVar9` until self-referential (leaf node found in `piVar5`).

Switch on `*(byte*)(param_2+8) & 7` (raster type field):
- **Cases 0, 4** (camera/normal raster):
  - Read reflectivity byte at `DAT_00911ae4 + 9 + piVar5`.
  - If low nibble == 0: call vtable+0x48 (CreateTexture path, no mipmaps).
  - Else: call vtable+0x48 (CreateTexture path, with mip param).
  - On success: call vtable+0x34 (LockRect-equivalent). Set flags at param_2+0x22.
- **Cases 2, 5** (render-target / cube):
  - Case 5 extra: call vtable+0x48 with &param_2 out-param.
  - Case 2: use DAT_007d4118 or plugin ptr.
  - Call vtable+0x30 (GetSurfaceLevel).
  - Call vtable+0x90 (CreateRenderTarget-like with D3DUSAGE).
  - If success and bit 1 of uStack_14: call vtable+0x80.
  - Call vtable+0x34 (LockRect).
- On success: update raster dimensions + pitch (param_2[1], [3], [4], [6], [10], [11]).
- **Default**: push error 0x8000000e, return 0.

## Key globals
| Address | Role |
|---------|------|
| 0x00911ae4 | RW plugin slot offset |
| 0x007d4110 | D3D device vtable ptr |
| 0x007d4118 | Default D3D surface ptr |

## Callers/Callees
- Callee: FUN_004d7ff0 (error format), FUN_004d8480 (error push).
- Caller: FUN_004d1020 (raster pixel-write dispatcher).

## Evidence for C2
- Existing C1 note: "Signature `bool f(u32 a, RwRaster* raster, u32 flags)`." Confirmed.
- Decompiler output complete, switch structure fully resolved.
- Called by FUN_004d1020 (also being promoted) confirming role as raster-lock entry.
