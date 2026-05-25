# FUN_0047afa0 — Script VM handler: set slot type=4 (2 floats at +0x98, +0x9c)

**RVA:** `0x0047afa0`  
**Body:** `0x0047afa0`–`0x0047aff7` (88 bytes)  
**Subsystem:** render / script-VM handler bank  
**Batch:** batch-render-2-s3  
**Promoted:** C1 → C2  
**U-id:** U-4552

## Decompilation (Ghidra, Mashed_pool11)

```c
undefined4 FUN_0047afa0(undefined4 param_1)
{
  int iVar1;
  int iVar2;
  float10 fVar3;

  iVar1 = FUN_004b6fc0(param_1);
  FUN_004b7090(param_1, iVar1 + -2);      // pop 2
  iVar2 = FUN_004a2c48();
  fVar3 = (float10)FUN_004b7090(param_1, iVar1 + -1);
  *(float *)(&DAT_0086cb98 + iVar2 * 0x110) = (float)fVar3;   // slot field_0x98
  fVar3 = (float10)FUN_004b7090(param_1, iVar1);
  *(float *)(&DAT_0086cb9c + iVar2 * 0x110) = (float)fVar3;   // slot field_0x9c
  (&DAT_0086cac0)[iVar2 * 0x44] = 4;                          // type = 4
  return 0;
}
```

## Analysis

Script VM handler: pops 2 floats, writes to `+0x98` and `+0x9c` of the slot (stride `0x110`), sets type = 4. Structurally mirrors 0x47aef0 (type=2) but at different field offsets.

## Call graph

**Callers:** 0  
**Callees:**
- `FUN_004b6fc0` (0x004b6fc0)
- `FUN_004b7090` (0x004b7090)
- `FUN_004a2c48` (0x004a2c48)

## Evidence for C2

- Full decompilation recovered; clear 2-float + type assignment pattern.
- Fields `+0x98`/`+0x9c` cite exact Ghidra addresses `DAT_0086cb98`/`DAT_0086cb9c` at `0x0047afbb`/`0x0047afc9`.
