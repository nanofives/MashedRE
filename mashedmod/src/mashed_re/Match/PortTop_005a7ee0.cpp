// RO-AUDIOINIT-45D460-REACH (2026-11-21): exe-only port body of rva 0x005a7ee0 = re/match/s25/005a7ee0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x0045d460 (the permdict loader 0x004669b0 calls at 0x004669f6; RO-AUDIOINIT-45D460-REACH).
// Callees bind to exe bodies by name; a callee with no matched, admitted or listed provisional body is a named stop
// (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005a7ee0 PortTop_Bound_005a7ee0
#define FUN_005adfe0 PortTop_Bound_005adfe0
#define FUN_005ae010 PortTop_Bound_005ae010
#define FUN_005ae0c0 PortTop_Bound_005ae0c0
extern "C" {
#include "../../../../re/match/s25/005a7ee0.c"
}
#endif // MASHED_STANDALONE
