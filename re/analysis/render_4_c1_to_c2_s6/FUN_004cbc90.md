# FUN_004cbc90 — GetD3DDeviceHandle C1→C2

**RVA:** 0x004cbc90
**Body:** 0x004cbc90..0x004cbc95
**Session:** batch-render-4-s6
**Pool:** Mashed_pool14
**U-id:** U-5039

## Decompilation summary

Returns the value of global `DAT_007d459c`.

- At 0x004cbc90: `return DAT_007d459c`.
- Body size: 6 bytes. Single-instruction getter. Companion setter noted at 0x004cbc80 (per prior C1 note).

Global reads:
- `DAT_007d459c` at 0x004cbc90 — the returned 4-byte value (D3D device handle or related handle)

## Callers (2)
- `FUN_00493710` (0x00493710)
- `FUN_004f42d0` (0x004f42d0)

## Callees (0)
Leaf.

## C2 evidence
- Single-instruction body; trivially verified.
- Setter companion at 0x004cbc80 noted from prior C1 record.
- No UNCERTAIN items.

## Line count
~2 decompiled lines — within 300-line cap.
