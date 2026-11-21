// PORT-TOP-AUDIO-5BA1D0-STOPS (2026-11-21): exe-only port body of rva 0x005bf670 = re/match/s28/005bf670.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x005ba1d0 (census 71, the code pointer 0x005b9f30 stores at +0x28 and 0x005aa560 calls).
// Callees bind to exe bodies by name; a callee with no matched, admitted or listed provisional body is a named stop
// (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005bf670 PortTop_Bound_005bf670
extern "C" {
#include "../../../../re/match/s28/005bf670.c"
}
#endif // MASHED_STANDALONE
