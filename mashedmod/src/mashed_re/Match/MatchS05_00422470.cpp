// MATCH-S05: port body of 0x004223f0 and of its caller 0x00422470 = the byte-matched source re/match/s05/00422470.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100% for both MATCH lines).
// The same source text is these RVAs' only body in the targets that compile this TU (0x004223f0 moved out of
// Util/PromoLoop_sessionB.cpp; 0x00422470 had no port body and has no install and no port caller).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The matched 0x004223f0 is `static`: in the original the mask (`seed`) is in EAX and the hashed value (`i`) in ECX
// (0x004223f1 test eax,eax / 0x004223f5 and ecx,eax; caller 0x004224ab mov eax,esi / 0x004224a5 lea ecx,[edx+1]).
// Under VS2022 it is a plain cdecl static (i, seed), named Hash4223f0_impl here; Hash4223f0 below is the entry with
// the old export's convention, cdecl (a = EAX value, b = ECX value) (the hash_4223f0 registry entry drives it that
// way, and it is the RH_ScopedInstall target the old port had). Callee 0x00422440 binds to the port's CosineLerp
// (Math/CosineLerp.cpp). floor is the CRT floor (the original calls its CRT copy at 0x004a3090).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004223f0 Hash4223f0_impl
#define FUN_00422440 CosineLerp
extern "C" {
#include "../../../../re/match/s05/00422470.c"
}

// Entry with the old export's convention: cdecl (a = mask, b = value), re-passed as (i = b, seed = a).
#pragma comment(linker, "/EXPORT:Hash4223f0=_Hash4223f0")
extern "C" __declspec(naked) float __cdecl Hash4223f0(void)
{
    __asm {
        push dword ptr [esp+0x04]    // a (EAX value) -> seed
        push dword ptr [esp+0x0c]    // b (ECX value) -> i
        call Hash4223f0_impl
        add  esp, 8
        ret
    }
}
RH_ScopedInstall(Hash4223f0, 0x004223f0);
