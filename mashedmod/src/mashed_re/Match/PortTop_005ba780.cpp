// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005ba780 = re/match/s27/005ba780.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005ba780 PortTop_Bound_005ba780
#define FUN_005aaa00 PortTop_Bound_005aaa00
#define FUN_005ba760 PortTop_Bound_005ba760
#define FUN_005baf40 AudioRendererField3cSet   // exe body (Audio/AudioLeaves_ab3.cpp)
#define FUN_005bb000 PortTop_Bound_005bb000
#define FUN_005bbf30 PortTop_Bound_005bbf30
extern "C" {
#include "../../../../re/match/s27/005ba780.c"
}
#endif // MASHED_STANDALONE
