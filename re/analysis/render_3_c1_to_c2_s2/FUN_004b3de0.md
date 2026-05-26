# FUN_004b3de0 — StreamReadChunk1b C1→C2

**RVA:** 0x004b3de0
**Body:** 0x004b3de0..0x004b3e31
**Session:** batch-render-3-s2
**Pool:** Mashed_pool10
**U-id:** U-4737

## Decompilation summary

Structurally identical to FUN_004b3c60 pattern, chunk ID 0x1b:
1. `iVar1 = FUN_004cc230(2, 1, param_1)` — open stream
2. If `iVar1 == 0`: return 0.
3. `iVar2 = FUN_004cc5e0(iVar1, 0x1b, param_2, param_3)` — find chunk type 0x1b (27 decimal)
4. If `iVar2 != 0`: `uVar3 = FUN_0052daf0(iVar1)` — process chunk 0x1b data (animation data per D-5743 note)
5. `FUN_004cc160(iVar1, 0)` — close stream
6. Return `uVar3`

Chunk type: 0x1b = 27 decimal. Processor: FUN_0052daf0 (animation / .anm data reader per prior note D-5743).

## Callers (1)
- `FUN_0042a860` (0x0042a860) — render C2 (batch-render-s2): animation path file loader (.anm)

## Callees (4)
- `FUN_004cc160` (0x004cc160) — stream close
- `FUN_004cc230` (0x004cc230) — stream open
- `FUN_004cc5e0` (0x004cc5e0) — find chunk
- `FUN_0052daf0` (0x0052daf0) — animation chunk 0x1b processor

## C2 evidence
- Full decompilation read; pattern is identical to the sibling cluster members.
- Caller FUN_0042a860 is C2; prior note D-5743 documents this function.
- No UNCERTAIN items.

## Line count
~18 decompiled lines — within 300-line cap.
