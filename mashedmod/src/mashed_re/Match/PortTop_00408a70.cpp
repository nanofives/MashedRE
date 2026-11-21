// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00408a70 = re/match/s01/00408a70.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00408a70 PortTop_Bound_00408a70
#define FUN_004a2c48 MashedCrt_ftol2_004a2c48   // exe body (Match/PortTop_CrtHelpers.cpp)
#define FUN_004a2cbd PortTop_Crt_004a2cbd   // exe body (Match/PortTop_00493710.cpp)
extern "C" {
#include "../../../../re/match/s01/00408a70.c"
}
#endif // MASHED_STANDALONE
