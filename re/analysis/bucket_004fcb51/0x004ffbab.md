---
rva: 0x004ffbab
name_in_ghidra: FUN_004ffbab
size_bytes: 26
confidence: C1
subsystem: shader-compiler
callees_depth1: []
callers_noted: [FUN_004fcc34, FUN_004fe2fe, FUN_004fe9cf]
opened_in_slot: Mashed_pool8
session_date: 2026-05-18
---

## Mechanical description

Initializer for a 13-dword lexer-input-state struct (`__fastcall(this=param_1)`):
- `param_1[0] = 0` — cursor pointer
- `param_1[1] = 0` — end pointer
- `param_1[6] = 0` — file id
- `param_1[7] = 1` — line number (default 1, matches "lines are 1-indexed")
- `param_1[0xb] = 0` — extra pointer 1
- `param_1[0xc] = 0` — extra pointer 2

Other slots (2..5, 8..0xa) are zero-implicit (struct already zeroed by caller).

This is the lexer-input state that FUN_004ffbc5 fills with actual values.

## Constants

| Constant | Site | Note |
|---|---|---|
| 1 | param_1[7] init | line number base |

## Uncertainties

(none — pairs cleanly with FUN_004ffbc5 below)

## Stubs encountered

(none)
