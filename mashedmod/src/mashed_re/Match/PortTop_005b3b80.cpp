// RO-AUDIOINIT-45D460-REACH (2026-11-21): exe-only port body of rva 0x005b3b80 = re/match/s27/005b3b80.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x0045d460 (the permdict loader 0x004669b0 calls at 0x004669f6; RO-AUDIOINIT-45D460-REACH).
// Callees bind to exe bodies by name; a callee with no matched, admitted or listed provisional body is a named stop
// (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005b3b80 PortTop_Bound_005b3b80
#define FUN_004522d0 RwCore_004522d0   // exe body (RwCore/RwCore_004522d0.cpp)
#define FUN_005ac5f0 PortTop_Bound_005ac5f0
#define FUN_005ac740 PortTop_Bound_005ac740
#define FUN_005ac980 AudioFmtDescCopy   // exe body (Audio/AudioRwsLeaves.cpp)
#define FUN_005aea50 PortTop_Bound_005aea50
#define FUN_005b36f0 PortTop_Bound_005b36f0
#define FUN_005b3760 PortTop_Bound_005b3760
extern "C" {
#include "../../../../re/match/s27/005b3b80.c"
}
#endif // MASHED_STANDALONE
