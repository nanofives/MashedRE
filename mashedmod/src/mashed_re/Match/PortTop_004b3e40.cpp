// POWERUPINIT-A (L-111 step 1): exe-only port body of rva 0x004b3e40 = the matched source re/match/s18/004b3e40.c
// (MATCH-S18, 100%): RwStreamOpen(3 = memory, 1 = read, {buffer, length}), RwStreamFindChunk 0x10 (the clump chunk) with
// the caller's length / version out pointers, RpClumpStreamRead, RwStreamClose; returns the clump or 0. Callers on the
// MASHED_ORIG_TOP path: the matched builders 0x004587a0 (25 x IconCube.dff, Match/PortTop_004587a0.cpp) and 0x00411f30
// (6 x lazer.dff, Match/PortTop_00411f30.cpp), both from 0x0045bae0. Callees are the bindings 0x004b3bf0 uses
// (Match/PortTop_004b3bf0.cpp): the stream calls in Match/PortTop_RwStream04F.cpp (the open binding takes type 3 with an
// RwMemory record), RpClumpStreamRead 0x004e7420 in LibRw/RwApiBind.cpp.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004b3e40 Orig_004b3e40
#define FUN_004cc230 PortTop_RwStreamOpen_004cc230
#define FUN_004cc5e0 PortTop_RwStreamFindChunk_004cc5e0
#define FUN_004cc160 PortTop_RwStreamClose_004cc160
#define FUN_004e7420 MashedRw_ClumpStreamRead_004e7420
extern "C" {
#include "../../../../re/match/s18/004b3e40.c"
}
#endif  // MASHED_STANDALONE
