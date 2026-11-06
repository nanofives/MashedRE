// MATCH-S12: port body of 0x00475f30 = the byte-matched source re/match/s12/00475f30.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The original takes the output pointer in EAX (two float args on the stack) and never writes EAX.
// VS2022 has no EAX convention, so the matched body is the cdecl ScaledMulAdd475f30Body and the
// installed/exported entry ScaledMulAdd475f30 (arg_type eax_out_2float) is a register adapter.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00475f30 ScaledMulAdd475f30Body
#pragma comment(linker, "/EXPORT:ScaledMulAdd475f30=_ScaledMulAdd475f30")
extern "C" {
#include "../../../../re/match/s12/00475f30.c"
}

// EAX-out entry for the hook and the Frida export. Not a body: it only forwards to ScaledMulAdd475f30Body.
// Entry frame: [ESP+4]=a1, [ESP+8]=a2. EAX is preserved like the original. Plain RET (caller cleans).
extern "C" __declspec(naked) void __cdecl ScaledMulAdd475f30(void) {
    __asm {
        push dword ptr [esp+8]      // a2
        push dword ptr [esp+8]      // a1
        push eax                    // out (register arg in the original)
        call ScaledMulAdd475f30Body
        pop  eax
        add  esp, 8
        ret
    }
}
RH_ScopedInstall(ScaledMulAdd475f30, 0x00475f30);
