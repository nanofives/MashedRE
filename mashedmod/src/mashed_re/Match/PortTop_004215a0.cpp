// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x004215a0 = re/match/s05/00420e00.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004215a0 PortTop_Bound_004215a0
#define FUN_0041ede0 PortTop_Bound_0041ede0
#define FUN_0041ee50 PortTop_Bound_0041ee50
#define FUN_0041f1c0 GetEventFlag   // exe body (Match/MatchS04_0041f1c0.cpp)
#define FUN_0041f1e0 PortTop_Bound_0041f1e0
#define FUN_0046be50 PortTop_Bound_0046be50
#define FUN_0048f590 PortTop_Bound_0048f590
#define FUN_00490020 PortTop_RFStop_00490020   // named stop
#define FUN_00420e00 PortTop_Local_004215a0_00420e00   // file-local in this source
extern "C" {
#include "../../../../re/match/s05/00420e00.c"
}
#endif // MASHED_STANDALONE
