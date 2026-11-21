// RO-AUDIOINIT-45D460-REACH (2026-11-21): exe-only port body of rva 0x005ad0b0 = re/match/s26/005ad0b0.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x0045d460 (the permdict loader 0x004669b0 calls at 0x004669f6; RO-AUDIOINIT-45D460-REACH).
// Callees bind to exe bodies by name; a callee with no matched, admitted or listed provisional body is a named stop
// (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005ad0b0 PortTop_Bound_005ad0b0
#define FUN_005ad320 PortTop_Bound_005ad320
#define FUN_005adf30 AudioFmtKeyCompare   // exe body (Audio/AudioFmtKey.cpp)
#define FUN_005aea00 PortTop_Bound_005aea00
extern "C" {
#include "../../../../re/match/s26/005ad0b0.c"
}
#endif // MASHED_STANDALONE
