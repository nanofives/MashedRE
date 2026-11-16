// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005a8560 = re/match/s25/005a8560.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005a8560 PortTop_Bound_005a8560
#define FUN_005a8640 PortTop_AStop_005a8640   // named stop
#define FUN_005aab00 PortTop_Bound_005aab00
#define FUN_005aab70 PortTop_Bound_005aab70
#define FUN_005af010 PortTop_Bound_005af010
#define FUN_005af180 PortTop_Bound_005af180
#define FUN_005af200 AudioChainWalk   // exe body (Audio/AudioChainWalk.cpp)
extern "C" {
#include "../../../../re/match/s25/005a8560.c"
}
#endif // MASHED_STANDALONE
