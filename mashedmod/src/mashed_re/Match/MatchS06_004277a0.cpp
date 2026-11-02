// MATCH-S06: port body of 0x004277a0 = the byte-matched source re/match/s06/004277a0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/TextCtrlCodeRemap.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The original passes src in EAX and dst in EBX (callers 0x004278d0 / 0x00427990 / 0x00427ad0 / 0x004282a0 /
// 0x00427e00 / 0x00427ff0 / 0x00428140). VS2022 has no such convention, so the matched body is the cdecl
// TextCtrlCodeRemapBody (the symbol the port's matched callers bind) and the installed/exported entry
// TextCtrlCodeRemap is a register adapter (push EBX, EAX; call the body). EBX/ESI/EDI are preserved as in the
// original; EAX/ECX/EDX are clobbered by both. The adapter keeps the old .asi-only U-9065 boot guard
// (EBX == 0 returns without writing), which Frontend/TextCtrlCodeRemap.cpp carried under #ifndef MASHED_STANDALONE.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004277a0 TextCtrlCodeRemapBody
#pragma comment(linker, "/EXPORT:TextCtrlCodeRemap=_TextCtrlCodeRemap")
extern "C" {
#include "../../../../re/match/s06/004277a0.c"
}

// EAX = src, EBX = dst entry for the hook and the Frida export. Not a body: it only forwards to TextCtrlCodeRemapBody.
extern "C" __declspec(naked) void __cdecl TextCtrlCodeRemap(void) {
    __asm {
#ifndef MASHED_STANDALONE
        test ebx, ebx              // U-9065 dev-only boot guard: EBX == 0 writes nothing
        jnz  do_work
        ret
    do_work:
#endif
        push ebx                   // dst
        push eax                   // src
        call TextCtrlCodeRemapBody
        add  esp, 8
        ret
    }
}
RH_ScopedInstall(TextCtrlCodeRemap, 0x004277a0);
