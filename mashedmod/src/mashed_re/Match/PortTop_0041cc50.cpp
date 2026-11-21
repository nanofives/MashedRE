// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0041cc50 = re/match/s04/0041c410.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0041cc50 PortTop_Bound_0041cc50
#define FUN_0040b420 PortTop_Bound_0040b420
#define FUN_0040b540 PortTop_Bound_0040b540
#define FUN_0040b6d0 UtilArrayGet8a94e0   // exe body (Match/MatchS02_0040b6d0.cpp)
#define FUN_0040b890 PortTop_Bound_0040b890
#define FUN_0040b930 PortTop_Bound_0040b930
#define FUN_0040b970 PortTop_Bound_0040b970
#define FUN_0040b9a0 PortTop_Bound_0040b9a0
#define FUN_0040ba00 PortTop_Bound_0040ba00
#define FUN_004c13e0 RwCore_004c13e0   // exe body (RwCore/RwCore_004c13e0.cpp)
#define FUN_004c1480 PortTop_Bound_004c1480   // exe body (Match/PortTop_004c1480.cpp)
#define FUN_004c51a0 RwCore_004c51a0   // exe body (RwCore/RwCore_004c51a0.cpp)
#define FUN_0041c410 PortTop_Local_0041cc50_0041c410   // file-local in this source
extern "C" {
#include "../../../../re/match/s04/0041c410.c"
}
#endif // MASHED_STANDALONE
