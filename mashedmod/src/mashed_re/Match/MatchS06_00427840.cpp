// MATCH-S06: port body of 0x00427840 = the byte-matched source re/match/s06/00427840.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of HUD/HudBatch.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The original passes src in EAX and dst in ECX (callers 0x00427be0 / 0x00427f00 / 0x00428320). VS2022 has no
// such convention, so the matched body is the cdecl FontText_UTF16WidenCopyBody (the symbol the port's matched
// callers bind) and the installed/exported entry FontText_UTF16WidenCopy is a register adapter (push EAX, ECX;
// call the body). EBX/ESI/EDI are preserved as in the original; EAX/ECX/EDX are clobbered by both.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00427840 FontText_UTF16WidenCopyBody
#pragma comment(linker, "/EXPORT:FontText_UTF16WidenCopy=_FontText_UTF16WidenCopy")
extern "C" {
#include "../../../../re/match/s06/00427840.c"
}

// EAX = src, ECX = dst entry for the hook and the Frida export. Not a body: it only forwards to
// FontText_UTF16WidenCopyBody.
extern "C" __declspec(naked) void __cdecl FontText_UTF16WidenCopy(void) {
    __asm {
        push eax                   // src
        push ecx                   // dst
        call FontText_UTF16WidenCopyBody
        add  esp, 8
        ret
    }
}
RH_ScopedInstall(FontText_UTF16WidenCopy, 0x00427840);
