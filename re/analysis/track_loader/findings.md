# Track Loader — Session Findings

Session: track_loader-20260502-1943  
Slot: Mashed_pool2  
Date: 2026-05-02

---

## TRACK_LOAD_FN Identified

**FUN_00426e10** (0x00426e10) is the primary track loading function.

Anchor: `"toastart/tracks/"` at 0x005cd4c0 → 3 cross-references.
- FUN_00426340 (KTCScript loader) — references anchor string
- FUN_004264d0 (powerup loader) — references anchor string  
- **FUN_00426e10** — references anchor string; confirmed TRACK_LOAD_FN by:
  - Debug string `"Opening piz file [%s]\n"` at call to FUN_004987b0
  - Extracts COURSE.LUA and LAPDATA.LUA from track piz
  - Loads AI data (FUN_004235b0), line exceptions (FUN_00409710)
  - Terminates with `DAT_0066d704 = 1` (track-loaded flag)

Track path construction: `"toastart/tracks/" + trackname + ".piz"`  
(vtable at DAT_007d3ff8+0xcc for string-assign, +0xd4 for string-append; suffix ".piz" at 0x005cd280 confirmed by memory_read)

---

## Track Record Table

Track records are 0x48-byte structs stored starting at `s_training_005f33f8` (0x005f33f8), count in DAT_005f37a0 (0x005f37a0).

Known field layout:
- +0x00: name string (Ghidra detected "training" at base = first entry is "training" track)
- +0x10: int ID field (matched by FUN_0041e980 / FUN_0041e870)
- +0x14: function pointer (dispatched via FUN_0041e8b0; read via FUN_0041e9d0)
- +0x44: function pointer (dispatched via FUN_0041e970; read via FUN_0041ea90)

Global DAT_0063d7e4 (0x0063d7e4) holds the current track record pointer.

---

## Hardcoded Dev Paths Found

- `"d:\\ToastArt\\common\\led.piz"` — in FUN_00409710 (line exception loader)
- `"d:\\ToastArt\\Common\\ai.piz"` — in FUN_004235b0 (AI data loader)

These are retail builds that still contain the artist/dev path prefix. Both use the same piz-read API:
`FUN_004cc230(2, 1, output_ptr)` → handle  
`FUN_004cc5e0(handle, chunk_id, &size, flags)` → data_ptr  
`FUN_004cbd30(handle, dest_buf, size)` — reads chunk  
`FUN_004cc160(handle, 0)` — closes  
`FUN_004952f0()` — closes piz  

Chunk IDs: LED=0x13269901, AI=0x13269902 (sequential).

---

## In-Piz Path Builder

FUN_004260e0 builds internal piz paths like:
`"ToastArt[sep]tracks[sep]<trackname>[sep]<filename>[sep]<ext>"`  
param_1=0 → sep="/"; param_1!=0 → sep="\\" and prepends DAT_005cd4ac prefix.  
Always returns &DAT_008991e0 (global path buffer).

---

## Cross-Bucket Calls

TRACK_LOAD_FN → calls FUN_00495280 (the piz-open function, DEFERRED D-1180).  
FUN_00495280 also appears in piz_validation domain (session G). Cross-bucket: YES.

---

## Subsystem Init Sequence (from FUN_00426e10 call order)

1. FUN_0041e980 — get track record by ID
2. Build track path string → copy to DAT_008991e0
3. FUN_00426cd0 (DEFERRED)
4. FUN_0041e870 — set global current track record
5. FUN_0041ea90 + FUN_0041e970 — dispatch optional init callback (+0x44)
6. FUN_004987b0 — log "Opening piz file [%s]"
7. FUN_00495280 — open track .piz (DEFERRED)
8. FUN_004260e0 + FUN_0042a530 — extract COURSE.LUA
9. FUN_0047a020, FUN_00479330 — process COURSE.LUA (DEFERRED)
10. FUN_0042a530 + FUN_0047a0f0, FUN_00478660 — LAPDATA.LUA (DEFERRED)
11. FUN_00480340, FUN_004715a0, FUN_00412050, FUN_004952f0 — misc inits (DEFERRED)
12. FUN_00409680 + FUN_00409710 — clear/load line exceptions
13. FUN_00423630 + FUN_004235b0 — clear/load AI data
14. FUN_00425ab0 — clear 16 globals (0x008992a0 range)
15. FUN_004262f0 — process 64-byte records from DAT_00646c38
16. FUN_00462950 (DEFERRED)
17. FUN_0047c0b0, optionally FUN_0047c0f0 (DEFERRED)
18. Conditional FUN_004671a0 / FUN_0042f510 / FUN_004924c0 block (DEFERRED)
19. FUN_0041e9d0 + FUN_0041e8b0 — dispatch optional callback (+0x14)
20. FUN_00491780 (DEFERRED)
21. DAT_0066d704 = 1 (track loaded flag)
22. FUN_004671a0 + FUN_004053d0 — set globals DAT_00639d70/DAT_00639d78

---

## IDs Used
- U: U-0428, U-0429, U-0430 (3 used)
- D: D-1180 (1 used)
- S: S-0420..S-0431 (12 used)
