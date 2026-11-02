// MATCH-S08-FU: port body of 0x00413f90 = the byte-matched source re/match/s08fu/00413f90.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%, relocheck OK).
// The same source text is this RVA's only body in both targets (moved out of Util/TimerState.cpp, deleted).
// Compiled here as C++ with MATCH_PORT; the global binds to its original address (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00413f90 TimerGetBasePtr
#pragma comment(linker, "/EXPORT:TimerGetBasePtr=_TimerGetBasePtr")
extern "C" {
#include "../../../../re/match/s08fu/00413f90.c"
}
RH_ScopedInstall(TimerGetBasePtr, 0x00413f90);
