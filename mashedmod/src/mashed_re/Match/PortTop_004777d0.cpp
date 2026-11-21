// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x004777d0 = re/match/s12/00477490.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004777d0 PortTop_Bound_004777d0
#define FUN_004c13e0 RwCore_004c13e0   // exe body (RwCore/RwCore_004c13e0.cpp)
#define FUN_004c1480 PortTop_Bound_004c1480   // exe body (Match/PortTop_004c1480.cpp)
#define FUN_004773f0 PortTop_Local_004777d0_004773f0   // file-local in this source
#define FUN_00477490 PortTop_Local_004777d0_00477490   // file-local in this source
extern "C" {
#include "../../../../re/match/s12/00477490.c"
}
#endif // MASHED_STANDALONE
