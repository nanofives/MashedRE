// PORT-TOP-AUDIO-HWINIT-4627F0 (2026-11-13): exe-only port body of rva 0x00550da0 = re/match/s24/00550da0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x004627f0 (sound engine HW init, called by 0x004669b0). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00550da0 PortTop_Bound_00550da0
#define FUN_00550740 PortTop_Bound_00550740
#define FUN_00551550 PortTop_Bound_00551550
extern "C" {
#include "../../../../re/match/s24/00550da0.c"
}
#endif // MASHED_STANDALONE
