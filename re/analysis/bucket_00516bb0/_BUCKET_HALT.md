---
bucket: 0x00516bb0..0x0052df40
session: batch-v-s4
session_date: 2026-05-18
pool_slot: Mashed_pool3  # acquired via ghidra_pool.sh; pre-assignment was Mashed_pool4 but acquire returned pool3
status: HALTED
reason: bucket is statically-linked third-party library (libpng + zlib + image-format loader/registry, e.g. DevIL or equivalent), not Mashed code
adjacent_bucket: bucket_0051e6f0 (already-halted libpng+zlib island, batch-t-s4)
---

# Bucket 0x00516bb0 — HALT report (batch-v-s4)

## Decision

**Halted per STOP-AND-ASK rule** (library-residue clause). The bucket-range is
third-party library residue spanning **three libraries**:

1. **libpng**         (0x00516bb0..0x0051e6eb) — extends the already-halted
                       libpng region of bucket_0051e6f0 *backwards* into the
                       low-frequency entry points (png_set_*, png_do_*).
2. **zlib inflate**   (0x0052c4a0..~0x0052d52a) — picks up immediately after
                       the already-halted zlib region of bucket_0051e6f0
                       (continuation of `inflate_table` machinery).
3. **image-format loader / registry** (0x0052cb00..0x0052df40) — BMP loader
                       + DevIL-style format-handler registry indexed at
                       DAT_00912160; calls into a sibling format-handler ABI
                       at +0x108/+0x10c/+0x18 vtable offsets.

This is the **same class of issue** as the documented CRT-residue and the
libpng+zlib halt in batch-t-s4: statically-linked, well-known third-party
libraries that don't belong on the C0->C1 brute-force ladder for **Mashed**
game code. They should be batch-tagged via FidDb / Library bookmark, not
hand-plated.

The boundary between batch-v-s4's bucket and batch-t-s4's bucket is **literal
adjacency**: candidate #66 (0x0051e4a0) ends at body_end 0x0051e6eb, and
batch-t-s4's bucket starts at 0x0051e6f0 (5-byte alignment gap). Candidate
#67 (0x0052c4a0) starts 3 bytes after batch-t-s4's bucket end at 0x0052c49d.
The two buckets together form ONE contiguous third-party-library island
spanning 0x00516bb0..~0x0052df40 (~94 KB of library code).

## Evidence — function identifications (12 spot-checks across full range)

All evidence is bare decompiled strings/signatures from the program; no
guessing.

