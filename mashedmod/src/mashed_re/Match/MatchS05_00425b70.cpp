// MATCH-S05: port body of 0x00425b70 = the byte-matched source re/match/s05/00425b70.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/BatchAA_s4.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00425b70 SlotFieldSetter
#pragma comment(linker, "/EXPORT:SlotFieldSetter=_SlotFieldSetter")
extern "C" {
#include "../../../../re/match/s05/00425b70.c"
}
RH_ScopedInstall(SlotFieldSetter, 0x00425b70);
