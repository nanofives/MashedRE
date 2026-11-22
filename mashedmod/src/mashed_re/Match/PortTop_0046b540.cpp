// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0046b540 = re/match/s11/0046b540.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0046b540 PortTop_Bound_0046b540
#define FUN_0040ce80 PtrTable5f2770Get   // exe body (Match/MatchS02_0040ce80.cpp)
#define FUN_0042f6a0 GetRaceSubMode   // exe body (Match/MatchS07_0042f6a0.cpp)
#define FUN_00430790 GetDat0067f17c   // exe body (Match/MatchS07_00430790.cpp)
#define FUN_00431d70 PortTop_Bound_00431d70   // exe body (Match/PortTop_00431d70.cpp)
#define FUN_004c3b30 RwCore_004c3b30   // exe body (RwCore/RwCore_004c3b30.cpp)
extern "C" {
#include "../../../../re/match/s11/0046b540.c"
}
#endif // MASHED_STANDALONE
