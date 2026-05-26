# thunk_FUN_004cc820 (at 0x004cc9e0) — RW Memory Pool Allocator

**RVA:** 0x004cc9e0  
**Session:** batch-render-5-s1  
**Promoted:** C1 → C2  
**Note:** Ghidra marks this as a thunk (5-byte JMP to 0x004cc820). Decompiler resolves through to the full body of FUN_004cc820.

## Signature

```c
uint * thunk_FUN_004cc820(int param_1, uint param_2, uint param_3, int param_4, uint *param_5, uint param_6);
```

`param_1`: element size (in bytes, or stride).  
`param_2`: element count (or bit-width).  
`param_3`: alignment (defaults to 4 if 0).  
`param_4`: pre-allocate count (0 = no pre-alloc); zeroed if `DAT_006182b0 == 0`.  
`param_5`: optional existing pool header (NULL = allocate new).  
`param_6`: heap flags (passed to vtable alloc; upper byte = category).  
Returns: pointer to pool header on success, NULL on alloc failure.

## Callers

7 callers (excerpt): FUN_004d3db0, FUN_004d7c60, FUN_005540d0, FUN_00555360, FUN_00556d20, FUN_005571e0, FUN_00557250.

## Callees

None visible from thunk entry — resolves to FUN_004cc820 body which calls vtable slots at `DAT_007d3ff8 + 0x108` (alloc) and `DAT_007d3ff8 + 0x10c` (free) and `DAT_007d3ff8 + 0x118` (alloc from existing pool).

## Body Summary

Full FUN_004cc820 body accessed through the thunk:

1. If `DAT_006182b0 == 0`: zero `param_4` (disable pre-alloc) — cited at 0x004cc826.
2. If `param_3 == 0`: set `param_3 = 4` — cited at 0x004cc830.
3. If `param_5 == NULL`:
   - If `DAT_007d45fc == NULL`: alloc new pool header via vtable+0x108 (size=0x24, flags=`param_6 & 0xff0000`).
   - Else: alloc via vtable+0x118 from existing pool at DAT_007d45fc.
   - On alloc failure: return NULL.
   - Set `param_5[6] = 2` (ownership flag: pool owns header).
4. Else: set `param_5[6] = 3` (caller owns header).
5. Compute aligned element size: `uVar2 = (param_1 - 1 + param_3) & ~(param_3 - 1)` — cited at 0x004cc877.
6. Fill pool header fields: `[0]=uVar2, [1]=param_2, [2]=param_2+7>>3, [3]=param_3, [4]=[5]=(uint)&header[4]` (sentinel doubly-linked list).
7. Pre-alloc loop (`param_4` iterations): allocate node of size `uVar2*param_2 + bitmap_bytes + align + param_3`; on failure unwind all nodes; link each new node into doubly-linked list at `header[4]`; zero bitmap words.
8. Link pool header into global doubly-linked list at `DAT_007d45cc` — cited at 0x004cc9bd.
9. Return `param_5`.

## Cited Constants

| Address | Value | Note |
|---------|-------|------|
| 0x004cc826 | 0x006182b0 | Global guard flag |
| 0x004cc830 | 4 | Default alignment |
| 0x004cc877 | alignment formula | `(size-1+align) & ~(align-1)` |
| 0x004cc9bd | 0x007d45cc | Global pool list head |
| 0x004cc9bf | 0x007d45fc | Default pool header pointer |

## Uncertainties

[UNCERTAIN] U-5103: Purpose of `DAT_007d45fc` — appears to be a "default pool" or "freelist root" that FUN_004cc820 checks when allocating a pool header; semantic role not confirmed. Evidence needed: trace all writes to 0x007d45fc.
