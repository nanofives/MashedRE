# PIZFile.cs vs piz_extract.py — Spec Diff

Cross-reference of every structural claim. Three sources:
- **PIZFile.cs** = `re/prior_art/MashedFileExtractor/FileFormats/PIZ/PIZFile.cs` + companion files
- **ours** = `re/tools/piz_extract.py`
- **raw** = byte-level verification in `byte_check.md`

Companion CS files consulted:
- `FileFormats/PIZ/MagicNumbers.cs` — all numeric constants
- `FileFormats/PIZ/FileSignature.cs` — file-header parsing
- `FileFormats/PIZ/FileHeader.cs` — per-entry parsing

---

## 1. Global file header — magic bytes

**PIZFile.cs claims** (MagicNumbers.cs:8, FileSignature.cs:40-45):  
`FILE_SIGNATURE = { 0x50, 0x49, 0x5A }` — 3 bytes "PIZ".  
Immediately followed by `FILE_SIGNATURE_APPENDIX = { 0x00, 0x03, 0x00, 0x00, 0x00 }` (5 bytes) at offsets 3–7.  
The two together form a fixed 8-byte prefix at file offset 0x00.

**ours claims** (piz_extract.py:34, 38-43):  
`MAGIC = b"PIZ\x00"` — 4 bytes at offset 0x00.  
Followed by `version` = uint32 LE at offset 0x04 (line 43).  
Version is expected to equal 3 (implicit; the docstring calls it "version 3 in all known files").

**Agreement / disagreement:**  
Both describe the same physical bytes `50 49 5A 00 03 00 00 00`. The split point differs:  
- SciLor boundary: magic=3 bytes, appendix byte[0]=0x00 at offset 3, appendix bytes[1-4]=0x03 0x00 0x00 0x00 at offsets 4–7.  
- Ours boundary: magic=4 bytes (includes the 0x00), version=0x03000000 LE=3 at offsets 4–7.  
Both parse the same fixed sequence; the naming difference has no operational consequence.  
Raw bytes confirm: `50 49 5A 00 03 00 00 00` in all three samples. ✓

---

## 2. File-level entry count

