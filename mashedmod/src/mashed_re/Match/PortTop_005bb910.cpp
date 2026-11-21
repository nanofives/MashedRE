// RO-AUDIOINIT-45D460-REACH (2026-11-21): exe-only port body of rva 0x005bb910 = re/match/s27/005bb910.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x0045d460 (the permdict loader 0x004669b0 calls at 0x004669f6; RO-AUDIOINIT-45D460-REACH).
// Callees bind to exe bodies by name; a callee with no matched, admitted or listed provisional body is a named stop
// (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005bb910 PortTop_Bound_005bb910
#define FUN_005a7e70 PortTop_Bound_005a7e70
#define FUN_005ac540 PortTop_AStop_005ac540   // named stop
#define FUN_005bbdb0 PortTop_Bound_005bbdb0
#define FUN_005c91f0 PortTop_AStop_005c91f0   // named stop: re/match/s10/0045cbe0.c carries a second copy of 0x0045c4e0 (one exe body per rva)
extern "C" {
#include "../../../../re/match/s27/005bb910.c"
}
#endif // MASHED_STANDALONE
