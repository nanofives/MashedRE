# FUN_004c5dd0 — C1→C2 plate (batch-render-4-s3)

**RVA:** 0x004c5dd0  
**Body:** 004c5dd0..004c5df2  
**U-id:** U-4958

## Signature (Ghidra)
```c
bool FUN_004c5dd0(undefined4 param_1, undefined4 param_2);
```

## Decompilation (verbatim)
```c
/* [C1 2026-05-19] Signature `bool f(arg1, arg2)`. */

bool FUN_004c5dd0(undefined4 param_1, undefined4 param_2)
{
  int iVar1;

  iVar1 = (**(code **)(DAT_007d4054 + 0x2c + DAT_007d3ff8))(param_1, param_2);
  return iVar1 != 0;
}
```

## Mechanical analysis
- Calls vtable slot 0x2c at `(DAT_007d4054 + 0x2c + DAT_007d3ff8)` with `(param_1, param_2)`.
- Returns `bool` (non-zero test of return value).
- No callees; 1 caller: FUN_004d0290.
- Pattern: thin bool-wrapper around a RW plugin vtable call.

## Evidence for C2
- Body fully visible; trivial vtable dispatch + bool conversion.
- Constants: vtable offset 0x2c (@ 004c5dd5), DAT_007d4054 (@ 004c5dd3), DAT_007d3ff8 (@ 004c5dda).
- No guessing; mechanical description only.
