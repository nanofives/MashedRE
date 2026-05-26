# FUN_004b3cc0 — StreamReadChunk0c C1→C2

**RVA:** 0x004b3cc0
**Body:** 0x004b3cc0..0x004b3d11
**Session:** batch-render-3-s2
**Pool:** Mashed_pool10
**U-id:** U-4735

## Decompilation summary

Structurally identical to FUN_004b3c60 (chunk 0x0b), differing only in chunk ID and data processor:
1. `iVar1 = FUN_004cc230(2, 1, param_1)` at 0x004b3cce — open stream
2. If `iVar1 == 0`: return 0.
3. `iVar2 = FUN_004cc5e0(iVar1, 0xc, param_2, param_3)` at 0x004b3ce3 — find chunk type 0x0c (12 decimal)
4. If `iVar2 != 0`: `uVar3 = FUN_00545260(iVar1)` — process chunk 0x0c data (spline processor per D-5742 note)
5. `FUN_004cc160(iVar1, 0)` — close stream
6. Return `uVar3`

Chunk type: 0x0c = 12 decimal. Processor: FUN_00545260 (spline data reader per prior note D-5742).

## Callers (1)
- `FUN_0042a7f0` (0x0042a7f0) — render C2 (batch-render-s2): spline file loader (.spl)

## Callees (4)
- `FUN_004cc160` (0x004cc160) — stream close
- `FUN_004cc230` (0x004cc230) — stream open
- `FUN_004cc5e0` (0x004cc5e0) — find chunk
- `FUN_00545260` (0x00545260) — spline chunk processor

## C2 evidence
- Full decompilation read; pattern is the same as FUN_004b3c60, only chunk ID and processor differ.
- Caller FUN_0042a7f0 is C2 (spline file loader); prior note D-5742 documents this function.
- No UNCERTAIN items.

## Line count
~18 decompiled lines — within 300-line cap.
