// MATCH-S08: port body of 0x00432ad0 = the byte-matched source re/match/s08/00432ad0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/BucketMixed_t3.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00432ad0 MenuDimOverlayFadeStep
#define FUN_0042aae0 MatchS08_00432ad0_Call0042aae0
// Callee of the matched source: the old Frontend/BucketMixed_t3.cpp body called FUN_0042aae0 through its original
// address (s_MenuIm2DQuad, a __cdecl function-pointer cast to 0x0042aae0, argument 0). 0x0042aae0 has one port body in
// both targets, MenuIm2DQuad (Frontend/MenuChrome.cpp, declared __fastcall: the argument goes in ECX), so the
// forwarder calls it with the same argument.
extern "C" void __fastcall MenuIm2DQuad(int param_1);
extern "C" void MatchS08_00432ad0_Call0042aae0(int p0) {
    MenuIm2DQuad(p0);
}
#pragma comment(linker, "/EXPORT:MenuDimOverlayFadeStep=_MenuDimOverlayFadeStep")
extern "C" {
#include "../../../../re/match/s08/00432ad0.c"
}
RH_ScopedInstall(MenuDimOverlayFadeStep, 0x00432ad0);
