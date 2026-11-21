// RO-AUDIOINIT-45D460-REACH (2026-11-21): exe-only port body of rva 0x005abd30 = re/match/s25/005abd30.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x0045d460 (the permdict loader 0x004669b0 calls at 0x004669f6; RO-AUDIOINIT-45D460-REACH).
// Callees bind to exe bodies by name; a callee with no matched, admitted or listed provisional body is a named stop
// (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005abd30 PortTop_Bound_005abd30
#define FUN_005abf80 AudioWaveVtableSlot1cDispatch   // exe body (Audio/RwsStream.cpp)
#define FUN_005ac5f0 PortTop_Bound_005ac5f0
#define FUN_005aea10 PortTop_Bound_005aea10
#define FUN_005aea40 PortTop_Bound_005aea40
#define FUN_005b3b30 PortTop_Bound_005b3b30
#define FUN_005b3b60 PortTop_Bound_005b3b60
#define FUN_005b3b80 PortTop_Bound_005b3b80
extern "C" {
#include "../../../../re/match/s25/005abd30.c"
}
#endif // MASHED_STANDALONE
