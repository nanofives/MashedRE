# FUN_004cc4f0 — RW Chunk Type Validator

**RVA:** 0x004cc4f0  
**Session:** batch-render-5-s1  
**Promoted:** C1 → C2  

## Signature

```c
undefined4 FUN_004cc4f0(undefined4 *param_1);
```

`param_1`: pointer to a 32-bit chunk-type field (read as `*param_1`).  
Returns 1 if the type is a known/valid RW chunk type, 0 otherwise.

## Callers

| RVA | Name |
|-----|------|
| 0x004cc400 | FUN_004cc400 (RwStreamReadChunkHeader) |

## Callees

None — leaf function.

## Body Summary

Switch on `*param_1`. Returns 1 for the following RW chunk type IDs (hex): 5, 6, 7, 8, 9, 0xa, 0xb, 0xe, 0xf, 0x10, 0x12, 0x14, 0x1a. Returns 0 for all other values (default case). Pure predicate, no side effects.

## Cited Constants

| Address | Value | Note |
|---------|-------|------|
| 0x004cc4f0 (switch table) | 5..0x1a | Set of valid RW chunk type identifiers |

## Uncertainties

None. Mechanically complete — all branches literal.
