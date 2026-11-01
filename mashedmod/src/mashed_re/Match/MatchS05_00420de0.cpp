// MATCH-S05: port body of 0x00420de0 and of its callers 0x004215c0 and 0x004215e0 = the byte-matched source
// re/match/s05/00420de0.c (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%
// for all three MATCH lines). The same source text is these RVAs' only body in the targets that compile this TU
// (0x00420de0 and 0x004215c0 moved out of Util/PromoLoop_sessionB.cpp; 0x004215e0 had no port body and has no install
// and no port caller).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The matched 0x00420de0 is `static` (the DDK passes the record in EDX, the index in ECX, the float on the stack).
// Under VS2022 it is a plain cdecl static (r, j, d), named Accum420de0_impl here; Accum420de0 below is the entry
// with the old export's convention, cdecl (idx, base, val) (the accum_420de0 registry entry drives it that way, and
// it is the RH_ScopedInstall target the old port had). 0x004215c0 keeps the symbol Accum4215c0 (cdecl, same as the old
// export) and now calls the matched 0x00420de0 directly instead of the original's 0x00420de0 by address.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00420de0 Accum420de0_impl
#define FUN_004215c0 Accum4215c0
#pragma comment(linker, "/EXPORT:Accum4215c0=_Accum4215c0")
extern "C" {
#include "../../../../re/match/s05/00420de0.c"
}
RH_ScopedInstall(Accum4215c0, 0x004215c0);

// Entry with the old export's convention: cdecl (idx, base, val), re-passed as (base, idx, val).
#pragma comment(linker, "/EXPORT:Accum420de0=_Accum420de0")
extern "C" __declspec(naked) void __cdecl Accum420de0(void)
{
    __asm {
        push dword ptr [esp+0x0c]    // val  (arg 3 -> d)
        push dword ptr [esp+0x08]    // idx  (arg 1 -> j)
        push dword ptr [esp+0x10]    // base (arg 2 -> r)
        call Accum420de0_impl
        add  esp, 0x0c
        ret
    }
}
RH_ScopedInstall(Accum420de0, 0x00420de0);
