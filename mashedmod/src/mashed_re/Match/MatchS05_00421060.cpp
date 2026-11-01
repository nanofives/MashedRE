// MATCH-S05: port body of 0x00421060 and of its caller 0x00421690 = the byte-matched source re/match/s05/00421060.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100% for both MATCH lines).
// The same source text is these RVAs' only body in the targets that compile this TU (0x00421060 moved out of
// Util/PromoLoop_sessionB.cpp; 0x00421690 had no port body and has no install and no port caller).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The matched 0x00421060 takes its record pointer in ESI in the original. Under C++ it is MATCH_STATIC = extern, a
// plain cdecl(p) function named StructZero421060 (the old export, driven as __cdecl(buf) by the esi_struct_init
// registry entry); StructZero421060_RegAbi below is the register-convention entry (the RH_ScopedInstall target),
// the same naked shim the old port used. Callee 0x004b6520 binds to the port's ZeroFillWrapper (Util/TimerSlot.cpp).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00421060 StructZero421060
#define FUN_004b6520 ZeroFillWrapper
#pragma comment(linker, "/EXPORT:StructZero421060=_StructZero421060")
extern "C" {
#include "../../../../re/match/s05/00421060.c"
}

// Register-convention entry: record pointer in ESI, re-passed as cdecl(p).
#pragma comment(linker, "/EXPORT:StructZero421060_RegAbi=_StructZero421060_RegAbi")
extern "C" __declspec(naked) void __cdecl StructZero421060_RegAbi(void)
{
    __asm {
        push esi                     // p (register arg in the original)
        call StructZero421060
        add  esp, 4
        ret
    }
}
RH_ScopedInstall(StructZero421060_RegAbi, 0x00421060);
