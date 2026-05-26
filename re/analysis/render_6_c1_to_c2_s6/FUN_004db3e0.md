---
rva: 0x004db3e0
name_in_ghidra: FUN_004db3e0
size_bytes: 367
confidence: C2
subsystem: render
session: batch-render-6-s6
date: 2026-05-26
pool_slot: Mashed_pool14
prior_plate: re/analysis/render_d3d_reset/004db3e0.md
---

## Shape

- Signature: `int FUN_004db3e0(void)`
- Return: `1` on success, `0` on failure
- Subsystem: render-d3d9-vb-ib

## Mechanical description

Creates the shared vertex buffer (VB) and index buffer (IB) for D3D9 rendering.

Sets `DAT_007d7098 = 0`.

If `(DAT_00911fbc & 0x10000) == 0` (large-VB path enabled):
- Calls device vtable `+0x10` (GetAvailableTextureMem, vtable index 4); result in `uVar1`.
- If `uVar1 > 0x800000` (8 MB): calls device vtable `+0x68` (CreateVertexBuffer) with size `0x40000`. On success: sets `DAT_007d7098 = 1`, `DAT_007d709c = 0`.

Unconditional path: sets `DAT_007d70a4 = 0`.

Calls device vtable `+0x6C` (CreateIndexBuffer) count=20000, format `0x65` (D3DFMT_INDEX16). If result `iVar2 == 0` (success):
- Sets `uVar3 = 1`.
- Calls `FUN_004cb8a0(&uStack_3c, &DAT_007d70ac)` — lock/init the index buffer with desc at stack.
- Returns `uVar3` (1).

If CreateIndexBuffer failed — cleanup: releases all previously created buffers via vtable+8 (COM Release) and FUN_004cba80; returns 0.

## Constants

| RVA cited at | Raw hex | Signed dec | Notes |
|---|---|---|---|
| 0x004db3e0 body | 0x800000 | 8388608 | threshold for large VB (8 MB available texture mem) |
| 0x004db3e0 body | 0x40000 | 262144 | large shared VB size in bytes |
| 0x004db3e0 body | 20000 | 20000 | index buffer element count |
| 0x004db3e0 body | 0x65 | 101 | D3DFMT_INDEX16 |
| 0x004db3e0 body | 0x68 | 104 | vtable offset CreateVertexBuffer |
| 0x004db3e0 body | 0x6C | 108 | vtable offset CreateIndexBuffer |
| 0x004db3e0 body | 0x10 | 16 | vtable offset GetAvailableTextureMem |

## Callees (depth-1)

- `FUN_004cb8a0` — locks/inits index buffer with descriptor struct (DEFERRED D-2020; S-0702)
- `FUN_004cba80` — releases index buffer object at address (DEFERRED D-2020; S-0703)
- D3D9 device vtable `+0x10`, `+0x68`, `+0x6C`, `+0x8` (COM interface calls)

## Callers noted

None observed.

## Uncertainties

None.

## Stubs encountered

- [STUB S-0702] 0x004cb8a0 FUN_004cb8a0 — locks/inits index buffer; DEFERRED D-2020.
- [STUB S-0703] 0x004cba80 FUN_004cba80 — releases index buffer; DEFERRED D-2020.
