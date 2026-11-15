// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x0045d250 = re/match/pilot/a4_0045d250.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0045d250 PortTop_Bound_0045d250
#define FUN_0040e350 GetRenderSubMode   // exe body (Match/MatchS02_0040e350.cpp)
#define FUN_0040e4b0 PortTop_Bound_0040e4b0
#define FUN_0040e500 PortTop_Bound_0040e500
#define FUN_0045ca30 PortTop_Bound_0045ca30
#define FUN_0045cab0 PortTop_Bound_0045cab0
#define FUN_0045cb20 PortTop_Bound_0045cb20
#define FUN_0045d080 PortTop_Bound_0045d080
// The source declares its globals extern under #ifndef MATCH_PORT; the port reads them at their original addresses.
#define DAT_007f0fd0 (*(int *)0x007f0fd0u)
#define DAT_0088fbc0 (*(int *)0x0088fbc0u)
extern "C" {
#include "../../../../re/match/pilot/a4_0045d250.c"
}
#endif // MASHED_STANDALONE
