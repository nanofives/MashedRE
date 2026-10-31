// MATCH-S02: port body of 0x0040dfc0 = the byte-matched source re/match/s02/0040dfc0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of HUD/HudDispatch.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040dfc0 HudIngameDispatch
#define FUN_00403160 MatchS02_0040dfc0_Call00403160
#define FUN_0041a3e0 MatchS02_0040dfc0_Call0041a3e0
#define FUN_0041b630 MatchS02_0040dfc0_Call0041b630
#define FUN_0041c0c0 MatchS02_0040dfc0_Call0041c0c0
#define FUN_0041c300 MatchS02_0040dfc0_Call0041c300
#define FUN_0041ccc0 MatchS02_0040dfc0_Call0041ccc0
#define FUN_0041d870 MatchS02_0040dfc0_Call0041d870
#define FUN_0041db80 MatchS02_0040dfc0_Call0041db80
#define FUN_0041ded0 MatchS02_0040dfc0_Call0041ded0
#define FUN_0041e850 MatchS02_0040dfc0_Call0041e850
#define FUN_00426ba0 MatchS02_0040dfc0_Call00426ba0
#define FUN_0042f500 MatchS02_0040dfc0_Call0042f500
#define FUN_0042f6a0 MatchS02_0040dfc0_Call0042f6a0
// Callees of the matched source: the old HUD/HudDispatch.cpp body called all 13 through their original addresses
// (__cdecl function-pointer casts), so each forwarder makes that same call.
extern "C" void MatchS02_0040dfc0_Call00403160(void) {
    reinterpret_cast<void (__cdecl*)()>(0x00403160u)();
}
extern "C" void MatchS02_0040dfc0_Call0041a3e0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x0041a3e0u)();
}
extern "C" void MatchS02_0040dfc0_Call0041b630(void) {
    reinterpret_cast<void (__cdecl*)()>(0x0041b630u)();
}
extern "C" void MatchS02_0040dfc0_Call0041c0c0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x0041c0c0u)();
}
extern "C" void MatchS02_0040dfc0_Call0041c300(void) {
    reinterpret_cast<void (__cdecl*)()>(0x0041c300u)();
}
extern "C" void MatchS02_0040dfc0_Call0041ccc0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x0041ccc0u)();
}
extern "C" void MatchS02_0040dfc0_Call0041d870(void) {
    reinterpret_cast<void (__cdecl*)()>(0x0041d870u)();
}
extern "C" void MatchS02_0040dfc0_Call0041db80(void) {
    reinterpret_cast<void (__cdecl*)()>(0x0041db80u)();
}
extern "C" void MatchS02_0040dfc0_Call0041ded0(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x0041ded0u)(p0);
}
extern "C" void MatchS02_0040dfc0_Call0041e850(void) {
    reinterpret_cast<void (__cdecl*)()>(0x0041e850u)();
}
extern "C" int MatchS02_0040dfc0_Call00426ba0(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x00426ba0u)();
}
extern "C" int MatchS02_0040dfc0_Call0042f500(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x0042f500u)();
}
extern "C" int MatchS02_0040dfc0_Call0042f6a0(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x0042f6a0u)();
}
#pragma comment(linker, "/EXPORT:HudIngameDispatch=_HudIngameDispatch")
extern "C" {
#include "../../../../re/match/s02/0040dfc0.c"
}
RH_ScopedInstall(HudIngameDispatch, 0x0040dfc0);
