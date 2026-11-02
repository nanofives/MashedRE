// MATCH-S06: port body of 0x00427680 = the byte-matched source re/match/s06/00427680.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body: the port had no body of this RVA).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

#define MATCH_PORT 1
#define FUN_0042b8b0 ScreenWidthGet
#define FUN_0042b8c0 ScreenHeightGet
// FUN_005554d0: no port body (HUD/HudBatch.cpp FontText_StringWidthAccumulator is under #if 0), so a raw-address
// forwarder in both targets (S04 rule): in the .asi it calls the original; the exe has no caller of this function.
extern "C" float MatchS06_00427680_Call005554d0(int font, short *text, float size) {
    return reinterpret_cast<float (__cdecl*)(int, short *, float)>(0x005554d0u)(font, text, size);
}
#define FUN_005554d0 MatchS06_00427680_Call005554d0
#pragma comment(linker, "/EXPORT:FUN_00427680=_FUN_00427680")
extern "C" {
#include "../../../../re/match/s06/00427680.c"
}
