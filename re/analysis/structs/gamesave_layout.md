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
| 0x24440 | 0x600 (1,536) | all zero | (BSS) | Pre-championship gap — no code writes this; all zero in shipped file; in-memory BSS at 0x827798–0x827D97 |
| 0x24A40 | 0x520 (1,312) | see championship block | 0x00404F19 | Championship snapshot — REP MOVSD from `DAT_007F0A40` (0x148 dwords); see sub-layout below |
| 0x24EFC | 4 | 0x00000000 | 0x00404F23 | Save state counter — `MOV [0x00828254], EAX` where EAX = `DAT_008A95AC`; overwrites byte within championship block |
| 0x24F60 | 0xA0 (160) | all zero | (BSS) | Post-championship gap — no code writes this; all zero in shipped file; in-memory BSS at 0x8282B8–0x8282F7 |

**Total:** 0x24440 + 0x600 + 0x520 + 0xA0 = 0x24FA0 ✓  
(Note: 0x24EFC is inside the championship block; the save state counter DWORD at that offset is overwritten by a separate MOV after the championship REP MOVSD.)

---

## Tail (RESOLVED 2026-05-22 workstream E)

**Writer identification:** The tail is written by `Save::SerializeToBuffer` (0x00404EE0) and the
flat-blob I/O pair (`SAVE_WRITE_FN` 0x00404F50 / `SAVE_LOAD_FN` 0x00404E50). There is NO second
independent save function for the tail.

**Proof:** `mcp__ghidra__ghidra_eval` enumerated all Ghidra cross-references whose destination falls
in `0x827798..0x8282F7`; the complete set is:

| From RVA | Dest address | Ref type | Context |
|----------|-------------|----------|---------|
| 0x00404F14 | 0x00827D98 | DATA | `MOV EDI, 0x827D98` — destination for REP MOVSD |
| 0x00404F19 | 0x00827D98 | WRITE | `MOVSD.REP ES:EDI, ESI` — start of championship copy |
| 0x00404F19 | 0x00827D9C | WRITE | Continuation of same REP MOVSD |
| 0x00404E87 | 0x00827D98 | DATA | `MOV ESI, 0x827D98` — source for DeserializeFromBuffer REP MOVSD |
| 0x00404E91 | 0x00827D98 | READ | `MOVSD.REP ES:EDI, ESI` — DeserializeFromBuffer read |
| 0x00404E91 | 0x00827D9C | READ | Continuation of same REP MOVSD |
| 0x00404F23 | 0x00828254 | WRITE | `MOV [0x00828254], EAX` — save state counter |
| 0x00404EBC | 0x00828254 | READ | `MOV EAX, [0x00828254]` — DeserializeFromBuffer read |

All eight references belong to `SerializeToBuffer` (0x00404EE0) and `DeserializeFromBuffer`
(0x00404E80). No other function in the binary references any address in the tail range.

### Pre-championship gap (0x24440–0x24A3F, 0x600 bytes)

- In-memory range: 0x00827798–0x00827D97.
- All zero in the shipped `gamesave.bin`. Ghidra static image: all zero (BSS).
- No code writes this range. `SerializeToBuffer` does not touch it.
- Conclusion: this region is zero-initialized BSS that is serialized to file as-is by the flat
  blob I/O. It may be reserved for a future subsystem or is leftover alignment padding.
  [UNCERTAIN — zero in shipped file does not prove it is always zero at runtime]

### Championship snapshot block (0x24A40–0x24F5F, 0x520 bytes)

Written at RVA 0x00404F19 by `Save::SerializeToBuffer`: `MOVSD.REP` of `0x148` dwords
(328 dwords = 1,312 bytes) FROM `DAT_007F0A40` TO `DAT_00827D98` (= save_buf + 0x24A40).

Read at RVA 0x00404E91 by `Save::DeserializeFromBuffer`: reverse direction, same length.

Source global `DAT_007F0A40` is the live championship table. Code at 0x004368E0, 0x0043A174
and others accesses it as `(&DAT_007F0A40)[col + row * 0xC]` — stride 12 dwords per row.
The table has at least 13 rows (track count) × 12 columns per row = 156 dwords (624 bytes) for
the flag matrix. The full serialized block is 328 dwords which includes additional per-row fields
after the flag matrix; exact sub-field layout is [UNCERTAIN U-3560-ext].

Observed sub-regions in the shipped file (all offsets relative to start of championship block,
i.e. file offset = 0x24A40 + block_offset):

