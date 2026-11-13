// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x004671a0 = re/match/s11/004671a0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004671a0 PortTop_Bound_004671a0
#define FUN_0042b930 PortTop_AStop_0042b930   // named stop
#define FUN_0042f510 Vehicle0HandleGet   // exe body (Match/MatchS07_0042f510.cpp)
extern "C" {
#include "../../../../re/match/s11/004671a0.c"
}
#endif // MASHED_STANDALONE
