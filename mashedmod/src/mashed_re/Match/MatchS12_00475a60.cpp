// MATCH-S12: port body of 0x00475a60 = the byte-matched source re/match/s12/00475a60.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/UtilMid.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00475a60 PendingOpQueueFlush
#define FUN_004b6520 ZeroFillWrapper
#pragma comment(linker, "/EXPORT:PendingOpQueueFlush=_PendingOpQueueFlush")
extern "C" {
#include "../../../../re/match/s12/00475a60.c"
}
RH_ScopedInstall(PendingOpQueueFlush, 0x00475a60);
