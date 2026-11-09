// MATCH-S11: port body of 0x004671a0 = the byte-matched source re/match/s11/004671a0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/IntroSplash.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).


#define MATCH_PORT 1
#define FUN_004671a0 BackBuffRasterOpen
// [PORT-TOP-04B] exe: callees by name (DisplayInit / ViewportInit callees on the MASHED_ORIG_TOP path). The .asi keeps
// the raw-address calls.
#ifdef MASHED_STANDALONE
#define FUN_0042b930 MenuAlphaGet        // matched, Match/MatchS07_0042b930.cpp
#define FUN_0042f510 Vehicle0HandleGet   // matched, Match/MatchS07_0042f510.cpp
#else
#define FUN_0042b930 MatchS11_004671a0_Call0042b930
#define FUN_0042f510 MatchS11_004671a0_Call0042f510
// Callees: the old Frontend/IntroSplash.cpp (OrigVehicle0Get called 0x004671a0 itself, whose original body calls them)
// reached 0x0042b930 / 0x0042f510 through their original addresses, so each forwarder makes that same call.
extern "C" unsigned int MatchS11_004671a0_Call0042b930(void) {
    return reinterpret_cast<unsigned int (__cdecl*)()>(0x0042b930u)();
}
extern "C" unsigned int MatchS11_004671a0_Call0042f510(void) {
    return reinterpret_cast<unsigned int (__cdecl*)()>(0x0042f510u)();
}
#endif
#pragma comment(linker, "/EXPORT:BackBuffRasterOpen=_BackBuffRasterOpen")
extern "C" {
#include "../../../../re/match/s11/004671a0.c"
}
