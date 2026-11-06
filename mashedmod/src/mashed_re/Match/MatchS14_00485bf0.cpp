// MATCH-S14: port body of 0x00485bf0 = the byte-matched source re/match/s14/00485bf0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round48.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00485bf0 StackPush485bf0
#define FUN_004a332b MatchS14_00485bf0_Call004a332b
// Callee 0x004a332b (CRT exit, no return): the old body called it through its original address; no port body is
// compiled in the .asi (Boot/Boot.cpp CrtExitNoReturn_j5 is in neither rsp). This forwarder makes the same call.
extern "C" __declspec(noreturn) void MatchS14_00485bf0_Call004a332b(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x004a332bu)(p0);
    __assume(0);
}
#pragma comment(linker, "/EXPORT:StackPush485bf0=_StackPush485bf0")
extern "C" {
#include "../../../../re/match/s14/00485bf0.c"
}
RH_ScopedInstall(StackPush485bf0, 0x00485bf0);
