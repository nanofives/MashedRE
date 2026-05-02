---
rva: 0x004abbea
name_in_ghidra: FUN_004abbea
size_bytes: 104
confidence: C1
callees_depth1:
  - 0x004af2b6: ___initmbctable
  - 0x004affe0: FUN_004affe0
opened_in_slot: Mashed_pool0
session_date: 2026-05-02
---

## Mechanical description

- Takes no parameters; returns `byte *`.
- Reads global `DAT_008ab6d4`; if `== 0`, calls `___initmbctable()`.
  (`DAT_008ab6c4` was written to by `GetCommandLineA()` in entry at 0x004a4bb7.)
- If `DAT_008ab6c4 == NULL`:
  - Returns `(byte *)(&DAT_005cc4dc + 2)` — pointer to two bytes past `DAT_005cc4dc`.
- Otherwise:
  - Reads first byte: `bVar1 = *DAT_008ab6c4`; sets `pbVar3 = DAT_008ab6c4`.
  - If `bVar1 != 0x22`:
    - Loop: if `bVar1 < 0x21`, jumps to `LAB_004abc49`; else reads next byte and advances `pbVar3`.
  - If `bVar1 == 0x22`:
    - Advances `pbVar3 = DAT_008ab6c4 + 1`; reads `bVar1 = *pbVar3`.
    - If `bVar1 != 0x22`:
      - Inner loop while `bVar1 != 0x22`:
        - If `bVar1 == 0`: breaks.
        - Calls `FUN_004affe0(bVar1)` → `iVar2`. If `iVar2 != 0`: advances `pbVar3 += 1`.
        - Always advances `pbVar3 += 1`; reads next byte.
      - After loop: if `*pbVar3 != 0x22`, jumps to `LAB_004abc49`.
    - Post-quote advance loop: `pbVar3 += 1`.
  - `LAB_004abc49`: loop while `*pbVar3 != 0 && *pbVar3 < 0x21` — advances `pbVar3`.
- Returns `pbVar3`.

## Constants

| RVA cited at     | Raw hex    | Signed dec | Notes                                              |
|------------------|------------|------------|----------------------------------------------------|
| 0x004abbea body  | 0x008ab6d4 | 9091796    | Global checked for zero before calling ___initmbctable |
| 0x004abbea body  | 0x008ab6c4 | 9091780    | Global holding byte pointer (set by GetCommandLineA in entry) |
| 0x004abbea body  | 0x005cc4dc | 6079708    | Global; fallback return is &DAT_005cc4dc + 2       |
| 0x004abbea body  | 0x22       | 34         | Byte value 0x22 (double-quote character) used in branches |
| 0x004abbea body  | 0x21       | 33         | Byte threshold 0x21 used in loop exit conditions   |
| 0x004abbea body  | 0x0        | 0          | Byte value 0 (null terminator) in inner loop break |

## Uncertainties

None for this function at C1.

## Stubs encountered (callees not yet reversed)

Depth-2 relative to session entry point — see DEFERRED.md.
