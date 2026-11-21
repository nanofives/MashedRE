// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0044d5e0 = re/match/s08/0044d5e0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0044d5e0 PortTop_Bound_0044d5e0
#define FUN_004c18c0 PortTop_Bound_004c18c0
#define FUN_00547230 PortTop_RFStop_00547230   // named stop
extern "C" {
#include "../../../../re/match/s08/0044d5e0.c"
}
#endif // MASHED_STANDALONE
