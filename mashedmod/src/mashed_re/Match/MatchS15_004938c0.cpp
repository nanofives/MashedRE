// MATCH-S15: port body of 0x004938c0 = the byte-matched source re/match/s15/004938c0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Boot/LaunchHandshake.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004938c0 SoftwareTidyUpBeforeExiting
#define FUN_00558470 MatchS15_004938c0_Call00558470
#define FUN_00550390 MatchS15_004938c0_Call00550390
#define FUN_004c2f60 MatchS15_004938c0_Call004c2f60
#define FUN_004c3040 MatchS15_004938c0_Call004c3040
#define FUN_004c3270 MatchS15_004938c0_Call004c3270
// Callees of the matched source: the old Boot/LaunchHandshake.cpp body called each one through its original address, so these
// forwarders make that same raw-address call (in the .asi an installed callee hook is reached through its JMP).
extern "C" void MatchS15_004938c0_Call00558470(void) {
    reinterpret_cast<void (__cdecl*)()>(0x00558470u)();
}
extern "C" void MatchS15_004938c0_Call00550390(void) {
    reinterpret_cast<void (__cdecl*)()>(0x00550390u)();
}
extern "C" void MatchS15_004938c0_Call004c2f60(void) {
    reinterpret_cast<void (__cdecl*)()>(0x004c2f60u)();
}
extern "C" int MatchS15_004938c0_Call004c3040(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x004c3040u)();
}
extern "C" bool MatchS15_004938c0_Call004c3270(void) {
    return reinterpret_cast<bool (__cdecl*)()>(0x004c3270u)();
}
#pragma comment(linker, "/EXPORT:SoftwareTidyUpBeforeExiting=_SoftwareTidyUpBeforeExiting")
extern "C" {
#include "../../../../re/match/s15/004938c0.c"
}
RH_ScopedInstall(SoftwareTidyUpBeforeExiting, 0x004938c0);
