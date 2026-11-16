// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005b9f30 = re/match/s27/005b9f30.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005b9f30 PortTop_Bound_005b9f30
#define FUN_005a9e10 PortTop_Bound_005a9e10
#define FUN_005aee20 PortTop_Bound_005aee20
#define FUN_005ba1d0 PortTop_AStop_005ba1d0   // named stop
#define FUN_005ba780 PortTop_Bound_005ba780
#define FUN_005ba7f0 PortTop_AStop_005ba7f0   // named stop
#define FUN_005bad30 PortTop_AStop_005bad30   // named stop
extern "C" {
#include "../../../../re/match/s27/005b9f30.c"
}
#endif // MASHED_STANDALONE
