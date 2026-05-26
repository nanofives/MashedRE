# FUN_004c5cb0 — C1→C2 plate (batch-render-4-s3)

**RVA:** 0x004c5cb0  
**Body:** 004c5cb0..004c5d96  
**U-id:** U-4956

## Signature (Ghidra)
```c
int FUN_004c5cb0(undefined4 param_1, int param_2);
```

## Decompilation (verbatim)
```c
/* [C1 2026-05-19] Signature `int f(name, dict)`. */

int FUN_004c5cb0(undefined4 param_1, int param_2)
{
  undefined4 *puVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  int *piVar5;
  undefined4 uStack_8;
  undefined4 uStack_4;

  iVar4 = (**(code **)(DAT_007d4054 + 0x18 + DAT_007d3ff8))(param_1);
  if (iVar4 != 0) {
    *(int *)(iVar4 + 0x54) = *(int *)(iVar4 + 0x54) + 1;
    return iVar4;
  }
  iVar4 = (**(code **)(DAT_007d4054 + 0x14 + DAT_007d3ff8))(param_1, param_2);
  if (iVar4 == 0) {
    uStack_8 = 1;
    if (param_2 != 0) {
      uStack_4 = FUN_004d7ff0(0x16, param_1, param_2);
      FUN_004d8480(&uStack_8);
      return 0;
    }
    uStack_4 = FUN_004d7ff0(0x16, param_1, s__null__00618178);
    FUN_004d8480(&uStack_8);
    return 0;
  }
  iVar2 = *(int *)(DAT_007d4054 + 0x10 + DAT_007d3ff8);
  if (iVar2 != 0) {
    if (*(int *)(iVar4 + 4) != 0) {
      **(undefined4 **)(iVar4 + 0xc) = *(undefined4 *)(iVar4 + 8);
      *(undefined4 *)(*(int *)(iVar4 + 8) + 4) = *(undefined4 *)(iVar4 + 0xc);
    }
    *(int *)(iVar4 + 4) = iVar2;
    uVar3 = *(undefined4 *)(iVar2 + 8);
    piVar5 = (int *)(iVar2 + 8);
    puVar1 = (undefined4 *)(iVar4 + 8);
    *(int **)(iVar4 + 0xc) = piVar5;
    *puVar1 = uVar3;
    *(undefined4 **)(*piVar5 + 4) = puVar1;
    *piVar5 = (int)puVar1;
  }
  return iVar4;
}
```

## Mechanical analysis
- Calls vtable slot 0x18 (`param_1`) — lookup-by-name; if hit, increments refcount at offset +0x54 and returns.
- Otherwise calls vtable slot 0x14 (`param_1, param_2`) — load from dict; if fails, posts error 0x16 and returns 0.
- On success: reads plugin slot +0x10 (the global list head from FUN_004c5ca0).
- If list non-empty: detaches node (updates forward/back links at offsets +4,+8,+0xc) then re-inserts at head.
- Returns loaded node pointer.
- 2 callees: FUN_004d7ff0 (error posting), FUN_004d8480 (error flush).
- 8 callers (widespread).
- Consistent with RW texture dictionary load-or-refcount pattern.

## Evidence for C2
- Body fully visible; all branches covered.
- Constants: vtable offsets 0x18 (@ 004c5cba), 0x14 (@ 004c5ccf), list-slot 0x10 (@ 004c5ce9).
- Node link offsets +4, +8, +0xc (@ 004c5cf0..004c5d10), refcount offset +0x54 (@ 004c5cbe).
- Error code 0x16 (@ 004c5cd8), string s__null__ @ 00618178.
- No guessing; mechanical description only.
