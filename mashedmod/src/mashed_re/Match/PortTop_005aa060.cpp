// PORT-TOP-AUDIO-HWINIT-4627F0 (2026-11-13): exe-only port body of rva 0x005aa060 = re/match/s25/005aa060.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x004627f0 (sound engine HW init, called by 0x004669b0). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005aa060 PortTop_Bound_005aa060
#define FUN_005aa0c0 PortTop_Bound_005aa0c0
#define FUN_005aa1e0 PortTop_Bound_005aa1e0
extern "C" {
#include "../../../../re/match/s25/005aa060.c"
}
#endif // MASHED_STANDALONE
