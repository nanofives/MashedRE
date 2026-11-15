// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x0040e500 = re/match/s02/0040e500.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040e500 PortTop_Bound_0040e500
#define FUN_0040b6d0 UtilArrayGet8a94e0   // exe body (Match/MatchS02_0040b6d0.cpp)
#define FUN_0042f500 GetDat0067ea64   // exe body (Match/MatchS07_0042f500.cpp)
extern "C" {
#include "../../../../re/match/s02/0040e500.c"
}
#endif // MASHED_STANDALONE
