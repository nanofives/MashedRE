---
address: 0x004aedcf
name: FUN_004aedcf
confidence: C1
subsystem: boot
date: 2026-05-12
session: boot_crt_env_cont1
---

## Mechanical description

- Locale case-map and ctype-flag init; body 004aedcf–004aef60; SEH frame
- Signature: `void FUN_004aedcf(void)` — uses current DAT_008aa444 as code-page
- Builds 256-byte identity map with lead-byte ranges set to 0x20 (@004aede1–004aee16)
- Calls `___crtGetStringTypeA(1, map, 0x100, local_51c, codepage, locale, 0)` (@004aee5c) → CT_CTYPE1 bitmask array in local_51c
- Calls `___crtLCMapStringA(locale, 0x100, map, 0x100, local_21c, ...)` → uppercase table
- Calls `___crtLCMapStringA(locale, 0x200, map, 0x100, local_31c, ...)` → lowercase table
- Per-byte loop (@004aeeae–004aef20): if CT_CTYPE1&1 (upper): sets flag 0x10 in DAT_008aa340, writes upper char to DAT_008aa460; if CT_CTYPE1&2 (lower): sets flag 0x20 in DAT_008aa340, writes lower char; else: writes 0
- Fallback if GetCPInfo fails: ASCII A–Z gets flag 0x10 and lower, a–z gets flag 0x20 and upper
- Writes case-conversion table to DAT_008aa460 (256 × 1 byte)

## Why C1

Body decompiled; all addresses confirmed. Consistent with VS2003 `__setmbcp` case-table builder (`___crtGetStringTypeA`, `___crtLCMapStringA`).
