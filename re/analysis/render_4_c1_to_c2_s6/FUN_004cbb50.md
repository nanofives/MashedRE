# FUN_004cbb50 — D3DCOMRelease C1→C2

**RVA:** 0x004cbb50
**Body:** 0x004cbb50..0x004cbb5a
**Session:** batch-render-4-s6
**Pool:** Mashed_pool14
**U-id:** U-5035

## Decompilation summary

Calls `IUnknown::Release()` on a COM object pointer.

- At 0x004cbb50: `(*(*param_1 + 8))(param_1)` — calls vtable offset +8 on `*param_1`. IUnknown vtable layout: `[0]` = QueryInterface, `[4]` = AddRef, `[8]` = Release.
- Returns void.

Leaf, no globals read or written.

## Callers (3)
- `FUN_0049ab10` (0x0049ab10) — RainMode1Teardown; releases two handles
- `FUN_004f8730` (0x004f8730)
- `FUN_00543a40` (0x00543a40)

## Callees (0)
Leaf.

## C2 evidence
- Trivially readable; vtable+8 = IUnknown::Release is authoritative.
- No UNCERTAIN items.

## Line count
~3 decompiled lines — within 300-line cap.
