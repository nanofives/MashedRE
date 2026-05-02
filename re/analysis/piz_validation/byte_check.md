# Byte-Level Verification — Three Sample Files

Three files selected from inventory.md:
- **Smallest**: `Common\LED.piz` (118,784 bytes)
- **Median** (index 17/35 sorted by size): `TRACKS\City.piz` (3,147,776 bytes)
- **Largest**: `VEHICLES\Shorty.piz` (6,350,848 bytes)

Each section: (a) first-256-byte hexdump, (b) file header field identification, (c) first-entry record hexdump, (d) field identification.

Field layout used for comparison:

**File header (per MagicNumbers.cs + piz_extract.py):**

| offset | len | SciLor field | ours field | raw value (all 3 samples) |
|---|---|---|---|---|
| 0x00–0x02 | 3 | FILE_SIGNATURE "PIZ" | part of MAGIC | 50 49 5A |
| 0x03 | 1 | appendix[0] = 0x00 | part of MAGIC | 00 |
| 0x04–0x07 | 4 | appendix[1-4] = 03 00 00 00 | version=3 | 03 00 00 00 |
| 0x08 | 1 | OFFSET_FILE_COUNT (uint8) | low byte of count | varies |
| 0x09–0x0B | 3 | unnamed (gap) | count upper bytes | 00 00 00 |
| 0x0C | 1 | unnamed (gap) | — | 00 (LED/Shorty) or CC (City) |
| 0x0D–0x10 | 4 | OFFSET_SECOND_FILE_SIGNATURE_APPENDIX | not parsed | 00*4 (LED/Shorty) or CC CC CC 00 (City) |
| 0x11–0x7FF | — | pad (all 0x00) | pad | all 0x00 |

**Per-entry record (all samples; entry+0x00 = HEADER_SIZE + i*128):**

| offset-in-entry | len | SciLor field | ours field |
|---|---|---|---|
| +0x00–+0x72 | 115 | fileName (loop 0..0x72) | name slice [0:116] split on null |
| +0x73 | 1 | not read | included in slice (always 0x00) |
| +0x74–+0x77 | 4 | fileOffset uint32 LE | file_off uint32 LE |
| +0x78–+0x7A | 3 | fileSize bytes 0-2 LE | part of length uint32 LE |
| +0x7B | 1 | fileSize byte 3 (bug: reads 0x77 instead) | part of length uint32 LE |
| +0x7C–+0x7F | 4 | fileUnknown uint32 LE | file_id uint32 LE |

---

## Sample 1: Common\LED.piz (smallest — 118,784 bytes)

### (a) First 256 bytes hexdump

```
0000  50 49 5A 00 03 00 00 00 13 00 00 00 00 00 00 00  PIZ.............
0010  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0020  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0030  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0040  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0050  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0060  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0070  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0080  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0090  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00A0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00B0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00C0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00D0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00E0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00F0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
```

### (b) File header field identification

| offset | raw bytes | SciLor interpretation | our interpretation | agree? |
|---|---|---|---|---|
| 0x00–0x02 | 50 49 5A | FILE_SIGNATURE "PIZ" | part of MAGIC | ✓ |
| 0x03 | 00 | appendix[0]=0x00 | part of MAGIC | ✓ |
| 0x04–0x07 | 03 00 00 00 | appendix[1-4] | version=3 | ✓ |
| 0x08 | 13 | fileCount=19 | count low byte=19 | ✓ count=19 |
| 0x09–0x0B | 00 00 00 | unnamed gap | count upper bytes | ✓ both get 19 |
| 0x0C | 00 | unnamed gap | — | ✓ |
| 0x0D–0x10 | 00 00 00 00 | second appendix TYPE_4x00 | not parsed | ✓ |

**piz_extract.py:** `magic=b'PIZ\x00' version=3 count=19`  
**SciLor:** `sig3=b'PIZ' appendix=0003000000 filecount_byte=19 second_appendix=00000000`

All fields agree. ✓

### (c) First entry record (0x800–0x87F) hexdump

