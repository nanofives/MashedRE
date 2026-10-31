// MATCH-S04: port body of 0x0041b720 and 0x0041bf20 = the byte-matched source re/match/s04/0041b720.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is the only body of both RVAs in the targets that compile this TU
// (0x0041b720 moved out of Util/PromoLoop_sessionB.cpp; 0x0041bf20 had no port body and keeps FUN_0041bf20).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The original passes 0x0041b720's record in EAX (call site 0x0041bff0) and the caller keeps it in EAX
// across the call. VS2022 has no EAX convention, so the matched body is the cdecl Bits41b720Body and the
// installed/exported entry Bits41b720 is a register adapter (push the EAX record, call the body, restore EAX).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0041b720 Bits41b720Body
#define FUN_004b6520 ZeroFillWrapper
#pragma comment(linker, "/EXPORT:Bits41b720=_Bits41b720")
extern "C" {
#include "../../../../re/match/s04/0041b720.c"
}

// EAX-record entry for the hook and the Frida export (arg_type eax_ecx_insert). Not a body: it only
// forwards to Bits41b720Body. EAX is preserved like the original; ECX/EDX are clobbered by both.
extern "C" __declspec(naked) void __cdecl Bits41b720(void) {
    __asm {
        push eax
        call Bits41b720Body
        pop eax
        ret
    }
}
RH_ScopedInstall(Bits41b720, 0x0041b720);
