// PORT-OT-BOOT (L-111 (a) step 1): exe-only port body of rva 0x0047ba00 = the matched source re/match/topm01/0047ba00.c
// (PORT-TOP-MATCH-01, BYTE-IDENTICAL 14 B): returns 0x00496940(param_1). The matched AppInitialiseOnBootup 0x00402750
// (Match/PortTop_00402750.cpp) calls it with (FUN_004671a0(0), 0) on the MASHED_ORIG_TOP path.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0047ba00 Orig_0047ba00
#define FUN_00496940 Orig_00496940                           // matched, Match/PortTop_00496940.cpp
extern "C" {
#include "../../../../re/match/topm01/0047ba00.c"
}
#endif  // MASHED_STANDALONE
