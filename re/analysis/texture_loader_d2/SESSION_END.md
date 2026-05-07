---
session_id: texture_loader_d2-20260503-0350
slot_used: Mashed_pool14
slot_requested: Mashed_pool7
slot_note: pool7 acquired by parallel session during preflight; fell back to pool14
anchor_ok: true
sha256: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
session_date: 2026-05-03
---

## Work completed

### DEFERRED rows consumed (all from bucket texture_loader-cont1)
- D-1720 0x0042a470 — classified C1 (platform path builder)
- D-1721 0x00496400 — already C1/mapped; DEFERRED+STUB cleared
- D-1722 0x004cf7d0 — classified C2 (RwTexDictionaryStreamRead)
- D-1723 0x0054f8d0 — classified C1 (native texture bank reader — corrects prior "clump" label)
- D-1724 0x004cc230 — already C1/mapped; DEFERRED+STUBS cleared
- D-1725 0x004cc5e0 — classified C2 (RwStreamFindChunk)
- D-1726 0x004cc160 — already C1/mapped; DEFERRED+STUBS cleared

### New hooks.csv rows
| RVA | Name | Confidence | Notes |
|-----|------|-----------|-------|
| 0042a470 | FUN_0042a470 | C1 | platform-prefixed path builder |
| 004cc5e0 | FUN_004cc5e0 | C2 | RwStreamFindChunk |
| 004cf7d0 | FUN_004cf7d0 | C2 | RwTexDictionaryStreamRead |
| 0054f8d0 | FUN_0054f8d0 | C1 | native texture bank reader (NOT clump) |

### Stubs resolved: S-0421 S-0422 S-0424 S-0600 S-0601 S-0602 S-0603
### DEFERRED cleared: D-1720..D-1726 (all 7)

### New UNCERTAINTIES filed: U-1267..U-1275

### Depth-3 DEFERRED queued (bucket texture_loader_d2-cont1)
| D-ID | RVA | Purpose |
|------|-----|---------|
| D-3700 | 0x004c5890 | RwTexDictionaryCreate candidate |
| D-3701 | 0x004cee90 | native raster reader |
| D-3702 | 0x004c77c0 | RwRasterCreate candidate |
| D-3703 | 0x00550130 | post-chunk-6 shape reader |
| D-3704 | 0x004cbd30 | RwStreamRead candidate |
| D-3705 | 0x004cc400 | chunk header reader |
| D-3706 | 0x004cc050 | chunk body skip |
| D-3707 | 0x004cefd0 | raster post-load processor |
| D-3708 | 0x004c5bc0 | RwTexDictionaryAddTexture candidate |
| D-3709 | 0x004e1b60 | event dispatcher |

## Key findings

1. **0x0054f8d0 is NOT RpClumpStreamRead** — prior session's label was incorrect. Body reads raster data with LOD chains and hardware upload (FUN_004c77c0 = RwRasterCreate pattern). Outer chunk type 0x23 (passed by caller) is unknown — may be Mashed-specific.

2. **0x004cc5e0 confirmed as RwStreamFindChunk** — version range [0x35000..0x37002] matches RW3.x exactly; loop-skip-header pattern matches SDK spec.

3. **0x004cf7d0 confirmed as RwTexDictionaryStreamRead** — internal implementation (not import). Uses struct(type=1) header, count field in struct body, per-texture loop with chunk type 0x15, FUN_004c5890/FUN_004c5bc0 for dict create/add.

4. **004cc230/004cc160 were already classified** in a prior frontend session (localization subsystem found them first).

## Subsystem note
All 10 depth-3 DEFERRED items are stream-layer sub-functions (chunk I/O) or raster-layer sub-functions (image upload). Classifying them will complete the mechanical picture of the RW stream + texture subsystem.
