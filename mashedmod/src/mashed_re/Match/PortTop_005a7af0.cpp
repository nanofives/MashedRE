// PORT-TOP-AUDIO-BODY-45D460 (2026-11-15): exe-only port body of rva 0x005a7af0 = re/match/s25/005a7af0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through 0x004669b0's call to 0x0045d460 at 0x004669f6 (PERMDICT.RWS loader and its callees, closure generator
// verify/portorigtop_20261113/tools/audiotick_bind_gen.py). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005a7af0 PortTop_Bound_005a7af0
#define FUN_005aded0 AudioListNodeCount   // exe body (Audio/AudioLeaves_ab1.cpp)
extern "C" {
#include "../../../../re/match/s25/005a7af0.c"
}
#endif // MASHED_STANDALONE
