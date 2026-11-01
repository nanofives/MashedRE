// MATCH-S05: port body of 0x004218b0 and of its caller 0x004218d0 = the byte-matched source re/match/s05/004218b0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100% for both MATCH lines).
// The same source text is these RVAs' only body in the targets that compile this TU (both moved out of
// Physics/PromoLoop_standins_r7.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The matched 0x004218b0 is `static` (the DDK passes the first argument in EAX, the second on the stack). Under VS2022
// it is a plain cdecl static, named PhysBodyCreateAttach_impl here; PhysBodyCreateAttach below is the
// register-convention entry (the RH_ScopedInstall target and the old export), the same naked shim the old port used.
// 0x004218d0 keeps the symbol PhysWorldBodyBuild (cdecl, five dword arguments, same as the old export).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004218b0 PhysBodyCreateAttach_impl
#define FUN_004218d0 PhysWorldBodyBuild
#define FUN_0057c500 MatchS05_004218b0_Call0057c500
#define FUN_0057c550 MatchS05_004218b0_Call0057c550
#define FUN_0057c300 MatchS05_004218b0_Call0057c300
#define FUN_0057c220 MatchS05_004218b0_Call0057c220
#define FUN_0055c540 MatchS05_004218b0_Call0055c540
#define FUN_0055c4f0 MatchS05_004218b0_Call0055c4f0
#define FUN_0055c4a0 MatchS05_004218b0_Call0055c4a0
#define FUN_0055b940 MatchS05_004218b0_Call0055b940
#pragma comment(linker, "/EXPORT:PhysWorldBodyBuild=_PhysWorldBodyBuild")
extern "C" {
#include "../../../../re/match/s05/004218b0.c"
}
RH_ScopedInstall(PhysWorldBodyBuild, 0x004218d0);

// Callees of the matched source: the old Physics/PromoLoop_standins_r7.cpp bodies called all eight through their
// original addresses (__cdecl function-pointer casts), so each forwarder makes that same call.
extern "C" {
int *MatchS05_004218b0_Call0057c500(void) {
    return reinterpret_cast<int* (__cdecl*)()>(0x0057c500u)();
}
void MatchS05_004218b0_Call0057c550(int *p0) {
    reinterpret_cast<void (__cdecl*)(int*)>(0x0057c550u)(p0);
}
int MatchS05_004218b0_Call0057c300(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x0057c300u)(p0);
}
void MatchS05_004218b0_Call0057c220(int p0, int p1) {
    reinterpret_cast<void (__cdecl*)(int, int)>(0x0057c220u)(p0, p1);
}
void MatchS05_004218b0_Call0055c540(int p0, float p1) {
    reinterpret_cast<void (__cdecl*)(int, float)>(0x0055c540u)(p0, p1);
}
void MatchS05_004218b0_Call0055c4f0(int p0, int p1) {
    reinterpret_cast<void (__cdecl*)(int, int)>(0x0055c4f0u)(p0, p1);
}
void MatchS05_004218b0_Call0055c4a0(int p0, float p1) {
    reinterpret_cast<void (__cdecl*)(int, float)>(0x0055c4a0u)(p0, p1);
}
float MatchS05_004218b0_Call0055b940(int *p0, float p1, int p2) {
    return reinterpret_cast<float (__cdecl*)(int*, float, int)>(0x0055b940u)(p0, p1, p2);
}
}

// Register-convention entry: first argument in EAX, second on the stack, re-passed as cdecl (EAX, arg).
#pragma comment(linker, "/EXPORT:PhysBodyCreateAttach=_PhysBodyCreateAttach")
extern "C" __declspec(naked) void __cdecl PhysBodyCreateAttach()
{
    __asm {
        push dword ptr [esp+4]       // second argument (stack)
        push eax                     // first argument (EAX)
        call PhysBodyCreateAttach_impl
        add  esp, 8
        ret
    }
}
RH_ScopedInstall(PhysBodyCreateAttach, 0x004218b0);
