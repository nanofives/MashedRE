// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005af510 = re/match/s26/005af510.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005af510 PortTop_Bound_005af510
#define FUN_005af260 PortTop_Bound_005af260
#define FUN_005af600 PortTop_Bound_005af600
#define FUN_005af690 PortTop_AStop_005af690   // named stop
#define FUN_005af740 PortTop_Bound_005af740
#define FUN_005af7a0 PortTop_Bound_005af7a0
#define FUN_005af7d0 PortTop_AStop_005af7d0   // named stop
#define FUN_005af8f0 PortTop_AStop_005af8f0   // named stop
extern "C" {
#include "../../../../re/match/s26/005af510.c"
}
#endif // MASHED_STANDALONE
