// MATCH-S16: port body of 0x00495530 = the byte-matched source re/match/s16/00495530.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Input/DirectInput.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00495530 CreateDInputObject
// [PORT-TOP-04A] exe: DirectInput8Create (the 0x0049b300 import thunk) and the debug printf 0x004987b0 bind by name
// (Match/PortTop_InputPort.cpp, Match/PortTop_004987b0.cpp). The .asi keeps the raw-address calls.
#ifdef MASHED_STANDALONE
#define FUN_0049b300 PortTop_DirectInput8Create_0049b300
#define FUN_004987b0 DbgPrintf_004987b0
#else
#define FUN_0049b300 MatchS16_00495530_Call0049b300
#define FUN_004987b0 MatchS16_00495530_Call004987b0
#endif
#pragma comment(linker, "/EXPORT:CreateDInputObject=_CreateDInputObject")
extern "C" {
#include "../../../../re/match/s16/00495530.c"
}
#ifndef MASHED_STANDALONE
// Callees with no C-callable port body: raw-address forwarders, the same calls the old Input/DirectInput.cpp body
// made (its naked push/ret thunks game_dinput8_create -> 0x0049b300 and game_printf_dbg -> 0x004987b0).
extern "C" {
long __stdcall MatchS16_00495530_Call0049b300(HMODULE hinst, unsigned long ver, void *riid, void *out, void *outer) {
    return reinterpret_cast<long (__stdcall*)(HMODULE, unsigned long, void *, void *, void *)>(0x0049b300u)(hinst, ver, riid, out, outer);
}
void MatchS16_00495530_Call004987b0(char *p0) {
    reinterpret_cast<void (__cdecl*)(char *)>(0x004987b0u)(p0);
}
}
#endif
RH_ScopedInstall(CreateDInputObject, 0x00495530);
