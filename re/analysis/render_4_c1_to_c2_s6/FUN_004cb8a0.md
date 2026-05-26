# FUN_004cb8a0 — D3DCreateVertexDeclarationCached C1→C2

**RVA:** 0x004cb8a0
**Body:** 0x004cb8a0..0x004cba7f
**Session:** batch-render-4-s6
**Pool:** Mashed_pool14
**U-id:** U-5030

## Decompilation summary

Creates or retrieves a cached D3D9 VertexDeclaration from a D3DVERTEXELEMENT9 array.

**Count elements:**
- At 0x004cb8a0: walks `param_1 + 1` (the element array), counting until byte `0x11` (D3DDECL_END marker) is found, incrementing `iVar8`. Stride: +2 ints per element (= 8 bytes = sizeof D3DVERTEXELEMENT9).

**Cache lookup:**
- At 0x004cb8b8: if `DAT_007d4144 != 0` (cache has entries), walks `DAT_007d414c` in 0xc-byte strides.
  - At 0x004cb8c4: `piVar7[-1] == iVar8` — element count matches.
  - At 0x004cb8cc: compares `iVar8 * 8` bytes of the stored key against `param_1`. On full match, jumps to LAB_004cb904.
  - At LAB_004cb904: if cache index `uVar6 < DAT_007d4144`, reads `DAT_007d414c[uVar6 * 0xc]` (first field = IVertexDeclaration9 ptr or 0).
    - If 0: calls `(*DAT_007d4110)[0x158](DAT_007d4110, key_copy_ptr, cache_entry_ptr)` — re-creates.
    - If non-zero: calls `(*piVar7)[4](piVar7)` — AddRef.
    - Writes result to `*param_1`, returns `*param_1 != 0`.

**Cache miss (add new entry):**
- At 0x004cb8f8: if `DAT_007d4148 <= DAT_007d4144` (capacity exhausted), grow: `DAT_007d4148 += 0x10`, then alloc or realloc `DAT_007d414c` to `DAT_007d4148 * 0xc` bytes.
- At 0x004cb930: calls `(*DAT_007d4110)[0x158](DAT_007d4110, param_1, param_2)` — IDirect3DDevice9::CreateVertexDeclaration.
- If succeeds: stores `*param_2` at `DAT_007d414c + count * 0xc + 0`, stores `iVar8` (elem count) at `+ 4`, allocates `iVar8 * 8` bytes at `+ 8`, copies element bytes there, increments `DAT_007d4144`.
- Returns `*param_2 != 0`.

Cache layout at `DAT_007d414c` (stride 0xc = 12 bytes per entry):
- `[0]` = IVertexDeclaration9 handle (4 bytes)
- `[4]` = element count (4 bytes)
- `[8]` = ptr to key copy buffer (4 bytes)

Global reads/writes:
- `DAT_007d4144` at 0x004cb8b8 — cache entry count
- `DAT_007d4148` at 0x004cb8f8 — cache capacity
- `DAT_007d414c` at 0x004cb8c0 — cache array base pointer
- `DAT_007d4110` at 0x004cb930 — D3D device pointer
- `DAT_007d3ff8` at 0x004cb8fc — allocator vtable

## Callers (6)
- `FUN_004db3e0` (0x004db3e0)
- `FUN_004e0920` (0x004e0920)
- `FUN_004ea9a0` (0x004ea9a0)
- `FUN_004f13d0` (0x004f13d0)
- `FUN_004f1b00` (0x004f1b00)
- `FUN_00530c00` (0x00530c00)

## Callees (0)
Leaf (vtable calls only).

## C2 evidence
- Full decompilation read; cache structure and all field offsets cited.
- D3DDECL_END sentinel 0x11 noted at address 0x004cb8a6.
- No UNCERTAIN items.

## Line count
~80 decompiled lines — within 300-line cap.
