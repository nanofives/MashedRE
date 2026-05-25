# FUN_0047acd0 — Script handler: set float at sky context +0x20c

**RVA:** 0x0047acd0
**Size:** 0x24 bytes (0x0047acd0..0x0047acf3)
**Confidence:** C2 (batch-render-2-s2)
**U-id:** U-4536
**Subsystem:** render (script-VM / sky system)

## Signature
```c
undefined4 FUN_0047acd0(undefined4 param_1);
```

## Summary
Script-VM dispatch handler (no direct callers). Pops a float from the script stack
top and writes it to `DAT_006bf1cc + 0x20c`. Returns 0.

## Decompile (Ghidra)
```c
undefined4 FUN_0047acd0(undefined4 param_1)
{
  undefined4 uVar1;
  float10 fVar2;

  uVar1 = FUN_004b6fc0(param_1);                  // stack top index
  fVar2 = (float10)FUN_004b7090(param_1, uVar1);  // pop float
  *(float *)(DAT_006bf1cc + 0x20c) = (float)fVar2;
  return 0;
}
```

## Key globals / offsets
- DAT_006bf1cc: sky context object pointer
- +0x20c: float field (purpose: [UNCERTAIN] — likely fog density or sun angle)

## Callers
None found (registered via script-VM dispatch table).

## Callees
- FUN_004b6fc0 (0x004b6fc0) — script-VM stack top index getter
- FUN_004b7090 (0x004b7090) — script-VM pop float at index

## C2 evidence
Decompile is mechanically complete. Single-float write to fixed sky context offset.
Identical pattern to sibling handlers at +0x210. Specific semantic of +0x20c
[UNCERTAIN] but mechanics are clear. No further UNCERTAIN markers on the
mechanics.
