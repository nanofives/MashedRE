---
rva: 0x004daa00
name_in_ghidra: FUN_004daa00
size_bytes: 53
confidence: C2
subsystem: render
session: batch-render-6-s6
date: 2026-05-26
pool_slot: Mashed_pool14
prior_plate: re/analysis/bucket_004d7ac0/0x004daa00.md
---

## Shape

- Signature: `uint8_t FUN_004daa00(void* param_1, uint32_t param_2, int param_3)`
- Return: palette-index byte at leaf node
- Subsystem: rw-palette-quantizer

## Mechanical description

Descend the octree to a leaf and return the palette-index byte.

Loop: while `param_3 > 0`, advance `param_1 = *(int*)(param_1 + 0x1c + (param_2 & 0xf)*4)`; shift `param_2 >>= 4`; decrement `param_3`.

Returns `*(undefined1 *)(param_1 + 0x18)` — palette index byte at leaf.

## Constants

| Address | Value | Note |
|---------|-------|------|
| 0x004daa05 | `+0x1c` | child-array offset |
| 0x004daa1a | `+0x18` | leaf palette-index |

## Callees (depth-1)

None (leaf function).

## Callers noted

- `FUN_004da4f0` (0x004da4f0)

## Uncertainties

None.

## Stubs encountered

None.
