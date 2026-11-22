// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005c9230 = re/match/s28/005c9230.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005c9230 PortTop_Bound_005c9230
#define FUN_005aa060 PortTop_Bound_005aa060
#define FUN_005aae60 PortTop_AStop_005aae60   // named stop
#define FUN_005ac7b0 PortTop_Bound_005ac7b0
#define FUN_005ac7e0 PortTop_Bound_005ac7e0
#define FUN_005aca40 PortTop_Bound_005aca40
#define FUN_005c92c0 PortTop_Bound_005c92c0
#define FUN_005c92f0 PortTop_Bound_005c92f0
#define FUN_005c9310 PortTop_Bound_005c9310
extern "C" {
#include "../../../../re/match/s28/005c9230.c"
}
#endif // MASHED_STANDALONE
