// PORT-OT-VEHICLE (L-111 (a) step 1): exe-only port body of rva 0x00483a40 = the matched source re/match/s13/00483a40.c
// (MATCH-S13, BYTE-IDENTICAL 41 B): RwGlobals +0x10c (librw free, LibRw/RwApiBind.cpp) on *(p + 0x14), then on p, returns 0.
// Its only caller in the exe is the matched 0x004114e0 (Match/PortTop_004114e0.cpp), on the MASHED_ORIG_TOP path
// (AppDestroy 0x00402a40 -> 0x0040cfd0 -> 0x004114e0), and only for a nonzero DAT_0063bb04 / DAT_0063bb08.
// OT_MEASURE: RwGlobals OK, call slot +0x10c only. No raw-address call: the slot is read through DAT_007d3ff8, the exe's
// fake RW device table (B15 bridge). The source's `typedef int code();` is `int code(int)` under C++ only (C text unchanged).
// No other exe body of this RVA. The .asi calls the original at 0x00483a40 (Vehicle/Replay.cpp kFn_FUN_00483a40).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00483a40 Orig_00483a40
extern "C" {
#include "../../../../re/match/s13/00483a40.c"
}
#endif  // MASHED_STANDALONE
