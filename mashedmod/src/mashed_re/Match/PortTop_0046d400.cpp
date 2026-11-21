// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0046d400 = re/match/s11/0046d400.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0046d400 PortTop_Bound_0046d400
#define FUN_0046d3a0 PortTop_RFStop_0046d3a0   // named stop
#define FUN_00538c80 PortTop_RFStop_00538c80   // named stop
extern "C" {
#include "../../../../re/match/s11/0046d400.c"
}
#endif // MASHED_STANDALONE
