// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x004219f0 = re/match/s05/004219f0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004219f0 PortTop_Bound_004219f0
#define FUN_00487020 PortTop_Bound_00487020
#define FUN_004b4510 PortTop_Bound_004b4510
#define FUN_00559ee0 PortTop_RFStop_00559ee0   // named stop
#define FUN_0055dec0 UtilDeref55dec0   // exe body (Util/UtilLeaves_ab6.cpp)
#define FUN_00421980 PortTop_Local_004219f0_00421980   // file-local in this source
#define FUN_004219a0 PortTop_Local_004219f0_004219a0   // file-local in this source
extern "C" {
#include "../../../../re/match/s05/004219f0.c"
}
#endif // MASHED_STANDALONE
