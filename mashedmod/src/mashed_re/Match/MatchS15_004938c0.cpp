// MATCH-S15: port body of 0x004938c0 = the byte-matched source re/match/s15/004938c0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Boot/LaunchHandshake.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004938c0 SoftwareTidyUpBeforeExiting
// [PORT-TOP-03] exe: the five RW core calls bind to the RW API binding layer on librw (LibRw/RwApiBind.cpp, L-98):
// 0x00558470 shim (frees the two 0x005584c0 buffers), 0x00550390 RtFSManagerClose shim, 0x004c2f60 RwEngineStop,
// 0x004c3040 RwEngineClose, 0x004c3270 RwEngineTerm. The .asi keeps the raw-address calls (MASHED.exe's own RW core).
#ifdef MASHED_STANDALONE
#define FUN_00558470 MashedRw_Free7dc8b8_00558470
#define FUN_00550390 MashedRw_RtFSManagerClose_00550390
#define FUN_004c2f60 MashedRw_EngineStop_004c2f60
#define FUN_004c3040 MashedRw_EngineClose_004c3040
#define FUN_004c3270 MashedRw_EngineTerm_004c3270
#else
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
#endif
#pragma comment(linker, "/EXPORT:SoftwareTidyUpBeforeExiting=_SoftwareTidyUpBeforeExiting")
extern "C" {
#include "../../../../re/match/s15/004938c0.c"
}
RH_ScopedInstall(SoftwareTidyUpBeforeExiting, 0x004938c0);
