# FUN_0047ade0 — Script handler: write 3 strings into slot record at 0x86cac4

**RVA:** 0x0047ade0
**Size:** 0x91 bytes (0x0047ade0..0x0047ae70)
**Confidence:** C2 (batch-render-2-s2)
**U-id:** U-4538
**Subsystem:** render (script-VM / sky/level string table)

## Signature
```c
undefined4 FUN_0047ade0(undefined4 param_1);
```

## Summary
Script-VM dispatch handler (no direct callers). Pops 4 args: index (iVar1-3) used
as slot id, then 3 string pointers. Gets current slot index via FUN_004a2c48
(multiplied by 0x110 = 272-byte stride). Calls a vtable string-copy function
(DAT_007d3ff8+0xcc) three times to write 3 strings into the slot record at:
- 0x86cac4 + iVar2*0x110 (name 0)
- 0x86cb04 + iVar2*0x110 (name 1, +0x40)
- 0x86cb44 + iVar2*0x110 (name 2, +0x80)

Returns 0.

## Decompile (Ghidra)
```c
undefined4 FUN_0047ade0(undefined4 param_1)
{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 *puVar4;

  iVar1 = FUN_004b6fc0(param_1);
  FUN_004b7090(param_1, iVar1 + -3);       // pop slot-index arg
  iVar2 = FUN_004a2c48();                  // get current slot
  iVar2 = iVar2 * 0x110;                   // stride
  puVar4 = (undefined4 *)(DAT_007d3ff8 + 0xcc);  // vtable string-copy fn
  uVar3 = FUN_004b70d0(param_1, iVar1 + -2);     // string 0
  (*(code *)*puVar4)(iVar2 + 0x86cac4, uVar3);
  puVar4 = (undefined4 *)(DAT_007d3ff8 + 0xcc);
  uVar3 = FUN_004b70d0(param_1, iVar1 + -1);     // string 1
  (*(code *)*puVar4)(&DAT_0086cb04 + iVar2, uVar3);
  puVar4 = (undefined4 *)(DAT_007d3ff8 + 0xcc);
  uVar3 = FUN_004b70d0(param_1, iVar1);           // string 2
  (*(code *)*puVar4)(&DAT_0086cb44 + iVar2, uVar3);
  return 0;
}
```

## Key constants / offsets
- Slot stride: 0x110 (272 bytes)
- Base addresses: 0x86cac4, 0x86cb04 (+0x40), 0x86cb44 (+0x80)
- vtable string-copy: DAT_007d3ff8+0xcc
- FUN_004a2c48: current slot getter

## Callers
None found (registered via script-VM dispatch table).

## Callees
- FUN_004a2c48 (0x004a2c48) — get current slot index
- FUN_004b6fc0 (0x004b6fc0) — script-VM stack top index getter
- FUN_004b7090 (0x004b7090) — script-VM pop integer
- FUN_004b70d0 (0x004b70d0) — script-VM get string at stack slot

## C2 evidence
Decompile is mechanically complete. Three-string vtable-copy into a fixed-stride
array at known global addresses. Stride and offsets are all literal in the decompile.
No UNCERTAIN markers.
