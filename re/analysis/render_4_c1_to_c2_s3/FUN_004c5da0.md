# FUN_004c5da0 — C1→C2 plate (batch-render-4-s3)

**RVA:** 0x004c5da0  
**Body:** 004c5da0..004c5dc6  
**U-id:** U-4957

## Signature (Ghidra)
```c
void FUN_004c5da0(undefined4 param_1, undefined4 param_2, undefined4 param_3,
                  undefined4 param_4, undefined4 param_5);
```

## Decompilation (verbatim)
```c
/* [C1 2026-05-19] 5-argument forwarder: `FUN_004d7de0(&DAT_00618138, a, b, c, d, e)`. */

void FUN_004c5da0(undefined4 param_1, undefined4 param_2, undefined4 param_3,
                  undefined4 param_4, undefined4 param_5)
{
  FUN_004d7de0(&DAT_00618138, param_1, param_2, param_3, param_4, param_5);
  return;
}
```

## Mechanical analysis
- Pure forwarder: prepends `&DAT_00618138` as first argument to FUN_004d7de0, passing all 5 params through.
- DAT_00618138 is a global data structure (appears in FUN_004c6ba0 as `&DAT_00618138` passed to FUN_004d8000 — a dictionary/registry).
- 1 callee: FUN_004d7de0; 1 caller: FUN_00543e50.

## Evidence for C2
- Body fully visible; trivial forwarder, no logic.
- Constant: DAT_00618138 (@ 004c5da9) — same global used by FUN_004d8000 in FUN_004c6ba0.
- No guessing; mechanical description only.
