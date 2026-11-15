// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x004647f0 = re/match/s10/004647f0.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004647f0 PortTop_Bound_004647f0
#define FUN_0042b8d0 RwCore_0042b8d0   // exe body (RwCore/RwCore_0042b8d0.cpp)
#define FUN_0045e0f0 PortTop_Bound_0045e0f0
extern "C" {
#include "../../../../re/match/s10/004647f0.c"
}
#endif // MASHED_STANDALONE