```
0800  4C 45 39 2E 4C 45 44 00 00 00 00 00 00 00 00 00  LE9.LED.........
0810  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0820  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0830  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0840  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0850  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0860  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0870  00 00 00 00 00 18 00 00 0C 12 00 00 62 05 00 00  ............b...
```

### (d) Per-entry field identification

| entry-offset | raw bytes | SciLor | ours | agree? |
|---|---|---|---|---|
| +0x00–+0x06 | 4C 45 39 2E 4C 45 44 | fileName="LE9.LED" | name="LE9.LED" | ✓ |
| +0x07 | 00 | null terminator | null in split | ✓ |
| +0x73 | 00 | not read | included in slice | ✓ byte=0x00, no difference |
| +0x74–+0x77 | 00 18 00 00 | fileOffset=0x1800 | file_off=0x1800 | ✓ |
| +0x78–+0x7B | 0C 12 00 00 | fileSize bytes 0-2=0x120c, byte3=header[0x77]=0x00 | length=0x120c | ✓ (bug inert: header[0x77]=0x00) |
| +0x7C–+0x7F | 62 05 00 00 | fileUnknown=0x562 | file_id=0x562 | ✓ |

All fields agree for LED.piz first entry. ✓

---

## Sample 2: TRACKS\City.piz (median — 3,147,776 bytes)

### (a) First 256 bytes hexdump

```
0000  50 49 5A 00 03 00 00 00 3F 00 00 00 CC CC CC CC  PIZ.....?.......
0010  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0020  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0030  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0040  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0050  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0060  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0070  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0080  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0090  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00A0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00B0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00C0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00D0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00E0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00F0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
```

### (b) File header field identification

| offset | raw bytes | SciLor interpretation | our interpretation | agree? |
|---|---|---|---|---|
| 0x00–0x02 | 50 49 5A | FILE_SIGNATURE "PIZ" | part of MAGIC | ✓ |
| 0x03 | 00 | appendix[0]=0x00 | part of MAGIC | ✓ |
| 0x04–0x07 | 03 00 00 00 | appendix[1-4] | version=3 | ✓ |
| 0x08 | 3F | fileCount=63 | count low byte=63 | ✓ count=63 |
| 0x09–0x0B | 00 00 00 | unnamed gap | count upper bytes | ✓ both get 63 |
| 0x0C | CC | unnamed gap | — | ✗ SciLor sees this as unnamed gap; raw shows CC fill starting here, not at 0x0D |
| 0x0D–0x0F | CC CC CC | second appendix TYPE_4xCC (SciLor reads 0x0D-0x10) | not parsed | ✗ SciLor window 0x0D-0x10 = CC CC CC 00; actual CC fill is 0x0C-0x0F |
| 0x10 | 00 | last byte of SciLor's second-appendix window | — | — |

**piz_extract.py:** `magic=b'PIZ\x00' version=3 count=63`  
**SciLor:** `sig3=b'PIZ' appendix=0003000000 filecount_byte=63 second_appendix=cccccc00`

**KEY DISCREPANCY:** SciLor's `OFFSET_SECOND_FILE_SIGNATURE_APPENDIX = 0x0D` is off by one. The raw 4-byte CC fill occupies bytes **0x0C–0x0F**, not 0x0D–0x10. SciLor's 4-byte window (0x0D–0x10) reads `CC CC CC 00`, missing the first CC byte and including a trailing 0x00. SciLor's validation bug (always checks only index 0) masks this mismatch. [U-0127]

### (c) First entry record (0x800–0x87F) hexdump

```
0800  43 4F 55 52 53 45 2E 4C 55 41 00 00 00 00 00 00  COURSE.LUA......
0810  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0820  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0830  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0840  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0850  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0860  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0870  00 00 00 00 00 28 00 00 97 11 00 00 8D 11 00 00  .....(..........
```

### (d) Per-entry field identification

