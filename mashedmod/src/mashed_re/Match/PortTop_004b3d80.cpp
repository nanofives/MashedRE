// PORT-TOP-04F (pc.txd and perm families): exe-only port body of rva 0x004b3d80 = the matched source
// re/match/s18/004b3d80.c (match.py BYTE-IDENTICAL 82 B, relocheck OK 4/4): RwStreamOpen(2, 1, name), RwStreamFindChunk
// 0x23, the chunk-0x23 texture dictionary read 0x0054f8d0, RwStreamClose; returns the dictionary or 0. Callers on the
// MASHED_ORIG_TOP path: the matched 0x00402750 ("toastart\pc\pc.txd" -> DAT_00636ac8) and 0x00425bc0
// (Match/PortTop_00425bc0.cpp, Copters.txd from perm.piz). Callees are bindings: the stream calls in
// Match/PortTop_RwStream04F.cpp, 0x0054f8d0 in LibRw/RwApiBind.cpp.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004b3d80 Orig_004b3d80
#define FUN_004cc230 PortTop_RwStreamOpen_004cc230
#define FUN_004cc5e0 PortTop_RwStreamFindChunk_004cc5e0
#define FUN_004cc160 PortTop_RwStreamClose_004cc160
#define FUN_0054f8d0 MashedRw_TexDictStreamRead_0054f8d0
extern "C" {
#include "../../../../re/match/s18/004b3d80.c"
}
#endif  // MASHED_STANDALONE
