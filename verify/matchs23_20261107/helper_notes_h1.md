# h1 general finding: dead-arg-slot packing (pngrtran row functions). The original packs spilled locals into the
# LATER arg slot (row, [esp+0x18]); the official text gives the row_info slot. Fix = h2's trick on `row`:
# `void f(png_row_infop row_info, png_bytep row_arg) { png_bytep row = row_arg; ...`. 0x005196f0 / 0x005199f0 97 -> 100.
# h1 open residual (class a so far): PARTIAL SHRINK-WRAP. Our 9176 pushes only the regs used before an early-out, branches,
# then pushes the rest (and reorders pops). The original pushes all callee-saved regs first. 0x00516de0 98.4, 0x0051bb40 98.1,
# 0x00519820 96.3, 0x0051b750 94.8. No source form/flag found that stops it; a lever would lift all four to ~100.
# h1: the arg-copy trick generalizes: 0x0051bd70 74.9->100, 0x00519bc0 97.9->100 (row), 0x00519f70 84->98.4 (png_ptr+row),
# 0x0051b750 88.6->94.8 (gamma_table), 0x0051a6c0 74.2->85.9 (5 args; scratch/h1/greedy_args.py does a greedy search over
# which args to copy; combos matter, single copies alone showed no gain there).
# h1 WARNING: match_permute's `move` can break semantics (0x0051a6c0: moved a store above the computation it uses and
# scored +7 by restoring jump displacements). Diff every permuter result against your file before adopting it.
# h1: pngrtran helpers using png_composite / png_composite_16 need the NODIV macros from png.h (else they compile as
# implicit calls); fabs/memcpy/memcmp/memset via extern "C" decl + #pragma intrinsic keeps the C++ check clean.
0x00516bf0  70  100.0  100.0  cxx:ok  -  libpng png_set_read_fn (game variant, 4 args)
0x00516c40  128  79.5  79.5  cxx:ok  a  png_sig_cmp: return-0 exits not tail-merged / else-if block moved in original
0x00516d20  5  100.0  100.0  cxx:ok  -  png_zfree, jmp thunk to png_free 0x00520930
0x00516d50  69  83.3  83.3  cxx:ok  a  png_calculate_crc: original hoists flags load above the branch
0x00516da0  55  100.0  100.0  cxx:ok  -  png_create_info_struct (memset intrinsic)
0x00517200  70  100.0  100.0  cxx:ok  -  png_handle_as_unknown (early-return written as positive if around loop)
0x00517250  31  100.0  100.0  cxx:ok  -  png_error, default_error inlined as longjmp; whole symbol 32 B BYTE-IDENTICAL (--bytes)
0x005172b0  53  100.0  100.0  cxx:ok  -  png_chunk_error (png_error inlined); whole symbol 54 B BYTE-IDENTICAL (--bytes)
0x005172f0  143  100.0  100.0  cxx:ok  -  png_format_buffer, official source
0x00517da0  83  97.1  97.1  cxx:ok  a  png_read_image: pass-counter spill store scheduled one push earlier
0x005196f0  298  100.0  100.0  cxx:ok  -  png_do_unpack, row copied from row_arg (arg slot packing)
0x005199f0  248  100.0  100.0  cxx:ok  -  png_do_read_swap_alpha, row copied from row_arg
0x00519820  418  96.3  96.3  cxx:ok  a  png_do_unshift: prologue pushes vs palette early-out ordering (shrink-wrap)
0x005173f0  437  83.8  83.8  cxx:ok  a  png_create_read_struct_2: first return NULL gets its own epilogue in the original (no cross-jump); rest equal
0x0051bb40  555  98.1  98.1  cxx:ok  a  png_do_expand_palette (row_arg copy): only prologue shrink-wrap below the palette test differs
0x00518140  594  100.0  100.0  cxx:ok  -  png_read_destroy, official order; gamma_shift +0x158 is int
0x00519bc0  618  100.0  100.0  cxx:ok  -  png_do_read_filler, row copied from row_arg
0x0051bd70  912  100.0  100.0  cxx:ok  -  png_do_expand, row copied from row_arg (74.9 without)
0x00516de0  946  98.4  98.7  cxx:ok  a  png_free_data: only partial shrink-wrap of the prologue differs
0x0051b750  975  94.8  97.2  cxx:ok  a  png_do_gamma (gamma_table_arg copy, 88.6 without): only partial shrink-wrap differs
0x00517940  1112  99.4  100.0  cxx:ok  a  png_read_row (png_ptr_arg copy): two 'and ecx,7' vs 'and cl,7' peepholes
0x00519270  1137  100.0  100.0  cxx:ok  -  png_do_read_transformations, png_do_chop written inline
0x00519f70  1858  100.0  100.0  cxx:ok  -  png_do_rgb_to_gray: png_ptr/row arg copies + 16-bit branches inverted (permuter)
0x005185a0  3242  27.3  46.2  cxx:ok  a  png_init_read_transformations: 14 inline fistp/fldcw ftol sites + aligned double frame (9176 calls __ftol)
0x0051a6c0  4228  85.9  86.1  cxx:ok  a  png_do_background (5 arg copies + NODIV composite macros): xor/mov vs movzx byte->word at 8 sites shifts displacements; partial shrink-wrap
