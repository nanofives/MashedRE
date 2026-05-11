# gamesave.bin — Save Buffer Layout (first-pass)

**Session:** save_gamesave_d3-20260511  
**File:** `original/gamesave.bin` (shipped with game; loaded into / saved from `DAT_00803358`)  
**File size:** 0x24FA0 bytes (151,456)  

All offsets are from the start of the file (= start of the save buffer at 0x00803358).  
First-write RVA = instruction that first writes each field into the buffer.

---

## Layout

| File offset | Size (bytes) | Value (fresh file) | First-write RVA | Notes |
|-------------|-------------|--------------------|-----------------|-------|
| 0x0000 | 4 | `0xDEADBEEF` | 0x00404F37 | Magic sentinel — `MOV DWORD PTR [0x803358], 0xDEADBEEF` |
| 0x0004 | 0x2443C (150,076) | all-zero (no saves) | 0x00404F34 | Profile data block — REP MOVSD from `*DAT_008A94A8` (0x928F dwords) |
| 0x24440 | 0xB60 (2,912) | see tail notes | [UNCERTAIN U-3559] | Tail — NOT written by Save::SerializeToBuffer; shipped with non-zero defaults |

**Total:** 0x24440 + 0xB60 = 0x24FA0 ✓

---

## Tail data (0x24440–0x24F9F) — observed values in shipped file

First non-zero byte at file offset 0x24A44 (0x604 bytes past tail start).

### Sparse non-zero offsets (0x24A44–0x24C9F): likely championship progression flags

| File offset | DWORD value | Observation |
|-------------|-------------|-------------|
| 0x24A44 | 0x00000001 | — |
| 0x24A4C | 0x00000002 | — |
| 0x24A50 | 0x00000002 | — |
| 0x24A6C | 0x00000001 | — |
| 0x24A7C | 0x00000002 | — |
| 0x24A80 | 0x00000002 | sparse pattern continues; spaced 0x08–0x30 bytes |

### Dense region (0x24CB0–0x24DE7): track ID sequences

Values are 4-byte little-endian integers, stride 4 bytes:
```
0x24CB0: 09 13 0C 07 FF FF FF FF 09 13 0C 07 0B 09 12 0C
0x24CC0: 07 0B 09 11 10 13 07 09 11 13 0C 0B 09 11 0C 07
0x24CD0: 0B 09 12 0C 07 0B 09 11 12 0C 0B 09 09 09 09 09
...continuing through 0x24DE7 (all value 0x09 = 9 for last dozen entries)
```
Max track ID seen: 0x13 = 19. Range 0x07–0x13 consistent with 20 tracks (0-indexed or 1-indexed).  
0xFFFFFFFF = -1 appears at 0x24CC0 and 0x24D38 — likely sentinel/unset entry.

### Float array (0x24E1C–0x24E4F, 13 floats)

All entries = `0x3F7D70A4` = ~0.9898 (IEEE 754 little-endian).  
13 entries × 4 bytes = 52 bytes. Count 13 matches 13-track championship table.

### Byte flag run (0x24E50–0x24EEF, 160 bytes)

All 0x01. Likely a 160-element byte array of unlock/completion flags.

### Float array (0x24F00–0x24F0B, 3 floats)

All entries = `0x3F333333` = ~0.7 (IEEE 754).

### Trailing non-zero (0x24F10, 0x24F57–0x24F5B)

```
0x24F10: 0x02
0x24F57-0x24F5B: 0x01 0x01 0x01 0x01 0x01
```

---

## Key globals

| Address | Role |
|---------|------|
| 0x00803358 | Save buffer base (= file offset 0) |
| 0x0080335C | Save buffer + 4 (profile data region start) |
| 0x008A94A8 | Profile data pointer (DWORD → 150,076-byte block) [UNCERTAIN U-3560] |
| 0x007F0A40 | Championship track table (13×48B = 624 bytes) |
| 0x00827D98 | Championship snapshot area (outside save buffer) |
| 0x007F0F54 | Stride-pack staging (12 bytes) [UNCERTAIN U-3558] |
| 0x007F105C | Struct array first field (stride 0x4C, 12 entries) [UNCERTAIN U-3558] |
| 0x00828254 | Save state counter mirror |
| 0x008A95AC | Live save state counter |

---

## Serialize / Deserialize functions

| RVA | Name | Direction |
|-----|------|-----------|
| 0x00404EE0 | Save::SerializeToBuffer | game state → save_buf |
| 0x00404E80 | Save::DeserializeFromBuffer | save_buf → game state |
| 0x00404F50 | SAVE_WRITE_FN (sub_00404F50) | save_buf → gamesave.bin |
| 0x00404E50 | SAVE_LOAD_FN (sub_00404E50) | gamesave.bin → save_buf |

---

## Uncertainties

- **U-3558**: 12 bytes at stride 0x4C from 0x7F105C — which struct fields?
- **U-3559**: Who writes save_buf tail [0x24440..0x24F9F]? Not Save::SerializeToBuffer.
- **U-3560**: DAT_008A94A8 points to what struct? 150,076-byte profile block layout TBD.
