// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x005a7520 = re/match/s25/005a7520.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005a7520 PortTop_Bound_005a7520
#define FUN_005a7460 PortTop_Bound_005a7460
#define FUN_005a7560 PortTop_Bound_005a7560
#define FUN_005a75b0 PortTop_AStop_005a75b0   // named stop
extern "C" {
#include "../../../../re/match/s25/005a7520.c"
}
#endif // MASHED_STANDALONE
