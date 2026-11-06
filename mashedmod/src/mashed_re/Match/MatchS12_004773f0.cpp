// MATCH-S12: port body of 0x004773f0 = the byte-matched source re/match/s12/004773f0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Its in-file caller 0x00477730 is HELD (not compiled with MATCH_PORT): re/match/s12/00477730.c is that RVA's file.
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The original takes the Decal* in EAX and never writes EAX. VS2022 has no EAX convention, so the matched
// body is the cdecl Init4773f0Body and the installed/exported entry Init4773f0 (arg_type eax_implicit_void)
// is a register adapter.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004773f0 Init4773f0Body
#pragma comment(linker, "/EXPORT:Init4773f0=_Init4773f0")
extern "C" {
#include "../../../../re/match/s12/004773f0.c"
}

// EAX-record entry for the hook and the Frida export. Not a body: it only forwards to Init4773f0Body.
// EAX is preserved like the original. ECX/EDX are clobbered by both.
extern "C" __declspec(naked) void __cdecl Init4773f0(void) {
    __asm {
        push eax
        call Init4773f0Body
        pop  eax
        ret
    }
}
RH_ScopedInstall(Init4773f0, 0x004773f0);
