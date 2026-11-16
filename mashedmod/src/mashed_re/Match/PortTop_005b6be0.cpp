// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005b6be0 = re/match/s27/005b6be0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005b6be0 PortTop_Bound_005b6be0
#define FUN_004d7ff0 Orig_004d7ff0   // exe body (Match/PortTop_004d7ff0.cpp)
#define FUN_005a9e10 PortTop_Bound_005a9e10
#define FUN_005aee20 PortTop_Bound_005aee20
#define FUN_005b6e60 PortTop_AStop_005b6e60   // named stop
#define FUN_005b7010 PortTop_Bound_005b7010
#define FUN_005b7040 PortTop_AStop_005b7040   // named stop
#define FUN_005b70e0 PortTop_Bound_005b70e0
extern "C" {
#include "../../../../re/match/s27/005b6be0.c"
}
#endif // MASHED_STANDALONE
