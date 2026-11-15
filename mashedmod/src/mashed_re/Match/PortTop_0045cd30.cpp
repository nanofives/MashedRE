// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x0045cd30 = re/match/s10/0045cd30.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0045cd30 PortTop_Bound_0045cd30
#define FUN_0045c720 PortTop_Bound_0045c720
#define FUN_004b44f0 PortTop_Bound_004b44f0
#define FUN_0045c820 PortTop_Local_0045cd30_0045c820   // file-local in this source
extern "C" {
#include "../../../../re/match/s10/0045cd30.c"
}
#endif // MASHED_STANDALONE
