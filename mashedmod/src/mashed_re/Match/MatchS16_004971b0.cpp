// MATCH-S16: port body of 0x004971b0 = the byte-matched source re/match/s16/004971b0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Input/DInput.cpp).
// The file also holds the static callee FUN_00497190 (own MATCH line, 100%): the port has no other body of 0x00497190,
// so this file is its body too (not installed: the original's other caller 0x00497230 keeps calling the original).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
//
// The original passes the slot index in EAX (DDK custom convention of the static). Under VS2022 the matched function
// (MATCH_STATIC = extern) takes it on the stack, so the installed/exported entry ControllerConfigLoad_j5 is a naked
// EAX-to-stack bridge, the same bridge the old Input/DInput.cpp entry was. It is not a body: it only calls the core.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004971b0 ControllerConfigLoad_j5_Core
#define FUN_00496400 ConfigLogDebug
#define FUN_004a2b60 MatchS16_004971b0_Call004a2b60
#define FUN_004a4541 MatchS16_004971b0_Call004a4541
#define FUN_004a49cf MatchS16_004971b0_Call004a49cf
#define FUN_004a4368 MatchS16_004971b0_Call004a4368
#pragma comment(linker, "/EXPORT:ControllerConfigLoad_j5=_ControllerConfigLoad_j5")
extern "C" {
#include "../../../../re/match/s16/004971b0.c"
}
// Game-CRT callees: raw-address forwarders, the same calls the old Input/DInput.cpp body made (s_GameFsopenSafe /
// s_GameFread / s_GameFclose at their game VAs; 0x004a4541 deliberately NOT the FsopenSafe export, so the fix_fopen
// cave on the patched reference still runs and the FILE* stays the game CRT's). FUN_004a2b60 (game sprintf, no port
// body) was reached by the old body through the original 0x00497190; it is variadic, so the forwarder is a naked jmp.
extern "C" {
__declspec(naked) int MatchS16_004971b0_Call004a2b60(char *buf, const char *fmt, ...) {
    __asm {
        mov eax, 0x004a2b60
        jmp eax
    }
}
FILE_h4 *MatchS16_004971b0_Call004a4541(const char *name, const char *mode) {
    return reinterpret_cast<FILE_h4 *(__cdecl*)(const char *, const char *)>(0x004a4541u)(name, mode);
}
unsigned int MatchS16_004971b0_Call004a49cf(void *buf, unsigned int size, unsigned int n, FILE_h4 *f) {
    return reinterpret_cast<unsigned int (__cdecl*)(void *, unsigned int, unsigned int, FILE_h4 *)>(0x004a49cfu)(buf, size, n, f);
}
int MatchS16_004971b0_Call004a4368(FILE_h4 *f) {
    return reinterpret_cast<int (__cdecl*)(FILE_h4 *)>(0x004a4368u)(f);
}

// Naked entry: slot index via EAX (the original's convention), bridged onto the core's stack argument.
__declspec(naked) int ControllerConfigLoad_j5(void) {
    __asm {
        push eax
        call ControllerConfigLoad_j5_Core
        add esp, 4
        ret
    }
}
}
RH_ScopedInstall(ControllerConfigLoad_j5, 0x004971b0);
