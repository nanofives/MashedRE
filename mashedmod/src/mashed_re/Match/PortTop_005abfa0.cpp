// RO-AUDIOINIT-45D460-REACH (2026-11-21): exe-only port body of rva 0x005abfa0 = re/match/s25/005abfa0.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x0045d460 (the permdict loader 0x004669b0 calls at 0x004669f6; RO-AUDIOINIT-45D460-REACH).
// Callees bind to exe bodies by name; a callee with no matched, admitted or listed provisional body is a named stop
// (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005abfa0 PortTop_Bound_005abfa0
#define FUN_004522d0 RwCore_004522d0   // exe body (RwCore/RwCore_004522d0.cpp)
#define FUN_004cbd30 RwCore_004cbd30   // exe body (RwCore/RwCore_004cbd30.cpp)
#define FUN_005ab380 PortTop_Bound_005ab380
#define FUN_005abcf0 PortTop_Bound_005abcf0
#define FUN_005abd30 PortTop_Bound_005abd30
#define FUN_005abf80 AudioWaveVtableSlot1cDispatch   // exe body (Audio/RwsStream.cpp)
#define FUN_005ac210 PortTop_Bound_005ac210
#define FUN_005adf30 AudioFmtKeyCompare   // exe body (Audio/AudioFmtKey.cpp)
#define FUN_005aea00 PortTop_Bound_005aea00
#define FUN_005aea10 PortTop_Bound_005aea10
#define FUN_005aea40 PortTop_Bound_005aea40
#define FUN_005aec30 PortTop_Bound_005aec30
extern "C" {
#include "../../../../re/match/s25/005abfa0.c"
}
#endif // MASHED_STANDALONE
