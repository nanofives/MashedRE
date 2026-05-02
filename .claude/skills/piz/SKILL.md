---
name: piz
description: Extract, inspect, or repack Mashed `.piz` archive files. Use this skill when the user wants to look inside a .piz file, list its entries, export individual files, or rebuild a .piz from a directory. Triggers on words like "piz", "extract", "asset archive", "Frontend.piz", "AI.piz", "TRACKS\\*.piz", "VEHICLES\\*.piz".
---

# Mashed .piz archive skill

Mashed's asset containers — `Common\*.piz`, `TRACKS\*.piz`, `VEHICLES\*.piz`, `POWERUPS\*.piz`, `PANEL\*.piz` — are a custom format documented on XeNTaX. The first-pass parser is at `re/tools/piz_extract.py`.

## Format (verified against XeNTaX thread + Common/AI.piz first 16 bytes)

```
Header (2048 bytes total):
  +0x00  4B  magic     "PIZ\0"
  +0x04  4B  version   uint32  (= 3 in all known files)
  +0x08  4B  count     uint32  (number of file entries)
  +0x0C  ?   pad       null bytes up to 2048

File entries (each 0x80 = 128 bytes):
  +0x00 116B name      null-padded ASCII path
  +0x74   4B offset    uint32  (file offset, in 2048-byte sectors? OR raw bytes — verify on first run)
  +0x78   4B length    uint32  (raw byte length)
  +0x7C   4B id        uint32  (file id / hash — purpose unconfirmed)

Data:
  Files concatenated, each padded to 2048-byte boundary.
```

The offset semantics (sector vs raw bytes) is the one variable we have to confirm on a real archive — the parser tries raw bytes first and falls back to sector × 2048 if the read goes out of bounds.

**Authoritative reference**: `re/prior_art/MashedFileExtractor/` (SciLor, C#) — has the working parser. Cross-check our Python against `FileFormats/PIZFile.cs` if behavior diverges.

## Usage

```bash
# List entries
py -3.12 re/tools/piz_extract.py list original/TOASTART/Common/AI.piz

# Extract all
py -3.12 re/tools/piz_extract.py extract original/TOASTART/Common/AI.piz -o re/extracted/AI/

# Inspect a single entry
py -3.12 re/tools/piz_extract.py extract original/TOASTART/Common/Frontend.piz -o re/extracted/Frontend/ --filter "*.tga"
```

## When to invoke

- "What's in `Frontend.piz`?" → `list`
- "Extract the AI scripts" → `extract original/TOASTART/Common/AI.piz`
- "I want to swap a vehicle texture" → extract → modify → repack (repack not yet implemented; flag this and offer to add it)
- Asset format questions → read parser source first; never speculate on layout

## Trust hierarchy when format details are ambiguous

1. The actual archive bytes (always run the parser, look at hex)
2. `re/prior_art/MashedFileExtractor/FileFormats/*.cs`
3. The XeNTaX header doc above
4. Generic RenderWare conventions

Never guess offsets. If the parser fails, dump the first 256 bytes of the archive with `Format-Hex` and reason from there.
