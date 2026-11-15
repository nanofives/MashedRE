// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005aea60 = re/match/s30/005aea60.c
// (L-126 admitted (re/match/ADMITTED.tsv, EQUIV)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005aea60 PortTop_Bound_005aea60
#define FUN_004a460d MashedCrt_free_004a460d   // exe body (Match/PortTop_CrtHeap.cpp)
#define FUN_004ccef0 PortTop_AStop_004ccef0   // named stop
#define FUN_005aead0 PortTop_AStop_005aead0   // named stop
#define FUN_005aeaf0 PortTop_Bound_005aeaf0
extern "C" {
#include "../../../../re/match/s30/005aea60.c"
}
#endif // MASHED_STANDALONE
