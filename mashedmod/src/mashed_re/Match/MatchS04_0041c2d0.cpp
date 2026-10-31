// MATCH-S04: port body of 0x0041c2d0 and of its caller 0x0041c300 = the byte-matched source re/match/s04/0041c2d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100% for both MATCH lines).
// The same source text is these RVAs' only body in the targets that compile this TU (moved out of HUD/HudDispatch.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The matched 0x0041c2d0 is `static` (the DDK passes its block pointer in EAX). Under VS2022 it is a plain cdecl
// static, named HudDispatchDraw4Body here; FUN_0041c2d0 below is the EAX-convention entry that the original's callers
// and HUD/HudDispatch.cpp HudDispatchMode10 (inline asm, EAX = 0x0063c610) call: the RH_ScopedInstall target and the
// old export, the same naked shim shape the old HudDispatch.cpp port used.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0041c2d0 HudDispatchDraw4Body
#define FUN_0041c300 HudDispatchMode5
#pragma comment(linker, "/EXPORT:HudDispatchMode5=_HudDispatchMode5")
extern "C" {
#include "../../../../re/match/s04/0041c2d0.c"
}
#undef FUN_0041c2d0

// EAX-convention entry: block pointer in EAX, re-passed as the cdecl stack argument.
#pragma comment(linker, "/EXPORT:FUN_0041c2d0=_FUN_0041c2d0")
extern "C" __declspec(naked) unsigned int __cdecl FUN_0041c2d0()
{
    __asm {
        push eax
        call HudDispatchDraw4Body
        add  esp, 4
        ret
    }
}
RH_ScopedInstall(FUN_0041c2d0, 0x0041c2d0);
RH_ScopedInstall(HudDispatchMode5, 0x0041c300);
