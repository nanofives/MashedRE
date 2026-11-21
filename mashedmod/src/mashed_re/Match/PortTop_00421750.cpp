// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00421750 = re/match/s05/00421750.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00421750 PortTop_Bound_00421750
#define FUN_0055ac00 PortTop_RFStop_0055ac00   // named stop
#define FUN_0055ad30 PortTop_Bound_0055ad30
#define FUN_0055b650 PortTop_RFStop_0055b650   // named stop
#define FUN_0057c210 PortTop_RFStop_0057c210   // named stop
#define FUN_00421800 PortTop_Bound_00421800   // bound rva of the same source: this TU carries its only exe copy
extern "C" {
#include "../../../../re/match/s05/00421750.c"
}
#endif // MASHED_STANDALONE
