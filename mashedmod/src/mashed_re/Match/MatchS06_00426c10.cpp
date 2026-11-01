// MATCH-S06: port body of 0x00426c10 = the byte-matched source re/match/s06/00426c10.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/UtilBatch.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees 0x0041ea40 / 0x0041e920 / 0x00480100: no port body; the old body called them through their original
// addresses, so each forwarder makes that same call.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00426c10 TimerDispatch10
#define FUN_0041ea40 MatchS06_00426c10_Call0041ea40
#define FUN_0041e920 MatchS06_00426c10_Call0041e920
#define FUN_00480100 MatchS06_00426c10_Call00480100
extern "C" unsigned int MatchS06_00426c10_Call0041ea40(void) {
    return reinterpret_cast<unsigned int (__cdecl*)()>(0x0041ea40u)();
}
extern "C" void MatchS06_00426c10_Call0041e920(void * p0) {
    reinterpret_cast<void (__cdecl*)(void *)>(0x0041e920u)(p0);
}
extern "C" void MatchS06_00426c10_Call00480100(void) {
    reinterpret_cast<void (__cdecl*)()>(0x00480100u)();
}
#pragma comment(linker, "/EXPORT:TimerDispatch10=_TimerDispatch10")
extern "C" {
#include "../../../../re/match/s06/00426c10.c"
}
RH_ScopedInstall(TimerDispatch10, 0x00426c10);
