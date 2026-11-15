// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x0044b000 = re/match/s08/0044b000.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0044b000 PortTop_Bound_0044b000
#define FUN_0047d150 PortTop_Bound_0047d150
extern "C" {
#include "../../../../re/match/s08/0044b000.c"
}
#endif // MASHED_STANDALONE
