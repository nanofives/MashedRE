// MATCH-S19: port body of 0x004c5800 = the byte-matched source re/match/s19/004c5800.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/TextureLoaderCluster.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004c5800 RwTexDictionarySetCurrent
#pragma comment(linker, "/EXPORT:RwTexDictionarySetCurrent=_RwTexDictionarySetCurrent")
extern "C" {
#include "../../../../re/match/s19/004c5800.c"
}
RH_ScopedInstall(RwTexDictionarySetCurrent, 0x004c5800);
