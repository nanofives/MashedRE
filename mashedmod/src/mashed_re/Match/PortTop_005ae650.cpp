// PORT-TOP-AUDIOPOOL-5AE650 (2026-11-14): exe-only port body of rva 0x005ae650 = re/match/s26/005ae650.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005ae650 PortTop_Bound_005ae650
#define FUN_005ae780 PortTop_Bound_005ae780
#define FUN_005ae800 PortTop_Bound_005ae800
#define FUN_005aea00 PortTop_Bound_005aea00
extern "C" {
#include "../../../../re/match/s26/005ae650.c"
}
#endif // MASHED_STANDALONE
