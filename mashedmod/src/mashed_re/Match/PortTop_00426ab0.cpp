// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00426ab0 = re/match/s06/00426ab0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00426ab0 PortTop_Bound_00426ab0
#define FUN_0040b090 PortTop_Bound_0040b090
#define FUN_0041e8c0 PortTop_Bound_0041e8c0
#define FUN_0041e9b0 PortTop_Bound_0041e9b0
#define FUN_0041e9e0 PortTop_Bound_0041e9e0
#define FUN_00426700 PortTop_Bound_00426700
#define FUN_00426780 PortTop_Bound_00426780
#define FUN_00426810 PortTop_RFStop_00426810   // named stop
#define FUN_004671a0 RwCore_004671a0   // exe body (RwCore/RwCore_004671a0.cpp)
#define FUN_00467210 PortTop_Bound_00467210
#define FUN_00471ec0 PortTop_RFStop_00471ec0   // named stop
#define FUN_0047c160 PortTop_Bound_0047c160
#define FUN_00491490 PortTop_Bound_00491490
extern "C" {
#include "../../../../re/match/s06/00426ab0.c"
}
#endif // MASHED_STANDALONE
