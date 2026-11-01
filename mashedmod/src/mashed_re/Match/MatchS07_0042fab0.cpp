// MATCH-S07: port body of 0x0042fab0 = the byte-matched source re/match/s07/0042fab0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/SpriteDispatch.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042fab0 SpriteSlotDispatch
#define FUN_0040bb90 SpriteLookupTableB
#pragma comment(linker, "/EXPORT:SpriteSlotDispatch=_SpriteSlotDispatch")
extern "C" {
#include "../../../../re/match/s07/0042fab0.c"
}
RH_ScopedInstall(SpriteSlotDispatch, 0x0042fab0);
