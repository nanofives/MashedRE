// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0041bf20 = re/match/s04/0041b720.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0041bf20 PortTop_Bound_0041bf20
#define FUN_004b6520 Orig_004b6520   // exe body (Match/PortTop_004b6520.cpp)
#define FUN_0041b720 PortTop_Local_0041bf20_0041b720   // file-local in this source
extern "C" {
#include "../../../../re/match/s04/0041b720.c"
}
#endif // MASHED_STANDALONE
