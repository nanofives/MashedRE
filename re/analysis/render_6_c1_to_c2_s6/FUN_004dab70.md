---
rva: 0x004dab70
name_in_ghidra: FUN_004dab70
size_bytes: 83
confidence: C2
subsystem: render
session: batch-render-6-s6
date: 2026-05-26
pool_slot: Mashed_pool14
prior_plate: re/analysis/bucket_004d7ac0/0x004dab70.md
---

## Shape

- Signature: `void FUN_004dab70(void* param_1, void* param_2, int param_3)`
- Return: void
- Subsystem: rw-palette-quantizer

## Mechanical description

Recursive tree-free helper.

Bails on null `param_2`.

For interior depth (`param_3 > 0`): iterates 16 children at `param_2 + 0x1c`, recurses on each with `param_3-1`.

Calls engine vtable `+0x11c` (free) using arena `*(int*)(param_1 + 0x4004)` on `param_2`.

## Constants

| Address | Value | Note |
|---------|-------|------|
| 0x004dab80 | `+0x1c` | child array |
| 0x004dab86 | `0x10` | fanout (16 children) |
| 0x004daba0 | `+0x4004` | arena handle |
| 0x004daba6 | `+0x11c` | engine vtable free slot |

## Callees (depth-1)

- `FUN_004dab70` (self — recursive)
- engine vtable `+0x11c` — free

## Callers noted

- `FUN_004daaf0` (0x004daaf0)

## Uncertainties

None.

## Stubs encountered

- [STUB] engine vtable free at `+0x11c`.
