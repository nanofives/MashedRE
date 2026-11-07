# h6 notes (MATCH-S23 round 2, pngrutil crc helpers + chunk handlers)
# h6: flags-load hoist re-tried on png_crc_error 0x005232c0 (14 forms: flags local before/after the chunk test, in a
# block, chunk byte local, ptr-to-flags, short load, ternary, need_crc computed, swapped arms, png_ptr arg copy; flag sets
# /G3../G7, /Ox, /GS, /O1). Every flags-local form gives the same 41% code (chunk byte loaded into cl, png_ptr in edi).
# No form gives `test mem; mov eax,flags; je`. Stays class (a).
# h6 GENERAL: after arg copies (length, or info_ptr+length) 13 png_handle_* files are down to that ONE hoisted
# instruction (plus u16 movzx in hIST/bKGD). A future lever for the hoist would take all of them to ~100.
0x005232c0  147  88.2  88.2  cxx:ok  a  png_crc_error: flags-load hoist; 14 source forms + 9 flag sets tried, none moves it
0x00523170  324  90.6  90.6  cxx:ok  a  png_crc_finish: only the same flags hoist (1 ins, shifts jump displacements)
0x00523cd0  356  91.3  91.3  cxx:ok  a  png_handle_IEND: only the same flags hoist
0x00527c30  680  100.0  100.0  cxx:ok  -  png_handle_unknown: info_ptr arg copy (was 99.2)
0x00527a40  481  97.8  97.8  cxx:ok  b  png_handle_zTXt: data_len load scheduled after the pushes; arg copies (8 subsets), 24 text_ptr store orders, block/temp forms, permuter 219 compiles: no gain
0x00526310  786  92.0  92.0  cxx:ok  a  png_handle_hIST: flags hoist + get_uint_16 xor/mov vs movzx; arg copies no gain
0x00526910  732  93.5  93.5  cxx:ok  a  png_handle_oFFs: length arg copy (was 91.9); only the flags hoist left
0x00526630  731  93.5  93.5  cxx:ok  a  png_handle_pHYs: length arg copy (was 91.9); only the flags hoist left
0x00525870  851  93.0  93.0  cxx:ok  a  png_handle_sPLT: length arg copy (was 91.4); only the flags hoist left
0x005270e0  1147  90.5  90.5  cxx:ok  a  png_handle_sCAL: info_ptr arg copy (was 89.5); only the flags hoist left
0x005253c0  1188  90.7  90.7  cxx:ok  a  png_handle_iCCP: info_ptr+length copies + official <<24 profile_size via block-local p (was 88.6); only the flags hoist left
0x00527560  827  90.5  90.5  cxx:ok  a  png_handle_tIME: length arg copy (was 88.3); only the flags hoist left
0x005241f0  949  89.2  89.2  cxx:ok  a  png_handle_sBIT: length arg copy (was 87.9); only the flags hoist left
0x00523e40  932  90.0  90.0  cxx:ok  a  png_handle_gAMA: length arg copy (was 87.5); only the flags hoist left
0x005238c0  1034  87.1  87.1  cxx:ok  a  png_handle_PLTE: only the flags hoist; arg copies no gain
0x00525fb0  850  87.4  89.6  cxx:ok  a  png_handle_bKGD: length arg copy (was 86.7); u16 zero-ext idiom xor/mov vs movzx + flags hoist
0x00524f70  1098  88.7  88.7  cxx:ok  a  png_handle_sRGB: info_ptr+length copies (was 86.5); only the flags hoist left
0x005245b0  2481  91.2  92.8  cxx:ok  b  png_handle_cHRM: info_ptr+length copies give the 0x20 frame, official float order again (was 85.5); left: abs() operand reload reg (mov edx/lea vs mov eax/add) at 3 sites + png_set_cHRM_fixed arg regs; permuter no gain
