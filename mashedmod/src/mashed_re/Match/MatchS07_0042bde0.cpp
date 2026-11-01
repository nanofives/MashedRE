// MATCH-S07: port body of 0x0042bde0 = the byte-matched source re/match/s07/0042bde0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/BatchAA_s1.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042bde0 HudRectEmitter5
#define FUN_00472c60 MatchS07_0042bde0_Call00472c60
#pragma comment(linker, "/EXPORT:HudRectEmitter5=_HudRectEmitter5")
extern "C" {
#include "../../../../re/match/s07/0042bde0.c"
}
// Callee of the matched source: the old Frontend/BatchAA_s1.cpp body called FUN_00472c60 through its original
// address (g_ChromeBaseDraw, a __cdecl function-pointer cast to 0x00472c60, in both targets), so the forwarder makes
// that same call. Defined after the include because its parameter list uses the matched source's RGBA4 (four bytes
// on the stack, the same dword the old body pushed); the int params carry the float bits the old body pushed.
extern "C" void MatchS07_0042bde0_Call00472c60(int x, int y, int w, int h, RGBA4 col) {
    reinterpret_cast<void (__cdecl*)(int, int, int, int, RGBA4)>(0x00472c60u)(x, y, w, h, col);
}
RH_ScopedInstall(HudRectEmitter5, 0x0042bde0);
