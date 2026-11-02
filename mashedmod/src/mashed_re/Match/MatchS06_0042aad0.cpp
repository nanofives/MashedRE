// MATCH-S06: port body of 0x0042aad0 = the byte-matched source re/match/s06/0042aad0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuChrome.cpp).
// The matched 0x0042aad0 is `static` (the DDK passes the record in EAX). Under VS2022 it is the plain cdecl
// MenuDimSetBody (MATCH_STATIC); MenuDimSet below is the EAX entry (the RH_ScopedInstall target and the old export).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042aad0 MenuDimSetBody
extern "C" {
#include "../../../../re/match/s06/0042aad0.c"
}

// EAX-record entry. The original clobbers no register (U-0453: caller 0x0043c5b0 reads ECX/EDX after the call), so
// ECX/EDX/EAX are saved around the cdecl body. Not a body: it only forwards to MenuDimSetBody.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl MenuDimSet(void) {
    __asm {
        push ecx
        push edx
        push eax
        call MenuDimSetBody
        pop eax
        pop edx
        pop ecx
        ret
    }
}
RH_ScopedInstall(MenuDimSet, 0x0042aad0);
