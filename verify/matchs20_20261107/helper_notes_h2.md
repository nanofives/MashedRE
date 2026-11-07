# MATCH-S20 helper h2 notes (per-function lines appended as finished)
0x004cbb50  11  100.0  100.0  cxx:ok  -  __stdcall vtable pointer (no tail jmp)
0x004cbb00  30  100.0  100.0  cxx:ok  -  D3D COM call via __stdcall pointer
0x004cbb20  34  100.0  100.0  cxx:ok  -  __stdcall pointer, int return hr>=0
0x004cb840  41  100.0  100.0  cxx:ok  -  __stdcall pointer, int return hr>=0
0x004cbad0  48  100.0  100.0  cxx:ok  -  __stdcall pointer, hr>=0 twice
0x004cc4f0  28  100.0  100.0  cxx:ok  -  all 26 cases written out (direct jump table), --bytes identical
0x004cc580  82  100.0  100.0  cxx:ok  -  12-byte local struct passed by address
0x004cb740  244  100.0  100.0  cxx:ok  -  && chain + switch with if(!(..)) return 0; break
0x004cbd30  318  100.0  100.0  cxx:ok  -  RwStreamRead shape, /G5, length=avail before RWERROR
0x004cc160  179  100.0  100.0  cxx:ok  -  RwStreamClose shape, RwGlobals free-list call
0x004cc400  231  100.0  100.0  cxx:ok  -  _rwStreamReadChunkHeader shape, if/else unpack
0x004d2c80  426  100.0  100.0  cxx:ok  -  hand-written (raw did not compile), body under if((fmt&0x6000)==0)
0x004cbb70  82  93.3  96.7  cxx:ok  b  camera loaded into ecx+add, original eax+lea; dot written z+(x+y)
0x004cba80  76  90.3  90.3  cxx:ok  b  original pushes esi/edi before the count test (shared epilogue for empty table)
0x004cbbd0  130  75.5  77.4  cxx:ok  b  original keeps the 3 corner values on the x87 stack and multiplies by register
0x004cc6e0  142  45.9  54.1  cxx:ok  a  memcpy intrinsic + >64-byte local gives an aligned ebp frame in 9176, original has none (char[0x40] -> 72%, rest identical)
0x004cb4c0  320  83.7  83.7  cxx:ok  a  original has push ebp/mov ebp,esp with no locals (aligned-frame residue), plus down-counter loop
0x004cc050  250  78.5  81.7  cxx:ok  b  original pushes edi only in the file case, ours at entry
0x004cc5e0  244  59.8  60.9  cxx:ok  a  original packs readLength into the dead stream param home (frame 0xc vs 0x14)
0x004cecb0  468  79.6  79.6  cxx:ok  b  all instructions match, shared flag tail placed after the 0x420 loop instead of after default
0x004d3e90  546  82.8  86.6  cxx:ok  b  original pushes ebp at entry, ours in the locked path; index-fix loop register swap
0x004d0290  632  73.4  81.5  cxx:ok  b  ebp push at entry, duplicated entry RWERROR block, surface in the dead param_1 home
0x004d3510  914  55.8  75.3  cxx:ok  a  locals in dead param homes (no frame), bool-materialized automip check not found
0x004d07b0  1351  77.0  77.1  cxx:ok  a  only stack offsets differ: 4 locals in dead param homes (frame 0x58 vs 0x68)

## Final list (h2)
0x004cb4c0  320  83.7  83.7  cxx:ok  a  push ebp/mov ebp,esp frame with no locals in the original, down-counter loop
0x004cb740  244  100.0  100.0  cxx:ok  -  && chain + switch, if(!(..)) return 0; break
0x004cb840  41  100.0  100.0  cxx:ok  -  __stdcall COM pointer, int hr>=0
0x004cba80  76  90.3  90.3  cxx:ok  b  original pushes esi/edi before the count test
0x004cbad0  48  100.0  100.0  cxx:ok  -  __stdcall COM pointer, hr>=0 twice
0x004cbb00  30  100.0  100.0  cxx:ok  -  __stdcall COM pointer
0x004cbb20  34  100.0  100.0  cxx:ok  -  __stdcall COM pointer, int hr>=0
0x004cbb50  11  100.0  100.0  cxx:ok  -  __stdcall vtable call (no tail jmp)
0x004cbb70  82  93.3  96.7  cxx:ok  b  camera into ecx+add vs original eax+lea
0x004cbbd0  130  75.5  77.4  cxx:ok  b  corner values kept on the x87 stack in the original
0x004cbd30  318  100.0  100.0  cxx:ok  -  RwStreamRead shape, /G5, --bytes identical
0x004cc050  250  78.5  81.7  cxx:ok  b  edi pushed only in the file case in the original
0x004cc160  179  100.0  100.0  cxx:ok  -  RwStreamClose shape
0x004cc400  231  100.0  100.0  cxx:ok  -  _rwStreamReadChunkHeader shape, if/else unpack
0x004cc4f0  28  100.0  100.0  cxx:ok  -  all 26 cases written out, --bytes identical
0x004cc580  82  100.0  100.0  cxx:ok  -  12-byte local struct by address
0x004cc5e0  244  59.8  60.9  cxx:ok  a  local packed into dead param home in the original (frame 0xc vs 0x14)
0x004cc6e0  142  45.9  54.1  cxx:ok  a  aligned ebp frame from memcpy + 256-byte buffer in 9176 only
0x004d2c80  426  100.0  100.0  cxx:ok  -  hand-written (raw did not compile)
0x004cecb0  468  79.6  79.6  cxx:ok  b  block layout of the shared flag tail only
0x004d3e90  546  82.8  86.6  cxx:ok  b  ebp push placement, index-fix loop registers
0x004d0290  632  73.4  81.5  cxx:ok  b  ebp push placement, entry error block not duplicated, param-home surface
0x004d3510  914  55.8  75.3  cxx:ok  a  locals in dead param homes, bool-materialized automip check
0x004d07b0  1351  77.0  77.1  cxx:ok  a  only stack offsets differ (locals in dead param homes)
