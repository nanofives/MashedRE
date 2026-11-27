// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005b8910 = re/match/s27/005b8910.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005b8910 PortTop_Bound_005b8910
#define FUN_005aeb90 PortTop_Bound_005aeb90
#define FUN_005b8c40 PortTop_Bound_005b8c40
#define FUN_005b9460 PortTop_Bound_005b9460
#define FUN_005c6bb0 PortTop_Bound_005c6bb0
extern "C" {
#include "../../../../re/match/s27/005b8910.c"
}
#endif // MASHED_STANDALONE
