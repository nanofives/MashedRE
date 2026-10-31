// MATCH-S02: port body of 0x0040cf40 = the byte-matched source re/match/s02/0040cf40.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Gameplay/Thresholds_ah4.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// asi only, like the old body: the callee (FUN_004c5a60) has no port implementation, so the call
// goes into the original at its fixed address, as the old body did. The exe gets an empty TU.
// The .asi's Frida/install entry stays ReleaseResourcePair (Gameplay/Thresholds_ah4.cpp shadow A/B
// wrapper), which calls ReleaseResourcePair_impl.

#ifndef MASHED_STANDALONE
// Call-through into the original's destructor (not a body of that function).
extern "C" unsigned int __cdecl MatchS02_0040cf40_CallOrig4c5a60(int p0) {
    typedef unsigned int (__cdecl* Fn)(int);
    return reinterpret_cast<Fn>(0x004c5a60u)(p0);
}

#define MATCH_PORT 1
#define FUN_0040cf40 ReleaseResourcePair_impl
#define FUN_004c5a60 MatchS02_0040cf40_CallOrig4c5a60
#pragma comment(linker, "/EXPORT:ReleaseResourcePair_impl=_ReleaseResourcePair_impl")
extern "C" {
#include "../../../../re/match/s02/0040cf40.c"
}
#endif  // !MASHED_STANDALONE
