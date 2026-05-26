# FUN_004cb4c0 — D3DSetLightMatrix C1→C2

**RVA:** 0x004cb4c0
**Body:** 0x004cb4c0..0x004cb5ff
**Session:** batch-render-4-s6
**Pool:** Mashed_pool14
**U-id:** U-5026

## Decompilation summary

Sets a D3D light matrix. Takes a `u32* light_struct` (or NULL) and dispatches two code paths:

**Null / already-enabled path (param_1 == NULL or `param_1[3] & 0x20000`):**
- At 0x004cb4c9: if `DAT_007d4154 == 0`, copies 16 dwords from `DAT_005d8b90` (identity/default matrix) into `DAT_007d4558`, then calls vtable method `(*DAT_007d4110)[0xb0](DAT_007d4110, 0x100, &DAT_005d8b90)`.
- Sets `DAT_007d4154 = 1`. Returns `iVar2 >= 0`.

**Normal path (param_1 != NULL and `!(param_1[3] & 0x20000)`):**
- At 0x004cb4eb..0x004cb545: copies 12 float/dword fields from `param_1` into a packed global struct starting at `DAT_00618258` (offsets [0],[1],[2],[4],[5],[6],[8],[9],[10],[12],[13],[14] — 3×4 matrix rows excluding [3],[7],[11]).
- At 0x004cb54c: if `DAT_007d4154 != 0`, jumps to LAB_004cb57b and immediately uploads via `(*DAT_007d4110)[0xb0]` using `&DAT_00618258`.
- Otherwise (DAT_007d4154 == 0): compares the 16 dwords of the newly built struct against the cache at `DAT_007d4558 - 0x186096` [UNCERTAIN: offset arithmetic is non-obvious, raw: `DAT_007d4558 + (-0x186096 + (int)piVar1) - (int)piVar1`; delta = DAT_007d4558 - &DAT_00618258 = 0x186096 raw signed]. If any dword differs, uploads and clears `DAT_007d4154 = 0`.
- If all 16 dwords match → no upload, returns true.

Global reads/writes:
- `DAT_007d4154` at 0x004cb4d4 — dirty flag (1 = identity active, 0 = normal matrix active)
- `DAT_005d8b90` at 0x004cb4e1 — identity/default matrix (16 dwords)
- `DAT_007d4558` at 0x004cb4db — cache buffer pointer (16 dwords)
- `DAT_007d4110` at 0x004cb4f3 — D3D device vtable pointer
- `DAT_00618258` at 0x004cb4eb — unpacked matrix staging area (3×4 floats)

Vtable call: `(*DAT_007d4110)[vtable+0xb0]` — [UNCERTAIN: D3D9 IDirect3DDevice9 vtable offset 0xb0 = `SetTransform` (transform type 0x100 = D3DTS_WORLD)]

## Callers (0)
None found by MCP.

## Callees (0)
Leaf (vtable calls only, no named sub-calls).

## C2 evidence
- Full decompilation read; all branches and data addresses cited.
- Two code paths fully traced; global dirty-flag protocol documented.
- One [UNCERTAIN] on vtable offset interpretation flagged.

## Line count
~60 decompiled lines — within 300-line cap.
