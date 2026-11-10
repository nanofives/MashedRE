// PORT-OT-BOOT (L-111 (a) step 1): exe-only port body of rva 0x00471eb0 = the matched source re/match/s11/00471eb0.c
// (MATCH-S11 h6: a 5-byte jmp 0x00471df0). The matched AppInitialiseOnBootup 0x00402750 (Match/PortTop_00402750.cpp)
// calls it on the MASHED_ORIG_TOP path.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00471eb0 Orig_00471eb0
#define FUN_00471df0 Orig_00471df0                           // matched, Match/PortTop_00471df0.cpp
extern "C" {
#include "../../../../re/match/s11/00471eb0.c"
}
#endif  // MASHED_STANDALONE
