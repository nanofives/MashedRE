// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x0045d3a0 = re/match/s10/0045d3a0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0045d3a0 PortTop_Bound_0045d3a0
#define FUN_0040e350 GetRenderSubMode   // exe body (Match/MatchS02_0040e350.cpp)
#define FUN_0042f6a0 GetRaceSubMode   // exe body (Match/MatchS07_0042f6a0.cpp)
#define FUN_0045c810 PortTop_Bound_0045c810
#define FUN_0045d1e0 PortTop_Bound_0045d1e0
#define FUN_0045d330 PortTop_Bound_0045d330
#define FUN_004657b0 PortTop_Bound_004657b0
extern "C" {
#include "../../../../re/match/s10/0045d3a0.c"
}
#endif // MASHED_STANDALONE
