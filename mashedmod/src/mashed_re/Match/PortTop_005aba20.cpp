// RO-AUDIOINIT-45D460-REACH (2026-11-21): exe-only port body of rva 0x005aba20 = re/match/s25/005aba20.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x0045d460 (the permdict loader 0x004669b0 calls at 0x004669f6; RO-AUDIOINIT-45D460-REACH).
// Callees bind to exe bodies by name; a callee with no matched, admitted or listed provisional body is a named stop
// (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005aba20 PortTop_Bound_005aba20
#define FUN_005a7b00 PortTop_Bound_005a7b00
#define FUN_005a7b50 PortTop_Bound_005a7b50
#define FUN_005abcb0 PortTop_Bound_005abcb0
#define FUN_005ac5f0 PortTop_Bound_005ac5f0
#define FUN_005ac980 AudioFmtDescCopy   // exe body (Audio/AudioRwsLeaves.cpp)
#define FUN_005acd10 PortTop_Bound_005acd10
#define FUN_005ae0b0 AudioSubStructZeroInit   // exe body (Audio/AudioRwsLeaves.cpp)
#define FUN_005ae650 PortTop_Bound_005ae650
#define FUN_005ae800 PortTop_Bound_005ae800
#define FUN_005aea00 PortTop_Bound_005aea00
#define FUN_005b3b60 PortTop_Bound_005b3b60
extern "C" {
#include "../../../../re/match/s25/005aba20.c"
}
#endif // MASHED_STANDALONE
