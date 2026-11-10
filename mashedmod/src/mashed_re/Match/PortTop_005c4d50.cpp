// PORT-OT-HUD-2 (L-111 (a) step 1): exe-only port body of rva 0x005c4d50 = the matched source re/match/s28/005c4d50.c:
// when param_1[0] != 0 and param_1[2] != 0, frees param_1[0] through RwGlobals +0x10c and zeroes param_1[0] / [2], then
// frees param_1 itself through RwGlobals +0x10c, returns 1. RwGlobals +0x10c is librw's free in the exe
// (LibRw/RwApiBind.cpp MashedRwGlobals). Its only caller in the exe is the matched 0x00555830 (Match/PortTop_00555830.cpp,
// MASHED_ORIG_TOP path with MASHED_ORIG_TOP_RWMOD=1). No other body of this RVA exists in either target.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005c4d50 Orig_005c4d50
extern "C" {
#include "../../../../re/match/s28/005c4d50.c"
}
#endif  // MASHED_STANDALONE
