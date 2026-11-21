// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00484de0 = re/match/s14/00484de0.c
// (L-126 admitted (re/match/ADMITTED.tsv, EQUIV)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00484de0 PortTop_Bound_00484de0
extern "C" {
#include "../../../../re/match/s14/00484de0.c"
}
#endif // MASHED_STANDALONE
