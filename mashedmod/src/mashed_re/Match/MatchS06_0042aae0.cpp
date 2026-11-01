// MATCH-S06: port body of 0x0042aae0 = the byte-matched source re/match/s06/0042aae0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuChrome.cpp).
// The old body was __fastcall(int) only to forward ECX as a third render-state argument; the matched body is
// cdecl void(void) with two-argument render-state calls. No port caller.
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042aae0 MenuIm2DQuad
#pragma comment(linker, "/EXPORT:MenuIm2DQuad=_MenuIm2DQuad")
extern "C" {
#include "../../../../re/match/s06/0042aae0.c"
}
RH_ScopedInstall(MenuIm2DQuad, 0x0042aae0);