| Block offset | File offset | Size (bytes) | Values in shipped file | Mechanical description |
|-------------|-------------|-------------|----------------------|----------------------|
| 0x0000 | 0x24A40 | 0x270 (624) | sparse 0/1/2 | Championship mode flags: `[col + row*0xC]` matrix, 156 dwords (13 rows × 12 cols). Values: 0=not started, 1=won, 2=completed [UNCERTAIN — semantic labels inferred from context] |
| 0x0270 | 0x24CB0 | 0x138 (312) | track IDs 0x07–0x13, 0xFFFFFFFF=-1 | Track ID sequences — 78 dwords. -1 (0xFFFFFFFF) at block offsets 0x0280 and 0x02F8 are sentinels/unset entries [UNCERTAIN] |
| 0x03A8 | 0x24DE8 | 0x034 (52) | all 0x0000003B (59) | 13 dwords of value 59 [UNCERTAIN — field identity unknown] |
| 0x03DC | 0x24E1C | 0x034 (52) | all `0x3F7D70A4` | 13 floats = ~0.9898f (IEEE 754 single). Count 13 matches track count [UNCERTAIN — semantic: best-time or rating] |
| 0x0410 | 0x24E50 | 0x09C (156) | all 0x01 (byte-packed) | 156 bytes all 0x01; likely a 156-element byte flag array (unlock/completion per championship slot) [UNCERTAIN] |
| 0x04AC | 0x24EEC | 0x010 (16) | all zero | Zero gap within championship block |
| 0x04BC | 0x24EFC | 0x004 (4) | 0x00000000 | **Save state counter** — overwritten at RVA 0x00404F23: `MOV [0x00828254], EAX` where EAX = `DAT_008A95AC` (live save counter). Loaded from `DAT_008A95AC` at RVA 0x00404F03 before the championship REP MOVSD, then stored here after it. |
| 0x04C0 | 0x24F00 | 0x00C (12) | all `0x3F333333` | 3 floats = ~0.7f (IEEE 754 single) [UNCERTAIN — field identity unknown] |
| 0x04CC | 0x24F0C | 0x004 (4) | all zero | Zero gap |
| 0x04D0 | 0x24F10 | 0x004 (4) | 0x00000002 | 1 dword = 2 [UNCERTAIN — field identity unknown] |
| 0x04D4 | 0x24F14 | 0x040 (64) | mostly zero; 0x01010100 at 0x24F54, 0x01010101 at 0x24F58 | Trailing championship block bytes [UNCERTAIN] |

### Save state counter (0x24EFC, 4 bytes)

- In-memory address: 0x00828254 = save_buf + 0x24EFC.
- Offset within championship block: 0x4BC (dword index 303 of 328).
- Write RVA: 0x00404F23 — `MOV [0x00828254], EAX`.
- Source: `DAT_008A95AC` (live save state counter loaded at 0x00404F03).
- Note: the REP MOVSD at 0x00404F19 first copies championship data covering this position from
  `DAT_007F0A40[0x12F]`, then RVA 0x00404F23 immediately OVERWRITES that dword with the live
  counter. So `0x828254` holds the save state counter, NOT the championship table element at
  dword 303.

### Post-championship gap (0x24F60–0x24F9F, 0xA0 bytes)

- In-memory range: 0x008282B8–0x008282F7.
- All zero in the shipped `gamesave.bin`. Ghidra static image: all zero (BSS).
- No code writes this range. Reserved/alignment padding.

### Hypothesis outcome

- **Hypothesis 1 (separate save function):** REFUTED. No second writer exists; `SerializeToBuffer`
  itself writes the championship snapshot into the tail via REP MOVSD at 0x00404F19 and the
  save-counter MOV at 0x00404F23.
- **Hypothesis 2 (second independent record):** REFUTED. The tail is part of the same flat
  0x24FA0-byte blob; the I/O functions at 0x00404E50 / 0x00404F50 treat the whole buffer as one
  record.
- **Hypothesis 3 (load-only / game never touches):** PARTIALLY CONFIRMED for the pre- and
  post-championship gaps (0x600 + 0xA0 bytes). The championship block itself is actively
  written by SerializeToBuffer.

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

## WS-G5 (2026-06-16) — standalone writer

`SerializeToBuffer` (0x00404EE0) re-confirmed by disasm (Mashed_pool6): the
counter MOV at 0x00404F23 (`MOV [0x00828254],EAX`, EAX = DAT_008A95AC loaded at
0x00404F03) runs **after** the championship REP MOVSD (0x00404F19) — the
decompiler hoists it into the gather loop, but the byte order is span-copy →
counter-overwrite → [profile, skipped when DAT_008A94A8==0] → magic. So span+0x4BC
(file 0x24EFC) holds the counter, not the table dword (matches the Tail section).

`SAVE_WRITE_FN` (0x00404F50) = `FUN_004b3bb0("gamesave.bin", &save_buf, 0x24fa0)`
= plain open/write/close (FUN_004cc230 / 004cbe80 / 004cc160). No special framing.

**Standalone port:** `mashedmod/src/mashed_re/Save/GameSaveFormat.h` — header-only,
buffer-based port of the same format (the existing `Save/GameSaveBuffer.cpp` is a
raw-pointer dev-.asi hook on live 0x803358; this is the exe-linkable, testable
twin). `BuildImage`/`ParseImage` produce/consume the real 0x24FA0 envelope (magic
@0, championship span @0x24A40, counter @0x24EFC; profile region zero — the
standalone has no live profile block). `Race/GameFlow.cpp` now persists campaign
progression via this format to a **standalone-copy** `mashed_re_gamesave.bin`
(NEVER original/gamesave.bin), replacing the old MRP1 sidecar; the same image is
fed to `Nav_GameStateLoadSave` so menu unlock/grey-out state tracks the save.
Round-trip verified GREEN: `Save/tests/gamesave_format_test.cpp` (byte layout +
write/read identity + magic gate + per-area unlock/trophy recovery).

---

## Uncertainties

- **U-3558**: 12 bytes at stride 0x4C from 0x7F105C — which struct fields?
- **~~U-3559~~**: RESOLVED 2026-05-22 workstream E — see "Tail (RESOLVED)" section above.
- **U-3560**: DAT_008A94A8 points to what struct? 150,076-byte profile block layout TBD.
- **U-3560-ext** [NEW]: Championship block sub-field layout at offsets 0x0270–0x051F: exact field
  identities for track IDs, 0x3B dwords, 0.9898f floats, byte flags, 0.7f floats, trailing 0x02
  dword unknown. Requires decomp of championship init/reset functions (callers of 0x7F0A40).
