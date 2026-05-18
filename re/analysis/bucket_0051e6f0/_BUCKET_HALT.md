---
bucket: 0x0051e6f0..0x0052c49d
session: batch-t-s4
session_date: 2026-05-18
pool_slot: Mashed_pool2  # acquired via ghidra_pool.sh; pre-assignment was Mashed_pool4 but acquire returned pool2
status: HALTED
reason: bucket is statically-linked third-party library (libpng + zlib), not Mashed code
---

# Bucket 0x0051e6f0 — HALT report (batch-t-s4)

## Decision

**Halted per STOP-AND-ASK rule** — the bucket-range is third-party library
residue (libpng + zlib), analogous to the documented CRT-residue halt:

> "Bucket-range turns out to be CRT residue (FidDB-attested Library tags) -> halt,
>  report. batch_t sessions should NOT promote CRT - separate library-tag drain."

libpng/zlib are the same class of issue: statically-linked, well-known third-party
libraries that don't belong on the C0->C1 brute-force ladder for **Mashed** game
code. They should be batch-tagged via FidDb / Library bookmark (Ghidra's
function-id matcher or a dedicated library-tag drain), not hand-plated.

## Evidence — function identifications (8 spot-checks across full range)

All evidence is bare decompiled strings from the program; no guessing.

| RVA | Identification | Diagnostic strings / signatures |
|-----|----------------|---------------------------------|
| 0x0051e6f0 | libpng `png_write_hIST` | writes "hIST" magic at DAT_005e3254 (raw bytes `68 49 53 54`); calls FUN_00516bb0 = `png_write_data` which errors with "Call to NULL write function" (0x006213e8); also writes "iCCP" magic. CONCAT11 byte-swap pattern = big-endian PNG chunk format. |
| 0x0051e7f0 | libpng `png_check_keyword` | strings: "invalid character in keyword" (0x00621b60), "trailing spaces removed from keyword" (0x00621b38), "leading spaces removed from keyword" (0x00621b14), "extra interior spaces removed" (0x00621ae8), "keyword length must be 1-79 characters" (0x00621aa8 — note the 0x4f=79 cap), "Zero length keyword" (0x00621ad4). |
| 0x0051fa50 | libpng `png_write_find_filter` | Implements all five PNG filter types (None / Sub / Up / Average / Paeth) with the textbook Paeth predictor `abs(p-a), abs(p-b), abs(p-a-b+c)` selection. Tests against bVar1 mask 0x08/0x10/0x20/0x40/0x80 (`PNG_FILTER_NONE..PAETH`). |
| 0x005207c0 | libpng `png_create_struct_2` | Allocates 500 bytes for param=1 (`PNG_STRUCT_INFO`), 0x120 bytes for param=2 (`PNG_STRUCT_PNG`); zero-fills via 4-byte loop; falls back to CRT `_malloc` when user-supplied `malloc_fn` is NULL. |
| 0x00520850 | libpng `png_destroy_struct` | thin wrapper over CRT `_free`. |
| 0x00520930 | libpng `png_free` | calls user `free_fn` at offset 0x1f0, else CRT `_free`. |
| 0x00523110 | libpng `png_get_uint_32` | 4-byte big-endian byte assembly: `((b[0]<<24) | (b[1]<<16) | (b[2]<<8) | b[3])`. |
| 0x00523140 | libpng `png_crc_read` | calls `png_calculate_crc` (0x0051c9f0) then `png_read_data` (0x00516d50) — exact libpng pair. |
| 0x00525870 | libpng `png_handle_sPLT` | strings "Missing IHDR before sPLT" (0x006222e4), "Invalid sPLT after IDAT" (0x006222cc), "sPLT chunk has bad length" (0x00622298), "malformed sPLT chunk" (0x006222b4). |
| 0x00528e00 | zlib `zcalloc` | thin wrapper over CRT `_calloc(items, size)`. |
| 0x00528e20 | zlib `zcfree` | thin wrapper over CRT `_free`. |
| 0x00529050 | zlib `_tr_align` (`bi_buf` flusher) | classic deflate bit-buffer at state offsets 0x16b0 (`bi_buf`), 0x16b4 (`bi_valid`), 0x14 (`pending`); emits the 3-bit + 7-bit static-block sentinel. |
| 0x0052ab00 | zlib `inflate` | the giant state machine (state cases 0..9); strings "invalid stored block lengths" (0x00622954), "invalid block type" (0x00622974), "too many length or distance symbols" (0x00622930), "invalid bit length repeat" (0x00622914). |
| 0x0052bff0 | zlib `inflate_table` | Huffman table builder; returns the canonical `Z_DATA_ERROR=-3` (0xfffffffd), `Z_MEM_ERROR=-4` (0xfffffffc), `Z_BUF_ERROR=-5` (0xfffffffb); local stack arrays count[16] + offs[16] + ENOUGH=0x5a0=1440. |

## Whole-program-string confirmation

Bare strings present in the binary (from `search_defined_strings`):

- 0x00620ed4 "Incompatible libpng version in application and library"
- 0x006215a8 "Width too large for libpng to process image data."
- 0x00620f0c "zlib error"
- 0x00621474 "Unknown zlib error"
- 0x00621488 "zlib version error"
- 0x0062149c "zlib memory error"

## Bucket boundaries (observed)

- **0x0051e6f0 .. ~0x00528200** — libpng (chunk writers, encoders, validators,
  filters, palette/sPLT/hIST/iCCP/IDAT helpers, png_create/destroy/free).
- **~0x00528200 .. 0x0052c49d** — zlib (zcalloc/zcfree at 0x00528e00..0x00528e2b,
  deflate bit-buffer at 0x00529050, inflate state machine at 0x0052ab00,
  inflate_table at 0x0052bff0).

Boundary is approximate; the libpng/zlib split is at ~0x00528200 where libpng's
deflate-glue functions hand off to zlib internals. A dedicated library-tag drain
should pin the exact split.

## Why brute-force C0->C1 plates are wrong for this bucket

1. **Wasted effort** — these are not Mashed functions; reimplementing them
   re-derives well-known reference code.
2. **No greenfield value** — `mashed_re.exe` (greenfield target) will link
   libpng + zlib normally as deps, not hand-port them.
3. **Tracker pollution** — 80 rows in hooks.csv for "library code" muddies the
   real game-code RVA inventory.
4. **Wrong methodology** — libraries are identified by FidDb signatures and
   bookmarked as `Library`/`External`, then excluded from discovery sweeps.

## Recommended action (for the orchestrator / sweep)

1. Tag the range 0x0051e6f0..0x0052c49d in `hooks.csv` (or a new
   `library_residue.csv`) with `status=library`, `library=libpng/zlib`.
2. Add to `DEFERRED.md` with re-pickup condition: "if a libpng/zlib function is
   on a hot path or a hook target, plate it individually then; otherwise leave
   library-tagged."
3. Apply Ghidra's FidDb (Function ID database) to bulk-name libpng + zlib
   functions across the binary — this likely covers the rest of any
   library-residue ranges too.
4. Do **not** re-issue this bucket in a future batch_X sweep.

## What this session did NOT produce

- No per-RVA 80x C1 plates (would be wrong).
- No mutations to hooks.csv / STUBS.md / UNCERTAINTIES.md / DEFERRED.md (those
  belong to the sweep, per parallel-safety rule).
- No git commits.

## Queue-row note

The queue row appended to re/SCRIBE_QUEUE.md flags the halt; the sweep will read
this bucket's `_BUCKET_HALT.md` rather than expecting 80 plate files.
