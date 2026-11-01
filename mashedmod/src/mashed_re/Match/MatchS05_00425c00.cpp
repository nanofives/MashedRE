// MATCH-S05: port body of 0x00425c00 = the byte-matched source re/match/s05/00425c00.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuLeaves_af4.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// No RH_ScopedInstall: the old body was not installed either (it loads a copter clump off disk and consumes a slot
// non-idempotently, so it cannot run at the diff-attach menu), and nothing in the port calls it.

#define MATCH_PORT 1
#define FUN_00425c00 CopterClumpRegister
#define FUN_004c5c80 MatchS05_00425c00_Call004c5c80
#define FUN_004781b0 MatchS05_00425c00_Call004781b0
#define FUN_004a2b60 MatchS05_00425c00_Call004a2b60
#define FUN_00425b90 MatchS05_00425c00_Call00425b90
#pragma comment(linker, "/EXPORT:CopterClumpRegister=_CopterClumpRegister")
extern "C" {
#include "../../../../re/match/s05/00425c00.c"
}

// Callees of the matched source: the old Frontend/MenuLeaves_af4.cpp body called all four through their original
// addresses (__cdecl function-pointer casts s_F004c5c80 / s_F004781b0 / s_F004a2b60 / s_F00425b90), so each
// forwarder makes that same call. 0x004a2b60 is variadic (sprintf-style): its forwarder jumps, so the caller's
// whole argument list reaches the original unchanged.
extern "C" {
void MatchS05_00425c00_Call004c5c80(undefined4 p0) {
    reinterpret_cast<void (__cdecl*)(undefined4)>(0x004c5c80u)(p0);
}
undefined4 MatchS05_00425c00_Call004781b0(undefined1 *p0, int p1) {
    return reinterpret_cast<undefined4 (__cdecl*)(undefined1*, int)>(0x004781b0u)(p0, p1);
}
__declspec(naked) undefined4 MatchS05_00425c00_Call004a2b60(undefined1 *p0, const char *p1, ...) {
    __asm {
        mov eax, 0x004a2b60
        jmp eax
    }
}
void MatchS05_00425c00_Call00425b90(undefined4 *p0) {
    reinterpret_cast<void (__cdecl*)(undefined4*)>(0x00425b90u)(p0);
}
}
