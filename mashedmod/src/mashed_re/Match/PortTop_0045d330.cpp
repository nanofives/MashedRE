// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x0045d330 = re/match/s10/0045d330.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0045d330 PortTop_Bound_0045d330
#define FUN_0040e350 GetRenderSubMode   // exe body (Match/MatchS02_0040e350.cpp)
#define FUN_0045c6b0 PortTop_Bound_0045c6b0
#define FUN_0045c7f0 PortTop_Bound_0045c7f0
#define FUN_0045cc50 PortTop_Bound_0045cc50
#define FUN_0045cc80 PortTop_Bound_0045cc80
#define FUN_0045cfe0 PortTop_Bound_0045cfe0
#define FUN_0045d250 PortTop_Bound_0045d250
#define FUN_004b44f0 PortTop_Bound_004b44f0
extern "C" {
#include "../../../../re/match/s10/0045d330.c"
}
#endif // MASHED_STANDALONE
