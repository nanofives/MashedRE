# h2 notes (MATCH-S23)
GENERAL (pngwutil chunk writers): the original has png_write_chunk / _start / _data / _end and png_save_uint_32 inline.
Write each inlined helper as its own block `{ unsigned char buf[4]; unsigned int length = ...; ... }` (separate block
per helper, not one shared buf) -- the original packs those buffers into dead arg slots. When a local (e.g. new_name in
iCCP) should sit in the png_ptr arg slot but the compiler gives it a new local, rename the param `png_ptr_arg` and start
the body with `int png_ptr = png_ptr_arg;`: that frees the arg slot (0x0051d400 82.2 -> 100).
MOST IMPORTANT: the arg-copy trick works for ANY param (png_ptr, sbit, num_hist, row_info+row, back, tran, text_len...):
`void f(T x_arg) { T x = x_arg; ...}` frees x's arg slot for stack packing. 12 of my 17 100% files needed 1-2 copies.
Sweep all subsets mechanically: D:/MashedWT/MATCH-S23_data/scratch/h2/argcopy.py <file.c> [maxk] (uses the
`/*PREAMBLE*/` placeholder form; writes best_<va>.c). Inlined chunk_data: try `length` declared before `data`.
Some writers call png_save_uint_32 0x0051ca60 instead of inlining it (bKGD palette branch, tRNS, zTXt's inlined tEXt).
strlen in this TU is repne scasb (class a), float->int in libpng (gAMA) is an inline fnstcw/fldcw/fistp sequence 9176 never emits (class a).

0x0051ca10  70  100.0  100.0  cxx:ok  -  libpng png_set_read_fn (PNG_NO_STDIO), first pass
0x0051ca90  174  100.0  100.0  cxx:ok  -  png_write_chunk with chunk_start/data/end + save_uint_32 inline; one buf[] reused for crc
0x0051cb40  46  82.6  87.0  cxx:ok  b  png_write_chunk_data; original pushes esi in prologue, ours shrink-wraps it
0x0051cfc0  303  76.7  95.1  cxx:ok  b  png_write_PLTE; num_pal arg copy fixes slots, loop preheader regs differ
0x0051d0f0  186  100.0  100.0  cxx:ok  -  png_write_IDAT (libpair said gAMA), ca90 form + mode|=4
0x0051d1b0  133  100.0  100.0  cxx:ok  -  png_write_IEND (libpair said write_chunk), block-scoped buf per inlined helper
0x0051d240  247  58.4  58.4  cxx:ok  a  png_write_gAMA; orig inline fnstcw/fldcw/fistp float->int, 9176 calls __ftol
0x0051d340  182  100.0  100.0  cxx:ok  -  png_write_sRGB, IEND form
0x0051d400  348  100.0  100.0  cxx:ok  -  png_write_iCCP; png_ptr local copy frees arg slot for new_name
0x0051d800  243  100.0  100.0  cxx:ok  -  png_write_compressed_data_out; length before data, png_ptr arg copy
0x0051db30  366  100.0  100.0  cxx:ok  -  png_write_sBIT; sbit arg copy frees slot for buf
0x0051e6f0  246  100.0  100.0  cxx:ok  -  png_write_hIST; num_hist arg copy
0x0051e990  306  68.3  74.0  cxx:ok  a  png_write_tEXt; strlen = repne scasb in orig, 9176 byte loop
0x0051ed60  258  100.0  100.0  cxx:ok  -  png_write_oFFs, block-scoped helper buffers
0x0051f240  258  100.0  100.0  cxx:ok  -  png_write_pHYs, oFFs form
0x0051f0e0  339  73.9  76.9  cxx:ok  a  png_write_sCAL_s (not paired); strlen = repne scasb in orig
0x0051e7f0  406  54.4  78.8  cxx:ok  a  png_check_keyword; repne scasb strlen moves the register allocation
0x0051d900  546  100.0  100.0  cxx:ok  -  png_write_sPLT; png_ptr + spalette arg copies, length before data
0x0051f810  575  100.0  100.0  cxx:ok  -  png_do_write_interlace; row_info + row arg copies
0x0051e4a0  588  100.0  100.0  cxx:ok  -  png_write_bKGD (not paired); back arg copy, palette branch calls png_save_uint_32 0x0051ca60
0x0051ee70  610  67.4  81.1  cxx:ok  a  png_write_pCAL; purpose arg copy, repne scasb strlen x2
0x0051e230  611  100.0  100.0  cxx:ok  -  png_write_tRNS (not paired); png_ptr + tran arg copies, save_uint_32 calls in 2 branches
0x0051ead0  641  63.8  72  cxx:ok  a  png_write_zTXt (not paired); tEXt inlined, repne scasb strlen x2, frame homes
0x0051d560  670  100.0  100.0  cxx:ok  -  png_text_compress; text_len arg copy, COMPRESSION_LAST=3
0x0051f4f0  793  97.6  98.4  cxx:ok  b  png_write_finish_row; IDAT x2 inline; only ebx/edi push placement (shrink-wrap) differs
0x0051cbe0  922  95.6  96.4  cxx:ok  b  png_write_IHDR; strategy store setne vs branch on a cached-0 ebx
0x0051dca0  1411  40.0  40.0  cxx:ok  a  png_write_cHRM (not paired); 8 inline fistp float->int (gAMA issue)
0x0051c220  1990  25.1  35.0  cxx:FAIL  a  png_build_gamma_table; raw g2s form kept, 7 inline fistp float->int, >1000 B
