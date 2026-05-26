# FUN_004c3dc0 — C1→C2 plate (batch-render-4-s1)

## Signature
```c
undefined4 FUN_004c3dc0(undefined4 param_1, undefined4 param_2, undefined4 param_3);
// 0x004c3dc0 .. 0x004c3de5
```

## Callers (3)
- `FUN_00426810` (0x00426810)
- `FUN_005422c0` (0x005422c0)
- `FUN_00552890` (0x00552890)

## Callees
None (indirect call only).

## Body summary
3-argument dispatch shim through the third function pointer in the RW engine render-path vtable:

1. Loads function pointer from `*(DAT_007d3ffc + 0x10 + DAT_007d3ff8)`.
2. Calls it with `(param_1, param_2, param_3)`.
3. Returns `param_1`.

This is the fnC dispatch slot (offset +0x10). The default target is `FUN_004c3880` (Vec3TransformVector, per C1 comment in FUN_004c3dc0's existing plate). Third sibling of the FUN_004c3d60 / FUN_004c3d90 / FUN_004c3dc0 dispatch trio.

## Cited constants
- `DAT_007d3ffc + 0x10 + DAT_007d3ff8` — render-path vtable slot for fnC

## Uncertainties
None blocking C2.
