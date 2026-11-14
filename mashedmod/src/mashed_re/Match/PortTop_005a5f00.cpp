// PORT-TOP-AUDIO-HWINIT-4627F0 (2026-11-13): exe-only port body of rva 0x005a5f00 = re/match/s25/005a5f00.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x004627f0 (sound engine HW init, called by 0x004669b0). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005a5f00 PortTop_Bound_005a5f00
#define FUN_005a5f60 PortTop_Bound_005a5f60
#define FUN_005a5fd0 PortTop_Bound_005a5fd0
#define FUN_005a6010 PortTop_Bound_005a6010
#define FUN_005a6190 PortTop_Bound_005a6190
extern "C" {
#include "../../../../re/match/s25/005a5f00.c"
}
#endif // MASHED_STANDALONE
