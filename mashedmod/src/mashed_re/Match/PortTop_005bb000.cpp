// PORT-TOP-AUDIO-5BA1D0-STOPS (2026-11-21): exe-only port body of rva 0x005bb000 = re/match/s27/005bb000.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x005ba1d0 (census 71, the code pointer 0x005b9f30 stores at +0x28 and 0x005aa560 calls).
// Callees bind to exe bodies by name; a callee with no matched, admitted or listed provisional body is a named stop
// (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005bb000 PortTop_Bound_005bb000
#define FUN_004522d0 RwCore_004522d0   // exe body (RwCore/RwCore_004522d0.cpp)
#define FUN_005aea50 PortTop_Bound_005aea50
#define FUN_005aeea0 AudioSemaphoreCreate   // exe body (Audio/AudioDSound.cpp)
#define FUN_005aef00 AudioThreadDescInit   // exe body (Audio/AudioDSound.cpp)
#define FUN_005aef30 PortTop_Knob_005aef30   // knob stand-in, default off = the named stop (Match/PortTop_Knob_005aef30.cpp)
#define FUN_005bb380 PortTop_AStop_005bb380   // named stop
#define FUN_005bbdb0 PortTop_Bound_005bbdb0
#define FUN_005bbf30 PortTop_Bound_005bbf30
#define FUN_005bc470 PortTop_Bound_005bc470
#define FUN_005bc640 PortTop_Bound_005bc640
extern "C" {
#include "../../../../re/match/s27/005bb000.c"
}
#endif // MASHED_STANDALONE
