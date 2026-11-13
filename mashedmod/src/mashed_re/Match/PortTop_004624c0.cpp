// PORT-TOP-AUDIO-HWINIT-4627F0 (2026-11-13): exe-only port body of rva 0x004624c0 = re/match/s10/004624c0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x004627f0 (sound engine HW init, called by 0x004669b0). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004624c0 PortTop_Bound_004624c0
#define FUN_00550430 PortTop_Bound_00550430
#define FUN_00551190 PortTop_Bound_00551190
#define FID_conflict__wprintf PortTop_Printf04G   // printf -> log (Match/PortTop_0040bbb0.cpp)
extern "C" {
#include "../../../../re/match/s10/004624c0.c"
}
#endif // MASHED_STANDALONE
