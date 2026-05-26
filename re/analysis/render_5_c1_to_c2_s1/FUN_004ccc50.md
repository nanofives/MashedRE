# FUN_004ccc50 — Memory Pool GC: Free Zero-Checksum Nodes

**RVA:** 0x004ccc50  
**Session:** batch-render-5-s1  
**Promoted:** C1 → C2  

## Signature

```c
int FUN_004ccc50(int *param_1);
```

`param_1`: pool/container header pointer.  
Returns: `*param_1 * freed_count` (stride × number of freed nodes).

## Callers

| RVA | Name |
|-----|------|
| 0x004c8690 | FUN_004c8690 (D3D VB cache init/reset) |
| 0x004c9ad0 | FUN_004c9ad0 |
| 0x004caea0 | FUN_004caea0 (D3D VB cache init/reset) |
| 0x004ccde0 | FUN_004ccde0 (pool GC orchestrator) |

## Callees

None — all dispatch via `DAT_007d3ff8 + 0x10c` (vtable free slot) only.

## Body Summary

1. Reads element stride from `param_1[0]`, bitmap byte-count from `param_1[2]`, and doubly-linked list head from `param_1[4]`.
2. Iterates the doubly-linked list of pool nodes:
   - Unlinks current node: `*piVar4[1] = *piVar4; *(*piVar4 + 4) = piVar4[1]`.
   - Computes byte checksum of the bitmap (`param_1[2]` bytes at node+8).
   - If checksum == 0: frees node via `(**(code **)(DAT_007d3ff8 + 0x10c))(node)`, increments counter.
   - If checksum != 0: re-inserts node at list head.
3. Returns `param_1[0] * freed_count` — i.e. total bytes freed capacity.

## Cited Constants

| Address | Value | Note |
|---------|-------|------|
| 0x004ccc62 | +0 | param_1[0] = element stride |
| 0x004ccc66 | +8 | param_1[2] = bitmap byte count |
| 0x004ccc6e | +16 | param_1[4] = list sentinel |
| 0x004ccc9a | DAT_007d3ff8+0x10c | vtable free callback |

## Uncertainties

None. Mechanically complete.
