// RO-AUDIOINIT-45D460-REACH (2026-11-21): exe-only port body of rva 0x005ab410 = re/match/s25/005ab410.c
// (L-126 admitted (re/match/ADMITTED.tsv, EQUIV)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x0045d460 (the permdict loader 0x004669b0 calls at 0x004669f6; RO-AUDIOINIT-45D460-REACH).
// Callees bind to exe bodies by name; a callee with no matched, admitted or listed provisional body is a named stop
// (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005ab410 PortTop_Bound_005ab410
#define FUN_004cc050 RwCore_004cc050   // exe body (RwCore/RwCore_004cc050.cpp)
#define FUN_005ab380 PortTop_AStop_005ab380   // named stop
extern "C" {
#include "../../../../re/match/s25/005ab410.c"
}
#endif // MASHED_STANDALONE
