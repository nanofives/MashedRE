// MATCH-S06: port body of 0x00426c30 = the byte-matched source re/match/s06/00426c30.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/UtilBatch.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees 0x0041ea30 / 0x0041e910: no port body; the old body called them through their original addresses.
// The matched function has one unused stack parameter (the original stores &DAT_00646e58 into it and tail-jumps).
// The port has no C++ caller, only the original's call through the installed JMP.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00426c30 TimerDispatch30
#define FUN_0041ea30 MatchS06_00426c30_Call0041ea30
#define FUN_0041e910 MatchS06_00426c30_Call0041e910
extern "C" int MatchS06_00426c30_Call0041ea30(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x0041ea30u)();
}
extern "C" void MatchS06_00426c30_Call0041e910(void * p0) {
    reinterpret_cast<void (__cdecl*)(void *)>(0x0041e910u)(p0);
}
#pragma comment(linker, "/EXPORT:TimerDispatch30=_TimerDispatch30")
extern "C" {
#include "../../../../re/match/s06/00426c30.c"
}
RH_ScopedInstall(TimerDispatch30, 0x00426c30);
