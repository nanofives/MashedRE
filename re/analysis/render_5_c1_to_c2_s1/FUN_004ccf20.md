# FUN_004ccf20 — RW Allocator Subsystem Teardown

**RVA:** 0x004ccf20  
**Session:** batch-render-5-s1  
**Promoted:** C1 → C2  

## Signature

```c
void FUN_004ccf20(void);
```

No parameters. Returns void.

## Callers

| RVA | Name |
|-----|------|
| 0x004c3270 | FUN_004c3270 |
| 0x004c32b0 | FUN_004c32b0 (RW engine init orchestrator) |

## Callees

Calls vtable slots at `DAT_007d3ff8 + 0x10c` (free) and `DAT_007d3ff8 + 0x11c` (pool-aware free). No named function callees.

## Body Summary

1. Walks outer doubly-linked list at `DAT_007d45cc` until back to sentinel (`&DAT_007d45cc`).
2. For each outer node (pool header, base = node - 7):
   - Unlinks node from the outer list.
   - Walks inner doubly-linked list at pool_base + 1 (freelist), freeing each inner node via vtable+0x10c.
   - Checks ownership flag byte at pool_base - 1 (`& 1`):
     - If 0 (pool owns): if `DAT_007d45fc != pool_base` free via vtable+0x10c; else via vtable+0x11c.
     - If 1 (caller owns): skip free.
3. After outer loop: unlinks and frees the default pool at `DAT_007d45fc` itself.
4. Clears `DAT_007d45fc = NULL`, `DAT_007d45f8 = 0` — cited at 0x004cd050–0x004cd053.

## Cited Constants

| Address | Value | Note |
|---------|-------|------|
| 0x004ccf20 | DAT_007d45cc | Global pool list sentinel |
| 0x004cd050 | DAT_007d45fc | Default pool cleared |
| 0x004cd053 | DAT_007d45f8 | Init flag cleared to 0 |
| 0x004ccf8c | DAT_007d3ff8+0x10c | vtable free |
| 0x004ccfa2 | DAT_007d3ff8+0x11c | vtable pool-aware free |

## Uncertainties

[UNCERTAIN] U-5106: Exact meaning of the ownership-flag byte at pool_base-1 (bit 0 test) — observed to control which free path is taken; semantic name not confirmed. Evidence needed: cross-ref FUN_004cc820 param_5[6] field which writes 2 or 3 (not 0/1 directly); may be a downcast or different field.
