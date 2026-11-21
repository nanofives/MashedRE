// RO-AUDIOINIT-45D460-REACH (2026-11-21): exe-only port body of rva 0x005ac770 = re/match/tlr/005ac770.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x0045d460 (the permdict loader 0x004669b0 calls at 0x004669f6; RO-AUDIOINIT-45D460-REACH).
// Callees bind to exe bodies by name; a callee with no matched, admitted or listed provisional body is a named stop
// (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005ac770 PortTop_Bound_005ac770
extern "C" {
#include "../../../../re/match/tlr/005ac770.c"
}
#endif // MASHED_STANDALONE
