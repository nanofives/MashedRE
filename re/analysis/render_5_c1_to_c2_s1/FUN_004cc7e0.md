# FUN_004cc7e0 — RW Global Setter: DAT_006182b0

**RVA:** 0x004cc7e0  
**Session:** batch-render-5-s1  
**Promoted:** C1 → C2  

## Signature

```c
void FUN_004cc7e0(undefined4 param_1);
```

`param_1`: value to store.  
Returns void.

## Callers

| RVA | Name |
|-----|------|
| 0x004c32b0 | FUN_004c32b0 (RW engine init orchestrator) |

## Callees

None — leaf function.

## Body Summary

Single instruction body: stores `param_1` into global `DAT_006182b0`. 9-byte leaf. No branching, no stack frame.

## Cited Constants

| Address | Value | Note |
|---------|-------|------|
| 0x004cc7e2 | 0x006182b0 | Target global written |

## Uncertainties

[UNCERTAIN] U-5102: Semantic role of `DAT_006182b0` — observed written from RW engine init orchestrator FUN_004c32b0; used as a guard in `thunk_FUN_004cc820` (0x004cc9e0) to zero `param_4` when false. Name/type not confirmed. Evidence needed: trace all reads of 0x006182b0.
