// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00558100 = re/match/s24/00558100.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00558100 PortTop_Bound_00558100
#define FUN_004e5f90 RwCore_004e5f90   // exe body (RwCore/RwCore_004e5f90.cpp)
#define FUN_00557b70 PortTop_RFStop_00557b70   // named stop
extern "C" {
#include "../../../../re/match/s24/00558100.c"
}
#endif // MASHED_STANDALONE
