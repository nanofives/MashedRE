// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00411170 = re/match/s02/00411170.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00411170 PortTop_Bound_00411170
#define FUN_00410510 PortTop_Bound_00410510
#define FUN_00410d10 PortTop_Bound_00410d10
#define FUN_00411600 PortTop_Bound_00411600
#define FUN_00411ae0 PortTop_Bound_00411ae0
#define FUN_004194f0 PortTop_RFStop_004194f0   // named stop
#define FUN_00430290 PortTop_RFStop_00430290   // named stop
#define thunk_FUN_004194f0 PortTop_RFStop_004194f0   // Ghidra thunk name of the jump to 0x004194f0: the port calls its exe body
extern "C" {
#include "../../../../re/match/s02/00411170.c"
}
#endif // MASHED_STANDALONE
