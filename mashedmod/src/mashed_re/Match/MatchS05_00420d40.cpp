// MATCH-S05: port body of 0x00420d40 = the byte-matched source re/match/s05/00420d40.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/TimerInit.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00420d40 TimerStructArrayClear
#define FUN_004b6520 ZeroFillWrapper   // Util/TimerSlot.cpp, the port's 0x004b6520 (memset(p, 0, n)); the old body inlined it as memset(p, 0, 8)
#pragma comment(linker, "/EXPORT:TimerStructArrayClear=_TimerStructArrayClear")
extern "C" {
#include "../../../../re/match/s05/00420d40.c"
}
RH_ScopedInstall(TimerStructArrayClear, 0x00420d40);
