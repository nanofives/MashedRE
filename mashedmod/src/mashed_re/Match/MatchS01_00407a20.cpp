// MATCH-TOPDOWN-T13: port body of 0x00407a20 = the byte-matched source re/match/s01/00407a20.c
// (port-includable form of re/match/s01/raw/00407a20.c; XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1,
// as C; re/tools/match/match.py = 100%, relocheck OK). This RVA's only body in both targets (moved out of
// Gameplay/RangeTable_ah1.cpp, asi-only). The exe calls it from AiPreTickRubberBand (0x004177b0,
// Ai/AiStandalone.cpp), which read 0x008a9648 + v*0x30c inline before.
// Compiled here as C++ with MASHED_RE_PORT; the global binds to its original address.
#include "../Core/HookSystem.h"

#define MASHED_RE_PORT 1
#define DAT_008a9648 (*(unsigned int*)0x008a9648u)
#define FUN_00407a20 GetAiLapCounter
#pragma comment(linker, "/EXPORT:GetAiLapCounter=_GetAiLapCounter")
extern "C" {
#include "../../../../re/match/s01/00407a20.c"
}
RH_ScopedInstall(GetAiLapCounter, 0x00407a20);  // c3_batch_ah s2 install, moved with the body
