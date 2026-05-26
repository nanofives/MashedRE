# FUN_004ccce0 — Memory Pool Iterator with Callback

**RVA:** 0x004ccce0  
**Session:** batch-render-5-s1  
**Promoted:** C1 → C2  

## Signature

```c
int * FUN_004ccce0(int *param_1, code *param_2, undefined4 param_3);
```

`param_1`: pool/container header pointer.  
`param_2`: callback invoked per active element.  
`param_3`: user data passed to callback.  
Returns: `param_1` on success, NULL on alloc failure.

## Callers

| RVA | Name |
|-----|------|
| 0x004d7ca0 | FUN_004d7ca0 (RW engine teardown) |

## Callees

Calls: vtable alloc at `DAT_007d3ff8 + 0x108`; vtable free at `DAT_007d3ff8 + 0x10c`; `param_2` callback.

## Body Summary

1. Reads bitmap byte-count from `param_1[2]`; gets list head from `param_1[4]`.
2. If list is empty (`head == param_1 + 4`): return `param_1` immediately.
3. For each node in doubly-linked list:
   a. Alloc a temporary copy-buffer of `param_1[2]` bytes via vtable+0x108 (flag=0x10000); on failure return NULL.
   b. Memcopy node+8 (the bitmap) into the temp buffer.
   c. Scan bits MSB-first: for each set bit at position `(byte_idx * 8 + bit_idx)`, call `param_2(element_addr, param_3)` where `element_addr = (node_addr + bitmap_bytes + align_padding) + bit_position * stride`.
   d. Free the temp buffer via vtable+0x10c.
4. Return `param_1`.

Element address formula (cited at 0x004ccd79):
`((int)piVar5 + uVar2 + param_1[3] + 7) & ~(param_1[3] - 1) + (bit_pos * param_1[0])`

## Cited Constants

| Address | Value | Note |
|---------|-------|------|
| 0x004ccd2d | 0x10000 | Heap alloc flags for temp copy |
| 0x004ccd79 | formula | Element address from node+bitmap+align+stride |
| 0x004ccd8f | DAT_007d3ff8+0x10c | vtable free |

## Uncertainties

[UNCERTAIN] U-5104: Purpose/name of this iterator — observationally it visits all allocated elements of a pool and fires a callback; likely `RwPluginRegistryForAllObjectCBs` or similar RW3 pool iteration pattern; name not confirmed. Evidence needed: cross-ref RW3 plugin registry API.
