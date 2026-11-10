// POWERUPINIT-A (L-111 step 1): exe-only port body of rva 0x00453f30 = the matched source re/match/s09/00453f30.c
// (MATCH-S09, 100%): stores 0xffffffff in the first dword of each 0x24-byte record from 0x006870b4 up to 0x00687ec4.
// No callees. Caller on the MASHED_ORIG_TOP path: the matched 0x0045bae0 (Match/PortTop_0045bae0.cpp). No exe code
// reads or writes 0x006870b4..0x00687ec4 (grep of mashedmod/src, 2026-11-10). The RVA also has an .asi-only body
// (Match/MatchS09_00453f30.cpp, same source).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00453f30 Orig_00453f30
extern "C" {
#include "../../../../re/match/s09/00453f30.c"
}
#endif  // MASHED_STANDALONE
