# h7 general finding (round 2): MASK-COMPARE OPERAND ORDER. `(x & 7) != 4` on a 32-bit x gives `and cl, 7; cmp cl, 4`;
# the original's `and ecx, 7; cmp cl, 4` comes from `4 != (x & 7)` (constant on the left). 0x00517940 99.4 -> 100.
# Worth trying wherever ours narrows an `and` to a byte register that the original keeps 32-bit.
# h7 shrink-wrap family: not cracked. Our c2 9178 sinks pushes to the dominator of first use (nested, one sink point
# per early-out); the original sinks less. No flag (/G3-/G7, /Ox, /Og.. explicit, /Oy-, /Ob2, /Gy, /Op, /Os) and no
# source form tried (arg copies, nested ifs, goto, do/while(0), typed int/unsigned/pointer tests, shared return block
# via goto) moves it. Only lever seen: an early-out that jumps into a block with real code shared with the body
# (0x00517200 positive-if form) blocks the sink, which does not apply to void functions.
0x00517940  1112  100.0  100.0  cxx:ok  -  png_read_row: cases 2/4 written `4 != (row_number & 7)` (and ecx vs and cl)
0x0051cbe0  922  98.8  98.8  cxx:ok  a  png_write_IHDR: strategy as store-1-then-test (no setne, xor ebx,ebx back); if/else layout left
0x00520ef0  257  100.0  100.0  cxx:ok  -  png_do_write_swap_alpha: row copied from row_arg (save[1] into the row slot)
0x00516d50  69  83.3  83.3  cxx:ok  a  png_calculate_crc: flags hoist; flags local / anc local / comma / struct / per-arm locals / mask local: none
0x00521410  267  94.0  97.6  cxx:ok  b  deflateReset: xor eax,eax scheduled before the hash_size reload; 11 CLEAR_HASH spellings, arg copy: none
0x0051cfc0  303  76.7  95.1  cxx:ok  b  png_write_PLTE: loop preheader palette lea + crc-tail byte order; save32 forceinline, inline chunk_data, struct form: none
0x0052df40  45  88.9  88.9  cxx:ok  b  pixel memcpy: src kept in edx in the original; hdr+1 / char* / inline helpers / locals / arg copies: none
0x0051cb40  46  82.6  87.0  cxx:ok  a  png_write_chunk_data: shrink-wrap; 17 source forms (types, nesting, goto, do/while, arg copies) + 13 flag sets: none
0x00517da0  83  97.1  97.1  cxx:ok  b  png_read_image: pass spill stored before push edi; loop forms, arg copies: none
0x0052aa40  190  89.3  89.3  cxx:ok  a  inflate_blocks_new: `!= 0) {} else return s;` stops the ebx shrink-wrap (82.1->89.3); that return then moves to the shared tail
0x00516de0  946  98.4  98.7  cxx:ok  a  png_free_data: shrink-wrap; empty-then / else-return / nested / goto forms, arg copies: none
0x0051bb40  555  98.1  98.1  cxx:ok  a  png_do_expand_palette: shrink-wrap; arg-copy subsets (3) no gain
0x00528810  912  97.8  98.1  cxx:ok  a  png_read_finish_row: shrink-wrap; else-return, flag local, operand swap: none
0x0051f4f0  793  97.6  98.4  cxx:ok  a  png_write_finish_row: shrink-wrap; else-return form, arg copies: none
0x00519820  418  96.3  96.3  cxx:ok  a  png_do_unshift: shrink-wrap (all 4 pushes sunk below the palette test); arg copies: none
0x0051b750  975  94.8  97.2  cxx:ok  a  png_do_gamma: shrink-wrap; arg-copy subsets (3) no gain
0x0052c4a0  393  84.1  87.8  cxx:ok  a  inflate_trees_dynamic: `!= 0) {} else return -4;` stops the shrink-wrap but moves that return to the end (81.1), kept official
# h7 shrink-wrap lever (value-returning functions only): `if ((p = alloc()) != 0) {} else return X;` makes our c2 push all
# callee-saved regs in the prologue (0x0052aa40 82.1 -> 89.3), but it also moves that return block to the function end.
# In 9178 "exit inline at the fallthrough" and "shrink-wrapped below it" go together; the original has inline exits with
# no shrink-wrap. Void early returns (empty exit block) did not respond to any form.
