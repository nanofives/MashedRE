# FUN_0047aad0 — Script handler: set Vec3+alpha at sky context +0x1d8

**RVA:** 0x0047aad0
**Size:** 0x5d bytes (0x0047aad0..0x0047ab2c)
**Confidence:** C2 (batch-render-2-s2)
**U-id:** U-4535
**Subsystem:** render (script-VM / sky system)

## Signature
```c
undefined4 FUN_0047aad0(undefined4 param_1);
```

## Summary
Script-VM dispatch handler (no direct callers). Reads 3 float args from the script
stack (at indices iVar1-2, iVar1-1, iVar1) and writes them to:
- DAT_006bf1cc+0x1d8 (x)
- DAT_006bf1cc+0x1dc (y)
- DAT_006bf1cc+0x1e0 (z)
Then writes 0x3f800000 (1.0f) to +0x1e4 (alpha/w). Returns 0.

## Decompile (Ghidra)
```c
undefined4 FUN_0047aad0(undefined4 param_1)
{
  int iVar1;
  float10 fVar2;

  iVar1 = FUN_004b6fc0(param_1);                         // stack top
  fVar2 = (float10)FUN_004b7090(param_1, iVar1 + -2);   // x
  *(float *)(DAT_006bf1cc + 0x1d8) = (float)fVar2;
  fVar2 = (float10)FUN_004b7090(param_1, iVar1 + -1);   // y
  *(float *)(DAT_006bf1cc + 0x1dc) = (float)fVar2;
  fVar2 = (float10)FUN_004b7090(param_1, iVar1);        // z
  iVar1 = DAT_006bf1cc;
  *(float *)(DAT_006bf1cc + 0x1e0) = (float)fVar2;
  *(undefined4 *)(iVar1 + 0x1e4) = 0x3f800000;          // alpha = 1.0f
  return 0;
}
```

## Key globals / offsets
- DAT_006bf1cc: sky context object pointer
- +0x1d8: Vec3.x (float)
- +0x1dc: Vec3.y (float)
- +0x1e0: Vec3.z (float)
- +0x1e4: alpha / w = 1.0f (0x3f800000)

## Callers
None found (registered via script-VM dispatch table).

## Callees
- FUN_004b6fc0 (0x004b6fc0) — script-VM stack top index getter
- FUN_004b7090 (0x004b7090) — script-VM pop float at index

## C2 evidence
Decompile is mechanically complete. Vec3 + alpha write into sky context at known
offsets. 0x3f800000 = IEEE 754 1.0f confirmed. No UNCERTAIN markers.
