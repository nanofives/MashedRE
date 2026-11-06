// MATCH-S16: port body of 0x00496db0 = the byte-matched source re/match/s16/00496db0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/PromoLoop_standins_r4.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00496db0 PipelineFragment4eb9d0Create
#define FUN_004d4170 MatchS16_00496db0_Call004d4170
#define FUN_004d4dd0 MatchS16_00496db0_Call004d4dd0
#define FUN_004eb9d0 MatchS16_00496db0_Call004eb9d0
#define FUN_004d4f90 MatchS16_00496db0_Call004d4f90
#define FUN_004d4380 MatchS16_00496db0_Call004d4380
#define FUN_004d41e0 MatchS16_00496db0_Call004d41e0
// Callees of the matched source: the port has no body of any of them (rva_body_scan), so each forwarder calls the
// callee through its original address, the same calls the old Render/PromoLoop_standins_r4.cpp body made.
extern "C" int MatchS16_00496db0_Call004d4170(void) { return reinterpret_cast<int (__cdecl*)(void)>(0x004d4170u)(); }
extern "C" int MatchS16_00496db0_Call004d4dd0(int p0) { return reinterpret_cast<int (__cdecl*)(int)>(0x004d4dd0u)(p0); }
extern "C" int MatchS16_00496db0_Call004eb9d0(void) { return reinterpret_cast<int (__cdecl*)(void)>(0x004eb9d0u)(); }
extern "C" int MatchS16_00496db0_Call004d4f90(int p0, int p1, int p2, int p3) {
    return reinterpret_cast<int (__cdecl*)(int, int, int, int)>(0x004d4f90u)(p0, p1, p2, p3);
}
extern "C" int MatchS16_00496db0_Call004d4380(int p0) { return reinterpret_cast<int (__cdecl*)(int)>(0x004d4380u)(p0); }
extern "C" void MatchS16_00496db0_Call004d41e0(int p0) { reinterpret_cast<void (__cdecl*)(int)>(0x004d41e0u)(p0); }
#pragma comment(linker, "/EXPORT:PipelineFragment4eb9d0Create=_PipelineFragment4eb9d0Create")
extern "C" {
#include "../../../../re/match/s16/00496db0.c"
}
RH_ScopedInstall(PipelineFragment4eb9d0Create, 0x00496db0);