| RVA | Identification | Diagnostic strings / signatures |
|-----|----------------|---------------------------------|
| 0x00516bb0 | libpng `png_write_data` | calls error with bare string `s_Call_to_NULL_write_function_006213e8` ("Call to NULL write function"); single-callback-dispatch at offset +0x4c. The same NULL-error string is referenced by batch-t-s4's halt as the libpng identifier. |
| 0x00516bd0 | libpng `png_flush_data` | mirrors 0x00516bb0; single-callback-dispatch at offset +0x14c (flush callback). |
| 0x00516bf0 | libpng `png_set_write_fn` | writes 3 callback pointers (param2 -> +0x54, param3 -> +0x4c, param4 -> +0x14c) and errors via `png_warning` with bare strings `s_Attempted_to_set_both_read_data__0062143c` ("Attempted to set both read_data...") and `s_the_same_structure__Resetting_re_00621404` ("the same structure. Resetting re..."). Textbook libpng read/write-callback mutual-exclusion check. |
| 0x00516c40 | libpng `png_sig_cmp` | declares the literal PNG signature on the stack: bytes `89 50 4e 47 0d 0a 1a 0a` (`\x89PNG\r\n\x1a\n`) at local_8[0..7]. Param2=start, param3=num — exact libpng signature-compare prototype `int png_sig_cmp(png_bytep sig, png_size_t start, png_size_t num_to_check)`. |
| 0x00516cc0 | libpng `png_zalloc` (or `png_calloc`) | zlib-style allocator: `calloc`-like `param2 * param3` size; uses fast 0x8000-byte initial zero-fill loop (0x2000 dwords = 0x8000 bytes) then byte-loop tail — classic libpng `png_calloc` body. |
| 0x00516d20 | libpng `png_free_default` (thunk to png_free at 0x00520930) | thunk_FUN_00520930 — direct alias of the libpng `png_free` already identified in batch-t-s4 at 0x00520930. |
| 0x00516d30 | libpng `png_reset_crc` | wraps zlib `_crc32(0, 0, 0)` at FUN_00522970 (== zlib `crc32`); stores into +0x110 (= libpng `png_struct.crc`). |
| 0x00516d50 | libpng `png_calculate_crc` | conditional crc32 update guarded by flag bits 0x800 / 0x300 / 0x20 against state-byte +0x11c (libpng `transformations.flags`); calls FUN_00522970 (zlib crc32). |
| 0x00516da0 | libpng `png_create_info_struct` | allocates 0x48 dwords (= 288 bytes = sizeof(png_info_struct)) via `png_malloc_default` (FUN_005207c0, identified as `png_create_struct_2` in batch-t-s4); zero-fills via 0x48-iter loop. |
| 0x00516de0 | libpng `png_free_data` (or `png_destroy_info_struct`) | the giant info-struct teardown: tests bit flags on `info_ptr->valid` (+0xb8) for 0x4000=PALETTE, 0x2000=tRNS, 0x100=hIST, 0x80=ICCP, 0x10=tEXt, 0x200=sPLT, 8=PCAL, 0x1000=iCCP, 0x40=unknown chunks, 0x20=splt; for each, frees via FUN_00520930 (= libpng `png_free` from batch-t-s4). Recursive self-call (param_4 = -1 -> per-item iteration). |
| 0x005185a0 | libpng `png_do_background` | per-pixel alpha compositing + gamma correction with `_pow()` calls (`png_pow_table` build pattern); reads palette/trans index at +0x114/+0x118; switches on bit-depth byte at +0x127 (cases 1/2/4/8/16); handles bVar1==3 = palette mode. The `(0xff - alpha) * dst * fg / 0xffff` integer composite is textbook libpng `png_do_background`. |
| 0x00519270 | libpng `png_do_read_transformations` (or `png_do_write_transformations`) | giant chain of conditional sub-calls keyed off `transformations` bitmask at +0x70: 0x1000=PALETTE_TO_RGB (calls 0x0051bd70/0x0051bb40), 0x40000=invert, 0x600000=rgb_to_gray, 0x4000=swap_alpha, 0x80=expand, 0x4000=expand_tRNS, 0x600080=background, 0x2000=gamma, 0x400=strip_16, 0x40=dither (with error str `s_png_do_dither_returned_rowbytes__006215fc`), 0x20=swap, 8=shift, 4=swap_bytes, 1=swap, 0x10000=invert_mono, 0x800=quantize, 0x80000=premultiply, 0x20000=add_alpha, 0x10=user_transform — exactly the libpng transformation flag layout. |
| 0x0051a6c0 | libpng `png_do_expand_palette` (with tRNS) | 12-parameter dispatch over `info->color_type` byte (`bVar4`) cases 0/2/4/6 (PALETTE/RGB/GA/RGBA) with sub-dispatch on bit_depth 1/2/4/8/16; massive shift-and-mask palette unpack with optional gamma LUT and alpha-compositing — libpng expand-palette path. |
| 0x0051c220 | libpng `png_build_gamma_table` | allocates 256-byte LUT (or 4*(1<<(8-shift)) for 16-bit); fills via `_pow(x * 1/255, gamma_value)` then `* 0xff + 0.5` round — textbook libpng gamma-table builder. References DAT_005e3388 (= 255.0f), DAT_005e3390 (= 1/255.0f), DAT_005e33a0 (= 1/65535.0f), DAT_005ce1d8 (= 0.5f). |
| 0x0051e230 | libpng `png_write_tRNS` | error strings `s_Invalid_number_of_transparent_co_00621a10` ("Invalid number of transparent colors..."), `s_Can_t_write_tRNS_with_an_alpha_c_006219e8` ("Can't write tRNS with an alpha c..."); writes chunk magic at DAT_005e32b4 (raw bytes `74 52 4e 53` = "tRNS"). Calls png_write_data (0x00516bb0) + png_reset_crc (0x00516d30) + png_calculate_crc (0x00516d50) — exact libpng chunk-write idiom. |
| 0x0052c4a0 | zlib `inflate_fast` setup / huffman setup | error strings `s_oversubscribed_distance_tree_00623b98`, `s_incomplete_distance_tree_00623b7c`, `s_empty_distance_tree_with_lengths_00623b58`, `s_oversubscribed_literal_length_tr_00623b34`, `s_incomplete_literal_length_tree_00623b14` — all verbatim zlib `inflate_table` / `inflate` error messages. Calls FUN_0052bff0 (= zlib `inflate_table` identified in batch-t-s4 halt). Returns -3 (Z_DATA_ERROR) / -4 (Z_MEM_ERROR). |
| 0x0052cb00 | image-library BMP loader | reads 4-byte file header then tests `(local_43c[0] & 0xffff) == 0x4d42` (=="BM" little-endian); reads BITMAPINFOHEADER (0xe bytes after); branches on `iVar3 == 0xc` (BITMAPCOREHEADER) vs `iVar3 == 0x28` (BITMAPINFOHEADER); handles bit_depth cases 1/4/8 with palette + 24/32-bit; RLE-decode loop with `(local_44c >> 8) | (local_44c & 0xff)` repeat-byte / literal-runs (= BMP RLE4/RLE8); calls FUN_0052d170 to write per-scanline. Calls into low-level I/O via FUN_004cc230, FUN_004cbd30, FUN_004cc160 (file open/read/close in a separate library — likely the same image-library's I/O abstraction). |
| 0x0052daf0 | image-library format-handler registry | iterates global array at DAT_00912160 (stride 0x30, count at DAT_007dbcd8); each entry has type-id at +0, size-multiplier at +0x18, header-size at +0x2c, callback ptr at +0x20 (called with `(*local_8[8])(param_1, puVar2)`); allocates `size_mult * width + 0x18 + header_size` via callback at DAT_007d3ff8+0x108 (= image-library `imalloc`-style). This is a **format-handler dispatch table** — pattern matches DevIL's `IL_image` / `iCurImage` registry. |

## Whole-program-string confirmation (subset already attested by batch-t-s4 halt)

These strings, all referenced by functions in **this** bucket, were already
identified by batch-t-s4 as libpng/zlib markers:

- 0x006213e8 "Call to NULL write function"            (libpng — 0x00516bb0)
- 0x0062143c "Attempted to set both read_data..."     (libpng — 0x00516bf0)
- 0x00621404 "the same structure. Resetting re..."    (libpng — 0x00516bf0)
- 0x006215fc "png_do_dither returned rowbytes..."     (libpng — 0x00519270)
- 0x00621648 "NULL row buffer"                        (libpng — 0x00519270)
- 0x00621a10 "Invalid number of transparent colors..." (libpng — 0x0051e230)
- 0x006219e8 "Can't write tRNS with an alpha c..."    (libpng — 0x0051e230)
- 0x00623b98 "oversubscribed distance tree"           (zlib   — 0x0052c4a0)
- 0x00623b7c "incomplete distance tree"               (zlib   — 0x0052c4a0)
- 0x00623b58 "empty distance tree with lengths"       (zlib   — 0x0052c4a0)
- 0x00623b34 "oversubscribed literal/length tree"     (zlib   — 0x0052c4a0)
- 0x00623b14 "incomplete literal/length tree"         (zlib   — 0x0052c4a0)

## Bucket boundaries (observed)

- **0x00516bb0 .. 0x0051e6eb** — libpng (low-level I/O dispatch, callback
  registration, signature compare, info struct lifecycle, gamma + alpha
  composite transforms, palette expand, transformations chain, tRNS writer).
  **Adjacent** to and continuous with batch-t-s4's libpng region starting at
  0x0051e6f0.
- **0x0052c4a0 .. ~0x0052d52a** — zlib (inflate_fast / huffman-tree-setup
  continuation; calls into the zlib `inflate_table` at 0x0052bff0 already
  identified by batch-t-s4).
- **~0x0052cb00 .. 0x0052df40** — image-library (BMP loader at 0x0052cb00;
  format-handler registry at 0x0052daf0; supporting allocators/helpers).
  Pattern matches DevIL or a similar image-loading library. Calls into a
  separate I/O abstraction at FUN_004cc230 / FUN_004cbd30 / FUN_004cc160
  (file open/read/close, likely sitting in another adjacent library bucket).

Combined with batch-t-s4's bucket, the contiguous third-party-library island
in MASHED.exe spans **0x00516bb0..~0x0052df40 (~94 KB)**.

## Why brute-force C0->C1 plates are wrong for this bucket

(Same reasoning as batch-t-s4's halt — repeated here for completeness.)

1. **Wasted effort** — these are not Mashed functions; reimplementing them
   re-derives well-known reference code.
2. **No greenfield value** — `mashed_re.exe` (greenfield target) will link
   libpng + zlib + an image library normally as deps, not hand-port them.
3. **Tracker pollution** — 80 rows in hooks.csv for "library code" muddies
   the real game-code RVA inventory.
4. **Wrong methodology** — libraries are identified by FidDb signatures and
   bookmarked as `Library` / `External`, then excluded from discovery
   sweeps.

## Recommended action (for the orchestrator / sweep)

1. **Merge with batch-t-s4's halt** — treat 0x00516bb0..~0x0052df40 as a
   single library-residue range in `hooks.csv` (or `library_residue.csv`)
   with `status=library`, `library=libpng+zlib+image-library`. The boundary
   batch-t-s4 reported (0x0051e6f0..0x0052c49d) was a slice of this larger
   island; this halt extends both ends.
2. Add to `DEFERRED.md` with re-pickup condition: "if a libpng/zlib/image-
   library function is on a hot path or a hook target, plate it
   individually then; otherwise leave library-tagged."
3. Apply Ghidra's FidDb (Function ID database) to bulk-name libpng + zlib +
   the image-library functions across the binary — should cover the third
   library too (DevIL or whatever it is has published FidDb signatures).
4. Investigate the image-library identity — the format-handler registry at
   DAT_00912160 with the BMP loader at 0x0052cb00 looks like **DevIL**
   (`iCurImage` / `ILimage` / format-handler array) but the function-pointer
   ABI (+0x108 alloc, +0x10c free, +0x20 callback) needs cross-checking
   against DevIL's public source before claiming. A dedicated `re/tools/`
   FidDb pass should resolve this.
5. Do **not** re-issue this bucket in a future batch_X sweep.

## What this session did NOT produce

- **No per-RVA 80x C1 plates** (would be wrong; library residue).
- No mutations to hooks.csv / STUBS.md / UNCERTAINTIES.md / DEFERRED.md
  (those belong to the sweep, per parallel-safety rule).
- No git commits.

## Queue-row note

The queue row appended to re/SCRIBE_QUEUE.md flags the halt; the sweep will
read this bucket's `_BUCKET_HALT.md` rather than expecting 80 plate files.
