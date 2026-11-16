// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005a8060 = re/match/s25/005a8060.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005a8060 PortTop_Bound_005a8060
#define FUN_005a8390 PortTop_Bound_005a8390
#define FUN_005a8460 PortTop_AStop_005a8460   // named stop
#define FUN_005a84d0 PortTop_AStop_005a84d0   // named stop
#define FUN_005a8560 PortTop_Bound_005a8560
#define FUN_005a8680 PortTop_AStop_005a8680   // named stop
#define FUN_005a9e10 PortTop_Bound_005a9e10
#define FUN_005aee20 PortTop_Bound_005aee20
#define FUN_005aeea0 AudioSemaphoreCreate   // exe body (Audio/AudioDSound.cpp)
#define FUN_005aef00 AudioThreadDescInit   // exe body (Audio/AudioDSound.cpp)
#define FUN_005aef30 PortTop_Knob_005aef30   // knob stand-in, default off = the named stop (Match/PortTop_Knob_005aef30.cpp)
extern "C" {
#include "../../../../re/match/s25/005a8060.c"
}
#endif // MASHED_STANDALONE
