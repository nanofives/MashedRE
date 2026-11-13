// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x005a6f30 = re/match/s25/005a6f30.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005a6f30 PortTop_Bound_005a6f30
#define FUN_005a6d60 PortTop_Bound_005a6d60
#define FUN_005a6dc0 PortTop_Bound_005a6dc0
#define FUN_005a6df0 PortTop_Bound_005a6df0
#define FUN_005a6e10 PortTop_Bound_005a6e10
#define FUN_005a7460 PortTop_Bound_005a7460
#define FUN_005acda0 PortTop_Bound_005acda0
extern "C" {
#include "../../../../re/match/s25/005a6f30.c"
}
#endif // MASHED_STANDALONE
