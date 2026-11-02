// MATCH-S07: port body of 0x0042d290 = the byte-matched source re/match/s07/0042d290.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of
// Frontend/MenuMixed.cpp, .asi only, by MATCH-S07-FU, 2026-11-01).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL). x87 TU.
//
// What the swap changes (.asi only; no exe body before or after): the old body took param_2 as an unused dword and
// formatted the second value from whatever ST0 held when it called 0x004a2c48 (_ftol); the original converts its
// float param_2 to int (matched source `iVar1 = (int)param_2;`).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042d290 MenusLapTimeFmt
#define FUN_004a2b60 MatchS07_0042d290_Call004a2b60
// Callee of the matched source: 0x004a2b60 (sprintf, platform CRT, L-67) has no port body; the old body called it
// through its original address. It is variadic, so the forwarder is a naked tail jump and the caller's stack
// arguments pass through unchanged (the MatchS05_00423480 form).
extern "C" __declspec(naked) int MatchS07_0042d290_Call004a2b60(char *dst, const char *fmt, ...) {
    __asm {
        mov eax, 004A2B60h
        jmp eax
    }
}
#pragma comment(linker, "/EXPORT:MenusLapTimeFmt=_MenusLapTimeFmt")
extern "C" {
#include "../../../../re/match/s07/0042d290.c"
}
RH_ScopedInstall(MenusLapTimeFmt, 0x0042d290);
