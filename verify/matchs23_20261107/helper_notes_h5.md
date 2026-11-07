# h5 general finding: dead-arg-slot packing needs ARG COPIES FOR EVERY ARG THE BODY READS LATE. png_read_filter_row 0x00528630
# 72.4 -> 100 with png_ptr/row_info/row/prev_row all copied (`T x = x_arg;` at the top); any 3 of the 4 gave 72-73.
# Brute-force the 2^n subsets of copies (cheap: 31 compiles for 5 args) when the original has no frame or a smaller one.
# h5 also: a 4-byte png_memcmp against png_IDAT is the memcmp intrinsic (mov ecx,[p]; mov eax,offset; cmp ecx,[eax]); png_crc_error inlined keeps its int result (setne+test).
0x00528e30  181  100.0  100.0  cxx:ok  -  _tr_init, init_block inline, split chained zero stores
0x00528ef0  346  100.0  100.0  cxx:ok  -  _tr_stored_block, send_bits/copy_block/bi_windup inline, callee args copied to locals
0x0052bf40  163  100.0  100.0  cxx:ok  -  inflate_trees_bits, official source with explicit z_stream layout
0x0052aa40  190  82.1  83.3  cxx:ok  b  inflate_blocks_new + inline reset; original pushes ebx in prologue + per-return epilogues, ours shrink-wraps
0x0052c4a0  393  84.1  87.8  cxx:ok  b  inflate_trees_dynamic official source; original pushes ebp/esi in prologue, ours shrink-wraps (relocheck BAD only from the shift)
0x0052d7e0  255  100.0  100.0  cxx:ok  -  BMP row writer; pad byte stored in the dead ras arg slot
0x0052d980  160  100.0  100.0  cxx:ok  -  image-format registrar; if (count < 16) {...} with the error after
0x0052da20  202  100.0  100.0  cxx:ok  -  image create by format id; __forceinline find helper, field store order rows/b/a
0x0052ddc0  35  100.0  100.0  cxx:ok  -  ring-list count; compare against obj[4] in the loop condition
0x0052df40  45  88.9  88.9  cxx:ok  b  pixel payload memcpy; src arg loaded in the prologue in the original
0x00534920  40  100.0  100.0  cxx:ok  -  RW plugin attach, first form
0x0052de60  189  100.0  100.0  cxx:ok  -  row-converter bind; raw g2s + permuter moves, C++ casts
0x00528630  449  100.0  100.0  cxx:ok  -  png_read_filter_row official + arg copies for png_ptr/row_info/row/prev_row (dead-arg-slot packing)
0x00529050  740  100.0  100.0  cxx:ok  -  _tr_align, official macros with bi_flush inline, static_ltree consts folded
0x00527ee0  785  80.6  84.1  cxx:ok  a  png_combine_row official + png_ptr/row arg copies; only partial shrink-wrap left
0x0052c7a0  849  83.0  83.0  cxx:ok  a  inflate_fast official; block layout (DUMPBITS hoist, literal tail-merge, loop-head duplication)
0x00528810  912  97.8  98.1  cxx:ok  a  png_read_finish_row; crc_error int result, memcmp intrinsic, crc_read arg locals; shrink-wrap left
0x0052d170  955  57.7  73.0  cxx:ok  b  image row converter switch; register / dead-slot assignment differs
0x0052bff0  1227  86.7  87.3  cxx:ok  b  huft_build official + do-while min-length search; null-input test loads c[0]/m into regs in the original (5-byte shift = rest of diff)
0x0052cb00  1633  22.9  43.7  cxx:ok  b  BMP reader one pass; original keeps each close+return-0 exit separate (545 vs 473 ins), bigger frame
0x0052b820  1749  45.5  57.0  cxx:ok  b  inflate_codes official one pass; register/frame-slot assignment (m in frame slot in the original)
0x0052ab00  3084  32.2  50.3  cxx:ok  b  inflate_blocks official one pass + r_arg copy; register/frame assignment from the prologue on
