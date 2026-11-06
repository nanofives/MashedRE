// MATCH-S10: port body of 0x0045d3f0 = the byte-matched source re/match/s10/0045d3f0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Lane2/L2_0045d3f0.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0045d3f0 L2_FUN_0045d3f0
#define FUN_00550be0 MatchS10_0045d3f0_Call00550be0
#define FUN_005a60b0 MatchS10_0045d3f0_Call005a60b0
#pragma comment(linker, "/EXPORT:L2_FUN_0045d3f0=_L2_FUN_0045d3f0")
extern "C" {
#include "../../../../re/match/s10/0045d3f0.c"
}
RH_ScopedInstall(L2_FUN_0045d3f0, 0x0045d3f0);

// Raw-address forwarders (call-throughs into the original at the fixed address, not bodies of the callees;
// the old Lane2 body made the same raw calls, and neither callee has a port body).
extern "C" {
void MatchS10_0045d3f0_Call00550be0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x00550be0u)();
}
void MatchS10_0045d3f0_Call005a60b0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x005a60b0u)();
}
}
