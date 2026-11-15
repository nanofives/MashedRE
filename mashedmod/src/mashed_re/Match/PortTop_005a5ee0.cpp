// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005a5ee0 = re/match/s30/005a5ee0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005a5ee0 PortTop_Bound_005a5ee0
#define FUN_004c2f60 RwCore_004c2f60   // exe body (RwCore/RwCore_004c2f60.cpp)
#define FUN_004c3040 RwCore_004c3040   // exe body (RwCore/RwCore_004c3040.cpp)
#define FUN_004c3270 RwCore_004c3270   // exe body (RwCore/RwCore_004c3270.cpp)
extern "C" {
#include "../../../../re/match/s30/005a5ee0.c"
}
#endif // MASHED_STANDALONE
