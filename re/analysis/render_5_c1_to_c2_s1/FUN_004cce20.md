# FUN_004cce20 — RW Allocator Subsystem Init

**RVA:** 0x004cce20  
**Session:** batch-render-5-s1  
**Promoted:** C1 → C2  

## Signature

```c
undefined4 FUN_004cce20(undefined4 *param_1);
```

`param_1`: optional allocator function table pointer (4-slot: alloc, free, calloc, realloc); if NULL, default malloc/free/etc. are used.  
Returns 1 on success, 0 on alloc failure.

## Callers

| RVA | Name |
|-----|------|
| 0x004c32b0 | FUN_004c32b0 (RW engine init orchestrator) |

## Callees

| RVA | Name |
|-----|------|
| 0x004cc820 | FUN_004cc820 (pool allocator) |

## Body Summary

1. Initialises the global doubly-linked pool list: `DAT_007d45cc = &DAT_007d45cc`; `DAT_007d45d0 = &DAT_007d45cc`; `DAT_007d45f8 = 1` — cited at 0x004cce20–0x004cce30.
2. Calls `FUN_004cc820(0x24, 0x10, 4, 0, &DAT_007d45d4, 0x40000)` to create the default pool at `DAT_007d45fc` — cited at 0x004cce35.
3. If alloc fails: clears `DAT_007d45f8 = 0`; returns 0.
4. Unlinks the freshly-allocated pool from the list sentinel (avoids double-accounting).
5. If `param_1 != NULL`: copies 4 function pointers from `param_1` into vtable slots `DAT_007d3ff8 + 0x108..+0x114`.
6. If `param_1 == NULL`: installs defaults: alloc=`LAB_004ccef0`, free=`_free`, calloc=`LAB_005aead0`, realloc=`LAB_004ccf00` — cited at 0x004cceda.
7. Returns 1.

## Cited Constants

| Address | Value | Note |
|---------|-------|------|
| 0x004cce20 | 0x007d45cc | Global pool list head |
| 0x004cce2e | 0x007d45f8 | Pool subsystem init flag |
| 0x004cce35 | 0x24, 0x10, 4, 0x40000 | Default pool params: 0x24b elem, 0x10 count, 4 align, 0x40000 flags |
| 0x004cceda | LAB_004ccef0 | Default alloc stub |
| 0x004ccee0 | LAB_005aead0 | Default calloc stub |
| 0x004ccee6 | LAB_004ccf00 | Default realloc stub |

## Uncertainties

[UNCERTAIN] U-5105: Names of LAB_004ccef0 / LAB_005aead0 / LAB_004ccf00 — Ghidra shows LAB_ labels, not FUN_ symbols; their actual implementations and SDK names are not confirmed. Evidence needed: disassemble those addresses to verify.
