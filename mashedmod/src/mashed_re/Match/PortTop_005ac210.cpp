// RO-AUDIOINIT-45D460-REACH (2026-11-21): exe-only port body of rva 0x005ac210 = re/match/s25/005ac210.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x0045d460 (the permdict loader 0x004669b0 calls at 0x004669f6; RO-AUDIOINIT-45D460-REACH).
// Callees bind to exe bodies by name; a callee with no matched, admitted or listed provisional body is a named stop
// (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005ac210 PortTop_Bound_005ac210
#define FUN_005aa060 PortTop_Bound_005aa060
#define FUN_005aa0c0 PortTop_Bound_005aa0c0
#define FUN_005aaaa0 PortTop_Bound_005aaaa0
#define FUN_005aaac0 PortTop_Bound_005aaac0
#define FUN_005aba20 PortTop_Bound_005aba20
#define FUN_005ac770 PortTop_Bound_005ac770
#define FUN_005ac7b0 PortTop_Bound_005ac7b0
#define FUN_005ac900 PortTop_Bound_005ac900
#define FUN_005ac980 AudioFmtDescCopy   // exe body (Audio/AudioRwsLeaves.cpp)
#define FUN_005aca80 AudioFmtSizeCalc   // exe body (Audio/AudioRwsLeaves.cpp)
#define FUN_005acaa0 PortTop_Bound_005acaa0
#define FUN_005acd10 PortTop_Bound_005acd10
#define FUN_005adf30 AudioFmtKeyCompare   // exe body (Audio/AudioFmtKey.cpp)
#define FUN_005ae0c0 PortTop_Bound_005ae0c0
#define FUN_005aea00 PortTop_Bound_005aea00
#define FUN_005aec00 AudioByteReverse   // exe body (Audio/AudioMemory.cpp)
extern "C" {
#include "../../../../re/match/s25/005ac210.c"
}
#endif // MASHED_STANDALONE
