// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005aa480 = re/match/s30/005aa480.c
// (L-126 admitted (re/match/ADMITTED.tsv, EQUIV)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005aa480 PortTop_Bound_005aa480
#define FUN_005aa510 PortTop_AStop_005aa510   // named stop
#define FUN_005aa560 PortTop_AStop_005aa560   // named stop
#define FUN_005ae650 PortTop_AStop_005ae650   // named stop
#define FUN_005ae7e0 PortTop_Bound_005ae7e0
extern "C" {
#include "../../../../re/match/s30/005aa480.c"
}
#endif // MASHED_STANDALONE
