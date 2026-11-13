// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x005b1110 = re/match/s26/005b1110.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005b1110 PortTop_Bound_005b1110
#define FUN_005b10e0 AudioTableDispatchAt8   // exe body (Audio/AudioTableDispatch.cpp)
extern "C" {
#include "../../../../re/match/s26/005b1110.c"
}
#endif // MASHED_STANDALONE
