// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005ade90 = re/match/s26/005ade90.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005ade90 PortTop_Bound_005ade90
#define FUN_005ae920 PortTop_Bound_005ae920
extern "C" {
#include "../../../../re/match/s26/005ade90.c"
}
#endif // MASHED_STANDALONE
