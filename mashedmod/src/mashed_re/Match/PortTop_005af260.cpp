// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005af260 = re/match/s26/005af260.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005af260 PortTop_Bound_005af260
#define FUN_005af180 PortTop_Bound_005af180
#define FUN_005af200 AudioChainWalk   // exe body (Audio/AudioChainWalk.cpp)
#define FUN_005af230 PortTop_Bound_005af230
#define FUN_005af2a0 PortTop_AStop_005af2a0   // named stop
extern "C" {
#include "../../../../re/match/s26/005af260.c"
}
#endif // MASHED_STANDALONE
