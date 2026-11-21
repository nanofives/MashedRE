// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00421d20 = re/match/s05/00421d20.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00421d20 PortTop_Bound_00421d20
#define FUN_0041f1c0 GetEventFlag   // exe body (Match/MatchS04_0041f1c0.cpp)
#define FUN_0041f260 PortTop_Bound_0041f260
#define FUN_0041f290 PortTop_Bound_0041f290
#define FUN_00421750 PortTop_Bound_00421750
#define FUN_00421800 PortTop_Bound_00421800   // exe body (Match/PortTop_00421750.cpp)
#define FUN_0046cb30 PortTop_Bound_0046cb30
#define FUN_00476880 PortTop_RFStop_00476880   // named stop
#define FUN_00486460 PortTop_Bound_00486460
#define FUN_004b4510 PortTop_Bound_004b4510
#define FUN_004c1340 PortTop_Bound_004c1340   // exe body (Match/PortTop_004c1340.cpp)
#define FUN_004c1480 PortTop_Bound_004c1480   // exe body (Match/PortTop_004c1480.cpp)
#define FUN_004c39b0 RwCore_004c39b0   // exe body (RwCore/RwCore_004c39b0.cpp)
#define FUN_00559c40 PortTop_RFStop_00559c40   // named stop
#define FUN_0055ae70 PortTop_Bound_0055ae70
#define FUN_0055dec0 UtilDeref55dec0   // exe body (Util/UtilLeaves_ab6.cpp)
#define FUN_00421960 PortTop_Local_00421d20_00421960   // file-local in this source
extern "C" {
#include "../../../../re/match/s05/00421d20.c"
}
#endif // MASHED_STANDALONE
