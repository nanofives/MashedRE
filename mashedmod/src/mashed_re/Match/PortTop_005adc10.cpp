// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005adc10 = re/match/s26/005adc10.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005adc10 PortTop_Bound_005adc10
#define FUN_005a61b0 PortTop_AStop_005a61b0   // named stop
#define FUN_005ad770 PortTop_Bound_005ad770
#define FUN_005ad910 PortTop_AStop_005ad910   // named stop
#define FUN_005ad980 PortTop_Bound_005ad980
#define FUN_005ad9d0 PortTop_AStop_005ad9d0   // named stop
#define FUN_005ada40 PortTop_Bound_005ada40
#define FUN_005adfe0 PortTop_Bound_005adfe0
#define FUN_005ae010 PortTop_Bound_005ae010
#define FUN_005b3620 PortTop_Bound_005b3620
extern "C" {
#include "../../../../re/match/s26/005adc10.c"
}
#endif // MASHED_STANDALONE
