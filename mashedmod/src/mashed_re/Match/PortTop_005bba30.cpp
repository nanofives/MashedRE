// RO-AUDIOINIT-45D460-REACH (2026-11-21): exe-only port body of rva 0x005bba30 = re/match/s27/005bba30.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x0045d460 (the permdict loader 0x004669b0 calls at 0x004669f6; RO-AUDIOINIT-45D460-REACH).
// Callees bind to exe bodies by name; a callee with no matched, admitted or listed provisional body is a named stop
// (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005bba30 PortTop_Bound_005bba30
#define FUN_005bbf30 PortTop_Bound_005bbf30
#define FUN_005c9220 PortTop_Bound_005c9220
extern "C" {
#include "../../../../re/match/s27/005bba30.c"
}
#endif // MASHED_STANDALONE
