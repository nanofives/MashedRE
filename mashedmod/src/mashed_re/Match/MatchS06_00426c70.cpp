// MATCH-S06: port body of 0x00426c70 = the byte-matched source re/match/s06/00426c70.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/UtilBatch.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees 0x0041ea50 / 0x0041e930: no port body; the old body called them through their original addresses.
// The matched function has one unused stack parameter (the original stores &DAT_00646e58 into it and tail-jumps).
// The port has no C++ caller, only the original's call through the installed JMP.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00426c70 TimerDispatch70
#define FUN_0041ea50 MatchS06_00426c70_Call0041ea50
#define FUN_0041e930 MatchS06_00426c70_Call0041e930
extern "C" int MatchS06_00426c70_Call0041ea50(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x0041ea50u)();
}
extern "C" void MatchS06_00426c70_Call0041e930(void * p0) {
    reinterpret_cast<void (__cdecl*)(void *)>(0x0041e930u)(p0);
}
#pragma comment(linker, "/EXPORT:TimerDispatch70=_TimerDispatch70")
extern "C" {
#include "../../../../re/match/s06/00426c70.c"
}
RH_ScopedInstall(TimerDispatch70, 0x00426c70);
