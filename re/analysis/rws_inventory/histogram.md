# RWS Chunk ID Histogram — original/toastaudio/

Session: rws_inventory-20260502  
Files inspected: 45  
Distinct section IDs found: 6  
All IDs identified by vendor; chunk type names unresolved for 6 IDs → filed as Mysteries.

## Histogram

| section_id_hex | occurrence_count | total_bytes | distinct_files | identified_name | source |
|----------------|-----------------|-------------|----------------|-----------------|--------|
| 0x0000080D | 31 | 181,477,004 | 31 | U-0150 (RWA vendor 0x08, chunkID=0x0D; stream root container) | criterion-rwg37/misc/inc/rwplcore.h:1901 (vendor only) |
| 0x0000080E | 31 | 246,692 | 31 | U-0151 (RWA vendor 0x08, chunkID=0x0E; stream header sub-chunk) | criterion-rwg37/misc/inc/rwplcore.h:1901 (vendor only) |
| 0x0000080F | 31 | 181,229,568 | 31 | U-0152 (RWA vendor 0x08, chunkID=0x0F; stream audio payload, leaf) | criterion-rwg37/misc/inc/rwplcore.h:1901 (vendor only) |
| 0x00000809 | 14 | 26,278,684 | 14 | U-0147 (RWA vendor 0x08, chunkID=0x09; dictionary root container) | criterion-rwg37/misc/inc/rwplcore.h:1901 (vendor only) |
| 0x0000080A | 14 | 952 | 14 | U-0148 (RWA vendor 0x08, chunkID=0x0A; dictionary header sub-chunk) | criterion-rwg37/misc/inc/rwplcore.h:1901 (vendor only) |
| 0x0000080C | 14 | 26,277,396 | 14 | U-0149 (RWA vendor 0x08, chunkID=0x0C; dictionary audio payload, leaf) | criterion-rwg37/misc/inc/rwplcore.h:1901 (vendor only) |

**Total distinct files: 45** (31 of one type + 14 of another; no file appears in both groups)

## File type structure

The 45 .rws files fall into two structural patterns:

**Type A — 31 files** (individual track/speech streams; per-language files in `pcdics/english/`, `french/`, `german/`, `italian/`, `spanish/`; also most `pcdics/*.rws`):
```
0x0000080D  (root container, version 0x1C020018 = RW 3.7.0.2)
  0x0000080E  (header sub-chunk, ~8 KB)
  0x0000080F  (audio payload, leaf, ~5.8 MB average)
```

**Type B — 14 files** (dictionary/bank files: `arctic.rws`, `city.rws`, `dump.rws`, `egypt.rws`, `forest.rws`, `highway.rws`, `neustein.rws`, `permdict.rws`, `rouabout.rws`, `sands.rws`, `storm.rws`, `superg.rws`, `training.rws`, `warzone.rws`):
```
0x00000809  (root container, version 0x1C020018 = RW 3.7.0.2)
  0x0000080A  (header sub-chunk, exactly 68 bytes each)
  0x0000080C  (audio payload, leaf, ~1.8 MB average)
```

All chunks carry version stamp `0x1C020018`, which decodes to RenderWare 3.7.0.2 via `libraryIDUnpackVersion` (librw/src/rwbase.h:675–681).

## Vendor identification

All 6 IDs have vendor nibble 0x08. The vendor is identified:

> `rwVENDORID_CRITERIONRWA = 0x000008L, /* RenderWare Audio */`  
> — criterion-rwg37/misc/inc/rwplcore.h:1901  
> — criterion-rwg37/core/src/plcore/batype.h:56

The specific chunk type names within the RWA SDK (chunkID values 0x09, 0x0A, 0x0C, 0x0D, 0x0E, 0x0F) are **not present** in criterion-rwg37, librw, or librwgta. RenderWare Audio was a separate middleware SDK; its chunk definition headers are not in the available reference tree.

## Sanity check: 0x0000000B

The session prompt states: "section_id 0x0000000B is rwID_TEXDICTIONARY in graphics RW; for audio (RWS) it is the audio root container."

**Empirical result: 0x0000000B does NOT appear in any of the 45 .rws files.**

Two observations:

1. The identification "rwID_TEXDICTIONARY" for 0x0000000B is incorrect per both references:
   - librw/src/rwbase.h:579–580: `ID_WORLD = MAKEPLUGINID(VEND_CORE, 0x0B) = 0x0000000B`
   - criterion-rwg37/misc/inc/rwplcore.h:1959: `rwID_WORLD = MAKECHUNKID(rwVENDORID_CORE, 0x0B)`
   - rwID_TEXDICTIONARY is 0x00000016 (MAKECHUNKID(rwVENDORID_CORE, 0x16)).

2. The audio root containers in Mashed's .rws files are **0x00000809** (Type B, 14 files) and **0x0000080D** (Type A, 31 files) — both in the RWA vendor range, not the core RW range. The graphic-RW chunk 0x0000000B (rwID_WORLD) is absent from all audio files.

## Mysteries

IDs whose vendor is identified (rwVENDORID_CRITERIONRWA = 0x08) but whose chunk type name is not resolvable from available references:

| U-ID | section_id_hex | RWA chunkID | Observed role | Evidence gap |
|------|----------------|-------------|---------------|--------------|
| U-0147 | 0x00000809 | 0x09 | Root container for Type B (dictionary) files; contains 0x080A+0x080C | RWA SDK chunk type names not in criterion-rwg37 or librw |
| U-0148 | 0x0000080A | 0x0A | Header sub-chunk of Type B root; fixed 68-byte payload per file | Same |
| U-0149 | 0x0000080C | 0x0C | Audio payload leaf in Type B files; large binary blob (1–2 MB) | Same |
| U-0150 | 0x0000080D | 0x0D | Root container for Type A (stream) files; contains 0x080E+0x080F | Same |
| U-0151 | 0x0000080E | 0x0E | Header sub-chunk of Type A root; ~8 KB payload per file | Same |
| U-0152 | 0x0000080F | 0x0F | Audio payload leaf in Type A files; large binary blob (~5 MB) | Same |

Resolution path: obtain the RenderWare Audio SDK headers (rwaudioplugin.h or equivalent) and grep for `MAKECHUNKID(0x08, 0x09)` etc., or consult the GTAModding wiki's RWS audio format documentation.
