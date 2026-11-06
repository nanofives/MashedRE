// MATCH-S15: port body of 0x00494480 = the byte-matched source re/match/s15/00494480.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/IntroSplash.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).


// Raw-address forwarder: the old body was a call into the original 0x00494480 itself, which calls
// FUN_004942b0 through its address (no port body on this branch).
extern "C" void MatchS15_00494480_Call004942b0(void) {
    typedef void (__cdecl* Fn)(void);
    reinterpret_cast<Fn>(0x004942b0u)();
}

#define MATCH_PORT 1
#define FUN_00494480 IntroInputCheck
#define FUN_004942b0 MatchS15_00494480_Call004942b0
#define FUN_00493fc0 AspectRatioGlobalGet
#define FUN_004938e0 Walk4938e0
#pragma comment(linker, "/EXPORT:IntroInputCheck=_IntroInputCheck")
extern "C" {
#include "../../../../re/match/s15/00494480.c"
}
