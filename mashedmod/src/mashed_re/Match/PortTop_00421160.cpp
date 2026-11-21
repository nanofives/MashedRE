// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00421160 = re/match/s05/00421100.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00421160 PortTop_Bound_00421160
#define FUN_00472650 PortTop_Bound_00472650   // exe body (Match/PortTop_00472650.cpp)
#define FUN_00472690 PortTop_Bound_00472690   // exe body (Match/PortTop_00472690.cpp)
#define FUN_00420d80 PortTop_Local_00421160_00420d80   // file-local in this source
#define FUN_00421100 PortTop_Local_00421160_00421100   // file-local in this source
extern "C" {
#include "../../../../re/match/s05/00421100.c"
}
#endif // MASHED_STANDALONE
