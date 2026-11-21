// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0048b960 = re/match/s14/0048b960.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0048b960 PortTop_Bound_0048b960
#define FUN_00472650 PortTop_Bound_00472650   // exe body (Match/PortTop_00472650.cpp)
#define FUN_0048b440 PortTop_Bound_0048b440
#define FUN_0048b650 PortTop_Bound_0048b650
#define FUN_0048b7c0 PortTop_Bound_0048b7c0
#define FUN_004a2c48 MashedCrt_ftol2_004a2c48   // exe body (Match/PortTop_CrtHelpers.cpp)
#define FUN_004b40f0 PortTop_Bound_004b40f0
#define FUN_004c1340 PortTop_Bound_004c1340   // exe body (Match/PortTop_004c1340.cpp)
#define FUN_00544bf0 PortTop_RFStop_00544bf0   // named stop
extern "C" {
#include "../../../../re/match/s14/0048b960.c"
}
#endif // MASHED_STANDALONE
