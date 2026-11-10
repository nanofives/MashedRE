// PORT-OT-HUD (L-111 (a) step 1): exe-only port body of rva 0x00474db0 RwMallocArrayZeroFill = the matched source
// re/match/s11/00474db0.c (BYTE-IDENTICAL 42 B): p = (*(RwGlobals + 0x108))(n * size, 0), then FUN_004b6520(p, n * size).
// RwGlobals +0x108 is librw's malloc in the exe (LibRw/RwApiBind.cpp MashedRwGlobals, OT_MEASURE verdict OK).
// Its only caller in the exe is the matched 0x004770c0 (Match/PortTop_004770c0.cpp, MASHED_ORIG_TOP path). The other
// body of this RVA is the .asi-only Match/MatchS11_00474db0.cpp, so this TU is the ORIGTOP-ONLY class (CROSS-TARGET).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00474db0 Orig_00474db0
#define FUN_004b6520 Orig_004b6520                          // matched, Match/PortTop_004b6520.cpp
extern "C" {
#include "../../../../re/match/s11/00474db0.c"
}
#endif  // MASHED_STANDALONE
