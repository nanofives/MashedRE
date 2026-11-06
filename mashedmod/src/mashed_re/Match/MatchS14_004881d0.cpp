// MATCH-S14: port body of 0x004881d0 = the byte-matched source re/match/s14/004881d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Sky/PromoLoop_standins_r6.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004881d0 Obj7030a8Create
#define FUN_004880a0 Obj7030acCreate
#define FUN_004c0b30 MatchS14_004881d0_Call004c0b30
#define FUN_00534b60 MatchS14_004881d0_Call00534b60
#define FUN_004e7e30 MatchS14_004881d0_Call004e7e30
#define FUN_004e8090 MatchS14_004881d0_Call004e8090
// Callees of the matched source: the old Sky/PromoLoop_standins_r6.cpp body called all four through their original
// addresses (function-pointer casts to 0x004c0b30 / 0x00534b60 / 0x004e7e30 / 0x004e8090). None has a callable port
// body (rva_body_scan's hit for 0x00534b60, Render/RfxEmitter.cpp PTankNormaliseFlags, is a one-argument C++ helper
// for the flag-normalisation part only, not the 3-argument function), so these forwarders make the same raw calls.
extern "C" int MatchS14_004881d0_Call004c0b30(void) {
    return reinterpret_cast<int(__cdecl*)(void)>(0x004c0b30u)();
}
extern "C" int MatchS14_004881d0_Call00534b60(int p0, unsigned int p1, int p2) {
    return reinterpret_cast<int(__cdecl*)(int, unsigned int, int)>(0x00534b60u)(p0, p1, p2);
}
extern "C" int MatchS14_004881d0_Call004e7e30(int p0, int p1) {
    return reinterpret_cast<int(__cdecl*)(int, int)>(0x004e7e30u)(p0, p1);
}
extern "C" int* MatchS14_004881d0_Call004e8090(int p0, int p1) {
    return reinterpret_cast<int*(__cdecl*)(int, int)>(0x004e8090u)(p0, p1);
}
// FUN_004880a0 -> Obj7030acCreate = Match/MatchS14_004880a0.cpp (the old TU called its own Obj7030acCreate).
// The old body returned the guard / 0x004880a0's EAX; the matched source is void. The only caller, 0x00402918, does not
// read EAX (next: push 0x5cc3e0; call 0x004963e0).
#pragma comment(linker, "/EXPORT:Obj7030a8Create=_Obj7030a8Create")
extern "C" {
#include "../../../../re/match/s14/004881d0.c"
}
RH_ScopedInstall(Obj7030a8Create, 0x004881d0);
