---
address: 0x004affaf
name: FUN_004affaf
confidence: C1
subsystem: boot
date: 2026-05-12
session: boot_crt_env_cont1
---

## Mechanical description

- Char-class predicate; body 004affaf–004affdf
- Signature: `undefined4 FUN_004affaf(byte param_1, uint param_2, byte param_3)` — returns 0 or 1
- Guard: if `DAT_008aa340[param_1+1] & param_3 != 0` (@004affb8): returns 1 immediately
- Mask path: if `param_2 == 0` (@004affc0): result = 0; else result = `*(ushort *)(DAT_005d566a + param_1*2) & param_2` (@004affd2); if result == 0: return 0; else return 1
- `DAT_008aa340` is the CRT multi-byte char-class table (256 × 1 byte, indexed by char value +1); bit 3 = param_3 mask
- `DAT_005d566a` is a 256 × 2-byte table (read-only data segment)
- No callees; pure memory reads and arithmetic
- Called from FUN_004affe0 wrapper (0x004affe0) which passes args `(param_1, 0, 4)` — the pattern for `_ismbblead`-family checks
- Resolves D-0168, clears S-0047

## Why C1

Body decompiled; all addresses and tables confirmed by Ghidra in current session. Pattern consistent with VS2003 `_ismbblead`/`_ismbbtype` predicate.
