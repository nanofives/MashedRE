// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005b1500 = re/match/s27/005b1500.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005b1500 PortTop_Bound_005b1500
#define FUN_005ab150 PortTop_Bound_005ab150
#define FUN_005aea40 PortTop_Bound_005aea40
#define FUN_005b8080 PortTop_Bound_005b8080
extern "C" {
#include "../../../../re/match/s27/005b1500.c"
}
#endif // MASHED_STANDALONE
