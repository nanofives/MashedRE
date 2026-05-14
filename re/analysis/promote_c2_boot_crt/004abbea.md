# FUN_004abbea — command-line argument start pointer finder

RVA: `0x004abbea`  
Session: promote_c2_boot_crt  
Promoted from: entry_callees (C1)

## Signature

```c
byte * FUN_004abbea(void)
```

No parameters. Returns `byte *` — pointer into the command-line string (`DAT_008ab6c4`) at the start of the first argument (past executable name and any leading whitespace).

## Decompilation

```c
byte * FUN_004abbea(void)
{
  byte bVar1;
  int iVar2;
  byte *pbVar3;

  if (DAT_008ab6d4 == 0) {
    ___initmbctable();
  }
  if (DAT_008ab6c4 == (byte *)0x0) {
    pbVar3 = (byte *)((int)&DAT_005cc4dc + 2);
  }
  else {
    bVar1 = *DAT_008ab6c4;
    pbVar3 = DAT_008ab6c4;
    if (bVar1 != 0x22) {
      // unquoted: advance while byte >= 0x21
      do {
        if (bVar1 < 0x21) goto LAB_004abc49;
        bVar1 = pbVar3[1];
        pbVar3 = pbVar3 + 1;
      } while( true );
    }
    // quoted: skip opening quote, scan for closing quote
    pbVar3 = DAT_008ab6c4 + 1;
    bVar1 = *pbVar3;
    if (bVar1 != 0x22) {
      do {
        if (bVar1 == 0) break;
        iVar2 = FUN_004affe0(bVar1);
        if (iVar2 != 0) {
          pbVar3 = pbVar3 + 1;  // double-byte char: extra advance
        }
        pbVar3 = pbVar3 + 1;
        bVar1 = *pbVar3;
      } while (bVar1 != 0x22);
      if (*pbVar3 != 0x22) goto LAB_004abc49;
    }
    do {
      pbVar3 = pbVar3 + 1;
LAB_004abc49:
    } while ((*pbVar3 != 0) && (*pbVar3 < 0x21));  // skip whitespace
  }
  return pbVar3;
}
```

## Memory accesses

| Address | Width | Access | Notes |
|---|---|---|---|
| `0x008ab6d4` | u32 | read | MBCS init flag; calls `___initmbctable` if 0 |
| `0x008ab6c4` | ptr | read | cmdline pointer (set by `GetCommandLineA` in entry) |
| `0x005cc4dc` | — | addr | fallback: returns `&DAT_005cc4dc + 2` if cmdline is NULL |

## Constants

| Value (hex) | Value (dec) | Usage |
|---|---|---|
| `0x008ab6d4` | 9091796 | MBCS init flag |
| `0x008ab6c4` | 9091780 | global cmdline pointer |
| `0x005cc4dc` | 6079708 | fallback base address |
| `0x22` | 34 | `"` double-quote character |
| `0x21` | 33 | whitespace/control threshold (bytes < 0x21 are stop chars) |
| `0x0` | 0 | null-terminator break condition |

## Callees

- `___initmbctable` (0x004af2b6) — conditional MBCS init
- `FUN_004affe0` (0x004affe0) — tests if byte is lead byte of double-byte char (MBCS); returns non-zero if so

## Branches

1. `DAT_008ab6d4 == 0` → call `___initmbctable`.
2. `DAT_008ab6c4 == NULL` → return `&DAT_005cc4dc + 2` (fallback).
3. First byte `== 0x22` → quoted-exe-name path (scan for closing quote, with MBCS lead-byte handling).
4. First byte `!= 0x22` → unquoted path (advance while byte `>= 0x21`).
5. After exe name: `LAB_004abc49` — skip bytes `< 0x21` (whitespace) to land on first argument.

## Why C2

No FidDB library match — CRT command-line parsing helper.  
Evidence: decompilation verified end-to-end this session (pool4, 2026-05-13). Both the quoted and unquoted paths fully transcribed. Constants `0x22` (double-quote) and `0x21` (printable threshold) confirmed at raw hex.
