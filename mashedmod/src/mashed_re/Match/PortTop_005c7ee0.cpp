// PORT-TOP-AUDIO-5BA1D0-STOPS (2026-11-21): exe-only port body of rva 0x005c7ee0 = re/match/s28/005c7ee0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x005ba1d0 (census 71, the code pointer 0x005b9f30 stores at +0x28 and 0x005aa560 calls).
// Callees bind to exe bodies by name; a callee with no matched, admitted or listed provisional body is a named stop
// (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005c7ee0 PortTop_Bound_005c7ee0
#define FUN_005ab040 PortTop_Bound_005ab040
#define FUN_005c9740 PortTop_Bound_005c9740
#define FUN_005c98d0 PortTop_AStop_005c98d0   // named stop
#define FUN_005c9d10 PortTop_AStop_005c9d10   // named stop
extern "C" {
#include "../../../../re/match/s28/005c7ee0.c"
}
#endif // MASHED_STANDALONE
