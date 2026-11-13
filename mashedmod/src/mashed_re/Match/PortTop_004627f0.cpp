// PORT-TOP-AUDIO-HWINIT-4627F0 (2026-11-13): exe-only port body of rva 0x004627f0 = re/match/s10/004627f0.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the closure of 0x004627f0 (sound engine HW init, called by 0x004669b0). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004627f0 PortTop_Bound_004627f0
#define FUN_004624c0 PortTop_Bound_004624c0
#define FUN_004963e0 Orig_004963e0   // exe body (Match/PortTop_004963e0.cpp)
#define FUN_004a2b60 MashedCrt_sprintf_004a2b60   // exe body (Match/PortTop_CrtStdio.cpp)
#define FUN_005a5f00 PortTop_Bound_005a5f00
#define FUN_005a6110 PortTop_Bound_005a6110
#define FUN_005a8e70 PortTop_Bound_005a8e70
#define FUN_005a9e40 AudioOutputNodeCbDispatch   // exe body (Audio/AudioDSound.cpp)
#define FUN_005aa060 PortTop_Bound_005aa060
#define FUN_005b9e30 PortTop_Bound_005b9e30
#define FUN_005baf40 AudioRendererField3cSet   // exe body (Audio/AudioLeaves_ab3.cpp)
#define FUN_005baf60 AudioBufFieldSet   // exe body (Audio/AudioDSound.cpp)
#define FUN_005bbb20 PortTop_Bound_005bbb20
extern "C" {
#include "../../../../re/match/s10/004627f0.c"
}
#endif // MASHED_STANDALONE
