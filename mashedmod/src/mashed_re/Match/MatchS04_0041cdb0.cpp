// MATCH-S04: port body of 0x0041cdb0 and 0x0041d730 = the byte-matched source re/match/s04/0041cdb0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is the only body of both RVAs in the targets that compile this TU
// (0x0041cdb0 moved out of Util/PromoLoop_sessionB.cpp, 0x0041d730 PlayerSlotConfigInit out of Util/UtilMid.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The original passes 0x0041cdb0's record in EAX (call site 0x0041d800) and the caller keeps it in EAX
// across the call. VS2022 has no EAX convention, so the matched body is the cdecl Bits41cdb0Body and the
// installed/exported entry Bits41cdb0 is a register adapter (push the EAX record, call the body, restore EAX).
// PlayerSlotConfigInit stays uninstalled, as in Util/UtilMid.cpp (MASS-DISABLED 2026-05-24
// needs-canonical-slot-state).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0041cdb0 Bits41cdb0Body
#define FUN_0041d730 PlayerSlotConfigInit
#define FUN_004b6520 ZeroFillWrapper
#pragma comment(linker, "/EXPORT:Bits41cdb0=_Bits41cdb0")
#pragma comment(linker, "/EXPORT:PlayerSlotConfigInit=_PlayerSlotConfigInit")
extern "C" {
#include "../../../../re/match/s04/0041cdb0.c"
}

// EAX-record entry for the hook and the Frida export (arg_type eax_ecx_insert). Not a body: it only
// forwards to Bits41cdb0Body. EAX is preserved like the original; ECX/EDX are clobbered by both.
extern "C" __declspec(naked) void __cdecl Bits41cdb0(void) {
    __asm {
        push eax
        call Bits41cdb0Body
        pop eax
        ret
    }
}
RH_ScopedInstall(Bits41cdb0, 0x0041cdb0);
