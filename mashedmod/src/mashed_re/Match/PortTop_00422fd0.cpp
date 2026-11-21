// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00422fd0 = re/match/s05/00422fd0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00422fd0 PortTop_Bound_00422fd0
#define FUN_0040e470 CarSlotStateGet   // exe body (Match/MatchS02_0040e470.cpp)
#define FUN_00419760 PortTop_RFStop_00419760   // named stop
#define FUN_004215c0 PortTop_Bound_004215c0
#define FUN_0045ba00 RaceResultIndexedStore   // exe body (Match/MatchS10_0045ba00.cpp)
#define FUN_0046c5c0 VehicleSlotInit   // exe body (Match/MatchS11_0046c5c0.cpp)
#define FUN_0046c790 VehicleSlotFieldSet   // exe body (Match/MatchS11_0046c790.cpp)
#define thunk_FUN_00419760 PortTop_RFStop_00419760   // Ghidra thunk name of the jump to 0x00419760: the port calls its exe body
extern "C" {
#include "../../../../re/match/s05/00422fd0.c"
}
#endif // MASHED_STANDALONE
