// RO-AUDIOINIT-45D460-REACH (2026-11-21): exe-only port body of rva 0x005a7b60 = re/match/s25/005a7b60.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x0045d460 (the permdict loader 0x004669b0 calls at 0x004669f6; RO-AUDIOINIT-45D460-REACH).
// Callees bind to exe bodies by name; a callee with no matched, admitted or listed provisional body is a named stop
// (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005a7b60 PortTop_Bound_005a7b60
#define FUN_004522d0 RwCore_004522d0   // exe body (RwCore/RwCore_004522d0.cpp)
#define FUN_004cbd30 RwCore_004cbd30   // exe body (RwCore/RwCore_004cbd30.cpp)
#define FUN_004cc050 RwCore_004cc050   // exe body (RwCore/RwCore_004cc050.cpp)
#define FUN_005a79a0 PortTop_Bound_005a79a0
#define FUN_005a7b40 PortTop_AStop_005a7b40   // named stop
#define FUN_005a7b50 PortTop_Bound_005a7b50
#define FUN_005a7ee0 PortTop_Bound_005a7ee0
#define FUN_005ab380 PortTop_AStop_005ab380   // named stop
#define FUN_005ab410 PortTop_Bound_005ab410
#define FUN_005abcf0 PortTop_Bound_005abcf0
#define FUN_005abfa0 PortTop_AStop_005abfa0   // named stop
#define FUN_005ade10 PortTop_Bound_005ade10
#define FUN_005aea00 PortTop_Bound_005aea00
#define FUN_005aea10 PortTop_Bound_005aea10
#define FUN_005aea40 PortTop_Bound_005aea40
#define FUN_005aec00 AudioByteReverse   // exe body (Audio/AudioMemory.cpp)
extern "C" {
#include "../../../../re/match/s25/005a7b60.c"
}
#endif // MASHED_STANDALONE
