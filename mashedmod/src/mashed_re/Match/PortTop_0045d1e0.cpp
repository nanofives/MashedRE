// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x0045d1e0 = re/match/s10/0045d1e0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0045d1e0 PortTop_Bound_0045d1e0
#define FUN_0045c880 PortTop_Bound_0045c880
#define FUN_0045cbe0 PortTop_AStop_0045cbe0   // named stop: re/match/s10/0045cbe0.c carries a second copy of 0x0045c4e0 (one exe body per rva)
#define FUN_0045cd30 PortTop_AStop_0045cd30   // named stop
#define FUN_0045d0e0 PortTop_Bound_0045d0e0
#define FUN_004656e0 PortTop_Bound_004656e0
extern "C" {
#include "../../../../re/match/s10/0045d1e0.c"
}
#endif // MASHED_STANDALONE
