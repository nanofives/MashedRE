# FUN_004cbb20 — D3DSetTextureStageState C1→C2

**RVA:** 0x004cbb20
**Body:** 0x004cbb20..0x004cbb41
**Session:** batch-render-4-s6
**Pool:** Mashed_pool14
**U-id:** U-5034

## Decompilation summary

Thin wrapper around D3D9 `SetTextureStageState`.

- At 0x004cbb20: `iVar1 = (*(*DAT_007d4110)[0x1a8])(DAT_007d4110, param_1, param_2)` — calls vtable byte-offset 0x1a8 = index 0x6a = `SetTextureStageState(stage, type, value)`. [UNCERTAIN: standard signature takes 3 args (stage, type, value); decompiler shows only 2 + implicit device; may be (stage, type) with value rolled into type encoding.]
- Returns `iVar1 >= 0`.

Global reads:
- `DAT_007d4110` at 0x004cbb20 — D3D device pointer

## Callers (2)
- `FUN_0049aae0` (0x0049aae0) — RainMode1Init
- `FUN_00543710` (0x00543710)

## Callees (0)
Leaf.

## C2 evidence
- Full decompilation read; vtable offset 0x1a8 and HRESULT check cited.
- [UNCERTAIN] on arg count flagged.

## Line count
~6 decompiled lines — within 300-line cap.
