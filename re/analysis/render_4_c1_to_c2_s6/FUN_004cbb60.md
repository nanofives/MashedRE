# FUN_004cbb60 — GetRenderStateBlockPtr C1→C2

**RVA:** 0x004cbb60
**Body:** 0x004cbb60..0x004cbb65
**Session:** batch-render-4-s6
**Pool:** Mashed_pool14
**U-id:** U-5036

## Decompilation summary

Returns address of global `DAT_00911fa0`.

- At 0x004cbb60: `return &DAT_00911fa0`.
- Body size: 6 bytes. Single-instruction getter.

Global reads:
- `DAT_00911fa0` at 0x004cbb60 — the returned global (type unknown; callers store the pointer and dereference at +0xC4/+0xCC per existing C1 note)

## Callers (6)
- `FUN_004951f0` (0x004951f0)
- `FUN_004f46a0` (0x004f46a0)
- `FUN_00530c00` (0x00530c00)
- `FUN_005327d0` (0x005327d0)
- `FUN_00543710` (0x00543710)
- `FUN_00549970` (0x00549970)

## Callees (0)
Leaf.

## C2 evidence
- Single-instruction body; trivially verified.
- Return address DAT_00911fa0 cited.
- No UNCERTAIN items.

## Line count
~2 decompiled lines — within 300-line cap.