| entry-offset | raw bytes | SciLor | ours | agree? |
|---|---|---|---|---|
| +0x00–+0x09 | 43 4F 55 52 53 45 2E 4C 55 41 | fileName="COURSE.LUA" | name="COURSE.LUA" | ✓ |
| +0x0A | 00 | null terminator | null in split | ✓ |
| +0x73 | 00 | not read | included in slice | ✓ byte=0x00, no difference |
| +0x74–+0x77 | 00 28 00 00 | fileOffset=0x2800 | file_off=0x2800 | ✓ |
| +0x78–+0x7B | 97 11 00 00 | fileSize=0x1197 (bug inert: header[0x77]=0x00) | length=0x1197 | ✓ |
| +0x7C–+0x7F | 8D 11 00 00 | fileUnknown=0x118d | file_id=0x118d | ✓ |

All entry fields agree. ✓

---

## Sample 3: VEHICLES\Shorty.piz (largest — 6,350,848 bytes)

### (a) First 256 bytes hexdump

```
0000  50 49 5A 00 03 00 00 00 11 00 00 00 00 00 00 00  PIZ.............
0010  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0020  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0030  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0040  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0050  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0060  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0070  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0080  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0090  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00A0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00B0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00C0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00D0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00E0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
00F0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
```

### (b) File header field identification

| offset | raw bytes | SciLor interpretation | our interpretation | agree? |
|---|---|---|---|---|
| 0x00–0x02 | 50 49 5A | FILE_SIGNATURE "PIZ" | part of MAGIC | ✓ |
| 0x03 | 00 | appendix[0]=0x00 | part of MAGIC | ✓ |
| 0x04–0x07 | 03 00 00 00 | appendix[1-4] | version=3 | ✓ |
| 0x08 | 11 | fileCount=17 | count low byte=17 | ✓ count=17 |
| 0x09–0x0B | 00 00 00 | unnamed gap | count upper bytes | ✓ both get 17 |
| 0x0C | 00 | unnamed gap | — | ✓ |
| 0x0D–0x10 | 00 00 00 00 | second appendix TYPE_4x00 | not parsed | ✓ |

**piz_extract.py:** `magic=b'PIZ\x00' version=3 count=17`  
**SciLor:** `sig3=b'PIZ' appendix=0003000000 filecount_byte=17 second_appendix=00000000`

All fields agree. ✓

### (c) First entry record (0x800–0x87F) hexdump

```
0800  47 48 4F 53 54 2E 44 46 46 00 00 00 00 00 00 00  GHOST.DFF.......
0810  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0820  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0830  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0840  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0850  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0860  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
0870  00 00 00 00 00 18 00 00 D2 F6 02 00 A7 0D 00 00  ................
```

### (d) Per-entry field identification

| entry-offset | raw bytes | SciLor | ours | agree? |
|---|---|---|---|---|
| +0x00–+0x08 | 47 48 4F 53 54 2E 44 46 46 | fileName="GHOST.DFF" | name="GHOST.DFF" | ✓ |
| +0x09 | 00 | null terminator | null in split | ✓ |
| +0x73 | 00 | not read | included in slice | ✓ byte=0x00, no difference |
| +0x74–+0x77 | 00 18 00 00 | fileOffset=0x1800 | file_off=0x1800 | ✓ |
| +0x78–+0x7B | D2 F6 02 00 | fileSize: bytes 0-2=0xD2 0xF6 0x02, byte3=header[0x77]=0x00 → 0x0002F6D2 | length=0x2f6d2 | ✓ (bug inert: header[0x77]=0x00) |
| +0x7C–+0x7F | A7 0D 00 00 | fileUnknown=0xda7 | file_id=0xda7 | ✓ |

Note on Shorty GHOST.DFF fileSize: raw bytes at +0x78–0x7B = `D2 F6 02 00`. LE uint32 = 0x0002F6D2 = 194,258. SciLor reads byte at +0x7B = 0x00 (correct field location — bug is inert when 0x7B = 0x00 = same as 0x77 for this entry). Our parser reads 0x0002F6D2 correctly. ✓

All entry fields agree. ✓
