// MATCH-S07: port body of 0x0042e590 = the byte-matched source re/match/s07/0042e590.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/SpriteDispatch.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042e590 SpriteAnimFrameThunk
#define FUN_0040bb70 SpriteLookupTableA
#pragma comment(linker, "/EXPORT:SpriteAnimFrameThunk=_SpriteAnimFrameThunk")
extern "C" {
#include "../../../../re/match/s07/0042e590.c"
}
RH_ScopedInstall(SpriteAnimFrameThunk, 0x0042e590);
