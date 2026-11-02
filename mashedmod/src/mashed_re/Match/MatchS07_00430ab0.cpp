// MATCH-S07: port body of 0x00430ab0 = the byte-matched source re/match/s07/00430ab0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/SpriteGate.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00430ab0 HudSlotTypePlayer2
#pragma comment(linker, "/EXPORT:HudSlotTypePlayer2=_HudSlotTypePlayer2")
extern "C" {
#include "../../../../re/match/s07/00430ab0.c"
}
RH_ScopedInstall(HudSlotTypePlayer2, 0x00430ab0);
