# h3 general findings
- zlib crc32 0x00522970: the game DO1 is crc_table[(crc & 0xff) ^ *buf++], not the official ((int)crc ^ *buf++) & 0xff. The game zlib text differs from 1.1.3 in small expression forms: try such rewrites first.
- Tip: libpng internal helpers (png_write_IDAT -> png_write_chunk -> chunk_start/data/end, png_save_uint_32) are inlined in the game: write them inline (0x00520640 100% this way).
- png_crc_error family (0x005232c0, and inlined in png_crc_finish 0x00523170, png_handle_IEND 0x00523cd0, every png_handle_*): the original hoists the png_ptr->flags (+0x6c) load above the chunk_name[0] & 0x20 branch; official text gives ~88-91%. Not cracked (flags local, ternary, combined conditions tried). Evidence it is (a): where the original hoists, both arms start with the identical 'mov eax,[esi+0x6c]' and our build emits exactly that in both arms without hoisting; where the arms use different registers (edx/eax) the original does not hoist either. So it is a post-register-allocation common-instruction hoist our c2 never does. The rest of those functions matches with png_crc_finish/png_crc_read/png_crc_error as static __forceinline copies (D:/MashedWT/MATCH-S23_data/scratch/h3/pngcrc.h).
- Recurring residual (0x00520ef0, 0x00520c70, 0x00523360): the original reuses dead argument slots for locals/spills more than our compile (e.g. png_decompress_chunk: text in the png_ptr slot, ret in comp_type, 4-byte frame vs our 8). Declaration order/scope, struct typedefs, inverted ifs did not move it.
- png_handle_* chunk handlers: the original compiler inlined png_crc_finish unevenly per call site (some copies call png_crc_error 0x005232c0, some inline it but call png_get_uint_32 0x00523110, some fully inline). Mirror it per call site with static __forceinline variants: scratch/h3/pngcrc3.h has png_crc_finish (full), png_crc_finish_i (get_uint_32 called), png_crc_finish_e (crc_error called). gAMA 87.5%, sBIT 87.9% this way.
- zlib early returns (0x00522b50 inflateInit2_, 0x00521110 deflateInit2_, 0x00521520 deflate): the original keeps small `if (x) return K;` blocks inline at the fall-through, each with its own epilogue copy; ours moves them to the end and shares identical epilogues. else-nesting, result temps, permuter did not move it.
- deflate header: write `(s->w_bits<<12) - 0x7800` (pre-folded); the official `(Z_DEFLATED + ((w_bits-8)<<4)) << 8` gives sub/shl/add.

0x00520640  376  100.0  100.0  cxx:ok  -  png_write_filtered_row, png_write_IDAT/chunk inlined, first pass
0x00520ef0  257  96.9  96.9  cxx:ok  b  png_do_write_swap_alpha, save[1] spilled to row_info arg slot instead of row slot
0x00521410  267  94.0  97.6  cxx:ok  b  deflateReset+lm_init inline, post-call scheduling of head store / memset length
0x00522970  309  100.0  100.0  cxx:ok  -  crc32, DO1 expression form changed
0x00522b50  364  81.8  81.8  cxx:ok  b  inflateInit2_ + inflateEnd/inflateReset forceinlined, early-return epilogues tail-merged in ours
0x005232c0  147  88.2  88.2  cxx:ok  a  png_crc_error, flags load hoisted above the chunk_name branch in the original
0x00523170  324  90.6  90.6  cxx:ok  a  png_crc_finish + crc_read/crc_error forceinlined, same flags hoist as 0x005232c0
0x00523cd0  356  91.3  91.3  cxx:ok  a  png_handle_IEND + png_crc_finish forceinlined, same flags hoist as 0x005232c0
0x00523640  602  100.0  100.0  cxx:ok  -  png_handle_IHDR + crc helpers forceinlined, first pass
0x00520c70  629  52.3  60.5  cxx:ok  b  png_do_shift, frame 0x34 vs 0x28 (arg-slot reuse for spills) and loop register allocation
0x00523360  722  76.5  77.4  cxx:ok  b  png_decompress_chunk (NO_STDIO), frame packing: original puts text/ret into dead arg slots
0x00521110  753  83.9  84.7  cxx:ok  b  deflateInit2_ + deflateReset/lm_init forceinlined, early-return epilogue placement (as 0x00522b50)
0x00523e40  932  87.5  87.5  cxx:ok  b  png_handle_gAMA, crc_finish variants forceinlined; length-slot vs png_ptr-slot reuse + one flags hoist
0x005241f0  949  87.9  87.9  cxx:ok  b  png_handle_sBIT, crc_finish variants forceinlined; length-slot vs png_ptr-slot reuse + one flags hoist (a)
0x00522ce0  1005  66.6  77.1  cxx:ok  b  inflate, switch-head state reload / BAD-tail merging differ
0x005238c0  1034  87.1  87.1  cxx:ok  a  png_handle_PLTE, only the flags-load hoist in the inlined crc_error (rest is branch displacement)
0x00521520  1121  84.5  84.5  cxx:ok  b  deflate + putShortMSB/flush_pending forceinlined, header pre-folded; small return blocks placed inline in the original
0x005245b0  2481  85.5  86.0  cxx:ok  b  png_handle_cHRM, crc_finish per site (_e / called); frame 0x28 vs 0x20 (original keeps two ints in dead arg slots)
0x0051fa50  3041  83.9  86.0  cxx:ok  b  png_write_find_filter (weighted), preprocessed official text + padded png_struct; loop register allocation
