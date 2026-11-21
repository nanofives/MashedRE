// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0040bd80 = re/match/s02/0040bd80.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040bd80 PortTop_Bound_0040bd80
#define FUN_00412190 PortTop_RFStop_00412190   // named stop
#define FUN_00413f20 PortTop_RFStop_00413f20   // named stop
#define FUN_00421080 Orig_00421080   // exe body (Match/PortTop_00421080.cpp)
#define FUN_0045b350 RwInitNullStub   // exe body (Match/MatchS09_0045b350.cpp)
#define FUN_00475a60 PortTop_Bound_00475a60
#define FUN_00475ef0 Orig_00475ef0   // exe body (Match/PortTop_00475ef0.cpp)
#define FUN_00477730 PortTop_Bound_00477730
#define FUN_00486270 PortTop_Bound_00486270
#define FUN_00486f90 PortTop_Bound_00486f90
#define FUN_00487df0 PortTop_RFStop_00487df0   // named stop
#define FUN_0048a5d0 PortTop_Bound_0048a5d0
#define FUN_0048ade0 PortTop_Bound_0048ade0
#define FUN_0048bb70 PortTop_Bound_0048bb70
#define FUN_0048f260 PortTop_Bound_0048f260
#define FUN_0048f680 PortTop_Bound_0048f680
#define FUN_0048f740 PortTop_Bound_0048f740
#define FUN_004904d0 UtilZeroTable86a4a0   // exe body (Match/MatchS15_004904d0.cpp)
#define thunk_FUN_00412190 PortTop_RFStop_00412190   // Ghidra thunk name of the jump to 0x00412190: the port calls its exe body
#define thunk_FUN_00475ef0 Orig_00475ef0   // Ghidra thunk name of the jump to 0x00475ef0: the port calls its exe body
extern "C" {
#include "../../../../re/match/s02/0040bd80.c"
}
#endif // MASHED_STANDALONE
