# FUN_0047aaa0 — Script handler: set string at sky context +0x2640

**RVA:** 0x0047aaa0
**Size:** 0x2f bytes (0x0047aaa0..0x0047aace)
**Confidence:** C2 (batch-render-2-s2)
**U-id:** U-4534
**Subsystem:** render (script-VM / sky system)

## Signature
```c
undefined4 FUN_0047aaa0(undefined4 param_1);
```

## Summary
Script-VM dispatch handler (no direct callers). Pops an integer arg via
FUN_004b6fc0, retrieves a string pointer at that stack slot via FUN_004b70d0,
then copies the null-terminated string byte-by-byte into `DAT_006bf1c8 + 0x2640`.
Returns 0.

## Decompile (Ghidra)
```c
undefined4 FUN_0047aaa0(undefined4 param_1)
{
  char cVar1;
  undefined4 uVar2;
  char *pcVar3;
  char *pcVar4;

  uVar2 = FUN_004b6fc0(param_1);            // 0x0047aaa6 — stack top index
  pcVar3 = (char *)FUN_004b70d0(param_1, uVar2);  // 0x0047aaaf — get string ptr
  pcVar4 = (char *)(DAT_006bf1c8 + 0x2640); // destination
  do {
    cVar1 = *pcVar3;
    pcVar3 = pcVar3 + 1;
    *pcVar4 = cVar1;
    pcVar4 = pcVar4 + 1;
  } while (cVar1 != '\0');
  return 0;
}
```

## Key globals / offsets
- DAT_006bf1c8: sky context base pointer
- +0x2640: string field within sky context

## Callers
None found (registered via script-VM dispatch table).

## Callees
- FUN_004b6fc0 (0x004b6fc0) — script-VM stack top index getter
- FUN_004b70d0 (0x004b70d0) — script-VM get string at stack slot

## C2 evidence
Decompile is mechanically complete. Inline strcpy into a fixed-offset field of the
sky context is clear. No UNCERTAIN markers.
