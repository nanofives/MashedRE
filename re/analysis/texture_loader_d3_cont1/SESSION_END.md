---
session_id: texture_loader_d3_cont1-20260512
slot_used: Mashed_pool0
slot_requested: Mashed_pool4
slot_note: pool4 not available; pool0 acquired via ghidra_pool.sh (first unlocked slot)
anchor_ok: true
sha256: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
session_date: 2026-05-12
---

## Work completed

### DEFERRED rows consumed (all 10 from bucket texture_loader_d3-cont1, D-7900..D-7909)
- D-7900 0x004c5800 — classified C2 (RwTexDictionarySetCurrent)
- D-7901 0x004c5820 — classified C2 (RwTexDictionaryGetCurrent)
- D-7902 0x004c5830 — classified C1 (texture plugin slot setter +0x20)
- D-7903 0x004c5850 — classified C1 (texture plugin slot getter +0x20)
- D-7904 0x004c5a00 — classified C2 (RwTextureCreate)
- D-7905 0x004c5ae0 — classified C2 (RwTextureSetName)
- D-7906 0x004c5b50 — classified C2 (RwTextureSetMaskName)
- D-7907 0x004d8810 — classified C1 (native raster stream reader; NOT state guard — resolves U-2676)
- D-7908 0x004e1df0 — classified C2 (extension chunk drain / error cleanup)
- D-7909 0x004cdd60 — C1 retained (alloc path confirmed; RwImage layout fully documented)

### New hooks.csv rows
| RVA | Name | Status | Notes |
|-----|------|--------|-------|
| 004c5800 | FUN_004c5800 | C2 | RwTexDictionarySetCurrent |
| 004c5820 | FUN_004c5820 | C2 | RwTexDictionaryGetCurrent |
| 004c5830 | FUN_004c5830 | C1 | texture plugin slot setter +0x20; U-3666 |
| 004c5850 | FUN_004c5850 | C1 | texture plugin slot getter +0x20; U-3666 |
| 004c5a00 | FUN_004c5a00 | C2 | RwTextureCreate |
| 004c5ae0 | FUN_004c5ae0 | C2 | RwTextureSetName |
| 004c5b50 | FUN_004c5b50 | C2 | RwTextureSetMaskName |
| 004d8810 | FUN_004d8810 | C1 | native raster stream reader; U-3664 U-3665 |
| 004e1df0 | FUN_004e1df0 | C2 | extension chunk drain / error cleanup |

(004cdd60 row already exists at C1; not duplicated.)

### Struct doc written
- `re/analysis/structs/txd_texture.md` — first struct doc for the texture layer, covering RwTexture, RwTexDictionary, RwImage, plugin globals layout, and allocator vtable slots.

### Depth-1 callees walk
All depth-1 callees of the 10 functions are already mapped in hooks.csv:
- FUN_004d8000 (C1, render), FUN_004d7ff0 (C1), FUN_004d8480 (C1)
- FUN_004cc400 (C2), FUN_004cbd30 (C2), FUN_004cc050 (C2), FUN_004cc5e0 (C2)
- All vtable slots (+0xd0 strncpy, +0xf4 strlen, +0x108/+0x118 alloc) are known

No new depth-1 deferred rows required.

### Uncertainties resolved
- **U-2676** — RESOLVED: FUN_004d8810 is a native raster stream reader, not a state guard.
- **U-2677** — PARTIALLY RESOLVED: +0x54=refCount and +0x50=filterAddressing confirmed; the conditional zero semantics remain open.

### New uncertainties filed: U-3664..U-3666
| ID | Function | Statement |
|----|----------|-----------|
| U-3664 | 0x004d8810 | Chunk types 2 / 0x13 — cannot map to RW3 chunk IDs from Ghidra |
| U-3665 | 0x004d8810 | Alloc type 0x30002 — pool/class unknown |
| U-3666 | 0x004c5830 / 0x004c5850 | Plugin data slot +0x20 — no public RW3 API name |

### DEFERRED cleared: D-7900..D-7909 (all 10)
### Stubs filed: none (all callees already mapped)

## Key findings

1. **RwTextureCreate confirmed C2 (0x004c5a00)** — full RwTexture struct layout extracted:
   - +0x00: raster; +0x04: dict=NULL; +0x10/+0x30: name/mask (32B each); +0x50: filterAddressing (filter=1, addrUV=0x11); +0x54: refCount=1. sizeof=0x58.

2. **RwTexDictionarySetCurrent/GetCurrent confirmed C2 (0x004c5800/5820)** — write/read slot at DAT_007d4054+0x1c+DAT_007d3ff8. Plugin data block layout for offset 0x1c (TXD) and 0x20 (texture scratch) now documented.

3. **RwTextureSetName/SetMaskName confirmed C2 (0x004c5ae0/5b50)** — 32-byte strncpy with 31-char overflow guard. Confirms tex->name at +0x10 and tex->mask at +0x30.

4. **FUN_004d8810 is NOT a state guard** — it's a 608-byte native raster stream reader handling chunk types 2 (binary, 64B blocks) and 0x13 (interleaved, 128B→64B depack). Resolves U-2676. The "two calls" in 0x00550130's decompile was a Ghidra artifact from dropped unreachable blocks.

5. **Extension chunk drain confirmed C2 (0x004e1df0)** — FindChunk(type=3)+skip-all-sub-chunks pattern is complete and unambiguous.

6. **RwImage struct fully mapped (from 0x004cdd60)** — +0x00: flags; +0x04: width; +0x08: height; +0x0c: depth; +0x10: stride; +0x14: cpPixels; +0x18: palette. Single allocation covers pixel+palette for indexed formats.

7. **RwTextureStreamRead (0x00550130) now 8/9 callees resolved** — only U-2675 (dropped unreachable blocks) blocks C2. Promoting 0x00550130 to C2 pending listing_disassemble_range check.

## Subsystem note
RW texture I/O layer is now ~95% classified. Remaining open items:
- FUN_004c5830/5850 (plugin slot +0x20 purpose — C2 blocked by U-3666)
- FUN_004d8810 chunk type mapping (C2 blocked by U-3664/U-3665)
- FUN_00550130 unreachable blocks (U-2675 — C2 blocked)
The RwTexture struct is fully documented in txd_texture.md.
