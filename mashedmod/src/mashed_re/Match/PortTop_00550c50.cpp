// PORT-TOP-AUDIO-HWINIT-4627F0 (2026-11-13): exe-only port body of rva 0x00550c50 = re/match/s24/00550c50.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x004627f0 (sound engine HW init, called by 0x004669b0). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00550c50 PortTop_Bound_00550c50
#define FUN_00550740 PortTop_Bound_00550740
#define FUN_00551410 PortTop_Bound_00551410
#define FUN_00551460 PortTop_Bound_00551460   // exe body (Match/PortTop_00551460.cpp)
extern "C" {
#include "../../../../re/match/s24/00550c50.c"
}
#endif // MASHED_STANDALONE
