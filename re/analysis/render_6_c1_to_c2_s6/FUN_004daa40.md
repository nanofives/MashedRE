---
rva: 0x004daa40
name_in_ghidra: FUN_004daa40
size_bytes: 175
confidence: C2
subsystem: render
session: batch-render-6-s6
date: 2026-05-26
pool_slot: Mashed_pool14
prior_plate: re/analysis/bucket_004d7ac0/0x004daa40.md
---

## Shape

- Signature: `int FUN_004daa40(void* param_1)`
- Return: `1` on success
- Subsystem: rw-palette-quantizer

## Mechanical description

Quantizer initializer / state builder.

Phase 1 — fills `&DAT_007d6c98[256]` bit-spread lookup table: for `uVar8` in `[0, 2^DAT_00618430)`, computes `uVar7 = sum over j in [0..DAT_00618430): (uVar8 & (1<<j)) ? (1 << (4*(DAT_00618430-1-j))) : 0` — produces "spread-across-4-channels-with-stride-4" bit pattern used as 16-way key.

Phase 2 — allocates the root quantizer node via FUN_004cc7f0(0x5c, 0x400, 4, 0x30411). Stores the returned arena handle into `*(int*)(param_1+0x4004)`.

Phase 3 — allocates first 0x44+ tree-root node via engine vtable `+0x118` from the arena, stores into `*(int*)(param_1+0x4000)`, zeros its 16 child slots at `+0x1c`.

Returns `1`.

## Constants

| Address | Value | Note |
|---------|-------|------|
| 0x004daa43 | `DAT_00618430` | octree depth |
| 0x004daa4d | `1 << DAT_00618430` | range of LUT index |
| 0x004daa5f | `4` | per-channel bit-stride |
| 0x004daa9a | `&DAT_007d6c98` | LUT base |
| 0x004daaa4 | `0x5c`, `0x400`, `4`, `0x30411` | FUN_004cc7f0 args (arena create) |
| 0x004daac1 | `+0x4000` / `+0x4004` | quantizer state slots |

## Callees (depth-1)

- `FUN_004cc7f0` — arena/pool creator
- engine vtable `+0x118` — alloc

## Callers noted

None observed.

## Uncertainties

None.

## Stubs encountered

- [STUB] FUN_004cc7f0 (arena/pool creator).
- [STUB] engine vtable alloc at `+0x118`.
