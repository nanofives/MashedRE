// PORT-TOP-AUDIO-BODY-45D460 (2026-11-15): exe-only port body of rva 0x0045d460 = re/match/s10/0045d460.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through 0x004669b0's call to 0x0045d460 at 0x004669f6 (PERMDICT.RWS loader and its callees, closure generator
// verify/portorigtop_20261113/tools/audiotick_bind_gen.py). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0045d460 PortTop_Bound_0045d460
#define FUN_004a2cbd PortTop_Crt_004a2cbd   // exe body (Match/PortTop_00493710.cpp)
#define FUN_004cc160 RwCore_004cc160   // exe body (RwCore/RwCore_004cc160.cpp)
#define FUN_004cc230 RwCore_004cc230   // exe body (RwCore/RwCore_004cc230.cpp)
#define FUN_004cc5e0 RwCore_004cc5e0   // exe body (RwCore/RwCore_004cc5e0.cpp)
#define FUN_005a6280 PortTop_Bound_005a6280
#define FUN_005a6710 PortTop_Bound_005a6710
#define FUN_005a6dc0 PortTop_Bound_005a6dc0
#define FUN_005a71f0 PortTop_AStop_005a71f0   // named stop
#define FUN_005a7aa0 PortTop_Bound_005a7aa0
#define FUN_005a7af0 PortTop_Bound_005a7af0
#define FUN_005a7b60 PortTop_AStop_005a7b60   // named stop
#define FUN_005b8570 PortTop_Bound_005b8570
extern "C" {
#include "../../../../re/match/s10/0045d460.c"
}
#endif // MASHED_STANDALONE
