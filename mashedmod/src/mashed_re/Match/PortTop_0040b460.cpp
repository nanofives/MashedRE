// RVA-LINT: ORIGTOP-ONLY
// PORT-TOP-08 (L-96 top-down lane): exe-only port body of rva 0x0040b460 = the matched source re/match/s02/0040b460.c
// (match.py 100%), called by 0x0042c960 (Match/PortTop_0042c960.cpp). The .asi body is the hand MenuMenusBD in the
// multi-function Frontend/MenuMenusB.cpp.
#ifdef MASHED_STANDALONE
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040b460 Orig_0040b460
#define FUN_00417740 Float89a870ToInt   // matched exe body, Match/MatchGFFU_00417740.cpp
extern "C" {
#include "../../../../re/match/s02/0040b460.c"
}
#endif // MASHED_STANDALONE
