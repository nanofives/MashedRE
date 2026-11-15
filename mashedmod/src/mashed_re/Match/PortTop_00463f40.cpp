// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x00463f40 = re/match/s10/00463f40.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00463f40 PortTop_Bound_00463f40
#define FUN_0040e350 GetRenderSubMode   // exe body (Match/MatchS02_0040e350.cpp)
#define FUN_00426c00 GameStateFlagGet   // exe body (Match/MatchS06_00426c00.cpp)
#define FUN_0045daf0 PortTop_Bound_0045daf0
#define FUN_0045e0f0 PortTop_Bound_0045e0f0
#define FUN_0046cc10 VehicleFloatFieldAsInt   // exe body (Match/MatchS11_0046cc10.cpp)
#define FUN_0046d2e0 PortTop_Bound_0046d2e0
#define FUN_0046d6d0 PortTop_Bound_0046d6d0
#define FUN_00472650 PortTop_Bound_00472650
#define FUN_005a66d0 PortTop_Bound_005a66d0
#define FUN_005a6dc0 PortTop_Bound_005a6dc0
extern "C" {
#include "../../../../re/match/s10/00463f40.c"
}
#endif // MASHED_STANDALONE
