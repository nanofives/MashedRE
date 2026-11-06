// MATCH-S12: port body of 0x00477450 = the byte-matched source re/match/s12/00477450.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Its in-file caller 0x00477760 is HELD (not compiled with MATCH_PORT): re/match/s12/00477760.c is that RVA's file.
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The original takes the destination in EAX plus four stack args (src, col, tmax, rate) and never writes EAX.
// The matched body is the cdecl Init477450(dest, src, col, tmax, rate) (the Frida export, arg_type
// eax_dest_memcpy_init, same signature as the old cdecl body); the installed entry is the register adapter
// Init477450_RegAbi (moved here from Util/PromoLoop_sessionB.cpp, ABI fix 2026-07-27).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00477450 Init477450
#pragma comment(linker, "/EXPORT:Init477450=_Init477450")
#pragma comment(linker, "/EXPORT:Init477450_RegAbi=_Init477450_RegAbi")
extern "C" {
#include "../../../../re/match/s12/00477450.c"
}

// Register-ABI entry, installed at 0x00477450. Not a body: it only forwards to Init477450.
// Entry frame: [ESP+4]=src, [ESP+8]=col, [ESP+0xc]=tmax, [ESP+0x10]=rate. Each PUSH shifts those by 4,
// so the same literal [ESP+0x10] walks rate..src. EAX==dest is restored (the original preserves it).
// Plain RET (caller cleans its 4 args).
extern "C" __declspec(naked) void __cdecl Init477450_RegAbi(void) {
    __asm {
        push dword ptr [esp+0x10]   // rate
        push dword ptr [esp+0x10]   // tmax
        push dword ptr [esp+0x10]   // col
        push dword ptr [esp+0x10]   // src
        push eax                    // dest (register arg in the original)
        call Init477450
        mov  eax, dword ptr [esp]
        add  esp, 20
        ret
    }
}
RH_ScopedInstall(Init477450_RegAbi, 0x00477450);
