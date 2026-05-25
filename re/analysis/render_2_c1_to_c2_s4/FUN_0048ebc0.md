---
rva: 0x0048ebc0
name_in_ghidra: FUN_0048ebc0
size_bytes: 0x20
confidence: C2
callees_depth1: []
opened_in_slot: Mashed_pool12
session_date: 2026-05-25
---

## Mechanical description
- Reads `(&DAT_0076d994)[param_1 * 0x122]` — a dword field at stride 0x122 into the explosion ring buffer, offset -2 dwords from the ring base DAT_0076d99c (0x0048ebc8).
- Computes `(int)(8 / (longlong)(int)(&DAT_0076d994)[param_1 * 0x122])` via 64-bit integer division at 0x0048ebc8.
- Clamps: if `iVar1 > 7`, returns 8 (0x0048ebd9).
- Returns integer in range [0, 8] representing per-frame particle count.
- Callers: FUN_0048ebf0 (0x0048ebf0), FUN_00490500 (0x00490500).

## Constants
| RVA cited at | Raw hex | Signed dec | Notes |
|---|---|---|---|
| 0x0048ebc8 | 0x0076d994 | — | base of indexed field = ring buffer base - 8 bytes |
| 0x0048ebc8 | 0x122 | 290 | ring buffer entry stride in dwords |
| 0x0048ebc8 | 8 | 8 | dividend and return cap |
| 0x0048ebd9 | 7 | 7 | clamp threshold (> 7 → return 8) |

## Uncertainties
- none

## Stubs encountered
- none
