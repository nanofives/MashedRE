// RVA-LINT: ORIGTOP-ONLY
// PORT-TOP-08 (L-96 top-down lane): exe-only port body of rva 0x0043c000 = the matched source re/match/s08/0043c000.c
// (match.py 100%). Its one caller is the frontend machine 0x0043dfd0 (call site 0x0043e05d), which is still the hand
// stand-in on the MASHED_ORIG_TOP frame path (PortTop_StandIn05_0043dfd0) and the provisional body
// Match/ProvS08_0043dfd0.cpp (self-test only), so this body runs only from the provisional body's self-test today.
// The .asi copy (Match/MatchS08_0043c000.cpp) calls 0x004332a0 and 0x0042c960 at their original addresses, which the
// exe does not map; this copy binds every callee by name.
#ifdef MASHED_STANDALONE
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0043c000 Orig_0043c000
#define FUN_004332a0 Orig_004332a0          // admitted body (ADMITTED.tsv, EQUIV), Match/PortTop_004332a0.cpp
#define FUN_0042f7b0 FrontendCursorUpdate   // matched exe body, Match/MatchS07_0042f7b0.cpp
#define FUN_0042c960 Orig_0042c960          // matched exe body, Match/PortTop_0042c960.cpp
#define FUN_0042fa00 MenuTeamSelectTick     // matched exe body, Match/MatchS07_0042fa00.cpp
extern "C" {
#include "../../../../re/match/s08/0043c000.c"
}
#endif // MASHED_STANDALONE
