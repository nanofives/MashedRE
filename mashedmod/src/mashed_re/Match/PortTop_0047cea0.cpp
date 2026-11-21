// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0047cea0 = re/match/s13/0047cea0.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0047cea0 PortTop_Bound_0047cea0
#define FUN_0055ac00 PortTop_RFStop_0055ac00   // named stop
#define FUN_0055ac50 PortTop_Bound_0055ac50
#define FUN_0055ad30 PortTop_Bound_0055ad30
#define FUN_0055b650 PortTop_Bound_0055b650
#define FUN_0057c210 PortTop_RFStop_0057c210   // named stop
extern "C" {
#include "../../../../re/match/s13/0047cea0.c"
}
#endif // MASHED_STANDALONE
