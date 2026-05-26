---
rva: 0x004daaf0
name_in_ghidra: FUN_004daaf0
size_bytes: 115
confidence: C2
subsystem: render
session: batch-render-6-s6
date: 2026-05-26
pool_slot: Mashed_pool14
prior_plate: re/analysis/bucket_004d7ac0/0x004daaf0.md
---

## Shape

- Signature: `void FUN_004daaf0(void* param_1)`
- Return: void
- Subsystem: rw-palette-quantizer

## Mechanical description

Quantizer teardown: counterpart of FUN_004daa40.

Reads root node from `*(int*)(param_1+0x4000)`. If non-null:
- If `DAT_00618430 > 0`, walks 16 children at `root + 0x1c`, recurses into FUN_004dab70 (per-child tree-free) with depth `DAT_00618430-1`.
- Calls engine vtable `+0x11c` (free) on the root node, using arena handle `*(int*)(param_1+0x4004)`.

Clears the root slot. Calls FUN_004cc9f0 (arena destroy / pool teardown).

## Constants

| Address | Value | Note |
|---------|-------|------|
| 0x004daaf3 | `+0x4000` | root slot |
| 0x004daafa | `DAT_00618430` | depth |
| 0x004daafd | `+0x1c` | child array |
| 0x004dab12 | `0x10` | fanout (16 children) |
| 0x004dab2c | `+0x11c` | engine vtable free slot |
| 0x004dab33 | `+0x4004` | arena handle |

## Callees (depth-1)

- `FUN_004dab70` — per-child recursive tree-free
- engine vtable `+0x11c` — free
- `FUN_004cc9f0` — arena destroy / pool teardown

## Callers noted

None observed.

## Uncertainties

None.

## Stubs encountered

- [STUB] engine vtable free at `+0x11c`.
- [STUB] FUN_004cc9f0 (arena destroy).
