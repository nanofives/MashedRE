---
struct: render_dispatch_entry
base: DAT_00899264
stride: 0x4c (76 bytes)
count: 8 entries
addr_range: 0x00899264..0x008994c4
session_date: 2026-05-17
session: promote_c2_render_lowrva
referenced_by:
  - 0x00425ab0 (clears entry+0x3C and entry+0x40 to 0 — the "enabled" guards)
  - 0x00425e40 (per-entry render-dispatch loop)
---

## Struct shape

| Offset | Size | Field | Source |
|---|---|---|---|
| 0x00 | 4 | object[0] handle (vtable[0x48] called on it) | FUN_00425e40 piVar3[0] |
| 0x04 | 4 | object[1] handle (vtable[0x48] called on it, third in dispatch order) | FUN_00425e40 piVar3[1] |
| 0x08 | 4 | object[2] handle (vtable[0x48] called on it, second in dispatch order) | FUN_00425e40 piVar3[2] |
| 0x0c..0x38 | ? | (uninspected — middle of entry) | — |
| 0x3C | 4 | guard slot A (cleared by FUN_00425ab0; read as piVar3[-1] on iter k+1 from prior iter) | FUN_00425ab0 first per-entry dword |
| 0x40 | 4 | guard slot B / "enabled" flag (cleared by FUN_00425ab0 second dword; gates dispatch in FUN_00425e40) | FUN_00425e40 piVar3[0x10] |
| 0x44..0x4B | ? | (uninspected — tail of entry) | — |

Total entry size: 0x4c bytes (76 bytes / 0x13 dwords).

## Notes

- FUN_00425e40 reads `piVar3[-1]` (entry-0x04) as a guard. For the first iteration (entry 0), this reads `DAT_00899260` (4 bytes BEFORE the array). For subsequent iterations, it reads the prior entry's offset `+0x48` slot. So either:
  - (a) The array is preceded by a 4-byte sentinel at DAT_00899260 that's part of a wider parent struct.
  - (b) The decompiler off-by-one'd; the intended read is `piVar3[+0x12]` (entry+0x48). The latter is consistent with cleaning what FUN_00425ab0 clears at +0x3C: a "previous-iter-was-active" carry-flag pattern.

- FUN_00425ab0 clears `entry+0x3C` and `entry+0x40`; the latter is FUN_00425e40's primary guard. So `FUN_00425ab0` is a "disable all 8 entries" mass-clear.

- 3-object dispatch order [0, 2, 1] in FUN_00425e40 suggests these are not interchangeable; likely (a) shadow/base/highlight render passes, or (b) mesh/texture/material 3-tuple with a specific binding order.
