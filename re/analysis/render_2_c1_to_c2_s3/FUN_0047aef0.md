# FUN_0047aef0 — Script VM handler: set slot type=2 (2 floats)

**RVA:** `0x0047aef0`  
**Body:** `0x0047aef0`–`0x0047af47` (88 bytes)  
**Subsystem:** render / script-VM handler bank  
**Batch:** batch-render-2-s3  
**Promoted:** C1 → C2  
**U-id:** U-4550

## Decompilation (Ghidra, Mashed_pool11)

```c
undefined4 FUN_0047aef0(undefined4 param_1)
{
  int iVar1;
  int iVar2;
  float10 fVar3;

  iVar1 = FUN_004b6fc0(param_1);          // get stack top index
  FUN_004b7090(param_1, iVar1 + -2);      // pop 2 (first float)
  iVar2 = FUN_004a2c48();                 // get slot id
  fVar3 = (float10)FUN_004b7090(param_1, iVar1 + -1);
  (&DAT_0086cb90)[iVar2 * 0x44] = (float)fVar3;   // slot[id].field_0x90 = float1
  fVar3 = (float10)FUN_004b7090(param_1, iVar1);
  (&DAT_0086cb94)[iVar2 * 0x44] = (float)fVar3;   // slot[id].field_0x94 = float2
  (&DAT_0086cac0)[iVar2 * 0x44] = 2;              // slot[id].type = 2
  return 0;
}
```

## Analysis

Script VM handler. Reads 2 floats from the Lua/script stack and writes them into a global slot-array at stride `0x44` (= `0x110` bytes / 4). Sets `DAT_0086cac0[slot*0x44] = 2` (the type discriminant). The two float payload fields sit at `+0x90` and `+0x94` within the slot. Slot id is returned by `FUN_004a2c48` (0 callers outside this cluster = likely script-VM stack accessor).

## Call graph

**Callers:** 0 (registered via script dispatch table; not called directly in reachable code)  
**Callees:**
- `FUN_004b6fc0` (0x004b6fc0) — get stack top
- `FUN_004b7090` (0x004b7090) — pop / read from stack
- `FUN_004a2c48` (0x004a2c48) — get current slot id

## Evidence for C2

- Full decompilation recovered; no ambiguity in control flow.
- All three callees are shared with sibling handlers 0x47af50 / 0x47afa0 / 0x47b0a0 confirming uniform VM calling convention.
- Slot stride `0x44` confirmed consistent across entire handler cluster.
- C1 note in Ghidra comment already described this correctly; decompilation confirms the 2-float write + type=2 set.
