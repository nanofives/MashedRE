// PORT-TOP-AUDIO-5BA1D0-STOPS (2026-11-21): exe-only port body of rva 0x005ba1d0 = re/match/s27/005ba1d0.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x005ba1d0 (census 71, the code pointer 0x005b9f30 stores at +0x28 and 0x005aa560 calls).
// Callees bind to exe bodies by name; a callee with no matched, admitted or listed provisional body is a named stop
// (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005ba1d0 PortTop_Bound_005ba1d0
#define FUN_005ba720 PortTop_Bound_005ba720
#define FUN_005ba760 PortTop_Bound_005ba760
#define FUN_005bac00 PortTop_Bound_005bac00
#define FUN_005baf00 PortTop_Bound_005baf00
#define FUN_005baf60 AudioBufFieldSet   // exe body (Audio/AudioDSound.cpp)
#define FUN_005baf90 AudioDSoundRelease   // exe body (Audio/AudioDSound.cpp)
#define FUN_005bb000 PortTop_Bound_005bb000
#define FUN_005bbc10 PortTop_Bound_005bbc10
#define FUN_005bbd50 PortTop_Bound_005bbd50
#define FUN_005bbdb0 PortTop_Bound_005bbdb0
#define FUN_005bbf30 PortTop_Bound_005bbf30
#define FUN_005bbfc0 PortTop_Bound_005bbfc0
#define FUN_005bc400 AudioDSoundQIChain   // exe body (Audio/AudioDSound.cpp)
#define FUN_005c1920 PortTop_Imp_005c1920   // exe body (Match/PortTop_Imp_005c1920.cpp)
#define FUN_005c7990 PortTop_Bound_005c7990
extern "C" {
#include "../../../../re/match/s27/005ba1d0.c"
}
#endif // MASHED_STANDALONE
