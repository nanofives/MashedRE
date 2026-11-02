// MATCH-S07: port body of 0x0042f8d0 = the byte-matched source re/match/s07/0042f8d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuMenusB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
//
// The original takes the alpha byte in AL plus four stack floats (x, y, w, h). The matched source is `static` under
// the DDK only so the compiler gives it that register convention (MATCH_STATIC); in the port it is a plain __cdecl
// function MatchS07_0042f8d0_Core(alpha, pos, size). The export MenuMenusBC is the naked adapter the original's
// callers reach through the RH_ScopedInstall (and the a2-dialog fixture through `eax_arg`): it re-pushes the four
// floats and AL zero-extended, calls the core, and returns __cdecl. It is not a body of 0x0042f8d0.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042f8d0 MatchS07_0042f8d0_Core
#define FUN_00472c60 MatchS07_0042f8d0_Call00472c60
extern "C" {
#include "../../../../re/match/s07/0042f8d0.c"
}
// Callee of the matched source: the old Frontend/MenuMenusB.cpp body called FUN_00472c60 through its original address
// (`call dword ptr [g_menuBc_Quad]`, g_menuBc_Quad = 0x00472c60), so the forwarder makes that same call.
extern "C" void MatchS07_0042f8d0_Call00472c60(V2d0042f8d0 pos, V2d0042f8d0 size, Rgba0042f8d0 c) {
    reinterpret_cast<void (__cdecl*)(V2d0042f8d0, V2d0042f8d0, Rgba0042f8d0)>(0x00472c60u)(pos, size, c);
}

extern "C" __declspec(dllexport) __declspec(naked) void __cdecl MenuMenusBC(
    float /*x*/, float /*y*/, float /*w*/, float /*h*/)
{
    __asm {
        push dword ptr [esp + 0x10]             // h
        push dword ptr [esp + 0x10]             // w
        push dword ptr [esp + 0x10]             // y
        push dword ptr [esp + 0x10]             // x
        movzx eax, al
        push eax                                // alpha (AL)
        call MatchS07_0042f8d0_Core
        add  esp, 0x14
        ret
    }
}
RH_ScopedInstall(MenuMenusBC, 0x0042f8d0);
