// MATCH-S04: port body of 0x0041bc50 and of its caller 0x0041c0c0 = the byte-matched source re/match/s04/0041bc50.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100% for both MATCH lines).
// The same source text is these RVAs' only body in the targets that compile this TU (moved out of HUD/HudDispatch.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The matched 0x0041bc50 is `static` (the DDK passes its record in EAX). Under VS2022 it is a plain cdecl static,
// named HudRec29DispatchBody here; FUN_0041bc50 below is the EAX-convention entry the original's callers need
// (the RH_ScopedInstall target and the old export), the same naked shim shape the old HudDispatch.cpp port used.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0041bc50 HudRec29DispatchBody
#define FUN_0041c0c0 HudDispatchSlot2
#pragma comment(linker, "/EXPORT:HudDispatchSlot2=_HudDispatchSlot2")
extern "C" {
#include "../../../../re/match/s04/0041bc50.c"
}
#undef FUN_0041bc50

// EAX-convention entry: record pointer in EAX, re-passed as the cdecl stack argument.
#pragma comment(linker, "/EXPORT:FUN_0041bc50=_FUN_0041bc50")
extern "C" __declspec(naked) void __cdecl FUN_0041bc50()
{
    __asm {
        push eax
        call HudRec29DispatchBody
        add  esp, 4
        ret
    }
}
RH_ScopedInstall(FUN_0041bc50, 0x0041bc50);
RH_ScopedInstall(HudDispatchSlot2, 0x0041c0c0);
