// RO-AUDIOTICK-BODIES (RO-BIND-TIMER follow-up): exe-only port body of rva 0x0045d3f0 = the byte-matched source
// re/match/s10/0045d3f0.c (match.py BYTE-IDENTICAL 53 B, relocheck OK checked=5; hooks.csv C4 c4=match, src sha256 a629f43ada77542c).
// Its exe caller is the matched 0x004926c0 AudioTickAndAvg (Match/MatchS15_004926c0.cpp), run under MASHED_ORIG_TOP with
// MASHED_OT_AUDIOTICK=1 (Match/PortTop_00492370.cpp). The .asi keeps its own binding (raw-address call or L2 body).
// The body returns at once while DAT_006904e8 == 0. That word is .bss (0 at load) and its only writer is 0x004669b0
// (re/match/s10/004669b0.c: DAT_006904e8 = 1), which the exe runs as the hand stand-in piece 3 (HandTop_InitAudio,
// Match/PortTop_00402750_Pieces.cpp) that never writes it. So on the exe path no callee below is reached. Each callee
// binds to a named stop (log + exit code = rva, PortTop_AudioCalleeStop in Match/PortTop_00492370.cpp): the exe has
// no body for them, and a stop makes any change of that gate visible instead of calling into unmapped code.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <intrin.h>
#define MATCH_PORT 1
#define FUN_0045d3f0 PortTop_Bound_0045d3f0
#define FUN_00550be0 PortTop_AStop_00550be0
#define FUN_005a60b0 PortTop_AStop_005a60b0
extern "C" {
#include "../../../../re/match/s10/0045d3f0.c"
void PortTop_AudioCalleeStop(unsigned rva, unsigned caller, void* ret);   // Match/PortTop_00492370.cpp
void PortTop_AStop_00550be0(void) { PortTop_AudioCalleeStop(0x00550be0u, 0x0045d3f0u, _ReturnAddress()); }
void PortTop_AStop_005a60b0(void) { PortTop_AudioCalleeStop(0x005a60b0u, 0x0045d3f0u, _ReturnAddress()); }
}
#endif // MASHED_STANDALONE
