// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x0045cbe0 = re/match/s10/0045cbe0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0045cbe0 PortTop_Bound_0045cbe0
#define FUN_0045c640 PortTop_Bound_0045c640   // exe body (Match/PortTop_0045c6b0.cpp)
#define FUN_0045c4e0 PortTop_Local_0045cb20_0045c4e0   // the one exe copy (Match/PortTop_0045cb20.cpp); this source compiles its copy out
#define MATCH_PORT_SHARED_0045c4e0 1   // the exe copy of 0x0045c4e0 is the one in Match/PortTop_0045cb20.cpp
extern "C" {
#include "../../../../re/match/s10/0045cbe0.c"
}
#endif // MASHED_STANDALONE
