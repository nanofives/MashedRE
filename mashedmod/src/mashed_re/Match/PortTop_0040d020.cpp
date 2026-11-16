// RVA-LINT: ORIGTOP-ONLY
// PORT-OT-RACE (L-111 step 3): exe-only port body of rva 0x0040d020 = the matched source re/match/s02/0040d020.c (match.py 100%,
// C4 by match), 0x00426e10(**(param_1 + param_2 * 4)); 0x00426e10 is PortTop_RL_00426e10 (checkpoint 2: logs and calls
// the matched body PortTop_Bound_00426e10, Match/PortTop_00426e10.cpp; was the stand-in PortTop_StandInRL_00426e10). A callee of the matched 0x0040d270 (Match/PortTop_0040d270.cpp), reached only on the
// MASHED_ORIG_TOP path with MASHED_OT_RACELOAD=1. The .asi's copy of this RVA is a different TU; this one is exe only.
#ifdef MASHED_STANDALONE
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040d020 Orig_0040d020
#define FUN_00426e10 PortTop_RL_00426e10   // Match/PortTop_StandInsRL.cpp: logs, then the matched course load proper
extern "C" {
#include "../../../../re/match/s02/0040d020.c"
}
#endif // MASHED_STANDALONE
