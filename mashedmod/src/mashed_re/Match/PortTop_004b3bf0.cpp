// PORT-TOP-04H (family 3, perm clumps): exe-only port body of rva 0x004b3bf0 = the matched source
// re/match/s18/004b3bf0.c (match.py BYTE-IDENTICAL 98 B): RwStreamOpen(2, 1, name), RwStreamFindChunk 0x10,
// RpClumpStreamRead, RwFrameUpdateObjects on the clump's frame (+0x04), RwStreamClose; returns the clump or 0. The DFF
// counterpart of 0x004b3d80 (Match/PortTop_004b3d80.cpp). Caller on the MASHED_ORIG_TOP path: 0x00401630
// (Match/PortTop_00401630.cpp, "map_%d.dff" for 0x004025f0). Callees are bindings: the stream calls in
// Match/PortTop_RwStream04F.cpp, RpClumpStreamRead 0x004e7420 / RwFrameUpdateObjects 0x004c0e50 in LibRw/RwApiBind.cpp.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004b3bf0 Orig_004b3bf0
#define FUN_004cc230 PortTop_RwStreamOpen_004cc230
#define FUN_004cc5e0 PortTop_RwStreamFindChunk_004cc5e0
#define FUN_004cc160 PortTop_RwStreamClose_004cc160
#define FUN_004e7420 MashedRw_ClumpStreamRead_004e7420
#define FUN_004c0e50 MashedRw_FrameUpdateObjects_004c0e50
extern "C" {
#include "../../../../re/match/s18/004b3bf0.c"
}
#endif  // MASHED_STANDALONE
