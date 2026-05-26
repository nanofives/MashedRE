# FUN_004b7fd0 — C1→C2 plate (batch-render-3-s6)

**RVA:** 0x004b7fd0  
**Name:** FUN_004b7fd0  
**Subsystem:** render  
**Promoted:** 2026-05-26 by batch-render-3-s6  

## Decompilation (0x004b7fd0)

```c
void FUN_004b7fd0(undefined4 param_1, undefined4 param_2, undefined4 param_3)
{
  undefined4 *puVar1;
  puVar1 = (undefined4 *)FUN_004b7ff0(param_1, param_3);
  *puVar1 = param_2;
  *(undefined2 *)(puVar1 + 3) = 1;
  return;
}
```

Body: 0x004b7fd0 – 0x004b7fee. Small non-leaf.

## Mechanical description

- Three parameters: `param_1` (Lua state `L`), `param_2` (value), `param_3` (stack index / key).
- Calls `FUN_004b7ff0` (0x004b7ff0) with `(param_1, param_3)` to obtain a slot pointer.
- Writes `param_2` into the slot value field (`*puVar1 = param_2`).
- Writes `1` as a `uint16` at slot+12 bytes (`*(uint16*)(puVar1+3) = 1`) — the Lua type tag field for "true" (boolean true in Lua 5.0 TValue).

## Callers / callees

- Callers: 0 in Ghidra DB (reached indirectly or via thunk at 0x004b7200)
- Callees: `FUN_004b7ff0` (0x004b7ff0, slot-lookup helper)

## Evidence for C2

- Full body decompiles cleanly.
- Corroborates C1 note: "tag/value setter; FUN_004b7ff0(L,p3)→slot; *slot=p2; *(slot+3:uint16)=1."
- Contrast with `FUN_004b8080` which sets tag=0 (boolean false). Tag=1 → true.
- Key address: slot-lookup helper at 0x004b7ff0.

## U-ids

None required.
