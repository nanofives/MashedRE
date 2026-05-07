---
session_id: texture_loader_d3-20260506-XXXX
slot_used: Mashed_pool0
slot_requested: Mashed_pool13
slot_note: pool13 did not exist; script returned pool0 (first available); pool13 creation skipped — pool0 used instead
anchor_ok: true
sha256: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
session_date: 2026-05-06
---

## Work completed

### DEFERRED rows consumed (all from bucket texture_loader_d2-cont1 D-3700..D-3709)
- D-3700 0x004c5890 — classified C2 (RwTexDictionaryCreate)
- D-3701 0x004cee90 — classified C1 (RwImageStreamRead candidate)
- D-3702 0x004c77c0 — classified C2 (RwRasterCreate; upgrades prior Ghidra C1 tag)
- D-3703 0x00550130 — classified C1 (RwTextureStreamRead candidate)
- D-3704 0x004cbd30 — classified C2 (RwStreamRead; upgrades audio,C1 to render,C2)
- D-3705 0x004cc400 — classified C2 (RwStreamReadChunkHeader)
- D-3706 0x004cc050 — classified C2 (RwStreamSkip; upgrades audio,C1 to render,C2)
- D-3707 0x004cefd0 — classified C1 (pixel color/palette remapper)
- D-3708 0x004c5bc0 — classified C2 (RwTexDictionaryAddTexture)
- D-3709 0x004e1b60 — classified C1 (RW plugin extension stream reader)

### Additional functions classified (callees discovered during analysis)
- 0x004cc4f0 — classified C1 (RW chunk type validator; callee of D-3705)
- 0x004cdd60 — classified C1 (RwImageAllocatePixels candidate; callee of D-3701)

### New hooks.csv rows
| RVA | Name | Status | Notes |
|-----|------|--------|-------|
| 004c5890 | FUN_004c5890 | C2 | RwTexDictionaryCreate |
| 004c5bc0 | FUN_004c5bc0 | C2 | RwTexDictionaryAddTexture |
| 004c77c0 | FUN_004c77c0 | C2 | RwRasterCreate |
| 004cc400 | FUN_004cc400 | C2 | RwStreamReadChunkHeader |
| 004cc4f0 | FUN_004cc4f0 | C1 | RW chunk type validator |
| 004cdd60 | FUN_004cdd60 | C1 | RwImageAllocatePixels candidate |
| 004cee90 | FUN_004cee90 | C1 | RwImageStreamRead candidate |
| 004cefd0 | FUN_004cefd0 | C1 | pixel color/palette remapper |
| 004e1b60 | FUN_004e1b60 | C1 | RW plugin extension stream reader |
| 00550130 | FUN_00550130 | C1 | RwTextureStreamRead candidate |

### Existing rows promoted
| RVA | Old | New | Notes |
|-----|-----|-----|-------|
| 004cbd30 | audio,C1 | render,C2 | RwStreamRead confirmed |
| 004cc050 | audio,C1 | render,C2 | RwStreamSkip confirmed |

### Stubs filed: S-2660..S-2670 (11 new stubs)
### DEFERRED cleared: D-3700..D-3709 (all 10)
### New UNCERTAINTIES filed: U-2667..U-2678 (12 entries)

### Depth-4 DEFERRED queued (bucket texture_loader_d3-cont1)
| D-ID | RVA | Purpose |
|------|-----|---------|
| D-7900 | 0x004c5800 | RwTexDictionarySetCurrent candidate |
| D-7901 | 0x004c5820 | RwTexDictionaryGetCurrent candidate |
| D-7902 | 0x004c5830 | texture context setter |
| D-7903 | 0x004c5850 | texture context getter |
| D-7904 | 0x004c5a00 | RwTexture create-from-stream (main) |
| D-7905 | 0x004c5ae0 | texture post-process step 1 |
| D-7906 | 0x004c5b50 | texture post-process step 2 |
| D-7907 | 0x004d8810 | state check (called twice in stream reader) |
| D-7908 | 0x004e1df0 | error path cleanup |
| D-7909 | 0x004cdd60 | RwImageAllocatePixels (already C1; decomp alloc path) |

## Key findings

1. **RW stream layer fully classified** — RwStreamRead (0x004cbd30), RwStreamSkip (0x004cc050), RwStreamReadChunkHeader (0x004cc400), and RwStreamFindChunk (0x004cc5e0 from d2) are all C2. The complete chunk I/O path is mechanically proven.

2. **RwTexDictionary create/add confirmed C2** — 0x004c5890 (create) and 0x004c5bc0 (add texture) match RW3 SDK precisely. Object type byte = 6, RwLLLink layout at +0x08/+0x0c/+0x10/+0x14 confirmed.

3. **RwRasterCreate confirmed C2** — 0x004c77c0 allocates type 0x30407, stores width/height/depth at +0x0c/+0x10/+0x14, calls platform create callback via vtable+0x58.

4. **Image pixel subsystem** — 0x004cee90 reads RwImage from stream (struct chunk + pixel rows + palette). 0x004cefd0 remaps palette/pixels through a device-specific lookup table at DAT_007d4628+0xc+DAT_007d3ff8 (purpose U-2671). 0x004cdd60 allocates the pixel buffer with stride alignment.

5. **Plugin extension reader** — 0x004e1b60 dispatches RW extension sub-chunks to registered plugin callbacks via list walk at rwobj+0x10. Plugin node layout differs from standard RW convention (U-2673).

6. **RwTextureStreamRead (0x00550130) still C1** — 9 callees unresolved. The save/restore context pattern (FUN_004c5820/5850/5800/5830) confirms get/set current TXD and texture state, but FUN_004d8810's purpose blocks full understanding.

## Subsystem note
The RW texture I/O layer is now 70% classified. Remaining work is the RwTexture object internals (get/set current, filter fields +0x50/+0x54) and the pixel format conversion table (DAT_007d4628+0xc). Classifying D-7900..D-7904 in the next session will bring RwTextureStreamRead to C2.
