---
rva: 0x004dba60
name_in_ghidra: FUN_004dba60
size_bytes: 533
confidence: C2
subsystem: render
session: batch-render-6-s6
date: 2026-05-26
pool_slot: Mashed_pool14
prior_plate: re/analysis/bucket_004d7ac0/0x004dba60.md
---

## Shape

- Signature: `void FUN_004dba60(int param_1, void* param_2, int param_3)`
- Return: void
- Subsystem: rw-d3d9-immediate-draw

## Mechanical description

Submits `param_3` vertices (28 bytes each = 0x1c stride) at primitive-type `param_1` (1..5) to D3D9:

Primitive-count switch:
- case 1: prim count = `param_3 / 2` (LINELIST)
- case 2: prim count = `param_3 - 1` (LINESTRIP)
- case 3: prim count = `param_3 / 3` (TRIANGLELIST)
- case 4, 5: prim count = `param_3 - 2` (STRIP/FAN)

Acquires a write region in the global vertex buffer ring:
- If buffer not initialized (`DAT_007d7098 == 0`): calls FUN_004dcbb0(0x1c, param_3, &DAT_007d70a0, &local_8, &DAT_007d709c) — allocate fresh.
- Else if remaining capacity ≥ `param_3` (against threshold 0x2493 = 9363 verts): D3D9 vtable Lock at `+0x2c` with `0x1800` = D3DLOCK_NOOVERWRITE.
- Else (wrap): resets `DAT_007d709c = 0`, Locks with `0x2800` = D3DLOCK_DISCARD.

Two copy modes depending on viewport offset (`*(short*)(iVar3+0x1c)` and `+0x1e` both zero → memcpy; else — translate by (sx, sy) on first two floats while copying remaining 20 bytes raw per vertex).

Unlocks via D3D9 vtable `+0x30`.

Calls FUN_004db770 (render-state / stream setup).

DrawPrimitive via device vtable `+0x144` on `DAT_007d4110`.

## Constants

| Address | Value | Note |
|---------|-------|------|
| 0x004dbab8 | `0x1c` (28) | vertex stride |
| 0x004dbb20 | `0x2493` | max-vertex-count threshold (9363) |
| 0x004dbb29 | `0x1800` | D3DLOCK_NOOVERWRITE |
| 0x004dbb35 | `0x2800` | D3DLOCK_DISCARD |
| 0x004dbb91 | `DAT_007d70a0` | vertex buffer COM ptr |
| 0x004dbc34 | `DAT_007d4110` | D3D device COM ptr |
| 0x004dbc41 | `+0x144` | DrawPrimitive vtable slot (vtable[0x51]) |
| 0x004dbb2c | `+0x2c` | vertex-buffer Lock vtable slot |
| 0x004dbbac | `+0x30` | vertex-buffer Unlock vtable slot |

## Callees (depth-1)

- `FUN_004c75c0` — default camera/viewport accessor
- `FUN_004dcbb0` — ring-buffer initial-allocator
- `FUN_004db770` — render-state / stream setup (this batch)
- D3D9 device vtable `+0x144` (DrawPrimitive)
- D3D9 vertex buffer vtable `+0x2c` (Lock), `+0x30` (Unlock)

## Callers noted

None observed.

## Uncertainties

- [UNCERTAIN] U-5427: `(*(int*)(&DAT_005d8d8c + unaff_retaddr*4))` access using `unaff_retaddr` — Ghidra missed a register-passed arg, likely prim-type re-fed from caller's stack.

## Stubs encountered

- [STUB] FUN_004c75c0 (default camera/viewport accessor).
- [STUB] FUN_004dcbb0 (ring-buffer initial-allocator).
- [STUB] D3D9 vertex-buffer Lock/Unlock vtable, DrawPrimitive vtable slot (+0x144).
