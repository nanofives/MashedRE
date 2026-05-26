# FUN_004cbad0 — D3DSetRenderState C1→C2

**RVA:** 0x004cbad0
**Body:** 0x004cbad0..0x004cbaff
**Session:** batch-render-4-s6
**Pool:** Mashed_pool14
**U-id:** U-5032

## Decompilation summary

Sets a D3D render state and invalidates a global cache slot.

- At 0x004cbad0: `iVar1 = (*(*DAT_007d4110)[0x16c])(DAT_007d4110, param_1, param_2)` — calls D3D9 `SetRenderState(param_1, param_2)`. Vtable byte-offset 0x16c = index 0x5b = `SetRenderState`.
- At 0x004cbaea: if `iVar1 >= 0` (success), writes `DAT_006181d0 = 0xffffffff` — invalidates a cached value.
- Returns `iVar1 >= 0`.

Global reads/writes:
- `DAT_007d4110` at 0x004cbad0 — D3D device pointer
- `DAT_006181d0` at 0x004cbaea — state cache sentinel (set to 0xffffffff on any state change)

## Callers (1)
- `FUN_0049aae0` (0x0049aae0) — RainMode1Init; calls this to establish render state

## Callees (0)
Leaf.

## C2 evidence
- Full decompilation read; vtable offset 0x16c and cache invalidation at DAT_006181d0 cited.
- No UNCERTAIN items.

## Line count
~8 decompiled lines — within 300-line cap.
