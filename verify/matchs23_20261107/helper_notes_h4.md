# h4 notes (MATCH-S23, pngrutil.c chunk handlers)

General findings (apply to every png_handle_* in 0x00524f70..0x00527e00):
- The original has png_crc_read / png_crc_finish / png_crc_error / png_get_uint_32 / png_check_chunk_name INLINED
  (compiler auto-inline; a static helper under /O2 /Ob2 is NOT inlined by our DDK c2). Write them inline by hand.
  Per call site the original mixes forms: some copies call png_crc_error 0x005232c0 or png_get_uint_32 0x00523110,
  others inline them. Read each copy in odis and mirror it.
- png_chunk_error = 0x005172b0, png_chunk_warning = 0x00517380 (call operands are masked: a swap scores 100% but
  relocheck fails). png_error 0x00517250, png_warning 0x00517270, png_crc_finish 0x00523170, png_crc_error 0x005232c0.
- png_struct offsets: mode +0x68, flags +0x6c, zbuf +0xac, zbuf_size +0xb0, crc +0x110, num_palette +0x118 (u16),
  chunk_name +0x11c, read_user_chunk_fn +0x1cc. info_ptr->valid +8.
- Inlined png_crc_read(png_ptr, zbuf, zbuf_size): load buf/len into block locals ({uint len=..; uchar *buf=..;}),
  len declared FIRST, and call read_data/calculate_crc with them (re-reading the fields per call costs ~15%).
- Inlined png_crc_error: keep need_crc and a separate `crc_err = need_crc ? (crc != png_ptr->crc) : 0;` then
  `if (crc_err)` (gives the setne/mov/test). png_get_uint_32 written as ((b0<<8)+b1)<<8... (same code as <<24 form).
- Locals of inlined bodies (crc_bytes, data_len) go into an inner { } block: then c2 packs them into a dead
  parameter slot like the original (zTXt 89 -> 97.8).
- png_unknown_chunk has the trailing `png_byte location` (size 0x14): frame size depends on it.
- Residuals seen everywhere: (1) the original hoists `mov eax,[esi+0x6c]` above the chunk_name&0x20 branch in some
  inlined png_crc_error copies; our c2 never does (tried flags local, ternary, else-if). (2) which dead param slot
  (png_ptr vs length) the original picks for istop/crc_bytes. (3) strlen intrinsic: original `repne scasb`, our
  c2 13.00.9178 emits the byte loop under every flag set (/O1 /O2 /Ox /Os /G3-/G7): compiler-version evidence.
  (4) png_get_uint_16: original `xor dx,dx; mov dl,al`, ours `movzx dx,al` (same source in every spelling).

0x005278a0  415  75.8  90.1  cxx:ok  a  png_handle_tEXt; strlen = repne scasb in original, byte loop in our c2 (forces the edi/ebp allocation too)
0x00527a40  481  97.8  97.8  cxx:ok  b  png_handle_zTXt; text_ptr field-load order (data_len loaded before the pushes in the original); permuter no gain
0x00527c30  680  99.2  99.2  cxx:ok  b  png_handle_unknown; istop spilled to png_ptr slot, original info_ptr slot; permuter no gain
0x00526630  731  91.9  91.9  cxx:ok  b  png_handle_pHYs; 4 inlined crc_finish copies mirrored; residual = param-slot packing + flags-load hoist; permuter no gain
0x00526910  732  91.9  91.9  cxx:ok  b  png_handle_oFFs; same as pHYs
0x00526310  786  92.0  92.0  cxx:ok  b  png_handle_hIST; flags-load hoist + get_uint_16 xor/mov vs movzx
0x00527560  827  88.3  88.3  cxx:ok  b  png_handle_tIME; param-slot packing (orig length slot, ours png_ptr slot) + hoist
0x00525fb0  850  86.7  88.8  cxx:ok  a  png_handle_bKGD; u16 zero-extension idiom (orig xor dx,dx/mov dl,al + dword/word buffer loads, ours movzx) in the background math; istop slot of the length-check copy
0x00525870  851  91.4  91.4  cxx:ok  b  png_handle_sPLT; residual = flags-load hoist (1 ins, shifts every jump by 1 byte) + crc_bytes/pp-offset in png_ptr slot vs original length slot
0x00525bd0  988  72.6  77.7  cxx:ok  b  png_handle_tRNS; block layout (original places the length==0 check after the num_palette copy, ours right after the PLTE warning; tried if/else orders, flags) + istop frame-slot order + u16 zero-ext idiom
0x00524f70  1098  86.5  86.5  cxx:ok  b  png_handle_sRGB; same family residual: istop/crc_bytes/buf param-slot choice (original even packs buf[1] into png_ptr's slot) + flags-load hoist
0x005270e0  1147  89.5  89.5  cxx:ok  b  png_handle_sCAL; same family: flags-load hoist in the IDAT copy (1 ins, shifts all jumps) + istop slot (orig info_ptr slot, ours frame)
0x005253c0  1188  88.6  88.8  cxx:ok  b  png_handle_iCCP (raw converter failed); same family: hoist + istop/data_length slot (orig info_ptr/length slots, ours png_ptr slot); profile_size written in the shl-8 nested form
0x00526bf0  1255  78.3  78.6  cxx:ok  b  png_handle_pCAL; frame 0x18 vs 0x14: original keeps nparams in png_ptr's dead slot (so every esp offset shifts by 4) + istop/crc_bytes slots + hoist; tried decl order/block scope
relocheck: 0x00527a40 and 0x00527c30 OK (unchecked=0); the other 12 print BAD only at offsets past the first
instruction-count drift (positional comparison against shifted original bytes), not a wrong callee/global.
