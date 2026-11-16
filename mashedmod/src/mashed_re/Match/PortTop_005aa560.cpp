// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005aa560 = re/match/s25/005aa560.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005aa560 PortTop_Bound_005aa560
#define FUN_004522d0 RwCore_004522d0   // exe body (RwCore/RwCore_004522d0.cpp)
#define FUN_005a9f60 PortTop_AStop_005a9f60   // named stop
#define FUN_005aa030 PortTop_Bound_005aa030
#define FUN_005aa7a0 PortTop_Bound_005aa7a0
#define FUN_005aa7e0 PortTop_Bound_005aa7e0
#define FUN_005aa900 PortTop_Bound_005aa900
#define FUN_005aad40 PortTop_Bound_005aad40
#define FUN_005aea00 PortTop_Bound_005aea00
extern "C" {
#include "../../../../re/match/s25/005aa560.c"
}
#endif // MASHED_STANDALONE
