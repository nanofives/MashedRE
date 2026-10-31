// MATCH-S04: port body of 0x0041c320 and of its caller 0x0041cb10 = the byte-matched source re/match/s04/0041c320.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100% for both MATCH lines).
// The same source text is these RVAs' only body in the targets that compile this TU (0x0041c320 moved out of
// Render/ParticleEmitterCtors.cpp; 0x0041cb10 had no port body and has no install and no port caller).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The matched 0x0041c320 is `static` (the DDK passes the clump in EAX and the record in EBX). Under VS2022 it is a
// plain cdecl static, named ParticleEmitterCtorC_impl here; ParticleEmitterCtorC below is the register-convention
// entry (the RH_ScopedInstall target and the old export), the same naked shim the old port used.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0041c320 ParticleEmitterCtorC_impl
#define FUN_004b3fc0 MatchS04_0041c320_Call004b3fc0
#define FUN_004b6520 MatchS04_0041c320_Call004b6520
#define FUN_004b5190 MatchS04_0041c320_Call004b5190
#define FUN_0042a6b0 MatchS04_0041c320_Call0042a6b0
#define FUN_004c5c80 MatchS04_0041c320_Call004c5c80
#define FUN_0042a5d0 MatchS04_0041c320_Call0042a5d0
#define FUN_004e6ab0 MatchS04_0041c320_Call004e6ab0
extern "C" {
#include "../../../../re/match/s04/0041c320.c"
}

// Callees of the matched source: the old Render/ParticleEmitterCtors.cpp body called 0x004b3fc0 / 0x004b6520 /
// 0x004b5190 through their original addresses (__cdecl function-pointer casts), so each forwarder makes that same
// call. 0x0041cb10 had no port body: the original code there calls 0x0042a6b0 / 0x004c5c80 / 0x0042a5d0 /
// 0x004e6ab0 at their original addresses (in the .asi 0x004c5c80 carries the installed JMP into
// Util/PromoLoop_sessionB.cpp Set4c5c80, which is .asi-only), so these forwarders make those calls.
extern "C" {
void MatchS04_0041c320_Call004b3fc0(Clump_0041c320 *c, int *out) {
    reinterpret_cast<void (__cdecl*)(Clump_0041c320*, int*)>(0x004b3fc0u)(c, out);
}
void MatchS04_0041c320_Call004b6520(void *p, int n) {
    reinterpret_cast<void (__cdecl*)(void*, int)>(0x004b6520u)(p, n);
}
int MatchS04_0041c320_Call004b5190(int a, int b, int c) {
    return reinterpret_cast<int (__cdecl*)(int, int, int)>(0x004b5190u)(a, b, c);
}
int MatchS04_0041c320_Call0042a6b0(char *name, int a, int b) {
    return reinterpret_cast<int (__cdecl*)(char*, int, int)>(0x0042a6b0u)(name, a, b);
}
void MatchS04_0041c320_Call004c5c80(int a) {
    reinterpret_cast<void (__cdecl*)(int)>(0x004c5c80u)(a);
}
Clump_0041c320 *MatchS04_0041c320_Call0042a5d0(char *name, int a, int b) {
    return reinterpret_cast<Clump_0041c320* (__cdecl*)(char*, int, int)>(0x0042a5d0u)(name, a, b);
}
Clump_0041c320 *MatchS04_0041c320_Call004e6ab0(Clump_0041c320 *c) {
    return reinterpret_cast<Clump_0041c320* (__cdecl*)(Clump_0041c320*)>(0x004e6ab0u)(c);
}
}

// Register-convention entry: clump in EAX, record in EBX, re-passed as cdecl (clump, record). EBX is untouched.
#pragma comment(linker, "/EXPORT:ParticleEmitterCtorC=_ParticleEmitterCtorC")
extern "C" __declspec(naked) void __cdecl ParticleEmitterCtorC()
{
    __asm {
        push ebx                     // record (arg 2, pushed first)
        push eax                     // clump  (arg 1, pushed second)
        call ParticleEmitterCtorC_impl
        add  esp, 8
        ret
    }
}
RH_ScopedInstall(ParticleEmitterCtorC, 0x0041c320);