**PIZFile.cs claims** (MagicNumbers.cs:13, FileSignature.cs:56-58):  
`OFFSET_FILE_COUNT = 0x08`. Field type: `byte fileCount` (C# `byte` = uint8, 1 byte).  
Read: `fileCount = signature[0x08]`.

**ours claims** (piz_extract.py:43):  
`version, count = struct.unpack_from("<II", buf, 4)` — reads two uint32 LE values starting at offset 4. The second uint32 covers bytes 0x08–0x0B, so `count` is a 4-byte LE integer.

**Agreement / disagreement:**  
Conceptually different field widths (1 byte vs 4 bytes). Both produce the same numeric value because bytes 0x09–0x0B = `00 00 00` in all three samples verified. For any file with entry count ≤ 255, the values are identical.  
Raw bytes confirm: all three samples have `XX 00 00 00` at offsets 0x08–0x0B. ✓ (no practical disagreement)

---

## 3. Second header appendix (fill pattern)

**PIZFile.cs claims** (MagicNumbers.cs:14, FileSignature.cs:59-75):  
`OFFSET_SECOND_FILE_SIGNATURE_APPENDIX = 0x0D`.  
`FILE_SIGNATURE_APPENDIX2_LENGTH = 4` bytes.  
So the second appendix occupies bytes 0x0D–0x10. Two valid values: all-0x00 (`TYPE_4x00`) or all-0xCC (`TYPE_4xCC`).  
The validation code (`FileSignature.cs:64`) has a copy-paste bug — it checks `signature[pos + 0]` four times (never checking pos+1, pos+2, pos+3), so the four-byte uniformity check is not actually enforced.

**ours claims**: piz_extract.py does not parse the second appendix at all. No validation or use of bytes 0x0C–0x10.

**Agreement / disagreement:**  
SciLor claims the fill starts at 0x0D. Raw bytes for City.piz (TYPE_4xCC) show `CC CC CC CC` at bytes **0x0C–0x0F**, with byte 0x10 = 0x00. The actual 4-byte CC fill starts at **0x0C**, one byte earlier than SciLor documents.  
✗ SciLor vs raw bytes — OFFSET_SECOND_FILE_SIGNATURE_APPENDIX = 0x0D reads the CC block at 0x0D-0x10 as `CC CC CC 00`, not `CC CC CC CC`. SciLor's broken validation check (always tests index 0) masks this by only verifying the first byte.  
See U-0127.

---

## 4. Entry table start offset

**PIZFile.cs claims** (MagicNumbers.cs:15):  
`OFFSET_FILE_HEADER = 0x800` (2048 decimal).

**ours claims** (piz_extract.py:31):  
`HEADER_SIZE = 2048`.  
Entry table begins at `data[HEADER_SIZE + i * ENTRY_SIZE]` for i in range(count) (lines 71, 84).

**Agreement:** ✓ Both: entry table at byte offset 0x800.  
Raw bytes confirm: first entry for LED.piz starts at 0x800 with `4C 45 39 2E 4C 45 44 00` = "LE9.LED\x00". ✓

---

## 5. Per-entry record size

**PIZFile.cs claims** (MagicNumbers.cs:16):  
`OFFSET_FILE_HEADER_SIZE = 0x80` (128 decimal).

**ours claims** (piz_extract.py:32):  
`ENTRY_SIZE = 128`.

**Agreement:** ✓ Both: 128 bytes per entry.

---

## 6. Per-entry name field

**PIZFile.cs claims** (MagicNumbers.cs:18, FileHeader.cs:22-29):  
`OFFSET_FILE_HEADER_FILE_NAME_MAX_SIZE = 0x73` (115 decimal).  
Name loop: `for (int i = 0; i < 0x73; i++)` — reads indices 0..114, stops at first 0x00 byte. Maximum 115 bytes consumed.

**ours claims** (piz_extract.py:48-49):  
`name_raw = buf[off : off + 116]` — reads 116 bytes (indices 0..115).  
`name = name_raw.split(b"\x00", 1)[0].decode("ascii", errors="replace")` — splits at first null.

**Agreement / disagreement:**  
SciLor consumes 115 bytes; ours slices 116. Byte at entry+0x73 (index 115 in our slice) is NOT included in SciLor's name scan.  
Raw bytes confirm: byte at entry+0x73 = `0x00` in all three first-entry records verified. Both parsers produce identical name strings for these samples. ✓  
If a name ever fills all 115 bytes with no null, ours would include byte 0x73 in the name (incorrect per SciLor). That case requires further verification. See U-0128.

---

## 7. Per-entry file offset field

**PIZFile.cs claims** (MagicNumbers.cs:19, FileHeader.cs:33-38):  
`OFFSET_FILE_HEADER_FILE_OFFSET = 0x74`.  
4-byte signed int, assembled via explicit byte shifts (little-endian).

**ours claims** (piz_extract.py:50-51):  
`file_off, length, file_id = struct.unpack_from("<III", buf, off + 0x74)` — 3× uint32 LE starting at entry+0x74.

**Agreement:** ✓ Both: 4-byte LE integer at entry offset 0x74.

---

## 8. Per-entry file size field

**PIZFile.cs claims** (MagicNumbers.cs:20, FileHeader.cs:41-47):  
`OFFSET_FILE_HEADER_FILE_SIZE = 0x78`.  
4-byte int assembled via byte shifts. **Bug on FileHeader.cs:45**: the most-significant byte uses  
`header[MagicNumbers.OFFSET_FILE_HEADER_FILE_OFFSET + 3]` = `header[0x77]`  
instead of  
`header[MagicNumbers.OFFSET_FILE_HEADER_FILE_SIZE + 3]` = `header[0x7B]`.  
This reads the MSB of the *offset* field (byte 0x77) instead of the MSB of the *size* field (byte 0x7B).

**ours claims** (piz_extract.py:50-51):  
`struct.unpack_from("<III", buf, off + 0x74)` correctly reads 12 bytes as three uint32s: offset (0x74–0x77), length (0x78–0x7B), id (0x7C–0x7F).

**Agreement / disagreement:**  
✗ SciLor vs our parser — SciLor has a bug in the size MSB.  
Raw bytes confirm: for all three verified entries, `header[0x77]` = 0x00 and `header[0x7B]` = 0x00 (all offsets and sizes fit in 3 bytes for these files). Both produce the same size values despite the bug. No practical impact on these files.

---

## 9. Per-entry unknown/id field

**PIZFile.cs claims** (MagicNumbers.cs:21, FileHeader.cs:49-54):  
`OFFSET_FILE_HEADER_FILE_UNKNOWN = 0x7C`. 4-byte signed int, little-endian. Named `fileUnknown`.

**ours claims** (piz_extract.py:50-51):  
Third uint32 from `unpack_from("<III", buf, off + 0x74)` at offset 0x7C. Named `file_id`.

**Agreement:** ✓ Both: 4-byte LE integer at entry offset 0x7C. Semantic name differs (SciLor: "unknown"; ours: "id") — purpose of this field remains unconfirmed. See U-0129.

---

## 10. Compression / encryption

**PIZFile.cs claims**: No compression or encryption present. `extractFile()` (PIZFile.cs:48-55) copies raw bytes directly from the archive at `fileHeader.fileOffset` for `fileHeader.fileSize` bytes.

**ours claims** (piz_extract.py:95):  
`blob = data[real_off : real_off + length]` — direct slice, no decompression.

**Agreement:** ✓ Both: no compression or encryption. File data is stored raw.

---

## 11. Section-end / footer

**PIZFile.cs claims**: None documented. Parser reads header, then entry table, then individual file blobs on demand. No footer parsing.

**ours claims**: No footer parsing. File data appended after entry table; each blob padded to 2048-byte boundaries (piz_extract.py docstring line 18: "File data: each blob padded to 2048-byte boundary.").

**Agreement:** ✓ No footer.  
The 2048-byte padding claim in our docstring is an inference from observed data (offsets are multiples of 0x800), not derived from PIZFile.cs. See U-0130.

---

## Summary table

| # | Topic | PIZFile.cs | piz_extract.py | Status |
|---|---|---|---|---|
| 1 | Magic bytes | 3 bytes "PIZ" + 5-byte appendix | 4 bytes "PIZ\x00" + version uint32 | ✓ same bytes, different split |
| 2 | Entry count | uint8 at 0x08 | uint32 LE at 0x08 | ✓ same value for count ≤ 255 |
| 3 | Second appendix offset | 0x0D (MagicNumbers.cs:14) | not parsed | ✗ raw bytes show fill at 0x0C |
| 4 | Entry table start | 0x800 | 0x800 | ✓ |
| 5 | Entry record size | 128 bytes | 128 bytes | ✓ |
| 6 | Name field width | 115 bytes | 116 bytes | ✓ same result (byte 0x73=0x00) |
| 7 | File offset in entry | uint32 LE at +0x74 | uint32 LE at +0x74 | ✓ |
| 8 | File size in entry | uint32 at +0x78 (MSB bug) | uint32 LE at +0x78 | ✗ SciLor MSB reads from 0x77 |
| 9 | Unknown/id in entry | int at +0x7C | uint32 LE at +0x7C | ✓ |
| 10 | Compression | none | none | ✓ |
| 11 | Footer | none | none | ✓ |
