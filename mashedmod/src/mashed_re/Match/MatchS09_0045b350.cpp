// MATCH-S09: port body of 0x0045b350 = the byte-matched source re/match/s09/0045b350.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Input/DirectInput.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0045b350 RwInitNullStub
#pragma comment(linker, "/EXPORT:RwInitNullStub=_RwInitNullStub")
extern "C" {
#include "../../../../re/match/s09/0045b350.c"
}
RH_ScopedInstall(RwInitNullStub, 0x0045b350);
