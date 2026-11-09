// MATCH-S15: port body of 0x00492270 = the byte-matched source re/match/s15/00492270.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Boot/SubsystemInit.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00492270 SubsystemInit
// [PORT-TOP-03] exe: 0x00493710 binds to its matched source compiled in Match/PortTop_00493710.cpp (RW calls on librw,
// LibRw/RwApiBind.cpp); since PORT-TOP-04B 0x004921d0 / 0x00428590 to their matched exe bodies, whose callees bind by
// name. Reached only under MASHED_ORIG_TOP. The .asi keeps the raw-address calls.
#ifdef MASHED_STANDALONE
#define FUN_00493710 PortTop_Bound_00493710
#define FUN_004921d0 DisplayInit   // [PORT-TOP-04B] matched, Match/MatchS15_004921d0.cpp (callees by name)
#define FUN_00428590 ViewportInit  // [PORT-TOP-04B] matched, Match/MatchS06_00428590.cpp (callees by name)
#else
#define FUN_00493710 MatchS15_00492270_Call00493710
#define FUN_004921d0 MatchS15_00492270_Call004921d0
#define FUN_00428590 MatchS15_00492270_Call00428590
// Callees of the matched source: the old Boot/SubsystemInit.cpp body called each one through its original address, so these
// forwarders make that same raw-address call (in the .asi an installed callee hook is reached through its JMP).
extern "C" unsigned int MatchS15_00492270_Call00493710(int p0) {
    return reinterpret_cast<unsigned int (__cdecl*)(int p0)>(0x00493710u)(p0);
}
extern "C" void MatchS15_00492270_Call004921d0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x004921d0u)();
}
extern "C" unsigned int MatchS15_00492270_Call00428590(void) {
    return reinterpret_cast<unsigned int (__cdecl*)()>(0x00428590u)();
}
#endif
#pragma comment(linker, "/EXPORT:SubsystemInit=_SubsystemInit")
extern "C" {
#include "../../../../re/match/s15/00492270.c"
}
RH_ScopedInstall(SubsystemInit, 0x00492270);
