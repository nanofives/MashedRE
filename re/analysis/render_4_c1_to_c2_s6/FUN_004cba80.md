# FUN_004cba80 — D3DReleaseVertexDeclarationCached C1→C2

**RVA:** 0x004cba80
**Body:** 0x004cba80..0x004cbacb
**Session:** batch-render-4-s6
**Pool:** Mashed_pool14
**U-id:** U-5031

## Decompilation summary

Releases a cached D3D9 VertexDeclaration by handle value. Companion to `FUN_004cb8a0`.

- At 0x004cba80: if `DAT_007d4144 == 0` (cache empty) → return.
- At 0x004cba88: walks `piVar3 = DAT_007d414c` in +3-int strides, comparing `*piVar3 == param_1` (the VD handle).
- At 0x004cba92: if count exhausted without match → return.
- On match: at 0x004cba9c: calls `(*(*(int *)DAT_007d414c[uVar1 * 3] + 8))(DAT_007d414c[uVar1 * 3])` — vtable+8 on the VD object = `Release()`.
- At 0x004cbab2: if `iVar2 == 0` (refcount reached zero), sets `DAT_007d414c[uVar1 * 3] = 0` — nulls the cache slot handle.

Global reads/writes:
- `DAT_007d4144` at 0x004cba80 — VD cache entry count
- `DAT_007d414c` at 0x004cba88 — VD cache array base pointer (stride 3 ints = 0xc bytes)

## Callers (5)
- `FUN_004db3e0` (0x004db3e0)
- `FUN_004db550` (0x004db550)
- `FUN_004e08b0` (0x004e08b0)
- `FUN_004e0920` (0x004e0920)
- `FUN_004ebc30` (0x004ebc30)

## Callees (0)
Leaf (vtable calls only).

## C2 evidence
- Full decompilation read; all indices, stride 3 and vtable offset +8 cited.
- Companion relationship to FUN_004cb8a0 established via shared globals.
- No UNCERTAIN items.

## Line count
~20 decompiled lines — within 300-line cap.
