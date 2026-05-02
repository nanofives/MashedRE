# PIZ Cross-Validation Findings

Session: piz_validation-20260502  
Sources: `re/tools/piz_extract.py`, `re/prior_art/MashedFileExtractor/FileFormats/PIZ/` (PIZFile.cs + companions), raw bytes from 3 sample files.

---

## Confirmed structure (all three sources agree)

1. **Magic prefix**: bytes 0x00–0x07 = `50 49 5A 00 03 00 00 00` in every file. First 3 = "PIZ"; byte 3 = 0x00; bytes 4-7 = `03 00 00 00` (little-endian 3).
2. **Entry count**: 1-byte effective count at offset 0x08. Bytes 0x09–0x0B = `00 00 00` in all 3 samples. Count range observed: 2–86.
3. **Entry table start**: offset 0x800 (2048 bytes) in every file. Bytes 0x11–0x7FF are all 0x00 (padding).
4. **Entry record size**: 128 bytes (0x80).
5. **Name field**: null-terminated ASCII starting at entry+0x00. Byte at entry+0x73 = 0x00 (padding) in all 3 verified first entries. Both parsers produce identical names.
6. **File offset**: uint32 LE at entry+0x74.
7. **File size**: uint32 LE at entry+0x78 (correct interpretation per piz_extract.py and raw bytes).
8. **Unknown/ID field**: uint32 LE at entry+0x7C. Purpose unconfirmed [U-0129].
9. **No compression, no encryption**: file data stored raw; direct byte copy from archive offset.
10. **No footer**: format ends after last file blob.
11. **Version field**: all 35 files have version=3 (per our parser) / appendix[1-4]=03000000 (per SciLor).
12. **All 35 files parsed successfully** by piz_extract.py (exit code 0, "raw" offset mode).
13. **Two second-appendix fill types observed**: TYPE_4x00 (most files) and TYPE_4xCC (at least City.piz; see U-0127 for the offset discrepancy).
14. **Blob alignment**: All observed file offsets in list output are multiples of 0x800 (2048 bytes). Docstring claim of 2048-byte padding matches observation but is not verified against SciLor code directly [U-0130].

---

## Discrepancies

### D1 — Second appendix byte offset: SciLor vs raw bytes

**What disagrees:** SciLor (`MagicNumbers.cs:14`) vs raw bytes.  
**SciLor claims:** `OFFSET_SECOND_FILE_SIGNATURE_APPENDIX = 0x0D`. The 4-byte fill window is bytes 0x0D–0x10.  
**Raw bytes (City.piz):** The 4-byte CC fill occupies bytes **0x0C–0x0F**. Byte 0x0C = 0xCC; byte 0x10 = 0x00. SciLor's window (0x0D–0x10) reads `CC CC CC 00`, not `CC CC CC CC`.  
**Our parser:** Does not parse this field; unaffected.  
**Why SciLor still works:** `FileSignature.cs:64` has a copy-paste bug that checks `signature[pos+0]` four times, never verifying pos+1/pos+2/pos+3. So the check passes on `CC CC CC 00` because it only tests the first byte (0xCC).  
See [U-0127].

### D2 — fileCount field width: SciLor vs our parser

**What disagrees:** SciLor vs our parser (conceptual difference, not value difference).  
**SciLor:** `byte fileCount` — 1-byte uint8 at offset 0x08 (`FileSignature.cs:13, 57`).  
**Ours:** uint32 LE at offset 0x08 (reads bytes 0x08–0x0B, `piz_extract.py:43`).  
**Raw bytes:** Bytes 0x09–0x0B = `00 00 00` in all 3 samples. Both parsers produce the same numeric count.  
**Impact:** None for counts ≤ 255. A file with count > 255 (not observed) would break SciLor's parser but not ours.

### D3 — Name field slice width: SciLor vs our parser

**What disagrees:** SciLor vs our parser (no value difference for verified samples).  
**SciLor:** 115 bytes (loop `i < 0x73`, `FileHeader.cs:22-28`).  
**Ours:** 116 bytes slice, split on null (`piz_extract.py:48`).  
**Raw bytes:** Byte at entry+0x73 = 0x00 in all 3 verified first entries. Both produce identical names.  
**Impact:** If a name fills all 115 bytes with no null, ours would incorrectly include byte 0x73 in the name. That case is unverified; no entry with a 115-char name was found in the 3 samples. See [U-0128].

### D4 — extractFileSize MSB bug: SciLor vs our parser

**What disagrees:** SciLor vs our parser (SciLor code bug).  
**SciLor:** `FileHeader.cs:45` reads `header[OFFSET_FILE_HEADER_FILE_OFFSET + 3]` = `header[0x77]` (MSB of offset field) as the most-significant byte of fileSize, instead of `header[OFFSET_FILE_HEADER_FILE_SIZE + 3]` = `header[0x7B]`.  
**Ours:** `struct.unpack_from("<III", ...)` reads the correct 4 bytes for length.  
**Raw bytes:** `header[0x77]` = 0x00 and `header[0x7B]` = 0x00 for all 3 verified entries. Bug is inert here. Would corrupt fileSize if any entry's offset has a non-zero MSB (offset ≥ 0x01000000 = 16,777,216), which does not occur in these files.

---

## Open questions — UNCERTAINTIES.md rows

| ID | Statement | Source disagreement |
|---|---|---|
| U-0127 | True byte offset of the second-appendix fill field. Raw bytes show 4-byte CC fill at 0x0C–0x0F; SciLor documents it at 0x0D. Only one TYPE_4xCC file verified (City.piz). | SciLor vs raw bytes |
| U-0128 | Whether byte at entry+0x73 is always 0x00 across all entries in all 35 files. Only 3 first-entry records verified. A non-zero byte there would cause our parser's 116-byte name slice to produce a different (incorrect) name vs SciLor's 115-byte limit. | SciLor vs ours (unverified for all entries) |
| U-0129 | Semantic meaning of the uint32 at entry+0x7C (SciLor: "fileUnknown"; ours: "file_id"). Raw bytes confirm the field exists and is populated; purpose is unconfirmed. | semantic |
| U-0130 | Whether all file blobs are padded to 2048-byte boundaries, or only their start offsets are 2048-byte-aligned. Docstring claim confirmed by offset observation but not verified by checking actual data lengths. | structural (ours docstring, not verified vs SciLor or raw) |
