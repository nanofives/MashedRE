// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x0045cb20 = re/match/s10/0045cb20.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0045cb20 PortTop_Bound_0045cb20
#define FUN_0045c640 PortTop_Bound_0045c640   // exe body (Match/PortTop_0045c6b0.cpp)
#define FUN_0045caf0 PortTop_AStop_0045caf0   // named stop
#define FUN_0045c4e0 PortTop_Local_0045cb20_0045c4e0   // file-local in this source
#define FUN_0045c510 PortTop_Local_0045cb20_0045c510   // file-local in this source
#define FUN_0045c550 PortTop_Local_0045cb20_0045c550   // file-local in this source
extern "C" {
#include "../../../../re/match/s10/0045cb20.c"
}
#endif // MASHED_STANDALONE
