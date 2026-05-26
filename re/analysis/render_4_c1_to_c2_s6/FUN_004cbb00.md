# FUN_004cbb00 — D3DSetTexture C1→C2

**RVA:** 0x004cbb00
**Body:** 0x004cbb00..0x004cbb1d
**Session:** batch-render-4-s6
**Pool:** Mashed_pool14
**U-id:** U-5033

## Decompilation summary

Thin wrapper around D3D9 `SetTexture`.

- At 0x004cbb00: `(*(*DAT_007d4110)[0x178])(DAT_007d4110, param_1, param_2, param_3)` — calls vtable offset 0x178.
  - Vtable byte-offset 0x178 = index 0x5e = `SetTexture(stage, pTexture)`. [UNCERTAIN: 3 args passed; standard D3D9 SetTexture takes (stage, texture); param_3 presence suggests a different vtable or extended interface.]
- Returns void.

Global reads:
- `DAT_007d4110` at 0x004cbb00 — D3D device pointer

## Callers (1)
- `FUN_0049a750` (0x0049a750)

## Callees (0)
Leaf.

## C2 evidence
- Full decompilation read; vtable offset 0x178 cited; 3-arg anomaly noted as [UNCERTAIN].

## Line count
~5 decompiled lines — within 300-line cap.
