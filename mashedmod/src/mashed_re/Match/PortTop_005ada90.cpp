// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005ada90 = re/match/s30/005ada90.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005ada90 PortTop_Bound_005ada90
#define FUN_005add60 PortTop_Bound_005add60
#define FUN_005ae650 PortTop_AStop_005ae650   // named stop
#define FUN_005ae7e0 PortTop_Bound_005ae7e0
#define FUN_005b3580 AudioListInit   // exe body (Audio/AudioList_ag.cpp)
extern "C" {
#include "../../../../re/match/s30/005ada90.c"
}
#endif // MASHED_STANDALONE
